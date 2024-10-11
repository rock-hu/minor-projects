/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBPANDABASE_UTILS_PANDARGS_H
#define LIBPANDABASE_UTILS_PANDARGS_H

#include <algorithm>
#include <array>
#include <list>
#include <set>
#include <map>
#include <string>
#include <string_view>
#include <sstream>
#include <type_traits>
#include <vector>
#include <cerrno>
#include <optional>
#include <utility>

#include "macros.h"

namespace panda {
class PandArgBase;
using sub_args_t = std::vector<PandArgBase *>;
using arg_list_t = std::vector<std::string>;
using std::enable_if_t;
using std::is_same_v;

enum class PandArgType : uint8_t { STRING, INTEGER, DOUBLE, BOOL, LIST, UINT32, UINT64, COMPOUND, NOTYPE };

// Base class for panda argument
class PandArgBase {
public:
    explicit PandArgBase(std::string name, std::string desc, PandArgType type = PandArgType::NOTYPE)
        : name_(std::move(name)), desc_(std::move(desc)), type_(type)
    {
    }

    PandArgType GetType() const
    {
        return type_;
    }

    std::string GetName() const
    {
        return name_;
    }

    std::string GetDesc() const
    {
        return desc_;
    }

    bool WasSet() const
    {
        return was_set_;
    }

    void SetWasSet(bool value)
    {
        was_set_ = value;
    }

    virtual void ResetDefaultValue() = 0;

private:
    std::string name_;
    std::string desc_;
    PandArgType type_;
    bool was_set_ {false};
};

template <typename T,
          enable_if_t<is_same_v<std::string, T> || is_same_v<double, T> || is_same_v<bool, T> || is_same_v<int, T> ||
                      is_same_v<uint32_t, T> || is_same_v<uint64_t, T> || is_same_v<arg_list_t, T>> * = nullptr>
class PandArg : public PandArgBase {
public:
    explicit PandArg(const std::string &name, T default_val, const std::string &desc)
        : PandArgBase(name, desc, this->EvalType()), default_val_(default_val), real_val_(default_val)
    {
    }

    explicit PandArg(const std::string &name, T default_val, const std::string &desc, PandArgType type)
        : PandArgBase(name, desc, type), default_val_(default_val), real_val_(default_val)
    {
    }

    explicit PandArg(const std::string &name, int default_val, const std::string &desc, T min_val, T max_val)
        : PandArgBase(name, desc, this->EvalType()),
          default_val_(default_val),
          real_val_(default_val),
          min_max_val_(std::pair<T, T>(min_val, max_val))
    {
    }

    explicit PandArg(const std::string &name, const arg_list_t &default_val, const std::string &desc,
                     std::string delimiter)
        : PandArgBase(name, desc, PandArgType::LIST),
          default_val_(default_val),
          real_val_(default_val),
          delimiter_(std::move(delimiter))
    {
    }

    T GetValue() const
    {
        return real_val_;
    }

    T GetDefaultValue() const
    {
        return default_val_;
    }

    template <bool update_flag = true>
    void SetValue(T val)
    {
        real_val_ = val;
        // NOLINTNEXTLINE(bugprone-suspicious-semicolon)
        if constexpr (update_flag) {  // NOLINT(readability-braces-around-statements)
            SetWasSet(true);
        }
    }

    void ResetDefaultValue() override
    {
        real_val_ = default_val_;
    }

    std::optional<std::string> GetDelimiter() const
    {
        return delimiter_;
    }
    std::optional<std::pair<T, T>> GetMinMaxVal()
    {
        return min_max_val_;
    }

private:
    constexpr PandArgType EvalType()
    {
        // NOLINTNEXTLINE(bugprone-branch-clone)
        if constexpr (is_same_v<std::string, T>) {  // NOLINT(readability-braces-around-statements)
            return PandArgType::STRING;
            // NOLINTNEXTLINE(readability-braces-around-statements,readability-misleading-indentation)
        } else if constexpr (is_same_v<double, T>) {
            return PandArgType::DOUBLE;
            // NOLINTNEXTLINE(readability-braces-around-statements,readability-misleading-indentation)
        } else if constexpr (is_same_v<bool, T>) {
            return PandArgType::BOOL;
            // NOLINTNEXTLINE(readability-braces-around-statements,readability-misleading-indentation)
        } else if constexpr (is_same_v<int, T>) {
            return PandArgType::INTEGER;
            // NOLINTNEXTLINE(readability-braces-around-statements,readability-misleading-indentation)
        } else if constexpr (is_same_v<uint32_t, T>) {
            return PandArgType::UINT32;
            // NOLINTNEXTLINE(readability-braces-around-statements,readability-misleading-indentation)
        } else if constexpr (is_same_v<uint64_t, T>) {
            return PandArgType::UINT64;
            // NOLINTNEXTLINE(readability-braces-around-statements,readability-misleading-indentation)
        } else if constexpr (is_same_v<arg_list_t, T>) {
            return PandArgType::LIST;
        }
        UNREACHABLE();
    }

    T default_val_;
    T real_val_;

    // Only for integer arguments with range
    std::optional<std::pair<T, T>> min_max_val_;

    // Only for strings with delimiter
    std::optional<std::string> delimiter_;
};

class PandArgCompound : public PandArg<bool> {
public:
    PandArgCompound(const std::string &name, const std::string &desc, std::initializer_list<PandArgBase *> sub_args)
        : PandArg<bool>(name, false, desc, PandArgType::COMPOUND), sub_args_(sub_args)
    {
    }

    PandArgBase *FindSubArg(std::string_view name)
    {
        auto res = std::find_if(sub_args_.begin(), sub_args_.end(),
                                [name](const auto &arg) { return arg->GetName() == name; });
        return res == sub_args_.end() ? nullptr : *res;
    }

    void ResetDefaultValue() override
    {
        PandArg<bool>::ResetDefaultValue();
        std::for_each(sub_args_.begin(), sub_args_.end(), [](auto &arg) { arg->ResetDefaultValue(); });
    }

    const auto &GetSubArgs() const
    {
        return sub_args_;
    }

private:
    sub_args_t sub_args_;
};

class PandArgParser {
public:
    bool Add(PandArgBase *arg)
    {
        if (arg == nullptr) {
            errstr_ += "pandargs: Can't add `nullptr` as an argument\n";
            return false;
        }
        bool success = args_.insert(arg).second;
        if (!success) {
            errstr_ += "pandargs: Argument " + arg->GetName() + " has duplicate\n";
        }
        return success;
    }

    bool PushBackTail(PandArgBase *arg)
    {
        if (arg == nullptr) {
            errstr_ += "pandargs: Can't add `nullptr` as a tail argument\n";
            return false;
        }
        if (std::find(tail_args_.begin(), tail_args_.end(), arg) != tail_args_.end()) {
            errstr_ += "pandargs: Tail argument " + arg->GetName() + " is already in tail arguments list\n";
            return false;
        }
        tail_args_.emplace_back(arg);
        return true;
    }

    bool PopBackTail()
    {
        if (tail_args_.empty()) {
            errstr_ += "pandargs: Nothing to pop back from tail arguments\n";
            return false;
        }
        tail_args_.pop_back();
        return true;
    }

    void EraseTail()
    {
        tail_args_.erase(tail_args_.begin(), tail_args_.end());
    }

    bool Parse(const std::vector<std::string> &argv_vec)
    {
        InitDefault();
        std::copy(argv_vec.begin(), argv_vec.end(), std::back_inserter(argv_vec_));
        return ParseArgs();
    }

    bool Parse(int argc, const char *const argv[])  // NOLINT(modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    {
        InitDefault();
        for (int i = 1; i < argc; i++) {
            argv_vec_.emplace_back(argv[i]);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
        return ParseArgs();
    }

    /**
     * Parses a string to the option's value.
     */
    bool ParseSingleArg(PandArgBase *option, const std::string_view &option_value)
    {
        ASSERT(option != nullptr);
        argv_vec_ = {std::string(option_value)};
        argv_index_ = 0;
        argv_index_ += ParseNextParam(option, argv_vec_[argv_index_]);
        return errstr_.empty();
    }

    /**
     * Parses option's name and returns corresponding pointer.
     */
    PandArgBase *GetPandArg(const std::string_view &arg_name)
    {
        auto arg_it = args_.find(arg_name);
        return (arg_it != args_.end()) ? *arg_it : nullptr;
    }

    std::string GetErrorString() const
    {
        return errstr_;
    }

    void EnableTail()
    {
        tail_flag_ = true;
    }

    void DisableTail()
    {
        tail_flag_ = false;
    }

    bool IsTailEnabled() const
    {
        return tail_flag_;
    }

    std::size_t GetTailSize() const
    {
        return tail_args_.size();
    }

    void EnableRemainder() noexcept
    {
        remainder_flag_ = true;
    }

    void DisableRemainder() noexcept
    {
        remainder_flag_ = false;
    }

    bool IsRemainderEnabled() const
    {
        return remainder_flag_;
    }

    arg_list_t GetRemainder()
    {
        return remainder_;
    }

    bool IsArgSet(PandArgBase *arg) const
    {
        return args_.find(arg) != args_.end();
    }

    bool IsArgSet(const std::string &arg_name) const
    {
        return args_.find(arg_name) != args_.end();
    }

    std::string GetHelpString() const
    {
        std::ostringstream helpstr;
        for (auto i : args_) {
            if (i->GetType() == PandArgType::COMPOUND) {
                auto arg = static_cast<PandArgCompound *>(i);
                helpstr << DOUBLE_DASH << i->GetName() << ": " << i->GetDesc() << "\n";
                helpstr << "  Sub arguments:\n";
                for (auto sub_arg : arg->GetSubArgs()) {
                    helpstr << "    " << sub_arg->GetName() << ": " << sub_arg->GetDesc() << "\n";
                }
            } else {
                helpstr << DOUBLE_DASH << i->GetName() << ": " << i->GetDesc() << "\n";
            }
        }
        if (!tail_args_.empty()) {
            helpstr << "Tail arguments:\n";
            for (auto i : tail_args_) {
                helpstr << i->GetName() << ": " << i->GetDesc() << "\n";
            }
        }
        return helpstr.str();
    }

    std::string GetRegularArgs()
    {
        std::string args_str;
        std::string value;
        for (auto i : args_) {
            switch (i->GetType()) {
                case PandArgType::STRING:
                    value = static_cast<PandArg<std::string> *>(i)->GetValue();
                    break;
                case PandArgType::INTEGER:
                    value = std::to_string(static_cast<PandArg<int> *>(i)->GetValue());
                    break;
                case PandArgType::DOUBLE:
                    value = std::to_string(static_cast<PandArg<double> *>(i)->GetValue());
                    break;
                case PandArgType::BOOL:
                    // NOLINTNEXTLINE(readability-implicit-bool-conversion)
                    value = std::to_string(static_cast<PandArg<bool> *>(i)->GetValue());
                    break;
                case PandArgType::UINT32:
                    value = std::to_string(static_cast<PandArg<uint32_t> *>(i)->GetValue());
                    break;
                case PandArgType::UINT64:
                    value = std::to_string(static_cast<PandArg<uint64_t> *>(i)->GetValue());
                    break;
                case PandArgType::LIST: {
                    value = "";
                    std::vector<std::string> values_buf = static_cast<PandArg<arg_list_t> *>(i)->GetValue();
                    for (const auto &j : values_buf) {
                        value += j + ", ";
                    }
                    break;
                }
                default:
                    errstr_ += "Invalid argument type \"" + i->GetName() + "\"\n";
                    break;
            }
            args_str += DOUBLE_DASH + i->GetName() + "=" + value + "\n";
        }
        return args_str;
    }

private:
    struct PandArgPtrComparator {
        using is_transparent = void;
        bool operator()(const PandArgBase *lhs, const PandArgBase *rhs) const
        {
            return lhs->GetName() < rhs->GetName();
        }
        bool operator()(std::string_view lhs, const PandArgBase *rhs) const
        {
            return lhs < rhs->GetName();
        }
        bool operator()(const PandArgBase *lhs, std::string_view rhs) const
        {
            return lhs->GetName() < rhs;
        }
    };

    bool ParseArgs()
    {
        while (argv_index_ < argv_vec_.size()) {
            PandArgBase *parsed_arg = ParseNextArg();
            if (!errstr_.empty()) {
                return false;
            }
            if (argv_index_ < argv_vec_.size()) {
                argv_index_ += ParseNextParam(parsed_arg, argv_vec_[argv_index_]);
                if (!errstr_.empty()) {
                    return false;
                }
            }
        }
        return true;
    }

    void InitDefault()
    {
        equal_flag_ = false;
        tail_parsed_flag_ = false;
        argv_vec_.clear();
        argv_index_ = 0;
        errstr_ = "";
        // reset tail
        for (auto tail_arg : tail_args_) {
            switch (tail_arg->GetType()) {
                case PandArgType::STRING:
                    static_cast<PandArg<std::string> *>(tail_arg)->ResetDefaultValue();
                    break;
                case PandArgType::INTEGER:
                    static_cast<PandArg<int> *>(tail_arg)->ResetDefaultValue();
                    break;
                case PandArgType::DOUBLE:
                    static_cast<PandArg<double> *>(tail_arg)->ResetDefaultValue();
                    break;
                case PandArgType::BOOL:
                    static_cast<PandArg<bool> *>(tail_arg)->ResetDefaultValue();
                    break;
                case PandArgType::UINT32:
                    static_cast<PandArg<uint32_t> *>(tail_arg)->ResetDefaultValue();
                    break;
                case PandArgType::UINT64:
                    static_cast<PandArg<uint64_t> *>(tail_arg)->ResetDefaultValue();
                    break;
                case PandArgType::LIST:
                    static_cast<PandArg<arg_list_t> *>(tail_arg)->ResetDefaultValue();
                    break;
                default:
                    break;
            }
        }
        // reset remainder
        remainder_ = arg_list_t();
    }

    PandArgBase *FindArg(std::string_view arg_name)
    {
        auto arg_it = args_.find(arg_name);
        if (arg_it == args_.end()) {
            errstr_.append("pandargs: Invalid option \"");
            errstr_.append(arg_name);
            errstr_.append("\"\n");
            return nullptr;
        }
        return *arg_it;
    }

    bool ParseSubArgument(PandArgCompound *parent_arg, std::string_view str)
    {
        size_t assign_pos = str.find('=');
        std::string_view arg_name = str.substr(0, assign_pos);
        auto arg = parent_arg->FindSubArg(arg_name);
        if (arg == nullptr) {
            errstr_.append("pandargs: Invalid sub-argument \"");
            errstr_.append(arg_name);
            errstr_.append("\"\n");
            return false;
        }

        if (assign_pos != std::string_view::npos) {
            std::string_view value_str = str.substr(assign_pos + 1);
            ParseNextParam(arg, value_str);
            if (!errstr_.empty()) {
                return false;
            }
        } else {
            if (arg->GetType() != PandArgType::BOOL) {
                errstr_.append("pandargs: Only boolean arguments might have no value \"");
                errstr_.append(arg_name);
                errstr_.append("\"\n");
                return false;
            }
            static_cast<PandArg<bool> *>(arg)->SetValue(true);
        }

        return true;
    }

    PandArgBase *ParseCompoundArg(std::string_view argstr, size_t sep_pos)
    {
        auto arg_name = argstr.substr(0, sep_pos);

        auto arg = static_cast<PandArgCompound *>(FindArg(arg_name));
        if (arg == nullptr) {
            return nullptr;
        }

        arg->SetValue(true);

        auto sub_args_str = argstr.substr(sep_pos + 1);
        size_t start = 0;
        for (size_t pos = sub_args_str.find(',', 0); pos != std::string_view::npos;
             start = pos + 1, pos = sub_args_str.find(',', start)) {
            auto arg_str = sub_args_str.substr(start, pos - start);
            if (!ParseSubArgument(arg, arg_str)) {
                return nullptr;
            }
        }
        if (start < sub_args_str.size()) {
            if (!ParseSubArgument(arg, sub_args_str.substr(start))) {
                return nullptr;
            }
        }
        argv_index_++;
        return arg;
    }

    PandArgBase *ParseNextRegularArg()
    {
        std::string argstr = argv_vec_[argv_index_];

        const std::size_t SEP_FOUND = NextSeparator(argstr);

        const std::size_t SEP_COMPOUND = argstr.find_first_of(':', 0);
        if (SEP_COMPOUND != std::string::npos && (SEP_FOUND == std::string::npos || SEP_COMPOUND < SEP_FOUND)) {
            return ParseCompoundArg(std::string_view(argstr).substr(DASH_COUNT), SEP_COMPOUND - DASH_COUNT);
        }

        std::string arg_name;

        if (SEP_FOUND != std::string::npos) {
            equal_flag_ = true;
            argv_vec_[argv_index_] = argstr.substr(SEP_FOUND + 1);
            arg_name = argstr.substr(DASH_COUNT, SEP_FOUND - DASH_COUNT);
        } else {
            arg_name = argstr.substr(DASH_COUNT, SEP_FOUND);
            // check if there is next argv element to iterate into
            if (argv_index_ + 1 < argv_vec_.size()) {
                argv_index_++;
            } else {
                argv_vec_[argv_index_] = "";
            }
        }

        PandArgBase *arg = FindArg(arg_name);
        if (arg == nullptr) {
            return nullptr;
        }

        if (arg->GetType() == PandArgType::COMPOUND) {
            // It is forbidden to explicitly set compound option, e.g. `--compound=true`, must be `--compound`.
            if (SEP_FOUND != std::string::npos) {
                errstr_.append("pandargs: Compound option can not be explicitly set \"");
                errstr_.append(arg_name);
                errstr_.append("\"\n");
                return nullptr;
            }
            static_cast<PandArgCompound *>(arg)->SetValue(true);
        }

        return arg;
    }

    PandArgBase *ParseNextArg()
    {
        PandArgBase *arg = nullptr;
        std::string argstr = argv_vec_[argv_index_];
        equal_flag_ = false;

        // NOTE: currently we have only double dash argument prefix
        std::size_t dashes_found = argstr.find(DOUBLE_DASH);
        if (dashes_found == 0 && argstr.size() > DASH_COUNT) {
            // regular argument
            return ParseNextRegularArg();
        }

        if (dashes_found == 0 && argstr.size() == DASH_COUNT) {
            // remainder argument
            if (!remainder_flag_) {
                errstr_.append("pandargs: Remainder arguments are not enabled\n");
                errstr_.append("pandargs: Remainder found at literal \"");
                errstr_.append(argstr);
                errstr_.append("\"\n");
                return nullptr;
            }

            argv_index_++;
            ParseRemainder();
        } else if (dashes_found > 0) {
            // tail argument, N.B. std::string::npos > 0
            if (!tail_flag_) {
                errstr_.append("pandargs: Tail arguments are not enabled\n");
                errstr_.append("pandargs: Tail found at literal \"");
                errstr_.append(argstr);
                errstr_.append("\"\n");
                return nullptr;
            }
            if (tail_parsed_flag_) {
                errstr_.append("pandargs: Too many tail arguments\n");
                return nullptr;
            }
            ParseTail();

            if (argv_index_ < argv_vec_.size()) {
                if (argv_vec_[argv_index_] != DOUBLE_DASH && !remainder_flag_) {
                    errstr_ += "pandargs: Too many tail arguments given\n";
                }
            }
        } else {
            errstr_.append("pandargs: Invalid option \"");
            errstr_.append(argstr);
            errstr_.append("\"\n");
            UNREACHABLE();
        }
        return arg;
    }

    void ParseTail()
    {
        for (auto &tail_arg : tail_args_) {
            switch (tail_arg->GetType()) {
                case PandArgType::STRING:
                    argv_index_ +=
                        ParseStringArgParam(static_cast<PandArg<std::string> *>(tail_arg), argv_vec_[argv_index_]);
                    break;
                case PandArgType::INTEGER:
                    argv_index_ += ParseIntArgParam(static_cast<PandArg<int> *>(tail_arg), argv_vec_[argv_index_]);
                    break;
                case PandArgType::DOUBLE:
                    argv_index_ +=
                        ParseDoubleArgParam(static_cast<PandArg<double> *>(tail_arg), argv_vec_[argv_index_]);
                    break;
                case PandArgType::BOOL:
                    argv_index_ +=
                        ParseBoolArgParam(static_cast<PandArg<bool> *>(tail_arg), argv_vec_[argv_index_], true);
                    break;
                case PandArgType::UINT32:
                    argv_index_ +=
                        ParseUint32ArgParam(static_cast<PandArg<uint32_t> *>(tail_arg), argv_vec_[argv_index_]);
                    break;
                case PandArgType::UINT64:
                    argv_index_ +=
                        ParseUint64ArgParam(static_cast<PandArg<uint64_t> *>(tail_arg), argv_vec_[argv_index_]);
                    break;
                case PandArgType::LIST:
                    argv_index_ +=
                        ParseListArgParam(static_cast<PandArg<arg_list_t> *>(tail_arg), argv_vec_[argv_index_]);
                    break;
                default:
                    errstr_.append("pandargs: Invalid tail option type: \"");
                    errstr_.append(tail_arg->GetName());
                    errstr_.append("\"\n");
                    UNREACHABLE();
                    break;
            }
            if (argv_index_ >= argv_vec_.size() || !errstr_.empty()) {
                break;
            }
        }
        tail_parsed_flag_ = true;
    }

    void ParseRemainder()
    {
        remainder_ = arg_list_t(argv_vec_.begin() + argv_index_, argv_vec_.end());
        argv_index_ = argv_vec_.size();
    }

    size_t ParseNextParam(PandArgBase *arg, std::string_view argstr)
    {
        if (argv_index_ >= argv_vec_.size() || arg == nullptr) {
            return 0;
        }
        switch (arg->GetType()) {
            case PandArgType::STRING:
                return ParseStringArgParam(static_cast<PandArg<std::string> *>(arg), argstr);
            case PandArgType::INTEGER:
                return ParseIntArgParam(static_cast<PandArg<int> *>(arg), argstr);
            case PandArgType::DOUBLE:
                return ParseDoubleArgParam(static_cast<PandArg<double> *>(arg), argstr);
            case PandArgType::BOOL:
                return ParseBoolArgParam(static_cast<PandArg<bool> *>(arg), argstr);
            case PandArgType::UINT32:
                return ParseUint32ArgParam(static_cast<PandArg<uint32_t> *>(arg), argstr);
            case PandArgType::UINT64:
                return ParseUint64ArgParam(static_cast<PandArg<uint64_t> *>(arg), argstr);
            case PandArgType::LIST:
                return ParseListArgParam(static_cast<PandArg<arg_list_t> *>(arg), argstr);
            case PandArgType::COMPOUND:
                return argstr.empty() ? 1 : 0;
            case PandArgType::NOTYPE:
                errstr_.append("pandargs: Invalid option type: \"");
                errstr_.append(arg->GetName());
                errstr_.append("\"\n");
                UNREACHABLE();
                break;
            default:
                UNREACHABLE();
                break;
        }
        return 0;
    }

    std::size_t ParseStringArgParam(PandArg<std::string> *arg, std::string_view argstr)
    {
        arg->SetValue(std::string(argstr));
        return 1;
    }

    std::size_t ParseIntArgParam(PandArg<int> *arg, std::string_view argstr)
    {
        std::string param_str(argstr);
        if (IsIntegerNumber(param_str)) {
            int num;
            errno = 0;
            if (StartsWith(param_str, "0x")) {
                const int HEX = 16;
                num = std::stoi(param_str, nullptr, HEX);
            } else {
                num = std::stoi(param_str);
            }

            if (errno == ERANGE) {
                errstr_ +=
                    "pandargs: \"" + arg->GetName() + "\" argument has invalid parameter value \"" + param_str + "\"\n";
            }

            if (IsIntegerArgInRange(arg, num)) {
                arg->SetValue(num);
            } else {
                errstr_ += "pandargs: \"" + arg->GetName() + "\" argument has out of range parameter value \"" +
                           param_str + "\"\n";
            }
        } else {
            errstr_ += "pandargs: \"" + arg->GetName() + "\" argument has out of range parameter value \"" + param_str +
                       "\"\n";
        }
        return 1;
    }

    std::size_t ParseDoubleArgParam(PandArg<double> *arg, std::string_view argstr)
    {
        std::string param_str(argstr);
        if (IsRationalNumber(param_str)) {
            arg->SetValue(std::stod(param_str));
        } else {
            errstr_ +=
                "pandargs: \"" + arg->GetName() + "\" argument has invalid parameter value \"" + param_str + "\"\n";
        }
        return 1;
    }

    std::size_t ParseBoolArgParam(PandArg<bool> *arg, std::string_view argstr, bool is_tail_param = false)
    {
        std::string param_str(argstr);

        // if not a tail argument, assume two following cases
        if (!is_tail_param) {
            arg->SetValue(true);
            // bool with no param, next argument comes right after
            if (StartsWith(param_str, DOUBLE_DASH)) {
                // check that bool param comes without "="
                if (equal_flag_) {
                    SetBoolUnexpectedValueError(arg, param_str);
                }
                return 0;
            }
            // OR bool arg at the end of arguments line
            if (param_str.empty()) {
                // check that bool param comes without "="
                if (equal_flag_) {
                    SetBoolUnexpectedValueError(arg, param_str);
                }
                return 1;
            }
        }

        constexpr std::array<std::string_view, 3> TRUE_VALUES = {"on", "true", "1"};
        constexpr std::array<std::string_view, 3> FALSE_VALUES = {"off", "false", "0"};

        for (const auto &i : TRUE_VALUES) {
            if (param_str == i) {
                arg->SetValue(true);
                return 1;
            }
        }
        for (const auto &i : FALSE_VALUES) {
            if (param_str == i) {
                arg->SetValue(false);
                return 1;
            }
        }

        // if it's not a part of tail argument,
        // assume that it's bool with no param,
        // preceiding tail argument
        if (!is_tail_param) {
            // check that bool param came without "="
            if (equal_flag_) {
                SetBoolUnexpectedValueError(arg, param_str);
            } else {
                arg->SetValue(true);
            }
        } else {
            errstr_ +=
                "pandargs: Tail argument " + arg->GetName() + " has unexpected parameter value " + param_str + "\n";
            arg->ResetDefaultValue();
        }

        return 0;
    }

    std::size_t ParseUint64ArgParam(PandArg<uint64_t> *arg, std::string_view argstr)
    {
        std::string param_str(argstr);
        if (IsUintNumber(param_str)) {
            errno = 0;
            uint64_t num;
            if (StartsWith(param_str, "0x")) {
                const int HEX = 16;
                num = std::strtoull(param_str.c_str(), nullptr, HEX);
            } else {
                const int DEC = 10;
                num = std::strtoull(param_str.c_str(), nullptr, DEC);
            }
            if (errno == ERANGE) {
                errstr_ +=
                    "pandargs: \"" + arg->GetName() + "\" argument has invalid parameter value \"" + param_str + "\"\n";
            }

            if (IsIntegerArgInRange<uint64_t>(arg, num)) {
                arg->SetValue(num);
            } else {
                errstr_ += "pandargs: \"" + arg->GetName() + "\" argument has out of range parameter value \"" +
                           param_str + "\"\n";
            }
        } else {
            errstr_ +=
                "pandargs: \"" + arg->GetName() + "\" argument has invalid parameter value \"" + param_str + "\"\n";
        }
        return 1;
    }

    std::size_t ParseUint32ArgParam(PandArg<uint32_t> *arg, std::string_view argstr)
    {
        std::string param_str(argstr);
        if (IsUintNumber(param_str)) {
            errno = 0;
            uint32_t num;
            if (StartsWith(param_str, "0x")) {
                const int HEX = 16;
                num = std::strtoull(param_str.c_str(), nullptr, HEX);
            } else {
                const int DEC = 10;
                num = std::strtoull(param_str.c_str(), nullptr, DEC);
            }
            if (errno == ERANGE) {
                errstr_ +=
                    "pandargs: \"" + arg->GetName() + "\" argument has invalid parameter value \"" + param_str + "\"\n";
            }

            if (IsIntegerArgInRange<uint32_t>(arg, num)) {
                arg->SetValue(num);
            } else {
                errstr_ += "pandargs: \"" + arg->GetName() + "\" argument has out of range parameter value \"" +
                           param_str + "\"\n";
            }
        } else {
            errstr_ +=
                "pandargs: \"" + arg->GetName() + "\" argument has invalid parameter value \"" + param_str + "\"\n";
        }
        return 1;
    }

    std::size_t ParseListArgParam(PandArg<arg_list_t> *arg, std::string_view argstr)
    {
        std::string param_str(argstr);
        arg_list_t value;
        if (arg->WasSet()) {
            value = arg->GetValue();
        } else {
            value = arg_list_t();
        }
        if (!arg->GetDelimiter().has_value()) {
            value.push_back(param_str);
            arg->SetValue(value);
            return 1;
        }
        std::string delimiter = arg->GetDelimiter().value();
        std::size_t param_str_index = 0;
        std::size_t pos = param_str.find_first_of(delimiter, param_str_index);
        while (pos < param_str.size()) {
            value.push_back(param_str.substr(param_str_index, pos - param_str_index));
            param_str_index = pos;
            param_str_index = param_str.find_first_not_of(delimiter, param_str_index);
            pos = param_str.find_first_of(delimiter, param_str_index);
        }

        value.push_back(param_str.substr(param_str_index, pos - param_str_index));
        arg->SetValue(value);
        return 1;
    }

    static std::size_t NextSeparator(std::string_view argstr, std::size_t pos = 0,
                                     const std::string &separarors = EQ_SEPARATOR)
    {
        return argstr.find_first_of(separarors, pos);
    }

    static bool IsIntegerNumber(const std::string_view &str)
    {
        if (str.empty()) {
            return false;
        }
        std::size_t pos = 0;
        // look for dash if it's negative one
        if (str[0] == '-') {
            pos++;
        }
        // look for hex-style integer
        if ((str.size() > pos + 2U) && (str.compare(pos, 2U, "0x") == 0)) {
            pos += HEX_PREFIX_WIDTH;
            return str.find_first_not_of("0123456789abcdefABCDEF", pos) == std::string::npos;
        }
        return str.find_first_not_of("0123456789", pos) == std::string::npos;
    }

    static bool IsRationalNumber(const std::string_view &str)
    {
        if (str.empty()) {
            return false;
        }
        std::size_t pos = 0;
        // look for dash if it's negative one
        if (str[0] == '-') {
            pos++;
        }
        return str.find_first_not_of(".0123456789", pos) == std::string::npos;
    }

    static bool IsUintNumber(const std::string_view &str)
    {
        if (str.empty()) {
            return false;
        }

        constexpr std::string_view char_search = "0123456789";
        constexpr std::string_view char_search_hex = "0123456789abcdef";
        std::size_t pos = 0;
        // look for hex-style uint_t integer
        if (str[0] == '0' && str[1] == 'x') {
            pos += HEX_PREFIX_WIDTH;
        }
        return str.find_first_not_of((pos != 0) ? char_search_hex.data() : char_search.data(), pos) ==
               std::string::npos;
    }

    template <typename T,
              enable_if_t<is_same_v<T, int> || is_same_v<T, uint32_t> || is_same_v<T, uint64_t>> * = nullptr>
    bool IsIntegerArgInRange(PandArg<T> *arg, T num)
    {
        if (!(arg->GetMinMaxVal().has_value())) {
            return true;
        }
        std::pair<T, T> min_max = arg->GetMinMaxVal().value();
        return ((num >= std::get<0>(min_max)) && (num <= std::get<1>(min_max)));
    }

    static bool StartsWith(const std::string &haystack, const std::string &needle)
    {
        return std::equal(needle.begin(), needle.end(), haystack.begin());
    }

    void SetBoolUnexpectedValueError(PandArg<bool> *arg, const std::string &wrongvalue)
    {
        errstr_ += "pandargs: Bool argument " + arg->GetName() + " has unexpected parameter value " + wrongvalue + "\n";
        arg->ResetDefaultValue();
    }

    constexpr static size_t HEX_PREFIX_WIDTH = 2;
    constexpr static unsigned int DASH_COUNT = 2;

    std::vector<std::string> argv_vec_;
    std::size_t argv_index_ = 0;
    std::string errstr_ = "";
    bool tail_flag_ = false;
    bool remainder_flag_ = false;
    bool equal_flag_ = false;
    bool tail_parsed_flag_ = false;
    static constexpr const char *DOUBLE_DASH = "--";
    static constexpr const char *EQ_SEPARATOR = "=";
    std::set<PandArgBase *, PandArgPtrComparator> args_;
    std::vector<PandArgBase *> tail_args_;
    arg_list_t remainder_ = arg_list_t();
};

}  // namespace panda

#endif  // LIBPANDABASE_UTILS_PANDARGS_H

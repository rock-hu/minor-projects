/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef COMPILER_COMPILER_OPTIONS_H
#define COMPILER_COMPILER_OPTIONS_H

#include "utils/pandargs.h"
#include "libpandabase/utils/arch.h"
#include "cpu_features.h"
#include "compiler_options_gen.h"

#include <regex>

namespace panda::compiler {

#include "cpu_features.inc"

enum CpuFeature : uint8_t {
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEF(COMPONENT, ...) COMPONENT,
    CPU_FEATURE(DEF)
#undef DEF
        CPU_FEATURES_NUM
};

class CompilerOptions;
extern CompilerOptions options;

/**
 * Extends `compiler::Options`, which may be not sufficient to provide the desired functionality
 * (e.g. store an option-related variable)
 */
class CompilerOptions : public Options {
public:
    explicit CompilerOptions(const std::string &exe_path) : Options(exe_path) {}
    NO_MOVE_SEMANTIC(CompilerOptions);
    NO_COPY_SEMANTIC(CompilerOptions);
    ~CompilerOptions() = default;

    /**
     * `--compiler-regex` extension.
     * The purpose of this extension is to avoid unnecessary construction of std::regex from
     * `Options::GetCompilerRegex()` on every call to `MatchesRegex()`.
     *
     * Static local variable doesn't suit as soon as `Options::SetCompilerRegex()` is used (e.g. in
     * tests).
     */
    void SetCompilerRegex(const std::string &new_regex_pattern)
    {
        Options::SetCompilerRegex(new_regex_pattern);
        regex_ = new_regex_pattern;
    }
    template <typename T>
    bool MatchesRegex(const T &method_name)
    {
        if (!WasSetCompilerRegex()) {
            return true;
        }
        if (!regex_initialized_) {
            regex_ = GetCompilerRegex();
            regex_initialized_ = true;
        }
        return std::regex_match(method_name, regex_);
    }

    void AdjustCpuFeatures(bool cross_compilation)
    {
        ParseEnabledCpuFeatures();
        if (cross_compilation || WasSetCompilerCpuFeatures()) {
            return;
        }
        switch (RUNTIME_ARCH) {
            case Arch::AARCH64: {
                if (CpuFeaturesHasCrc32()) {
                    EnableCpuFeature(CRC32);
                }
                break;
            }
            case Arch::AARCH32:
                break;
            case Arch::X86:
                break;
            case Arch::X86_64:
                break;
            case Arch::NONE:
                break;
            default:
                break;
        }
    }

    bool IsCpuFeatureEnabled(CpuFeature feature) const
    {
        return features_.test(feature);
    }

private:
    void EnableCpuFeature(CpuFeature feature)
    {
        features_.set(feature);
    }

    void ParseEnabledCpuFeatures()
    {
        for (const auto &arg : GetCompilerCpuFeatures()) {
            if (arg == "none") {
                features_.reset();
                break;
            }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEF(FEATURE, NAME)         \
    if (NAME == arg) {             \
        EnableCpuFeature(FEATURE); \
        continue;                  \
    }
            CPU_FEATURE(DEF)
#undef DEF

            UNREACHABLE();
        }
    }

    // `--compiler-regex`:
    std::regex regex_;
    bool regex_initialized_ {false};
    std::bitset<CPU_FEATURES_NUM> features_;
};

}  // namespace panda::compiler

#endif  // COMPILER_COMPILER_OPTIONS_H

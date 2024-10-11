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

#ifndef ES2PANDA_UTIL_HELPERS_H
#define ES2PANDA_UTIL_HELPERS_H

#include "varbinder/variableFlags.h"
#include "mem/pool_manager.h"
#include "util/ustring.h"
#include "ir/module/importSpecifier.h"
#include "parser/program/program.h"

#include <cmath>
#include <string>

namespace ark::es2panda::varbinder {
class Variable;
}  // namespace ark::es2panda::varbinder

namespace ark::es2panda::checker {
class ETSObjectType;
class Type;
}  // namespace ark::es2panda::checker

namespace ark::es2panda::compiler {
class Literal;
}  // namespace ark::es2panda::compiler

namespace ark::es2panda::ir {
class Expression;
class ScriptFunction;
class ClassDefinition;
class ClassProperty;
class Identifier;
class MethodDefinition;
class AstNode;
class ReturnStatement;
class CallExpression;
class ClassStaticBlock;
class TSInterfaceDeclaration;
class TSEnumDeclaration;
class ETSImportDeclaration;
enum class AstNodeType;
}  // namespace ark::es2panda::ir

namespace ark::es2panda::util {
enum class LogLevel : std::uint8_t {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    INVALID,
};

class NodeAllocator {
public:
    template <typename T, typename... Args>
    static T *NoSetParent(ArenaAllocator *alloc, Args &&...args)
    {
        return alloc->New<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    static T *ForceSetParent(ArenaAllocator *alloc, Args &&...args)
    {
        auto *ret = NoSetParent<T>(alloc, std::forward<Args>(args)...);
        if (ret == nullptr) {
            return nullptr;
        }
        ret->Iterate([ret](ir::AstNode *child) { child->SetParent(ret); });
        return ret;
    }

    template <typename T, typename... Args>
    static T *Alloc(ArenaAllocator *alloc, Args &&...args)
    {
        auto *ret = NoSetParent<T>(alloc, std::forward<Args>(args)...);
        if (ret == nullptr) {
            return nullptr;
        }
        ret->Iterate([ret](ir::AstNode *child) {
            ASSERT(child->Parent() == nullptr);
            child->SetParent(ret);
        });
        return ret;
    }
};

class Helpers {
public:
    Helpers() = delete;

    static bool IsGlobalIdentifier(const util::StringView &str);
    static bool ContainSpreadElement(const ArenaVector<ir::Expression *> &args);
    static util::StringView LiteralToPropName(const ir::Expression *lit);

    template <typename T>
    static bool IsInteger(double number);
    static bool IsIndex(double number);
    static int64_t GetIndex(const util::StringView &str);

    static std::string ToString(double number);
    static util::StringView ToStringView(ArenaAllocator *allocator, double number);
    static util::StringView ToStringView(ArenaAllocator *allocator, int32_t number);
    static util::StringView ToStringView(ArenaAllocator *allocator, uint32_t number);
    static bool EndsWith(const std::string &str, const std::string &suffix);

    static const ir::ScriptFunction *GetContainingConstructor(const ir::AstNode *node);
    static const ir::ScriptFunction *GetContainingConstructor(const ir::ClassProperty *node);

    template <typename T,
              typename U = std::enable_if_t<
                  std::is_convertible_v<std::remove_const_t<std::remove_pointer_t<T>> *, ir::AstNode *>,
                  std::conditional_t<std::is_const_v<std::remove_pointer_t<T>>, const ir::AstNode *, ir::AstNode *>>>
    static U FindAncestorGivenByType(T node, ir::AstNodeType type)
    {
        U iter = node->Parent();

        while (iter->Type() != type) {
            if (iter->Parent() != nullptr) {
                iter = iter->Parent();
                continue;
            }

            return nullptr;
        }

        return iter;
    }

    static const checker::ETSObjectType *GetContainingObjectType(const ir::AstNode *node);
    static const ir::TSEnumDeclaration *GetContainingEnumDeclaration(const ir::AstNode *node);
    static const ir::ClassDefinition *GetContainingClassDefinition(const ir::AstNode *node);
    static const ir::TSInterfaceDeclaration *GetContainingInterfaceDeclaration(const ir::AstNode *node);
    static const ir::MethodDefinition *GetContainingClassMethodDefinition(const ir::AstNode *node);
    static const ir::ClassStaticBlock *GetContainingClassStaticBlock(const ir::AstNode *node);
    static const ir::ScriptFunction *GetContainingFunction(const ir::AstNode *node);
    static const ir::ClassDefinition *GetClassDefiniton(const ir::ScriptFunction *node);
    static bool IsSpecialPropertyKey(const ir::Expression *expr);
    static bool IsConstantPropertyKey(const ir::Expression *expr, bool isComputed);
    static compiler::Literal ToConstantLiteral(const ir::Expression *expr);
    static bool IsBindingPattern(const ir::AstNode *node);
    static bool IsPattern(const ir::AstNode *node);
    static std::vector<ir::Identifier *> CollectBindingNames(ir::AstNode *node);
    static util::StringView FunctionName(ArenaAllocator *allocator, const ir::ScriptFunction *func);
    static void CheckImportedName(const ArenaVector<ir::ImportSpecifier *> &specifiers,
                                  const ir::ImportSpecifier *specifier, const std::string &fileName);
    static void CheckDefaultImportedName(const ArenaVector<ir::ImportDefaultSpecifier *> &specifiers,
                                         const ir::ImportDefaultSpecifier *specifier, const std::string &fileName);
    static void CheckDefaultImport(const ArenaVector<ir::ETSImportDeclaration *> &statements);
    static std::tuple<util::StringView, bool> ParamName(ArenaAllocator *allocator, const ir::AstNode *param,
                                                        uint32_t index);

    template <typename T, typename V>
    static ArenaVector<T *> ConvertVector(const ArenaVector<V *> &src)
    {
        ArenaVector<T *> dst(src.begin(), src.end(), src.get_allocator());
        return dst;
    }

    template <typename Source, typename Target>
    static bool IsTargetFitInSourceRange(Target target)
    {
        if (!std::isfinite(target)) {
            return true;
        }

        // NOLINTNEXTLINE(misc-redundant-expression)
        return target >= std::numeric_limits<Source>::lowest() &&
               target <= static_cast<Target>(std::numeric_limits<Source>::max());
    }

    static const uint32_t INVALID_INDEX = 4294967295L;

    static std::string CreateEscapedString(const std::string &str);
    static std::string UTF16toUTF8(const char16_t c);

    template <typename... Elements>
    static void LogDebug(Elements &&...elems);
    template <typename... Elements>
    static void LogInfo(Elements &&...elems);
    template <typename... Elements>
    static void LogWarning(Elements &&...elems);
    template <typename... Elements>
    static void LogError(Elements &&...elems);
    template <typename... Elements>
    static void LogFatal(Elements &&...elems);

    template <typename... Elements>
    static std::string AppendAll(Elements &&...elems);

    static std::pair<std::string_view, std::string_view> SplitSignature(std::string_view signature);

    static std::vector<std::string> &StdLib();
    static bool IsStdLib(const parser::Program *program);

private:
    template <LogLevel LOG_L, typename... Elements>
    static void Log(Elements &&...elems);
};

template <typename T>
bool Helpers::IsInteger(double number)
{
    if (std::fabs(number) <= static_cast<double>(std::numeric_limits<T>::max())) {
        T intNum = static_cast<T>(number);

        if (static_cast<double>(intNum) == number) {
            return true;
        }
    }

    return false;
}

template <LogLevel LOG_L, typename... Elements>
void Helpers::Log(Elements &&...elems)
{
    constexpr auto ES2PANDA = ark::Logger::Component::ES2PANDA;
    constexpr auto LOG_LEVEL = []() {
        switch (LOG_L) {
            case LogLevel::DEBUG: {
                return ark::Logger::Level::DEBUG;
            }
            case LogLevel::INFO: {
                return ark::Logger::Level::INFO;
            }
            case LogLevel::WARNING: {
                return ark::Logger::Level::WARNING;
            }
            case LogLevel::ERROR: {
                return ark::Logger::Level::ERROR;
            }
            case LogLevel::FATAL: {
                return ark::Logger::Level::FATAL;
            }
            default: {
                UNREACHABLE_CONSTEXPR();
            }
        }
    }();

#ifndef NDEBUG
    const bool isMessageSuppressed = ark::Logger::IsMessageSuppressed(LOG_LEVEL, ES2PANDA);
#else
    const bool isMessageSuppressed = false;
#endif
    if (!ark::Logger::IsLoggingOnOrAbort(LOG_LEVEL, ES2PANDA) || isMessageSuppressed) {
        return;
    }

    (ark::Logger::Message(LOG_LEVEL, ES2PANDA, false).GetStream() << ... << std::forward<Elements>(elems));
}

template <typename... Elements>
void Helpers::LogDebug(Elements &&...elems)
{
    Helpers::Log<LogLevel::DEBUG>(std::forward<Elements>(elems)...);
}

template <typename... Elements>
void Helpers::LogInfo(Elements &&...elems)
{
    Helpers::Log<LogLevel::INFO>(std::forward<Elements>(elems)...);
}

template <typename... Elements>
void Helpers::LogWarning(Elements &&...elems)
{
    Helpers::Log<LogLevel::WARNING>(std::forward<Elements>(elems)...);
}

template <typename... Elements>
void Helpers::LogError(Elements &&...elems)
{
    Helpers::Log<LogLevel::ERROR>(std::forward<Elements>(elems)...);
}

template <typename... Elements>
void Helpers::LogFatal(Elements &&...elems)
{
    Helpers::Log<LogLevel::FATAL>(std::forward<Elements>(elems)...);
}

template <typename... Elements>
std::string Helpers::AppendAll(Elements &&...elems)
{
    std::string ret {};
    ((ret += std::forward<Elements>(elems)), ...);
    return ret;
}

}  // namespace ark::es2panda::util

#endif

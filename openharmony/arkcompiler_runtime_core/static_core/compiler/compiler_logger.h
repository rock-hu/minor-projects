/*
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

#ifndef COMPILER_COMPILER_LOGGER_H
#define COMPILER_COMPILER_LOGGER_H

#include "utils/logger.h"
#include <bitset>
#include <vector>

namespace ark::compiler {
#include "compiler_logger_components.inc"

enum CompilerLoggerComponents : uint8_t {
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEF(COMPONENT, ...) COMPONENT,  // CC-OFF(G.PRE.02) list generation
    COMPILER_LOG_COMPONENTS(DEF)
#undef DEF
        LOG_COMPONENTS_NUM
};

class CompilerLogger {
    NO_COPY_SEMANTIC(CompilerLogger);
    NO_MOVE_SEMANTIC(CompilerLogger);

public:
    static void SetComponents(const std::vector<std::string> &args);

    static void Init([[maybe_unused]] const std::vector<std::string> &args)
    {
#ifndef NDEBUG
        SetComponents(args);
#endif
    }

    static inline bool IsComponentEnabled(CompilerLoggerComponents comp)
    {
        return components_.test(comp);
    }

    static void EnableComponent(CompilerLoggerComponents comp)
    {
        components_.set(comp);
    }

private:
    CompilerLogger() = default;
    ~CompilerLogger() = default;

private:
    static std::bitset<CompilerLoggerComponents::LOG_COMPONENTS_NUM> components_;
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define COMPILER_LOG(level, comp) /* CC-OFFNXT(G.PRE.02) name part */ \
    CompilerLogger::IsComponentEnabled(CompilerLoggerComponents::comp) && LOG(level, COMPILER) << "[" #comp "] "
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define COMPILER_LOG_IF(cond, level, comp) /* CC-OFFNXT(G.PRE.02) name part */                          \
    CompilerLogger::IsComponentEnabled(CompilerLoggerComponents::comp) && LOG_IF(cond, level, COMPILER) \
                                                                              << "[" #comp "] "
}  // namespace ark::compiler

#endif  // COMPILER_COMPILER_LOGGER_H

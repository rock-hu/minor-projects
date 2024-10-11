/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef LIBLLVMBACKEND_LLVM_LOGGER_H
#define LIBLLVMBACKEND_LLVM_LOGGER_H

#include "utils/logger.h"
#include <bitset>
#include <vector>

namespace ark::llvmbackend {
#include "llvm_logger_components.inc"

enum LLVMLoggerComponents : uint8_t {
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEF(COMPONENT, ...) COMPONENT,
    LLVM_LOG_COMPONENTS(DEF)
#undef DEF
        LOG_COMPONENTS_NUM
};

class LLVMLogger {
    NO_COPY_SEMANTIC(LLVMLogger);
    NO_MOVE_SEMANTIC(LLVMLogger);

public:
    static void SetComponents(const std::vector<std::string> &args);

    static void Init([[maybe_unused]] const std::vector<std::string> &args)
    {
#ifndef NDEBUG
        SetComponents(args);
#endif
    }

    static inline bool IsComponentEnabled(LLVMLoggerComponents comp, Logger::Level level)
    {
        return components_.test(comp) || level == Logger::Level::FATAL;
    }

    static void EnableComponent(LLVMLoggerComponents comp)
    {
        components_.set(comp);
    }

private:
    LLVMLogger() = default;
    ~LLVMLogger() = default;

private:
    static std::bitset<LLVMLoggerComponents::LOG_COMPONENTS_NUM> components_;
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LLVM_LOG(level, comp)                                                                 \
    ark::llvmbackend::LLVMLogger::IsComponentEnabled(                                         \
        ark::llvmbackend::LLVMLoggerComponents::comp, /* CC-OFF(G.PRE.02) namespace member */ \
        ark::Logger::Level::level) &&                 /* CC-OFF(G.PRE.02) namespace member */ \
        LOG(level, LLVM) << "[" #comp "] "

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LLVM_LOG_IF(cond, level, comp) (cond) && LLVM_LOG(level, comp)

}  // namespace ark::llvmbackend

#endif  // LIBLLVMBACKEND_LLVM_LOGGER_H

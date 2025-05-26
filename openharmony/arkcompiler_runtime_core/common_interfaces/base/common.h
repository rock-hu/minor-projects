/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef COMMON_INTERFACES_BASE_COMMON_H
#define COMMON_INTERFACES_BASE_COMMON_H

#include <cstddef>
#include <cstdint>
#include <iostream>

// Windows platform will define ERROR, cause compiler error
#ifdef ERROR
#undef ERROR
#endif

namespace panda {
#ifndef PANDA_TARGET_WINDOWS
#define PUBLIC_API __attribute__((visibility ("default")))
#else
#define PUBLIC_API __declspec(dllexport)
#endif

#define NO_INLINE_CC __attribute__((noinline))

#define LIKELY_CC(exp) (__builtin_expect((exp) != 0, true))
#define UNLIKELY_CC(exp) (__builtin_expect((exp) != 0, false))

#define UNREACHABLE_CC()                                             \
    do {                                                             \
        std::cerr << "This line should be unreachable" << std::endl; \
        std::abort();                                                \
        __builtin_unreachable();                                     \
    } while (0)

#define CHECK_CC(expr) \
    do { \
        if (UNLIKELY_CC(!(expr))) { \
            std::cerr << "CHECK FAILED: " << #expr; \
            std::cerr << "          IN: " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << std::endl; \
            std::abort(); \
        } \
    } while (0)

#if defined(NDEBUG)
#define ALWAYS_INLINE_CC __attribute__((always_inline))
#define DCHECK_CC(x)
#else
#define ALWAYS_INLINE_CC
#define DCHECK_CC(x) CHECK_CC(x)
#endif

#if defined(__cplusplus)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NO_COPY_CTOR_CC(TypeName) \
    TypeName(const TypeName&) = delete

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NO_COPY_OPERATOR_CC(TypeName) \
    void operator=(const TypeName&) = delete

// Disabling copy ctor and copy assignment operator.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NO_COPY_SEMANTIC_CC(TypeName) \
    NO_COPY_CTOR_CC(TypeName);        \
    NO_COPY_OPERATOR_CC(TypeName)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NO_MOVE_CTOR_CC(TypeName)                   \
    /* NOLINTNEXTLINE(misc-macro-parentheses) */ \
    TypeName(TypeName&&) = delete

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NO_MOVE_OPERATOR_CC(TypeName)               \
    /* NOLINTNEXTLINE(misc-macro-parentheses) */ \
    TypeName& operator=(TypeName&&) = delete

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NO_MOVE_SEMANTIC_CC(TypeName) \
    NO_MOVE_CTOR_CC(TypeName);        \
    NO_MOVE_OPERATOR_CC(TypeName)

#endif  // defined(__cplusplus)

enum class PUBLIC_API LOG_LEVEL : uint8_t {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4,
    FOLLOW = 100, // if hilog enabled follow hilog, otherwise use INFO level
};
}  // panda

#endif  // COMMON_INTERFACES_BASE_COMMON_H

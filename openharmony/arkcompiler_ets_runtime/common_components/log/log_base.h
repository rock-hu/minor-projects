/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMON_COMPONENTS_LOG_LOG_BASE_H
#define COMMON_COMPONENTS_LOG_LOG_BASE_H
 
#include <cstdint>
#include <iostream>
#include <sstream>
 
#include "common_interfaces/base/common.h"
 
enum class Level: uint8_t {
    VERBOSE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL_WITHOUT_ABORT,
    FATAL,
};
 
using ComponentMark = uint64_t;
enum class Component: uint64_t {
    NONE = 0ULL,
    GC = 1ULL << 0ULL,
    INTERPRETER = 1ULL << 1ULL,
    COMPILER = 1ULL << 2ULL,
    DEBUGGER = 1ULL << 3ULL,
    ECMASCRIPT = 1ULL << 4ULL,
    BUILTINS = 1ULL << 5ULL,
    TRACE = 1ULL << 6ULL,
    JIT = 1UL << 7ULL,
    BASELINEJIT = 1UL << 8ULL,
    SA = 1ULL << 9ULL,
    PGO = 1ULL << 10ULL,
    COMMON = 1ULL << 11ULL,
    NO_TAG = 0xFFFFFFFFULL >> 1ULL,
    ALL = 0xFFFFFFFFULL,
};
 
namespace panda {
#ifdef ENABLE_HILOG

#if ECMASCRIPT_ENABLE_VERBOSE_LEVEL_LOG
// print Debug level log if enable Verbose log
#define LOG_VERBOSE LOG_DEBUG
#else
#define LOG_VERBOSE LOG_LEVEL_MIN
#endif
#endif  // ENABLE_HILOG

struct LogOptions {
    Level level;
    ComponentMark component;
};
}  // namespace panda

#endif  // COMMON_COMPONENTS_LOG_LOG_BASE_H
 
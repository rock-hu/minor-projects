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

#ifndef ECMASCRIPT_BASE_CONFIG_H
#define ECMASCRIPT_BASE_CONFIG_H

#include "common_components/base/config.h"
#include "common_interfaces/base/common.h"
#include <cstdint>

namespace panda::ecmascript {
extern uint32_t g_isEnableCMCGC;
extern uint32_t g_isEnableCMCGCConcurrentRootMarking;

enum class PUBLIC_API RBMode : uint8_t {
    DEFAULT_RB = 0,  // Default read barrier mode to support both gc.
    FAST_NO_RB,   // Use to old gc read barrier mode.
    FAST_CMC_RB,  // Use to cmc gc read barrier mode.
};

#define ARK_INLINE __attribute__((always_inline))
#define ARK_NOINLINE __attribute__((noinline))

#define ECMASCRIPT_ENABLE_DEBUG_MODE 0
#define ECMASCRIPT_ENABLE_ARK_CONTAINER 1
#define ECMASCRIPT_ENABLE_VERBOSE_LEVEL_LOG 0

#define ECMASCRIPT_ENABLE_BUILTIN_LOG 0

#define ECMASCRIPT_ENABLE_SNAPSHOT 0
#define ECMASCRIPT_ENABLE_HEAP_DETAIL_STATISTICS 0
#define ECMASCRIPT_ENABLE_OPT_CODE_PROFILER 0
#define ECMASCRIPT_ENABLE_JIT_WARMUP_PROFILER 0
#define ECMASCRIPT_ENABLE_FUNCTION_CALL_TIMER 0
#define ECMASCRIPT_ENABLE_ELEMENTSKIND_ALWAY_GENERIC 0
#define ECMASCRIPT_ENABLE_SCOPE_LOCK_STAT 0
#define ECMASCRIPT_ENABLE_TRACE_IC 0
#define ECMASCRIPT_ENABLE_MEGA_PROFILER 0
#define ECMASCRIPT_ENABLE_TRACE_LOAD 0
#define ECMASCRIPT_ENABLE_TRACE_LOAD_MORE 0
#define ECMASCRIPT_ENABLE_TRACE_LOAD_VALUE 0
#define ECMASCRIPT_ENABLE_TRACE_STORE 0
#define ECMASCRIPT_ENABLE_TRACE_CALL  0
#define ECMASCRIPT_ENABLE_TRACE_DEFINEFUNC 0
#define ECMASCRIPT_ENABLE_LAZY_DEOPT_TRACE 0

#ifndef NDEBUG
#define ECMASCRIPT_ENABLE_INTERPRETER_LOG 1
#define ECMASCRIPT_ENABLE_RUNTIME_STAT 1
#define ECMASCRIPT_ENABLE_INTERPRETER_RUNTIME_STAT 1
#define ECMASCRIPT_ENABLE_BUILTINS_RUNTIME_STAT 1
#define ECMASCRIPT_ENABLE_ALLOCATE_AND_GC_RUNTIME_STAT 1
#define ECMASCRIPT_ENABLE_THREAD_STATE_CHECK 1
#define ECMASCRIPT_ENABLE_COLLECTING_OPCODES 1
#define ECMASCRIPT_ENABLE_INTERPRETER_ARKUINAITVE_TRACE 1
#else
#define ECMASCRIPT_ENABLE_INTERPRETER_LOG 0
#define ECMASCRIPT_ENABLE_BASELINE_LOG 0
#define ECMASCRIPT_ENABLE_RUNTIME_STAT 0
#define ECMASCRIPT_ENABLE_INTERPRETER_RUNTIME_STAT 0
#define ECMASCRIPT_ENABLE_BUILTINS_RUNTIME_STAT 0
#define ECMASCRIPT_ENABLE_ALLOCATE_AND_GC_RUNTIME_STAT 0
#define ECMASCRIPT_ENABLE_THREAD_STATE_CHECK 0
#define ECMASCRIPT_ENABLE_COLLECTING_OPCODES 0
#define ECMASCRIPT_ENABLE_INTERPRETER_ARKUINAITVE_TRACE 0
#endif
/*
 * 1. close ic
 * 2. enable handle-scope zap, zap reclaimed regions
 * 3. switch gc mode to full gc
 * 4. enable Cast() check
 * 5. enable new handle check
 * 6. enable barrier check
 * 7. enable special value check in jsnapi
 */
#if ECMASCRIPT_ENABLE_DEBUG_MODE
    #define ECMASCRIPT_ENABLE_IC 0
    #define ECMASCRIPT_ENABLE_ZAP_MEM 1
    #define ECMASCRIPT_SWITCH_GC_MODE_TO_FULL_GC 1
    #define ECMASCRIPT_ENABLE_CAST_CHECK 1
    #define ECMASCRIPT_ENABLE_NEW_HANDLE_CHECK 1
    #define ECMASCRIPT_ENABLE_BARRIER_CHECK 1
    #define ECMASCRIPT_ENABLE_NAPI_SPECIAL_CHECK 1
    #define ECMASCRIPT_ENABLE_STUB_ARGV_CHECK 1
    #define ECMASCRIPT_ENABLE_STUB_RESULT_CHECK 1
#elif defined(ECMASCRIPT_ENABLE_ASAN_DFX_CONFIG)
    #define ECMASCRIPT_ENABLE_IC 1
    #define ECMASCRIPT_ENABLE_ZAP_MEM 0
    #define ECMASCRIPT_SWITCH_GC_MODE_TO_FULL_GC 0
    #define ECMASCRIPT_ENABLE_CAST_CHECK 0
    #define ECMASCRIPT_ENABLE_NEW_HANDLE_CHECK 0
    #define ECMASCRIPT_ENABLE_BARRIER_CHECK 0
    #define ECMASCRIPT_ENABLE_NAPI_SPECIAL_CHECK 1
    #define ECMASCRIPT_ENABLE_STUB_ARGV_CHECK 0
    #define ECMASCRIPT_ENABLE_STUB_RESULT_CHECK 0
#elif defined(ECMASCRIPT_ENABLE_GC_DFX_OPTIONS)
    #define ECMASCRIPT_ENABLE_IC 1
    #define ECMASCRIPT_ENABLE_ZAP_MEM 1
    #define ECMASCRIPT_SWITCH_GC_MODE_TO_FULL_GC 0
    #define ECMASCRIPT_ENABLE_CAST_CHECK 1
    #define ECMASCRIPT_ENABLE_NEW_HANDLE_CHECK 1
    #define ECMASCRIPT_ENABLE_BARRIER_CHECK 1
    #define ECMASCRIPT_ENABLE_NAPI_SPECIAL_CHECK 0
    #define ECMASCRIPT_ENABLE_STUB_ARGV_CHECK 1
    #define ECMASCRIPT_ENABLE_STUB_RESULT_CHECK 1
#else
    #define ECMASCRIPT_ENABLE_IC 1
    #define ECMASCRIPT_ENABLE_ZAP_MEM 0
    #define ECMASCRIPT_SWITCH_GC_MODE_TO_FULL_GC 0
    #define ECMASCRIPT_ENABLE_CAST_CHECK 0
    #define ECMASCRIPT_ENABLE_NEW_HANDLE_CHECK 0
    #define ECMASCRIPT_ENABLE_BARRIER_CHECK 0
    #define ECMASCRIPT_ENABLE_NAPI_SPECIAL_CHECK 0
    #define ECMASCRIPT_ENABLE_STUB_ARGV_CHECK 0
    #define ECMASCRIPT_ENABLE_STUB_RESULT_CHECK 0
#endif

#if ECMASCRIPT_ENABLE_ZAP_MEM
    constexpr int INVALID_VALUE = 0x7;
#endif

#if defined(PANDA_TARGET_32)
    #define ECMASCRIPT_DISABLE_CONCURRENT_MARKING 1
#else
    #define ECMASCRIPT_DISABLE_CONCURRENT_MARKING 0
#endif

#define UNSHARED_CONSTANTPOOL_COUNT 1024

#define TARGET_X64 "x86_64-unknown-linux-gnu"
#define TARGET_AARCH64 "aarch64-unknown-linux-gnu"
#define TARGET_ARM32 "arm-unknown-linux-gnu"

}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_BASE_CONFIG_H

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

#include <cstdlib>
#include <cinttypes>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils/logger.h"

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static const char PANDA_TRACE_KEY[] = "PANDA_TRACE";
// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static const char TRACE_MARKER_PATH[] = "/sys/kernel/debug/tracing/trace_marker";

namespace ark::trace::internal {

PANDA_PUBLIC_API int g_traceMarkerFd = -1;
bool DoInit()
{
    if (g_traceMarkerFd != -1) {
        LOG(ERROR, TRACE) << "Already init.";
        return false;
    }

    const char *pandaTraceVal = std::getenv(PANDA_TRACE_KEY);
    if (pandaTraceVal == nullptr) {
        return false;
    }

    if (pandaTraceVal != std::string("1")) {
        LOG(INFO, TRACE) << "Cannot init, " << PANDA_TRACE_KEY << "=" << pandaTraceVal;
        return false;
    }

    // NOLINTNEXTLINE(hicpp-signed-bitwise,cppcoreguidelines-pro-type-vararg)
    g_traceMarkerFd = open(TRACE_MARKER_PATH, O_CLOEXEC | O_WRONLY);
    if (g_traceMarkerFd == -1) {
        PLOG(ERROR, TRACE) << "Cannot open file: " << TRACE_MARKER_PATH;
        return false;
    }

    LOG(INFO, TRACE) << "Trace enabled";
    return true;
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define WRITE_MESSAGE(...)                                                                           \
    do {                                                                                             \
        ASSERT(g_traceMarkerFd != -1);                                                               \
        if (UNLIKELY(dprintf(g_traceMarkerFd, __VA_ARGS__) < 0)) {                                   \
            LOG(ERROR, TRACE) << "Cannot write trace event. Try enabling tracing and run app again"; \
        }                                                                                            \
    } while (0)

PANDA_PUBLIC_API void DoBeginTracePoint(const char *str)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    WRITE_MESSAGE("B|%d|%s", getpid(), str);
}

PANDA_PUBLIC_API void DoEndTracePoint()
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    WRITE_MESSAGE("E|");
}

void DoIntTracePoint(const char *str, int32_t val)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    WRITE_MESSAGE("C|%d|%s|%d", getpid(), str, val);
}

void DoInt64TracePoint(const char *str, int64_t val)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    WRITE_MESSAGE("C|%d|%s|%" PRId64, getpid(), str, val);
}

}  // end namespace ark::trace::internal

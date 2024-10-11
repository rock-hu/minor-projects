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

#ifndef PANDA_TRACE_H_
#define PANDA_TRACE_H_

#include <string>
#include <sstream>
#include "macros.h"

namespace ark::trace {

namespace internal {
extern int g_traceMarkerFd;
bool DoInit();
void DoBeginTracePoint(const char *str);
void DoEndTracePoint();
void DoIntTracePoint(const char *str, int32_t val);
void DoInt64TracePoint(const char *str, int64_t val);
}  // namespace internal

inline bool IsEnabled()
{
    return internal::g_traceMarkerFd != -1;
}

inline void BeginTracePoint(const char *str)
{
    if (UNLIKELY(IsEnabled())) {
        internal::DoBeginTracePoint(str);
    }
}

inline void EndTracePoint()
{
    if (UNLIKELY(IsEnabled())) {
        internal::DoEndTracePoint();
    }
}

inline void IntTracePoint(const char *str, int32_t val)
{
    if (UNLIKELY(IsEnabled())) {
        internal::DoIntTracePoint(str, val);
    }
}

inline void Int64TracePoint(const char *str, int64_t val)
{
    if (UNLIKELY(IsEnabled())) {
        internal::DoInt64TracePoint(str, val);
    }
}

class ScopedTrace {
public:
    explicit ScopedTrace(const char *str)
    {
        BeginTracePoint(str);
    }
    explicit ScopedTrace(const std::string &str) : ScopedTrace(str.c_str()) {}

    ~ScopedTrace()
    {
        EndTracePoint();
    }

    NO_COPY_SEMANTIC(ScopedTrace);
    NO_MOVE_SEMANTIC(ScopedTrace);
};

namespace internal {
class ScopeTraceStremHelperBegin {
public:
    ScopeTraceStremHelperBegin() = default;
    ~ScopeTraceStremHelperBegin()
    {
        BeginTracePoint(messageBuffer_.str().c_str());
    }

    std::ostream &GetStream()
    {
        return messageBuffer_;
    }

    NO_COPY_SEMANTIC(ScopeTraceStremHelperBegin);
    NO_MOVE_SEMANTIC(ScopeTraceStremHelperBegin);

private:
    std::ostringstream messageBuffer_;
};

class ScopeTraceStremHelperEnd {
public:
    ScopeTraceStremHelperEnd() = default;
    ~ScopeTraceStremHelperEnd()
    {
        EndTracePoint();
    }

    NO_COPY_SEMANTIC(ScopeTraceStremHelperEnd);
    NO_MOVE_SEMANTIC(ScopeTraceStremHelperEnd);
};
}  // namespace internal

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SCOPED_TRACE_STREAM                                                                  \
    ::ark::trace::internal::ScopeTraceStremHelperEnd MERGE_WORDS(end_trace_point, __LINE__); \
    UNLIKELY(::ark::trace::IsEnabled()) && ::ark::trace::internal::ScopeTraceStremHelperBegin().GetStream()

}  // namespace ark::trace

#endif  // PANDA_TRACE_H_

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

#ifndef PANDA_LIBPANDABASE_OS_EXEC_H_
#define PANDA_LIBPANDABASE_OS_EXEC_H_

#include "libpandabase/utils/expected.h"
#include "libpandabase/utils/span.h"
#include "libpandabase/os/error.h"
#include <array>

#if defined(PANDA_TARGET_UNIX)
#include "platforms/unix/libpandabase/exec.h"
#elif defined(PANDA_TARGET_WINDOWS)
#include "platforms/windows/libpandabase/exec.h"
#else
#error "Unsupported platform"
#endif

namespace ark::os::exec {

PANDA_PUBLIC_API Expected<int, Error> Exec(Span<const char *> args);
PANDA_PUBLIC_API Expected<int, Error> ExecNoWait(Span<const char *> args);
PANDA_PUBLIC_API Expected<int, Error> Wait(int64_t process, bool testStatus);

template <typename... Args>
decltype(auto) Exec(Args... args)
{
    std::array<const char *, sizeof...(args) + 1> arguments = {args..., nullptr};
    return os::exec::Exec(Span(arguments));
}

template <typename... Args>
decltype(auto) ExecNoWait(Args... args)
{
    std::array<const char *, sizeof...(args) + 1> arguments = {args..., nullptr};
    return os::exec::ExecNoWait(Span(arguments));
}

template <typename Callback, typename... Args>
decltype(auto) ExecWithCallback(Callback callback, Args... args)
{
    std::array<const char *, sizeof...(args) + 1> arguments = {args..., nullptr};
    return os::exec::ExecWithCallback(callback, Span(arguments));
}

template <typename Callback, typename... Args>
decltype(auto) ExecWithCallbackNoWait(Callback callback, Args... args)
{
    std::array<const char *, sizeof...(args) + 1> arguments = {args..., nullptr};
    return os::exec::ExecWithCallbackNoWait(callback, Span(arguments));
}

}  // namespace ark::os::exec

#endif  // PANDA_LIBPANDABASE_OS_EXEC_H_

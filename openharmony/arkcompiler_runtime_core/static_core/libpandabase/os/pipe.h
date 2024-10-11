/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_LIBPANDABASE_OS_PIPE_H
#define PANDA_LIBPANDABASE_OS_PIPE_H

#include "libpandabase/utils/expected.h"
#include "libpandabase/os/error.h"
#include "libpandabase/os/unique_fd.h"

#include <utility>
#include <optional>

namespace ark::os {

using UniqueFd = ark::os::unique_fd::UniqueFd;

PANDA_PUBLIC_API std::pair<UniqueFd, UniqueFd> CreatePipe();

PANDA_PUBLIC_API int SetFdNonblocking(const UniqueFd &fd);

PANDA_PUBLIC_API Expected<size_t, Error> ReadFromPipe(const UniqueFd &pipeFd, void *buf, size_t size);

PANDA_PUBLIC_API Expected<size_t, Error> WriteToPipe(const UniqueFd &pipeFd, const void *buf, size_t size);

enum class EventType { READY };

PANDA_PUBLIC_API Expected<size_t, Error> WaitForEvent(const UniqueFd *handles, size_t size, EventType type);

PANDA_PUBLIC_API std::optional<Error> Dup2(const UniqueFd &source, const UniqueFd &target);

}  // namespace ark::os

#endif  // PANDA_LIBPANDABASE_OS_PIPE_H

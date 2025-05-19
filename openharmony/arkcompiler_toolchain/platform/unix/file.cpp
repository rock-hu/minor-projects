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

#include "platform/file.h"

#include "common/log_wrapper.h"

namespace OHOS::ArkCompiler::Toolchain {
void FdsanExchangeOwnerTag(fd_t fd)
{
#if defined(OHOS_PLATFORM)
    fdsan_exchange_owner_tag(fd, 0, LOG_DOMAIN);
#else
    LOGD("Unsupport FdsanExchangeOwnerTag fd(%{public}d)", fd);
#endif
}

void FdsanClose(fd_t fd)
{
#if defined(OHOS_PLATFORM)
    fdsan_close_with_tag(fd, LOG_DOMAIN);
#else
    close(fd);
#endif
}
} // namespace OHOS::ArkCompiler::Toolchain
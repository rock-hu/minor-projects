/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/event/back_end_event_manager.h"

namespace OHOS::Ace {
namespace {

std::atomic<uint64_t> g_currentId = 0;

} // namespace

BackEndEventIdManager::BackEndEventIdManager() = default;
BackEndEventIdManager::~BackEndEventIdManager() = default;

std::string BackEndEventIdManager::GetAvailableId()
{
    ACE_DCHECK(g_currentId.load() != UINT64_MAX);
    return std::to_string(++g_currentId);
}

} // namespace OHOS::Ace
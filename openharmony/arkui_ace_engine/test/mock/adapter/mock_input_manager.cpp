/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "base/input_manager/input_manager.h"

#include "core/event/key_event.h"

namespace OHOS::Ace {
bool InputManager::GetSystemHotkeys(std::vector<HotKey>& hotkeys)
{
    std::set<int32_t> prekeys { static_cast<int32_t>(KeyCode::KEY_ALT_LEFT) };
    int32_t finalKey = static_cast<int32_t>(KeyCode::KEY_F4);
    hotkeys.push_back({ prekeys, finalKey });

    return true;
}

std::shared_ptr<MMI::PointerEvent> InputManager::CreatePointerEvent(
    const std::shared_ptr<const MMI::PointerEvent>& pointerEvent)
{
    return nullptr;
}
} // namespace OHOS::Ace
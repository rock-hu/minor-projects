/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#pragma once

#include "core/event/ace_events.h"
#include "arkoala_api_generated.h"

struct BaseEventPeer {
    virtual ~BaseEventPeer() = default;

    virtual OHOS::Ace::BaseEventInfo* GetBaseInfo() = 0;
};

namespace OHOS::Ace::NG::GeneratedModifier {
template<typename AceInfo,
    std::enable_if_t<std::is_base_of_v<BaseEventInfo, AceInfo>, bool> = true
>
class SomeEventPeer : public BaseEventPeer {
public:
    using AceEventInfo = AceInfo;

    ~SomeEventPeer() override = default;

    BaseEventInfo* GetBaseInfo() override
    {
        return GetEventInfo();
    }

    AceInfo* GetEventInfo()
    {
        return eventInfo->ptr;
    }

    void SetEventInfo(AceInfo* info)
    {
        eventInfo->ptr = info;
    }

    struct EventInfo {
        AceInfo* ptr = nullptr;
        std::optional<AceInfo> object;
    };

    const std::shared_ptr<EventInfo> eventInfo = std::make_shared<EventInfo>();
};

using BaseEventPeerImpl = SomeEventPeer<BaseEventInfo>;
} // namespace OHOS::Ace::NG::GeneratedModifier

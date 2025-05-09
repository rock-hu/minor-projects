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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_CUSTOM_PAINT_CANVAS_EVENT_HUB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_CUSTOM_PAINT_CANVAS_EVENT_HUB_H

#include "core/components_ng/event/event_hub.h"

namespace OHOS::Ace::NG {
using ReadyEvent = std::function<void()>;

class CanvasEventHub : public EventHub {
    DECLARE_ACE_TYPE(CanvasEventHub, EventHub)

public:
    CanvasEventHub() = default;
    ~CanvasEventHub() override = default;

    void SetOnReady(ReadyEvent&& readyEvent)
    {
        readyEvent_ = std::move(readyEvent);
    }

    void FireReadyEvent() const
    {
        ACE_SCOPED_TRACE("CanvasEventHub::FireReadyEvent");
        if (readyEvent_) {
            readyEvent_();
        }
    }

private:
    ReadyEvent readyEvent_;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_CUSTOM_PAINT_CANVAS_EVENT_HUB_H

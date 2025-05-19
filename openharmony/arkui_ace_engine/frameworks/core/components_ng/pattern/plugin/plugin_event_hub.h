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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PLUGIN_PLUGIN_EVENT_HUB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PLUGIN_PLUGIN_EVENT_HUB_H

#include <functional>

#include "base/memory/ace_type.h"
#include "core/components_ng/event/event_hub.h"

namespace OHOS::Ace::NG {
using PluginCallback = std::function<void(const std::string&)>;

class PluginEventHub : public EventHub {
    DECLARE_ACE_TYPE(PluginEventHub, EventHub)

public:
    PluginEventHub() = default;
    ~PluginEventHub() override = default;

    void SetOnError(PluginCallback&& onError)
    {
        onError_ = std::move(onError);
    }

    void SetOnComplete(PluginCallback&& OnComplete)
    {
        OnComplete_ = std::move(OnComplete);
    }

    void FireOnComplete(const std::string& param) const
    {
        if (OnComplete_) {
            OnComplete_(param);
        }
    }

    void FireOnError(const std::string& param) const
    {
        if (onError_) {
            onError_(param);
        }
    }

private:
    PluginCallback onError_;
    PluginCallback OnComplete_;

    ACE_DISALLOW_COPY_AND_MOVE(PluginEventHub);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PLUGIN_PLUGIN_EVENT_HUB_H

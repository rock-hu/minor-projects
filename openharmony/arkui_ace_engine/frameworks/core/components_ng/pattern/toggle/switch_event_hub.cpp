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

#include "core/components_ng/pattern/toggle/switch_event_hub.h"

#include "core/common/recorder/event_recorder.h"
#include "core/common/recorder/node_data_cache.h"
#include "core/components_ng/base/frame_node.h"
#include "core/pipeline/pipeline_base.h"

namespace OHOS::Ace::NG {

void SwitchEventHub::UpdateChangeEvent(bool isOn) const
{
    auto task = [changeEvent = changeEvent_, onChangeEvent = onChangeEvent_, isOn]() {
        if (onChangeEvent) {
            onChangeEvent(isOn);
        }
        if (changeEvent) {
            changeEvent(isOn);
        }
    };
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->PostAsyncEvent(task, "ArkUIToggleUpdateChangeEvent", TaskExecutor::TaskType::UI);

    if (Recorder::EventRecorder::Get().IsComponentRecordEnable()) {
        Recorder::EventParamsBuilder builder;
        auto host = GetFrameNode();
        if (host) {
            auto id = host->GetInspectorIdValue("");
            builder.SetId(id)
                .SetType(host->GetHostTag())
                .SetHost(host)
                .SetDescription(host->GetAutoEventParamValue(""));
            if (!id.empty()) {
                Recorder::NodeDataCache::Get().PutBool(host, id, isOn);
            }
        }
        builder.SetChecked(isOn);
        Recorder::EventRecorder::Get().OnChange(std::move(builder));
    }
}

} // namespace OHOS::Ace::NG

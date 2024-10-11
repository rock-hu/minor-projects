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

#include "core/components_ng/pattern/node_container/node_container_event_hub.h"

#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {

void NodeContainerEventHub::FireOnAppear()
{
    if (onAppearCallback_) {
        auto host = GetFrameNode();
        CHECK_NULL_VOID(host);
        auto pipeline = host->GetContext();
        CHECK_NULL_VOID(pipeline);
        auto taskScheduler = pipeline->GetTaskExecutor();
        CHECK_NULL_VOID(taskScheduler);
        taskScheduler->PostTask(
            [weak = WeakClaim(this)]() {
                auto eventHub = weak.Upgrade();
                CHECK_NULL_VOID(eventHub);
                if (eventHub->onAppearCallback_) {
                    // callback may be overwritten in its invoke so we copy it first
                    auto onAppear = eventHub->onAppearCallback_;
                    onAppear();
                }
            },
            TaskExecutor::TaskType::UI, "ArkUINodeControllerAboutToAppearEvent");
    }
    EventHub::FireOnAppear();
}

void NodeContainerEventHub::FireOnDisappear()
{
    if (onDisAppearCallback_) {
        // callback may be overwritten in its invoke so we copy it first
        auto onDisappear = onDisAppearCallback_;
        onDisappear();
    }
    EventHub::FireOnDisappear();
}
} // namespace OHOS::Ace::NG

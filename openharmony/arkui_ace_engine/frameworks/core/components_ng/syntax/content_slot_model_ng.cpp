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

#include "core/components_ng/syntax/content_slot_model_ng.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/syntax/content_slot_node.h"

namespace OHOS::Ace::NG::ContentSlotModel {

void Create(NodeContent* content)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto slot = ContentSlotNode::GetOrCreateContentSlot(nodeId);
    stack->Push(slot);
    if (content) {
        slot->AttachNodeContent(content);
    } else {
        slot->DetachNodeContent();
    }
}

} // namespace OHOS::Ace::NG::ContentSlotModel

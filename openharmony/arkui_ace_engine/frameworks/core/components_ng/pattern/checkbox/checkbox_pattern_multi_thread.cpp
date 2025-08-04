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

#include "core/components_ng/pattern/checkbox/checkbox_pattern.h"

#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
void CheckBoxPattern::OnAttachToFrameNodeMultiThread(const RefPtr<FrameNode>& frameNode)
{
    frameNode->GetLayoutProperty()->UpdateAlignment(Alignment::CENTER);
}

void CheckBoxPattern::OnAttachToMainTreeMultiThread(const RefPtr<FrameNode>& frameNode)
{
    UpdateNavIdAndState(frameNode);
    RegisterVisibleAreaChange();
}

void CheckBoxPattern::OnDetachFromFrameNodeMultiThread()
{
}

void CheckBoxPattern::OnDetachFromMainTreeMultiThread(const RefPtr<FrameNode>& frameNode)
{
    UpdateGroupStatus(frameNode.GetRawPtr());
}
} // namespace OHOS::Ace::NG
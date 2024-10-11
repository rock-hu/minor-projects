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

#include "core/components_ng/pattern/relative_container/relative_container_model_ng.h"

#include "core/components_ng/pattern/relative_container/relative_container_pattern.h"

namespace OHOS::Ace::NG {
void RelativeContainerModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    int32_t nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::RELATIVE_CONTAINER_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::RELATIVE_CONTAINER_ETS_TAG, nodeId,
        []() { return AceType::MakeRefPtr<OHOS::Ace::NG::RelativeContainerPattern>(); });
    ViewStackProcessor::GetInstance()->Push(frameNode);
}

void RelativeContainerModelNG::SetBarrier(const std::vector<BarrierInfo>& barrierInfo)
{
    if (!ViewStackProcessor::GetInstance()->IsCurrentVisualStateProcess()) {
        return;
    }
    ACE_UPDATE_LAYOUT_PROPERTY(RelativeContainerLayoutProperty, Barrier, barrierInfo);
}

void RelativeContainerModelNG::SetGuideline(const std::vector<GuidelineInfo>& guidelineInfo)
{
    if (!ViewStackProcessor::GetInstance()->IsCurrentVisualStateProcess()) {
        return;
    }
    ACE_UPDATE_LAYOUT_PROPERTY(RelativeContainerLayoutProperty, Guideline, guidelineInfo);
}

RefPtr<FrameNode> RelativeContainerModelNG::CreateFrameNode(int32_t nodeId)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::RELATIVE_CONTAINER_ETS_TAG, nodeId,
        []() { return AceType::MakeRefPtr<OHOS::Ace::NG::RelativeContainerPattern>(); });
    CHECK_NULL_RETURN(frameNode, frameNode);
    return frameNode;
}

void RelativeContainerModelNG::SetBarrier(FrameNode* frameNode, const std::vector<BarrierInfo>& barrierInfo)
{
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(RelativeContainerLayoutProperty, Barrier, barrierInfo, frameNode);
}

void RelativeContainerModelNG::SetGuideline(FrameNode* frameNode, const std::vector<GuidelineInfo>& guidelineInfo)
{
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(RelativeContainerLayoutProperty, Guideline, guidelineInfo, frameNode);
}

std::vector<BarrierInfo> RelativeContainerModelNG::GetBarrier(FrameNode* frameNode)
{
    std::vector<BarrierInfo> value;
    CHECK_NULL_RETURN(frameNode, value);
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(RelativeContainerLayoutProperty, Barrier, value, frameNode, value);
    return value;
}

std::vector<GuidelineInfo> RelativeContainerModelNG::GetGuideline(FrameNode* frameNode)
{
    std::vector<GuidelineInfo> value;
    CHECK_NULL_RETURN(frameNode, value);
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        RelativeContainerLayoutProperty, Guideline, value, frameNode, value);
    return value;
}

void RelativeContainerModelNG::ResetGuideline(FrameNode* frameNode)
{
    CHECK_NULL_VOID(frameNode);
    ACE_RESET_NODE_LAYOUT_PROPERTY(RelativeContainerLayoutProperty, Guideline, frameNode);
}

void RelativeContainerModelNG::ResetBarrier(FrameNode* frameNode)
{
    CHECK_NULL_VOID(frameNode);
    ACE_RESET_NODE_LAYOUT_PROPERTY(RelativeContainerLayoutProperty, Barrier, frameNode);
}
} // namespace OHOS::Ace::NG
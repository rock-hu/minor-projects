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

#include "core/components_ng/pattern/divider/divider_model_ng.h"

#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"

namespace OHOS::Ace::NG {

void DividerModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::DIVIDER_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::DIVIDER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<DividerPattern>(); });
    stack->Push(frameNode);
}

RefPtr<FrameNode> DividerModelNG::CreateFrameNode(int32_t nodeId)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::DIVIDER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<DividerPattern>(); });

    return frameNode;
}

void DividerModelNG::Vertical(bool value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(DividerLayoutProperty, Vertical, value);
}

void DividerModelNG::DividerColor(const Color& value)
{
    ACE_UPDATE_PAINT_PROPERTY(DividerRenderProperty, DividerColor, value);
}

void DividerModelNG::DividerColor(const RefPtr<ResourceObject>& resObj)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);

    auto dividerPattern = frameNode->GetPattern<DividerPattern>();
    CHECK_NULL_VOID(dividerPattern);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto frameNode = weak.Upgrade();
        CHECK_NULL_VOID(frameNode);
        auto dividerPattern = frameNode->GetPattern<DividerPattern>();
        CHECK_NULL_VOID(dividerPattern);
        std::string dividerColor = dividerPattern->GetResCacheMapByKey("divider.Color");
        Color result;
        if (dividerColor.empty()) {
            ResourceParseUtils::ParseResColor(resObj, result);
            dividerPattern->AddResCache("divider.Color", result.ColorToString());
        } else {
            result = Color::ColorFromString(dividerColor);
        }
        ACE_UPDATE_NODE_PAINT_PROPERTY(DividerRenderProperty, DividerColor, result, frameNode);
        frameNode->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    };
    updateFunc(resObj);
    dividerPattern->AddResObj("divider.Color", resObj, std::move(updateFunc));
}

void DividerModelNG::StrokeWidth(const Dimension& value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(DividerLayoutProperty, StrokeWidth, value);
}

void DividerModelNG::LineCap(const Ace::LineCap& value)
{
    ACE_UPDATE_PAINT_PROPERTY(DividerRenderProperty, LineCap, value);
}

void DividerModelNG::StrokeWidth(FrameNode* frameNode, const Dimension& value)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(DividerLayoutProperty, StrokeWidth, value, frameNode);
}

void DividerModelNG::LineCap(FrameNode* frameNode, const Ace::LineCap& value)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(DividerRenderProperty, LineCap, value, frameNode);
}

void DividerModelNG::SetDividerColor(FrameNode* frameNode, const Color& color)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(DividerRenderProperty, DividerColor, color, frameNode);
}

void DividerModelNG::SetVertical(FrameNode* frameNode, const bool& value)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(DividerLayoutProperty, Vertical, value, frameNode);
}

void DividerModelNG::ResetDividerColor()
{
    ACE_RESET_PAINT_PROPERTY_WITH_FLAG(DividerRenderProperty, DividerColor, PROPERTY_UPDATE_RENDER);
}
} // namespace OHOS::Ace::NG
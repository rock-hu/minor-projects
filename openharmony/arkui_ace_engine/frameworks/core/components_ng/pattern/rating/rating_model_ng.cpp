/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/rating/rating_model_ng.h"

#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/rating/rating_pattern.h"

namespace OHOS::Ace::NG {
void RatingModelNG::Create(double rating, bool indicator)
{
    auto* stack = ViewStackProcessor::GetInstance();
    CHECK_NULL_VOID(stack);
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::RATING_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::RATING_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RatingPattern>(); });
    stack->Push(frameNode);
    RatingModelNG::SetRatingScore(rating);
    RatingModelNG::SetIndicator(indicator);
}

void RatingModelNG::SetRatingScore(double value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto paintProperty = frameNode->GetPaintPropertyPtr<RatingRenderProperty>();
    CHECK_NULL_VOID(paintProperty);
    if (paintProperty->HasRatingScore() && !NearEqual(paintProperty->GetRatingScore().value(), value)) {
        TAG_LOGI(AceLogTag::ACE_SELECT_COMPONENT, "rating set score %{public}f", value);
    }
    paintProperty->UpdateRatingScore(value);
}

void RatingModelNG::SetIndicator(bool value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(RatingLayoutProperty, Indicator, value);
}

void RatingModelNG::SetStars(int32_t value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(RatingLayoutProperty, Stars, value);
}

void RatingModelNG::SetStepSize(double value)
{
    ACE_UPDATE_PAINT_PROPERTY(RatingRenderProperty, StepSize, value);
}

void RatingModelNG::SetForegroundSrc(const std::string& value, bool flag)
{
    if (flag) {
        ACE_RESET_LAYOUT_PROPERTY_WITH_FLAG(RatingLayoutProperty, ForegroundImageSourceInfo, PROPERTY_UPDATE_MEASURE);
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(RatingLayoutProperty, ForegroundImageSourceInfo, ImageSourceInfo(value));
    }
}

void RatingModelNG::SetSecondarySrc(const std::string& value, bool flag)
{
    if (flag) {
        ACE_RESET_LAYOUT_PROPERTY_WITH_FLAG(RatingLayoutProperty, SecondaryImageSourceInfo, PROPERTY_UPDATE_MEASURE);
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(RatingLayoutProperty, SecondaryImageSourceInfo, ImageSourceInfo(value));
    }
}

void RatingModelNG::SetBackgroundSrc(const std::string& value, bool flag)
{
    if (flag) {
        ACE_RESET_LAYOUT_PROPERTY_WITH_FLAG(RatingLayoutProperty, BackgroundImageSourceInfo, PROPERTY_UPDATE_MEASURE);
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(RatingLayoutProperty, BackgroundImageSourceInfo, ImageSourceInfo(value));
    }
}

void RatingModelNG::SetOnChange(RatingChangeEvent&& onChange)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<RatingEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChange(std::move(onChange));
}

void RatingModelNG::SetOnChangeEvent(RatingChangeEvent&& onChangeEvent)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<RatingEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChangeEvent(std::move(onChangeEvent));
}

RefPtr<FrameNode> RatingModelNG::CreateFrameNode(int32_t nodeId)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::RATING_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RatingPattern>(); });
    CHECK_NULL_RETURN(frameNode, nullptr);
    return frameNode;
}

void RatingModelNG::SetStars(FrameNode* frameNode, int32_t value)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(RatingLayoutProperty, Stars, value, frameNode);
}

void RatingModelNG::SetStepSize(FrameNode* frameNode, double value)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(RatingRenderProperty, StepSize, value, frameNode);
}

void RatingModelNG::SetForegroundSrc(FrameNode* frameNode, const std::string& value, bool flag)
{
    if (flag) {
        ACE_RESET_NODE_LAYOUT_PROPERTY_WITH_FLAG(
            RatingLayoutProperty, ForegroundImageSourceInfo, PROPERTY_UPDATE_MEASURE, frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            RatingLayoutProperty, ForegroundImageSourceInfo, ImageSourceInfo(value), frameNode);
    }
}

void RatingModelNG::SetSecondarySrc(FrameNode* frameNode, const std::string& value, bool flag)
{
    if (flag) {
        ACE_RESET_NODE_LAYOUT_PROPERTY_WITH_FLAG(
            RatingLayoutProperty, SecondaryImageSourceInfo, PROPERTY_UPDATE_MEASURE, frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            RatingLayoutProperty, SecondaryImageSourceInfo, ImageSourceInfo(value), frameNode);
    }
}

void RatingModelNG::SetBackgroundSrc(FrameNode* frameNode, const std::string& value, bool flag)
{
    if (flag) {
        ACE_RESET_NODE_LAYOUT_PROPERTY_WITH_FLAG(
            RatingLayoutProperty, BackgroundImageSourceInfo, PROPERTY_UPDATE_MEASURE, frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            RatingLayoutProperty, BackgroundImageSourceInfo, ImageSourceInfo(value), frameNode);
    }
}

void RatingModelNG::SetBuilderFunc(FrameNode* frameNode, NG::RatingMakeCallback&& makeFunc)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<RatingPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetBuilderFunc(std::move(makeFunc));
}

void RatingModelNG::SetChangeValue(FrameNode* frameNode, double value)
{
    auto pattern = frameNode->GetPattern<RatingPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetRatingScore(value);
}

void RatingModelNG::SetRatingOptions(FrameNode* frameNode, double rating, bool indicator)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(RatingLayoutProperty, Indicator, indicator, frameNode);
    ACE_UPDATE_NODE_PAINT_PROPERTY(RatingRenderProperty, RatingScore, rating, frameNode);
}

void RatingModelNG::SetOnChange(FrameNode* frameNode, RatingChangeEvent&& onChange)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<RatingEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChange(std::move(onChange));
}

void RatingModelNG::CreateWithMediaResourceObj(const RefPtr<ResourceObject>& resObj, const RatingUriType ratingUriType)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    CreateWithMediaResourceObj(frameNode, resObj, ratingUriType);
}

void RatingModelNG::CreateWithMediaResourceObj(
    FrameNode* frameNode, const RefPtr<ResourceObject>& resObj, const RatingUriType ratingUriType)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<RatingPattern>();
    CHECK_NULL_VOID(pattern);
    std::string key = "rating" + StringTypeToStr(ratingUriType);
    if (!resObj) {
        pattern->RemoveResObj(key);
        return;
    }
    auto updateFunc = [ratingUriType, weak = AceType::WeakClaim(frameNode)](
        const RefPtr<ResourceObject>& resObj) {
        auto frameNode = weak.Upgrade();
        CHECK_NULL_VOID(frameNode);
        std::string result;
        if (ResourceParseUtils::ParseResMedia(resObj, result)) {
            UpdateStarStyleImage(AceType::RawPtr(frameNode), ratingUriType, result);
        }
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void RatingModelNG::UpdateStarStyleImage(FrameNode* frameNode,
    const RatingUriType& ratingUriType, const std::string& result)
{
    CHECK_NULL_VOID(frameNode);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto ratingTheme = pipeline->GetTheme<RatingTheme>();
    CHECK_NULL_VOID(ratingTheme);
    auto iconTheme = pipeline->GetTheme<IconTheme>();
    CHECK_NULL_VOID(iconTheme);
    auto layoutProperty = frameNode->GetLayoutProperty<RatingLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto pattern = frameNode->GetPattern<RatingPattern>();
    CHECK_NULL_VOID(pattern);
    switch (ratingUriType) {
        case RatingUriType::BACKGROUND_URI:
            SetBackgroundSrc(frameNode, result, result.empty());
            pattern->LoadBackground(layoutProperty, ratingTheme, iconTheme);
            break;
        case RatingUriType::FOREGROUND_URI:
            SetForegroundSrc(frameNode, result, result.empty());
            pattern->LoadForeground(layoutProperty, ratingTheme, iconTheme);
            break;
        case RatingUriType::SECONDARY_URI:
            SetSecondarySrc(frameNode, result, result.empty());
            pattern->LoadSecondary(layoutProperty, ratingTheme, iconTheme);
            break;
        default:
            break;
    }
}

std::string RatingModelNG::StringTypeToStr(const RatingUriType ratingUriType)
{
    switch (ratingUriType) {
        case RatingUriType::BACKGROUND_URI:
            return "BackgroundUri";
        case RatingUriType::FOREGROUND_URI:
            return "ForegroundUri";
        case RatingUriType::SECONDARY_URI:
            return "SecondaryUri";
        default:
            return "";
    }
}
} // namespace OHOS::Ace::NG

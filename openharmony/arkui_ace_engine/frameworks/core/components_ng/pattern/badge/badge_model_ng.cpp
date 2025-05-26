/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/badge/badge_model_ng.h"

#include "core/common/resource/resource_parse_utils.h"
#include "core/components/badge/badge_theme.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::NG {
void BadgeModelNG::Create(BadgeParameters& badgeParameters)
{
    auto* stack = ViewStackProcessor::GetInstance();
    int32_t nodeId = (stack == nullptr ? 0 : stack->ClaimNodeId());
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::BADGE_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::BADGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<BadgePattern>(); });
    ViewStackProcessor::GetInstance()->Push(frameNode);

    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto badgeTheme = pipeline->GetTheme<BadgeTheme>();

    auto layoutProperty = frameNode->GetLayoutProperty<BadgeLayoutProperty>();
    layoutProperty->SetIsDefault(isDefaultFontSize_, isDefaultBadgeSize_);
    
    if (badgeParameters.badgeValue.has_value()) {
        layoutProperty->UpdateBadgeValue(badgeParameters.badgeValue.value());
    }
    if (badgeParameters.badgeCount.has_value()) {
        layoutProperty->UpdateBadgeCount(badgeParameters.badgeCount.value());
    }
    if (badgeParameters.badgeMaxCount.has_value()) {
        layoutProperty->UpdateBadgeMaxCount(badgeParameters.badgeMaxCount.value());
    } else {
        layoutProperty->UpdateBadgeMaxCount(badgeTheme->GetMaxCount());
    }

    if (badgeParameters.badgePosition.has_value()) {
        auto badgePosition = static_cast<BadgePosition>(badgeParameters.badgePosition.value());
        layoutProperty->UpdateBadgePosition(badgePosition);
    } else {
        layoutProperty->UpdateBadgePosition(badgeTheme->GetBadgePosition());
    }
    if (badgeParameters.badgePositionX.has_value()) {
        layoutProperty->UpdateBadgePositionX(badgeParameters.badgePositionX.value());
    } else {
        layoutProperty->UpdateBadgePositionX(badgeTheme->GetBadgePositionX());
    }
    if (badgeParameters.badgePositionY.has_value()) {
        layoutProperty->UpdateBadgePositionY(badgeParameters.badgePositionY.value());
    } else {
        layoutProperty->UpdateBadgePositionY(badgeTheme->GetBadgePositionY());
    }
    if (badgeParameters.isPositionXy.has_value()) {
        layoutProperty->UpdateIsPositionXy(badgeParameters.isPositionXy.value());
    } else {
        layoutProperty->UpdateIsPositionXy(badgeTheme->GetIsPositionXy());
    }
    UpdateBadgeStyle(badgeParameters, frameNode);
    if (SystemProperties::ConfigChangePerform()) {
        CreateWithResourceObj(frameNode, badgeParameters);
    }
}

void BadgeModelNG::UpdateBadgeStyle(BadgeParameters& badgeParameters, const RefPtr<FrameNode>& frameNode)
{
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
    auto layoutProperty = frameNode->GetLayoutProperty<BadgeLayoutProperty>();
    if (badgeParameters.badgeColor.has_value()) {
        layoutProperty->UpdateBadgeColor(badgeParameters.badgeColor.value());
    } else {
        layoutProperty->UpdateBadgeColor(badgeTheme->GetBadgeColor());
    }
    if (badgeParameters.badgeTextColor.has_value()) {
        layoutProperty->UpdateBadgeTextColor(badgeParameters.badgeTextColor.value());
    } else {
        layoutProperty->UpdateBadgeTextColor(badgeTheme->GetBadgeTextColor());
    }
    if (badgeParameters.badgeFontSize.has_value()) {
        layoutProperty->UpdateBadgeFontSize(badgeParameters.badgeFontSize.value());
    }
    if (badgeParameters.badgeCircleSize.has_value()) {
        layoutProperty->UpdateBadgeCircleSize(badgeParameters.badgeCircleSize.value());
    }
    if (badgeParameters.badgeBorderColor.has_value()) {
        layoutProperty->UpdateBadgeBorderColor(badgeParameters.badgeBorderColor.value());
    } else {
        layoutProperty->UpdateBadgeBorderColor(badgeTheme->GetBadgeBorderColor());
    }
    if (badgeParameters.badgeBorderWidth.has_value()) {
        layoutProperty->UpdateBadgeBorderWidth(badgeParameters.badgeBorderWidth.value());
    } else {
        layoutProperty->UpdateBadgeBorderWidth(badgeTheme->GetBadgeBorderWidth());
    }
    if (badgeParameters.badgeFontWeight.has_value()) {
        layoutProperty->UpdateBadgeFontWeight(badgeParameters.badgeFontWeight.value());
    } else {
        layoutProperty->UpdateBadgeFontWeight(FontWeight::NORMAL);
    }
}

RefPtr<FrameNode> BadgeModelNG::CreateFrameNode(int32_t nodeId)
{
    auto frameNode = FrameNode::CreateFrameNode(V2::BADGE_ETS_TAG, nodeId, AceType::MakeRefPtr<BadgePattern>());
    CHECK_NULL_RETURN(frameNode, nullptr);
    return frameNode;
}

void BadgeModelNG::SetBadgeParam(
    FrameNode* frameNode, BadgeParameters& badgeParameters, bool isDefaultFontSize, bool isDefaultBadgeSize)
{
    CHECK_NULL_VOID(frameNode);
    auto pipeline = frameNode->GetContextRefPtr();
    CHECK_NULL_VOID(pipeline);
    auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
    CHECK_NULL_VOID(badgeTheme);
    auto layoutProperty = frameNode->GetLayoutProperty<BadgeLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    layoutProperty->SetIsDefault(isDefaultFontSize, isDefaultBadgeSize);
    if (badgeParameters.badgeValue.has_value()) {
        layoutProperty->UpdateBadgeValue(badgeParameters.badgeValue.value());
    } else {
        layoutProperty->ResetBadgeValue();
    }
    if (badgeParameters.badgeCount.has_value()) {
        layoutProperty->UpdateBadgeCount(badgeParameters.badgeCount.value());
    }
    if (badgeParameters.badgeMaxCount.has_value()) {
        layoutProperty->UpdateBadgeMaxCount(badgeParameters.badgeMaxCount.value());
    } else {
        layoutProperty->UpdateBadgeMaxCount(badgeTheme->GetMaxCount());
    }

    if (badgeParameters.badgePosition.has_value()) {
        auto badgePosition = static_cast<BadgePosition>(badgeParameters.badgePosition.value());
        layoutProperty->UpdateBadgePosition(badgePosition);
    } else {
        layoutProperty->UpdateBadgePosition(badgeTheme->GetBadgePosition());
    }
    if (badgeParameters.badgePositionX.has_value()) {
        layoutProperty->UpdateBadgePositionX(badgeParameters.badgePositionX.value());
    } else {
        layoutProperty->UpdateBadgePositionX(badgeTheme->GetBadgePositionX());
    }
    if (badgeParameters.badgePositionY.has_value()) {
        layoutProperty->UpdateBadgePositionY(badgeParameters.badgePositionY.value());
    } else {
        layoutProperty->UpdateBadgePositionY(badgeTheme->GetBadgePositionY());
    }
    if (badgeParameters.isPositionXy.has_value()) {
        layoutProperty->UpdateIsPositionXy(badgeParameters.isPositionXy.value());
    } else {
        layoutProperty->UpdateIsPositionXy(badgeTheme->GetIsPositionXy());
    }
    UpdateBadgeStyle(badgeParameters, AceType::Claim(frameNode));
}

void BadgeModelNG::CreateWithResourceObj(const RefPtr<FrameNode>& frameNode, BadgeParameters& badgeParameters)
{
    CHECK_NULL_VOID(frameNode);
    auto badgePattern = frameNode->GetPattern<BadgePattern>();
    CHECK_NULL_VOID(badgePattern);
    ProcessBadgeValue(badgePattern, badgeParameters.resourceBadgeValueObject);
    ProcessBadgeTextColor(badgePattern, badgeParameters.resourceColorObject);
    ProcessBadgeColor(badgePattern, badgeParameters.resourceBadgeColorObject);
    ProcessBorderColor(badgePattern, badgeParameters.resourceBorderColorObject);
    ProcessFontWeight(badgePattern, badgeParameters.resourceFontWeightObject);
    ProcessFontSize(badgePattern, badgeParameters.resourceFontSizeObject);
    ProcessBadgeSize(badgePattern, badgeParameters.resourceBadgeSizeObject);
    ProcessBadgePositionX(badgePattern, badgeParameters.resourceBadgePositionXObject);
    ProcessBadgePositionY(badgePattern, badgeParameters.resourceBadgePositionYObject);
    ProcessBorderWidth(badgePattern, badgeParameters.resourceBorderWidthObject);
}

void BadgeModelNG::ProcessBadgeValue(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        std::string key = "badge.badgeValue";
        std::string badgeValue = badgePattern->GetResCacheMapByKey(key);
        if (badgeValue.empty()) {
            ResourceParseUtils::ParseResString(resObj, badgeValue);
            badgePattern->AddResCache(key, badgeValue);
        }
        if (!badgeValue.empty()) {
            badgePattern->UpdateBadgeValue(badgeValue, isFirstLoad);
        }
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.badgeValue", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessBadgeTextColor(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        std::string key = "badge.textColor";
        std::string cachedColor = badgePattern->GetResCacheMapByKey(key);
        Color result;
        if (cachedColor.empty()) {
            bool state = ResourceParseUtils::ParseResColor(resObj, result);
            if (state) {
                badgePattern->AddResCache(key, result.ColorToString());
            } else {
                auto pipeline = PipelineBase::GetCurrentContext();
                CHECK_NULL_VOID(pipeline);
                auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
                result = badgeTheme->GetBadgeTextColor();
            }
        } else {
            result = Color::ColorFromString(cachedColor);
        }
        badgePattern->UpdateColor(result, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.textColor", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessBadgeColor(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        std::string key = "badge.Color";
        std::string cachedColor = badgePattern->GetResCacheMapByKey(key);
        Color result;
        if (cachedColor.empty()) {
            bool state = ResourceParseUtils::ParseResColor(resObj, result);
            if (state) {
                badgePattern->AddResCache(key, result.ColorToString());
            } else {
                auto pipeline = PipelineBase::GetCurrentContext();
                CHECK_NULL_VOID(pipeline);
                auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
                result = badgeTheme->GetBadgeColor();
            }
        } else {
            result = Color::ColorFromString(cachedColor);
        }
        badgePattern->UpdateBadgeColor(result, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.Color", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessBorderColor(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        std::string key = "badge.BorderColor";
        std::string cachedBorderColor = badgePattern->GetResCacheMapByKey(key);
        Color result;
        if (cachedBorderColor.empty()) {
            bool state = ResourceParseUtils::ParseResColor(resObj, result);
            if (state) {
                badgePattern->AddResCache(key, result.ColorToString());
            } else {
                auto pipeline = PipelineBase::GetCurrentContext();
                CHECK_NULL_VOID(pipeline);
                auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
                result = badgeTheme->GetBadgeBorderColor();
            }
        } else {
            result = Color::ColorFromString(cachedBorderColor);
        }
        badgePattern->UpdateBorderColor(result, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.BorderColor", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessFontWeight(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        std::string key = "badge.FontWeight";
        std::optional<FontWeight> badgeFontWeight;
        std::string cachedFontWeight = badgePattern->GetResCacheMapByKey(key);
        std::string result;
        if (cachedFontWeight.empty()) {
            bool state = ResourceParseUtils::ParseResString(resObj, result);
            if (state) {
                badgePattern->AddResCache(key, result);
            }
        } else {
            badgeFontWeight = ConvertStrToFontWeight(cachedFontWeight);
        }

        badgePattern->UpdateFontWeight(
            badgeFontWeight.has_value() ? badgeFontWeight.value() : FontWeight::NORMAL, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.FontWeight", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessFontSize(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        bool isDefaultFontSize = true;
        CalcDimension result;
        bool state = ResourceParseUtils::ParseResDimensionFpNG(resObj, result);
        if (state) {
            if (result.IsNonNegative() && result.Unit() != DimensionUnit::PERCENT) {
                isDefaultFontSize = false;
            }
        } else {
            auto pipeline = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
            result = badgeTheme->GetBadgeFontSize();
        }
        badgePattern->UpdateFontSize(result, isDefaultFontSize, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.FontSize", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessBadgeSize(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        CalcDimension result;
        bool isDefaultBadgeSize = true;
        bool state = ResourceParseUtils::ParseResDimensionFpNG(resObj, result);
        if (state) {
            if (result.IsNonNegative() && result.Unit() != DimensionUnit::PERCENT) {
                isDefaultBadgeSize = false;
            }
        } else {
            auto pipeline = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
            result = badgeTheme->GetBadgeCircleSize();
        }
        badgePattern->UpdateBadgeCircleSize(result, isDefaultBadgeSize, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.CircleSize", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessBadgePositionX(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        CalcDimension result;
        if (!ResourceParseUtils::ParseResDimensionVp(resObj, result)) {
            auto pipeline = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
            result = badgeTheme->GetBadgePositionX();
        }
        badgePattern->UpdateBadgePositionX(result, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.positionX", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessBadgePositionY(
    const RefPtr<BadgePattern>& badgePattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [badgePattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        CalcDimension result;
        if (!ResourceParseUtils::ParseResDimensionVp(resObj, result)) {
            auto pipeline = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
            result = badgeTheme->GetBadgePositionY();
        }
        badgePattern->UpdateBadgePositionY(result, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    badgePattern->AddResObj("badge.positionY", resourceObject, std::move(updateFunc));
}

void BadgeModelNG::ProcessBorderWidth(const RefPtr<BadgePattern>& pattern, const RefPtr<ResourceObject>& resourceObject)
{
    if (!resourceObject) {
        return;
    }
    auto updateFunc = [pattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        CalcDimension result;
        if (!ResourceParseUtils::ParseResDimensionVp(resObj, result)) {
            auto pipeline = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto badgeTheme = pipeline->GetTheme<BadgeTheme>();
            result = badgeTheme->GetBadgeBorderWidth();
        }
        pattern->UpdateBorderWidth(result, isFirstLoad);
    };
    updateFunc(resourceObject, true);
    pattern->AddResObj("badge.borderWidth", resourceObject, std::move(updateFunc));
}
} // namespace OHOS::Ace::NG

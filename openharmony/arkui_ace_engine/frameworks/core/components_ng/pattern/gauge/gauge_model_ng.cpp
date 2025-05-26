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

#include "core/components_ng/pattern/gauge/gauge_model_ng.h"

#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/gauge/gauge_pattern.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
void GaugeModelNG::Create(float value, float min, float max)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::GAUGE_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::GAUGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<GaugePattern>(); });
    stack->Push(frameNode);

    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, Value, value);
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, Max, max);
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, Min, min);
}

void GaugeModelNG::SetValue(float value)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, Value, value);
}

void GaugeModelNG::SetStartAngle(float startAngle)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, StartAngle, startAngle);
    ACE_UPDATE_LAYOUT_PROPERTY(GaugeLayoutProperty, StartAngle, startAngle);
}

void GaugeModelNG::SetEndAngle(float endAngle)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, EndAngle, endAngle);
    ACE_UPDATE_LAYOUT_PROPERTY(GaugeLayoutProperty, EndAngle, endAngle);
}

void GaugeModelNG::SetColors(const std::vector<Color>& colors, const std::vector<float>& values)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, Colors, colors);
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, Values, values);
}

void GaugeModelNG::SetGradientColors(
    const std::vector<ColorStopArray>& colors, const std::vector<float>& values, const GaugeType& type)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, GradientColors, colors);
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, Values, values);
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, GaugeType, type);
}

void GaugeModelNG::SetStrokeWidth(const Dimension& strokeWidth)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, StrokeWidth, strokeWidth);
    ACE_UPDATE_LAYOUT_PROPERTY(GaugeLayoutProperty, StrokeWidth, strokeWidth);
}

void GaugeModelNG::SetDescription(const RefPtr<AceType>& customNode)
{
    auto customDescriptionNode = AceType::DynamicCast<NG::UINode>(customNode);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto gaugePattern = frameNode->GetPattern<GaugePattern>();
    CHECK_NULL_VOID(gaugePattern);
    gaugePattern->SetDescriptionNode(customDescriptionNode);
}

void GaugeModelNG::SetIsShowLimitValue(bool isShowLimitValue)
{
    ACE_UPDATE_LAYOUT_PROPERTY(GaugeLayoutProperty, IsShowLimitValue, isShowLimitValue);
}

void GaugeModelNG::SetIsShowDescription(bool isShowDescription)
{
    ACE_UPDATE_LAYOUT_PROPERTY(GaugeLayoutProperty, IsShowDescription, isShowDescription);
}

void GaugeModelNG::SetLabelMarkedText(const std::string labelTextString) {}

void GaugeModelNG::SetMarkedTextColor(const Color& color) {}

void GaugeModelNG::SetShadowOptions(const GaugeShadowOptions& shadowOptions)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<GaugePattern>();
    CHECK_NULL_VOID(pattern);
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>();
    auto&& updateFunc = [shadowOptions, weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto frameNode = weak.Upgrade();
        if (!frameNode) {
            return;
        }
        GaugeShadowOptions shadowValue = shadowOptions;
        shadowValue.ReloadResources();
        ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, ShadowOptions, shadowValue, frameNode);
    };
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, ShadowOptions, shadowOptions);
    updateFunc(resObj);
    pattern->AddResObj("gauge.trackShadow", resObj, std::move(updateFunc));
}

void GaugeModelNG::SetIsShowIndicator(bool isShowIndicator)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, IsShowIndicator, isShowIndicator);
}

void GaugeModelNG::SetIndicatorIconPath(
    const std::string& iconPath, const std::string& bundleName, const std::string& moduleName)
{
    ACE_UPDATE_PAINT_PROPERTY(
        GaugePaintProperty, IndicatorIconSourceInfo, ImageSourceInfo(iconPath, bundleName, moduleName));
}

void GaugeModelNG::SetIndicatorSpace(const Dimension& space)
{
    ACE_UPDATE_PAINT_PROPERTY(GaugePaintProperty, IndicatorSpace, space);
}

void GaugeModelNG::ResetGradientColors()
{
    ACE_RESET_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, GradientColors, PROPERTY_UPDATE_RENDER);
    ACE_RESET_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, Values, PROPERTY_UPDATE_RENDER);
    ACE_RESET_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, GaugeType, PROPERTY_UPDATE_RENDER);
}

void GaugeModelNG::ResetShadowOptions()
{
    ACE_RESET_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, ShadowOptions, PROPERTY_UPDATE_RENDER);
}

void GaugeModelNG::ResetIndicatorIconPath()
{
    ACE_RESET_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, IndicatorIconSourceInfo, PROPERTY_UPDATE_RENDER);
}

void GaugeModelNG::ResetIndicatorSpace()
{
    ACE_RESET_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, IndicatorSpace, PROPERTY_UPDATE_RENDER);
}

void GaugeModelNG::SetValue(FrameNode* frameNode, float value)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, Value, value, frameNode);
}

void GaugeModelNG::SetStartAngle(FrameNode* frameNode, float value)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, StartAngle, value, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(GaugeLayoutProperty, StartAngle, value, frameNode);
}

void GaugeModelNG::SetEndAngle(FrameNode* frameNode, float value)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, EndAngle, value, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(GaugeLayoutProperty, EndAngle, value, frameNode);
}

void GaugeModelNG::SetGaugeStrokeWidth(FrameNode* frameNode, const Dimension& strokeWidth)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, StrokeWidth, strokeWidth, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(GaugeLayoutProperty, StrokeWidth, strokeWidth, frameNode);
}

void GaugeModelNG::SetShadowOptions(FrameNode* frameNode, const GaugeShadowOptions& shadowOptions)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, ShadowOptions, shadowOptions, frameNode);
}

void GaugeModelNG::ResetShadowOptions(FrameNode* frameNode)
{
    ACE_RESET_NODE_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, ShadowOptions, PROPERTY_UPDATE_RENDER, frameNode);
}

void GaugeModelNG::SetIsShowIndicator(FrameNode* frameNode, bool isShowIndicator)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, IsShowIndicator, isShowIndicator, frameNode);
}

void GaugeModelNG::SetIndicatorIconPath(
    FrameNode* frameNode, const std::string& iconPath, const std::string& bundleName, const std::string& moduleName)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(
        GaugePaintProperty, IndicatorIconSourceInfo, ImageSourceInfo(iconPath, bundleName, moduleName), frameNode);
}

void GaugeModelNG::ResetIndicatorIconPath(FrameNode* frameNode)
{
    ACE_RESET_NODE_PAINT_PROPERTY_WITH_FLAG(
        GaugePaintProperty, IndicatorIconSourceInfo, PROPERTY_UPDATE_RENDER, frameNode);
}

void GaugeModelNG::SetIndicatorSpace(FrameNode* frameNode, const Dimension& space)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, IndicatorSpace, space, frameNode);
}

void GaugeModelNG::ResetIndicatorSpace(FrameNode* frameNode)
{
    ACE_RESET_NODE_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, IndicatorSpace, PROPERTY_UPDATE_RENDER, frameNode);
}

void GaugeModelNG::SetColors(FrameNode* frameNode, const std::vector<Color>& colors, const std::vector<float>& values)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, Colors, colors, frameNode);
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, Values, values, frameNode);
}

void GaugeModelNG::SetGradientColors(FrameNode* frameNode, const std::vector<ColorStopArray>& colors,
    const std::vector<float>& values, const GaugeType& type)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, GradientColors, colors, frameNode);
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, Values, values, frameNode);
    ACE_UPDATE_NODE_PAINT_PROPERTY(GaugePaintProperty, GaugeType, type, frameNode);
}

void GaugeModelNG::ResetGradientColors(FrameNode* frameNode)
{
    ACE_RESET_NODE_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, GradientColors, PROPERTY_UPDATE_RENDER, frameNode);
    ACE_RESET_NODE_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, Values, PROPERTY_UPDATE_RENDER, frameNode);
    ACE_RESET_NODE_PAINT_PROPERTY_WITH_FLAG(GaugePaintProperty, GaugeType, PROPERTY_UPDATE_RENDER, frameNode);
}

void GaugeModelNG::SetBuilderFunc(FrameNode* frameNode, NG::GaugeMakeCallback&& makeFunc)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<GaugePattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetBuilderFunc(std::move(makeFunc));
}

void HandleStrokeWidthResource(const RefPtr<ResourceObject>& resObj, const RefPtr<GaugePattern>& pattern)
{
    auto&& updateFunc = [pattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        CalcDimension result;
        if (!ResourceParseUtils::ParseResDimensionVpNG(resObj, result) || result.Unit() == DimensionUnit::PERCENT) {
            result = CalcDimension(0);
        }
        pattern->UpdateStrokeWidth(result, isFirstLoad);
    };
    updateFunc(resObj, true);
    pattern->AddResObj("gauge.strokeWidth", resObj, std::move(updateFunc));
}

void HandleIndicatorIconResource(const RefPtr<ResourceObject>& resObj, const RefPtr<GaugePattern>& pattern)
{
    auto&& updateFunc = [pattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        std::string result;
        auto frameNode = pattern->GetHost();
        CHECK_NULL_VOID(frameNode);
        auto pattern = frameNode->GetPattern<GaugePattern>();
        CHECK_NULL_VOID(pattern);
        auto pipelineContext = frameNode->GetContext();
        CHECK_NULL_VOID(pipelineContext);
        if (ResourceParseUtils::ParseResMedia(resObj, result)) {
            auto imageCache = pipelineContext->GetImageCache();
            auto gaugePaintProperty = frameNode->GetPaintProperty<NG::GaugePaintProperty>();
            ImageSourceInfo sourceInfo = gaugePaintProperty->GetIndicatorIconSourceInfo().value_or(ImageSourceInfo(""));
            imageCache->ClearCacheImgObj(sourceInfo.GetKey());
            pattern->UpdateIndicatorIconPath(result, resObj->GetBundleName(), resObj->GetModuleName(), isFirstLoad);
        }
    };
    updateFunc(resObj, true);
    pattern->AddResObj("gauge.indicator.icon", resObj, std::move(updateFunc));
}

void HandleIndicatorSpaceResource(const RefPtr<ResourceObject>& resObj, const RefPtr<GaugePattern>& pattern)
{
    auto&& updateFunc = [pattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        CalcDimension result;
        if (!ResourceParseUtils::ParseResDimensionVpNG(resObj, result)) {
            result = NG::INDICATOR_DISTANCE_TO_TOP;
        }
        if (result.IsNegative()) {
            result = NG::INDICATOR_DISTANCE_TO_TOP;
        }
        pattern->UpdateIndicatorSpace(result, isFirstLoad);
    };
    updateFunc(resObj, true);
    pattern->AddResObj("gauge.indicator.space", resObj, std::move(updateFunc));
}

void GaugeModelNG::CreateWithResourceObj(GaugeResourceType jsResourceType, const RefPtr<ResourceObject>& resObj)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<GaugePattern>();
    CHECK_NULL_VOID(pattern);
    if (resObj) {
        switch (jsResourceType) {
            case GaugeResourceType::STROKE_WIDTH: {
                HandleStrokeWidthResource(resObj, pattern);
                break;
            }
            case GaugeResourceType::INDICATOR_ICON: {
                HandleIndicatorIconResource(resObj, pattern);
                break;
            }
            case GaugeResourceType::INDICATOR_SPACE: {
                HandleIndicatorSpaceResource(resObj, pattern);
                break;
            }
            default:
                break;
        }
    }
}

} // namespace OHOS::Ace::NG

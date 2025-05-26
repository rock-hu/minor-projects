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

#include "core/components_ng/pattern/data_panel/data_panel_model_ng.h"

#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/data_panel/data_panel_pattern.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t TYPE_CYCLE = 0;

void SetDefaultBorderRadius(void)
{
    if (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        return;
    }
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<DataPanelTheme>();
    CHECK_NULL_VOID(theme);
    auto defaultRadiusDimension = theme->GetDefaultBorderRadius();
    ViewAbstract::SetBorderRadius(defaultRadiusDimension);
    ViewAbstract::SetClipEdge(true);
}
} // namespace

void DataPanelModelNG::Create(const std::vector<double>& values, double max, int32_t dataPanelType)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::DATA_PANEL_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::DATA_PANEL_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<DataPanelPattern>(); });
    stack->Push(frameNode);

    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, Values, values);
    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, Max, max);
    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, DataPanelType, dataPanelType);

    if (dataPanelType != TYPE_CYCLE) {
        SetDefaultBorderRadius();
    }
}

void DataPanelModelNG::SetEffect(bool isCloseEffect)
{
    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, Effect, !isCloseEffect);
}

void DataPanelModelNG::SetValueColors(const std::vector<Gradient>& valueColors)
{
    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, ValueColors, valueColors);
}

void DataPanelModelNG::SetTrackBackground(const Color& trackBackgroundColor)
{
    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, TrackBackground, trackBackgroundColor);
}

void DataPanelModelNG::SetStrokeWidth(const Dimension& strokeWidth)
{
    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, StrokeWidth, strokeWidth);
}

void DataPanelModelNG::SetShadowOption(const DataPanelShadow& shadowOption)
{
    ACE_UPDATE_PAINT_PROPERTY(DataPanelPaintProperty, ShadowOption, shadowOption);
}

void DataPanelModelNG::SetCloseEffect(FrameNode* frameNode, bool isClose)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(DataPanelPaintProperty, Effect, !isClose, frameNode);
}

void DataPanelModelNG::SetTrackBackground(FrameNode* frameNode, const Color& trackBackgroundColor)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(DataPanelPaintProperty, TrackBackground, trackBackgroundColor, frameNode);
}

void DataPanelModelNG::SetStrokeWidth(FrameNode* frameNode, const Dimension& strokeWidth)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(DataPanelPaintProperty, StrokeWidth, strokeWidth, frameNode);
}

void DataPanelModelNG::SetShadowOption(FrameNode* frameNode, const DataPanelShadow& shadowOption)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(DataPanelPaintProperty, ShadowOption, shadowOption, frameNode);
}

void DataPanelModelNG::SetValueColors(FrameNode* frameNode, const std::vector<Gradient>& valueColors)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(DataPanelPaintProperty, ValueColors, valueColors, frameNode);
}

void DataPanelModelNG::SetBuilderFunc(FrameNode* frameNode, NG::DataPanelMakeCallback&& makeFunc)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<DataPanelPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetBuilderFunc(std::move(makeFunc));
}

void HandleTrackBackgroundColor(
    const RefPtr<ResourceObject>& resObj, const RefPtr<DataPanelPattern>& pattern, const std::string& key)
{
    auto&& updateFunc = [pattern, key](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        std::string color = pattern->GetResCacheMapByKey(key);
        Color result;
        if (color.empty()) {
            if (ResourceParseUtils::ParseResColor(resObj, result)) {
                pattern->AddResCache(key, result.ColorToString());
            } else {
                auto pipeline = PipelineBase::GetCurrentContext();
                CHECK_NULL_VOID(pipeline);
                auto theme = pipeline->GetTheme<DataPanelTheme>();
                CHECK_NULL_VOID(theme);
                result = theme->GetBackgroundColor();
            }
        } else {
            result = Color::ColorFromString(color);
        }
        pattern->UpdateTrackBackground(result, isFirstLoad);
    };
    updateFunc(resObj, true);
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void HandleStrokeWidth(
    const RefPtr<ResourceObject>& resObj, const RefPtr<DataPanelPattern>& pattern, const std::string& key)
{
    auto&& updateFunc = [pattern](const RefPtr<ResourceObject>& resObj, bool isFirstLoad = false) {
        CalcDimension result;
        if (ResourceParseUtils::ParseResDimensionVpNG(resObj, result)) {
            pattern->UpdateStrokeWidth(result, isFirstLoad);
        } else {
            auto pipeline = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto theme = pipeline->GetTheme<DataPanelTheme>();
            CHECK_NULL_VOID(theme);
            result = theme->GetThickness();
            pattern->UpdateStrokeWidth(result, isFirstLoad);
        }
    };
    updateFunc(resObj, true);
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void DataPanelModelNG::CreateWithResourceObj(DataPanelResourceType jsResourceType, const RefPtr<ResourceObject>& resObj)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<DataPanelPattern>();
    CHECK_NULL_VOID(pattern);
    if (resObj) {
        switch (jsResourceType) {
            case DataPanelResourceType::TRACK_BACKGROUND_COLOR: {
                HandleTrackBackgroundColor(resObj, pattern, "dataPanel.trackBackgroundColor");
                break;
            }
            case DataPanelResourceType::STROKE_WIDTH: {
                HandleStrokeWidth(resObj, pattern, "dataPanel.StrokeWidth");
                break;
            }
            default:
                break;
        }
    }
}

} // namespace OHOS::Ace::NG

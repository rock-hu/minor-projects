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
#include "core/components_ng/pattern/data_panel/data_panel_pattern.h"

namespace OHOS::Ace::NG {

void DataPanelPattern::OnAttachToFrameNode() {}

bool DataPanelPattern::OnDirtyLayoutWrapperSwap(
    const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool /*skipLayout*/)
{
    if (skipMeasure || dirty->SkipMeasureContent()) {
        return false;
    }
    return true;
}

RefPtr<NodePaintMethod> DataPanelPattern::CreateNodePaintMethod()
{
    if (!dataPanelModifier_) {
        dataPanelModifier_ = AceType::MakeRefPtr<DataPanelModifier>();
    }
    auto paintMethod = MakeRefPtr<DataPanelPaintMethod>(dataPanelModifier_);
    auto host = GetHost();
    CHECK_NULL_RETURN(host, paintMethod);
    auto paintProperty = host->GetPaintProperty<DataPanelPaintProperty>();
    CHECK_NULL_RETURN(paintProperty, paintMethod);
    auto layoutProperty = host->GetLayoutProperty();
    CHECK_NULL_RETURN(layoutProperty, paintMethod);
    auto layoutDirection = layoutProperty->GetNonAutoLayoutDirection();
    dataPanelModifier_->SetIsRtl(layoutDirection == TextDirection::RTL);
    auto geometryNode = host->GetGeometryNode();
    auto frameSize = geometryNode->GetFrameSize();
    DataPanelShadow shadowOption;
    if (paintProperty->HasShadowOption()) {
        shadowOption = paintProperty->GetShadowOptionValue();
    }
    float x = std::min<float>(shadowOption.offsetX - shadowOption.radius, boundsRect_.GetX());
    float y = std::min<float>(shadowOption.offsetY - shadowOption.radius, boundsRect_.GetY());
    float width = std::max<float>(
        { boundsRect_.Width(), boundsRect_.GetX() + boundsRect_.Width() - x,
        shadowOption.offsetX - x + frameSize.Width() + shadowOption.radius * 2.0f,
        std::abs(shadowOption.offsetX) + frameSize.Width() + shadowOption.radius * 2.0f }
    );
    float height = std::max<float>(
        { boundsRect_.Height(), boundsRect_.GetY() + boundsRect_.Height() - y,
        shadowOption.offsetY - y + frameSize.Height() + shadowOption.radius * 2.0f,
        std::abs(shadowOption.offsetY) + frameSize.Height() + shadowOption.radius * 2.0f }
    );
    boundsRect_.SetRect(x, y, width, height);
    dataPanelModifier_->SetBoundsRect(boundsRect_);
    dataPanelModifier_->SetUseContentModifier(UseContentModifier());
    return paintMethod;
}

void DataPanelPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    FireBuilder();
    auto host = GetHost();
    CHECK_NULL_VOID(host);

    auto layoutProperty = host->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    if (layoutProperty->GetPositionProperty()) {
        layoutProperty->UpdateAlignment(
            layoutProperty->GetPositionProperty()->GetAlignment().value_or(Alignment::CENTER));
    } else {
        layoutProperty->UpdateAlignment(Alignment::CENTER);
    }
}

void DataPanelPattern::FireBuilder()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (!makeFunc_.has_value()) {
        host->RemoveChildAndReturnIndex(contentModifierNode_);
        contentModifierNode_ = nullptr;
        host->AddChild(nullptr, 0);
        host->MarkNeedFrameFlushDirty(PROPERTY_UPDATE_MEASURE);
        return;
    }
    host->RemoveChildAtIndex(0);
    CHECK_NULL_VOID(makeFunc_);
    contentModifierNode_ = BuildContentModifierNode();
    CHECK_NULL_VOID(contentModifierNode_);
    host->AddChild(contentModifierNode_, 0);
    host->MarkNeedFrameFlushDirty(PROPERTY_UPDATE_MEASURE);
}

RefPtr<FrameNode> DataPanelPattern::BuildContentModifierNode()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    auto paintProperty = host->GetPaintProperty<DataPanelPaintProperty>();
    CHECK_NULL_RETURN(paintProperty, nullptr);
    auto geometryNode = host->GetGeometryNode();

    std::vector<double> tmpArry;
    if (paintProperty->GetValues().value().size() > 0) {
        for (size_t i = 0; i < paintProperty->GetValues().value().size(); i++) {
            tmpArry.push_back(paintProperty->GetValues().value()[i]);
        }
    } else {
        tmpArry.push_back(0.0f);
    }

    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_RETURN(eventHub, nullptr);
    auto enabled = eventHub->IsEnabled();
    double max = paintProperty->GetMax().value_or(DEFAULT_MAX_VALUE);
    DataPanelConfiguration config(tmpArry, max, enabled);
    return (makeFunc_.value())(config);
}
} // namespace OHOS::Ace::NG

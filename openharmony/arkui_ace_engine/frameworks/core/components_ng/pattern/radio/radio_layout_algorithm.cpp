/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/radio/radio_layout_algorithm.h"

#include "core/components_ng/pattern/radio/radio_pattern.h"
#include "core/components_ng/property/measure_utils.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {

namespace {
constexpr double NUM_TWO = 2.0;
}

std::optional<SizeF> RadioLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(host, std::nullopt);
    auto pattern = host->GetPattern<RadioPattern>();
    CHECK_NULL_RETURN(pattern, std::nullopt);
    if (pattern->UseContentModifier()) {
        if (host->GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_EIGHTEEN)) {
            host->GetGeometryNode()->ResetContent();
        } else {
            host->GetGeometryNode()->Reset();
        }
        return std::nullopt;
    }
    InitializeParam(host);
    auto layoutPolicy = GetLayoutPolicy(layoutWrapper);

    if (layoutPolicy.has_value() && layoutPolicy->IsMatch()) {
        realSize_ = LayoutPolicyIsMatchParent(contentConstraint, layoutPolicy, layoutWrapper);
        return realSize_;
    }

    // Case 1: Width and height are set in the front end.
    if (contentConstraint.selfIdealSize.IsValid() && contentConstraint.selfIdealSize.IsNonNegative()) {
        auto height = contentConstraint.selfIdealSize.Height().value();
        auto width = contentConstraint.selfIdealSize.Width().value();
        auto length = std::min(width, height);
        return SizeF(length, length);
    }
    // Case 2: The front end only sets either width or height
    // 2.1 Width is set to a value
    if (contentConstraint.selfIdealSize.Width().has_value() &&
        NonNegative(contentConstraint.selfIdealSize.Width().value())) {
        auto width = contentConstraint.selfIdealSize.Width().value();
        return SizeF(width, width);
    }
    // 2.2 Height is set to a value
    if (contentConstraint.selfIdealSize.Height().has_value() &&
        NonNegative(contentConstraint.selfIdealSize.Height().value())) {
        auto height = contentConstraint.selfIdealSize.Height().value();
        return SizeF(height, height);
    }
    // Case 3: Width and height are not set in the front end, so return from the theme
    auto width = defaultWidth_ - 2 * horizontalPadding_;
    auto height = defaultHeight_ - 2 * verticalPadding_;
    auto size = SizeF(width, height);
    size.Constrain(contentConstraint.minSize, contentConstraint.maxSize);
    auto padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    MinusPaddingToSize(padding, size);
    if (!NearEqual(size.Width(), size.Height())) {
        auto length = std::min(size.Width(), size.Height());
        size.SetWidth(length);
        size.SetHeight(length);
    }
    return size;
}

void RadioLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = host->GetPattern<RadioPattern>();
    CHECK_NULL_VOID(pattern);
    if (pattern->UseContentModifier()) {
        BoxLayoutAlgorithm::Measure(layoutWrapper);
        return;
    }
    auto childConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
    const auto& content = layoutWrapper->GetGeometryNode()->GetContent();
    if (content) {
        auto contentSize = content->GetRect().GetSize();
        childConstraint.maxSize.SetSizeT(contentSize);
        childConstraint.percentReference.SetSizeT(contentSize);
    }
    for (auto &&child : layoutWrapper->GetAllChildrenWithBuild()) {
        auto layoutPolicy = GetLayoutPolicy(layoutWrapper);
        if (layoutPolicy.has_value() && layoutPolicy->IsMatch() && realSize_) {
            childConstraint.selfIdealSize.SetWidth(realSize_->Width() * DEFAULT_RADIO_IMAGE_SCALE);
            childConstraint.selfIdealSize.SetHeight(realSize_->Height() * DEFAULT_RADIO_IMAGE_SCALE);
        }
        child->Measure(childConstraint);
    }
    PerformMeasureSelf(layoutWrapper);
}

void RadioLayoutAlgorithm::InitializeParam(const RefPtr<FrameNode>& host)
{
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto radioTheme = pipeline->GetTheme<RadioTheme>();
    CHECK_NULL_VOID(radioTheme);
    defaultWidth_ = radioTheme->GetWidth().ConvertToPx();
    defaultHeight_ = radioTheme->GetHeight().ConvertToPx();
    if (host->GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        horizontalPadding_ = radioTheme->GetDefaultPaddingSize().ConvertToPx();
        verticalPadding_ = radioTheme->GetDefaultPaddingSize().ConvertToPx();
    } else {
        horizontalPadding_ = radioTheme->GetHotZoneHorizontalPadding().ConvertToPx();
        verticalPadding_ = radioTheme->GetHotZoneVerticalPadding().ConvertToPx();
    }
}

void RadioLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    BoxLayoutAlgorithm::Layout(layoutWrapper);
    auto size = layoutWrapper->GetGeometryNode()->GetFrameSize();
    const auto& padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    MinusPaddingToSize(padding, size);
    auto left = padding.left.value_or(0);
    auto top = padding.top.value_or(0);
    auto paddingOffset = OffsetF(left, top);
    auto align = Alignment::CENTER;
    auto childWrapper = layoutWrapper->GetOrCreateChildByIndex(0);
    CHECK_NULL_VOID(childWrapper);
    SizeF childSize = childWrapper->GetGeometryNode()->GetMarginFrameSize();
    NG::OffsetF child_offset;
    child_offset.SetX((1.0 + align.GetHorizontal()) * (size.Width() - childSize.Width()) / NUM_TWO);
    child_offset.SetY((1.0 + align.GetVertical()) * (size.Height() - childSize.Height()) / NUM_TWO);
    auto translate = child_offset + paddingOffset;
    childWrapper->GetGeometryNode()->SetMarginFrameOffset(translate);
    childWrapper->Layout();
    const auto& content = layoutWrapper->GetGeometryNode()->GetContent();
    if (content) {
        NG::OffsetF offset;
        offset.SetX((1.0 + align.GetHorizontal()) * (size.Width() - content->GetRect().GetSize().Width()) / NUM_TWO);
        offset.SetY((1.0 + align.GetVertical()) * (size.Height() - content->GetRect().GetSize().Height()) / NUM_TWO);
        auto translate = offset + paddingOffset;
        content->SetOffset(translate);
    }
}

std::optional<SizeF> RadioLayoutAlgorithm::LayoutPolicyIsMatchParent(const LayoutConstraintF& contentConstraint,
    std::optional<NG::LayoutPolicyProperty> layoutPolicy, LayoutWrapper* layoutWrapper)
{
    auto height = contentConstraint.parentIdealSize.Height().value_or(0.0f);
    auto width = contentConstraint.parentIdealSize.Width().value_or(0.0f);
    auto selfHeight = contentConstraint.selfIdealSize.Height().value_or(0.0f);
    auto selfWidth = contentConstraint.selfIdealSize.Width().value_or(0.0f);
    if (layoutPolicy->IsAllMatch()) {
        auto length = std::min(width, height);
        return SizeF(length, length);
    } else if (layoutPolicy->IsWidthMatch()) {
        auto realSize = std::min(width, selfHeight);
        if (!contentConstraint.selfIdealSize.Height().has_value()) {
            realSize = width;
        }
        return SizeF(realSize, realSize);
    } else if (layoutPolicy->IsHeightMatch()) {
        auto realSize = std::min(height, selfWidth);
        if (!contentConstraint.selfIdealSize.Width().has_value()) {
            realSize = height;
        }
        return SizeF(realSize, realSize);
    }
    return SizeF();
}

std::optional<NG::LayoutPolicyProperty> RadioLayoutAlgorithm::GetLayoutPolicy(LayoutWrapper* layoutWrapper)
{
    auto layoutProperty = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_RETURN(layoutProperty, NG::LayoutPolicyProperty());
    auto layoutPolicy = layoutProperty->GetLayoutPolicyProperty();
    CHECK_NULL_RETURN(layoutPolicy, NG::LayoutPolicyProperty());
    return layoutPolicy;
}
} // namespace OHOS::Ace::NG

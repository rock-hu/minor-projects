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

#include "core/components_ng/pattern/menu/menu_item/menu_item_layout_algorithm.h"

#include "core/components_ng/pattern/menu/menu_item/menu_item_pattern.h"
#include "core/components_ng/pattern/security_component/security_component_layout_property.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {
constexpr Dimension ITEM_BOTTOM_TOP_PADDING = 8.0_vp;
constexpr int32_t PADDING_MULTIPLE = 2;
// The maximum width of the right row is 1/3 of content area width
constexpr float RIGHT_ROW_MAX_WIDTH_WEIGHT = 3;
void MenuItemLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto hostNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(hostNode);
    auto pipeline = hostNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);

    horInterval_ = static_cast<float>(theme->GetMenuItemHorIntervalPadding().ConvertToPx());
    middleSpace_ = static_cast<float>(theme->GetIconContentPadding().ConvertToPx());
    auto props = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(props);

    auto curLayoutConstraint = props->GetLayoutConstraint();
    CHECK_NULL_VOID(curLayoutConstraint);
    std::optional<LayoutConstraintF> layoutConstraint;
    auto layoutPolicyProperty = props->GetLayoutPolicyProperty();
    if (layoutPolicyProperty.has_value() && layoutPolicyProperty.value().IsFix()) {
        std::optional<LayoutConstraintF> newLayoutConstraint = curLayoutConstraint;
        RemoveParentRestrictionsForFixIdeal(props, newLayoutConstraint.value());
        layoutConstraint = newLayoutConstraint;
    } else {
        layoutConstraint = curLayoutConstraint;
    }
    if (isOption_ && !showDefaultSelectedIcon_) {
        MeasureOption(layoutWrapper, theme, props, layoutConstraint);
    } else {
        MeasureMenuItem(layoutWrapper, theme, props, layoutConstraint);
    }
}

void MenuItemLayoutAlgorithm::RemoveParentRestrictionsForFixIdeal(
    const RefPtr<LayoutProperty> layoutProperty, LayoutConstraintF& layoutConstraint)
{
    CHECK_NULL_VOID(layoutProperty);
    auto layoutPolicyProperty = layoutProperty->GetLayoutPolicyProperty();
    if (layoutPolicyProperty.has_value()) {
        auto& layoutPolicy = layoutPolicyProperty.value();
        if (layoutPolicy.IsWidthFix()) {
            layoutConstraint.maxSize.SetWidth(std::numeric_limits<float>::infinity());
        }
        if (layoutPolicy.IsHeightFix()) {
            layoutConstraint.maxSize.SetHeight(std::numeric_limits<float>::infinity());
        }
    }
}

void MenuItemLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto layoutProperty = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);

    if (isOption_ && !showDefaultSelectedIcon_) {
        LayoutOption(layoutWrapper, layoutProperty);
    } else {
        LayoutMenuItem(layoutWrapper, layoutProperty);
    }
}

void MenuItemLayoutAlgorithm::CheckNeedMatchParent(LayoutWrapper* layoutWrapper,
    std::optional<LayoutConstraintF>& layoutConstraint)
{
    auto menuNode = layoutWrapper->GetHostNode();
    auto menuItemPattern = menuNode ? menuNode->GetPattern<MenuItemPattern>() : nullptr;
    SubMenuExpandingMode expandingMode = menuItemPattern ? menuItemPattern->GetExpandingMode()
        : SubMenuExpandingMode::STACK;
    
    auto isSubMenu = menuItemPattern ? menuItemPattern->IsSubMenu() : false;
    auto isEmbedded = menuItemPattern ? menuItemPattern->IsEmbedded() : false;
    bool matchParent = (expandingMode == SubMenuExpandingMode::STACK && isSubMenu) ||
                       (expandingMode == SubMenuExpandingMode::EMBEDDED && isEmbedded);
    if (matchParent) {
        auto width = layoutConstraint->maxSize.Width();
        layoutConstraint->minSize.SetWidth(width);
    }
}

void MenuItemLayoutAlgorithm::CheckUserHeight(LayoutWrapper* layoutWrapper)
{
    auto props = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(props);
    const auto& calcConstraint = props->GetCalcLayoutConstraint();
    CHECK_NULL_VOID(calcConstraint);
    auto layoutConstraint = props->GetLayoutConstraint();
    CHECK_NULL_VOID(layoutConstraint);
    if (calcConstraint->selfIdealSize.has_value() &&
        calcConstraint->selfIdealSize.value().Height().has_value()) {
        ScaleProperty scaleProperty;
        if (layoutWrapper->GetGeometryNode() && layoutWrapper->GetGeometryNode()->GetParentLayoutConstraint()) {
            scaleProperty = layoutWrapper->GetGeometryNode()->GetParentLayoutConstraint()->scaleProperty;
        } else {
            scaleProperty = layoutConstraint->scaleProperty;
        }
        userHeight_ =
            ConvertToPx(calcConstraint->selfIdealSize.value().Height()->GetDimension(),
                scaleProperty, layoutConstraint->percentReference.Height()).value_or(0.0f);
    }
}

float MenuItemLayoutAlgorithm::CalcItemHeight(float leftRowHeight, float rightRowHeight)
{
    return GreatNotEqual(idealHeight_, 0.0f) ? idealHeight_
            : std::max(leftRowHeight, rightRowHeight) + padding_.Height();
}

std::pair<float, float> MenuItemLayoutAlgorithm::MeasureRightRow(LayoutWrapper* layoutWrapper,
    LayoutConstraintF& childConstraint)
{
    auto defaultPair = std::make_pair(0.0f, 0.0f);
    CHECK_NULL_RETURN(layoutWrapper, defaultPair);
    auto rightRow = layoutWrapper->GetOrCreateChildByIndex(1);
    CHECK_NULL_RETURN(rightRow, defaultPair);
    auto children = rightRow->GetAllChildrenWithBuild();
    CHECK_EQUAL_RETURN(children.empty(), true, defaultPair);
    rightRow->Measure(childConstraint);

    auto itemNode = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(itemNode, defaultPair);
    auto pipeline = itemNode->GetContext();
    CHECK_NULL_RETURN(pipeline, defaultPair);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_RETURN(theme, defaultPair);
    float iconContentPadding = static_cast<float>(theme->GetIconContentPadding().ConvertToPx());
    float spaceWidth = childConstraint.maxSize.Width();
    float rowWidth = 0.0f;
    float rowHeight = Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_TWELVE) ?
        theme->GetMenuChildMinHeight().ConvertToPx() : minItemHeight_;
    for (auto it = children.rbegin(); it != children.rend();++it) {
        const auto& child = *it;
        if (child != children.front()) {
            child->Measure(childConstraint);
        } else {
            auto labelConstraint = childConstraint;
            labelConstraint.maxSize.SetWidth(spaceWidth);
            child->Measure(labelConstraint);
        }
        auto childGeometryNode = child->GetGeometryNode();
        CHECK_NULL_RETURN(childGeometryNode, defaultPair);
        auto childSize = childGeometryNode->GetMarginFrameSize();
        spaceWidth -= childSize.Width() + iconContentPadding;
        rowWidth += childSize.Width() + iconContentPadding;
        rowHeight = std::max(rowHeight, childSize.Height());
    }
    rowWidth -= iconContentPadding;
    auto rightRowGeometryNode = rightRow->GetGeometryNode();
    CHECK_NULL_RETURN(rightRowGeometryNode, defaultPair);
    rightRowGeometryNode->SetFrameSize(SizeF(rowWidth, rowHeight));
    
    auto marginFrameSize = rightRowGeometryNode->GetMarginFrameSize();
    return {marginFrameSize.Width(), marginFrameSize.Height()};
}

void MenuItemLayoutAlgorithm::CalcContentExpandWidth(std::optional<LayoutConstraintF>& layoutConstraint,
    float contentWidth, float leftRowWidth, float rightRowWidth)
{
    needExpandContent_ = false;
    emptyWidth_ = 0.0f;
    if (contentWidth < minRowWidth_) {
        emptyWidth_ = minRowWidth_ - contentWidth;
        needExpandContent_ = true;
    }

    idealWidth_ = 0.0f;
    if (layoutConstraint->selfIdealSize.Width().has_value()) {
        idealWidth_ = std::max(layoutConstraint->minSize.Width(),
            std::min(layoutConstraint->maxSize.Width(), layoutConstraint->selfIdealSize.Width().value()));
        float newLeftRowWidth = idealWidth_ - rightRowWidth - middleSpace_;
        newLeftRowWidth -= padding_.Width();
        if (newLeftRowWidth > leftRowWidth) {
            emptyWidth_ = newLeftRowWidth - leftRowWidth;
            needExpandContent_ = true;
        }
    }
}

void MenuItemLayoutAlgorithm::MeasureItemViews(LayoutConstraintF& childConstraint,
    std::optional<LayoutConstraintF>& layoutConstraint, LayoutWrapper* layoutWrapper)
{
    auto leftRow = layoutWrapper->GetOrCreateChildByIndex(0);
    CHECK_NULL_VOID(leftRow);

    // measure right row
    childConstraint.maxSize.SetWidth((maxRowWidth_ - middleSpace_) / RIGHT_ROW_MAX_WIDTH_WEIGHT);
    auto [rightRowWidth, rightRowHeight] = MeasureRightRow(layoutWrapper, childConstraint);
    
    // measure left row
    auto maxWidth = maxRowWidth_ - rightRowWidth - middleSpace_;
    childConstraint.maxSize.SetWidth(maxWidth);
    MeasureRow(layoutWrapper, leftRow, childConstraint);
    float leftRowWidth = leftRow->GetGeometryNode()->GetMarginFrameSize().Width();
    float leftRowHeight = leftRow->GetGeometryNode()->GetMarginFrameSize().Height();
    float contentWidth = 0.0f;
    contentWidth = leftRowWidth + rightRowWidth + padding_.Width() + middleSpace_;

    CalcContentExpandWidth(layoutConstraint, contentWidth, leftRowWidth, rightRowWidth);

    auto width = std::max(minRowWidth_, contentWidth);
    auto actualWidth = GreatNotEqual(idealWidth_, 0.0f) ? idealWidth_ : width;
    childConstraint.minSize.SetWidth(actualWidth - padding_.Width());
    childConstraint.maxSize.SetWidth(actualWidth - padding_.Width());
    auto expandableHeight = MeasureExpandableHeight(childConstraint, layoutWrapper);

    float itemHeight = CalcItemHeight(leftRowHeight, rightRowHeight);
    UpdateSelfSize(layoutWrapper, actualWidth, itemHeight, expandableHeight);
}

float MenuItemLayoutAlgorithm::MeasureExpandableHeight(LayoutConstraintF& childConstraint, LayoutWrapper* layoutWrapper)
{
    auto expandableHeight = 0.0f;
    auto expandableArea = layoutWrapper->GetOrCreateChildByIndex(EXPANDABLE_AREA_VIEW_INDEX);
    if (expandableArea) {
        expandableArea->Measure(childConstraint);
        auto expandableAreaGeometryNode = expandableArea->GetGeometryNode();
        CHECK_NULL_RETURN(expandableAreaGeometryNode, expandableHeight);
        expandableHeight = std::max(expandableAreaGeometryNode->GetMarginFrameSize().Height(), 0.0f);
    }
    return expandableHeight;
}

void MenuItemLayoutAlgorithm::MeasureRow(LayoutWrapper* layoutWrapper, const RefPtr<LayoutWrapper>& row,
    const LayoutConstraintF& constraint)
{
    auto itemNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(itemNode);
    auto pipeline = itemNode->GetContext();
    CHECK_NULL_VOID(pipeline);

    auto children = row->GetAllChildrenWithBuild();
    CHECK_EQUAL_VOID(isOption_ && !showDefaultSelectedIcon_ && children.empty(), true);
    
    float spaceWidth = constraint.maxSize.Width();
    float rowWidth = 0.0f;
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    float rowHeight = 0.0f;
    rowHeight = isOption_ && !showDefaultSelectedIcon_ ? 0.0f : (
        Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_TWELVE) ?
            theme->GetMenuChildMinHeight().ConvertToPx() : minItemHeight_
    );
    float iconContentPadding = 0.0f;
    if (!isOption_) {
        iconContentPadding = static_cast<float>(theme->GetIconContentPadding().ConvertToPx());
    }

    for (const auto& child : children) {
        if (child != children.back()) {
            // not content node
            child->Measure(constraint);
        } else {
            // content node update constraint max width
            auto contentConstraint = constraint;
            contentConstraint.maxSize.SetWidth(spaceWidth);
            child->Measure(contentConstraint);
        }
        auto childSize = child->GetGeometryNode()->GetMarginFrameSize();
        spaceWidth -= childSize.Width() + iconContentPadding;
        rowWidth += childSize.Width() + iconContentPadding;
        rowHeight = std::max(rowHeight, childSize.Height());
    }
    if (!isOption_ && GreatNotEqual(rowWidth, iconContentPadding)) {
        rowWidth -= iconContentPadding;
    }
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_FOURTEEN) && isOption_ &&
        !showDefaultSelectedIcon_) {
        rowHeight += (ITEM_BOTTOM_TOP_PADDING * PADDING_MULTIPLE).ConvertToPx();
    }
    row->GetGeometryNode()->SetFrameSize(SizeF(rowWidth, rowHeight));
}

void MenuItemLayoutAlgorithm::CheckNeedExpandContent(LayoutWrapper* layoutWrapper,
    LayoutConstraintF& childConstraint)
{
    if (needExpandContent_) {
        auto menuItemNode = layoutWrapper->GetHostNode();
        CHECK_NULL_VOID(menuItemNode);
        auto pattern = menuItemNode->GetPattern<MenuItemPattern>();
        CHECK_NULL_VOID(pattern);
        auto contentNode = pattern->GetContentNode();
        CHECK_NULL_VOID(contentNode);

        auto leftRow = layoutWrapper->GetChildByIndex(0);
        CHECK_NULL_VOID(leftRow);
        auto newRowSize = leftRow->GetGeometryNode()->GetFrameSize();
        newRowSize.SetWidth(emptyWidth_ + newRowSize.Width());
        leftRow->GetGeometryNode()->SetFrameSize(newRowSize);

        auto oldTextSize = contentNode->GetGeometryNode()->GetFrameSize();
        float newTextWidth = emptyWidth_ + oldTextSize.Width();
        childConstraint.minSize.SetWidth(newTextWidth);
        childConstraint.maxSize.SetWidth(newTextWidth);
        contentNode->Measure(childConstraint);
    }
}

void MenuItemLayoutAlgorithm::UpdateSelfSize(LayoutWrapper* layoutWrapper,
    float width, float itemHeight, float expandableHeight)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto menuNode = layoutWrapper->GetHostNode();
    auto menuItemPattern = menuNode ? menuNode->GetPattern<MenuItemPattern>() : nullptr;
    auto isEmbedded = menuItemPattern ? menuItemPattern->IsEmbedded() : false;
    auto expandingMode = menuItemPattern ? menuItemPattern->GetExpandingMode() : SubMenuExpandingMode::SIDE;
    float menuItemHeight = itemHeight;
    if (expandingMode == SubMenuExpandingMode::EMBEDDED && !isEmbedded && GreatNotEqual(menuItemHeight, 0.0f)) {
        auto props = layoutWrapper->GetLayoutProperty();
        CHECK_NULL_VOID(props);
        const auto& calcConstraint = props->GetCalcLayoutConstraint();
        if (calcConstraint) {
            auto minWidth = CalcLength(minRowWidth_);
            auto height = CalcLength(menuItemHeight + expandableHeight);
            auto maxWidth = calcConstraint->maxSize->Width();
            calcConstraint->UpdateMaxSizeWithCheck(CalcSize(maxWidth, height));
            calcConstraint->UpdateMinSizeWithCheck(CalcSize(minWidth, height));
        }
    }

    auto bordersHeight = 0.0f;
    itemHeight += GetDividerStroke(layoutWrapper);
    auto clickableArea = layoutWrapper->GetOrCreateChildByIndex(CLICKABLE_AREA_VIEW_INDEX);
    if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        float height = CalcSelfHeight(itemHeight, bordersHeight);
        layoutWrapper->GetGeometryNode()->SetContentSize(SizeF(width, height + expandableHeight));
        if (clickableArea) {
            clickableArea->GetGeometryNode()->SetFrameSize(SizeF(width, height + bordersHeight));
        }
    } else {
        layoutWrapper->GetGeometryNode()->SetContentSize(SizeF(width, itemHeight));
    }
    BoxLayoutAlgorithm::PerformMeasureSelf(layoutWrapper);
    auto geometryNode = layoutWrapper->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    TAG_LOGD(AceLogTag::ACE_MENU, "MenuItem measure, itemContentSizeH:%{public}f, item marginFrameSizeH:%{public}f",
        geometryNode->GetContentSize().Height(), geometryNode->GetMarginFrameSize().Height());
}

float MenuItemLayoutAlgorithm::CalcSelfHeight(float itemHeight, float bordersHeight)
{
    return std::max(itemHeight - bordersHeight, minItemHeight_);
}

float MenuItemLayoutAlgorithm::GetDividerStroke(LayoutWrapper* layoutWrapper)
{
    auto menuItemNode = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(menuItemNode, 0.0f);
    auto pattern = menuItemNode->GetPattern<MenuItemPattern>();
    CHECK_NULL_RETURN(pattern, 0.0f);
    auto topDivider = pattern->GetTopDivider();
    if (topDivider && topDivider->GetParent()) {
        return 0.0f;
    }
    return pattern->GetDividerStroke();
}

float MenuItemLayoutAlgorithm::GetBordersHeight(LayoutWrapper* layoutWrapper)
{
    auto props = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_RETURN(props, 0.0f);
    const auto& border = props->GetBorderWidthProperty();
    CHECK_NULL_RETURN(border, 0.0f);
    return border->topDimen.value_or(Dimension(0.0)).ConvertToPx() +
        border->bottomDimen.value_or(Dimension(0.0)).ConvertToPx();
}

float MenuItemLayoutAlgorithm::GetMenuItemVerticalPadding(LayoutWrapper* layoutWrapper)
{
    float ret = 0.0f;
    CHECK_NULL_RETURN(layoutWrapper, ret);
    auto hostNode = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(hostNode, ret);
    auto pipeline = hostNode->GetContext();
    CHECK_NULL_RETURN(pipeline, ret);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_RETURN(theme, ret);
    return theme->GetMenuItemVerticalPadding().ConvertToPx();
}

std::optional<float> MenuItemLayoutAlgorithm::GetIdealWidth(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_RETURN(layoutWrapper, std::nullopt);
    // layout property not update in layoutWrapper when measure
    auto optionProps = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_RETURN(optionProps, std::nullopt);

    const auto& layoutConstraint = optionProps->GetCalcLayoutConstraint();
    CHECK_NULL_RETURN(layoutConstraint, std::nullopt);

    const auto& minSize = layoutConstraint->minSize;
    CHECK_NULL_RETURN(minSize, std::nullopt);

    const auto& width = minSize->Width();
    if (width->IsValid()) {
        return width->GetDimension().ConvertToPx();
    }

    return std::nullopt;
}

void MenuItemLayoutAlgorithm::UpdateIconMargin(LayoutWrapper* layoutWrapper)
{
    auto optionNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(optionNode);
    auto optionPattern = optionNode->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(optionPattern);
    auto layoutProps = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProps);
    auto direction = layoutProps->GetNonAutoLayoutDirection();
    bool isRtl = direction == TextDirection::RTL;
    const auto& selectTheme = optionPattern->GetSelectTheme();
    CHECK_NULL_VOID(selectTheme);
    auto calcLength = CalcLength(selectTheme->GetIconContentPadding());
    MarginProperty margin;
    if (isRtl) {
        margin.left = calcLength;
        margin.right = CalcLength();
    } else {
        margin.left = CalcLength();
        margin.right = calcLength;
    }
    Alignment align = isRtl ? Alignment::CENTER_RIGHT : Alignment::CENTER_LEFT;
    auto child = layoutWrapper->GetOrCreateChildByIndex(0);
    for (auto iconChild : child->GetAllChildrenWithBuild()) {
        if ((iconChild->GetHostTag() == V2::IMAGE_ETS_TAG) || (iconChild->GetHostTag() == V2::SYMBOL_ETS_TAG)) {
            auto iconProps = iconChild->GetLayoutProperty();
            iconProps->UpdateAlignment(align);
            iconProps->UpdateMargin(margin);
        }
    }
}

void MenuItemLayoutAlgorithm::InitPadding(const RefPtr<LayoutProperty>& props,
    std::optional<LayoutConstraintF>& layoutConstraint)
{
    padding_ = props->CreatePaddingAndBorderWithDefault(horInterval_, verInterval_, 0.0f, 0.0f);
}

void MenuItemLayoutAlgorithm::UpdateIdealSize(LayoutWrapper* layoutWrapper, const RefPtr<LayoutProperty>& props,
    std::optional<LayoutConstraintF>& layoutConstraint)
{
    const auto& calcConstraint = props->GetCalcLayoutConstraint();
    if (calcConstraint && calcConstraint->selfIdealSize.has_value() &&
        calcConstraint->selfIdealSize.value().Width().has_value()) {
        ScaleProperty scaleProperty;
        if (layoutWrapper->GetGeometryNode() && layoutWrapper->GetGeometryNode()->GetParentLayoutConstraint()) {
            scaleProperty = layoutWrapper->GetGeometryNode()->GetParentLayoutConstraint()->scaleProperty;
        } else {
            scaleProperty = layoutConstraint->scaleProperty;
        }
        layoutConstraint->selfIdealSize.SetWidth(
            ConvertToPx(calcConstraint->selfIdealSize.value().Width()->GetDimension(), scaleProperty,
                layoutConstraint->percentReference.Width()));
    }
    if (calcConstraint && calcConstraint->selfIdealSize.has_value() &&
        calcConstraint->selfIdealSize.value().Height().has_value()) {
        idealHeight_ = calcConstraint->selfIdealSize.value().Height()->GetDimension().ConvertToPx();
    }
}

void MenuItemLayoutAlgorithm::MeasureMenuItem(LayoutWrapper* layoutWrapper, const RefPtr<SelectTheme>& selectTheme,
    const RefPtr<LayoutProperty>& props, std::optional<LayoutConstraintF>& layoutConstraint)
{
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        verInterval_ = GetMenuItemVerticalPadding(layoutWrapper) - GetBordersHeight(layoutWrapper);
    }
    InitPadding(props, layoutConstraint);
    CHECK_NULL_VOID(layoutWrapper);
    auto menuItemNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(menuItemNode);
    auto pattern = menuItemNode->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(pattern);
    if (isOption_ && showDefaultSelectedIcon_) {
        if (pattern->IsSelectOption() && pattern->GetHasOptionWidth()) {
            auto selectOptionWidth = pattern->GetSelectOptionWidth();
            layoutConstraint->minSize.SetWidth(selectOptionWidth);
            layoutConstraint->maxSize.SetWidth(selectOptionWidth);
            layoutConstraint->selfIdealSize.SetWidth(selectOptionWidth);
        }
        UpdateIconMargin(layoutWrapper);
    }
    maxRowWidth_ = layoutConstraint->maxSize.Width() - padding_.Width();
    UpdateIdealSize(layoutWrapper, props, layoutConstraint);
    if (layoutConstraint->selfIdealSize.Width().has_value()) {
        maxRowWidth_ =
            std::max(layoutConstraint->minSize.Width(),
                std::min(layoutConstraint->maxSize.Width(), layoutConstraint->selfIdealSize.Width().value())) -
            padding_.Width();
    }
    CheckNeedMatchParent(layoutWrapper, layoutConstraint);
    minRowWidth_ = layoutConstraint->minSize.Width();

    auto childConstraint = props->CreateChildConstraint();
    minItemHeight_ = static_cast<float>(selectTheme->GetOptionMinHeight().ConvertToPx());
    childConstraint.minSize.SetHeight(Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_TWELVE) ?
        selectTheme->GetMenuChildMinHeight().ConvertToPx() : minItemHeight_);
    
    iconSize_ = selectTheme->GetIconSideLength().ConvertToPx();
    MeasureItemViews(childConstraint, layoutConstraint, layoutWrapper);
    MeasureClickableArea(layoutWrapper);
    CheckNeedExpandContent(layoutWrapper, childConstraint);
}

void MenuItemLayoutAlgorithm::MeasureClickableArea(LayoutWrapper* layoutWrapper)
{
    auto clickableArea = layoutWrapper->GetOrCreateChildByIndex(CLICKABLE_AREA_VIEW_INDEX);
    if (GreatNotEqual(idealWidth_, 0.0f)) {
        layoutWrapper->GetGeometryNode()->SetFrameWidth(idealWidth_);
        if (clickableArea) {
            clickableArea->GetGeometryNode()->SetFrameWidth(idealWidth_);
        }
    }
}

void MenuItemLayoutAlgorithm::MeasureOption(LayoutWrapper* layoutWrapper, const RefPtr<SelectTheme>& selectTheme,
    const RefPtr<LayoutProperty>& props, const std::optional<LayoutConstraintF>& layoutConstraint)
{
    auto optionNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(optionNode);
    auto optionPattern = optionNode->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(optionPattern);

    auto idealSize = CreateIdealSize(
        layoutConstraint.value(), Axis::HORIZONTAL, props->GetMeasureType(MeasureType::MATCH_CONTENT), true);
    float maxChildWidth = layoutConstraint->maxSize.Width() - horInterval_ * 2.0f;
    // measure child
    auto childConstraint = props->CreateChildConstraint();
    childConstraint.maxSize.SetWidth(maxChildWidth);
    // set self size based on childNode size;
    auto minOptionHeight = static_cast<float>(selectTheme->GetOptionMinHeight().ConvertToPx());
    auto child = layoutWrapper->GetOrCreateChildByIndex(0);
    CHECK_NULL_VOID(child);

    auto rowChild = child->GetOrCreateChildByIndex(0);
    if (rowChild && (rowChild->GetHostTag() == V2::PASTE_BUTTON_ETS_TAG)) {
        auto securityLayoutProperty = DynamicCast<SecurityComponentLayoutProperty>(rowChild->GetLayoutProperty());
        CHECK_NULL_VOID(securityLayoutProperty);
        securityLayoutProperty->UpdateBackgroundLeftPadding(Dimension(horInterval_));
    }
    UpdateIconMargin(layoutWrapper);
    MeasureRow(layoutWrapper, child, childConstraint);
    auto childSize = child->GetGeometryNode()->GetMarginFrameSize();
    childSize.AddWidth(horInterval_ * 2.0f);
    idealSize.UpdateSizeWithCheck(childSize);
    auto idealWidth = GetIdealWidth(layoutWrapper);
    if (idealWidth.has_value()) {
        auto optionPaintProperty = optionNode->GetPaintProperty<MenuItemPaintProperty>();
        if (optionPaintProperty && (optionPaintProperty->GetIdealWidthForWeb() > 0) &&
            (idealWidth.value() < optionPaintProperty->GetIdealWidthForWeb())) {
            idealSize.SetWidth(std::min(optionPaintProperty->GetIdealWidthForWeb(), layoutConstraint->maxSize.Width()));
        } else {
            idealSize.SetWidth(idealWidth.value());
        }
    }
    idealSize.SetHeight(std::max(minOptionHeight, idealSize.Height()));
    if (optionPattern->IsSelectOption() && optionPattern->GetHasOptionWidth()) {
        idealSize.SetWidth(optionPattern->GetSelectOptionWidth());
    }
    if (rowChild && (rowChild->GetHostTag() == V2::PASTE_BUTTON_ETS_TAG)) {
        float dividerWidth = static_cast<float>(selectTheme->GetDefaultDividerWidth().ConvertToPx());
        SizeF idealSizePaste(idealSize.Width() - dividerWidth, idealSize.Height() - dividerWidth);
        childConstraint.selfIdealSize.SetSize(idealSizePaste);
        rowChild->Measure(childConstraint);
    }
    layoutWrapper->GetGeometryNode()->SetFrameSize(idealSize);
    auto textAlign = static_cast<TextAlign>(selectTheme->GetOptionContentNormalAlign());
    if (textAlign == TextAlign::CENTER) {
        ExtendTextAndRowNode(child, idealSize, horInterval_, childConstraint);
    }
}

float MenuItemLayoutAlgorithm::CalcRowTopSpace(float rowsHeight, float itemHeight,
    LayoutWrapper* layoutWrapper, float leftOrRightRowHeight)
{
    return (itemHeight - leftOrRightRowHeight + GetBordersHeight(layoutWrapper)) / 2.0f;
}

void MenuItemLayoutAlgorithm::LayoutMenuItem(LayoutWrapper* layoutWrapper, const RefPtr<LayoutProperty>& props)
{
    auto layoutDirection = props->GetNonAutoLayoutDirection();
    auto leftRow = layoutWrapper->GetOrCreateChildByIndex(0);
    auto leftRowSize = leftRow ? leftRow->GetGeometryNode()->GetFrameSize() : SizeT(0.0f, 0.0f);
    auto rightRow = layoutWrapper->GetOrCreateChildByIndex(1);
    auto rightRowSize = rightRow ? rightRow->GetGeometryNode()->GetFrameSize() : SizeT(0.0f, 0.0f);
    float rowsHeight = 0.0f;
    float itemHeight = idealHeight_ > 0.0f ? idealHeight_ :
            std::max(leftRowSize.Height(), rightRowSize.Height()) + padding_.Height();

    CHECK_NULL_VOID(leftRow);
    float topSpace = CalcRowTopSpace(rowsHeight, itemHeight, layoutWrapper, leftRowSize.Height());
    leftRow->GetLayoutProperty()->UpdatePropertyChangeFlag(PROPERTY_UPDATE_LAYOUT);
    leftRow->GetGeometryNode()->SetMarginFrameOffset(OffsetF(padding_.left.value_or(horInterval_), topSpace));
    if (layoutDirection == TextDirection::RTL) {
        leftRow->GetGeometryNode()->SetMarginFrameOffset(
            OffsetF(layoutWrapper->GetGeometryNode()->GetFrameSize().Width() - padding_.right.value_or(horInterval_) -
                    leftRow->GetGeometryNode()->GetFrameSize().Width(),
            topSpace));
    }
    leftRow->Layout();

    CHECK_NULL_VOID(rightRow);
    topSpace = CalcRowTopSpace(rowsHeight, itemHeight, layoutWrapper, rightRowSize.Height());
    rightRow->GetGeometryNode()->SetMarginFrameOffset(
        OffsetF(layoutWrapper->GetGeometryNode()->GetFrameSize().Width() - padding_.right.value_or(horInterval_) -
            rightRow->GetGeometryNode()->GetFrameSize().Width(), topSpace));
    if (layoutDirection == TextDirection::RTL) {
        rightRow->GetGeometryNode()->SetMarginFrameOffset(OffsetF(padding_.left.value_or(horInterval_), topSpace));
    }
    rightRow->Layout();

    auto clickableArea = layoutWrapper->GetOrCreateChildByIndex(CLICKABLE_AREA_VIEW_INDEX);
    CHECK_NULL_VOID(clickableArea);
    clickableArea->Layout();

    auto expandableArea = layoutWrapper->GetOrCreateChildByIndex(EXPANDABLE_AREA_VIEW_INDEX);
    CHECK_NULL_VOID(expandableArea);
    expandableArea->GetLayoutProperty()->UpdatePropertyChangeFlag(PROPERTY_UPDATE_LAYOUT);
    expandableArea->GetGeometryNode()->SetMarginFrameOffset(
        OffsetF(padding_.left.value_or(horInterval_),
        itemHeight));
    expandableArea->Layout();
}

void MenuItemLayoutAlgorithm::LayoutOption(LayoutWrapper* layoutWrapper, const RefPtr<LayoutProperty>& props)
{
    auto optionNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(optionNode);
    auto optionPattern = optionNode->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(optionPattern);
    auto optionSize = layoutWrapper->GetGeometryNode()->GetFrameSize();
    auto optionHeight = optionSize.Height();
    auto child = layoutWrapper->GetOrCreateChildByIndex(0);
    child->GetLayoutProperty()->UpdatePropertyChangeFlag(PROPERTY_UPDATE_LAYOUT);
    auto rowChild = child->GetOrCreateChildByIndex(0);
    if (rowChild && (rowChild->GetHostTag() == V2::PASTE_BUTTON_ETS_TAG)) {
        float horInterval = 0.0f;
        if (props->GetNonAutoLayoutDirection() == TextDirection::RTL) {
            SizeF childSize = child->GetGeometryNode()->GetMarginFrameSize();
            horInterval = optionSize.Width() - childSize.Width();
        }
        child->GetGeometryNode()->SetMarginFrameOffset(
            OffsetF(horInterval, (optionHeight - child->GetGeometryNode()->GetFrameSize().Height()) / 2.0f));
        child->Layout();
        return;
    }
    const auto& selectTheme = optionPattern->GetSelectTheme();
    CHECK_NULL_VOID(selectTheme);

    float horInterval = horInterval_;
    auto textAlign = static_cast<TextAlign>(selectTheme->GetOptionContentNormalAlign());
    if (textAlign != TextAlign::CENTER && props->GetNonAutoLayoutDirection() == TextDirection::RTL) {
        SizeF childSize = child->GetGeometryNode()->GetMarginFrameSize();
        horInterval = optionSize.Width() - childSize.Width() - horInterval_;
    }
    child->GetGeometryNode()->SetMarginFrameOffset(
        OffsetF(horInterval, (optionHeight - child->GetGeometryNode()->GetFrameSize().Height()) / 2.0f));
    child->Layout();
}

void MenuItemLayoutAlgorithm::ExtendTextAndRowNode(const RefPtr<LayoutWrapper>& row,
    const SizeF& optSize, float interval, const LayoutConstraintF& constraint)
{
    CHECK_NULL_VOID(row);
    auto children = row->GetAllChildrenWithBuild();
    if (children.empty()) {
        return;
    }
    SizeF frameSize;
    float imageNodeWidth = 0.0f;
    for (auto textChild : children) {
        if (!textChild) {
            continue;
        }
        auto geometryNode = textChild->GetGeometryNode();
        if (!geometryNode) {
            continue;
        }
        if (textChild->GetHostTag() != V2::TEXT_ETS_TAG) {
            imageNodeWidth += geometryNode->GetMarginFrameSize().Width();
            continue;
        }
        frameSize = geometryNode->GetMarginFrameSize();
        auto width = optSize.Width() - 2.0f * interval - imageNodeWidth;
        auto contentConstraint = constraint;
        contentConstraint.minSize.SetWidth(width);
        textChild->Measure(contentConstraint);
    }
    auto geometryNode = row->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    frameSize = geometryNode->GetMarginFrameSize();
    geometryNode->SetFrameSize(SizeF(optSize.Width() - 2.0f * interval, frameSize.Height()));
}
} // namespace OHOS::Ace::NG

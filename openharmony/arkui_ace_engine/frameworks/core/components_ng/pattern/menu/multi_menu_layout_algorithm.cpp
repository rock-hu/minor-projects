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

#include "core/components_ng/pattern/menu/multi_menu_layout_algorithm.h"

#include "core/components/common/layout/grid_system_manager.h"
#include "core/components_ng/pattern/menu/menu_theme.h"
#include "core/components_ng/pattern/menu/menu_pattern.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_layout_property.h"

namespace OHOS::Ace::NG {
bool UpdateColumnWidth(const RefPtr<FrameNode>& frameNode, const RefPtr<GridColumnInfo>& columnInfo)
{
    CHECK_NULL_RETURN(frameNode && columnInfo, false);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_RETURN(pipeline, false);
    if (Positive(pipeline->GetRootWidth())) {
        return false;
    }

    auto currentId = Container::CurrentId();
    CHECK_NULL_RETURN(currentId >= MIN_SUBCONTAINER_ID, false);
    auto subwindowManager = SubwindowManager::GetInstance();
    CHECK_NULL_RETURN(subwindowManager, false);
    auto parentContainerId = subwindowManager->GetParentContainerId(currentId);
    auto subwindow = subwindowManager->GetSubwindow(parentContainerId);
    CHECK_NULL_RETURN(subwindow, false);
    auto width = subwindow->GetRect().Width();
    CHECK_NULL_RETURN(Positive(width), false);
    auto parent = columnInfo->GetParent();
    CHECK_NULL_RETURN(parent, false);
    parent->BuildColumnWidth(width);
    return true;
}

void MultiMenuLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto layoutProperty = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    auto layoutConstraint = layoutProperty->GetLayoutConstraint();
    CHECK_NULL_VOID(layoutConstraint);
    auto childConstraint = layoutProperty->CreateChildConstraint();
    childConstraint.maxSize.SetWidth(layoutConstraint->maxSize.Width());
    // constraint max size minus padding
    const auto& padding = layoutProperty->CreatePaddingAndBorder();
    auto node = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(node);
    auto pattern = node->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(pattern);
    if (!pattern->IsEmbedded()) {
        MinusPaddingToSize(padding, childConstraint.maxSize);
    } else {
        UpdateEmbeddedPercentReference(layoutWrapper, childConstraint, layoutConstraint);
    }
    if (layoutConstraint->selfIdealSize.Width().has_value()) {
        // when Menu is set self ideal width, make children node adaptively fill up.
        if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
            if (LessNotEqual(layoutConstraint->selfIdealSize.Width().value(), MIN_MENU_WIDTH.ConvertToPx())) {
                RefPtr<GridColumnInfo> columnInfo;
                columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::MENU);
                columnInfo->GetParent()->BuildColumnWidth();
                auto minWidth = static_cast<float>(columnInfo->GetWidth(MENU_MIN_GRID_COUNTS));
                layoutConstraint->selfIdealSize.SetWidth(minWidth);

                UpdateMenuDefaultConstraintByDevice(pattern, childConstraint, padding.Width(), layoutConstraint, true);
            }
        }
        auto idealWidth =
            std::max(layoutConstraint->minSize.Width(),
                std::min(layoutConstraint->maxSize.Width(), layoutConstraint->selfIdealSize.Width().value())) -
            padding.Width();
        childConstraint.selfIdealSize.SetWidth(idealWidth);
    } else {
        // constraint min width base on grid column
        auto columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::MENU);
        CHECK_NULL_VOID(columnInfo);
        CHECK_NULL_VOID(columnInfo->GetParent());
        if (!UpdateColumnWidth(node, columnInfo)) {
            columnInfo->GetParent()->BuildColumnWidth();
        }
        auto minWidth = static_cast<float>(columnInfo->GetWidth()) - padding.Width();
        childConstraint.minSize.SetWidth(minWidth);

        UpdateMenuDefaultConstraintByDevice(pattern, childConstraint, padding.Width(), layoutConstraint, false);
    }
    // Calculate max width of menu items
    UpdateConstraintBaseOnMenuItems(layoutWrapper, childConstraint);
    UpdateSelfSize(layoutWrapper, childConstraint, layoutConstraint);
}

void MultiMenuLayoutAlgorithm::UpdateMenuDefaultConstraintByDevice(const RefPtr<MenuPattern>& pattern,
    LayoutConstraintF& childConstraint, float paddingWidth, std::optional<LayoutConstraintF>& layoutConstraint,
    bool idealSizeHasVal)
{
    CHECK_NULL_VOID(pattern);

    // only 2in1 device has restrictions on the menu width in API13
    CHECK_NULL_VOID(Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_THIRTEEN));
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    auto expandDisplay = theme->GetExpandDisplay();
    CHECK_NULL_VOID(expandDisplay);

    auto mainMenuPattern = pattern->GetMainMenuPattern();
    CHECK_NULL_VOID(mainMenuPattern);
    if (!mainMenuPattern->IsContextMenu() && !mainMenuPattern->IsMenu()) {
        return;
    }

    if (idealSizeHasVal) {
        layoutConstraint->selfIdealSize.SetWidth(theme->GetMenuDefaultWidth().ConvertToPx());
    } else {
        childConstraint.minSize.SetWidth(theme->GetMenuDefaultWidth().ConvertToPx() - paddingWidth);
    }
}

void MultiMenuLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    BoxLayoutAlgorithm::PerformLayout(layoutWrapper);

    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    auto layoutProperty = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    auto node = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(node);
    auto pattern = node->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(pattern);
    OffsetF translate(0.0f, 0.0f);
    const auto& padding = layoutProperty->CreatePaddingAndBorder();
    auto outPadding = static_cast<float>(theme->GetOutPadding().ConvertToPx());
    if (!pattern->IsEmbedded()) {
        translate.AddX(padding.left.value_or(outPadding));
        translate.AddY(padding.top.value_or(outPadding));
    }
    // translate each option by the height of previous options
    for (const auto& child : layoutWrapper->GetAllChildrenWithBuild()) {
        child->GetGeometryNode()->SetMarginFrameOffset(translate);
        child->Layout();
        translate.AddY(child->GetGeometryNode()->GetMarginFrameSize().Height());
    }
}

void MultiMenuLayoutAlgorithm::UpdateEmbeddedPercentReference(LayoutWrapper* layoutWrapper,
    LayoutConstraintF& childConstraint, std::optional<LayoutConstraintF>& layoutConstraint)
{
    // Set percent reference for embedded menu the same as for parent menu item
    auto node = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(node);
    auto parentNode = AceType::DynamicCast<FrameNode>(node->GetParent());
    CHECK_NULL_VOID(parentNode);
    auto parentProperty = parentNode->GetLayoutProperty<MenuItemLayoutProperty>();
    CHECK_NULL_VOID(parentProperty);
    auto parentConstraint = parentProperty->GetLayoutConstraint();
    CHECK_NULL_VOID(parentConstraint);
    auto parentPercentReference = parentConstraint->percentReference.Height();
    layoutConstraint->percentReference.SetHeight(parentPercentReference);

    auto props = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(props);
    const auto& calcConstraint = props->GetCalcLayoutConstraint();
    if (calcConstraint && calcConstraint->selfIdealSize.has_value() &&
        calcConstraint->selfIdealSize.value().Height().has_value()) {
        ScaleProperty scaleProperty;
        if (layoutWrapper->GetGeometryNode() && layoutWrapper->GetGeometryNode()->GetParentLayoutConstraint()) {
            scaleProperty = layoutWrapper->GetGeometryNode()->GetParentLayoutConstraint()->scaleProperty;
        } else {
            scaleProperty = layoutConstraint->scaleProperty;
        }
        userHeight_ = ConvertToPx(calcConstraint->selfIdealSize.value().Height()->GetDimension(),
            scaleProperty, layoutConstraint->percentReference.Height()).value_or(0.0f);
    }

    // Update embedded menu items percent reference dependent of the submenu constraint
    if (GreatNotEqual(userHeight_, 0.0f)) {
        childConstraint.percentReference.SetHeight(userHeight_);
    } else {
        childConstraint.percentReference.SetHeight(parentPercentReference);
    }
}

void MultiMenuLayoutAlgorithm::UpdateSelfSize(LayoutWrapper* layoutWrapper,
    LayoutConstraintF& childConstraint, std::optional<LayoutConstraintF>& layoutConstraint)
{
    float contentHeight = 0.0f;
    float contentWidth = childConstraint.selfIdealSize.Width().value();
    for (const auto& child : layoutWrapper->GetAllChildrenWithBuild()) {
        child->Measure(ResetLayoutConstraintMinWidth(child, childConstraint));
        auto childHeight = std::max(child->GetGeometryNode()->GetMarginFrameSize().Height(),
            child->GetGeometryNode()->GetContentSize().Height());
        contentHeight += childHeight;
    }
    layoutWrapper->GetGeometryNode()->SetContentSize(SizeF(contentWidth, contentHeight));
    BoxLayoutAlgorithm::PerformMeasureSelf(layoutWrapper);

    auto node = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(node);
    auto pattern = node->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(pattern);
    // Stack or Embedded submenu must follow parent width
    if (pattern->IsStackSubmenu() || pattern->IsEmbedded()) {
        auto idealSize = layoutWrapper->GetGeometryNode()->GetFrameSize();
        auto width = layoutConstraint->maxSize.Width();
        idealSize.SetWidth(width);
        if (pattern->IsEmbedded()) {
            auto idealHeight = GreatNotEqual(userHeight_, 0.0f) ? userHeight_ : contentHeight;
            idealSize.SetHeight(idealHeight);
        }
        layoutWrapper->GetGeometryNode()->SetFrameSize(idealSize);
    } else if (layoutConstraint->selfIdealSize.Width().has_value()) {
        auto idealWidth = std::max(layoutConstraint->minSize.Width(),
            std::min(layoutConstraint->maxSize.Width(), layoutConstraint->selfIdealSize.Width().value()));
        auto idealSize = layoutWrapper->GetGeometryNode()->GetFrameSize();
        idealSize.SetWidth(idealWidth);
        layoutWrapper->GetGeometryNode()->SetFrameSize(idealSize);
    }
}

void MultiMenuLayoutAlgorithm::UpdateConstraintBaseOnMenuItems(
    LayoutWrapper* layoutWrapper, LayoutConstraintF& constraint)
{
    // multiMenu children are menuItem or menuItemGroup, constrain width is same as the menu
    auto maxChildrenWidth = GetChildrenMaxWidth(layoutWrapper, constraint);
    constraint.selfIdealSize.SetWidth(maxChildrenWidth);
}

float MultiMenuLayoutAlgorithm::GetChildrenMaxWidth(
    LayoutWrapper* layoutWrapper, const LayoutConstraintF& layoutConstraint)
{
    float maxWidth = 0.0f;
    for (const auto& child : layoutWrapper->GetAllChildrenWithBuild()) {
        auto childConstraint = ResetLayoutConstraintMinWidth(child, layoutConstraint);
        child->Measure(childConstraint);
        auto childSize = child->GetGeometryNode()->GetMarginFrameSize();
        maxWidth = std::max(maxWidth, childSize.Width());
    }
    return maxWidth;
}

LayoutConstraintF MultiMenuLayoutAlgorithm::ResetLayoutConstraintMinWidth(
    const RefPtr<LayoutWrapper>& child, const LayoutConstraintF& layoutConstraint)
{
    auto childLayoutProps = child->GetLayoutProperty();
    CHECK_NULL_RETURN(childLayoutProps, layoutConstraint);
    auto childConstraint = layoutConstraint;
    const auto& calcConstraint = childLayoutProps->GetCalcLayoutConstraint();
    if (calcConstraint && calcConstraint->selfIdealSize.has_value() &&
        calcConstraint->selfIdealSize.value().Width().has_value()) {
        childConstraint.minSize.Reset();
    }
    return childConstraint;
}
} // namespace OHOS::Ace::NG

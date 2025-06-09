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

#include "core/components_ng/pattern/menu/sub_menu_layout_algorithm.h"

#include "core/components/container_modal/container_modal_constants.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_pattern.h"
namespace OHOS::Ace::NG {

void SubMenuLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto size = layoutWrapper->GetGeometryNode()->GetFrameSize();
    auto menuNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(menuNode);
    auto menuPattern = menuNode->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(menuPattern);
    auto props = AceType::DynamicCast<MenuLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(props);
    auto parentMenuItem = menuPattern->GetParentMenuItem();
    CHECK_NULL_VOID(parentMenuItem);
    InitCanExpandCurrentWindow(props->GetShowInSubWindowValue(false));
    if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
        ModifySubMenuWrapper(layoutWrapper);
    }
    CheckMenuPadding(layoutWrapper);
    const auto& geometryNode = layoutWrapper->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto parentItemPattern = parentMenuItem->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(parentItemPattern);
    auto expandingMode = parentItemPattern->GetExpandingMode();
    OffsetF position = GetSubMenuLayoutOffset(layoutWrapper, parentMenuItem, size, expandingMode);
    geometryNode->SetMarginFrameOffset(position);
    if (parentMenuItem) {
        UpdateHoverRegion(parentMenuItem, position, size);
    }
    auto child = layoutWrapper->GetOrCreateChildByIndex(0);
    CHECK_NULL_VOID(child);
    child->Layout();
    ClipMenuPath(layoutWrapper);
}

void SubMenuLayoutAlgorithm::UpdateHoverRegion(
    RefPtr<FrameNode>& parentMenuItem, const OffsetF& position, const SizeF& size)
{
    auto parentPattern = parentMenuItem->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(parentPattern);
    auto bottomRightPoint = position + OffsetF(size.Width(), size.Height());
    auto pipelineContext = parentMenuItem->GetContextWithCheck();
    CHECK_NULL_VOID(pipelineContext);
    auto isContainerModal = pipelineContext->GetWindowModal() == WindowModal::CONTAINER_MODAL;
    OffsetF wrapperOffset;
    if ((!canExpandCurrentWindow_) && isContainerModal) {
        auto newOffsetX = static_cast<float>(CONTAINER_BORDER_WIDTH.ConvertToPx());
        auto newOffsetY = static_cast<float>(pipelineContext->GetCustomTitleHeight().ConvertToPx()) +
                          static_cast<float>(CONTAINER_BORDER_WIDTH.ConvertToPx());
        wrapperOffset = OffsetF(newOffsetX, newOffsetY);
    }
    parentPattern->AddHoverRegions(position + wrapperOffset, bottomRightPoint + wrapperOffset);
}

OffsetF SubMenuLayoutAlgorithm::GetSubMenuLayoutOffset(LayoutWrapper* layoutWrapper,
    const RefPtr<FrameNode>& parentMenuItem, const SizeF& size, SubMenuExpandingMode expandingMode)
{
    OffsetF position;
    auto layoutDirection = layoutWrapper->GetLayoutProperty()->GetNonAutoLayoutDirection();
    position = MenuLayoutAvoidAlgorithm(parentMenuItem, size, expandingMode, layoutWrapper);
    if (layoutDirection == TextDirection::RTL) {
        position.SetX(wrapperSize_.Width() - position.GetX() - size.Width());
    }
    return position;
}

OffsetF SubMenuLayoutAlgorithm::MenuLayoutAvoidAlgorithm(const RefPtr<FrameNode>& parentMenuItem, const SizeF& size,
    SubMenuExpandingMode expandingMode, LayoutWrapper* layoutWrapper)
{
    auto pipelineContext = PipelineContext::GetMainPipelineContext();
    CHECK_NULL_RETURN(pipelineContext, NG::OffsetF(0.0f, 0.0f));
    auto menuItemSize = parentMenuItem->GetGeometryNode()->GetFrameSize();
    position_ = GetSubMenuPosition(parentMenuItem, expandingMode);
    bool stacked = expandingMode == SubMenuExpandingMode::STACK;
    if (layoutWrapper != nullptr) {
        auto menuLayoutProperty = layoutWrapper->GetLayoutProperty();
        CHECK_NULL_RETURN(menuLayoutProperty, NG::OffsetF(0.0f, 0.0f));
        auto layoutDirection = menuLayoutProperty->GetNonAutoLayoutDirection();
        if (layoutDirection == TextDirection::RTL) {
            float leftSpace = position_.GetX() - menuItemSize.Width();
            position_ = OffsetF(wrapperSize_.Width() - leftSpace, position_.GetY());
        }
    }
    float x = HorizontalLayoutSubMenu(size, position_.GetX(), menuItemSize);
    x = std::clamp(x, paddingStart_, wrapperSize_.Width() - size.Width() - paddingEnd_);
    float y = 0.0f;
    if (canExpandCurrentWindow_ || !Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
        y = VerticalLayoutSubMenu(size, position_.GetY(), menuItemSize, parentMenuItem, stacked, layoutWrapper);
    } else {
        y = VerticalLayoutSubMenuHalfScreen(size, position_.GetY(), menuItemSize, parentMenuItem, stacked,
            layoutWrapper);
    }
    float yMinAvoid = wrapperRect_.Top() + paddingTop_;
    float yMaxAvoid = wrapperRect_.Bottom() - paddingBottom_ - size.Height();
    if (stacked) {
        yMinAvoid = wrapperRect_.Top() + param_.topSecurity;
        yMaxAvoid = wrapperRect_.Bottom() - param_.bottomSecurity - size.Height();
    }
    y = std::clamp(y, yMinAvoid, yMaxAvoid);
    return NG::OffsetF(x, y);
}

OffsetF SubMenuLayoutAlgorithm::GetSubMenuPosition(
    const RefPtr<FrameNode>& parentMenuItem, SubMenuExpandingMode expandingMode)
{
    auto parentItemFrameSize = parentMenuItem->GetGeometryNode()->GetMarginFrameSize();
    OffsetF position;
    if (expandingMode == SubMenuExpandingMode::STACK) {
        auto parentItemPattern = parentMenuItem->GetPattern<MenuItemPattern>();
        if (parentItemPattern != nullptr) {
            auto parentMenu = parentItemPattern->GetMenu();
            position = parentMenu == nullptr
                ? parentMenuItem->GetPaintRectOffset(false, true) + OffsetF(parentItemFrameSize.Width(), 0.0)
                : OffsetF(parentMenu->GetPaintRectOffset(false, true).GetX(),
                    parentMenuItem->GetPaintRectOffset(false, true).GetY() +
                    parentItemFrameSize.Height()); // * 0.95
        }
    } else if (expandingMode == SubMenuExpandingMode::SIDE) {
        auto pipeline = parentMenuItem->GetContext();
        auto selectTheme = pipeline ? pipeline->GetTheme<SelectTheme>() : nullptr;
        float contentPadding = selectTheme ? -static_cast<float>(selectTheme->GetMenuPadding().ConvertToPx()) : 0.0f;
        position =
            parentMenuItem->GetPaintRectOffset(false, true) + OffsetF(parentItemFrameSize.Width(), contentPadding);
    } else {
        position = parentMenuItem->GetPaintRectOffset(false, true) + OffsetF(parentItemFrameSize.Width(), 0.0);
    }

    auto pipelineContext = parentMenuItem->GetContextWithCheck();
    CHECK_NULL_RETURN(pipelineContext, OffsetF());
    auto isContainerModal = pipelineContext->GetWindowModal() == WindowModal::CONTAINER_MODAL;
    if ((!canExpandCurrentWindow_) && isContainerModal) {
        auto newOffsetX = static_cast<float>(CONTAINER_BORDER_WIDTH.ConvertToPx());
        auto newOffsetY = static_cast<float>(pipelineContext->GetCustomTitleHeight().ConvertToPx()) +
                            static_cast<float>(CONTAINER_BORDER_WIDTH.ConvertToPx());
        position -= OffsetF(newOffsetX, newOffsetY);
    }
    auto parentMenu = AceType::DynamicCast<FrameNode>(parentMenuItem->GetParent());
    CHECK_NULL_RETURN(parentMenu, position);
    auto scroll = AceType::DynamicCast<FrameNode>(parentMenu->GetParent());
    CHECK_NULL_RETURN(scroll, position);
    while (scroll && (scroll->GetTag() != V2::SCROLL_ETS_TAG)) {
        scroll = AceType::DynamicCast<FrameNode>(scroll->GetParent());
    }
    CHECK_NULL_RETURN(scroll, position);
    auto scrollGeometryNode = scroll->GetGeometryNode();
    CHECK_NULL_RETURN(scrollGeometryNode, position);
    auto scrollTop = scroll->GetPaintRectOffset(false, true).GetY();
    auto scrollHeight = scrollGeometryNode->GetFrameSize().Height();
    auto bottomOffset = scrollTop + scrollHeight;
    if (parentMenuItem->GetPaintRectOffset(false, true).GetY() > bottomOffset) {
        return scroll->GetPaintRectOffset(false, true) + OffsetF(parentItemFrameSize.Width(), 0.0);
    }
    return position;
}

float SubMenuLayoutAlgorithm::CalcStackSubMenuPositionYHalfScreenWithPreview(
    const SizeF& size,
    const RefPtr<FrameNode>& parentMenu,
    LayoutWrapper* layoutWrapper
)
{
    auto parentMenuPattern = parentMenu->GetPattern<MenuPattern>();
    auto parentPlacement = parentMenuPattern->GetLastPlacement().value_or(Placement::NONE);
    auto firstItemBottomPositionY = GetFirstItemBottomPositionY(parentMenu);
    float parentMenuBottomY = GetMenuBottomPositionY(parentMenu);
    auto lastItemPositionY = GetLastItemTopPositionY(parentMenu);
    auto containerModalOffsetY = GetContainerModalOffsetY(parentMenu);
    //correct position when window modal is containerModal
    if (isContainerModal(parentMenu)) {
        firstItemBottomPositionY -= containerModalOffsetY;
        parentMenuBottomY -= containerModalOffsetY;
        lastItemPositionY -= containerModalOffsetY;
    }
    if (parentPlacement == Placement::TOP_LEFT || parentPlacement == Placement::TOP ||
        parentPlacement == Placement::TOP_RIGHT) {
        auto bottomSpace = parentMenuBottomY - position_.GetY();
        if (bottomSpace >= size.Height()) {
            return position_.GetY();
        }
        bottomSpace = lastItemPositionY - wrapperRect_.Top() - param_.topSecurity;
        if (size.Height() <= bottomSpace) {
            return lastItemPositionY - size.Height();
        }
        auto subMenuNode = layoutWrapper->GetHostNode();
        auto subMenuPattern = subMenuNode->GetPattern<MenuPattern>();
        auto diffY = bottomSpace - size.Height();
        subMenuPattern->SetTranslateYForStack(diffY);
        return wrapperRect_.Top() + param_.topSecurity;
    } else {
        auto bottomSpace = wrapperRect_.Bottom() - param_.bottomSecurity - position_.GetY();
        if (bottomSpace >= size.Height()) {
            return position_.GetY();
        }
        bottomSpace = wrapperRect_.Bottom() - param_.bottomSecurity - firstItemBottomPositionY;
        if (size.Height() <= bottomSpace) {
            return wrapperRect_.Bottom() - param_.bottomSecurity - size.Height();
        }
        auto subMenuNode = layoutWrapper->GetHostNode();
        auto subMenuPattern = subMenuNode->GetPattern<MenuPattern>();
        auto diffY = size.Height() - bottomSpace;
        subMenuPattern->SetTranslateYForStack(diffY);
        return wrapperRect_.Bottom() - param_.bottomSecurity - size.Height();
    }
}

float SubMenuLayoutAlgorithm::CalcStackSubMenuPositionYHalfScreen(
    const SizeF& size, const RefPtr<FrameNode>& parentMenu, const RefPtr<FrameNode>& parentMenuItem
)
{
    auto parentMenuPattern = parentMenu->GetPattern<MenuPattern>();
    auto parentPlacement = parentMenuPattern->GetLastPlacement().value_or(Placement::NONE);
    auto firstItemBottomPositionY = GetFirstItemBottomPositionY(parentMenu);
    float parentMenuBottomY = GetMenuBottomPositionY(parentMenu);
    float lastMenuItemPositionY = GetLastItemTopPositionY(parentMenu);
    auto containerModalOffsetY = GetContainerModalOffsetY(parentMenu);
    auto parentMenuPositionY = parentMenu->GetPaintRectOffset(false, true).GetY();
    //correct position when window modal is containerModal
    if (isContainerModal(parentMenu)) {
        firstItemBottomPositionY -= containerModalOffsetY;
        parentMenuBottomY -= containerModalOffsetY;
        lastMenuItemPositionY -= containerModalOffsetY;
        parentMenuPositionY -= containerModalOffsetY;
    }
    float bottomSpace = 0.0f;
    if (parentPlacement == Placement::TOP_LEFT || parentPlacement == Placement::TOP ||
            parentPlacement == Placement::TOP_RIGHT) {
        bottomSpace = parentMenuBottomY - position_.GetY();
        if (bottomSpace >= size.Height()) {
            return position_.GetY();
        }
        bottomSpace = lastMenuItemPositionY - wrapperRect_.Top() - param_.topSecurity;
        if (bottomSpace >= size.Height()) {
            return lastMenuItemPositionY - size.Height();
        }
        return wrapperRect_.Top() + param_.topSecurity;
    }
    bottomSpace = wrapperRect_.Bottom() - param_.bottomSecurity - position_.GetY();
    if (bottomSpace >= size.Height()) {
        return position_.GetY();
    }
    if (size.Height() < wrapperRect_.Height()) {
        bottomSpace = wrapperRect_.Bottom() - param_.bottomSecurity - firstItemBottomPositionY;
        if (size.Height() <= bottomSpace) {
            return wrapperRect_.Bottom() - param_.bottomSecurity - size.Height();
        }
        if (bottomSpace < parentMenuItem->GetGeometryNode()->GetFrameSize().Height()) {
            return parentMenuPositionY;
        }
        return firstItemBottomPositionY;
    }
    // can't fit in screen, line up with top of the screen
    return 0.0f;
}

float SubMenuLayoutAlgorithm::VerticalLayoutSubMenuHalfScreen(
    const SizeF& size, float position, const SizeF& menuItemSize,
    const RefPtr<FrameNode>& parentMenuItem, bool stacked, LayoutWrapper* layoutWrapper)
{
    auto pipelineContext = PipelineContext::GetMainPipelineContext();
    CHECK_NULL_RETURN(pipelineContext, 0.0f);
    auto safeAreaManager = pipelineContext->GetSafeAreaManager();
    CHECK_NULL_RETURN(safeAreaManager, 0.0f);
    float wrapperHeight = wrapperSize_.Height();
    float bottomSpace = 0.0f;
    if (!stacked) {
        bottomSpace = wrapperSize_.Height() - (position_.GetY() - param_.windowsOffsetY) - margin_ * 2.0f;
        // line up top of subMenu with top of the menuItem
        if (bottomSpace >= size.Height()) {
            return position;
        }
        // line up bottom of menu with bottom of the screen
        if (size.Height() < wrapperHeight) {
            return wrapperHeight - size.Height();
        }
        // can't fit in screen, line up with top of the screen
        return 0.0f;
    }
    auto parentItemPattern = parentMenuItem->GetPattern<MenuItemPattern>();
    auto parentMenu = parentItemPattern->GetMenu(true);
    auto parentMenuPattern = parentMenu->GetPattern<MenuPattern>();
    if (parentMenuPattern->GetPreviewMode() != MenuPreviewMode::NONE) {
        return CalcStackSubMenuPositionYHalfScreenWithPreview(size, parentMenu, layoutWrapper);
    } else {
        return CalcStackSubMenuPositionYHalfScreen(size, parentMenu, parentMenuItem);
    }
}

// return submenu vertical offset
float SubMenuLayoutAlgorithm::VerticalLayoutSubMenu(const SizeF& size, float position, const SizeF& menuItemSize,
    const RefPtr<FrameNode>& parentMenuItem, bool stacked, LayoutWrapper* layoutWrapper)
{
    float bottomSpace = 0.0f;
    if (!stacked) {
        bottomSpace = wrapperRect_.Bottom() - position - paddingBottom_;
        // line up top of subMenu with top of the menuItem
        if (bottomSpace >= size.Height()) {
            return position;
        }
        // line up bottom of menu with bottom of the screen
        if (size.Height() < wrapperRect_.Height()) {
            return wrapperRect_.Bottom() - size.Height() - paddingBottom_;
        }
        // can't fit in screen, line up with top of the screen
        return wrapperRect_.Top() + paddingTop_;
    }
    auto parentItemPattern = parentMenuItem->GetPattern<MenuItemPattern>();
    auto parentMenu = parentItemPattern->GetMenu(true);
    auto parentMenuPattern = parentMenu->GetPattern<MenuPattern>();
    if (parentMenuPattern->GetPreviewMode() != MenuPreviewMode::NONE) {
        return CalcStackSubMenuPositionYHalfScreenWithPreview(size, parentMenu, layoutWrapper);
    } else {
        return CalcStackSubMenuPositionYHalfScreen(size, parentMenu, parentMenuItem);
    }
}

// returns submenu horizontal offset
float SubMenuLayoutAlgorithm::HorizontalLayoutSubMenu(
    const SizeF& size, float position, const SizeF& menuItemSize, LayoutWrapper* layoutWrapper)
{
    float wrapperWidth = wrapperSize_.Width();
    float rightSpace = wrapperWidth - position - paddingEnd_;
    float leftSpace = position - menuItemSize.Width();
    if (layoutWrapper != nullptr) {
        auto menuLayoutProperty = layoutWrapper->GetLayoutProperty();
        CHECK_NULL_RETURN(menuLayoutProperty, 0.0f);
        auto layoutDirection = menuLayoutProperty->GetNonAutoLayoutDirection();
        if (layoutDirection == TextDirection::RTL) {
            rightSpace = position - menuItemSize.Width();
            leftSpace = wrapperWidth - position;
        }
    }
    // can fit subMenu on the right side of menuItem
    if (rightSpace >= size.Width()) {
        return position;
    }
    // fit subMenu on the left side of menuItem
    if (leftSpace >= size.Width()) {
        return position - size.Width() - menuItemSize.Width();
    }
    // line up right side of menu with right boundary of the screen
    if (size.Width() < wrapperWidth) {
        return wrapperWidth - size.Width() - paddingEnd_;
    }
    // can't fit in screen, line up with left side of the screen
    return 0.0f;
}

void SubMenuLayoutAlgorithm::ModifySubMenuWrapper(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto pipelineContext = PipelineContext::GetMainPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    auto safeAreaManager = pipelineContext->GetSafeAreaManager();
    CHECK_NULL_VOID(safeAreaManager);
    auto bottom = safeAreaManager->GetSystemSafeArea().bottom_.Length();
    if (!canExpandCurrentWindow_) {
        wrapperSize_ = SizeF(param_.menuWindowRect.Width(), param_.menuWindowRect.Height() - bottom);
    } else {
        wrapperSize_ = SizeF(wrapperSize_.Width(), wrapperSize_.Height());
    }
}

void SubMenuLayoutAlgorithm::InitializePadding(LayoutWrapper* layoutWrapper)
{
    auto menuPattern = layoutWrapper->GetHostNode()->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(menuPattern);
    auto host = menuPattern->GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContextWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    if (!menuPattern->IsSelectOverlayExtensionMenu()) {
        margin_ = static_cast<float>(theme->GetMenuPadding().ConvertToPx());
        paddingStart_ = static_cast<float>(theme->GetDefaultPaddingStart().ConvertToPx());
        paddingEnd_ = static_cast<float>(theme->GetDefaultPaddingEnd().ConvertToPx());
        if (!AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
            paddingTop_ = static_cast<float>(theme->GetDefaultPaddingTop().ConvertToPx());
            paddingBottom_ = static_cast<float>(theme->GetDefaultPaddingBottomFixed().ConvertToPx());
        }
    }
}

void SubMenuLayoutAlgorithm::InitializePaddingAPI12(LayoutWrapper* layoutWrapper)
{
    auto menuNode = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(menuNode);
    auto menuPattern = menuNode->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(menuPattern);
    auto pipeline = PipelineContext::GetMainPipelineContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    if (!menuPattern->IsSelectOverlayExtensionMenu()) {
        margin_ = static_cast<float>(theme->GetMenuPadding().ConvertToPx());
        if (!canExpandCurrentWindow_) {
            paddingStart_ = static_cast<float>(theme->GetMenuLargeMargin().ConvertToPx());
            paddingEnd_ = static_cast<float>(theme->GetMenuLargeMargin().ConvertToPx());
        } else {
            paddingStart_ = static_cast<float>(theme->GetMenuMediumMargin().ConvertToPx());
            paddingEnd_ = static_cast<float>(theme->GetMenuMediumMargin().ConvertToPx());
        }
    }
}

void SubMenuLayoutAlgorithm::CheckMenuPadding(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        InitializePaddingAPI12(layoutWrapper);
    } else {
        InitializePadding(layoutWrapper);
    }
}

} // namespace OHOS::Ace::NG

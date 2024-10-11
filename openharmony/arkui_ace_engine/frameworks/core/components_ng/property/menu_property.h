/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MENU_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MENU_PROPERTY_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/placement.h"
#include "core/components_ng/event/gesture_event_hub.h"
#include "core/components_ng/property/transition_property.h"

namespace OHOS::Ace::NG {

enum class MenuType {
    // ----- Menu Containers ------
    MENU,         // corresponds to .bindMenu attribute
    CONTEXT_MENU, // corresponds to .bindContextMenu attribute, lives in a SubWindow
    SUB_MENU,     // secondary menu container in a multi-level menu

    // ----- innerMenu Node, corersponds to <Menu> tag in the frontend ------
    MULTI_MENU,   // called multi because it's a multi-leveled menu, its MenuItems can trigger subMenus
    DESKTOP_MENU, // menu specialized for desktop UI, enabled when multiple sibiling <Menu> nodes are present

    // ----- special menu used in other components ------
    NAVIGATION_MENU,               // menu used in a Navigation component
    SELECT_OVERLAY_EXTENSION_MENU, // menu used in SelectOverlay Extension of text component,skip menu layout algorithm
    SELECT_OVERLAY_CUSTOM_MENU,    // menu used in SelectOverlay for custom menu
                                   // click menu item whill not trigger close menu
    SELECT_OVERLAY_SUB_MENU,       // menu type used for select overlay sub menu
    SELECT_OVERLAY_RIGHT_CLICK_MENU, // menu type used for select overlay menu triggered by right-click
};

enum class ContextMenuRegisterType : char {
    NORMAL_TYPE = 0,
    CUSTOM_TYPE = 1,
};

struct MenuParam {
    std::string title;
    OffsetF positionOffset;
    bool setShow = false;
    bool isShow = false;
    MenuBindingType menuBindType = MenuBindingType::LONG_PRESS;
    ContextMenuRegisterType contextMenuRegisterType = ContextMenuRegisterType::NORMAL_TYPE;
    std::function<void(const std::string&)> onStateChange;
    std::optional<Placement> placement;
    bool enableHoverMode = false;
    std::function<void()> onAppear;
    std::function<void()> onDisappear;
    std::function<void()> aboutToAppear;
    std::function<void()> aboutToDisappear;
    std::optional<bool> enableArrow;
    std::optional<Dimension> arrowOffset;
    bool isShowInSubWindow = true;
    bool hasTransitionEffect = false;
    RefPtr<NG::ChainedTransitionEffect> transition;
    bool hasPreviewTransitionEffect = false;
    RefPtr<NG::ChainedTransitionEffect> previewTransition;
    MenuType type = MenuType::MENU;
    MenuPreviewMode previewMode = MenuPreviewMode::NONE;
    MenuPreviewAnimationOptions previewAnimationOptions;
    bool isShowHoverImage = false;
    MenuPreviewAnimationOptions hoverImageAnimationOptions;
    std::optional<EffectOption> backgroundEffectOption;
    std::optional<Color> backgroundColor;
    std::optional<int32_t> backgroundBlurStyle;
    std::optional<NG::BorderRadiusProperty> borderRadius;
    std::optional<NG::BorderRadiusProperty> previewBorderRadius;
    bool isSetPreviewNone = false; // user set MenuPreviewMode::NONE
    std::optional<NG::MarginProperty> layoutRegionMargin;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MENU_PROPERTY_H

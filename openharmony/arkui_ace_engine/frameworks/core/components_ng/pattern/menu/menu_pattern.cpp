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

#include "core/components_ng/pattern/menu/menu_pattern.h"

#include "base/geometry/dimension.h"
#include "base/log/dump_log.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/select/select_theme.h"
#include "core/components/theme/shadow_theme.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/manager/drag_drop/utils/drag_animation_helper.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_layout_property.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_pattern.h"
#include "core/components_ng/pattern/menu/menu_item_group/menu_item_group_pattern.h"
#include "core/components_ng/pattern/menu/menu_layout_property.h"
#include "core/components_ng/pattern/menu/menu_theme.h"
#include "core/components_ng/pattern/menu/menu_view.h"
#include "core/components_ng/pattern/menu/multi_menu_layout_algorithm.h"
#include "core/components_ng/pattern/menu/preview/menu_preview_pattern.h"
#include "core/components_ng/pattern/menu/sub_menu_layout_algorithm.h"
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_pattern.h"
#include "core/components_ng/pattern/option/option_pattern.h"
#include "core/components_ng/pattern/option/option_view.h"
#include "core/components_ng/pattern/scroll/scroll_pattern.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/property/border_property.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/event/touch_event.h"
#include "core/pipeline/pipeline_base.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
constexpr float PAN_MAX_VELOCITY = 2000.0f;
constexpr Dimension MIN_SELECT_MENU_WIDTH = 64.0_vp;
constexpr int32_t COLUMN_NUM = 2;
constexpr int32_t STACK_EXPAND_DISAPPEAR_DURATION = 300;
constexpr int32_t HALF_FOLD_HOVER_DURATION = 1000;
constexpr double MENU_ORIGINAL_SCALE = 0.6f;
constexpr double MOUNT_MENU_OPACITY = 0.4f;

constexpr double VELOCITY = 0.0f;
constexpr double MASS = 1.0f;
constexpr double STIFFNESS = 228.0f;
constexpr double DAMPING = 22.0f;
constexpr double STACK_MENU_DAMPING = 26.0f;
const RefPtr<InterpolatingSpring> MENU_ANIMATION_CURVE =
    AceType::MakeRefPtr<InterpolatingSpring>(VELOCITY, MASS, STIFFNESS, DAMPING);
const RefPtr<InterpolatingSpring> STACK_MENU_CURVE =
    AceType::MakeRefPtr<InterpolatingSpring>(VELOCITY, MASS, STIFFNESS, STACK_MENU_DAMPING);
const RefPtr<Curve> CUSTOM_PREVIEW_ANIMATION_CURVE =
    AceType::MakeRefPtr<InterpolatingSpring>(0.0f, 1.0f, 328.0f, 34.0f);
const RefPtr<InterpolatingSpring> MAIN_MENU_ANIMATION_CURVE =
    AceType::MakeRefPtr<InterpolatingSpring>(0.0f, 1.0f, 528.0f, 35.0f);
const float MINIMUM_AMPLITUDE_RATION = 0.08f;

constexpr double MOUNT_MENU_FINAL_SCALE = 0.95f;
constexpr double SEMI_CIRCLE_ANGEL = 90.0f;
constexpr Dimension PADDING = 4.0_vp;

void UpdateFontStyle(RefPtr<MenuLayoutProperty>& menuProperty, RefPtr<MenuItemLayoutProperty>& itemProperty,
    RefPtr<MenuItemPattern>& itemPattern, bool& contentChanged, bool& labelChanged)
{
    auto contentNode = itemPattern->GetContentNode();
    CHECK_NULL_VOID(contentNode);
    auto textLayoutProperty = contentNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    auto label = itemPattern->GetLabelNode();
    RefPtr<TextLayoutProperty> labelProperty = label ? label->GetLayoutProperty<TextLayoutProperty>() : nullptr;
    if (menuProperty->GetItalicFontStyle().has_value()) {
        if (!itemProperty->GetItalicFontStyle().has_value()) {
            textLayoutProperty->UpdateItalicFontStyle(menuProperty->GetItalicFontStyle().value());
            contentChanged = true;
        }
        if (labelProperty && !itemProperty->GetLabelItalicFontStyle().has_value()) {
            labelProperty->UpdateItalicFontStyle(menuProperty->GetItalicFontStyle().value());
            labelChanged = true;
        }
    }
    if (menuProperty->GetFontFamily().has_value()) {
        if (!itemProperty->GetFontFamily().has_value()) {
            textLayoutProperty->UpdateFontFamily(menuProperty->GetFontFamily().value());
            contentChanged = true;
        }
        if (labelProperty && !itemProperty->GetLabelFontFamily().has_value()) {
            labelProperty->UpdateFontFamily(menuProperty->GetFontFamily().value());
            labelChanged = true;
        }
    }
}

void UpdateMenuItemTextNode(RefPtr<MenuLayoutProperty>& menuProperty, RefPtr<MenuItemLayoutProperty>& itemProperty,
    RefPtr<MenuItemPattern>& itemPattern)
{
    auto contentNode = itemPattern->GetContentNode();
    CHECK_NULL_VOID(contentNode);
    auto textLayoutProperty = contentNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    auto label = itemPattern->GetLabelNode();
    RefPtr<TextLayoutProperty> labelProperty = label ? label->GetLayoutProperty<TextLayoutProperty>() : nullptr;
    bool contentChanged = false;
    bool labelChanged = false;
    if (menuProperty->GetFontSize().has_value()) {
        if (!itemProperty->GetFontSize().has_value()) {
            textLayoutProperty->UpdateFontSize(menuProperty->GetFontSize().value());
            contentChanged = true;
        }
        if (labelProperty && !itemProperty->GetLabelFontSize().has_value()) {
            labelProperty->UpdateFontSize(menuProperty->GetFontSize().value());
            labelChanged = true;
        }
    }
    if (menuProperty->GetFontWeight().has_value()) {
        if (!itemProperty->GetFontWeight().has_value()) {
            textLayoutProperty->UpdateFontWeight(menuProperty->GetFontWeight().value());
            contentChanged = true;
        }
        if (labelProperty && !itemProperty->GetLabelFontWeight().has_value()) {
            labelProperty->UpdateFontWeight(menuProperty->GetFontWeight().value());
            labelChanged = true;
        }
    }
    if (menuProperty->GetFontColor().has_value()) {
        if (!itemProperty->GetFontColor().has_value()) {
            textLayoutProperty->UpdateTextColor(menuProperty->GetFontColor().value());
            contentChanged = true;
        }
        if (labelProperty && !itemProperty->GetLabelFontColor().has_value()) {
            labelProperty->UpdateTextColor(menuProperty->GetFontColor().value());
            labelChanged = true;
        }
    }
    UpdateFontStyle(menuProperty, itemProperty, itemPattern, contentChanged, labelChanged);
    if (contentChanged) {
        contentNode->MarkModifyDone();
        contentNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    }
    if (labelChanged) {
        label->MarkModifyDone();
        label->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    }
}

void ShowMenuOpacityAnimation(const RefPtr<MenuTheme>& menuTheme, const RefPtr<RenderContext>& renderContext,
    int32_t delay)
{
    CHECK_NULL_VOID(menuTheme);
    CHECK_NULL_VOID(renderContext);

    renderContext->UpdateOpacity(0.0);
    AnimationOption option = AnimationOption();
    option.SetCurve(Curves::FRICTION);
    option.SetDuration(menuTheme->GetContextMenuAppearDuration());
    option.SetDelay(delay);
    AnimationUtils::Animate(option, [renderContext]() {
        if (renderContext) {
            renderContext->UpdateOpacity(1.0);
        }
    });
}
} // namespace

void MenuPattern::OnAttachToFrameNode()
{
    RegisterOnTouch();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto focusHub = host->GetOrCreateFocusHub();
    CHECK_NULL_VOID(focusHub);
    RegisterOnKeyEvent(focusHub);
    DisableTabInMenu();
    InitTheme(host);
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto targetNode = FrameNode::GetFrameNode(targetTag_, targetId_);
    CHECK_NULL_VOID(targetNode);
    auto eventHub = targetNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    halfFoldHoverCallbackId_ = RegisterHalfFoldHover(targetNode);
    OnAreaChangedFunc onAreaChangedFunc = [menuNodeWk = WeakPtr<FrameNode>(host)](const RectF& oldRect,
                                              const OffsetF& oldOrigin, const RectF& /* rect */,
                                              const OffsetF& /* origin */) {
        // Not handle first change
        if (oldRect.IsEmpty() && oldOrigin.NonOffset()) {
            return;
        }
        auto pipelineContext = PipelineContext::GetCurrentContext();
        AnimationOption option;
        option.SetCurve(pipelineContext->GetSafeAreaManager()->GetSafeAreaCurve());
        AnimationUtils::Animate(
            option,
            [weakPipeline = WeakPtr<PipelineContext>(pipelineContext), weakMenu = menuNodeWk]() {
                auto menu = weakMenu.Upgrade();
                CHECK_NULL_VOID(menu);
                auto menuPattern = menu->GetPattern<MenuPattern>();
                CHECK_NULL_VOID(menuPattern);
                auto menuWarpper = menuPattern->GetMenuWrapper();
                CHECK_NULL_VOID(menuWarpper);
                auto warpperPattern = menuWarpper->GetPattern<MenuWrapperPattern>();
                CHECK_NULL_VOID(warpperPattern);
                if (!warpperPattern->IsHide()) {
                    auto pipeline = weakPipeline.Upgrade();
                    CHECK_NULL_VOID(pipeline);
                    menu->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
                    pipeline->FlushUITasks();
                }
            });
        pipelineContext->FlushPipelineImmediately();
    };
    eventHub->AddInnerOnAreaChangedCallback(host->GetId(), std::move(onAreaChangedFunc));
}

int32_t MenuPattern::RegisterHalfFoldHover(const RefPtr<FrameNode>& menuNode)
{
    // register when hoverMode enabled
    auto pipelineContext = menuNode->GetContext();
    CHECK_NULL_RETURN(pipelineContext, 0);
    int32_t callbackId = pipelineContext->RegisterHalfFoldHoverChangedCallback(
        [weak = WeakClaim(this), pipelineContext](bool isHalfFoldHover) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto host = pattern->GetHost();
        CHECK_NULL_VOID(host);
        AnimationOption optionPosition;
        auto motion = AceType::MakeRefPtr<ResponsiveSpringMotion>(0.35f, 1.0f, 0.0f);
        optionPosition.SetDuration(HALF_FOLD_HOVER_DURATION);
        optionPosition.SetCurve(motion);
        pipelineContext->FlushUITasks();
        pipelineContext->Animate(optionPosition, motion, [host, pipelineContext]() {
            host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            pipelineContext->FlushUITasks();
        });
    });
    return callbackId;
}

void MenuPattern::OnDetachFromFrameNode(FrameNode* frameNode)
{
    CHECK_NULL_VOID(frameNode);
    auto targetNode = FrameNode::GetFrameNode(targetTag_, targetId_);
    CHECK_NULL_VOID(targetNode);
    auto eventHub = targetNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->RemoveInnerOnAreaChangedCallback(frameNode->GetId());
}

void MenuPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    isNeedDivider_ = false;
    auto uiNode = AceType::DynamicCast<UINode>(host);
    UpdateMenuItemChildren(uiNode);

    auto innerMenuCount = GetInnerMenuCount();
    if (innerMenuCount == 1) {
        ResetTheme(host, false);
    } else if (innerMenuCount > 1) {
        // multiple inner menus, reset outer container's shadow for desktop UX
        ResetTheme(host, true);
    }

    auto menuWrapperNode = GetMenuWrapper();
    CHECK_NULL_VOID(menuWrapperNode);
    auto menuWrapperPattern = menuWrapperNode->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_VOID(menuWrapperPattern);
    if (!menuWrapperPattern->GetHasCustomRadius()) {
        auto menuFirstNode = GetFirstInnerMenu();
        if (menuFirstNode) {
            CopyMenuAttr(menuFirstNode);
        }
    }

    auto menuLayoutProperty = GetLayoutProperty<MenuLayoutProperty>();
    CHECK_NULL_VOID(menuLayoutProperty);
    if (menuLayoutProperty->GetBorderRadius().has_value()) {
        BorderRadiusProperty borderRadius = menuLayoutProperty->GetBorderRadiusValue();
        UpdateBorderRadius(host, borderRadius);
    }

    SetAccessibilityAction();

    if (previewMode_ != MenuPreviewMode::NONE) {
        auto node = host->GetChildren().front();
        CHECK_NULL_VOID(node);
        auto scroll = AceType::DynamicCast<FrameNode>(node);
        CHECK_NULL_VOID(scroll);
        auto hub = scroll->GetEventHub<EventHub>();
        CHECK_NULL_VOID(hub);
        auto gestureHub = hub->GetOrCreateGestureEventHub();
        CHECK_NULL_VOID(gestureHub);
        InitPanEvent(gestureHub);
    }
}

RefPtr<FrameNode> CreateMenuScroll(const RefPtr<UINode>& node)
{
    auto scroll = FrameNode::CreateFrameNode(
        V2::SCROLL_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ScrollPattern>());
    CHECK_NULL_RETURN(scroll, nullptr);
    auto props = scroll->GetLayoutProperty<ScrollLayoutProperty>();
    props->UpdateAxis(Axis::VERTICAL);
    props->UpdateAlignment(Alignment::CENTER_LEFT);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_RETURN(theme, nullptr);
    auto contentPadding = static_cast<float>(theme->GetOutPadding().ConvertToPx());
    PaddingProperty padding;
    padding.left = padding.right = padding.top = padding.bottom = CalcLength(contentPadding);
    props->UpdatePadding(padding);
    if (node) {
        node->MountToParent(scroll);
    }
    auto renderContext = scroll->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, nullptr);
    BorderRadiusProperty borderRadius;
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        borderRadius.SetRadius(theme->GetMenuDefaultRadius());
    } else {
        borderRadius.SetRadius(theme->GetMenuBorderRadius());
    }
    renderContext->UpdateBorderRadius(borderRadius);
    return scroll;
}

void MenuPattern::FireBuilder()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->RemoveChild(builderNode_.Upgrade());
    if (!makeFunc_.has_value()) {
        return;
    }
    auto column = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));
    auto scroll = CreateMenuScroll(column);
    CHECK_NULL_VOID(scroll);
    builderNode_ = scroll;
    for (size_t i = 0; i < selectProperties_.size(); i++) {
        auto contentModifierNode = BuildContentModifierNode(i);
        if (contentModifierNode) {
            contentModifierNode->MarkModifyDone();
            contentModifierNode->MountToParent(column);
        }
    }
    auto scrollPattern = scroll->GetPattern<ScrollPattern>();
    CHECK_NULL_VOID(scrollPattern);
    scrollPattern->SetIsSelectScroll(true);
    scroll->MountToParent(host);
    scroll->MarkModifyDone();
    host->MarkModifyDone();
    SetIsSelectMenu(true);
}

RefPtr<FrameNode> MenuPattern::BuildContentModifierNode(int index)
{
    if (!makeFunc_.has_value()) {
        return nullptr;
    }
    auto property = selectProperties_[index];
    MenuItemConfiguration menuItemConfiguration(property.value, property.icon, property.symbolModifier,
        index, property.selected, property.selectEnable);
    return (makeFunc_.value())(menuItemConfiguration);
}

void MenuPattern::UpdateSelectIndex(int32_t index)
{
    for (size_t i = 0; i < selectParams_.size(); i++) {
        selectProperties_[i].selected = index == static_cast<int32_t>(i);
    }
    FireBuilder();
}

void InnerMenuPattern::BeforeCreateLayoutWrapper()
{
    RecordItemsAndGroups();

    // determine menu type based on sibling menu count
    auto count = FindSiblingMenuCount();
    if (count > 0) {
        SetType(MenuType::DESKTOP_MENU);
        ApplyDesktopMenuTheme();
    } else {
        SetType(MenuType::MULTI_MENU);
        ApplyMultiMenuTheme();
    }
}

void InnerMenuPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    ResetNeedDivider();
    auto uiNode = AceType::DynamicCast<UINode>(host);
    UpdateMenuItemChildren(uiNode);
    SetAccessibilityAction();
}

// close menu on touch up
void MenuPattern::RegisterOnTouch()
{
    CHECK_NULL_VOID(!onTouch_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto gesture = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gesture);
    auto touchTask = [weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->OnTouchEvent(info);
    };
    onTouch_ = MakeRefPtr<TouchEventImpl>(std::move(touchTask));
    gesture->AddTouchEvent(onTouch_);
}

void MenuPattern::OnTouchEvent(const TouchEventInfo& info)
{
    if (GetInnerMenuCount() > 0 || IsMultiMenu() || IsDesktopMenu()|| IsSelectOverlayCustomMenu()) {
        // not click hide menu for multi menu or select overlay custom menu
        return;
    }
    if (!options_.empty()) {
        // not click hide menu for select and bindMenu default option
        return;
    }
    if (!needHideAfterTouch_) {
        // not click hide menu if needn't hide after touch
        return;
    }
    auto touchType = info.GetTouches().front().GetTouchType();
    if (touchType == TouchType::DOWN) {
        lastTouchOffset_ = info.GetTouches().front().GetLocalLocation();
    } else if (touchType == TouchType::UP) {
        auto touchUpOffset = info.GetTouches().front().GetLocalLocation();
        if (lastTouchOffset_.has_value() &&
            (touchUpOffset - lastTouchOffset_.value()).GetDistance() <= DEFAULT_CLICK_DISTANCE) {
            auto touchGlobalLocation = info.GetTouches().front().GetGlobalLocation();
            auto position = OffsetF(static_cast<float>(touchGlobalLocation.GetX()),
                static_cast<float>(touchGlobalLocation.GetY()));
            TAG_LOGI(AceLogTag::ACE_MENU, "will hide menu, position is %{public}s.", position.ToString().c_str());
            HideMenu(true, position);
        }
        lastTouchOffset_.reset();
    }
}

void MenuPattern::RegisterOnKeyEvent(const RefPtr<FocusHub>& focusHub)
{
    auto onKeyEvent = [wp = WeakClaim(this)](const KeyEvent& event) -> bool {
        auto pattern = wp.Upgrade();
        CHECK_NULL_RETURN(pattern, false);
        return pattern->OnKeyEvent(event);
    };
    focusHub->SetOnKeyEventInternal(std::move(onKeyEvent));
}

bool MenuPattern::OnKeyEvent(const KeyEvent& event) const
{
    if (event.action != KeyAction::DOWN || IsMultiMenu() || IsDesktopMenu()) {
        return false;
    }
    if ((event.code == KeyCode::KEY_DPAD_LEFT || event.code == KeyCode::KEY_ESCAPE) &&
        (IsSubMenu() || IsSelectOverlaySubMenu())) {
        auto menuWrapper = GetMenuWrapper();
        CHECK_NULL_RETURN(menuWrapper, true);
        auto wrapperPattern = menuWrapper->GetPattern<MenuWrapperPattern>();
        CHECK_NULL_RETURN(wrapperPattern, true);
        wrapperPattern->HideSubMenu();
        return true;
    }
    return false;
}

void MenuPattern::RemoveParentHoverStyle()
{
    if (!IsSubMenu()) {
        return;
    }
    auto menuItemParent = GetParentMenuItem();
    CHECK_NULL_VOID(menuItemParent);
    auto menuItemPattern = menuItemParent->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(menuItemPattern);
    menuItemPattern->SetIsSubMenuShowed(false);
    menuItemPattern->OnHover(false);
}

void MenuPattern::UpdateMenuItemChildren(RefPtr<UINode>& host)
{
    CHECK_NULL_VOID(host);
    auto layoutProperty = GetLayoutProperty<MenuLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    const auto& children = host->GetChildren();
    int32_t index = 0;
    for (auto child : children) {
        if (child->GetTag() == V2::MENU_ITEM_ETS_TAG) {
            auto itemNode = AceType::DynamicCast<FrameNode>(child);
            CHECK_NULL_VOID(itemNode);
            auto itemProperty = itemNode->GetLayoutProperty<MenuItemLayoutProperty>();
            CHECK_NULL_VOID(itemProperty);
            auto itemPattern = itemNode->GetPattern<MenuItemPattern>();
            CHECK_NULL_VOID(itemPattern);

            auto expandingMode = layoutProperty->GetExpandingMode().value_or(SubMenuExpandingMode::SIDE);
            if (expandingMode != itemPattern->GetExpandingMode() || IsEmbedded()) {
                auto expandNode = itemPattern->GetHost();
                CHECK_NULL_VOID(expandNode);
                expandNode->MarkModifyDone();
                expandNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            }
            UpdateMenuItemTextNode(layoutProperty, itemProperty, itemPattern);
            itemPattern->UpdateNeedDivider(isNeedDivider_);
            isNeedDivider_ = true;
            itemPattern->SetIndex(index);
        } else if (child->GetTag() == V2::MENU_ITEM_GROUP_ETS_TAG) {
            auto pattern = DynamicCast<FrameNode>(child)->GetPattern<MenuItemGroupPattern>();
            CHECK_NULL_VOID(pattern);
            pattern->ModifyDivider();
            auto itemGroupNode = AceType::DynamicCast<UINode>(child);
            CHECK_NULL_VOID(itemGroupNode);
            isNeedDivider_ = false;
            UpdateMenuItemChildren(itemGroupNode);
            isNeedDivider_ = false;
            auto accessibilityProperty =
                DynamicCast<FrameNode>(child)->GetAccessibilityProperty<AccessibilityProperty>();
            CHECK_NULL_VOID(accessibilityProperty);
            accessibilityProperty->SetAccessibilityLevel(AccessibilityProperty::Level::NO_STR);
        } else if (child->GetTag() == V2::JS_FOR_EACH_ETS_TAG || child->GetTag() == V2::JS_SYNTAX_ITEM_ETS_TAG
            ||  child->GetTag() == V2::JS_IF_ELSE_ETS_TAG || child->GetTag() == V2::JS_REPEAT_ETS_TAG) {
            auto nodesSet = AceType::DynamicCast<UINode>(child);
            CHECK_NULL_VOID(nodesSet);
            UpdateMenuItemChildren(nodesSet);
        } else {
            // do nothing
        }
        index++;
    }
}

void MenuPattern::UpdateSelectParam(const std::vector<SelectParam>& params)
{
    if (!isSelectMenu_) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    const auto& children = GetOptions();
    auto childCount = children.size();
    auto paramCount = params.size();
    size_t updateCount = std::min(paramCount, childCount);
    auto childIt = children.begin();
    for (size_t i = 0; i < updateCount; i++, childIt++) {
        const auto& childNode = AceType::DynamicCast<FrameNode>(*childIt);
        CHECK_NULL_VOID(childNode);
        if (i == 0) {
            auto props = childNode->GetPaintProperty<OptionPaintProperty>();
            CHECK_NULL_VOID(props);
            props->UpdateNeedDivider(false);
            auto focusHub = childNode->GetOrCreateFocusHub();
            CHECK_NULL_VOID(focusHub);
            focusHub->SetIsDefaultFocus(true);
        }
        auto optionPattern = childNode->GetPattern<OptionPattern>();
        CHECK_NULL_VOID(optionPattern);
        optionPattern->UpdateText(params.at(i).text);
        optionPattern->UpdateIcon(params.at(i).icon, params.at(i).symbolIcon);
        childNode->MarkModifyDone();
        childNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    }
    for (size_t i = updateCount; i < paramCount; i++) {
        auto optionNode = OptionView::CreateSelectOption(params.at(i), i);
        if (i == 0) {
            auto props = optionNode->GetPaintProperty<OptionPaintProperty>();
            props->UpdateNeedDivider(false);
        }
        MountOption(optionNode);
        optionNode->MarkModifyDone();
        optionNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    }
    for (size_t i = childCount; i > updateCount; i--) {
        RemoveOption();
    }
    host->MarkModifyDone();
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
}

void MenuPattern::HideMenu(bool isMenuOnTouch, OffsetF position) const
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    auto expandDisplay = theme->GetExpandDisplay();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto rootMenuPattern = AceType::DynamicCast<MenuPattern>(host->GetPattern());
    CHECK_NULL_VOID(rootMenuPattern);
    // copy menu pattern properties to rootMenu
    auto layoutProperty = rootMenuPattern->GetLayoutProperty<MenuLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    bool isShowInSubWindow = layoutProperty->GetShowInSubWindowValue(true);
    auto wrapper = GetMenuWrapper();
    CHECK_NULL_VOID(wrapper);
    if (wrapper->GetTag() == V2::SELECT_OVERLAY_ETS_TAG) {
        return;
    }
    if (((IsContextMenu() || (expandDisplay && isShowInSubWindow))) && (targetTag_ != V2::SELECT_ETS_TAG)) {
        TAG_LOGI(AceLogTag::ACE_MENU, "will hide menu, tagetNode id %{public}d.", targetId_);
        SubwindowManager::GetInstance()->HideMenuNG(wrapper, targetId_);
        return;
    }

    if (HideStackExpandMenu(position)) {
        return;
    }

    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    TAG_LOGI(AceLogTag::ACE_MENU, "will hide menu, tagetNode id %{public}d.", targetId_);
    overlayManager->HideMenu(wrapper, targetId_, isMenuOnTouch);
    overlayManager->EraseMenuInfo(targetId_);
}

bool MenuPattern::HideStackExpandMenu(const OffsetF& position) const
{
    auto wrapper = GetMenuWrapper();
    CHECK_NULL_RETURN(wrapper, false);
    auto outterMenu = wrapper->GetFirstChild();
    CHECK_NULL_RETURN(outterMenu, false);
    auto menuWrapperPattern = wrapper->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_RETURN(menuWrapperPattern, false);
    auto innerMenu = menuWrapperPattern->GetMenuChild(outterMenu);
    CHECK_NULL_RETURN(innerMenu, false);
    auto innerMenuPattern = AceType::DynamicCast<MenuPattern>(innerMenu->GetPattern());
    CHECK_NULL_RETURN(innerMenuPattern, false);
    auto layoutProps = innerMenuPattern->GetLayoutProperty<MenuLayoutProperty>();
    CHECK_NULL_RETURN(layoutProps, false);
    auto expandingMode = layoutProps->GetExpandingMode().value_or(SubMenuExpandingMode::SIDE);
    if (IsSubMenu() && expandingMode == SubMenuExpandingMode::STACK) {
        auto host = GetHost();
        CHECK_NULL_RETURN(host, false);
        auto hostZone = host->GetPaintRectOffset();
        auto scroll = host->GetFirstChild();
        CHECK_NULL_RETURN(scroll, false);
        auto column = scroll->GetFirstChild();
        CHECK_NULL_RETURN(column, false);
        auto clickAreaNode = AceType::DynamicCast<FrameNode>(column->GetFirstChild());
        CHECK_NULL_RETURN(clickAreaNode, false);
        auto clickAreaZone = clickAreaNode->GetGeometryNode()->GetFrameRect();
        clickAreaZone.SetLeft(hostZone.GetX());
        clickAreaZone.SetTop(hostZone.GetY());
        if (clickAreaZone.IsInRegion(PointF(position.GetX(), position.GetY()))) {
            HideStackMenu();
            return true;
        }
    } else if (expandingMode == SubMenuExpandingMode::STACK) {
        auto host = GetHost();
        CHECK_NULL_RETURN(host, false);
        auto hostZone = host->GetPaintRectOffset();
        auto clickAreaZone = host->GetGeometryNode()->GetFrameRect();
        clickAreaZone.SetLeft(hostZone.GetX());
        clickAreaZone.SetTop(hostZone.GetY());
        if (clickAreaZone.IsInRegion(PointF(position.GetX(), position.GetY()))) {
            auto wrapperPattern = wrapper->GetPattern<MenuWrapperPattern>();
            CHECK_NULL_RETURN(wrapperPattern, false);
            wrapperPattern->HideSubMenu();
            return true;
        }
    }
    return false;
}

void MenuPattern::HideStackMenu() const
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto wrapper = GetMenuWrapper();
    CHECK_NULL_VOID(wrapper);
    AnimationOption option;
    option.SetOnFinishEvent(
        [weak = WeakClaim(RawPtr(wrapper)), subMenuWk = WeakClaim(RawPtr(host))] {
            auto pipeline = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto taskExecutor = pipeline->GetTaskExecutor();
            CHECK_NULL_VOID(taskExecutor);
            taskExecutor->PostTask(
                [weak, subMenuWk]() {
                    auto subMenuNode = subMenuWk.Upgrade();
                    CHECK_NULL_VOID(subMenuNode);
                    auto menuWrapper = weak.Upgrade();
                    CHECK_NULL_VOID(menuWrapper);
                    menuWrapper->RemoveChild(subMenuNode);
                    menuWrapper->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
                },
                TaskExecutor::TaskType::UI, "HideStackMenu");
    });
    auto menuPattern = AceType::DynamicCast<MenuPattern>(host->GetPattern());
    if (menuPattern) {
        menuPattern->RemoveParentHoverStyle();
        auto frameNode = FrameNode::GetFrameNode(menuPattern->GetTargetTag(), menuPattern->GetTargetId());
        CHECK_NULL_VOID(frameNode);
        auto menuItem = frameNode->GetPattern<MenuItemPattern>();
        if (menuItem) {
            menuItem->SetIsSubMenuShowed(false);
        }
    }
    auto menuNode = AceType::DynamicCast<FrameNode>(wrapper->GetFirstChild());
    CHECK_NULL_VOID(menuNode);
    ShowStackExpandDisappearAnimation(menuNode, host, option);
}

void MenuPattern::HideSubMenu()
{
    if (!showedSubMenu_) {
        return;
    }
    auto subMenuPattern = showedSubMenu_->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(subMenuPattern);
    subMenuPattern->RemoveParentHoverStyle();

    auto menuItem = subMenuPattern->GetParentMenuItem();
    CHECK_NULL_VOID(menuItem);
    auto menuItemPattern = menuItem->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(menuItemPattern);
    menuItemPattern->SetIsSubMenuShowed(false);
    menuItemPattern->ClearHoverRegions();
    menuItemPattern->ResetWrapperMouseEvent();

    auto wrapper = GetMenuWrapper();
    CHECK_NULL_VOID(wrapper);
    wrapper->RemoveChild(showedSubMenu_);
    wrapper->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
    showedSubMenu_.Reset();
}

RefPtr<FrameNode> MenuPattern::GetMenuWrapper() const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    auto parent = host->GetParent();
    while (parent) {
        if (parent->GetTag() == V2::MENU_WRAPPER_ETS_TAG || parent->GetTag() == V2::SELECT_OVERLAY_ETS_TAG) {
            return AceType::DynamicCast<FrameNode>(parent);
        }
        parent = parent->GetParent();
    }
    return nullptr;
}

// search for inner <Menu> node, once found a <Menu> node, count the number of sibling <Menu>
uint32_t MenuPattern::GetInnerMenuCount() const
{
    if (type_ == MenuType::MULTI_MENU || type_ == MenuType::DESKTOP_MENU || IsSelectOverlayCustomMenu()) {
        return 0;
    }

    auto host = GetHost();
    CHECK_NULL_RETURN(host, 0);
    auto child = host->GetChildAtIndex(0);
    uint32_t depth = 0;
    while (child && depth < MAX_SEARCH_DEPTH) {
        // found component <Menu>
        if (child->GetTag() == V2::JS_VIEW_ETS_TAG) {
            child = child->GetFrameChildByIndex(0, false);
            if (child && child->GetTag() == V2::JS_VIEW_ETS_TAG) {
                child = child->GetChildAtIndex(0);
                ++depth;
            }
            continue;
        }
        if (child->GetTag() == V2::MENU_ETS_TAG) {
            auto parent = child->GetParent();
            CHECK_NULL_RETURN(parent, 0);
            return parent->GetChildren().size();
        }
        child = child->GetChildAtIndex(0);
        ++depth;
    }
    return 0;
}

RefPtr<FrameNode> MenuPattern::GetFirstInnerMenu() const
{
    if (type_ == MenuType::MULTI_MENU || type_ == MenuType::DESKTOP_MENU) {
        return nullptr;
    }

    auto host = GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    uint32_t depth = 0;
    auto child = host->GetChildAtIndex(0);
    while (child && depth < MAX_SEARCH_DEPTH) {
        // found component <Menu>
        if (child->GetTag() == V2::JS_VIEW_ETS_TAG) {
            child = child->GetFrameChildByIndex(0, false);
            if (child && child->GetTag() == V2::JS_VIEW_ETS_TAG) {
                child = child->GetChildAtIndex(0);
                ++depth;
            }
            continue;
        }
        if (child->GetTag() == V2::MENU_ETS_TAG) {
            return AceType::DynamicCast<FrameNode>(child);
        }
        child = child->GetChildAtIndex(0);
        ++depth;
    }
    return nullptr;
}

void MenuPattern::CopyMenuAttr(const RefPtr<FrameNode>& menuNode) const
{
    auto pattern = AceType::DynamicCast<MenuPattern>(menuNode->GetPattern());
    CHECK_NULL_VOID(pattern);

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto rootMenuPattern = AceType::DynamicCast<MenuPattern>(host->GetPattern());
    CHECK_NULL_VOID(rootMenuPattern);

    // copy menu pattern properties to rootMenu
    auto layoutProperty = pattern->GetLayoutProperty<MenuLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto rootMenuLayoutProperty = rootMenuPattern->GetLayoutProperty<MenuLayoutProperty>();
    CHECK_NULL_VOID(rootMenuLayoutProperty);
    if (layoutProperty->GetBorderRadius().has_value()) {
        rootMenuLayoutProperty->UpdateBorderRadius(layoutProperty->GetBorderRadiusValue());
    }
}

// mount option on menu
void MenuPattern::MountOption(const RefPtr<FrameNode>& option)
{
    auto column = GetMenuColumn();
    CHECK_NULL_VOID(column);
    auto pattern = option->GetPattern<OptionPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetMenu(GetHost());
    AddOptionNode(option);
    option->MountToParent(column);
}

// remove option from menu
void MenuPattern::RemoveOption()
{
    auto column = GetMenuColumn();
    CHECK_NULL_VOID(column);
    auto endOption = column->GetChildren().back();
    CHECK_NULL_VOID(endOption);
    column->RemoveChild(endOption);
    PopOptionNode();
}

RefPtr<FrameNode> MenuPattern::GetMenuColumn() const
{
    auto menu = GetHost();
    CHECK_NULL_RETURN(menu, nullptr);
    auto scroll = menu->GetChildren().front();
    CHECK_NULL_RETURN(scroll, nullptr);
    auto column = scroll->GetChildren().front();
    return DynamicCast<FrameNode>(column);
}

void MenuPattern::DisableTabInMenu()
{
    if (IsMultiMenu() || IsDesktopMenu()) {
        // multi menu not has scroll and column
        return;
    }
    // disable tab in menu
    auto column = GetMenuColumn();
    CHECK_NULL_VOID(column);
    auto columnFocusHub = column->GetOrCreateFocusHub();
    CHECK_NULL_VOID(columnFocusHub);

    auto onKeyEvent = [](const KeyEvent& event) -> bool {
        if (event.action != KeyAction::DOWN) {
            return false;
        }
        return event.code == KeyCode::KEY_TAB;
    };
    columnFocusHub->SetOnKeyEventInternal(std::move(onKeyEvent));
}

RefPtr<LayoutAlgorithm> MenuPattern::CreateLayoutAlgorithm()
{
    switch (type_) {
        case MenuType::MULTI_MENU:
        case MenuType::DESKTOP_MENU:
            return MakeRefPtr<MultiMenuLayoutAlgorithm>();
        case MenuType::SUB_MENU:
        case MenuType::SELECT_OVERLAY_SUB_MENU:
            return MakeRefPtr<SubMenuLayoutAlgorithm>();
        default:
            return MakeRefPtr<MenuLayoutAlgorithm>(targetId_, targetTag_, lastPosition_);
    }
}

bool MenuPattern::GetShadowFromTheme(ShadowStyle shadowStyle, Shadow& shadow)
{
    auto colorMode = SystemProperties::GetColorMode();
    if (shadowStyle == ShadowStyle::None) {
        return true;
    }
    auto host = GetHost();
    auto pipelineContext = host->GetContextRefPtr();
    CHECK_NULL_RETURN(pipelineContext, false);
    auto shadowTheme = pipelineContext->GetTheme<ShadowTheme>();
    CHECK_NULL_RETURN(shadowTheme, false);
    shadow = shadowTheme->GetShadow(shadowStyle, colorMode);
    return true;
}

void MenuPattern::ResetTheme(const RefPtr<FrameNode>& host, bool resetForDesktopMenu)
{
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto scroll = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(scroll);

    if (resetForDesktopMenu) {
        // DesktopMenu apply shadow on inner Menu node
        Shadow shadow;
        if (GetShadowFromTheme(ShadowStyle::None, shadow)) {
            renderContext->UpdateBackShadow(shadow);
        }
    } else {
        Shadow shadow;
        if (GetShadowFromTheme(ShadowStyle::OuterDefaultMD, shadow)) {
            renderContext->UpdateBackShadow(shadow);
        }
    }
    // to enable inner menu shadow effect for desktopMenu, need to remove clipping from container
    bool clip = !resetForDesktopMenu;
    scroll->GetRenderContext()->SetClipToBounds(clip);

    // move padding from scroll to inner menu
    auto scrollProp = scroll->GetLayoutProperty();
    scrollProp->UpdatePadding(PaddingProperty());
}

void MenuPattern::ResetScrollTheme(const RefPtr<FrameNode>& host)
{
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto scroll = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(scroll);
    scroll->GetRenderContext()->UpdateClipEdge(false);
}

void MenuPattern::InitTheme(const RefPtr<FrameNode>& host)
{
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);

    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    auto expandDisplay = theme->GetExpandDisplay();
    expandDisplay_ = expandDisplay;
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_ELEVEN) || !renderContext->IsUniRenderEnabled()) {
        auto bgColor = theme->GetBackgroundColor();
        renderContext->UpdateBackgroundColor(bgColor);
    }
    Shadow shadow;
    if (GetShadowFromTheme(ShadowStyle::OuterDefaultMD, shadow)) {
        renderContext->UpdateBackShadow(shadow);
    }
    // make menu round rect
    BorderRadiusProperty borderRadius;
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        borderRadius.SetRadius(theme->GetMenuDefaultRadius());
    } else {
        borderRadius.SetRadius(theme->GetMenuBorderRadius());
    }
    renderContext->UpdateBorderRadius(borderRadius);
}

void InnerMenuPattern::InitTheme(const RefPtr<FrameNode>& host)
{
    MenuPattern::InitTheme(host);
    // inner menu applies shadow in OnModifyDone(), where it can determine if it's a DesktopMenu or a regular menu

    auto layoutProperty = host->GetLayoutProperty();
    if (layoutProperty->GetPaddingProperty()) {
        // if user defined padding exists, skip applying default padding
        return;
    }
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    // apply default padding from theme on inner menu
    PaddingProperty padding;
    padding.SetEdges(CalcLength(theme->GetOutPadding()));
    host->GetLayoutProperty()->UpdatePadding(padding);

    host->GetRenderContext()->SetClipToBounds(true);
}

void MenuPattern::SetAccessibilityAction()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto accessibilityProperty = host->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetActionScrollForward([weakPtr = WeakClaim(this)]() {
        const auto& pattern = weakPtr.Upgrade();
        auto host = pattern->GetHost();
        CHECK_NULL_VOID(host);
        auto firstChild = DynamicCast<FrameNode>(host->GetChildAtIndex(0));
        CHECK_NULL_VOID(firstChild);
        if (firstChild && firstChild->GetTag() == V2::SCROLL_ETS_TAG) {
            auto scrollPattern = firstChild->GetPattern<ScrollPattern>();
            CHECK_NULL_VOID(scrollPattern);
            scrollPattern->ScrollPage(false, true);
        }
    });

    accessibilityProperty->SetActionScrollBackward([weakPtr = WeakClaim(this)]() {
        const auto& pattern = weakPtr.Upgrade();
        auto host = pattern->GetHost();
        CHECK_NULL_VOID(host);
        auto firstChild = DynamicCast<FrameNode>(host->GetChildAtIndex(0));
        CHECK_NULL_VOID(firstChild);
        if (firstChild && firstChild->GetTag() == V2::SCROLL_ETS_TAG) {
            auto scrollPattern = firstChild->GetPattern<ScrollPattern>();
            CHECK_NULL_VOID(scrollPattern);
            scrollPattern->ScrollPage(true, true);
        }
    });
}

Offset MenuPattern::GetTransformCenter() const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, Offset());
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, Offset());
    auto size = geometryNode->GetFrameSize();
    auto layoutAlgorithmWrapper = host->GetLayoutAlgorithm();
    CHECK_NULL_RETURN(layoutAlgorithmWrapper, Offset());
    auto layoutAlgorithm = AceType::DynamicCast<MenuLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithm, Offset());
    auto placement = layoutAlgorithm->GetPlacement();
    switch (placement) {
        case Placement::BOTTOM_LEFT:
        case Placement::RIGHT_TOP:
            return Offset();
        case Placement::BOTTOM_RIGHT:
        case Placement::LEFT_TOP:
            return Offset(size.Width(), 0.0f);
        case Placement::TOP_LEFT:
        case Placement::RIGHT_BOTTOM:
            return Offset(0.0f, size.Height());
        case Placement::TOP_RIGHT:
        case Placement::LEFT_BOTTOM:
            return Offset(size.Width(), size.Height());
        case Placement::BOTTOM:
            return Offset(size.Width() / 2, 0.0f);
        case Placement::LEFT:
            return Offset(size.Width(), size.Height() / 2);
        case Placement::TOP:
            return Offset(size.Width() / 2, size.Height());
        case Placement::RIGHT:
            return Offset(0.0f, size.Height() / 2);
        default:
            return Offset();
    }
}

void MenuPattern::ShowPreviewMenuScaleAnimation()
{
    auto menuWrapper = GetMenuWrapper();
    CHECK_NULL_VOID(menuWrapper);
    auto menuWrapperPattern = menuWrapper->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_VOID(menuWrapperPattern);

    auto preview = isShowHoverImage_ ? menuWrapperPattern->GetHoverImageFlexNode() : menuWrapperPattern->GetPreview();
    CHECK_NULL_VOID(preview);
    bool isHoverImageTarget = isShowHoverImage_ && preview->GetTag() == V2::FLEX_ETS_TAG;
    auto previewRenderContext = preview->GetRenderContext();
    CHECK_NULL_VOID(previewRenderContext);
    auto previewGeometryNode = preview->GetGeometryNode();
    CHECK_NULL_VOID(previewGeometryNode);
    auto previewPosition = previewGeometryNode->GetFrameOffset();
    OffsetF previewOriginPosition = GetPreviewOriginOffset();

    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto menuTheme = pipeline->GetTheme<NG::MenuTheme>();
    CHECK_NULL_VOID(menuTheme);
    auto springMotionResponse = menuTheme->GetSpringMotionResponse();
    auto springMotionDampingFraction = menuTheme->GetSpringMotionDampingFraction();
    auto delay = isHoverImageTarget ? menuTheme->GetHoverImageDelayDuration() : 0;

    previewRenderContext->UpdatePosition(
        OffsetT<Dimension>(Dimension(previewOriginPosition.GetX()), Dimension(previewOriginPosition.GetY())));
    AnimationOption scaleOption = AnimationOption();
    auto motion = AceType::MakeRefPtr<ResponsiveSpringMotion>(springMotionResponse, springMotionDampingFraction);
    if (isHoverImageTarget) {
        scaleOption.SetCurve(CUSTOM_PREVIEW_ANIMATION_CURVE);
    } else {
        scaleOption.SetCurve(motion);
    }
    scaleOption.SetDelay(delay);
    AnimationUtils::Animate(scaleOption, [previewRenderContext, previewPosition]() {
        CHECK_NULL_VOID(previewRenderContext);
        previewRenderContext->UpdatePosition(
            OffsetT<Dimension>(Dimension(previewPosition.GetX()), Dimension(previewPosition.GetY())));
    });
}

void MenuPattern::ShowPreviewMenuAnimation()
{
    CHECK_NULL_VOID(isFirstShow_ && previewMode_ != MenuPreviewMode::NONE);
    ShowPreviewMenuScaleAnimation();

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    MenuView::ShowPixelMapAnimation(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateTransformCenter(DimensionOffset(GetTransformCenter()));
    auto menuPosition = host->GetPaintRectOffset();

    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto menuTheme = pipeline->GetTheme<NG::MenuTheme>();
    CHECK_NULL_VOID(menuTheme);
    auto menuAnimationScale = menuTheme->GetMenuAnimationScale();
    auto springMotionResponse = menuTheme->GetSpringMotionResponse();
    auto springMotionDampingFraction = menuTheme->GetSpringMotionDampingFraction();

    renderContext->UpdateTransformScale(VectorF(menuAnimationScale, menuAnimationScale));

    renderContext->UpdatePosition(
        OffsetT<Dimension>(Dimension(originOffset_.GetX()), Dimension(originOffset_.GetY())));

    auto delay = isShowHoverImage_ ? menuTheme->GetHoverImageDelayDuration() : 0;
    ShowMenuOpacityAnimation(menuTheme, renderContext, delay);

    AnimationOption scaleOption = AnimationOption();
    auto motion = AceType::MakeRefPtr<ResponsiveSpringMotion>(springMotionResponse, springMotionDampingFraction);
    scaleOption.SetCurve(motion);
    scaleOption.SetDelay(delay);
    AnimationUtils::Animate(scaleOption, [renderContext, menuPosition]() {
        if (renderContext) {
            renderContext->UpdateTransformScale(VectorF(1.0f, 1.0f));
            renderContext->UpdatePosition(
                OffsetT<Dimension>(Dimension(menuPosition.GetX()), Dimension(menuPosition.GetY())));
        }
    });
    isFirstShow_ = false;
}

void MenuPattern::ShowMenuAppearAnimation()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (isMenuShow_ && Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE) &&
        previewMode_ == MenuPreviewMode::NONE) {
        auto renderContext = host->GetRenderContext();
        CHECK_NULL_VOID(renderContext);
        auto offset = GetTransformCenter();
        renderContext->UpdateTransformCenter(DimensionOffset(offset));
        auto menuPosition = host->GetPaintRectOffset();
        if (IsSelectOverlayExtensionMenu() && !isExtensionMenuShow_) {
            menuPosition = GetEndOffset();
        }
        if (IsSelectOverlayExtensionMenu()) {
            SetEndOffset(menuPosition);
        }

        renderContext->UpdateTransformScale(VectorF(MENU_ORIGINAL_SCALE, MENU_ORIGINAL_SCALE));
        renderContext->UpdateOpacity(0.0f);

        AnimationOption option = AnimationOption();
        option.SetCurve(MAIN_MENU_ANIMATION_CURVE);
        AnimationUtils::Animate(option, [this, renderContext, menuPosition]() {
            CHECK_NULL_VOID(renderContext);
            if (IsSelectOverlayExtensionMenu()) {
                renderContext->UpdatePosition(
                    OffsetT<Dimension>(Dimension(menuPosition.GetX()), Dimension(menuPosition.GetY())));
            }
            renderContext->UpdateOpacity(1.0f);
            renderContext->UpdateTransformScale(VectorF(1.0f, 1.0f));
        });
        isExtensionMenuShow_ = false;
    }
    isMenuShow_ = false;
}

void MenuPattern::ShowStackExpandMenu()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (!isSubMenuShow_) {
        return;
    }
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto menuWarpper = GetMenuWrapper();
    CHECK_NULL_VOID(menuWarpper);
    auto menuWrapperPattern = menuWarpper->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_VOID(menuWrapperPattern);
    auto outterMenu = menuWrapperPattern->GetMenu();
    CHECK_NULL_VOID(outterMenu);

    auto [originOffset, endOffset] = GetMenuOffset(outterMenu, true);
    auto outterMenuContext = outterMenu->GetRenderContext();
    CHECK_NULL_VOID(outterMenuContext);

    renderContext->UpdatePosition(
        OffsetT<Dimension>(Dimension(originOffset.GetX()), Dimension(originOffset.GetY())));

    AnimationOption opacityOption = AnimationOption();
    opacityOption.SetCurve(Curves::FRICTION);
    opacityOption.SetDuration(STACK_EXPAND_DISAPPEAR_DURATION);
    AnimationUtils::Animate(opacityOption, [renderContext, outterMenuContext]() {
        if (renderContext) {
            renderContext->UpdateOpacity(1.0f);
        }
        if (outterMenuContext) {
            outterMenuContext->UpdateOpacity(MOUNT_MENU_OPACITY);
        }
    });

    AnimationOption translateOption = AnimationOption();
    translateOption.SetCurve(STACK_MENU_CURVE);
    AnimationUtils::Animate(translateOption, [renderContext, menuPosition = endOffset, outterMenuContext]() {
        if (renderContext) {
            renderContext->UpdatePosition(
                OffsetT<Dimension>(Dimension(menuPosition.GetX()), Dimension(menuPosition.GetY())));
        }
        if (outterMenuContext) {
            outterMenuContext->UpdateTransformScale(VectorF(MOUNT_MENU_FINAL_SCALE, MOUNT_MENU_FINAL_SCALE));
        }
    });
    ShowArrowRotateAnimation();
    isSubMenuShow_ = false;
}

std::pair<OffsetF, OffsetF> MenuPattern::GetMenuOffset(const RefPtr<FrameNode>& outterMenu,
    bool isNeedRestoreNodeId) const
{
    CHECK_NULL_RETURN(outterMenu, std::make_pair(OffsetF(), OffsetF()));
    auto scroll = outterMenu->GetFirstChild();
    CHECK_NULL_RETURN(scroll, std::make_pair(OffsetF(), OffsetF()));
    auto innerMenu = scroll->GetFirstChild();
    CHECK_NULL_RETURN(innerMenu, std::make_pair(OffsetF(), OffsetF()));
    auto children = innerMenu->GetChildren();
    MenuItemInfo menuItemInfo;
    for (auto child : children) {
        menuItemInfo = GetInnerMenuOffset(child, isNeedRestoreNodeId);
        if (menuItemInfo.isFindTargetId) {
            break;
        }
    }
    return {menuItemInfo.originOffset, menuItemInfo.endOffset};
}

MenuItemInfo MenuPattern::GetInnerMenuOffset(const RefPtr<UINode>& child, bool isNeedRestoreNodeId) const
{
    MenuItemInfo menuItemInfo;
    CHECK_NULL_RETURN(child, menuItemInfo);
    if (child->GetTag() == V2::MENU_ITEM_ETS_TAG) {
        menuItemInfo = GetMenuItemInfo(child, isNeedRestoreNodeId);
        if (menuItemInfo.isFindTargetId) {
            return menuItemInfo;
        }
    } else {
        const auto& groupChildren = child->GetChildren();
        for (auto child : groupChildren) {
            menuItemInfo = GetInnerMenuOffset(child, isNeedRestoreNodeId);
            if (menuItemInfo.isFindTargetId) {
                return menuItemInfo;
            }
        }
    }
    return menuItemInfo;
}

MenuItemInfo MenuPattern::GetMenuItemInfo(const RefPtr<UINode>& child, bool isNeedRestoreNodeId) const
{
    MenuItemInfo menuItemInfo;
    auto menuItem = AceType::DynamicCast<FrameNode>(child);
    CHECK_NULL_RETURN(menuItem, menuItemInfo);
    if (menuItem->GetTag() == V2::MENU_ITEM_ETS_TAG) {
        auto menuItemPattern = menuItem->GetPattern<MenuItemPattern>();
        CHECK_NULL_RETURN(menuItemPattern, menuItemInfo);
        if (menuItem->GetId() == menuItemPattern->GetClickMenuItemId()) {
            auto offset = menuItem->GetPaintRectOffset();
            menuItemInfo.originOffset = offset - OffsetF(PADDING.ConvertToPx(), PADDING.ConvertToPx());
            auto menuItemFrameSize = menuItem->GetGeometryNode()->GetFrameSize();
            menuItemInfo.endOffset = menuItemInfo.originOffset + OffsetF(0.0f, menuItemFrameSize.Height());
            menuItemInfo.isFindTargetId = true;
            if (isNeedRestoreNodeId) {
                menuItemPattern->SetClickMenuItemId(-1);
            }
        }
    }
    return menuItemInfo;
}

void MenuPattern::ShowArrowRotateAnimation() const
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto subImageNode = GetImageNode(host);
    CHECK_NULL_VOID(subImageNode);
    auto subImageContext = subImageNode->GetRenderContext();
    CHECK_NULL_VOID(subImageContext);
    subImageContext->UpdateTransformRotate(Vector5F(0.0f, 0.0f, 1.0f, 0.0f, 0.0f));
    AnimationOption option = AnimationOption();
    option.SetCurve(MENU_ANIMATION_CURVE);
    AnimationUtils::Animate(option, [subImageContext]() {
        if (subImageContext) {
            subImageContext->UpdateTransformRotate(Vector5F(0.0f, 0.0f, 1.0f, SEMI_CIRCLE_ANGEL, 0.0f));
        }
    });
}

RefPtr<FrameNode> MenuPattern::GetImageNode(const RefPtr<FrameNode>& host) const
{
    auto scroll = host->GetFirstChild();
    CHECK_NULL_RETURN(scroll, nullptr);
    auto innerMenu = scroll->GetFirstChild();
    CHECK_NULL_RETURN(innerMenu, nullptr);
    auto menuItem = innerMenu->GetFirstChild();
    CHECK_NULL_RETURN(menuItem, nullptr);
    auto rightRow = menuItem->GetChildAtIndex(1);
    CHECK_NULL_RETURN(rightRow, nullptr);
    auto image = AceType::DynamicCast<FrameNode>(rightRow->GetChildren().back());
    return image;
}

void MenuPattern::ShowStackExpandDisappearAnimation(const RefPtr<FrameNode>& menuNode,
    const RefPtr<FrameNode>& subMenuNode, AnimationOption& option) const
{
    CHECK_NULL_VOID(menuNode);
    CHECK_NULL_VOID(subMenuNode);

    auto [originOffset, endOffset] = GetMenuOffset(menuNode, true);
    auto subMenuPos = subMenuNode->GetPaintRectOffset();
    auto menuPosition = OffsetF(subMenuPos.GetX(), originOffset.GetY());

    option.SetCurve(STACK_MENU_CURVE);
    AnimationUtils::Animate(option, [menuNode, menuPosition, subMenuNode]() {
        auto menuContext = menuNode->GetRenderContext();
        auto subMenuContext = subMenuNode->GetRenderContext();
        if (subMenuContext) {
            subMenuContext->UpdatePosition(
                OffsetT<Dimension>(Dimension(menuPosition.GetX()), Dimension(menuPosition.GetY())));
        }
        if (menuContext) {
            menuContext->UpdateTransformScale(VectorF(1.0f, 1.0f));
        }
    });

    option.SetCurve(MENU_ANIMATION_CURVE);
    auto subImageNode = GetImageNode(subMenuNode);
    AnimationUtils::Animate(option, [subImageNode]() {
        CHECK_NULL_VOID(subImageNode);
        auto subImageContext = subImageNode->GetRenderContext();
        CHECK_NULL_VOID(subImageContext);
        subImageContext->UpdateTransformRotate(Vector5F(0.0f, 0.0f, 1.0f, 0.0f, 0.0f));
    });

    option.SetCurve(Curves::FRICTION);
    option.SetDuration(STACK_EXPAND_DISAPPEAR_DURATION);
    AnimationUtils::Animate(option, [menuNode, subMenuNode]() {
        auto menuContext = menuNode->GetRenderContext();
        auto subMenuContext = subMenuNode->GetRenderContext();
        if (subMenuContext) {
            subMenuContext->UpdateOpacity(0.0f);
        }
        if (menuContext) {
            menuContext->UpdateOpacity(1.0f);
        }
    }, option.GetOnFinishEvent());
}

void MenuPattern::ShowMenuDisappearAnimation()
{
    if (!Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto menuContext = host->GetRenderContext();
    MAIN_MENU_ANIMATION_CURVE->UpdateMinimumAmplitudeRatio(MINIMUM_AMPLITUDE_RATION);
    auto menuPosition = GetEndOffset();
    AnimationOption option = AnimationOption();
    option.SetCurve(MAIN_MENU_ANIMATION_CURVE);
    AnimationUtils::Animate(option, [menuContext, menuPosition]() {
        if (menuContext) {
            menuContext->UpdatePosition(
                OffsetT<Dimension>(Dimension(menuPosition.GetX()), Dimension(menuPosition.GetY())));
            menuContext->UpdateTransformScale(VectorF(MENU_ORIGINAL_SCALE, MENU_ORIGINAL_SCALE));
            menuContext->UpdateOpacity(0.0f);
        }
    });
}

void MenuPattern::UpdateClipPath(const RefPtr<LayoutWrapper>& dirty)
{
    auto layoutAlgorithmWrapper = DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    CHECK_NULL_VOID(layoutAlgorithmWrapper);
    auto menuLayoutAlgorithm = DynamicCast<MenuLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_VOID(menuLayoutAlgorithm);
    auto clipPath = menuLayoutAlgorithm->GetClipPath();
    auto host = dirty->GetHostNode();
    CHECK_NULL_VOID(host);
    auto paintProperty = host->GetPaintProperty<MenuPaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    paintProperty->UpdateClipPath(clipPath);
}

bool MenuPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    ShowPreviewMenuAnimation();
    ShowMenuAppearAnimation();
    ShowStackExpandMenu();
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto menuPosition = host->GetPaintRectOffset();
    SetEndOffset(menuPosition);
    if (config.skipMeasure || dirty->SkipMeasureContent()) {
        return false;
    }

    UpdateClipPath(dirty);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_RETURN(theme, false);
    auto renderContext = dirty->GetHostNode()->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, false);

    auto menuProp = DynamicCast<MenuLayoutProperty>(dirty->GetLayoutProperty());
    CHECK_NULL_RETURN(menuProp, false);
    BorderRadiusProperty radius;
    auto defaultRadius = Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE) ?
        theme->GetMenuDefaultRadius() : theme->GetMenuBorderRadius();
    radius.SetRadius(defaultRadius);
    if (menuProp->GetBorderRadius().has_value()) {
        auto borderRadius = menuProp->GetBorderRadiusValue();
        auto geometryNode = dirty->GetGeometryNode();
        CHECK_NULL_RETURN(geometryNode, false);
        auto idealSize = geometryNode->GetMarginFrameSize();
        radius = CalcIdealBorderRadius(borderRadius, idealSize);
        UpdateBorderRadius(dirty->GetHostNode(), radius);
    }
    auto menuWrapper = GetMenuWrapper();
    DragAnimationHelper::ShowGatherAnimationWithMenu(menuWrapper);
    return true;
}

BorderRadiusProperty MenuPattern::CalcIdealBorderRadius(const BorderRadiusProperty& borderRadius, const SizeF& menuSize)
{
    Dimension defaultDimension(0);
    BorderRadiusProperty radius = { defaultDimension, defaultDimension, defaultDimension, defaultDimension };
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, radius);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_RETURN(theme, radius);
    auto defaultRadius = Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE) ?
        theme->GetMenuDefaultRadius() : theme->GetMenuBorderRadius();
    radius.SetRadius(defaultRadius);
    auto radiusTopLeft = borderRadius.radiusTopLeft.value_or(Dimension()).ConvertToPx();
    auto radiusTopRight = borderRadius.radiusTopRight.value_or(Dimension()).ConvertToPx();
    auto radiusBottomLeft = borderRadius.radiusBottomLeft.value_or(Dimension()).ConvertToPx();
    auto radiusBottomRight = borderRadius.radiusBottomRight.value_or(Dimension()).ConvertToPx();
    auto maxRadiusW = std::max(radiusTopLeft + radiusTopRight, radiusBottomLeft + radiusBottomRight);
    auto maxRadiusH = std::max(radiusTopLeft + radiusBottomLeft, radiusTopRight + radiusBottomRight);
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        if (LessOrEqual(maxRadiusW, menuSize.Width()) && LessOrEqual(maxRadiusH, menuSize.Height())) {
            radius = borderRadius;
        }
    } else {
        if (LessNotEqual(maxRadiusW, menuSize.Width())) {
            radius = borderRadius;
        }
    }

    return radius;
}

void MenuPattern::UpdateBorderRadius(const RefPtr<FrameNode>& menuNode, const BorderRadiusProperty& borderRadius)
{
    CHECK_NULL_VOID(menuNode);
    auto menuRenderContext = menuNode->GetRenderContext();
    CHECK_NULL_VOID(menuRenderContext);
    menuRenderContext->UpdateBorderRadius(borderRadius);
    if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
        menuRenderContext->UpdateOuterBorderRadius(borderRadius);
    }
    auto node = menuNode->GetChildren().front();
    CHECK_NULL_VOID(node);
    auto scrollNode = AceType::DynamicCast<FrameNode>(node);
    CHECK_NULL_VOID(scrollNode);
    auto scrollRenderContext = scrollNode->GetRenderContext();
    CHECK_NULL_VOID(scrollRenderContext);
    scrollRenderContext->UpdateBorderRadius(borderRadius);
}

void InnerMenuPattern::UpdateBorderRadius(const RefPtr<FrameNode>& menuNode, const BorderRadiusProperty& borderRadius)
{
    CHECK_NULL_VOID(menuNode);
    auto menuRenderContext = menuNode->GetRenderContext();
    CHECK_NULL_VOID(menuRenderContext);
    menuRenderContext->UpdateBorderRadius(borderRadius);
}

RefPtr<MenuPattern> MenuPattern::GetMainMenuPattern() const
{
    auto wrapperFrameNode = GetMenuWrapper();
    CHECK_NULL_RETURN(wrapperFrameNode, nullptr);
    auto mainMenuUINode = wrapperFrameNode->GetChildAtIndex(0);
    CHECK_NULL_RETURN(mainMenuUINode, nullptr);
    auto mainMenuFrameNode = AceType::DynamicCast<FrameNode>(mainMenuUINode);
    return mainMenuFrameNode->GetPattern<MenuPattern>();
}

void InnerMenuPattern::RecordItemsAndGroups()
{
    itemsAndGroups_.clear();
    auto host = GetHost();
    std::stack<RefPtr<UINode>> nodeStack;
    nodeStack.emplace(host);
    bool isMenu = true;

    while (!nodeStack.empty()) {
        auto currentNode = nodeStack.top();
        nodeStack.pop();
        // push items and item groups, skip menu node
        if (!isMenu && AceType::InstanceOf<FrameNode>(currentNode)) {
            itemsAndGroups_.emplace_back(WeakClaim(RawPtr(currentNode)));
            continue;
        }
        isMenu = false;
        // skip other type UiNode, such as ForEachNode
        for (int32_t index = static_cast<int32_t>(currentNode->GetChildren().size()) - 1; index >= 0; index--) {
            nodeStack.push(currentNode->GetChildAtIndex(index));
        }
    }
}

uint32_t InnerMenuPattern::FindSiblingMenuCount()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, 0);
    auto parent = host->GetParent();
    CHECK_NULL_RETURN(parent, 0);
    auto siblings = parent->GetChildren();
    uint32_t count = 0;
    for (auto&& sibling : siblings) {
        if (sibling->GetTag() == V2::MENU_ETS_TAG && sibling != host) {
            ++count;
        }
    }
    return count;
}

void InnerMenuPattern::ApplyDesktopMenuTheme()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    Shadow shadow;
    if (GetShadowFromTheme(ShadowStyle::OuterDefaultSM, shadow)) {
        host->GetRenderContext()->UpdateBackShadow(shadow);
    }
}

void InnerMenuPattern::ApplyMultiMenuTheme()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    Shadow shadow;
    if (GetShadowFromTheme(ShadowStyle::None, shadow)) {
        host->GetRenderContext()->UpdateBackShadow(shadow);
    }
}

void MenuPattern::OnColorConfigurationUpdate()
{
    auto host = GetHost();
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);

    auto menuTheme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(menuTheme);

    auto menuPattern = host->GetPattern<MenuPattern>();
    CHECK_NULL_VOID(menuPattern);

    auto renderContext = host->GetRenderContext();
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_ELEVEN) || !renderContext->IsUniRenderEnabled()) {
        renderContext->UpdateBackgroundColor(menuTheme->GetBackgroundColor());
    } else {
        renderContext->UpdateBackBlurStyle(renderContext->GetBackBlurStyle());
    }

    auto optionNode = menuPattern->GetOptions();
    for (const auto& child : optionNode) {
        auto optionsPattern = child->GetPattern<OptionPattern>();
        optionsPattern->SetFontColor(menuTheme->GetFontColor());

        child->MarkModifyDone();
        child->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    }
    host->SetNeedCallChildrenUpdate(false);
}

void MenuPattern::InitPanEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    CHECK_NULL_VOID(gestureHub);
    auto actionEndTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (pattern->IsMenuScrollable()) {
            return;
        }
        auto offsetX = static_cast<float>(info.GetOffsetX());
        auto offsetY = static_cast<float>(info.GetOffsetY());
        auto offsetPerSecondX = info.GetVelocity().GetOffsetPerSecond().GetX();
        auto offsetPerSecondY = info.GetVelocity().GetOffsetPerSecond().GetY();
        auto velocity =
            static_cast<float>(std::sqrt(offsetPerSecondX * offsetPerSecondX + offsetPerSecondY * offsetPerSecondY));
        pattern->HandleDragEnd(offsetX, offsetY, velocity);
    };
    auto actionScrollEndTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (pattern->IsMenuScrollable()) {
            return;
        }
        auto offsetX = static_cast<float>(info.GetOffsetX());
        auto offsetY = static_cast<float>(info.GetOffsetY());
        auto offsetPerSecondX = info.GetVelocity().GetOffsetPerSecond().GetX();
        auto offsetPerSecondY = info.GetVelocity().GetOffsetPerSecond().GetY();
        auto velocity =
            static_cast<float>(std::sqrt(offsetPerSecondX * offsetPerSecondX + offsetPerSecondY * offsetPerSecondY));
        pattern->HandleScrollDragEnd(offsetX, offsetY, velocity);
    };
    PanDirection panDirection;
    panDirection.type = PanDirection::ALL;
    auto panEvent = MakeRefPtr<PanEvent>(nullptr, nullptr, std::move(actionEndTask), nullptr);
    gestureHub->AddPanEvent(panEvent, panDirection, 1, DEFAULT_PAN_DISTANCE);
    gestureHub->AddPreviewMenuHandleDragEnd(std::move(actionScrollEndTask));
}

void MenuPattern::HandleDragEnd(float offsetX, float offsetY, float velocity)
{
    if ((LessOrEqual(std::abs(offsetY), std::abs(offsetX)) || LessOrEqual(offsetY, 0.0f)) &&
        LessOrEqual(velocity, PAN_MAX_VELOCITY)) {
        return;
    }
    auto menuWrapper = GetMenuWrapper();
    CHECK_NULL_VOID(menuWrapper);
    auto wrapperPattern = menuWrapper->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_VOID(wrapperPattern);
    TAG_LOGI(AceLogTag::ACE_DRAG, "will hide menu.");
    wrapperPattern->HideMenu();
}

void MenuPattern::HandleScrollDragEnd(float offsetX, float offsetY, float velocity)
{
    if ((LessOrEqual(std::abs(offsetY), std::abs(offsetX)) || !NearZero(offsetY)) &&
        LessOrEqual(velocity, PAN_MAX_VELOCITY)) {
        return;
    }
    auto menuWrapper = GetMenuWrapper();
    CHECK_NULL_VOID(menuWrapper);
    auto wrapperPattern = menuWrapper->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_VOID(wrapperPattern);
    TAG_LOGI(AceLogTag::ACE_DRAG, "will hide menu.");
    wrapperPattern->HideMenu();
}

void MenuPattern::DumpInfo()
{
    DumpLog::GetInstance().AddDesc(
        std::string("MenuType: ").append(std::to_string(static_cast<int32_t>(GetMenuType()))));
}

float MenuPattern::GetSelectMenuWidth()
{
    RefPtr<GridColumnInfo> columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::MENU);
    CHECK_NULL_RETURN(columnInfo, MIN_SELECT_MENU_WIDTH.ConvertToPx());
    auto parent = columnInfo->GetParent();
    CHECK_NULL_RETURN(parent, MIN_SELECT_MENU_WIDTH.ConvertToPx());
    parent->BuildColumnWidth();
    auto defaultWidth = static_cast<float>(columnInfo->GetWidth(COLUMN_NUM));
    float finalWidth = MIN_SELECT_MENU_WIDTH.ConvertToPx();

    if (IsWidthModifiedBySelect()) {
        auto menuLayoutProperty = GetLayoutProperty<MenuLayoutProperty>();
        auto selectmodifiedwidth = menuLayoutProperty->GetSelectMenuModifiedWidth();
        finalWidth = selectmodifiedwidth.value();
    } else {
        finalWidth = defaultWidth;
    }

    if (finalWidth < MIN_SELECT_MENU_WIDTH.ConvertToPx()) {
        finalWidth = defaultWidth;
    }

    return finalWidth;
}

void MenuPattern::OnItemPressed(const RefPtr<UINode>& parent, int32_t index, bool press, bool hover)
{
    CHECK_NULL_VOID(parent);
    if (parent->GetTag() == V2::MENU_ITEM_GROUP_ETS_TAG) {
        auto pattern = DynamicCast<FrameNode>(parent)->GetPattern<MenuItemGroupPattern>();
        CHECK_NULL_VOID(pattern);
        pattern->OnIntItemPressed(index, press);
    }
    RefPtr<UINode> nextNode = nullptr;
    if (parent->GetTag() == V2::JS_SYNTAX_ITEM_ETS_TAG) {
        nextNode = GetForEachMenuItem(parent, true);
    } else {
        const size_t size = parent->GetChildren().size();
        if (size == 0 || index >= static_cast<int32_t>(size - 1)) {
            return;
        }
        nextNode = parent->GetChildAtIndex(index + 1);
    }
    CHECK_NULL_VOID(nextNode);
    if (nextNode->GetTag() == V2::JS_FOR_EACH_ETS_TAG) {
        nextNode = GetForEachMenuItem(nextNode, true);
    }
    CHECK_NULL_VOID(nextNode);
    if (!InstanceOf<FrameNode>(nextNode)) {
        LOGW("next menuNode is not a frameNode! type = %{public}s", nextNode->GetTag().c_str());
        return;
    }
    if (nextNode->GetTag() == V2::MENU_ITEM_GROUP_ETS_TAG) {
        auto pattern = DynamicCast<FrameNode>(nextNode)->GetPattern<MenuItemGroupPattern>();
        CHECK_NULL_VOID(pattern);
        pattern->OnExtItemPressed(press, true);
    } else {
        auto props = DynamicCast<FrameNode>(nextNode)->GetPaintProperty<MenuItemPaintProperty>();
        CHECK_NULL_VOID(props);
        // need save needDivider property due to some items shoud not have divide in not pressed state
        hover ? props->UpdateHover(press) : props->UpdatePress(press);
        nextNode->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    }
    if (index > 0) {
        auto prevNode = parent->GetChildAtIndex(index - 1);
        CHECK_NULL_VOID(prevNode);
        if (!InstanceOf<FrameNode>(prevNode)) {
            LOGW("prev menuNode is not a frameNode! type = %{public}s", prevNode->GetTag().c_str());
            return;
        }
        if (prevNode->GetTag() == V2::MENU_ITEM_GROUP_ETS_TAG) {
            auto pattern = DynamicCast<FrameNode>(prevNode)->GetPattern<MenuItemGroupPattern>();
            CHECK_NULL_VOID(pattern);
            pattern->OnExtItemPressed(press, false);
        }
    }
}

RefPtr<UINode> MenuPattern::GetForEachMenuItem(const RefPtr<UINode>& parent, bool next)
{
    CHECK_NULL_RETURN(parent, nullptr);
    if (parent->GetTag() == V2::JS_SYNTAX_ITEM_ETS_TAG) {
        auto forEachNode = AceType::DynamicCast<UINode>(parent->GetParent());
        CHECK_NULL_RETURN(forEachNode, nullptr);
        auto syntIndex = forEachNode->GetChildIndex(parent);
        const size_t size = forEachNode->GetChildren().size();
        if (next) {
            if (size > 0 && syntIndex < static_cast<int32_t>(size - 1)) { // next is inside forEach
                auto nextSyntax = forEachNode->GetChildAtIndex(syntIndex + 1);
                CHECK_NULL_RETURN(nextSyntax, nullptr);
                return nextSyntax->GetFirstChild();
            } else { // next is after forEach
                return GetOutsideForEachMenuItem(forEachNode, true);
            }
        } else {
            if (syntIndex > 0) { // prev is inside forEach
                auto prevSyntax = forEachNode->GetChildAtIndex(syntIndex - 1);
                CHECK_NULL_RETURN(prevSyntax, nullptr);
                return prevSyntax->GetFirstChild();
            } else { // prev is before forEach
                return GetOutsideForEachMenuItem(forEachNode, false);
            }
        }
    }
    if (parent->GetTag() == V2::JS_FOR_EACH_ETS_TAG) {
        auto nextSyntax = next? parent->GetFirstChild(): parent->GetLastChild();
        CHECK_NULL_RETURN(nextSyntax, nullptr);
        return nextSyntax->GetFirstChild();
    }
    return nullptr;
}

RefPtr<UINode> MenuPattern::GetOutsideForEachMenuItem(const RefPtr<UINode>& forEachNode, bool next)
{
    auto parentForEachNode = AceType::DynamicCast<UINode>(forEachNode->GetParent());
    CHECK_NULL_RETURN(parentForEachNode, nullptr);
    auto forEachIndex = parentForEachNode->GetChildIndex(forEachNode);
    int32_t shift = next ? 1 : -1;
    const size_t size = parentForEachNode->GetChildren().size();
    if (size > 0 && (forEachIndex + shift) >= 0 && (forEachIndex + shift) <= static_cast<int32_t>(size - 1)) {
        return parentForEachNode->GetChildAtIndex(forEachIndex + shift);
    } else {
        return nullptr;
    }
}

bool MenuPattern::IsMenuScrollable() const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto firstChild = DynamicCast<FrameNode>(host->GetChildAtIndex(0));
    CHECK_NULL_RETURN(firstChild, false);
    if (firstChild->GetTag() == V2::SCROLL_ETS_TAG) {
        auto scrollPattern = firstChild->GetPattern<ScrollPattern>();
        CHECK_NULL_RETURN(scrollPattern, false);
        return scrollPattern->IsScrollable() && Positive(scrollPattern->GetScrollableDistance());
    }
    return false;
}

void MenuPattern::DumpInfo(std::unique_ptr<JsonValue>& json)
{
    json->Put("MenuType", static_cast<int32_t>(GetMenuType()));
}
} // namespace OHOS::Ace::NG

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

#include "core/components_ng/pattern/menu/menu_item/menu_item_model_ng.h"

#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_pattern.h"

namespace OHOS::Ace::NG {
void MenuItemModelNG::Create(const RefPtr<UINode>& customNode)
{
    auto* stack = ViewStackProcessor::GetInstance();
    int32_t nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::MENU_ITEM_ETS_TAG, nodeId);
    auto menuItem = FrameNode::GetOrCreateFrameNode(
        V2::MENU_ITEM_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<CustomMenuItemPattern>(); });
    CHECK_NULL_VOID(menuItem);
    stack->Push(menuItem);

    auto layoutProps = menuItem->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProps);
    layoutProps->UpdateAlignment(Alignment::CENTER_LEFT);
    // set border radius
    auto renderContext = menuItem->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    BorderRadiusProperty border;
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        border.SetRadius(theme->GetMenuDefaultInnerRadius());
    } else {
        border.SetRadius(theme->GetInnerBorderRadius());
    }
    renderContext->UpdateBorderRadius(border);

    CHECK_NULL_VOID(customNode);
    if (!menuItem->GetChildren().empty()) {
        menuItem->Clean();
    }
    menuItem->AddChild(customNode);
}

/*
 * The structure of menu item is designed as follows :
 * |--menu_item
 *   |--left_row
 *     |--icon
 *     |--content
 *   |--right_row
 *     |--label
 *     |--end_icon
 */
void MenuItemModelNG::Create(const MenuItemProperties& menuItemProps)
{
    auto* stack = ViewStackProcessor::GetInstance();
    int32_t nodeId = stack->ClaimNodeId();
    auto menuItem = FrameNode::GetOrCreateFrameNode(
        V2::MENU_ITEM_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<MenuItemPattern>(); });
    CHECK_NULL_VOID(menuItem);
    stack->Push(menuItem);

    // set border radius
    auto renderContext = menuItem->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    BorderRadiusProperty border;
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        border.SetRadius(theme->GetMenuDefaultInnerRadius());
    } else {
        border.SetRadius(theme->GetInnerBorderRadius());
    }
    renderContext->UpdateBorderRadius(border);

    if (menuItem->GetChildren().empty()) {
        auto leftRow = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
            AceType::MakeRefPtr<LinearLayoutPattern>(false));
        CHECK_NULL_VOID(leftRow);
        auto leftRowLayoutProps = leftRow->GetLayoutProperty<LinearLayoutProperty>();
        CHECK_NULL_VOID(leftRowLayoutProps);
        leftRowLayoutProps->UpdateMainAxisAlign(FlexAlign::FLEX_START);
        leftRowLayoutProps->UpdateCrossAxisAlign(FlexAlign::CENTER);
        leftRowLayoutProps->UpdateSpace(theme->GetIconContentPadding());

        leftRow->MountToParent(menuItem);
        auto rightRow = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
            AceType::MakeRefPtr<LinearLayoutPattern>(false));
        CHECK_NULL_VOID(rightRow);
        auto rightRowLayoutProps = rightRow->GetLayoutProperty<LinearLayoutProperty>();
        CHECK_NULL_VOID(rightRowLayoutProps);
        rightRowLayoutProps->UpdateMainAxisAlign(FlexAlign::CENTER);
        rightRowLayoutProps->UpdateCrossAxisAlign(FlexAlign::CENTER);
        rightRowLayoutProps->UpdateSpace(theme->GetIconContentPadding());

        rightRow->MountToParent(menuItem);
    }
    auto buildFunc = menuItemProps.buildFunc;
    auto pattern = menuItem->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(pattern);
    if (buildFunc.has_value()) {
        pattern->SetSubBuilder(buildFunc.value_or(nullptr));
    }

    UpdateMenuProperty(menuItem, menuItemProps);
}

void MenuItemModelNG::UpdateMenuProperty(const RefPtr<NG::FrameNode>& menuItem, const MenuItemProperties& menuItemProps)
{
    auto menuProperty = menuItem->GetLayoutProperty<MenuItemLayoutProperty>();
    CHECK_NULL_VOID(menuProperty);

    menuProperty->UpdateStartIcon(menuItemProps.startIcon.value_or(ImageSourceInfo("")));
    menuProperty->UpdateContent(menuItemProps.content);
    menuProperty->UpdateEndIcon(menuItemProps.endIcon.value_or(ImageSourceInfo("")));
    menuProperty->UpdateLabel(menuItemProps.labelInfo.value_or(""));
    menuProperty->SetStartSymbol(menuItemProps.startApply);
    menuProperty->SetEndSymbol(menuItemProps.endApply);
}

void MenuItemModelNG::SetSelected(bool isSelected)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSelected(isSelected);
    auto eventHub = frameNode->GetEventHub<MenuItemEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetCurrentUIState(UI_STATE_SELECTED, isSelected);
}

void MenuItemModelNG::SetSelectIcon(bool isShow)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, SelectIcon, isShow);
}

void MenuItemModelNG::SetSelectIconSrc(const std::string& src)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, SelectIconSrc, src);
}

void MenuItemModelNG::SetSelectIconSymbol(std::function<void(WeakPtr<NG::FrameNode>)>&& symbolApply)
{
    if (symbolApply != nullptr) {
        auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
        CHECK_NULL_VOID(frameNode);
        auto menuProperty = frameNode->GetLayoutProperty<MenuItemLayoutProperty>();
        menuProperty->SetSelectSymbol(symbolApply);
    }
}

void MenuItemModelNG::SetOnChange(std::function<void(bool)>&& onChange)
{
    auto eventHub = ViewStackProcessor::GetInstance()->GetMainFrameNodeEventHub<MenuItemEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChange(onChange);
}

void MenuItemModelNG::SetOnChange(FrameNode* frameNode, std::function<void(bool)>&& onChange)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<MenuItemEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChange(std::move(onChange));
}

void MenuItemModelNG::SetFontSize(const Dimension& fontSize)
{
    if (fontSize.IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontSize, fontSize);
    } else {
        ACE_RESET_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontSize);
    }
}

void MenuItemModelNG::SetFontWeight(FontWeight weight)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontWeight, weight);
}

void MenuItemModelNG::SetFontStyle(Ace::FontStyle style)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, ItalicFontStyle, style);
}

void MenuItemModelNG::SetFontColor(const std::optional<Color>& color)
{
    if (color.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontColor, color.value());
    } else {
        ACE_RESET_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontColor);
    }
}

void MenuItemModelNG::SetFontFamily(const std::vector<std::string> &families)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontFamily, families);
}

void MenuItemModelNG::SetLabelFontSize(const Dimension& fontSize)
{
    if (fontSize.IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontSize, fontSize);
    } else {
        ACE_RESET_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontSize);
    }
}

void MenuItemModelNG::SetLabelFontWeight(FontWeight weight)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontWeight, weight);
}

void MenuItemModelNG::SetLabelFontStyle(Ace::FontStyle style)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelItalicFontStyle, style);
}

void MenuItemModelNG::SetLabelFontColor(const std::optional<Color>& color)
{
    if (color.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontColor, color.value());
    } else {
        ACE_RESET_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontColor);
    }
}

void MenuItemModelNG::SetLabelFontFamily(const std::vector<std::string> &families)
{
    ACE_UPDATE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontFamily, families);
}

void MenuItemModelNG::SetSelectedChangeEvent(std::function<void(bool)>&& selectedChangeEvent)
{
    auto eventHub = ViewStackProcessor::GetInstance()->GetMainFrameNodeEventHub<MenuItemEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetSelectedChangeEvent(selectedChangeEvent);
}

void MenuItemModelNG::SetSelected(FrameNode* frameNode, bool isSelected)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<MenuItemPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSelected(isSelected);
    auto eventHub = frameNode->GetEventHub<MenuItemEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetCurrentUIState(UI_STATE_SELECTED, isSelected);
}

void MenuItemModelNG::SetLabelFontColor(FrameNode* frameNode, const std::optional<Color>& color)
{
    if (color.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontColor, color.value(), frameNode);
    } else {
        ACE_RESET_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontColor, frameNode);
    }
}

void MenuItemModelNG::SetFontColor(FrameNode* frameNode, const std::optional<Color>& color)
{
    if (color.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontColor, color.value(), frameNode);
    } else {
        ACE_RESET_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontColor, frameNode);
    }
}

void MenuItemModelNG::SetLabelFontSize(FrameNode* frameNode, const Dimension& fontSize)
{
    if (fontSize.IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontSize, fontSize, frameNode);
    } else {
        ACE_RESET_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontSize, frameNode);
    }
}

void MenuItemModelNG::SetLabelFontWeight(FrameNode* frameNode, FontWeight weight)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontWeight, weight, frameNode);
}

void MenuItemModelNG::SetLabelFontFamily(FrameNode* frameNode, const std::vector<std::string> &families)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelFontFamily, families, frameNode);
}

void MenuItemModelNG::SetLabelFontStyle(FrameNode* frameNode, Ace::FontStyle style)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, LabelItalicFontStyle, style, frameNode);
}

void MenuItemModelNG::SetFontSize(FrameNode* frameNode, const Dimension& fontSize)
{
    if (fontSize.IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontSize, fontSize, frameNode);
    } else {
        ACE_RESET_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontSize, frameNode);
    }
}

void MenuItemModelNG::SetFontWeight(FrameNode* frameNode, FontWeight weight)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontWeight, weight, frameNode);
}

void MenuItemModelNG::SetFontFamily(FrameNode* frameNode, const std::vector<std::string> &families)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, FontFamily, families, frameNode);
}

void MenuItemModelNG::SetFontStyle(FrameNode* frameNode, Ace::FontStyle style)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, ItalicFontStyle, style, frameNode);
}

void MenuItemModelNG::SetSelectIcon(FrameNode* frameNode, bool isShow)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, SelectIcon, isShow, frameNode);
}

void MenuItemModelNG::SetSelectIconSrc(FrameNode* frameNode, const std::string& src)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(MenuItemLayoutProperty, SelectIconSrc, src, frameNode);
}

void MenuItemModelNG::SetSelectIconSymbol(FrameNode* frameNode,
    std::function<void(WeakPtr<NG::FrameNode>)>&& symbolApply)
{
    if (symbolApply != nullptr) {
        auto menuProperty = frameNode->GetLayoutProperty<MenuItemLayoutProperty>();
        CHECK_NULL_VOID(menuProperty);
        menuProperty->SetSelectSymbol(symbolApply);
    }
}
} // namespace OHOS::Ace::NG

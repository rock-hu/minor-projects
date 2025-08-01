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

#include "core/components_ng/pattern/navrouter/navdestination_model_ng.h"

#include "base/i18n/localization.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/image/image_render_property.h"
#include "core/components_ng/pattern/navigation/navdestination_content_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_title_util.h"
#include "core/components_ng/pattern/navigation/navigation_toolbar_util.h"
#include "core/components_ng/pattern/navigation/title_bar_node.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navigation/tool_bar_node.h"
#include "core/components_ng/pattern/navigation/tool_bar_pattern.h"
#include "core/components_ng/pattern/navrouter/navdestination_group_node.h"
#include "core/components_ng/pattern/navrouter/navdestination_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "frameworks/base/system_bar/system_bar_style.h"

namespace OHOS::Ace::NG {
namespace {
void SetNeedResetTitleProperty(const RefPtr<FrameNode>& titleBarNode)
{
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->SetNeedResetMainTitleProperty(true);
    titleBarPattern->SetNeedResetSubTitleProperty(true);
}
} // namespace
bool NavDestinationModelNG::ParseCommonTitle(
    bool hasSubTitle, bool hasMainTitle, const std::string& subtitle, const std::string& title)
{
    if (!hasSubTitle && !hasMainTitle) {
        return false;
    }
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_RETURN(navDestinationNode, false);

    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, false);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_RETURN(titleBarPattern, false);
    if (navDestinationNode->GetPrevTitleIsCustomValue(false)) {
        // remove custom title if any.
        titleBarNode->RemoveChild(titleBarNode->GetTitle());
        titleBarNode->SetTitle(nullptr);
        auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
        CHECK_NULL_RETURN(titleBarLayoutProperty, false);
        // reset custom title height if any.
        if (titleBarLayoutProperty->HasTitleHeight()) {
            titleBarLayoutProperty->ResetTitleHeight();
        }
    }
    navDestinationNode->UpdatePrevTitleIsCustom(false);

    auto theme = NavigationGetTheme();
    // create or update main title
    auto mainTitle = AceType::DynamicCast<FrameNode>(titleBarNode->GetTitle());
    if (hasMainTitle) {
        if (mainTitle) {
            // update main title
            auto textLayoutProperty = mainTitle->GetLayoutProperty<TextLayoutProperty>();
            textLayoutProperty->UpdateMaxLines(hasSubTitle ? 1 : TITLEBAR_MAX_LINES);
            if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
                textLayoutProperty->UpdateHeightAdaptivePolicy(hasSubTitle ? TextHeightAdaptivePolicy::MAX_LINES_FIRST :
                    TextHeightAdaptivePolicy::MIN_FONT_SIZE_FIRST);
            }
            textLayoutProperty->UpdateContent(title);
            textLayoutProperty->UpdateMaxFontScale(STANDARD_FONT_SCALE);
        } else {
            // create and init main title
            mainTitle = FrameNode::CreateFrameNode(
                V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
            auto textLayoutProperty = mainTitle->GetLayoutProperty<TextLayoutProperty>();
            textLayoutProperty->UpdateContent(title);
            titleBarPattern->SetNeedResetMainTitleProperty(true);
            titleBarNode->SetTitle(mainTitle);
            titleBarNode->AddChild(mainTitle);
        }
    } else {
        // remove main title if any.
        titleBarNode->RemoveChild(mainTitle);
        titleBarNode->SetTitle(nullptr);
    }

    // create or update subtitle
    auto subTitle = AceType::DynamicCast<FrameNode>(titleBarNode->GetSubtitle());
    if (!hasSubTitle) {
        // remove subtitle if any.
        titleBarNode->RemoveChild(subTitle);
        titleBarNode->SetSubtitle(nullptr);
        return true;
    }
    if (subTitle) {
        // update subtitle
        auto textLayoutProperty = subTitle->GetLayoutProperty<TextLayoutProperty>();
        textLayoutProperty->UpdateContent(subtitle);
        textLayoutProperty->UpdateMaxFontScale(STANDARD_FONT_SCALE);
    } else {
        // create and init subtitle
        subTitle = FrameNode::CreateFrameNode(
            V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
        auto textLayoutProperty = subTitle->GetLayoutProperty<TextLayoutProperty>();
        textLayoutProperty->UpdateContent(subtitle);
        titleBarPattern->SetNeedResetSubTitleProperty(true);
        titleBarNode->SetSubtitle(subTitle);
        titleBarNode->AddChild(subTitle);
    }
    return true;
}

bool NavDestinationModelNG::ParseCommonTitle(bool hasSubTitle, bool hasMainTitle,
    const RefPtr<ResourceObject>& subResObj, const RefPtr<ResourceObject>& mainResObj)
{
    if (!hasSubTitle && !hasMainTitle) {
        return false;
    }
    std::string subtitle = "";
    std::string title = "";
    if (hasSubTitle) {
        ResourceParseUtils::ParseResString(subResObj, subtitle);
    }
    ResourceParseUtils::ParseResString(mainResObj, title);
    ParseCommonTitle(hasSubTitle, hasMainTitle, subtitle, title);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_RETURN(navDestinationNode, false);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, false);
    UpdateMainTitle(titleBarNode, mainResObj);
    UpdateSubTitle(titleBarNode, subResObj);
    return true;
}

void NavDestinationModelNG::UpdateMainTitle(
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& mainResObj)
{
    std::string key = "navDestination.title.commonMainTitle";
    auto updateFunc = [this, key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& mainResObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string title = titleBarPattern->GetResCacheMapByKey(key);
        if (title.empty()) {
            ResourceParseUtils::ParseResString(mainResObj, title);
            titleBarPattern->AddResCache(key, title);
        }
        auto mainTitle = AceType::DynamicCast<FrameNode>(titleBarNode->GetTitle());
        if (mainTitle) {
            auto textLayoutProperty = mainTitle->GetLayoutProperty<TextLayoutProperty>();
            textLayoutProperty->UpdateContent(title);
            titleBarNode->MarkModifyDone();
            titleBarNode->MarkDirtyNode();
        }
    };
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, mainResObj, std::move(updateFunc));
}

void NavDestinationModelNG::UpdateSubTitle(
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& subResObj)
{
    std::string key = "navDestination.title.commonSubTitle";
    auto updateFunc = [key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& resObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string subTitleValue = titleBarPattern->GetResCacheMapByKey(key);
        if (subTitleValue.empty()) {
            ResourceParseUtils::ParseResString(resObj, subTitleValue);
            titleBarPattern->AddResCache(key, subTitleValue);
        }
        auto subTitleNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetSubtitle());
        if (subTitleNode) {
            auto textLayoutProperty = subTitleNode->GetLayoutProperty<TextLayoutProperty>();
            textLayoutProperty->UpdateContent(subTitleValue);
            titleBarNode->MarkModifyDone();
            titleBarNode->MarkDirtyNode();
        }
    };
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, subResObj, std::move(updateFunc));
}

void CreateContentNode(const RefPtr<NavDestinationGroupNode>& navDestinationNode)
{
    if (navDestinationNode->GetContentNode()) {
        return;
    }
    int32_t contentNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::NAVDESTINATION_CONTENT_ETS_TAG, contentNodeId);
    auto contentNode = FrameNode::GetOrCreateFrameNode(V2::NAVDESTINATION_CONTENT_ETS_TAG, contentNodeId,
        []() { return AceType::MakeRefPtr<NavDestinationContentPattern>(true); });
    navDestinationNode->AddChild(contentNode);
    navDestinationNode->SetContentNode(contentNode);

    if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
        SafeAreaExpandOpts opts = { .type = SAFE_AREA_TYPE_SYSTEM | SAFE_AREA_TYPE_CUTOUT,
            .edges = SAFE_AREA_EDGE_ALL };
        contentNode->GetLayoutProperty()->UpdateSafeAreaExpandOpts(opts);
    }
}

void CreateToolBarNode(const RefPtr<NavDestinationGroupNode>& navDestinationNode)
{
    if (navDestinationNode->GetToolBarNode()) {
        return;
    }
    int32_t toolBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::TOOL_BAR_ETS_TAG, toolBarNodeId);
    auto toolBarNode = NavToolbarNode::GetOrCreateToolbarNode(
        V2::TOOL_BAR_ETS_TAG, toolBarNodeId, []() { return AceType::MakeRefPtr<NavToolbarPattern>(); });
    toolBarNode->MarkModifyDone();
    auto property = toolBarNode->GetLayoutProperty();
    CHECK_NULL_VOID(property);
    property->UpdateVisibility(VisibleType::GONE);
    navDestinationNode->AddChild(toolBarNode);
    navDestinationNode->SetToolBarNode(toolBarNode);
    navDestinationNode->SetPreToolBarNode(toolBarNode);
    navDestinationNode->UpdatePrevToolBarIsCustom(false);
}

RefPtr<NG::FrameNode> NavDestinationModelNG::CreateFrameNode(int32_t nodeId)
{
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::NAVDESTINATION_VIEW_ETS_TAG, nodeId);
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    return navDestinationNode;
}

void NavDestinationModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    // navDestination node
    int32_t nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::NAVDESTINATION_VIEW_ETS_TAG, nodeId);
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    if (!navDestinationNode->GetTitleBarNode()) {
        if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TEN)) {
            CreateImageButton(navDestinationNode);
        } else {
            CreateBackButton(navDestinationNode);
        }
        auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
        if (titleBarNode) {
            auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
            CHECK_NULL_VOID(titleBarLayoutProperty);
            titleBarLayoutProperty->UpdateTitleBarParentType(TitleBarParentType::NAV_DESTINATION);
        }
    }
    // content node
    if (!navDestinationNode->GetContentNode()) {
        int32_t contentNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::NAVDESTINATION_CONTENT_ETS_TAG, contentNodeId);
        auto contentNode = FrameNode::GetOrCreateFrameNode(V2::NAVDESTINATION_CONTENT_ETS_TAG, contentNodeId,
            []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        navDestinationNode->AddChild(contentNode);
        navDestinationNode->SetContentNode(contentNode);
    }
    CreateToolBarNode(navDestinationNode);
    stack->Push(navDestinationNode);
    RegisterBackgroundColorUpdateCallback(navDestinationNode);
}

void NavDestinationModelNG::CreateImageButton(const RefPtr<NavDestinationGroupNode>& navDestinationNode)
{
    int32_t titleBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::TITLE_BAR_ETS_TAG, titleBarNodeId);
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        V2::TITLE_BAR_ETS_TAG, titleBarNodeId, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    navDestinationNode->AddChild(titleBarNode);
    navDestinationNode->SetTitleBarNode(titleBarNode);

    int32_t backButtonNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto backButtonNode =
        FrameNode::CreateFrameNode(V2::BACK_BUTTON_ETS_TAG, backButtonNodeId, AceType::MakeRefPtr<ImagePattern>());
    auto focusHub = backButtonNode->GetOrCreateFocusHub();
    CHECK_NULL_VOID(focusHub);
    focusHub->SetFocusDependence(FocusDependence::SELF);
    titleBarNode->AddChild(backButtonNode);
    titleBarNode->SetBackButton(backButtonNode);

    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    ImageSourceInfo imageSourceInfo;
    imageSourceInfo.SetResourceId(theme->GetBackResourceId());
    auto backButtonLayoutProperty = backButtonNode->GetLayoutProperty<ImageLayoutProperty>();
    CHECK_NULL_VOID(backButtonLayoutProperty);
    backButtonLayoutProperty->UpdateImageSourceInfo(imageSourceInfo);
    auto imageRenderProperty = backButtonNode->GetPaintProperty<ImageRenderProperty>();
    CHECK_NULL_VOID(imageRenderProperty);
    imageRenderProperty->UpdateMatchTextDirection(true);
    backButtonNode->MarkModifyDone();
}

void CreateImageBackButton(RefPtr<FrameNode>& backButtonNode, RefPtr<TitleBarNode>& titleBarNode)
{
    auto backButtonImageNode = FrameNode::CreateFrameNode(V2::BACK_BUTTON_IMAGE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    CHECK_NULL_VOID(backButtonImageNode);

    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    ImageSourceInfo imageSourceInfo;
    imageSourceInfo.SetResourceId(theme->GetBackResourceId());
    auto backButtonImageLayoutProperty = backButtonImageNode->GetLayoutProperty<ImageLayoutProperty>();
    CHECK_NULL_VOID(backButtonImageLayoutProperty);
    backButtonImageLayoutProperty->UpdateImageSourceInfo(imageSourceInfo);
    backButtonImageLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);
    auto imageRenderProperty = backButtonImageNode->GetPaintProperty<ImageRenderProperty>();
    CHECK_NULL_VOID(imageRenderProperty);
    imageRenderProperty->UpdateMatchTextDirection(true);
    backButtonNode->AddChild(backButtonImageNode);
    backButtonImageNode->MarkModifyDone();
    backButtonNode->MarkModifyDone();
}

void CreateSymbolBackButton(RefPtr<FrameNode>& backButtonNode, RefPtr<TitleBarNode>& titleBarNode)
{
    auto symbolNode = FrameNode::GetOrCreateFrameNode(V2::SYMBOL_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<TextPattern>(); });
    CHECK_NULL_VOID(symbolNode);
    auto symbolProperty = symbolNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(symbolProperty);
    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    symbolProperty->UpdateSymbolSourceInfo(SymbolSourceInfo(theme->GetBackSymbolId()));
    symbolNode->MountToParent(backButtonNode);
    auto iconColor = theme->GetIconColor();
    symbolProperty->UpdateSymbolColorList({ iconColor });
    symbolNode->MarkDirtyNode();
    backButtonNode->MarkModifyDone();
}

void NavDestinationModelNG::CreateBackButton(const RefPtr<NavDestinationGroupNode>& navDestinationNode)
{
    int32_t titleBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::TITLE_BAR_ETS_TAG, titleBarNodeId);
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        V2::TITLE_BAR_ETS_TAG, titleBarNodeId, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    navDestinationNode->AddChild(titleBarNode);
    navDestinationNode->SetTitleBarNode(titleBarNode);

    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    int32_t backButtonNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto backButtonNode =
        FrameNode::CreateFrameNode(V2::BACK_BUTTON_ETS_TAG, backButtonNodeId, AceType::MakeRefPtr<ButtonPattern>());
    auto focusHub = backButtonNode->GetOrCreateFocusHub();
    CHECK_NULL_VOID(focusHub);
    focusHub->SetFocusDependence(FocusDependence::SELF);
    auto buttonPattern = backButtonNode->GetPattern<ButtonPattern>();
    CHECK_NULL_VOID(buttonPattern);
    buttonPattern->SetSkipColorConfigurationUpdate();
    buttonPattern->setComponentButtonType(ComponentButtonType::NAVIGATION);
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        buttonPattern->SetBlendColor(theme->GetBackgroundPressedColor(), theme->GetBackgroundHoverColor());
        buttonPattern->SetFocusBorderColor(theme->GetBackgroundFocusOutlineColor());
        buttonPattern->SetFocusBorderWidth(theme->GetBackgroundFocusOutlineWeight());
    }

    // read navdestination back button
    std::string message = theme->GetNavigationBack();
    NavigationTitleUtil::SetAccessibility(backButtonNode, message);
    
    titleBarNode->AddChild(backButtonNode);
    titleBarNode->SetBackButton(backButtonNode);
    auto backButtonLayoutProperty = backButtonNode->GetLayoutProperty<ButtonLayoutProperty>();
    CHECK_NULL_VOID(backButtonLayoutProperty);
    auto renderContext = backButtonNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    backButtonLayoutProperty->UpdateType(ButtonType::NORMAL);
    backButtonLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);

    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        backButtonLayoutProperty->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(theme->GetIconBackgroundWidth()), CalcLength(theme->GetIconBackgroundHeight())));
        backButtonLayoutProperty->UpdateBorderRadius(BorderRadiusProperty(theme->GetCornerRadius()));
        renderContext->UpdateBackgroundColor(theme->GetCompBackgroundColor());
        PaddingProperty padding;
        padding.SetEdges(CalcLength(MENU_BUTTON_PADDING));
        backButtonLayoutProperty->UpdatePadding(padding);
    } else {
        backButtonLayoutProperty->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(BACK_BUTTON_SIZE), CalcLength(BACK_BUTTON_SIZE)));
        backButtonLayoutProperty->UpdateBorderRadius(BorderRadiusProperty(BUTTON_RADIUS_SIZE));
        renderContext->UpdateBackgroundColor(Color::TRANSPARENT);
        PaddingProperty padding;
        padding.SetEdges(CalcLength(BUTTON_PADDING));
        backButtonLayoutProperty->UpdatePadding(padding);
    }

    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE) &&
        SystemProperties::IsNeedSymbol()) {
        CreateSymbolBackButton(backButtonNode, titleBarNode);
    } else {
        CreateImageBackButton(backButtonNode, titleBarNode);
    }
}

void NavDestinationModelNG::RegisterBackgroundColorUpdateCallback(
    const RefPtr<NavDestinationGroupNode>& navDestinationNode)
{
    if (SystemProperties::ConfigChangePerform()) {
        auto pattern = navDestinationNode->GetPattern<NavDestinationPattern>();
        CHECK_NULL_VOID(pattern);
        auto resourceObject = AceType::MakeRefPtr<ResourceObject>("", "", 0);
        auto updateFunc = [weakPattern = AceType::WeakClaim(AceType::RawPtr(pattern))](
                              const RefPtr<ResourceObject>& resObj) {
            auto pattern = weakPattern.Upgrade();
            CHECK_NULL_VOID(pattern);
            pattern->UpdateBackgroundColor();
        };
        pattern->AddResObj("navigation.navdesBGColorUpdate", resourceObject, std::move(updateFunc));
    }
}

void NavDestinationModelNG::Create(std::function<void()>&& deepRenderFunc, RefPtr<NG::NavDestinationContext> context)
{
    auto* stack = ViewStackProcessor::GetInstance();
    // navDestination node
    auto nodeId = stack->ClaimNodeId();
    auto deepRender = [nodeId, deepRenderFunc = std::move(deepRenderFunc)]() -> RefPtr<UINode> {
        CHECK_NULL_RETURN(deepRenderFunc, nullptr);
        auto parent = AceType::DynamicCast<UINode>(FrameNode::GetFrameNode(V2::NAVDESTINATION_VIEW_ETS_TAG, nodeId));
        auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(parent);
        if (navDestinationNode && navDestinationNode->GetContentNode()) {
            parent = navDestinationNode->GetContentNode();
        }
        ScopedViewStackProcessor scopedViewStackProcessor;
        ViewStackProcessor::GetInstance()->Push(parent);
        deepRenderFunc();
        ViewStackProcessor::GetInstance()->PopContainer();
        ViewStackProcessor::GetInstance()->Finish();
        return parent;
    };
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::NAVDESTINATION_VIEW_ETS_TAG, nodeId);
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(V2::NAVDESTINATION_VIEW_ETS_TAG, nodeId,
        [shallowBuilder = AceType::MakeRefPtr<ShallowBuilder>(std::move(deepRender)), context]() {
            auto pattern = AceType::MakeRefPtr<NavDestinationPattern>(shallowBuilder);
            pattern->SetNavDestinationContext(context);
            return pattern;
        });
    if (context) {
        context->SetUniqueId(navDestinationNode->GetId());
    }
    if (!navDestinationNode->GetTitleBarNode()) {
        if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TEN)) {
            CreateImageButton(navDestinationNode);
        } else {
            CreateBackButton(navDestinationNode);
        }
        auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
        if (titleBarNode) {
            auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
            CHECK_NULL_VOID(titleBarLayoutProperty);
            titleBarLayoutProperty->UpdateTitleBarParentType(TitleBarParentType::NAV_DESTINATION);
        }
    }
    // content node
    CreateContentNode(navDestinationNode);
    // toolbar node
    CreateToolBarNode(navDestinationNode);
    stack->Push(navDestinationNode);
    RegisterBackgroundColorUpdateCallback(navDestinationNode);
}

void NavDestinationModelNG::SetHideTitleBar(bool hideTitleBar, bool animated)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavDestinationLayoutProperty, HideTitleBar, hideTitleBar);
    ACE_UPDATE_LAYOUT_PROPERTY(NavDestinationLayoutProperty, IsAnimatedTitleBar, animated);
}

void NavDestinationModelNG::SetHideBackButton(bool hideBackButton)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    CHECK_NULL_VOID(navDestinationLayoutProperty);
    if (!navDestinationLayoutProperty->HasHideBackButton() ||
        (hideBackButton != navDestinationLayoutProperty->GetHideBackButtonValue())) {
        SetNeedResetTitleProperty(AceType::DynamicCast<FrameNode>(navDestinationGroupNode->GetTitleBarNode()));
    }
    navDestinationLayoutProperty->UpdateHideBackButton(hideBackButton);
}

void NavDestinationModelNG::SetTitle(const std::string& title, bool hasSubTitle)
{
    ParseCommonTitle(hasSubTitle, true, "", title);
}

void NavDestinationModelNG::SetTitlebarOptions(NavigationTitlebarOptions&& opt)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto navDestinationPattern = navDestinationNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    navDestinationPattern->SetTitleBarStyle(opt.brOptions.barStyle);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->SetTitlebarOptions(opt);
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>("", "", -1);
    auto updateFunc = [weakTitleBarNode = AceType::WeakClaim(AceType::RawPtr(titleBarNode)),
                          weakTitleBarPattern = AceType::WeakClaim(AceType::RawPtr(titleBarPattern)),
                          opt](const RefPtr<ResourceObject>& resObj) mutable {
        opt.bgOptions.ReloadResources();
        if (opt.bgOptions.blurStyleOption.has_value()) {
            opt.bgOptions.blurStyleOption->ReloadResources();
        }
        if (opt.bgOptions.effectOption.has_value()) {
            opt.bgOptions.effectOption->ReloadResources();
        }
        auto titleBarPattern = weakTitleBarPattern.Upgrade();
        CHECK_NULL_VOID(titleBarPattern);
        titleBarPattern->SetTitlebarOptions(opt);
        auto titleBarNode = weakTitleBarNode.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        titleBarNode->MarkModifyDone();
        titleBarNode->MarkDirtyNode();
    };
    auto pattern = titleBarNode->GetPattern();
    CHECK_NULL_VOID(pattern);
    pattern->AddResObj("navDestination.navigationTitlebarOptions", resObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetBackButtonIcon(const std::function<void(WeakPtr<NG::FrameNode>)>& symbolApply,
    const std::string& src, const ImageOption& imageOption, RefPtr<PixelMap>& pixMap,
    const std::vector<std::string>& nameList, bool userDefinedAccessibilityText,
    const std::string& backButtonAccessibilityText)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);

    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_VOID(titleBarLayoutProperty);
    ImageSourceInfo imageSourceInfo(src, nameList[0], nameList[1]);
    titleBarLayoutProperty->UpdateImageSource(imageSourceInfo);
    titleBarLayoutProperty->UpdateNoPixMap(imageOption.noPixMap);
    titleBarLayoutProperty->UpdatePixelMap(pixMap);
    titleBarLayoutProperty->SetBackIconSymbol(symbolApply);
    titleBarLayoutProperty->UpdateIsValidImage(imageOption.isValidImage);
    auto backButtonNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetBackButton());
    CHECK_NULL_VOID(backButtonNode);
    if (userDefinedAccessibilityText) {
        NavigationTitleUtil::SetAccessibility(backButtonNode, backButtonAccessibilityText);
    } else {
        auto theme = NavigationGetTheme();
        std::string message = theme ? theme->GetNavigationBack() : "";
        NavigationTitleUtil::SetAccessibility(backButtonNode, message);
    }
}

void NavDestinationModelNG::SetBackButtonIcon(const std::function<void(WeakPtr<NG::FrameNode>)>& symbolApply,
    const RefPtr<ResourceObject>& resObj, const ImageOption& imageOption, RefPtr<PixelMap>& pixMap,
    const std::vector<std::string>& nameList, bool userDefinedAccessibilityText,
    const std::string& backButtonAccessibilityText)
{
    std::string result;
    ResourceParseUtils::ParseResMedia(resObj, result);
    SetBackButtonIcon(symbolApply, result, imageOption, pixMap, nameList, userDefinedAccessibilityText,
        backButtonAccessibilityText);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    std::string key = "navDestination.backButtonIcon.icon";
    auto updateFunc = [nameList, key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& resObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string result = titleBarPattern->GetResCacheMapByKey(key);
        if (result.empty()) {
            ResourceParseUtils::ParseResMedia(resObj, result);
            titleBarPattern->AddResCache(key, result);
        }
        ImageSourceInfo imageSourceInfo(result, nameList[0], nameList[1]);
        auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
        CHECK_NULL_VOID(titleBarLayoutProperty);
        titleBarLayoutProperty->UpdateImageSource(imageSourceInfo);
        titleBarNode->MarkModifyDone();
        titleBarNode->MarkDirtyNode();
    };
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, resObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetBackButtonIconSrcAndTextRes(
    const std::function<void(WeakPtr<NG::FrameNode>)>& symbolApply,
    const RefPtr<ResourceObject>& backButtonIconResObj, const ImageOption& imageOption, RefPtr<PixelMap>& pixMap,
    const std::vector<std::string>& nameList, bool userDefinedAccessibilityText,
    const RefPtr<ResourceObject>& backButtonTextResObj)
{
    std::string result;
    ResourceParseUtils::ParseResMedia(backButtonIconResObj, result);
    std::string backButtonAccessibilityText;
    ResourceParseUtils::ParseResMedia(backButtonTextResObj, backButtonAccessibilityText);
    SetBackButtonIcon(symbolApply, result, imageOption, pixMap, nameList, true, backButtonAccessibilityText);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    UpdateBackButtonIcon(nameList, titleBarNode, backButtonIconResObj);
    UpdateBackButtonIconText(userDefinedAccessibilityText, titleBarNode, backButtonTextResObj);
}

void NavDestinationModelNG::UpdateBackButtonIcon(const std::vector<std::string>& nameList,
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& backButtonIconResObj)
{
    std::string key = "navDestination.backButtonIcon.icon";
    auto updateFunc = [nameList, key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& backButtonIconResObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string result = titleBarPattern->GetResCacheMapByKey(key);
        if (result.empty()) {
            ResourceParseUtils::ParseResMedia(backButtonIconResObj, result);
            titleBarPattern->AddResCache(key, result);
        }
        ImageSourceInfo imageSourceInfo(result, nameList[0], nameList[1]);
        auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
        CHECK_NULL_VOID(titleBarLayoutProperty);
        titleBarLayoutProperty->UpdateImageSource(imageSourceInfo);
        titleBarNode->MarkModifyDone();
        titleBarNode->MarkDirtyNode();
    };
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, backButtonIconResObj, std::move(updateFunc));
}

void NavDestinationModelNG::UpdateBackButtonIconText(bool userDefinedAccessibilityText,
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& backButtonTextResObj)
{
    std::string key = "navDestination.backButtonIcon.accessibilityText";
    auto updateFunc = [userDefinedAccessibilityText, key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& backButtonTextResObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto backButtonNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetBackButton());
        CHECK_NULL_VOID(backButtonNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string backButtonAccessibilityText = titleBarPattern->GetResCacheMapByKey(key);
        if (backButtonAccessibilityText.empty()) {
            ResourceParseUtils::ParseResString(backButtonTextResObj, backButtonAccessibilityText);
            titleBarPattern->AddResCache(key, backButtonAccessibilityText);
        }
        if (userDefinedAccessibilityText) {
            NavigationTitleUtil::SetAccessibility(backButtonNode, backButtonAccessibilityText);
        } else {
            std::string message = Localization::GetInstance()->GetEntryLetters("navigation.back");
            NavigationTitleUtil::SetAccessibility(backButtonNode, message);
        }
        titleBarNode->MarkModifyDone();
        titleBarNode->MarkDirtyNode();
    };
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, backButtonTextResObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetBackButtonIconTextRes(const std::function<void(WeakPtr<NG::FrameNode>)>& symbolApply,
    const std::string& src, const ImageOption& imageOption, RefPtr<PixelMap>& pixMap,
    const std::vector<std::string>& nameList, bool userDefinedAccessibilityText,
    const RefPtr<ResourceObject>& resObj)
{
    std::string backButtonAccessibilityText;
    ResourceParseUtils::ParseResString(resObj, backButtonAccessibilityText);
    SetBackButtonIcon(symbolApply, src, imageOption, pixMap, nameList, true, backButtonAccessibilityText);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    std::string key = "navDestination.backButtonIcon.accessibilityText";
    auto updateFunc = [userDefinedAccessibilityText, key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& resObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto backButtonNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetBackButton());
        CHECK_NULL_VOID(backButtonNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string backButtonAccessibilityText = titleBarPattern->GetResCacheMapByKey(key);
        if (backButtonAccessibilityText.empty()) {
            ResourceParseUtils::ParseResString(resObj, backButtonAccessibilityText);
            titleBarPattern->AddResCache(key, backButtonAccessibilityText);
        }
        if (userDefinedAccessibilityText) {
            NavigationTitleUtil::SetAccessibility(backButtonNode, backButtonAccessibilityText);
        } else {
            std::string message = Localization::GetInstance()->GetEntryLetters("navigation.back");
            NavigationTitleUtil::SetAccessibility(backButtonNode, message);
        }
        titleBarNode->MarkModifyDone();
        titleBarNode->MarkDirtyNode();
    };
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, resObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetSubtitle(const std::string& subtitle)
{
    ParseCommonTitle(true, false, subtitle, "");
}

void NavDestinationModelNG::SetCustomTitle(const RefPtr<AceType>& customNode)
{
    auto customTitle = AceType::DynamicCast<NG::UINode>(customNode);
    CHECK_NULL_VOID(customTitle);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    if (!navDestinationNode->GetPrevTitleIsCustomValue(false)) {
        // remove main and subtitle if any.
        titleBarNode->RemoveChild(titleBarNode->GetTitle());
        titleBarNode->RemoveChild(titleBarNode->GetSubtitle());
        titleBarNode->SetTitle(nullptr);
        titleBarNode->SetSubtitle(nullptr);
    }
    navDestinationNode->UpdatePrevTitleIsCustom(true);
    auto currentTitle = titleBarNode->GetTitle();
    if (currentTitle && customTitle->GetId() == currentTitle->GetId()) {
        // do nothing
        return;
    }
    // update custom title
    titleBarNode->RemoveChild(currentTitle);
    titleBarNode->SetTitle(customTitle);
    titleBarNode->AddChild(customTitle);
    titleBarNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
}

void NavDestinationModelNG::SetTitleHeight(const Dimension& titleHeight, bool isValid)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);

    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationGroupNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_VOID(titleBarLayoutProperty);
    if (isValid) {
        titleBarLayoutProperty->UpdateTitleHeight(titleHeight);
    } else {
        titleBarLayoutProperty->ResetTitleHeight();
    }
}

void NavDestinationModelNG::SetTitleHeight(const Dimension& height, const RefPtr<ResourceObject>& resObj)
{
    SetTitleHeight(height);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);

    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationGroupNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_VOID(titleBarLayoutProperty);
    UpdateTitleHeight(titleBarNode, resObj);
}

CalcDimension NavDestinationModelNG::ParseTitleHeight(
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& resObj)
{
    CHECK_NULL_RETURN(titleBarNode, Dimension());
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_RETURN(titleBarPattern, Dimension());
    std::string key = "navDestination.title.customtitle";
    std::string heightString = titleBarPattern->GetResCacheMapByKey(key);
    std::string heightValue;
    if (heightString.empty() && ResourceParseUtils::ParseResString(resObj, heightValue)) {
        titleBarPattern->AddResCache(key, heightValue);
    } else {
        heightValue = heightString;
    }
    if (heightValue == NG::TITLE_MAIN_WITH_SUB) {
        return NG::DOUBLE_LINE_TITLEBAR_HEIGHT;
    }
    if (heightValue == NG::TITLE_MAIN) {
        return NG::SINGLE_LINE_TITLEBAR_HEIGHT;
    }

    CalcDimension height;
    if (heightString.empty()) {
        bool isValid = ResourceParseUtils::ParseResDimensionVpNG(resObj, height);
        titleBarPattern->AddResCache(key, height.ToString());
        if (!isValid || height.Value() < 0) {
            return Dimension();
        }
    } else {
        height = StringUtils::StringToCalcDimension(heightString);
    }
    return height;
}

void NavDestinationModelNG::UpdateTitleHeight(
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& resObj)
{
    auto updateFunc = [this, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& resObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
        CHECK_NULL_VOID(titleBarLayoutProperty);
        CalcDimension height = ParseTitleHeight(titleBarNode, resObj);
        titleBarLayoutProperty->UpdateTitleHeight(height);
        titleBarNode->MarkModifyDone();
        titleBarNode->MarkDirtyNode();
    };
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj("navDestination.title.customtitle", resObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetOnShown(std::function<void()>&& onShow)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnShown(onShow);
}

void NavDestinationModelNG::SetOnShown(FrameNode* frameNode, std::function<void()>&& onShow)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnShown(onShow);
}

void NavDestinationModelNG::SetOnHidden(std::function<void()>&& onHidden)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnHidden(onHidden);
}

void NavDestinationModelNG::SetOnHidden(FrameNode* frameNode, std::function<void()>&& onHidden)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnHidden(onHidden);
}

void NavDestinationModelNG::SetOnBackPressed(std::function<bool()>&& onBackPressed)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnBackPressed(onBackPressed);
}

void NavDestinationModelNG::SetOnBackPressed(FrameNode* frameNode, std::function<bool()>&& onBackPressed)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(frameNode->GetEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnBackPressed(onBackPressed);
}

void NavDestinationModelNG::SetOnReady(std::function<void(RefPtr<NavDestinationContext>)>&& onReady)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnReady(onReady);
}

void NavDestinationModelNG::SetOnReady(
    FrameNode* frameNode, std::function<void(RefPtr<NavDestinationContext>)>&& onReady)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(frameNode->GetEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnReady(onReady);
}

RefPtr<AceType> NavDestinationModelNG::CreateEmpty()
{
    ScopedViewStackProcessor processor;
    Create();
    auto uiNode = ViewStackProcessor::GetInstance()->Finish();
    uiNode->SetRemoveSilently(true);
    auto navigationNode = AceType::DynamicCast<NavDestinationGroupNode>(uiNode);
    CHECK_NULL_RETURN(navigationNode, uiNode);
    auto pattern = navigationNode->GetPattern<NavDestinationPattern>();
    auto context = AceType::MakeRefPtr<NavDestinationContext>();
    CHECK_NULL_RETURN(context, uiNode);
    context->SetIsEmpty(true);
    pattern->SetNavDestinationContext(context);
    return uiNode;
}

void NavDestinationModelNG::SetHideTitleBar(FrameNode* frameNode, bool hideTitleBar, bool animated)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(NavDestinationLayoutProperty, HideTitleBar, hideTitleBar, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(NavDestinationLayoutProperty, IsAnimatedTitleBar, animated, frameNode);
}

void NavDestinationModelNG::SetHideBackButton(FrameNode* frameNode, bool hideBackButton)
{
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    CHECK_NULL_VOID(navDestinationLayoutProperty);
    if (!navDestinationLayoutProperty->HasHideBackButton() ||
        (hideBackButton != navDestinationLayoutProperty->GetHideBackButtonValue())) {
        SetNeedResetTitleProperty(AceType::DynamicCast<FrameNode>(navDestinationGroupNode->GetTitleBarNode()));
    }
    navDestinationLayoutProperty->UpdateHideBackButton(hideBackButton);
}

void NavDestinationModelNG::ResetResObj(FrameNode* frameNode, NavDestinationPatternType type, const std::string& key)
{
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    RefPtr<Pattern> pattern = nullptr;
    if (type == NavDestinationPatternType::TITLE_BAR) {
        auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
        CHECK_NULL_VOID(titleBarNode);
        pattern = titleBarNode->GetPattern<TitleBarPattern>();
    } else if (type == NavDestinationPatternType::NAV_DESTINATION) {
        pattern = navDestinationNode->GetPattern<NavDestinationPattern>();
    }
    CHECK_NULL_VOID(pattern);
    pattern->RemoveResObj(key);
}

void NavDestinationModelNG::SetBackButtonIcon(
    FrameNode* frameNode, const std::string& src, bool noPixMap, RefPtr<PixelMap>& pixMap)
{
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);

    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_VOID(titleBarLayoutProperty);
    ImageSourceInfo imageSourceInfo(src);
    titleBarLayoutProperty->UpdateImageSource(imageSourceInfo);
    titleBarLayoutProperty->UpdateNoPixMap(noPixMap);
    titleBarLayoutProperty->UpdatePixelMap(pixMap);
}

void NavDestinationModelNG::SetBackButtonIcon(
    FrameNode* frameNode, bool noPixMap, RefPtr<PixelMap>& pixMap, const RefPtr<ResourceObject>& backButtonIconResObj)
{
    CHECK_NULL_VOID(frameNode);
    std::string result;
    ResourceParseUtils::ParseResMedia(backButtonIconResObj, result);
    SetBackButtonIcon(frameNode, result, noPixMap, pixMap);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    std::string key = "navDestination.backButtonIcon.icon";
    auto updateFunc = [key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& backButtonIconResObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarPattern = titleBarNode->GetPattern();
        CHECK_NULL_VOID(titleBarPattern);
        std::string result = titleBarPattern->GetResCacheMapByKey(key);
        if (result.empty()) {
            ResourceParseUtils::ParseResMedia(backButtonIconResObj, result);
            titleBarPattern->AddResCache(key, result);
        }
        ImageSourceInfo imageSourceInfo(result);
        auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
        CHECK_NULL_VOID(titleBarLayoutProperty);
        titleBarLayoutProperty->UpdateImageSource(imageSourceInfo);
        titleBarNode->MarkModifyDone();
        titleBarNode->MarkDirtyNode();
    };
    auto titleBarPattern = titleBarNode->GetPattern();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, backButtonIconResObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetNavDestinationMode(FrameNode* frameNode, NavDestinationMode mode)
{
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    navDestinationNode->SetNavDestinationMode(mode);
}

void NavDestinationModelNG::SetNavDestinationMode(NavDestinationMode mode)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    navDestination->SetNavDestinationMode(mode);
}

void NavDestinationModelNG::SetRecoverable(FrameNode* frameNode, bool recoverable)
{
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    navDestination->SetRecoverable(recoverable);
}

void NavDestinationModelNG::SetRecoverable(bool recoverable)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    navDestination->SetRecoverable(recoverable);
}

void NavDestinationModelNG::SetMenuItems(std::vector<NG::BarItem>&& menuItems)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    // if previous menu is custom, just remove it and create new menu, otherwise update old menu
    if (navDestinationGroupNode->GetPrevMenuIsCustom().value_or(false)) {
        navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
    } else {
        if (navDestinationGroupNode->GetMenu()) {
            navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
        } else {
            navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::ADD);
        }
    }
    auto navDestinationPattern = navDestinationGroupNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    navDestinationPattern->SetTitleBarMenuItems(menuItems);
    navDestinationPattern->SetMenuNodeId(ElementRegister::GetInstance()->MakeUniqueId());
    navDestinationPattern->SetLandscapeMenuNodeId(ElementRegister::GetInstance()->MakeUniqueId());
    navDestinationGroupNode->UpdatePrevMenuIsCustom(false);
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>("", "", -1);
    auto updateFunc = [wekFrameNode = AceType::WeakClaim(frameNode), menuItems](
                          const RefPtr<ResourceObject>& resObj) mutable {
        for (BarItem& item : menuItems) {
            item.ReloadResources();
        }
        auto frameNode = wekFrameNode.Upgrade();
        CHECK_NULL_VOID(frameNode);
        auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
        CHECK_NULL_VOID(navDestinationGroupNode);
        auto navDestinationPattern = navDestinationGroupNode->GetPattern<NavDestinationPattern>();
        CHECK_NULL_VOID(navDestinationPattern);
        navDestinationPattern->SetTitleBarMenuItems(menuItems);
        navDestinationGroupNode->MarkModifyDone();
        navDestinationGroupNode->MarkDirtyNode();
    };
    navDestinationPattern->AddResObj("navDestination.menuItems", resObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetCustomMenu(const RefPtr<AceType>& customNode)
{
    auto customMenu = AceType::DynamicCast<NG::UINode>(customNode);
    CHECK_NULL_VOID(customMenu);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    // if previous menu exists, remove it if their ids are not the same
    // if previous node is not custom, their ids must not be the same
    if (navDestinationGroupNode->GetMenu()) {
        if (customMenu->GetId() == navDestinationGroupNode->GetMenu()->GetId()) {
            navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        navDestinationGroupNode->SetMenu(customMenu);
        navDestinationGroupNode->UpdatePrevMenuIsCustom(true);
        navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
        return;
    }
    navDestinationGroupNode->SetMenu(customMenu);
    navDestinationGroupNode->UpdatePrevMenuIsCustom(true);
    navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::ADD);
}

void NavDestinationModelNG::SetMenuOptions(NavigationMenuOptions&& opt)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto navDestinationPattern = navDestinationNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    navDestinationPattern->SetMenuOptions(opt);
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>("", "", -1);
    auto updateFunc = [weakFrameNode = AceType::WeakClaim(frameNode),
                          weakNavDestinationPattern = AceType::WeakClaim(AceType::RawPtr(navDestinationPattern)),
                          opt](const RefPtr<ResourceObject>& resObj) mutable {
        opt.mbOptions.bgOptions.ReloadResources();
        if (opt.mbOptions.bgOptions.blurStyleOption.has_value()) {
            opt.mbOptions.bgOptions.blurStyleOption->ReloadResources();
        }
        if (opt.mbOptions.bgOptions.effectOption.has_value()) {
            opt.mbOptions.bgOptions.effectOption->ReloadResources();
        }
        auto navDestinationPattern = weakNavDestinationPattern.Upgrade();
        CHECK_NULL_VOID(navDestinationPattern);

        navDestinationPattern->SetMenuOptions(opt);
        auto frameNode = weakFrameNode.Upgrade();
        auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
        CHECK_NULL_VOID(navDestinationNode);
        navDestinationNode->MarkModifyDone();
        navDestinationNode->MarkDirtyNode();
    };
    navDestinationPattern->AddResObj("navDestination.navigationMenuOptions", resObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetBackgroundColor(
    const Color& color, bool isVaild, const RefPtr<ResourceObject>& backgroundColorResObj)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto navDestinationPattern = navDestinationNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    if (!isVaild) {
        navDestinationPattern->SetIsUserDefinedBgColor(false);
        return;
    }
    if (SystemProperties::ConfigChangePerform() && backgroundColorResObj) {
        ViewAbstract::SetBackgroundColorWithResourceObj(color, backgroundColorResObj);
    } else {
        ViewAbstract::SetBackgroundColor(color);
    }
    navDestinationPattern->SetIsUserDefinedBgColor(true);
}

void NavDestinationModelNG::SetBackgroundColor(
    FrameNode* frameNode, const Color& color, bool isVaild, const RefPtr<ResourceObject>& backgroundColorResObj)
{
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto navDestinationPattern = navDestinationNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    if (!isVaild) {
        navDestinationPattern->SetIsUserDefinedBgColor(false);
        return;
    }
    if (SystemProperties::ConfigChangePerform() && backgroundColorResObj) {
        ViewAbstract::SetBackgroundColor(frameNode, color, backgroundColorResObj);
    } else {
        ViewAbstract::SetBackgroundColor(frameNode, color);
    }
    
    navDestinationPattern->SetIsUserDefinedBgColor(true);
}

void NavDestinationModelNG::SetOnWillAppear(std::function<void()>&& willAppear)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillAppear(willAppear);
}

void NavDestinationModelNG::SetOnWillAppear(FrameNode* frameNode, std::function<void()>&& willAppear)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(frameNode->GetEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillAppear(willAppear);
}

void NavDestinationModelNG::SetOnWillHide(std::function<void()>&& willHide)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillHide(willHide);
}

void NavDestinationModelNG::SetOnWillHide(FrameNode* frameNode, std::function<void()>&& willHide)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(frameNode->GetEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillHide(willHide);
}

void NavDestinationModelNG::SetOnWillShow(std::function<void()>&& willShow)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillShow(willShow);
}

void NavDestinationModelNG::SetOnWillShow(FrameNode* frameNode, std::function<void()>&& willShow)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(frameNode->GetEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillShow(willShow);
}

void NavDestinationModelNG::SetOnWillDisAppear(std::function<void()>&& willDisAppear)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(
        frameNode->GetOrCreateEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillDisAppear(willDisAppear);
}

void NavDestinationModelNG::SetOnWillDisAppear(FrameNode* frameNode, std::function<void()>&& willDisAppear)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationEventHub = AceType::DynamicCast<NavDestinationEventHub>(frameNode->GetEventHub<EventHub>());
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnWillDisAppear(willDisAppear);
}

void NavDestinationModelNG::SetIgnoreLayoutSafeArea(const NG::IgnoreLayoutSafeAreaOpts& opts)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    auto navdestinationLayoutProperty = navDestination->GetLayoutProperty<NavDestinationLayoutProperty>();
    CHECK_NULL_VOID(navdestinationLayoutProperty);
    navdestinationLayoutProperty->UpdateIgnoreLayoutSafeAreaOpts(opts);
    auto content = AceType::DynamicCast<FrameNode>(navDestination->GetContentNode());
    CHECK_NULL_VOID(content);
    auto contentLayoutProperty = content->GetLayoutProperty();
    CHECK_NULL_VOID(contentLayoutProperty);
    contentLayoutProperty->UpdateIgnoreLayoutSafeAreaOpts(opts);
}

void NavDestinationModelNG::SetIgnoreLayoutSafeArea(FrameNode* frameNode, const NG::IgnoreLayoutSafeAreaOpts& opts)
{
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    auto navdestinationLayoutProperty = navDestination->GetLayoutProperty<NavDestinationLayoutProperty>();
    CHECK_NULL_VOID(navdestinationLayoutProperty);
    navdestinationLayoutProperty->UpdateIgnoreLayoutSafeArea(opts);
    auto content = AceType::DynamicCast<FrameNode>(navDestination->GetContentNode());
    CHECK_NULL_VOID(content);
    auto contentLayoutProperty = content->GetLayoutProperty();
    CHECK_NULL_VOID(contentLayoutProperty);
    contentLayoutProperty->UpdateIgnoreLayoutSafeAreaOpts(opts);
}

void NavDestinationModelNG::SetNavDestinationPathInfo(const std::string& moduleName, const std::string& pagePath)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    navDestination->SetNavDestinationPathInfo(moduleName, pagePath);
}

void NavDestinationModelNG::SetSystemBarStyle(const RefPtr<SystemBarStyle>& style)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    auto pattern = navDestination->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSystemBarStyle(style);
}

void NavDestinationModelNG::ResetResObj(NavDestinationPatternType type, const std::string& key)
{
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    RefPtr<Pattern> pattern = nullptr;
    if (type == NavDestinationPatternType::TITLE_BAR) {
        auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
        CHECK_NULL_VOID(titleBarNode);
        pattern = titleBarNode->GetPattern<TitleBarPattern>();
    } else if (type == NavDestinationPatternType::NAV_DESTINATION) {
        pattern = navDestinationNode->GetPattern<NavDestinationPattern>();
    }
    CHECK_NULL_VOID(pattern);
    pattern->RemoveResObj(key);
}

void NavDestinationModelNG::ParseCommonTitle(FrameNode* frameNode, const NG::NavigationTitleInfo& titleInfo)
{
    if (!titleInfo.hasSubTitle && !titleInfo.hasMainTitle) {
        return;
    }
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);

    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    if (navDestinationNode->GetPrevTitleIsCustomValue(false)) {
        // remove custom title if any.
        titleBarNode->RemoveChild(titleBarNode->GetTitle());
        titleBarNode->SetTitle(nullptr);
        auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
        CHECK_NULL_VOID(titleBarLayoutProperty);
        // reset custom title height if any.
        if (titleBarLayoutProperty->HasTitleHeight()) {
            titleBarLayoutProperty->ResetTitleHeight();
        }
    }
    navDestinationNode->UpdatePrevTitleIsCustom(false);

    auto theme = NavigationGetTheme();
    // create or update main title
    NavigationTitleUtil::CreateOrUpdateDestinationMainTitle(titleBarNode, titleInfo);

    // create or update subtitle
    NavigationTitleUtil::CreateOrUpdateDestinationSubtitle(titleBarNode, titleInfo);
    return;
}

void NavDestinationModelNG::ParseCommonTitle(
    FrameNode* frameNode, const RefPtr<ResourceObject>& titleResObj, const RefPtr<ResourceObject>& subtitleResObj)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    std::string subtitle = "";
    std::string title = "";
    bool hasSubTitle = false;
    bool hasMainTitle = false;
    if (subtitleResObj) {
        hasSubTitle = ResourceParseUtils::ParseResString(subtitleResObj, subtitle);
    }
    hasMainTitle = ResourceParseUtils::ParseResString(titleResObj, title);
    if (!hasSubTitle && !hasMainTitle) {
        return;
    }
    NG::NavigationTitleInfo ngTitleInfo = { hasSubTitle, hasMainTitle, subtitle, title };
    ParseCommonTitle(frameNode, ngTitleInfo);
    UpdateMainTitleInfo(titleBarNode, titleResObj);
    if (subtitleResObj) {
        UpdateSubTitleInfo(titleBarNode, subtitleResObj);
    }
    return;
}

void NavDestinationModelNG::UpdateMainTitleInfo(
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& mainResObj)
{
    CHECK_NULL_VOID(titleBarNode);
    std::string key = "navDestination.title.commonMainTitle";
    auto updateFunc = [key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& mainResObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string title = titleBarPattern->GetResCacheMapByKey(key);
        if (title.empty()) {
            ResourceParseUtils::ParseResString(mainResObj, title);
            titleBarPattern->AddResCache(key, title);
        }
        auto mainTitle = AceType::DynamicCast<FrameNode>(titleBarNode->GetTitle());
        if (mainTitle) {
            auto textLayoutProperty = mainTitle->GetLayoutProperty<TextLayoutProperty>();
            textLayoutProperty->UpdateContent(title);
            titleBarNode->MarkModifyDone();
            titleBarNode->MarkDirtyNode();
        }
    };
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, mainResObj, std::move(updateFunc));
}

void NavDestinationModelNG::UpdateSubTitleInfo(
    const RefPtr<NG::TitleBarNode>& titleBarNode, const RefPtr<ResourceObject>& subResObj)
{
    CHECK_NULL_VOID(titleBarNode);
    std::string key = "navDestination.title.commonSubTitle";
    auto updateFunc = [key, weak = AceType::WeakClaim(AceType::RawPtr(titleBarNode))](
                          const RefPtr<ResourceObject>& resObj) {
        auto titleBarNode = weak.Upgrade();
        CHECK_NULL_VOID(titleBarNode);
        auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_VOID(titleBarPattern);
        std::string subTitle = titleBarPattern->GetResCacheMapByKey(key);
        if (subTitle.empty()) {
            ResourceParseUtils::ParseResString(resObj, subTitle);
            titleBarPattern->AddResCache(key, subTitle);
        }
        auto subTitleNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetSubtitle());
        if (subTitleNode) {
            auto textLayoutProperty = subTitleNode->GetLayoutProperty<TextLayoutProperty>();
            textLayoutProperty->UpdateContent(subTitle);
            titleBarNode->MarkModifyDone();
            titleBarNode->MarkDirtyNode();
        }
    };
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->AddResObj(key, subResObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetTitlebarOptions(FrameNode* frameNode, NavigationTitlebarOptions&& opt)
{
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto navDestinationPattern = navDestinationNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    navDestinationPattern->SetTitleBarStyle(opt.brOptions.barStyle);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    CHECK_NULL_VOID(titleBarPattern);
    titleBarPattern->SetTitlebarOptions(opt);
}

void NavDestinationModelNG::SetHideToolBar(bool hideToolBar, bool animated)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    CHECK_NULL_VOID(navDestinationLayoutProperty);
    navDestinationLayoutProperty->UpdateHideToolBar(hideToolBar);
    navDestinationLayoutProperty->UpdateIsAnimatedToolBar(animated);
}

void NavDestinationModelNG::SetHideToolBar(FrameNode* frameNode, bool hideToolBar, bool animated)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    CHECK_NULL_VOID(navDestinationLayoutProperty);
    navDestinationLayoutProperty->UpdateHideToolBar(hideToolBar);
    navDestinationLayoutProperty->UpdateIsAnimatedToolBar(animated);
}

void NavDestinationModelNG::SetToolbarConfiguration(std::vector<NG::BarItem>&& toolBarItems, MoreButtonOptions&& opt)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navdestinationGroupNode =
        AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(navdestinationGroupNode);
    auto localOpt = opt;
    NavigationToolbarUtil::SetToolbarMoreButtonOptions(navdestinationGroupNode, std::move(opt));
    bool enabled = false;
    auto hub = navdestinationGroupNode->GetOrCreateEventHub<EventHub>();
    if (hub) {
        enabled = hub->IsEnabled();
    }
    FieldProperty fieldProperty;
    fieldProperty.parentId = navdestinationGroupNode->GetInspectorId().value_or("");
    fieldProperty.field = NG::DES_FIELD;
    auto localToolBarItems = toolBarItems;
    NavigationToolbarUtil::SetToolbarConfiguration(
        navdestinationGroupNode, std::move(toolBarItems), enabled, fieldProperty);
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>("", "", -1);
    auto updateFunc = [enabled, weak = AceType::WeakClaim(AceType::RawPtr(navdestinationGroupNode)), fieldProperty,
                          localToolBarItems, localOpt](const RefPtr<ResourceObject>& resObj) mutable {
        auto navdestinationGroupNode = weak.Upgrade();
        CHECK_NULL_VOID(navdestinationGroupNode);
        if (localOpt.bgOptions.blurStyleOption.has_value()) {
            localOpt.bgOptions.blurStyleOption->ReloadResources();
        }
        if (localOpt.bgOptions.effectOption.has_value()) {
            localOpt.bgOptions.effectOption->ReloadResources();
        }
        auto localOptCopy = localOpt;
        NavigationToolbarUtil::SetToolbarMoreButtonOptions(navdestinationGroupNode, std::move(localOptCopy));
        for (auto& item : localToolBarItems) {
            item.ReloadResources();
        }
        auto toolBarItemCopy = localToolBarItems;
        NavigationToolbarUtil::SetToolbarConfiguration(
            navdestinationGroupNode, std::move(toolBarItemCopy), enabled, fieldProperty);
        navdestinationGroupNode->MarkModifyDone();
        navdestinationGroupNode->MarkDirtyNode();
    };
    auto pattern = navdestinationGroupNode->GetPattern();
    CHECK_NULL_VOID(pattern);
    pattern->AddResObj("navDestination.toolbarConfiguration", resObj, std::move(updateFunc));
}

void NavDestinationModelNG::SetToolbarConfiguration(std::vector<NG::BarItem>&& toolBarItems)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navdestinationGroupNode =
        AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(navdestinationGroupNode);
    bool enabled = false;
    auto hub = navdestinationGroupNode->GetOrCreateEventHub<EventHub>();
    if (hub) {
        enabled = hub->IsEnabled();
    }
    FieldProperty fieldProperty;
    fieldProperty.parentId = navdestinationGroupNode->GetInspectorId().value_or("");
    fieldProperty.field = NG::DES_FIELD;
    NavigationToolbarUtil::SetToolbarConfiguration(
        navdestinationGroupNode, std::move(toolBarItems), enabled, fieldProperty);
}

void NavDestinationModelNG::SetCustomToolBar(const RefPtr<AceType>& customNode)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode =
        AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    NavigationToolbarUtil::SetCustomToolBar(navDestinationGroupNode, customNode);
}

void NavDestinationModelNG::SetHideItemText(bool isHideItemText)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode =
        AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(navDestinationGroupNode);
    NavigationToolbarUtil::SetHideItemText(navDestinationGroupNode, isHideItemText);
}

void NavDestinationModelNG::SetToolBarOptions(NavigationToolbarOptions&& opt)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode =
        AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(navDestinationGroupNode);
    NavigationToolbarUtil::SetToolbarOptions(navDestinationGroupNode, std::move(opt));
}

void NavDestinationModelNG::SetToolbarMorebuttonOptions(MoreButtonOptions&& opt)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode =
        AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(navDestinationGroupNode);
    NavigationToolbarUtil::SetToolbarMoreButtonOptions(navDestinationGroupNode, std::move(opt));
}

void NavDestinationModelNG::SetMenuItems(FrameNode* frameNode, std::vector<NG::BarItem>&& menuItems)
{
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    // if previous menu is custom, just remove it and create new menu, otherwise update old menu
    if (navDestinationGroupNode->GetPrevMenuIsCustom().value_or(false)) {
        navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
    } else {
        if (navDestinationGroupNode->GetMenu()) {
            navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
        } else {
            navDestinationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::ADD);
        }
    }
    auto navDestinationPattern = navDestinationGroupNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    navDestinationPattern->SetTitleBarMenuItems(menuItems);
    navDestinationPattern->SetMenuNodeId(ElementRegister::GetInstance()->MakeUniqueId());
    navDestinationPattern->SetLandscapeMenuNodeId(ElementRegister::GetInstance()->MakeUniqueId());
    navDestinationGroupNode->UpdatePrevMenuIsCustom(false);
}

void NavDestinationModelNG::SetMenuItemAction(FrameNode* frameNode, std::function<void()>&& action, uint32_t index)
{
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationPattern = navDestinationGroupNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    auto menuItems = navDestinationPattern->GetTitleBarMenuItems();
    if (menuItems.size() > index) {
        menuItems.at(index).action = action;
        navDestinationPattern->SetTitleBarMenuItems(menuItems);
    }
}

void NavDestinationModelNG::SetMenuItemSymbol(FrameNode* frameNode,
    std::function<void(WeakPtr<NG::FrameNode>)>&& symbol, uint32_t index)
{
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationPattern = navDestinationGroupNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    auto menuItems = navDestinationPattern->GetTitleBarMenuItems();
    if (menuItems.size() > index) {
        menuItems.at(index).iconSymbol = symbol;
        navDestinationPattern->SetTitleBarMenuItems(menuItems);
    }
}

void NavDestinationModelNG::SetSystemTransitionType(NG::NavigationSystemTransitionType type)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    navDestination->SetSystemTransitionType(type);
}

void NavDestinationModelNG::SetSystemTransitionType(FrameNode* frameNode, NG::NavigationSystemTransitionType type)
{
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestination);
    navDestination->SetSystemTransitionType(type);
}

void NavDestinationModelNG::SetScrollableProcessor(
    const std::function<RefPtr<NG::NavDestinationScrollableProcessor>()>& creator)
{
    CHECK_NULL_VOID(creator);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    auto pattern = node->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(pattern);
    if (!pattern->GetScrollableProcessor()) {
        auto processor = creator();
        if (processor) {
            processor->SetNodeId(node->GetId());
            processor->SetNavDestinationPattern(WeakPtr(pattern));
        }
        pattern->SetScrollableProcessor(processor);
    }
}

void NavDestinationModelNG::UpdateBindingWithScrollable(
    std::function<void(const RefPtr<NG::NavDestinationScrollableProcessor>& processor)>&& callback)
{
    CHECK_NULL_VOID(callback);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    auto pattern = node->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(pattern);
    auto processor = pattern->GetScrollableProcessor();
    callback(processor);
}

void NavDestinationModelNG::SetOnPop(std::function<void(const RefPtr<NavPathInfo>&)>&& onPop)
{
    if (!onPop) {
        return;
    }
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(
        ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(navDestination);
    auto pattern = navDestination->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(pattern);
    auto context = pattern->GetNavDestinationContext();
    CHECK_NULL_VOID(context);
    auto navPathInfo = context->GetNavPathInfo();
    CHECK_NULL_VOID(navPathInfo);
    onPop(navPathInfo);
}

void NavDestinationModelNG::SetCustomTitle(FrameNode* frameNode, const RefPtr<AceType>& customNode)
{
    auto customTitle = AceType::DynamicCast<NG::UINode>(customNode);
    CHECK_NULL_VOID(customTitle);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    if (!navDestinationNode->GetPrevTitleIsCustomValue(false)) {
        titleBarNode->RemoveChild(titleBarNode->GetTitle());
        titleBarNode->RemoveChild(titleBarNode->GetSubtitle());
        titleBarNode->SetTitle(nullptr);
        titleBarNode->SetSubtitle(nullptr);
    }
    navDestinationNode->UpdatePrevTitleIsCustom(true);
    auto currentTitle = titleBarNode->GetTitle();
    if (currentTitle && customTitle->GetId() == currentTitle->GetId()) {
        // do nothing
        return;
    }
    // update custom title
    titleBarNode->RemoveChild(currentTitle);
    titleBarNode->SetTitle(customTitle);
    titleBarNode->AddChild(customTitle);
    titleBarNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
}

RefPtr<FrameNode> NavDestinationModelNG::GetCustomTitle(FrameNode* frameNode)
{
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_RETURN(navDestinationGroupNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationGroupNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, nullptr);
    auto title = AceType::DynamicCast<FrameNode>(titleBarNode->GetTitle());
    return title;
}

void NavDestinationModelNG::SetTitleHeight(FrameNode* frameNode, const Dimension& titleHeight, bool isValid)
{
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationGroupNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_VOID(titleBarLayoutProperty);
    if (isValid) {
        titleBarLayoutProperty->UpdateTitleHeight(titleHeight);
    } else {
        titleBarLayoutProperty->ResetTitleHeight();
    }
}

void NavDestinationModelNG::SetOnCoordScrollStartAction(
    FrameNode* frameNode, std::function<void()>&& onCoordScrollStart)
{
    CHECK_NULL_VOID(onCoordScrollStart);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationEventHub = navDestinationGroupNode->GetOrCreateEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnCoordScrollStartAction(std::move(onCoordScrollStart));
}

void NavDestinationModelNG::SetOnCoordScrollUpdateAction(
    FrameNode* frameNode, std::function<void(float)>&& onCoordScrollUpdate)
{
    CHECK_NULL_VOID(onCoordScrollUpdate);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationEventHub = navDestinationGroupNode->GetOrCreateEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnCoordScrollUpdateAction(std::move(onCoordScrollUpdate));
}

void NavDestinationModelNG::SetOnCoordScrollEndAction(FrameNode* frameNode, std::function<void()>&& onCoordScrollEnd)
{
    CHECK_NULL_VOID(onCoordScrollEnd);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationEventHub = navDestinationGroupNode->GetOrCreateEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetOnCoordScrollEndAction(std::move(onCoordScrollEnd));
}

void NavDestinationModelNG::SetSystemBarStyle(FrameNode* frameNode, const RefPtr<SystemBarStyle>& style)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto pattern = navDestinationGroupNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSystemBarStyle(style);
}

void NavDestinationModelNG::SetCustomBackButtonNode(FrameNode* frameNode, FrameNode* backButtonNode)
{
    CHECK_NULL_VOID(frameNode);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationGroupNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto backButton = AceType::DynamicCast<UINode>(Referenced::Claim<FrameNode>(backButtonNode));
    titleBarNode->SetCustomBackButton(backButton);
}

void NavDestinationModelNG::SetOnActive(std::function<void(int32_t)>&& onActive)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnActive(onActive);
}

void NavDestinationModelNG::SetOnInactive(std::function<void(int32_t)>&& onInactive)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnInactive(onInactive);
}

void NavDestinationModelNG::SetCustomTransition(NG::NavDestinationTransitionDelegate&& transitionDelegate)
{
    CHECK_NULL_VOID(transitionDelegate);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    node->SetNavDestinationTransitionDelegate(std::move(transitionDelegate));
}

void NavDestinationModelNG::SetOnNewParam(NG::NavDestinationOnNewParamCallback&& onNewParamCallback)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnNewParam(std::move(onNewParamCallback));
}

void NavDestinationModelNG::SetPreferredOrientation(const std::optional<Orientation>& ori)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    node->SetOrientation(ori);
}

void NavDestinationModelNG::SetPreferredOrientation(FrameNode* frameNode, const std::optional<Orientation>& ori)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    node->SetOrientation(ori);
}

void NavDestinationModelNG::SetEnableStatusBar(const std::optional<std::pair<bool, bool>>& statusBar)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    node->SetStatusBarConfig(statusBar);
}

void NavDestinationModelNG::SetEnableStatusBar(
    FrameNode* frameNode, const std::optional<std::pair<bool, bool>>& statusBar)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    node->SetStatusBarConfig(statusBar);
}

void NavDestinationModelNG::SetEnableNavigationIndicator(const std::optional<bool>& navigationIndicator)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    node->SetNavigationIndicatorConfig(navigationIndicator);
}

void NavDestinationModelNG::SetEnableNavigationIndicator(
    FrameNode* frameNode, const std::optional<bool>& navigationIndicator)
{
    CHECK_NULL_VOID(frameNode);
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(Referenced::Claim<FrameNode>(frameNode));
    CHECK_NULL_VOID(node);
    node->SetNavigationIndicatorConfig(navigationIndicator);
}

void NavDestinationModelNG::SetBeforeCreateLayoutWrapperCallBack(
    FrameNode* frameNode, std::function<void()>&& beforeCreateLayoutWrapper)
{
    CHECK_NULL_VOID(beforeCreateLayoutWrapper);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_VOID(navDestinationGroupNode);
    auto navDestinationEventHub = navDestinationGroupNode->GetOrCreateEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(navDestinationEventHub);
    navDestinationEventHub->SetBeforeCreateLayoutWrapperCallBack(std::move(beforeCreateLayoutWrapper));
}
} // namespace OHOS::Ace::NG

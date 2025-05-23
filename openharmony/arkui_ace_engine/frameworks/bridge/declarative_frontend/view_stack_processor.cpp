/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/view_stack_processor.h"

#include <string>

#include "base/log/ace_trace.h"
#include "base/utils/system_properties.h"
#include "core/accessibility/accessibility_node.h"
#include "core/common/ace_application_info.h"
#include "core/components/button/button_component.h"
#include "core/components/common/layout/constants.h"
#include "core/components/form/form_component.h"
#include "core/components/grid_layout/grid_layout_item_component.h"
#include "core/components/remote_window/remote_window_component.h"
#include "core/components/scoring/scoring_component.h"
#include "core/components/text_field/text_field_component.h"
#include "core/components/video/video_component_v2.h"
#include "core/components/web/web_component.h"
#include "core/components/xcomponent/xcomponent_component.h"
#include "core/components_v2/list/list_item_component.h"
#include "core/components_v2/water_flow/water_flow_item_component.h"

namespace OHOS::Ace::Framework {
thread_local std::unique_ptr<ViewStackProcessor> ViewStackProcessor::instance = nullptr;

ViewStackProcessor* ViewStackProcessor::GetInstance()
{
    if (!instance) {
        instance.reset(new ViewStackProcessor);
    }
    return instance.get();
}

ViewStackProcessor::ViewStackProcessor()
{
    radioGroups_ = std::make_shared<JsPageRadioGroups>();
    checkboxGroups_ = std::make_shared<JsPageCheckboxGroups>();
}

std::shared_ptr<JsPageRadioGroups> ViewStackProcessor::GetRadioGroupComponent()
{
    return radioGroups_;
}

std::shared_ptr<JsPageCheckboxGroups> ViewStackProcessor::GetCheckboxGroupComponent()
{
    return checkboxGroups_;
}

RefPtr<ComposedComponent> ViewStackProcessor::GetRootComponent(const std::string& id, const std::string& name)
{
    auto& wrappingComponentsMap = componentsStack_.top();
    auto iter = wrappingComponentsMap.find("root");
    if (iter != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<ComposedComponent>(iter->second);
    }

    RefPtr<ComposedComponent> rootComponent = AceType::MakeRefPtr<OHOS::Ace::ComposedComponent>(id, name);
    wrappingComponentsMap.emplace("root", rootComponent);
    return rootComponent;
}

RefPtr<CoverageComponent> ViewStackProcessor::GetCoverageComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("coverage") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<CoverageComponent>(wrappingComponentsMap["coverage"]);
    }

    std::list<RefPtr<Component>> children;
    RefPtr<CoverageComponent> coverageComponent = AceType::MakeRefPtr<OHOS::Ace::CoverageComponent>(children);
    wrappingComponentsMap.emplace("coverage", coverageComponent);
    return coverageComponent;
}

#ifndef WEARABLE_PRODUCT
RefPtr<PopupComponentV2> ViewStackProcessor::GetPopupComponent(bool createNewComponent)
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("popup") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<PopupComponentV2>(wrappingComponentsMap["popup"]);
    }

    if (!createNewComponent) {
        return nullptr;
    }

    RefPtr<PopupComponentV2> popupComponent =
        AceType::MakeRefPtr<OHOS::Ace::PopupComponentV2>(V2::InspectorComposedComponent::GenerateId(), "popup");
    wrappingComponentsMap.emplace("popup", popupComponent);
    return popupComponent;
}
#endif

RefPtr<MenuComponent> ViewStackProcessor::GetMenuComponent(bool createNewComponent)
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("menu") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<MenuComponent>(wrappingComponentsMap["menu"]);
    }

    if (!createNewComponent) {
        return nullptr;
    }

    RefPtr<MenuComponent> menuComponent =
        AceType::MakeRefPtr<OHOS::Ace::MenuComponent>(V2::InspectorComposedComponent::GenerateId(), "menu");
    wrappingComponentsMap.emplace("menu", menuComponent);
    return menuComponent;
}

RefPtr<PositionedComponent> ViewStackProcessor::GetPositionedComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("position") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<PositionedComponent>(wrappingComponentsMap["position"]);
    }

    RefPtr<PositionedComponent> positionedComponent = AceType::MakeRefPtr<OHOS::Ace::PositionedComponent>();
    wrappingComponentsMap.emplace("position", positionedComponent);
    return positionedComponent;
}

RefPtr<FlexItemComponent> ViewStackProcessor::GetFlexItemComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("flexItem") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<FlexItemComponent>(wrappingComponentsMap["flexItem"]);
    }
    // flex values are all changed to 0.0 in Render.
    RefPtr<FlexItemComponent> flexItem = AceType::MakeRefPtr<OHOS::Ace::FlexItemComponent>(0.0, -1.0, 0.0);
    wrappingComponentsMap.emplace("flexItem", flexItem);
    return flexItem;
}

RefPtr<StepperItemComponent> ViewStackProcessor::GetStepperItemComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("stepperItem") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<StepperItemComponent>(wrappingComponentsMap["stepperItem"]);
    }

    RefPtr<StepperItemComponent> stepperItem = AceType::MakeRefPtr<StepperItemComponent>(RefPtr<Component>());
    wrappingComponentsMap.emplace("stepperItem", stepperItem);
    return stepperItem;
}

RefPtr<DisplayComponent> ViewStackProcessor::GetStepperDisplayComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("stepperDisplay") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<DisplayComponent>(wrappingComponentsMap["stepperDisplay"]);
    }

    RefPtr<DisplayComponent> stepperDisplay = AceType::MakeRefPtr<DisplayComponent>();
    wrappingComponentsMap.emplace("stepperDisplay", stepperDisplay);
    return stepperDisplay;
}

RefPtr<ScrollComponent> ViewStackProcessor::GetStepperScrollComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("stepperScroll") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<ScrollComponent>(wrappingComponentsMap["stepperScroll"]);
    }

    RefPtr<ScrollComponent> stepperScroll = AceType::MakeRefPtr<ScrollComponent>(RefPtr<Component>());
    wrappingComponentsMap.emplace("stepperScroll", stepperScroll);
    return stepperScroll;
}

RefPtr<BoxComponent> ViewStackProcessor::GetBoxComponent()
{
#if defined(PREVIEW)
    if (componentsStack_.empty()) {
        RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<OHOS::Ace::BoxComponent>();
        std::unordered_map<std::string, RefPtr<Component>> wrappingComponentsMap;
        wrappingComponentsMap.emplace("box", boxComponent);
        componentsStack_.push(wrappingComponentsMap);
        return boxComponent;
    }
#endif
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("box") != wrappingComponentsMap.end()) {
        auto boxComponent = AceType::DynamicCast<BoxComponent>(wrappingComponentsMap["box"]);
        if (boxComponent) {
            return boxComponent;
        }
    }

    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<OHOS::Ace::BoxComponent>();
    boxComponent->SetEnableDebugBoundary(true);
    wrappingComponentsMap.emplace("box", boxComponent);
    return boxComponent;
}

RefPtr<Component> ViewStackProcessor::GetMainComponent() const
{
    if (componentsStack_.empty()) {
        return nullptr;
    }
    auto& wrappingComponentsMap = componentsStack_.top();
    auto main = wrappingComponentsMap.find("main");
    if (main == wrappingComponentsMap.end()) {
        return nullptr;
    }
    return main->second;
}

bool ViewStackProcessor::HasDisplayComponent() const
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("display") != wrappingComponentsMap.end()) {
        return true;
    }
    return false;
}

RefPtr<DisplayComponent> ViewStackProcessor::GetDisplayComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("display") != wrappingComponentsMap.end()) {
        auto displayComponent = AceType::DynamicCast<DisplayComponent>(wrappingComponentsMap["display"]);
        if (displayComponent) {
            return displayComponent;
        }
    }

    RefPtr<DisplayComponent> displayComponent = AceType::MakeRefPtr<OHOS::Ace::DisplayComponent>();
    wrappingComponentsMap.emplace("display", displayComponent);
    return displayComponent;
}

RefPtr<TransformComponent> ViewStackProcessor::GetTransformComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("transform") != wrappingComponentsMap.end()) {
        auto transformComponent = AceType::DynamicCast<TransformComponent>(wrappingComponentsMap["transform"]);
        if (transformComponent) {
            return transformComponent;
        }
    }

    RefPtr<TransformComponent> transformComponent = AceType::MakeRefPtr<OHOS::Ace::TransformComponent>();
    wrappingComponentsMap.emplace("transform", transformComponent);
    return transformComponent;
}

bool ViewStackProcessor::HasTouchListenerComponent() const
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("touch") != wrappingComponentsMap.end()) {
        return true;
    }
    return false;
}

RefPtr<TouchListenerComponent> ViewStackProcessor::GetTouchListenerComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("touch") != wrappingComponentsMap.end()) {
        auto touchListenerComponent = AceType::DynamicCast<TouchListenerComponent>(wrappingComponentsMap["touch"]);
        if (touchListenerComponent) {
            return touchListenerComponent;
        }
    }

    RefPtr<TouchListenerComponent> touchComponent = AceType::MakeRefPtr<OHOS::Ace::TouchListenerComponent>();
    wrappingComponentsMap.emplace("touch", touchComponent);
    return touchComponent;
}

RefPtr<MouseListenerComponent> ViewStackProcessor::GetMouseListenerComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("mouse") != wrappingComponentsMap.end()) {
        auto mouseListenerComponent = AceType::DynamicCast<MouseListenerComponent>(wrappingComponentsMap["mouse"]);
        if (mouseListenerComponent) {
            return mouseListenerComponent;
        }
    }

    RefPtr<MouseListenerComponent> mouseComponent = AceType::MakeRefPtr<OHOS::Ace::MouseListenerComponent>();
    wrappingComponentsMap.emplace("mouse", mouseComponent);
    return mouseComponent;
}

bool ViewStackProcessor::HasClickGestureListenerComponent() const
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("click_gesture") != wrappingComponentsMap.end()) {
        return true;
    }
    return false;
}

RefPtr<GestureListenerComponent> ViewStackProcessor::GetClickGestureListenerComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("click_gesture") != wrappingComponentsMap.end()) {
        auto gestureListenerComponent =
            AceType::DynamicCast<GestureListenerComponent>(wrappingComponentsMap["click_gesture"]);
        if (gestureListenerComponent) {
            return gestureListenerComponent;
        }
    }

    RefPtr<GestureListenerComponent> clickGestureComponent =
        AceType::MakeRefPtr<OHOS::Ace::GestureListenerComponent>();
    wrappingComponentsMap.emplace("click_gesture", clickGestureComponent);
    return clickGestureComponent;
}

RefPtr<FocusableComponent> ViewStackProcessor::GetFocusableComponent(bool createIfNotExist)
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("focusable") != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<FocusableComponent>(wrappingComponentsMap["focusable"]);
    }
    if (createIfNotExist) {
        RefPtr<FocusableComponent> focusableComponent = AceType::MakeRefPtr<OHOS::Ace::FocusableComponent>();
        wrappingComponentsMap.emplace("focusable", focusableComponent);
        Component::MergeRSNode(focusableComponent);
        return focusableComponent;
    }
    return nullptr;
}

RefPtr<GestureListenerComponent> ViewStackProcessor::GetPanGestureListenerComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("pan_gesture") != wrappingComponentsMap.end()) {
        auto gestureListenerComponent =
            AceType::DynamicCast<GestureListenerComponent>(wrappingComponentsMap["pan_gesture"]);
        if (gestureListenerComponent) {
            return gestureListenerComponent;
        }
    }

    RefPtr<GestureListenerComponent> panGestureComponent = AceType::MakeRefPtr<OHOS::Ace::GestureListenerComponent>();
    wrappingComponentsMap.emplace("pan_gesture", panGestureComponent);
    return panGestureComponent;
}

RefPtr<SharedTransitionComponent> ViewStackProcessor::GetSharedTransitionComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("shared_transition") != wrappingComponentsMap.end()) {
        auto sharedTransitionComponent =
            AceType::DynamicCast<SharedTransitionComponent>(wrappingComponentsMap["shared_transition"]);
        if (sharedTransitionComponent) {
            return sharedTransitionComponent;
        }
    }

    RefPtr<SharedTransitionComponent> sharedTransitionComponent =
        AceType::MakeRefPtr<OHOS::Ace::SharedTransitionComponent>("", "", "");
    wrappingComponentsMap.emplace("shared_transition", sharedTransitionComponent);
    return sharedTransitionComponent;
}

RefPtr<GestureComponent> ViewStackProcessor::GetGestureComponent()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("gesture") != wrappingComponentsMap.end()) {
        auto gestureComponent = AceType::DynamicCast<GestureComponent>(wrappingComponentsMap["gesture"]);
        if (gestureComponent) {
            return gestureComponent;
        }
    }

    RefPtr<GestureComponent> gestureComponent = AceType::MakeRefPtr<OHOS::Ace::GestureComponent>();
    wrappingComponentsMap.emplace("gesture", gestureComponent);
    return gestureComponent;
}

RefPtr<PageTransitionComponent> ViewStackProcessor::GetPageTransitionComponent()
{
    if (!pageTransitionComponent_) {
        pageTransitionComponent_ = AceType::MakeRefPtr<PageTransitionComponent>();
    }
    return pageTransitionComponent_;
}

void ViewStackProcessor::ClearPageTransitionComponent()
{
    if (pageTransitionComponent_) {
        pageTransitionComponent_.Reset();
    }
}

void ViewStackProcessor::Push(const RefPtr<Component>& component, bool isCustomView)
{
    CHECK_NULL_VOID(component);
    std::unordered_map<std::string, RefPtr<Component>> wrappingComponentsMap;
    if (componentsStack_.size() > 1 && ShouldPopImmediately()) {
        Pop();
    }
    wrappingComponentsMap.emplace("main", component);
    componentsStack_.push(wrappingComponentsMap);
    std::string name;
    auto composedComponent = AceType::DynamicCast<ComposedComponent>(component);
    if (composedComponent) {
        name = composedComponent->GetName();
    }
    auto tag = component->GetInspectorTag();
    tag = tag.empty() ? (name.empty() ? AceType::TypeName(component) : name) : tag;
    CreateInspectorComposedComponent(tag);
    CreateScoringComponent(tag);

#if defined(PREVIEW)
    if (!isCustomView && !AceType::InstanceOf<MultiComposedComponent>(component) &&
        !AceType::InstanceOf<TextSpanComponent>(component)) {
        GetBoxComponent();
    }
#else
    bool isAccessEnable =
        AceApplicationInfo::GetInstance().IsAccessibilityEnabled()
        || SystemProperties::GetAccessibilityEnabled()
        || SystemProperties::GetDebugBoundaryEnabled();
    if (!isCustomView && !AceType::InstanceOf<MultiComposedComponent>(component) &&
        !AceType::InstanceOf<TextSpanComponent>(component) && isAccessEnable) {
        GetBoxComponent();
    }
#endif
}

bool ViewStackProcessor::ShouldPopImmediately()
{
    auto type = AceType::TypeName(GetMainComponent());
    auto componentGroup = AceType::DynamicCast<ComponentGroup>(GetMainComponent());
    auto multiComposedComponent = AceType::DynamicCast<MultiComposedComponent>(GetMainComponent());
    auto soleChildComponent = AceType::DynamicCast<SoleChildComponent>(GetMainComponent());
    auto menuComponent = AceType::DynamicCast<MenuComponent>(GetMainComponent());
    return ((type && strcmp(type, AceType::TypeName<TextSpanComponent>()) == 0) ||
            !(componentGroup || multiComposedComponent || soleChildComponent || menuComponent));
}

void ViewStackProcessor::Pop()
{
    if (componentsStack_.empty() || componentsStack_.size() == 1) {
        return;
    }

    auto component = WrapComponents().first;
    if (AceType::DynamicCast<ComposedComponent>(component)) {
        auto childComponent = AceType::DynamicCast<ComposedComponent>(component)->GetChild();
        SetZIndex(childComponent);
        SetIsPercentSize(childComponent);
    } else {
        SetZIndex(component);
        SetIsPercentSize(component);
    }

    UpdateTopComponentProps(component);

    componentsStack_.pop();
    auto componentGroup = AceType::DynamicCast<ComponentGroup>(GetMainComponent());
    auto multiComposedComponent = AceType::DynamicCast<MultiComposedComponent>(GetMainComponent());
    if (componentGroup) {
        componentGroup->AppendChild(component);
    } else if (multiComposedComponent) {
        multiComposedComponent->AddChild(component);
    } else {
        auto singleChild = AceType::DynamicCast<SingleChild>(GetMainComponent());
        if (singleChild) {
            singleChild->SetChild(component);
        }
    }
}

RefPtr<Component> ViewStackProcessor::GetNewComponent()
{
    if (componentsStack_.empty()) {
        return nullptr;
    }
    auto component = WrapComponents().first;
    if (AceType::DynamicCast<ComposedComponent>(component)) {
        auto childComponent = AceType::DynamicCast<ComposedComponent>(component)->GetChild();
        SetZIndex(childComponent);
        SetIsPercentSize(childComponent);
    } else {
        SetZIndex(component);
        SetIsPercentSize(component);
    }
    UpdateTopComponentProps(component);
    componentsStack_.pop();
    return component;
}

void ViewStackProcessor::PopContainer()
{
    auto type = AceType::TypeName(GetMainComponent());
    auto componentGroup = AceType::DynamicCast<ComponentGroup>(GetMainComponent());
    auto multiComposedComponent = AceType::DynamicCast<MultiComposedComponent>(GetMainComponent());
    auto soleChildComponent = AceType::DynamicCast<SoleChildComponent>(GetMainComponent());
    if ((componentGroup && type && strcmp(type, AceType::TypeName<TextSpanComponent>()) != 0) ||
        multiComposedComponent || soleChildComponent) {
        Pop();
        return;
    }

    while ((!componentGroup && !multiComposedComponent && !soleChildComponent) ||
           (type && strcmp(type, AceType::TypeName<TextSpanComponent>()) == 0)) {
        if (componentsStack_.size() <= 1) {
            break;
        }
        Pop();
        type = AceType::TypeName(GetMainComponent());
        componentGroup = AceType::DynamicCast<ComponentGroup>(GetMainComponent());
        multiComposedComponent = AceType::DynamicCast<MultiComposedComponent>(GetMainComponent());
        soleChildComponent = AceType::DynamicCast<SoleChildComponent>(GetMainComponent());
    }
    Pop();
}

#ifdef ACE_DEBUG
void ViewStackProcessor::DumpStack()
{
    if (componentsStack_.empty()) {
        return;
    }
    std::stack<std::unordered_map<std::string, RefPtr<Component>>> tmp;
    while (!componentsStack_.empty()) {
        tmp.push(componentsStack_.top());
        componentsStack_.pop();
    }
    while (!tmp.empty()) {
        componentsStack_.push(tmp.top());
        tmp.pop();
    }
}
#endif

std::pair<RefPtr<Component>, RefPtr<Component>> ViewStackProcessor::WrapComponents()
{
    auto& wrappingComponentsMap = componentsStack_.top();
    std::vector<RefPtr<Component>> components;

    auto mainComponent = wrappingComponentsMap["main"];

    auto videoComponentV2 = AceType::DynamicCast<VideoComponentV2>(mainComponent);
    SaveComponentEvent saveComponentEvent;
    if (videoComponentV2) {
        saveComponentEvent = videoComponentV2->GetSaveComponentEvent();
    }
    std::unordered_map<std::string, RefPtr<Component>> videoMap;

    bool isItemComponent = AceType::InstanceOf<V2::ListItemComponent>(mainComponent) ||
                           AceType::InstanceOf<GridLayoutItemComponent>(mainComponent) ||
                           AceType::InstanceOf<V2::WaterFlowItemComponent>(mainComponent);

    RefPtr<Component> itemChildComponent;

    if (isItemComponent) {
        itemChildComponent = AceType::DynamicCast<SingleChild>(mainComponent)->GetChild();
        components.emplace_back(mainComponent);
    }

    auto composedComponent = GetInspectorComposedComponent();
    if (composedComponent) {
        components.emplace_back(composedComponent);
    }

    auto scoringComponent = GetScoringComponent();
    if (scoringComponent) {
        components.emplace_back(scoringComponent);
    }

    std::string componentNames[] = { "stepperItem", "stepperDisplay", "flexItem", "display", "transform", "touch",
        "pan_gesture", "click_gesture", "focusable", "coverage", "box", "shared_transition", "mouse",
        "stepperScroll" };
    // In RS extra case, use isFirstNode to determine the top node.
    bool isFirstNode = true;
    for (auto& name : componentNames) {
        auto iter = wrappingComponentsMap.find(name);
        if (iter != wrappingComponentsMap.end()) {
            if (isFirstNode) {
                iter->second->SetIsFirst(isFirstNode);
                isFirstNode = false;
            }
            iter->second->OnWrap();
            components.emplace_back(iter->second);
            if (videoComponentV2 && saveComponentEvent) {
                videoMap.emplace(std::make_pair(name, iter->second));
            }
        }
    }

    // Adjust layout param in TextComponent
    auto isTextComponent = AceType::InstanceOf<TextComponent>(mainComponent);
    if (isTextComponent) {
        auto iter = wrappingComponentsMap.find("box");
        if (iter != wrappingComponentsMap.end()) {
            auto boxComponent = AceType::DynamicCast<BoxComponent>(wrappingComponentsMap["box"]);
            if (boxComponent) {
                boxComponent->SetDeliverMinToChild(false);
            }
        }
    }

    if (wrappingComponentsMap.find("shared_transition") != wrappingComponentsMap.end() &&
        wrappingComponentsMap.find("display") != wrappingComponentsMap.end()) {
        auto sharedTransitionComponent =
            AceType::DynamicCast<SharedTransitionComponent>(wrappingComponentsMap["shared_transition"]);
        auto displayComponent = AceType::DynamicCast<DisplayComponent>(wrappingComponentsMap["display"]);
        if (sharedTransitionComponent && displayComponent) {
            sharedTransitionComponent->SetOpacity(displayComponent->GetOpacity());
        }
    }

    if (videoComponentV2 && saveComponentEvent) {
        saveComponentEvent(videoMap);
        videoComponentV2->SetSaveComponentEvent(nullptr);
    }

    if (isItemComponent) {
        // itemComponent is placed before other components, they should share the same RSNode.
        // we should not touch itemChildComponent as it's already marked.
        //    (head)       (tail)      (unchanged)
        // mainComponent - others - itemChildComponent
        Component::MergeRSNode(mainComponent);
        Component::MergeRSNode(components);
        if (itemChildComponent) {
            components.emplace_back(itemChildComponent);
        }
    } else if (!components.empty() && (
        AceType::InstanceOf<BoxComponent>(mainComponent) ||
        AceType::InstanceOf<FormComponent>(mainComponent) ||
        AceType::InstanceOf<TextFieldComponent>(mainComponent) ||
        AceType::InstanceOf<TextureComponent>(mainComponent) ||
        AceType::InstanceOf<WebComponent>(mainComponent) ||
        AceType::InstanceOf<XComponentComponent>(mainComponent))) {
        // special types of mainComponent need be marked as standalone.
        // (head)(tail)  (standalone)
        //    others  -  mainComponent
        Component::MergeRSNode(components);
        Component::MergeRSNode(mainComponent);
        components.emplace_back(mainComponent);
    } else if (AceType::InstanceOf<RemoteWindowComponent>(mainComponent)) {
        // mark head component, it should use external RSNode stored in tail component.
        components.emplace_back(mainComponent);
        Component::MergeRSNode(components);
        components.front()->MarkUseExternalRSNode(true);
    } else {
        // by default, mainComponent is placed after other components, they should share the same RSNode.
        //  (head)      (tail)
        // (others) - mainComponent
        components.emplace_back(mainComponent);
        Component::MergeRSNode(components);
    }

    // First, composite all components.
    for (int32_t idx = static_cast<int32_t>(components.size()) - 1; idx - 1 >= 0; --idx) {
        auto singleChild = AceType::DynamicCast<SingleChild>(components[idx - 1]);
        if (singleChild) {
            singleChild->SetChild(components[idx]);
            continue;
        }

        auto coverageComponent = AceType::DynamicCast<CoverageComponent>(components[idx - 1]);
        if (coverageComponent) {
            coverageComponent->InsertChild(0, components[idx]);
            if (coverageComponent->IsOverLay()) {
                coverageComponent->Initialization();
                Component::MergeRSNode(components[idx], coverageComponent);
            }
#ifndef WEARABLE_PRODUCT
            auto popupComponent = GetPopupComponent(false);
            if (popupComponent) {
                coverageComponent->AppendChild(popupComponent);
            }
#endif
            auto menuComponent = GetMenuComponent(false);
            if (menuComponent) {
                coverageComponent->AppendChild(menuComponent);
            }
        }

        auto focusableComponent = AceType::DynamicCast<FocusableComponent>(components[idx - 1]);
        if (focusableComponent) {
            Component::MergeRSNode(components[idx], focusableComponent);
        }
    }

    auto component = components.front();
    auto iter = wrappingComponentsMap.find("box");
    if (iter != wrappingComponentsMap.end() && (iter->second->GetTextDirection() != component->GetTextDirection())) {
        component->SetTextDirection(iter->second->GetTextDirection());
    }

    for (auto&& component : components) {
        component->SetTouchable(mainComponent->IsTouchable() &&
        mainComponent->GetHitTestMode() != HitTestMode::HTMNONE);
    }

    for (auto&& component : components) {
        component->SetHitTestMode(mainComponent->GetHitTestMode());
    }

    for (auto&& component : components) {
        component->SetDisabledStatus(mainComponent->IsDisabledStatus());
    }
    return std::pair<RefPtr<Component>, RefPtr<Component>>(components[0], components[components.size() - 1]);
}

void ViewStackProcessor::UpdateTopComponentProps(const RefPtr<Component>& component)
{
    auto& wrappingComponentsMap = componentsStack_.top();
    if (wrappingComponentsMap.find("position") != wrappingComponentsMap.end()) {
        auto renderComponent = AceType::DynamicCast<RenderComponent>(component);
        if (renderComponent) {
            auto positionedComponent = GetPositionedComponent();
            if (positionedComponent->HasPositionStyle()) {
                renderComponent->SetMotionPathOption(positionedComponent->GetMotionPathOption());
                renderComponent->SetHasLeft(positionedComponent->HasLeft());
                renderComponent->SetLeft(positionedComponent->GetLeft());
                renderComponent->SetHasTop(positionedComponent->HasTop());
                renderComponent->SetTop(positionedComponent->GetTop());
                renderComponent->SetHasBottom(positionedComponent->HasBottom());
                renderComponent->SetBottom(positionedComponent->GetBottom());
                renderComponent->SetHasRight(positionedComponent->HasRight());
                renderComponent->SetRight(positionedComponent->GetRight());
                renderComponent->SetPositionType(positionedComponent->GetPositionType());
            }
        }
    }
}

std::pair<RefPtr<Component>, RefPtr<Component>> ViewStackProcessor::FinishReturnMain()
{
    if (componentsStack_.empty()) {
        LOGE("ViewStackProcessor FinishInternal failed, input empty render or invalid root component");
        return std::pair<RefPtr<Component>, RefPtr<Component>>(nullptr, nullptr);
    }
    const auto& componentsPair = WrapComponents();
    auto component = componentsPair.first;
    if (AceType::DynamicCast<ComposedComponent>(component)) {
        auto childComponent = AceType::DynamicCast<ComposedComponent>(component)->GetChild();
        SetZIndex(childComponent);
    } else {
        SetZIndex(component);
    }
    componentsStack_.pop();
    return componentsPair;
}

void ViewStackProcessor::PushKey(const std::string& key)
{
    if (viewKey_.empty()) {
        viewKey_ = key;
        keyStack_.emplace(key.length());
    } else {
        viewKey_.append("_").append(key);
        keyStack_.emplace(key.length() + 1);
    }
}

void ViewStackProcessor::PopKey()
{
    size_t length = keyStack_.top();
    keyStack_.pop();

    if (length > 0) {
        viewKey_.erase(viewKey_.length() - length);
    }
}

std::string ViewStackProcessor::GetKey()
{
    return viewKey_.empty() ? "" : viewKey_;
}

std::string ViewStackProcessor::ProcessViewId(const std::string& viewId)
{
    return viewKey_.empty() ? viewId : viewKey_ + "_" + viewId;
}

void ViewStackProcessor::SetImplicitAnimationOption(const AnimationOption& option)
{
    implicitAnimationOption_ = option;
}

const AnimationOption& ViewStackProcessor::GetImplicitAnimationOption() const
{
    return implicitAnimationOption_;
}

void ViewStackProcessor::SetZIndex(RefPtr<Component>& component)
{
    int32_t zIndex = 0;
    auto mainComponent = AceType::DynamicCast<RenderComponent>(GetMainComponent());
    if (mainComponent) {
        zIndex = mainComponent->GetZIndex();
    }

    auto renderComponent = AceType::DynamicCast<RenderComponent>(component);
    if (renderComponent) {
        renderComponent->SetZIndex(zIndex);
    }
}

RefPtr<V2::InspectorComposedComponent> ViewStackProcessor::GetInspectorComposedComponent() const
{
    if (componentsStack_.empty()) {
        return nullptr;
    }
    auto& wrappingComponentsMap = componentsStack_.top();
    auto iter = wrappingComponentsMap.find("inspector");
    if (iter != wrappingComponentsMap.end()) {
        return AceType::DynamicCast<V2::InspectorComposedComponent>(iter->second);
    }
    return nullptr;
}

RefPtr<Component> ViewStackProcessor::GetScoringComponent() const
{
    if (componentsStack_.empty()) {
        return nullptr;
    }
    auto& wrappingComponentsMap = componentsStack_.top();
    auto iter = wrappingComponentsMap.find("scoring");
    if (iter != wrappingComponentsMap.end()) {
        return iter->second;
    }
    return nullptr;
}

RefPtr<ComposedComponent> ViewStackProcessor::CreateInspectorWrapper(const std::string& inspectorTag)
{
    return AceType::MakeRefPtr<V2::InspectorComposedComponent>(
        V2::InspectorComposedComponent::GenerateId(), inspectorTag);
}

void ViewStackProcessor::CreateInspectorComposedComponent(const std::string& inspectorTag)
{
    if (V2::InspectorComposedComponent::HasInspectorFinished(inspectorTag)) {
        auto composedComponent = AceType::MakeRefPtr<V2::InspectorComposedComponent>(
            V2::InspectorComposedComponent::GenerateId(), inspectorTag);
        auto& wrappingComponentsMap = componentsStack_.top();
#if defined(PREVIEW)
        composedComponent->SetViewId(viewKey_);
#endif
        wrappingComponentsMap.emplace("inspector", composedComponent);
    }
}

void ViewStackProcessor::CreateScoringComponent(const std::string& tag)
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [this]() {
        auto name = AceApplicationInfo::GetInstance().GetProcessName().empty()
                        ? AceApplicationInfo::GetInstance().GetPackageName()
                        : AceApplicationInfo::GetInstance().GetProcessName();
        isScoringEnable_ = SystemProperties::IsScoringEnabled(name);
    });

    if (isScoringEnable_ && V2::InspectorComposedComponent::HasInspectorFinished(tag)) {
        auto component =
            AceType::MakeRefPtr<ScoringComponent>(V2::InspectorComposedComponent::GetEtsTag(tag), viewKey_);
        auto& wrappingComponentsMap = componentsStack_.top();
        wrappingComponentsMap.emplace("scoring", component);
    }
}

void ViewStackProcessor::SetIsPercentSize(RefPtr<Component>& component)
{
    bool isPercentSize = false;
    auto mainComponent = AceType::DynamicCast<RenderComponent>(GetMainComponent());
    if (mainComponent) {
        isPercentSize = mainComponent->GetIsPercentSize();
    }

    auto renderComponent = AceType::DynamicCast<RenderComponent>(component);
    if (renderComponent) {
        renderComponent->SetIsPercentSize(isPercentSize);
    }
}

void ViewStackProcessor::ClaimElementId(const RefPtr<Component>& component)
{
    component->AssignUniqueElementId(reservedElementId_);
    reservedElementId_ = ElementRegister::UndefinedElementId;
}

ScopedViewStackProcessor::ScopedViewStackProcessor()
{
    std::swap(instance_, ViewStackProcessor::instance);
}

ScopedViewStackProcessor::~ScopedViewStackProcessor()
{
    std::swap(instance_, ViewStackProcessor::instance);
}

} // namespace OHOS::Ace::Framework

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
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_nav_destination_bridge.h"

#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_navigation_utils.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"
#include "bridge/declarative_frontend/jsview/js_utils.h"
#include "core/components_ng/pattern/navrouter/navdestination_model_ng.h"
namespace OHOS::Ace::NG {
constexpr int NUM_0 = 0;
constexpr int NUM_1 = 1;
constexpr int NUM_2 = 2;
constexpr int32_t JS_EMUN_TRANSITIONTYPE_NONE = 1;
constexpr int32_t JS_EMUN_TRANSITIONTYPE_TITLE = 2;
constexpr int32_t JS_EMUN_TRANSITIONTYPE_CONTENT = 3;

// sources in js_window_utils.h
enum class ApiOrientation : uint32_t {
    BEGIN = 0,
    UNSPECIFIED = BEGIN,
    PORTRAIT = 1,
    LANDSCAPE = 2,
    PORTRAIT_INVERTED = 3,
    LANDSCAPE_INVERTED = 4,
    AUTO_ROTATION = 5,
    AUTO_ROTATION_PORTRAIT = 6,
    AUTO_ROTATION_LANDSCAPE = 7,
    AUTO_ROTATION_RESTRICTED = 8,
    AUTO_ROTATION_PORTRAIT_RESTRICTED = 9,
    AUTO_ROTATION_LANDSCAPE_RESTRICTED = 10,
    LOCKED = 11,
    AUTO_ROTATION_UNSPECIFIED = 12,
    USER_ROTATION_PORTRAIT = 13,
    USER_ROTATION_LANDSCAPE = 14,
    USER_ROTATION_PORTRAIT_INVERTED = 15,
    USER_ROTATION_LANDSCAPE_INVERTED = 16,
    FOLLOW_DESKTOP = 17,
    END = FOLLOW_DESKTOP,
};

const std::map<ApiOrientation, Orientation> JS_TO_NATIVE_ORIENTATION_MAP {
    {ApiOrientation::UNSPECIFIED,                           Orientation::UNSPECIFIED                        },
    {ApiOrientation::PORTRAIT,                              Orientation::VERTICAL                           },
    {ApiOrientation::LANDSCAPE,                             Orientation::HORIZONTAL                         },
    {ApiOrientation::PORTRAIT_INVERTED,                     Orientation::REVERSE_VERTICAL                   },
    {ApiOrientation::LANDSCAPE_INVERTED,                    Orientation::REVERSE_HORIZONTAL                 },
    {ApiOrientation::AUTO_ROTATION,                         Orientation::SENSOR                             },
    {ApiOrientation::AUTO_ROTATION_PORTRAIT,                Orientation::SENSOR_VERTICAL                    },
    {ApiOrientation::AUTO_ROTATION_LANDSCAPE,               Orientation::SENSOR_HORIZONTAL                  },
    {ApiOrientation::AUTO_ROTATION_RESTRICTED,              Orientation::AUTO_ROTATION_RESTRICTED           },
    {ApiOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED,     Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED  },
    {ApiOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED,    Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED },
    {ApiOrientation::LOCKED,                                Orientation::LOCKED                             },
    {ApiOrientation::AUTO_ROTATION_UNSPECIFIED,             Orientation::AUTO_ROTATION_UNSPECIFIED          },
    {ApiOrientation::USER_ROTATION_PORTRAIT,                Orientation::USER_ROTATION_PORTRAIT             },
    {ApiOrientation::USER_ROTATION_LANDSCAPE,               Orientation::USER_ROTATION_LANDSCAPE            },
    {ApiOrientation::USER_ROTATION_PORTRAIT_INVERTED,       Orientation::USER_ROTATION_PORTRAIT_INVERTED    },
    {ApiOrientation::USER_ROTATION_LANDSCAPE_INVERTED,      Orientation::USER_ROTATION_LANDSCAPE_INVERTED   },
    {ApiOrientation::FOLLOW_DESKTOP,                        Orientation::FOLLOW_DESKTOP                     },
};

NavigationSystemTransitionType ParseTransitionType(int32_t value)
{
    switch (value) {
        case JS_EMUN_TRANSITIONTYPE_NONE:
            return NG::NavigationSystemTransitionType::NONE;
        case JS_EMUN_TRANSITIONTYPE_TITLE:
            return NG::NavigationSystemTransitionType::TITLE;
        case JS_EMUN_TRANSITIONTYPE_CONTENT:
            return NG::NavigationSystemTransitionType::CONTENT;
        default:
            return NG::NavigationSystemTransitionType::DEFAULT;
    }
}

ArkUINativeModuleValue NavDestinationBridge::SetHideTitleBar(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> hideArg = runtimeCallInfo->GetCallArgRef(1);
    const int32_t argsNum = 2; // 2: parameter index
    Local<JSValueRef> animatedArg = runtimeCallInfo->GetCallArgRef(argsNum);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    bool hide = false;
    bool animated = false;
    if (!hideArg->IsNull() && !hideArg->IsUndefined() && hideArg->IsBoolean()) {
        hide = hideArg->ToBoolean(vm)->Value();
    }
    if (!animatedArg->IsNull() && !animatedArg->IsUndefined() && animatedArg->IsBoolean()) {
        animated = animatedArg->ToBoolean(vm)->Value();
    }
    GetArkUINodeModifiers()->getNavDestinationModifier()->setHideTitleBar(nativeNode, hide, animated);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetHideTitleBar(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetHideTitleBar(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetHideToolBar(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> hideArg = runtimeCallInfo->GetCallArgRef(1);
    const int32_t argsNum = 2; // 2: parameter index
    Local<JSValueRef> animatedArg = runtimeCallInfo->GetCallArgRef(argsNum);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    bool hide = false;
    bool animated = false;
    if (!hideArg->IsNull() && !hideArg->IsUndefined() && hideArg->IsBoolean()) {
        hide = hideArg->ToBoolean(vm)->Value();
    }
    if (!animatedArg->IsNull() && !animatedArg->IsUndefined() && animatedArg->IsBoolean()) {
        animated = animatedArg->ToBoolean(vm)->Value();
    }
    GetArkUINodeModifiers()->getNavDestinationModifier()->setNavDestinationHideToolBar(nativeNode, hide, animated);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetHideToolBar(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetNavDestinationHideToolBar(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetBackButtonIcon(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto* frameNode = reinterpret_cast<FrameNode*>(nativeNode);

    Framework::JsiCallbackInfo info = Framework::JsiCallbackInfo(runtimeCallInfo);
    std::string src;
    auto noPixMap = Framework::JSViewAbstract::ParseJsMedia(info[1], src);

    RefPtr<PixelMap> pixMap = nullptr;
#if defined(PIXEL_MAP_SUPPORTED)
    if (!noPixMap) {
        pixMap = CreatePixelMapFromNapiValue(info[1]);
    }
#endif
    std::string bundleName;
    std::string moduleName;
    
    Framework::JSViewAbstract::GetJsMediaBundleInfo(info[1], bundleName, moduleName);
    NavDestinationModelNG::SetBackButtonIcon(frameNode, src, noPixMap, pixMap);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetHideBackButton(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> hideArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (hideArg->IsBoolean()) {
        bool hide = hideArg->ToBoolean(vm)->Value();
        GetArkUINodeModifiers()->getNavDestinationModifier()->setNavDestinationHideBackButton(nativeNode, hide);
    } else {
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetNavDestinationHideBackButton(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetHideBackButton(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetNavDestinationHideBackButton(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetBackButtonIcon(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto* frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    bool noPixMap = false;
    RefPtr<PixelMap> pixMap = nullptr;
    std::string src;
    NavDestinationModelNG::SetBackButtonIcon(frameNode, src, noPixMap, pixMap);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetNavDestinationMode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    auto* frameNode = reinterpret_cast<FrameNode*>(nativeNode);

    Framework::JsiCallbackInfo info = Framework::JsiCallbackInfo(runtimeCallInfo);
    if (!info[1]->IsNumber()) {
        // set default back ground color
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetNavDestinationMode(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    auto mode = info[1]->ToNumber<int32_t>();
    NavDestinationModelNG::SetNavDestinationMode(frameNode, static_cast<NG::NavDestinationMode>(mode));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetNavDestinationMode(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetNavDestinationMode(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetIgnoreLayoutSafeArea(ArkUIRuntimeCallInfo *runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> thirdArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    std::string typeCppStr;
    std::string edgesCppStr;
    if (secondArg->IsString(vm)) {
        typeCppStr = secondArg->ToString(vm)->ToString(vm);
    } else {
        //type system
        typeCppStr = "0";
    }

    if (thirdArg->IsString(vm)) {
        edgesCppStr = thirdArg->ToString(vm)->ToString(vm);
    } else {
        //edge top and bottom
        edgesCppStr = "0|1";
    }
    const char* typeStr = typeCppStr.c_str();
    const char* edgesStr = edgesCppStr.c_str();
    GetArkUINodeModifiers()->getNavDestinationModifier()->setIgnoreLayoutSafeArea(nativeNode, typeStr, edgesStr);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetIgnoreLayoutSafeArea(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetIgnoreLayoutSafeArea(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetTitle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> titleArg = runtimeCallInfo->GetCallArgRef(1);
    Local<JSValueRef> optionsArg = runtimeCallInfo->GetCallArgRef(2);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    std::string title;
    std::string subtitle;
    bool hasMain = false;
    bool hasSub = false;
    if (ArkTSUtils::ParseJsString(vm, titleArg, title)) {
        // Resource and string type.
        subtitle = "";
        hasMain = true;
        hasSub = false;
    } else if (titleArg->IsObject(vm)) {
        // NavigationCommonTitle
        auto obj = titleArg->ToObject(vm);
        auto main = obj->Get(vm, panda::StringRef::NewFromUtf8(vm, "main"));
        auto sub = obj->Get(vm, panda::StringRef::NewFromUtf8(vm, "sub"));
        hasMain = ArkTSUtils::ParseJsString(vm, main, title);
        hasSub = ArkTSUtils::ParseJsString(vm, sub, subtitle);
        // NavigationCustomTitle or CustomBuilder is not supported
    } else {
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetTitle(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }

    ArkUINavigationTitlebarOptions options;
    if (optionsArg->IsObject(vm)) {
        NativeNavigationUtils::ParseTitleOptions(vm, optionsArg, options);
    }
    ArkUINavigationTitleInfo titleInfo = { hasSub, hasMain, subtitle.c_str(), title.c_str() };
    GetArkUINodeModifiers()->getNavDestinationModifier()->setTitle(nativeNode, titleInfo, options);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetTitle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetTitle(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetMenus(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> menusArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    std::vector<ArkUIBarItem> menuItems;
    if (menusArg->IsArray(vm)) {
        NativeNavigationUtils::ParseBarItems(vm, menusArg, menuItems);
        GetArkUINodeModifiers()->getNavDestinationModifier()->setMenus(nativeNode, menuItems.data(), menuItems.size());
        // get action and symbolModifier single
        auto actionSendFunc = GetArkUINodeModifiers()->getNavDestinationModifier()->setMenuItemAction;
        auto symbolSendFunc = GetArkUINodeModifiers()->getNavDestinationModifier()->setMenuItemSymbol;
        NativeNavigationUtils::ParseAndSendFunctionParam(runtimeCallInfo, menusArg, actionSendFunc, symbolSendFunc);
    } else if (menusArg->IsUndefined()) {
        menuItems = {};
        GetArkUINodeModifiers()->getNavDestinationModifier()->setMenus(nativeNode, menuItems.data(), menuItems.size());
    } else if (menusArg->IsObject(vm)) {
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetMenus(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetMenus(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetMenus(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetRecoverable(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> recoverableArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (recoverableArg->IsNull() || recoverableArg->IsUndefined() || !recoverableArg->IsBoolean()) {
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetRecoverable(nativeNode);
    } else {
        bool recoverable = recoverableArg->ToBoolean(vm)->Value();
        GetArkUINodeModifiers()->getNavDestinationModifier()->setRecoverable(nativeNode, recoverable);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetRecoverable(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetRecoverable(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetNavDestinationSystemTransition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());

    Framework::JsiCallbackInfo info = Framework::JsiCallbackInfo(runtimeCallInfo);
    if (!info[1]->IsNumber()) {
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetNavDestinationSystemTransition(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    auto value = info[1]->ToNumber<int32_t>();
    NavigationSystemTransitionType type = ParseTransitionType(value);
    GetArkUINodeModifiers()->getNavDestinationModifier()->setNavDestinationSystemTransition(
        nativeNode,
        static_cast<ArkUI_Int32>(type));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetNavDestinationSystemTransition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetNavDestinationSystemTransition(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetPreferredOrientation(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());

    Framework::JsiCallbackInfo info = Framework::JsiCallbackInfo(runtimeCallInfo);
    if (!info[1]->IsNumber()) {
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetPreferredOrientation(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    auto ori = info[1]->ToNumber<int32_t>();
    if (ori < static_cast<int32_t>(ApiOrientation::BEGIN) || ori > static_cast<int32_t>(ApiOrientation::END)) {
        GetArkUINodeModifiers()->getNavDestinationModifier()->resetPreferredOrientation(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    ori = static_cast<int32_t>(JS_TO_NATIVE_ORIENTATION_MAP.at(static_cast<ApiOrientation>(ori)));
    GetArkUINodeModifiers()->getNavDestinationModifier()->setPreferredOrientation(nativeNode, ori);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetPreferredOrientation(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetPreferredOrientation(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetEnableStatusBar(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());

    ArkUIOptionalBool enable;
    enable.isSet = false;
    ArkUIOptionalBool animated;
    animated.isSet = false;
    Framework::JsiCallbackInfo info = Framework::JsiCallbackInfo(runtimeCallInfo);
    if (info[1]->IsBoolean()) {
        enable.isSet = true;
        enable.value = info[1]->ToBoolean();
    }
    if (info[2]->IsBoolean()) {
        animated.isSet = true;
        animated.value = info[2]->ToBoolean();
    }
    GetArkUINodeModifiers()->getNavDestinationModifier()->setEnableStatusBar(nativeNode, enable, animated);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetEnableStatusBar(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetEnableStatusBar(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::SetEnableNavigationIndicator(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    ArkUIOptionalBool enable;
    enable.isSet = false;
    Framework::JsiCallbackInfo info = Framework::JsiCallbackInfo(runtimeCallInfo);
    if (info[1]->IsBoolean()) {
        enable.isSet = true;
        enable.value = info[1]->ToBoolean();
    }
    GetArkUINodeModifiers()->getNavDestinationModifier()->setEnableNavigationIndicator(nativeNode, enable);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue NavDestinationBridge::ResetEnableNavigationIndicator(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getNavDestinationModifier()->resetEnableNavigationIndicator(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
} // namespace OHOS::Ace::NG

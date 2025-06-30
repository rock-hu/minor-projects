/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_pattern_lock_bridge.h"

#include "base/geometry/dimension.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/pipeline/pipeline_base.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"
#include "frameworks/core/components_v2/pattern_lock/pattern_lock_component.h"

namespace OHOS::Ace::NG {
constexpr int NUM_0 = 0;
constexpr int NUM_1 = 1;

ArkUINativeModuleValue PatternLockBridge::SetSideLength(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CalcDimension sideLength;
    RefPtr<ResourceObject> sideLengthResObj;
    if (!(ArkTSUtils::ParseJsDimensionVp(vm, secondArg, sideLength, sideLengthResObj))) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockSideLength(nativeNode);
    }
    auto sideLengthRawPtr = AceType::RawPtr(sideLengthResObj);
    GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockSideLengthRes(
        nativeNode, sideLength.Value(), static_cast<int8_t>(sideLength.Unit()), sideLengthRawPtr);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetSideLength(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockSideLength(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetAutoReset(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    uint32_t value = 1;
    if (secondArg->IsBoolean()) {
        value = static_cast<uint32_t>(secondArg->ToBoolean(vm)->Value());
    }
    GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockAutoReset(nativeNode, value);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetAutoReset(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockAutoReset(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPathStrokeWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CalcDimension strokeWidth;
    if (!(ArkTSUtils::ParseJsDimensionVp(vm, secondArg, strokeWidth))) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockPathStrokeWidth(nativeNode);
    }
    GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockPathStrokeWidth(
        nativeNode, strokeWidth.Value(), static_cast<int8_t>(strokeWidth.Unit()));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPathStrokeWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockPathStrokeWidth(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetRegularColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    RefPtr<ResourceObject> regularColorResObj;
    auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color, regularColorResObj, nodeInfo)) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockRegularColor(nativeNode);
    } else {
        auto regularColorRawPtr = AceType::RawPtr(regularColorResObj);
        GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockRegularColorRes(
            nativeNode, color.GetValue(), regularColorRawPtr);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetRegularColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockRegularColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPathColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    RefPtr<ResourceObject> pathColorResObj;
    auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color, pathColorResObj, nodeInfo)) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockPathColor(nativeNode);
    } else {
        auto pathColorRawPtr = AceType::RawPtr(pathColorResObj);
        GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockPathColorRes(
            nativeNode, color.GetValue(), pathColorRawPtr);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPathColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockPathColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPatternLockActiveColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    RefPtr<ResourceObject> activeColorResObj;
    auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color, activeColorResObj, nodeInfo)) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveColor(nativeNode);
    } else {
        auto activeColorRawPtr = AceType::RawPtr(activeColorResObj);
        GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockActiveColorRes(
            nativeNode, color.GetValue(), activeColorRawPtr);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPatternLockActiveColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPatternLockCircleRadius(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CalcDimension circleRadius;
    RefPtr<ResourceObject> circleRadiusResObj;
    if (ArkTSUtils::ParseJsDimensionVp(vm, secondArg, circleRadius, circleRadiusResObj) &&
        !(circleRadius.IsNonPositive())) {
        auto circleRadiusRawPtr = AceType::RawPtr(circleRadiusResObj);
        GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockCircleRadiusRes(
            nativeNode, circleRadius.Value(), static_cast<int8_t>(circleRadius.Unit()), circleRadiusRawPtr);
    } else {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockCircleRadius(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPatternLockCircleRadius(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockCircleRadius(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPatternLockSelectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    RefPtr<ResourceObject> selectedColorResObj;
    auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color, selectedColorResObj, nodeInfo)) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockSelectedColor(nativeNode);
    } else {
        auto selectedColorRawPtr = AceType::RawPtr(selectedColorResObj);
        GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockSelectedColorRes(
            nativeNode, color.GetValue(), selectedColorRawPtr);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPatternLockSelectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockSelectedColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPatternLockActivateCircleStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    if (!secondArg->IsObject(vm)) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveCircleColor(nativeNode);
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveCircleRadius(nativeNode);
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockEnableWaveEffect(nativeNode);
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockEnableForeground(nativeNode);
    } else {
        auto obj = secondArg->ToObject(vm);
        auto jsColor = obj->Get(vm, panda::StringRef::NewFromUtf8(vm, "color"));
        auto jsRadius = obj->Get(vm, panda::StringRef::NewFromUtf8(vm, "radius"));
        auto jsEnable = obj->Get(vm, panda::StringRef::NewFromUtf8(vm, "enableWaveEffect"));
        auto jsEnableForeground = obj->Get(vm, panda::StringRef::NewFromUtf8(vm, "enableForeground"));
        Color color;
        RefPtr<ResourceObject> colorResObj;
        auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
        if (!ArkTSUtils::ParseJsColorAlpha(vm, jsColor, color, colorResObj, nodeInfo)) {
            GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveCircleColor(nativeNode);
        } else {
            auto activeCircleColorRawPtr = AceType::RawPtr(colorResObj);
            GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockActiveCircleColorRes(
                nativeNode, color.GetValue(), activeCircleColorRawPtr);
        }
        CalcDimension circleRadius;
        if (jsRadius->IsObject(vm) && ArkTSUtils::ParseJsLengthMetrics(vm, jsRadius, circleRadius) &&
            !(circleRadius.IsNonPositive())) {
            GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockActiveCircleRadius(
                nativeNode, circleRadius.Value(), static_cast<int8_t>(circleRadius.Unit()));
        } else {
            GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveCircleRadius(nativeNode);
        }
        uint32_t value = 1;
        if (jsEnable->IsBoolean()) {
            value = static_cast<uint32_t>(jsEnable->ToBoolean(vm)->Value());
        }
        GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockEnableWaveEffect(nativeNode, value);
        uint32_t enableForegroundValue = 0;
        if (jsEnableForeground->IsBoolean()) {
            enableForegroundValue = static_cast<uint32_t>(jsEnableForeground->ToBoolean(vm)->Value());
        }
        GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockEnableForeground(
            nativeNode, enableForegroundValue);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPatternLockActivateCircleStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveCircleColor(nativeNode);
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockActiveCircleRadius(nativeNode);
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockEnableWaveEffect(nativeNode);
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockEnableForeground(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPatternLockSkipUnselectedPoint(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    uint32_t value = 0;
    if (secondArg->IsBoolean()) {
        value = static_cast<uint32_t>(secondArg->ToBoolean(vm)->Value());
    }
    GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockSkipUnselectedPoint(nativeNode, value);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPatternLockSkipUnselectedPoint(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockSkipUnselectedPoint(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPatternLockOnPatternComplete(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> callbackArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    if (callbackArg->IsUndefined() || callbackArg->IsNull() || !callbackArg->IsFunction(vm)) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockOnPatternComplete(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    panda::Local<panda::FunctionRef> func = callbackArg->ToObject(vm);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    std::function<void(const BaseEventInfo* event)> callback = [vm, frameNode, func = panda::CopyableGlobal(vm, func)](
                                                                   const BaseEventInfo* event) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        PipelineContext::SetCallBackNode(AceType::WeakClaim(frameNode));
        const auto* eventInfo = TypeInfoHelper::DynamicCast<V2::PatternCompleteEvent>(event);
        CHECK_NULL_VOID(eventInfo);
        panda::Local<panda::JSValueRef> params[] = { ArkTSUtils::ChoosePointToJSValue(vm, eventInfo->GetInput()) };
        func->Call(vm, func.ToLocal(), params, 1);
    };
    GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockOnPatternComplete(
        nativeNode, reinterpret_cast<void*>(&callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPatternLockOnPatternComplete(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockOnPatternComplete(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::SetPatternLockOnDotConnect(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> callbackArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    if (callbackArg->IsUndefined() || callbackArg->IsNull() || !callbackArg->IsFunction(vm)) {
        GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockOnDotConnect(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    panda::Local<panda::FunctionRef> func = callbackArg->ToObject(vm);
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    std::function<void(int32_t code)> callback = [vm, frameNode, func = panda::CopyableGlobal(vm, func)](int32_t code) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        PipelineContext::SetCallBackNode(AceType::WeakClaim(frameNode));
        panda::Local<panda::JSValueRef> params = ArkTSUtils::ToJSValueWithVM(vm, code);
        func->Call(vm, func.ToLocal(), &params, 1);
    };
    GetArkUINodeModifiers()->getPatternLockModifier()->setPatternLockOnDotConnect(
        nativeNode, reinterpret_cast<void*>(&callback));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue PatternLockBridge::ResetPatternLockOnDotConnect(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    CHECK_NULL_RETURN(nativeNode, panda::JSValueRef::Undefined(vm));
    GetArkUINodeModifiers()->getPatternLockModifier()->resetPatternLockOnDotConnect(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
} // namespace OHOS::Ace::NG

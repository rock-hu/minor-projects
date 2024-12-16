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
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_alphabet_indexer_bridge.h"

#include "core/interfaces/native/node/node_api.h"
#include "bridge/declarative_frontend/jsview/models/indexer_model_impl.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_common_bridge.h"
namespace OHOS::Ace::NG {
namespace {
constexpr int NUM_0 = 0;
constexpr int NUM_1 = 1;
constexpr int NUM_2 = 2;
constexpr int NUM_3 = 3;
constexpr int NUM_4 = 4;
const std::string FORMAT_FONT = "%s|%s|%s";
const std::string DEFAULT_FAMILY = "HarmonyOS Sans";
constexpr double DEFAULT_POPUP_ITEM_FONT_SIZE = 24.0;
constexpr Dimension DEFAULT_FONT_SIZE_VAL = 12.0_fp;
const std::string DEFAULT_POPUP_ITEM_FONT_WEIGHT = "medium";
constexpr Dimension DEFAULT_POPUP_POSITION_X = 60.0_vp;
constexpr Dimension DEFAULT_POPUP_POSITION_Y = 48.0_vp;
constexpr double RADIUS_OFFSET = 4.0;
} // namespace

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupItemFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> weightArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    CalcDimension fontSize;
    if (!fontSizeArg->IsNull()) {
        CalcDimension fontSizeData;
        if (ArkTSUtils::ParseJsDimensionFp(vm, fontSizeArg, fontSizeData) && !fontSizeData.IsNegative() &&
            fontSizeData.Unit() != DimensionUnit::PERCENT) {
            fontSize = fontSizeData;
        }
    }
    std::string weight = DEFAULT_POPUP_ITEM_FONT_WEIGHT;
    if (!weightArg->IsNull() && !weightArg->IsUndefined()) {
        if (weightArg->IsNumber()) {
            weight = std::to_string(weightArg->Int32Value(vm));
        } else {
            if (!ArkTSUtils::ParseJsString(vm, weightArg, weight) || weight.empty()) {
                weight = DEFAULT_POPUP_ITEM_FONT_WEIGHT;
            }
        }
    }
    
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupItemFont(
        nativeNode, fontSize.Value(), static_cast<int>(fontSize.Unit()), weight.c_str());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupItemFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupItemFont(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetSelectedFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> weightArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(NUM_3);
    Local<JSValueRef> styleValArg = runtimeCallInfo->GetCallArgRef(NUM_4);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    bool isUndefinedValue = (fontSizeArg->IsNull() || fontSizeArg->IsUndefined()) &&
                            (weightArg->IsNull() || weightArg->IsUndefined()) &&
                            (fontFamilyArg->IsNull() || fontFamilyArg->IsUndefined()) &&
                            (styleValArg->IsNull() || styleValArg->IsUndefined());
    if (isUndefinedValue) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetSelectedFont(nativeNode);
    }
    CalcDimension fontSizeData(DEFAULT_FONT_SIZE_VAL);
    std::string fontSize = fontSizeData.ToString();
    if (!fontSizeArg->IsNull() && !fontSizeArg->IsUndefined() &&
        ArkTSUtils::ParseJsDimensionFp(vm, fontSizeArg, fontSizeData) && !fontSizeData.IsNegative() &&
        fontSizeData.Unit() != DimensionUnit::PERCENT) {
        fontSize = fontSizeData.ToString();
    }
    std::string weight = "normal";
    if (!weightArg->IsNull() && !weightArg->IsUndefined() && (weightArg->IsString(vm) || weightArg->IsNumber())) {
        weight = weightArg->ToString(vm)->ToString(vm);
    }
    std::string fontFamily;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily) || fontFamily.empty()) {
        fontFamily = DEFAULT_FAMILY;
    }
    int32_t styleVal = 0;
    if (!styleValArg->IsNull() && !styleValArg->IsUndefined() && styleValArg->IsNumber()) {
        styleVal = styleValArg->Int32Value(vm);
    }
    std::string fontInfo =
        StringUtils::FormatString(FORMAT_FONT.c_str(), fontSize.c_str(), weight.c_str(), fontFamily.c_str());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setSelectedFont(nativeNode, fontInfo.c_str(), styleVal);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetSelectedFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetSelectedFont(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> weightArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(NUM_3);
    Local<JSValueRef> styleValArg = runtimeCallInfo->GetCallArgRef(NUM_4);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    bool isUndefinedValue = (fontSizeArg->IsNull() || fontSizeArg->IsUndefined()) &&
                            (weightArg->IsNull() || weightArg->IsUndefined()) &&
                            (fontFamilyArg->IsNull() || fontFamilyArg->IsUndefined()) &&
                            (styleValArg->IsNull() || styleValArg->IsUndefined());
    if (isUndefinedValue) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupFont(nativeNode);
    }
    CalcDimension fontSizeData(Dimension(DEFAULT_POPUP_ITEM_FONT_SIZE, DimensionUnit::FP));
    std::string fontSize = fontSizeData.ToString();
    if (!fontSizeArg->IsNull() && !fontSizeArg->IsUndefined() &&
        ArkTSUtils::ParseJsDimensionFp(vm, fontSizeArg, fontSizeData) && !fontSizeData.IsNegative() &&
        fontSizeData.Unit() != DimensionUnit::PERCENT) {
        fontSize = fontSizeData.ToString();
    }
    std::string weight = "normal";
    if (!weightArg->IsNull() && !weightArg->IsUndefined() && (weightArg->IsString(vm) || weightArg->IsNumber())) {
        weight = weightArg->ToString(vm)->ToString(vm);
    }
    std::string fontFamily;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily) || fontFamily.empty()) {
        fontFamily = DEFAULT_FAMILY;
    }
    int32_t styleVal = 0;
    if (!styleValArg->IsNull() && !styleValArg->IsUndefined() && styleValArg->IsNumber()) {
        styleVal = styleValArg->Int32Value(vm);
    }
    std::string fontInfo =
        StringUtils::FormatString(FORMAT_FONT.c_str(), fontSize.c_str(), weight.c_str(), fontFamily.c_str());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupFont(nativeNode, fontInfo.c_str(), styleVal);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupFont(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> weightArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(NUM_3);
    Local<JSValueRef> styleValArg = runtimeCallInfo->GetCallArgRef(NUM_4);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    bool isUndefinedValue = (fontSizeArg->IsNull() || fontSizeArg->IsUndefined()) &&
                            (weightArg->IsNull() || weightArg->IsUndefined()) &&
                            (fontFamilyArg->IsNull() || fontFamilyArg->IsUndefined()) &&
                            (styleValArg->IsNull() || styleValArg->IsUndefined());
    if (isUndefinedValue) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerFont(nativeNode);
    }
    CalcDimension fontSizeData(DEFAULT_FONT_SIZE_VAL);
    std::string fontSize = fontSizeData.ToString();
    if (!fontSizeArg->IsNull() && !fontSizeArg->IsUndefined() &&
        ArkTSUtils::ParseJsDimensionFp(vm, fontSizeArg, fontSizeData) && !fontSizeData.IsNegative() &&
        fontSizeData.Unit() != DimensionUnit::PERCENT) {
        fontSize = fontSizeData.ToString();
    }
    std::string weight = "normal";
    if (!weightArg->IsNull() && !weightArg->IsUndefined() && (weightArg->IsString(vm) || weightArg->IsNumber())) {
        weight = weightArg->ToString(vm)->ToString(vm);
    }
    std::string fontFamily;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily) || fontFamily.empty()) {
        fontFamily = DEFAULT_FAMILY;
    }
    int32_t styleVal = 0;
    if (!styleValArg->IsNull() && !styleValArg->IsUndefined() && styleValArg->IsNumber()) {
        styleVal = styleValArg->Int32Value(vm);
    }
    std::string fontInfo =
        StringUtils::FormatString(FORMAT_FONT.c_str(), fontSize.c_str(), weight.c_str(), fontFamily.c_str());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setAlphabetIndexerFont(
        nativeNode, fontInfo.c_str(), styleVal);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerFont(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupItemBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupItemBackgroundColor(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupItemBackgroundColor(
            nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupItemBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupItemBackgroundColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerColor(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setAlphabetIndexerColor(nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupColor(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupColor(nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetSelectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerSelectedColor(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setAlphabetIndexerSelectedColor(
            nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetSelectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerSelectedColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupBackground(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupBackground(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupBackground(nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupBackground(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupBackground(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetSelectedBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetSelectedBackgroundColor(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setSelectedBackgroundColor(
            nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetSelectedBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetSelectedBackgroundColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupUnselectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupUnselectedColor(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupUnselectedColor(nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupUnselectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupUnselectedColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetAlignStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> thirdArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    if (!secondArg->IsNumber()) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlignStyle(nativeNode);
    } else {
        int32_t alignValue = secondArg->Int32Value(vm);
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setAlignStyle(nativeNode, alignValue);
    }
    CalcDimension popupHorizontalSpace;
    if (!thirdArg->IsNull() && !thirdArg->IsUndefined() &&
        ArkTSUtils::ParseJsDimensionVp(vm, thirdArg, popupHorizontalSpace)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupHorizontalSpace(
            nativeNode, popupHorizontalSpace.Value(), static_cast<int>(popupHorizontalSpace.Unit()));
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupHorizontalSpace(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetAlignStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlignStyle(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupSelectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, secondArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupSelectedColor(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupSelectedColor(nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupSelectedColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupSelectedColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetUsingPopup(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> usingPopupArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (usingPopupArg->IsNull() || usingPopupArg->IsUndefined()) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetUsingPopup(nativeNode);
    } else {
        bool usingPopup = usingPopupArg->ToBoolean(vm)->Value();
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setUsingPopup(nativeNode, usingPopup);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetUsingPopup(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetUsingPopup(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetSelected(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> selectedArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    int32_t selected = 0;
    if (selectedArg->IsNull() || selectedArg->IsUndefined() || !ArkTSUtils::ParseJsInteger(vm, selectedArg, selected)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerSelected(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setAlphabetIndexerSelected(nativeNode, selected);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetSelected(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAlphabetIndexerSelected(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetItemSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> itemSizeArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    CalcDimension itemSize;

    if (itemSizeArg->IsNull() || itemSizeArg->IsUndefined()) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetItemSize(nativeNode);
    }

    if (ArkTSUtils::ParseJsDimensionVp(vm, itemSizeArg, itemSize) && GreatNotEqual(itemSize.Value(), 0.0) &&
        itemSize.Unit() != DimensionUnit::PERCENT) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setItemSize(
            nativeNode, itemSize.Value(), static_cast<int>(itemSize.Unit()));
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetItemSize(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetItemSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetItemSize(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupPosition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> sizeX = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> sizeY = runtimeCallInfo->GetCallArgRef(NUM_2);
    CalcDimension x;
    CalcDimension y;
    if (sizeX->IsNull() || sizeX->IsUndefined() || !ArkTSUtils::ParseJsDimensionVp(vm, sizeX, x)) {
        x = DEFAULT_POPUP_POSITION_X;
    }
    if (sizeY->IsNull() || sizeY->IsUndefined() || !ArkTSUtils::ParseJsDimensionVp(vm, sizeY, y)) {
        y = DEFAULT_POPUP_POSITION_Y;
    }
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupPosition(
        nativeNode, x.Value(), static_cast<int>(x.Unit()), y.Value(), static_cast<int>(y.Unit()));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupPosition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupPosition(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupItemBorderRadius(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> radiusArg = runtimeCallInfo->GetCallArgRef(NUM_1);

    CalcDimension radius;
    CalcDimension popupRadius;
    if (radiusArg->IsNull() || radiusArg->IsUndefined() || !radiusArg->IsNumber()) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupItemBorderRadius(nativeNode);
    } else {
        ArkTSUtils::ParseJsDimensionVp(vm, radiusArg, radius);
        popupRadius.SetValue(radius.Value() + RADIUS_OFFSET);
        popupRadius.SetUnit(DimensionUnit::VP);
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupItemBorderRadius(nativeNode, radius.Value(),
            static_cast<int>(radius.Unit()), popupRadius.Value(), static_cast<int>(popupRadius.Unit()));
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupItemBorderRadius(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupItemBorderRadius(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetItemBorderRadius(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> radiusArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    auto radius = CalcDimension(ZERO_RADIUS, DimensionUnit::VP);
    auto indexerRadius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    if (radiusArg->IsNull() || radiusArg->IsUndefined() || !radiusArg->IsNumber()) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetItemBorderRadius(nativeNode);
    } else {
        ArkTSUtils::ParseJsDimensionVp(vm, radiusArg, radius);
        indexerRadius.SetValue(radius.Value() + RADIUS_OFFSET);
        indexerRadius.SetUnit(DimensionUnit::VP);
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setItemBorderRadius(nativeNode, radius.Value(),
            static_cast<int>(radius.Unit()), indexerRadius.Value(), static_cast<int>(indexerRadius.Unit()));
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetItemBorderRadius(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetItemBorderRadius(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupBackgroundBlurStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> blurStyleArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    int32_t blurStyle = blurStyleArg->Int32Value(vm);
    if (!ArkTSUtils::ParseJsInteger(vm, blurStyleArg, blurStyle)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupBackgroundBlurStyle(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupBackgroundBlurStyle(nativeNode, blurStyle);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupBackgroundBlurStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupBackgroundBlurStyle(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
ArkUINativeModuleValue AlphabetIndexerBridge::SetPopupTitleBackground(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, color)) {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupTitleBackground(nativeNode);
    } else {
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setPopupTitleBackground(nativeNode, color.GetValue());
    }
    return panda::JSValueRef::Undefined(vm);
}
ArkUINativeModuleValue AlphabetIndexerBridge::ResetPopupTitleBackground(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetPopupTitleBackground(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetAdaptiveWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);

    CalcDimension width;
    ArkTSUtils::ParseJsDimensionVpNG(vm, secondArg, width);
    if (width.Unit() == DimensionUnit::AUTO) {
        CommonBridge::SetWidth(runtimeCallInfo);
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setAdaptiveWidth(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    } else {
        CommonBridge::SetWidth(runtimeCallInfo);
        GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAdaptiveWidth(nativeNode);
    }
    CommonBridge::SetWidth(runtimeCallInfo);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetAdaptiveWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    CommonBridge::ResetWidth(runtimeCallInfo);
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAdaptiveWidth(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetAutoCollapse(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> autoCollapseArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    bool autoCollapse = true;
    if (autoCollapseArg->IsBoolean()) {
        autoCollapse = autoCollapseArg->ToBoolean(vm)->Value();
    }
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setAutoCollapse(nativeNode, autoCollapse);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetAutoCollapse(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetAutoCollapse(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::SetEnableHapticFeedback(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> hapticFeedbackArg = runtimeCallInfo->GetCallArgRef(1);
    CHECK_NULL_RETURN(nodeArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    bool isHapticEnable = true;
    if (hapticFeedbackArg->IsBoolean()) {
        isHapticEnable = hapticFeedbackArg->ToBoolean(vm)->Value();
    }
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->setEnableHapticFeedback(nativeNode, isHapticEnable);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue AlphabetIndexerBridge::ResetEnableHapticFeedback(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    CHECK_NULL_RETURN(firstArg->IsNativePointer(vm), panda::JSValueRef::Undefined(vm));
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getAlphabetIndexerModifier()->resetEnableHapticFeedback(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
} // namespace OHOS::Ace::NG

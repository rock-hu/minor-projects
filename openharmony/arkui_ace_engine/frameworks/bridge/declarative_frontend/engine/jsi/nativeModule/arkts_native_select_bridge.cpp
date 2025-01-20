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
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_select_bridge.h"

#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "bridge/declarative_frontend/jsview/js_symbol_modifier.h"
#include "core/components_ng/base/frame_node.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int NUM_0 = 0;
constexpr int NUM_1 = 1;
constexpr int NUM_3 = 3;
const int32_t SIZE_OF_TWO = 2;
const std::string FORMAT_FONT = "%s|%s|%s";
const std::string DEFAULT_STR = "-1";
const char* SELECT_NODEPTR_OF_UINODE = "nodePtr_";

constexpr int32_t ARG_GROUP_LENGTH = 3;
bool ParseDividerDimension(const EcmaVM* vm, const Local<JSValueRef>& value, CalcDimension& valueDim)
{
    return !ArkTSUtils::ParseJsDimensionVpNG(vm, value, valueDim, false) || LessNotEqual(valueDim.Value(), 0.0f) ||
           (valueDim.Unit() != DimensionUnit::PX && valueDim.Unit() != DimensionUnit::VP &&
           valueDim.Unit() != DimensionUnit::LPX && valueDim.Unit() != DimensionUnit::FP);
}

void PopulateValues(const CalcDimension& dividerStrokeWidth, const CalcDimension& dividerStartMargin,
    const CalcDimension& dividerEndMargin, ArkUI_Float32 values[], uint32_t size)
{
    values[0] = static_cast<ArkUI_Float32>(dividerStrokeWidth.Value());
    values[1] = static_cast<ArkUI_Float32>(dividerStartMargin.Value());
    values[2] = static_cast<ArkUI_Float32>(dividerEndMargin.Value());
}

void PopulateUnits(const CalcDimension& dividerStrokeWidth, const CalcDimension& dividerStartMargin,
    const CalcDimension& dividerEndMargin, int32_t units[], uint32_t size)
{
    units[0] = static_cast<int32_t>(dividerStrokeWidth.Unit());
    units[1] = static_cast<int32_t>(dividerStartMargin.Unit());
    units[2] = static_cast<int32_t>(dividerEndMargin.Unit());
}
} // namespace

panda::Local<panda::JSValueRef> JsSelectChangeCallback(panda::JsiRuntimeCallInfo* runtimeCallInfo)
{
    auto vm = runtimeCallInfo->GetVM();
    auto firstArg = runtimeCallInfo->GetCallArgRef(0);
    auto secondArg = runtimeCallInfo->GetCallArgRef(1);
    if (!firstArg->IsNumber()) {
        return panda::JSValueRef::Undefined(vm);
    }
    auto index = firstArg->ToNumber(vm)->Value();
    auto value = secondArg->ToString(vm)->ToString(vm);
    auto ref = runtimeCallInfo->GetThisRef();
    auto obj = ref->ToObject(vm);
    FrameNode* frameNode = nullptr;
    if (obj->GetNativePointerFieldCount(vm) < 1) {
        if (!ref->IsProxy(vm)) {
            return panda::JSValueRef::Undefined(vm);
        }
        auto frameNodeIdValue = obj->Get(vm, "frameNodeId_");
        CHECK_EQUAL_RETURN(
            frameNodeIdValue.IsEmpty() || frameNodeIdValue->IsNull(), true, panda::JSValueRef::Undefined(vm));
        auto frameNodeId = frameNodeIdValue->Int32Value(vm);
        frameNode = ElementRegister::GetInstance()->GetFrameNodePtrById(frameNodeId);
    } else {
        frameNode = static_cast<FrameNode*>(obj->GetNativePointerField(vm, 0));
    }
    CHECK_NULL_RETURN(frameNode, panda::JSValueRef::Undefined(vm));
    SelectModelNG::SetChangeValue(frameNode, index, value);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SelectBridge::SetSpace(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, panda::NativePointerRef::New(vm, nullptr));
    auto selectTheme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_RETURN(selectTheme, panda::NativePointerRef::New(vm, nullptr));

    CalcDimension space;
    if (!ArkTSUtils::ParseJsDimensionVp(vm, secondArg, space)) {
        space = selectTheme->GetContentSpinnerPadding();
    }
    if (LessNotEqual(space.Value(), 0.0) || space.Unit() == DimensionUnit::PERCENT) {
        space = selectTheme->GetContentSpinnerPadding();
    }

    GetArkUINodeModifiers()->getSelectModifier()->setSpace(
        nativeNode, space.Value(), static_cast<int32_t>(space.Unit()));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetValue(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    std::string value;
    ArkTSUtils::ParseJsString(vm, secondArg, value);
    GetArkUINodeModifiers()->getSelectModifier()->setValue(nativeNode, value.c_str());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetSelected(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    int32_t value = 0;
    if (ArkTSUtils::ParseJsIntegerWithResource(vm, secondArg, value)) {
        GetArkUINodeModifiers()->getSelectModifier()->setSelected(nativeNode, value);
    } else {
        GetArkUINodeModifiers()->getSelectModifier()->resetSelected(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetFontColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());

    Color fontColor;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, fontColor)) {
        return ResetFontColor(runtimeCallInfo);
    }
    GetArkUINodeModifiers()->getSelectModifier()->setSelectFontColor(nativeNode, fontColor.GetValue());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetSelectedOptionBgColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());

    Color selectedOptionBgColor;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, selectedOptionBgColor)) {
        return ResetSelectedOptionBgColor(runtimeCallInfo);
    }

    GetArkUINodeModifiers()->getSelectModifier()->setSelectedOptionBgColor(
        nativeNode, selectedOptionBgColor.GetValue());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetOptionBgColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());

    Color optionBgColor;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, optionBgColor)) {
        return ResetOptionBgColor(runtimeCallInfo);
    }
    GetArkUINodeModifiers()->getSelectModifier()->setOptionBgColor(nativeNode, optionBgColor.GetValue());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetOptionFontColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());

    Color optionFontColor;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, optionFontColor)) {
        return ResetOptionFontColor(runtimeCallInfo);
    }
    GetArkUINodeModifiers()->getSelectModifier()->setOptionFontColor(nativeNode, optionFontColor.GetValue());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetSelectedOptionFontColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Color optionFontColor;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, optionFontColor)) {
        return ResetSelectedOptionFontColor(runtimeCallInfo);
    }
    GetArkUINodeModifiers()->getSelectModifier()->setSelectedOptionFontColor(nativeNode, optionFontColor.GetValue());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SelectBridge::SetArrowPosition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    int32_t arrowPosition = secondArg->Int32Value(vm);
    GetArkUINodeModifiers()->getSelectModifier()->setArrowPosition(nativeNode, arrowPosition);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetMenuAlign(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> alignTypeArg = runtimeCallInfo->GetCallArgRef(1); // 1: index of alignType
    Local<JSValueRef> offsetDx = runtimeCallInfo->GetCallArgRef(2);     // 2: index of offset Dx
    Local<JSValueRef> offsetDy = runtimeCallInfo->GetCallArgRef(3);     // 3: index of offset Dy
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());

    int32_t alignType = 0;
    if (alignTypeArg->IsNumber()) {
        alignType = alignTypeArg->Int32Value(vm);
    }

    CalcDimension menuAlignOffsetDx = Dimension(0.0);
    CalcDimension menuAlignOffsetDy = Dimension(0.0);
    if (offsetDx->IsNull() || !ArkTSUtils::ParseJsDimensionVp(vm, offsetDx, menuAlignOffsetDx)) {
        menuAlignOffsetDx = Dimension(0.0);
    }

    if (offsetDy->IsNull() || !ArkTSUtils::ParseJsDimensionVp(vm, offsetDy, menuAlignOffsetDy)) {
        menuAlignOffsetDy = Dimension(0.0);
    }

    uint32_t size = SIZE_OF_TWO;
    float values[size];
    int32_t units[size];
    values[0] = menuAlignOffsetDx.Value();
    units[0] = static_cast<int32_t>(menuAlignOffsetDx.Unit());
    values[1] = menuAlignOffsetDy.Value();
    units[1] = static_cast<int32_t>(menuAlignOffsetDy.Unit());
    GetArkUINodeModifiers()->getSelectModifier()->setMenuAlign(nativeNode, alignType, values, units, SIZE_OF_TWO);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(1);   // 1： index of font size value
    Local<JSValueRef> fontWeightArg = runtimeCallInfo->GetCallArgRef(2); // 2: index of font weight value
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(3); // 3: index of font family value
    Local<JSValueRef> styleArg = runtimeCallInfo->GetCallArgRef(4);      // 4: index of font style value
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());

    std::string fontSizeStr = DEFAULT_STR;
    CalcDimension fontSize;
    if (!fontSizeArg->IsNull() && !fontSizeArg->IsUndefined() &&
        ArkTSUtils::ParseJsDimensionFp(vm, fontSizeArg, fontSize)) {
        fontSizeStr = fontSize.ToString();
    }

    std::string fontWeight = DEFAULT_STR;
    if (!fontWeightArg->IsNull() && !fontWeightArg->IsUndefined()) {
        if (fontWeightArg->IsNumber()) {
            fontWeight = std::to_string(fontWeightArg->Int32Value(vm));
        } else {
            if (!ArkTSUtils::ParseJsString(vm, fontWeightArg, fontWeight) || fontWeight.empty()) {
                fontWeight = DEFAULT_STR;
            }
        }
    }
    std::string fontFamily;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily) || fontFamily.empty()) {
        fontFamily = DEFAULT_STR;
    }
    int32_t styleVal = 0;
    if (!styleArg->IsNull() && !styleArg->IsUndefined() && styleArg->IsNumber()) {
        styleVal = styleArg->Int32Value(vm);
    }

    std::string fontInfo =
        StringUtils::FormatString(FORMAT_FONT.c_str(), fontSizeStr.c_str(), fontWeight.c_str(), fontFamily.c_str());

    GetArkUINodeModifiers()->getSelectModifier()->setFont(nativeNode, fontInfo.c_str(), styleVal);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetOptionFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(1);   // 1： index of font size value
    Local<JSValueRef> fontWeightArg = runtimeCallInfo->GetCallArgRef(2); // 2: index of font weight value
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(3); // 3: index of font family value
    Local<JSValueRef> styleArg = runtimeCallInfo->GetCallArgRef(4);      // 4: index of font style value
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, panda::NativePointerRef::New(vm, nullptr));
    auto selectTheme = pipeline->GetTheme<SelectTheme>();

    CalcDimension fontSize;
    if (fontSizeArg->IsNull() || fontSizeArg->IsUndefined() ||
        !ArkTSUtils::ParseJsDimensionFp(vm, fontSizeArg, fontSize)) {
        fontSize = selectTheme->GetMenuFontSize();
    }

    std::string fontWeight = DEFAULT_STR;
    if (!fontWeightArg->IsNull() && !fontWeightArg->IsUndefined()) {
        if (fontWeightArg->IsNumber()) {
            fontWeight = std::to_string(fontWeightArg->Int32Value(vm));
        } else {
            if (!ArkTSUtils::ParseJsString(vm, fontWeightArg, fontWeight) || fontWeight.empty()) {
                fontWeight = DEFAULT_STR;
            }
        }
    }
    std::string fontFamily;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily) || fontFamily.empty()) {
        fontFamily = DEFAULT_STR;
    }
    int32_t styleVal = 0;
    if (!styleArg->IsNull() && !styleArg->IsUndefined() && styleArg->IsNumber()) {
        styleVal = styleArg->Int32Value(vm);
    }
    std::string fontSizeStr = fontSize.ToString();
    std::string fontInfo =
        StringUtils::FormatString(FORMAT_FONT.c_str(), fontSizeStr.c_str(), fontWeight.c_str(), fontFamily.c_str());

    GetArkUINodeModifiers()->getSelectModifier()->setOptionFont(nativeNode, fontInfo.c_str(), styleVal);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetSelectedOptionFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(1);   // 1： index of font size value
    Local<JSValueRef> fontWeightArg = runtimeCallInfo->GetCallArgRef(2); // 2: index of font weight value
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(3); // 3: index of font family value
    Local<JSValueRef> styleArg = runtimeCallInfo->GetCallArgRef(4);      // 4: index of font style value
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());

    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, panda::NativePointerRef::New(vm, nullptr));
    auto selectTheme = pipeline->GetTheme<SelectTheme>();

    CalcDimension fontSize;
    if (fontSizeArg->IsNull() || fontSizeArg->IsUndefined() ||
        !ArkTSUtils::ParseJsDimensionFp(vm, fontSizeArg, fontSize)) {
        fontSize = selectTheme->GetFontSize();
    }

    std::string fontWeight = DEFAULT_STR;
    if (!fontWeightArg->IsNull() && !fontWeightArg->IsUndefined()) {
        if (fontWeightArg->IsNumber()) {
            fontWeight = std::to_string(fontWeightArg->Int32Value(vm));
        } else {
            if (!ArkTSUtils::ParseJsString(vm, fontWeightArg, fontWeight) || fontWeight.empty()) {
                fontWeight = DEFAULT_STR;
            }
        }
    }
    std::string fontFamily;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily) || fontFamily.empty()) {
        fontFamily = DEFAULT_STR;
    }
    int32_t styleVal = 0;
    if (!styleArg->IsNull() && !styleArg->IsUndefined() && styleArg->IsNumber()) {
        styleVal = styleArg->Int32Value(vm);
    }
    std::string fontSizeStr = fontSize.ToString();
    std::string fontInfo =
        StringUtils::FormatString(FORMAT_FONT.c_str(), fontSizeStr.c_str(), fontWeight.c_str(), fontFamily.c_str());

    GetArkUINodeModifiers()->getSelectModifier()->setSelectedOptionFont(nativeNode, fontInfo.c_str(), styleVal);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetSpace(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSpace(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetValue(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetValue(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetSelected(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelected(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetFontColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectFontColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetSelectedOptionBgColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectedOptionBgColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetOptionBgColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetOptionBgColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetOptionFontColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetOptionFontColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetSelectedOptionFontColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectedOptionFontColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetFont(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetOptionFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetOptionFont(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetSelectedOptionFont(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectedOptionFont(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetMenuAlign(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetMenuAlign(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetArrowPosition(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetArrowPosition(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetOptionWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> optionWidthArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    CalcDimension width;
    if (optionWidthArg->IsString(vm)) {
        std::string modeFlag = optionWidthArg->ToString(vm)->ToString(vm);
        if (modeFlag.compare("fit_content") == 0) {
            GetArkUINodeModifiers()->getSelectModifier()->setOptionWidthFitTrigger(nativeNode, false);
            return panda::JSValueRef::Undefined(vm);
        } else if (modeFlag.compare("fit_trigger") == 0) {
            GetArkUINodeModifiers()->getSelectModifier()->setOptionWidthFitTrigger(nativeNode, true);
            return panda::JSValueRef::Undefined(vm);
        } else if (ArkTSUtils::IsPercentStr(modeFlag)) {
            return panda::JSValueRef::Undefined(vm);
        } else {
            ArkTSUtils::ParseJsDimensionVpNG(vm, optionWidthArg, width, false);
            if (width.IsNegative()) {
                width.Reset();
            }
        }
    } else {
        ArkTSUtils::ParseJsDimensionVpNG(vm, optionWidthArg, width, false);
        if (width.IsNegative()) {
            width.Reset();
        }
    }

    GetArkUINodeModifiers()->getSelectModifier()->setSelectOptionWidth(
        nativeNode, width.Value(), static_cast<int32_t>(width.Unit()));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetOptionWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectOptionWidth(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetOptionHeight(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> optionHeightArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    CalcDimension height;
    if (optionHeightArg->IsString(vm)) {
        std::string modeFlag = optionHeightArg->ToString(vm)->ToString(vm);
        if (ArkTSUtils::IsPercentStr(modeFlag)) {
            return panda::JSValueRef::Undefined(vm);
        }
    }

    ArkTSUtils::ParseJsDimensionVpNG(vm, optionHeightArg, height, false);
    if (height.IsNegative()) {
        return panda::JSValueRef::Undefined(vm);
    }
    if (NEAR_ZERO(height.Value())) {
        return panda::JSValueRef::Undefined(vm);
    }

    GetArkUINodeModifiers()->getSelectModifier()->setSelectOptionHeight(
        nativeNode, height.Value(), static_cast<int32_t>(height.Unit()));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetOptionHeight(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectOptionHeight(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> widthArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    CalcDimension width;
    if (!ArkTSUtils::ParseJsDimensionVpNG(vm, widthArg, width)) {
        GetArkUINodeModifiers()->getSelectModifier()->resetSelectWidth(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    if (LessNotEqual(width.Value(), 0.0)) {
        width.SetValue(0.0);
    }
    std::string widthCalc = width.CalcValue();
    GetArkUINodeModifiers()->getSelectModifier()->setSelectWidth(
        nativeNode, width.Value(), static_cast<int32_t>(width.Unit()), widthCalc.c_str());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetWidth(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectWidth(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetHeight(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> heightArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    CalcDimension height;
    if (!ArkTSUtils::ParseJsDimensionVpNG(vm, heightArg, height)) {
        GetArkUINodeModifiers()->getSelectModifier()->resetSelectHeight(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    if (LessNotEqual(height.Value(), 0.0)) {
        height.SetValue(0.0);
    }
    std::string heightCalc = height.CalcValue();
    GetArkUINodeModifiers()->getSelectModifier()->setSelectHeight(
        nativeNode, height.Value(), static_cast<int32_t>(height.Unit()), heightCalc.c_str());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetHeight(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectHeight(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> widthArg = runtimeCallInfo->GetCallArgRef(1);  // 1: index of width value
    Local<JSValueRef> heightArg = runtimeCallInfo->GetCallArgRef(2); // 2: index of heigth value
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    CalcDimension width;
    CalcDimension height;
    if (!ArkTSUtils::ParseJsDimensionVpNG(vm, widthArg, width)) {
        GetArkUINodeModifiers()->getSelectModifier()->resetSelectWidth(nativeNode);
    } else {
        std::string widthCalc = width.CalcValue();
        if (LessNotEqual(width.Value(), 0.0)) {
            width.SetValue(0.0);
        }
        GetArkUINodeModifiers()->getSelectModifier()->setSelectWidth(
            nativeNode, width.Value(), static_cast<int32_t>(width.Unit()), widthCalc.c_str());
    }
    if (!ArkTSUtils::ParseJsDimensionVpNG(vm, heightArg, height)) {
        GetArkUINodeModifiers()->getSelectModifier()->resetSelectHeight(nativeNode);
    } else {
        std::string heightCalc = height.CalcValue();
        if (LessNotEqual(height.Value(), 0.0)) {
            height.SetValue(0.0);
        }
        GetArkUINodeModifiers()->getSelectModifier()->setSelectHeight(
            nativeNode, height.Value(), static_cast<int32_t>(height.Unit()), heightCalc.c_str());
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetControlSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> controlSizeArg = runtimeCallInfo->GetCallArgRef(1);  // 1: index of width value
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto controlSize = controlSizeArg->Int32Value(vm);
    if (!ArkTSUtils::ParseJsInteger(vm, controlSizeArg, controlSize)) {
        GetArkUINodeModifiers()->getSelectModifier()->resetControlSize(nativeNode);
    } else {
        GetArkUINodeModifiers()->getSelectModifier()->setControlSize(nativeNode, controlSize);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetControlSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetControlSize(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetSize(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectSize(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

Local<panda::ObjectRef> SelectBridge::ConstructSelect(EcmaVM* vm, FrameNode* frameNode, MenuItemConfiguration& config)
{
    RefPtr<Framework::JSSymbolGlyphModifier> selectSymbol =
        AceType::DynamicCast<Framework::JSSymbolGlyphModifier>(config.symbolModifier_);
    const char* keysOfSelect[] = { "value", "icon", "symbolIcon", "selected", "index", "triggerSelect",
        "frameNodeId_"};
    Local<JSValueRef> valuesOfSelect[] = { panda::StringRef::NewFromUtf8(vm, config.value_.c_str()),
        panda::StringRef::NewFromUtf8(vm, config.icon_.c_str()),
        selectSymbol->symbol_->GetLocalHandle(),
        panda::BooleanRef::New(vm, config.selected_),
        panda::NumberRef::New(vm, config.index_),
        panda::FunctionRef::New(vm, JsSelectChangeCallback),
        panda::NumberRef::New(vm, frameNode->GetId())};
    return panda::ObjectRef::NewWithNamedProperties(vm, ArraySize(keysOfSelect), keysOfSelect, valuesOfSelect);
}

ArkUINativeModuleValue SelectBridge::SetContentModifierBuilder(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(1);
    auto* frameNode = reinterpret_cast<FrameNode*>(firstArg->ToNativePointer(vm)->Value());
    if (!secondArg->IsObject(vm)) {
        SelectModelNG::ResetBuilderFunc(frameNode);
        return panda::JSValueRef::Undefined(vm);
    }
    panda::CopyableGlobal<panda::ObjectRef> obj(vm, secondArg);
    SelectModelNG::SetBuilderFunc(frameNode,
        [vm, frameNode, obj = std::move(obj), containerId = Container::CurrentId()](
            MenuItemConfiguration config) -> RefPtr<FrameNode> {
            ContainerScope scope(containerId);
            auto context = ArkTSUtils::GetContext(vm);
            CHECK_EQUAL_RETURN(context->IsUndefined(), true, nullptr);
            auto select = ConstructSelect(vm, frameNode, config);
            select->SetNativePointerFieldCount(vm, 1);
            select->SetNativePointerField(vm, 0, static_cast<void*>(frameNode));
            panda::Local<panda::JSValueRef> params[] = { context, select };
            LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            auto jsObject = obj.ToLocal();
            auto makeFunc = jsObject->Get(vm, panda::StringRef::NewFromUtf8(vm, "makeContentModifierNode"));
            CHECK_EQUAL_RETURN(makeFunc->IsFunction(vm), false, nullptr);
            panda::Local<panda::FunctionRef> func = makeFunc;
            auto result = func->Call(vm, jsObject, params, 2);
            JSNApi::ExecutePendingJob(vm);
            CHECK_EQUAL_RETURN(result.IsEmpty() || trycatch.HasCaught() || !result->IsObject(vm), true, nullptr);
            auto resultObj = result->ToObject(vm);
            panda::Local<panda::JSValueRef> nodeptr =
                resultObj->Get(vm, panda::StringRef::NewFromUtf8(vm, SELECT_NODEPTR_OF_UINODE));
            CHECK_EQUAL_RETURN(nodeptr.IsEmpty() || nodeptr->IsUndefined() || nodeptr->IsNull(), true, nullptr);
            auto* frameNode = reinterpret_cast<FrameNode*>(nodeptr->ToNativePointer(vm)->Value());
            CHECK_NULL_RETURN(frameNode, nullptr);
            return AceType::Claim(frameNode);
        });
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetMenuBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Color color;
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, color)) {
        return ResetMenuBackgroundColor(runtimeCallInfo);
    }
    GetArkUINodeModifiers()->getSelectModifier()->setMenuBgColor(nativeNode, color.GetValue());
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetMenuBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetMenuBgColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetMenuBackgroundBlurStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> styleArg = runtimeCallInfo->GetCallArgRef(1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (styleArg->IsUndefined() || !styleArg->IsNumber()) {
        return ResetMenuBackgroundBlurStyle(runtimeCallInfo);
    }
    int32_t styleVal = styleArg->Int32Value(vm);
    GetArkUINodeModifiers()->getSelectModifier()->setMenuBgBlurStyle(nativeNode, styleVal);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetMenuBackgroundBlurStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetMenuBgBlurStyle(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetDivider(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> dividerStrokeWidthArgs = runtimeCallInfo->GetCallArgRef(1);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(2);
    Local<JSValueRef> dividerStartMarginArgs = runtimeCallInfo->GetCallArgRef(3);
    Local<JSValueRef> dividerEndMarginArgs = runtimeCallInfo->GetCallArgRef(4);
    CalcDimension dividerStrokeWidth;
    CalcDimension dividerStartMargin;
    CalcDimension dividerEndMargin;
    Color colorObj;
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::NativePointerRef::New(vm, nullptr));
    auto context = frameNode->GetContext();
    CHECK_NULL_RETURN(context, panda::NativePointerRef::New(vm, nullptr));
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_RETURN(themeManager, panda::NativePointerRef::New(vm, nullptr));
    auto selectTheme = themeManager->GetTheme<SelectTheme>();
    CHECK_NULL_RETURN(selectTheme, panda::NativePointerRef::New(vm, nullptr));
    if (ParseDividerDimension(vm, dividerStrokeWidthArgs, dividerStrokeWidth)) {
        if (selectTheme) {
            dividerStrokeWidth = selectTheme->GetDefaultDividerWidth();
        } else {
            dividerStrokeWidth = 0.0_vp;
        }
    }
    if (!ArkTSUtils::ParseJsColorAlpha(vm, colorArg, colorObj)) {
        if (selectTheme) {
            colorObj = selectTheme->GetLineColor();
        } else {
            colorObj = Color::TRANSPARENT;
        }
    }
    if (ParseDividerDimension(vm, dividerStartMarginArgs, dividerStartMargin)) {
        dividerStartMargin = -1.0_vp;
    }
    if (ParseDividerDimension(vm, dividerEndMarginArgs, dividerEndMargin)) {
        dividerEndMargin = -1.0_vp;
    }
    uint32_t size = ARG_GROUP_LENGTH;
    ArkUI_Float32 values[size];
    int32_t units[size];
    PopulateValues(dividerStrokeWidth, dividerStartMargin, dividerEndMargin, values, size);
    PopulateUnits(dividerStrokeWidth, dividerStartMargin, dividerEndMargin, units, size);
    GetArkUINodeModifiers()->getSelectModifier()->setSelectDivider(
        nativeNode, colorObj.GetValue(), values, units, size);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetDivider(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    Local<JSValueRef> dividerStrokeWidthArgs = runtimeCallInfo->GetCallArgRef(1);
    if (dividerStrokeWidthArgs->IsNull()) {
        GetArkUINodeModifiers()->getSelectModifier()->resetSelectDividerNull(nativeNode);
    } else {
        GetArkUINodeModifiers()->getSelectModifier()->resetSelectDivider(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

static std::string ParseValues(const EcmaVM* vm, const Local<JSValueRef>& jsValue)
{
    std::string result;
    ArkTSUtils::ParseJsString(vm, jsValue, result);
    return result;
}

ArkUINativeModuleValue SelectBridge::SetOptions(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> valuesArg = runtimeCallInfo->GetCallArgRef(1);
    Local<JSValueRef> iconsArg = runtimeCallInfo->GetCallArgRef(2);
    Local<JSValueRef> symbolIconArg = runtimeCallInfo->GetCallArgRef(3);
    Local<JSValueRef> lengthArg = runtimeCallInfo->GetCallArgRef(4);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto length = lengthArg->Uint32Value(vm);
    auto valueArray = std::make_unique<std::string[]>(length);
    auto iconArray = std::make_unique<std::string[]>(length);
    auto selectParamArray = std::make_unique<SelectParam[]>(length);

    bool valueParseResult = ArkTSUtils::ParseArray<std::string>(vm, valuesArg, valueArray.get(), length, ParseValues);
    bool iconParseResult = ArkTSUtils::ParseArray<std::string>(
        vm, iconsArg, iconArray.get(), length, [](const EcmaVM* vm, const Local<JSValueRef>& jsValue) {
            std::string result;
            ArkTSUtils::ParseJsMedia(vm, jsValue, result);
            return result;
        });
    bool symbolIconParseResult = ArkTSUtils::ParseArray<SelectParam>(
        vm, symbolIconArg, selectParamArray.get(), length, [runtimeCallInfo](const EcmaVM* vm,
            const Local<JSValueRef>& jsValue) {
            auto selectSymbol = AceType::MakeRefPtr<Framework::JSSymbolGlyphModifier>();
            auto jsRefValue = Framework::JSRef<Framework::JsiValue>(Framework::JsiValue(jsValue));
            selectSymbol->symbol_ = jsRefValue;
            std::function<void(WeakPtr<NG::FrameNode>)> symbolIcoResult;
            RefPtr<SymbolModifier> symbolModifier;
            if (jsValue->IsObject(vm)) {
                std::function<void(WeakPtr<NG::FrameNode>)> symbolApply = nullptr;
                Framework::JSViewAbstract::SetSymbolOptionApply(runtimeCallInfo, symbolApply, jsRefValue);
                symbolIcoResult = symbolApply;
            }
            return SelectParam{.symbolIcon=symbolIcoResult, .symbolModifier= selectSymbol};
        });
    std::vector<SelectParam> params(length);
    if (valueParseResult && iconParseResult && symbolIconParseResult) {
        for (uint32_t i = 0; i < length; i++) {
            params[i].text = valueArray[i];
            params[i].symbolModifier = selectParamArray[i].symbolModifier;
            params[i].symbolIcon = selectParamArray[i].symbolIcon;
            if (!(selectParamArray[i].symbolIcon)) {
                params[i].icon = iconArray[i];
            }
        }
    }
    SelectModelNG::InitSelect(reinterpret_cast<FrameNode*>(nativeNode), params);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::SetSelectDirection(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    std::string dir;
    int32_t direction = NUM_3;
    if (secondArg->IsString(vm)) {
        dir = secondArg->ToString(vm)->ToString(vm);
        if (dir == "Ltr") {
            direction = NUM_0;
        } else if (dir == "Rtl") {
            direction = NUM_1;
        } else if (dir == "Auto") {
            direction = NUM_3;
        }
    }
    GetArkUINodeModifiers()->getSelectModifier()->setSelectDirection(nativeNode, direction);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue SelectBridge::ResetSelectDirection(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getSelectModifier()->resetSelectDirection(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
} // namespace OHOS::Ace::NG
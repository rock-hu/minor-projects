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
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_timepicker_bridge.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"
#include "frameworks/base/i18n/time_format.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"
#include "core/components/picker/picker_base_component.h"

namespace OHOS::Ace::NG {
namespace {
const std::string DEFAULT_ERR_CODE = "-1";
const std::string FORMAT_FONT = "%s|%s|%s";
constexpr int NUM_0 = 0;
constexpr int NUM_1 = 1;
constexpr int NUM_2 = 2;
constexpr int NUM_3 = 3;
constexpr int NUM_4 = 4;
constexpr int NUM_5 = 5;
constexpr uint32_t DEFAULT_TIME_PICKER_TEXT_COLOR = 0xFF182431;
constexpr uint32_t DEFAULT_TIME_PICKER_SELECTED_TEXT_COLOR = 0xFF0A59F7;
constexpr int PARAM_ARR_LENGTH_1 = 1;

std::string ParseFontSize(const EcmaVM* vm, const Local<JSValueRef>& fontSizeArg,
    RefPtr<ResourceObject>& fontSizeResObj)
{
    CalcDimension fontSizeData;
    if (fontSizeArg->IsNull() || fontSizeArg->IsUndefined()) {
        fontSizeData = Dimension(-1);
    } else {
        if (!ArkTSUtils::ParseJsDimensionNG(vm, fontSizeArg, fontSizeData, DimensionUnit::FP, fontSizeResObj, false)) {
            fontSizeData = Dimension(-1);
        }
    }
    return fontSizeData.ToString();
}

std::string ParseFontWeight(const EcmaVM* vm, const Local<JSValueRef>& fontWeightArg)
{
    std::string weight = DEFAULT_ERR_CODE;
    if (!fontWeightArg->IsNull() && !fontWeightArg->IsUndefined()) {
        if (fontWeightArg->IsNumber()) {
            weight = std::to_string(fontWeightArg->Int32Value(vm));
        } else {
            if (!ArkTSUtils::ParseJsString(vm, fontWeightArg, weight) || weight.empty()) {
                weight = DEFAULT_ERR_CODE;
            }
        }
    }
    return weight;
}

} // namespace

ArkUINativeModuleValue TimepickerBridge::SetTimepickerBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    uint32_t color = secondArg->Uint32Value(vm);
    GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerBackgroundColor(nativeNode, color);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetTimepickerBackgroundColor(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerBackgroundColor(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetTextStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    Local<JSValueRef> fontWeightArg = runtimeCallInfo->GetCallArgRef(NUM_3);
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(NUM_4);
    Local<JSValueRef> fontStyleArg = runtimeCallInfo->GetCallArgRef(NUM_5);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());

    ArkUIPickerTextStyleStruct textStyleStruct;
    Color color;
    RefPtr<ResourceObject> textColorResObj;
    auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
    if (colorArg->IsNull() || colorArg->IsUndefined() ||
        !ArkTSUtils::ParseJsColorAlpha(vm, colorArg, color, textColorResObj, nodeInfo)) {
        color.SetValue(DEFAULT_TIME_PICKER_TEXT_COLOR);
        textStyleStruct.textColorSetByUser = false;
    } else {
        textStyleStruct.textColorSetByUser = true;
    }

    RefPtr<ResourceObject> fontSizeResObj;
    std::string fontSizeStr = ParseFontSize(vm, fontSizeArg, fontSizeResObj);

    std::string weight = ParseFontWeight(vm, fontWeightArg);

    std::string fontFamily;
    RefPtr<ResourceObject> fontFamilyResObj;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily, fontFamilyResObj) ||
        fontFamily.empty()) {
        fontFamily = DEFAULT_ERR_CODE;
    }
    int32_t styleVal = 0;
    if (!fontStyleArg->IsNull() && !fontStyleArg->IsUndefined()) {
        styleVal = fontStyleArg->Int32Value(vm);
    }

    std::string fontInfo = StringUtils::FormatString(
        FORMAT_FONT.c_str(), fontSizeStr.c_str(), weight.c_str(), fontFamily.c_str());

    textStyleStruct.textColor = color.GetValue();
    textStyleStruct.fontStyle = styleVal;
    textStyleStruct.fontInfo = fontInfo.c_str();
    textStyleStruct.fontSizeRawPtr = AceType::RawPtr(fontSizeResObj);
    textStyleStruct.fontFamilyRawPtr = AceType::RawPtr(fontFamilyResObj);
    textStyleStruct.textColorRawPtr = AceType::RawPtr(textColorResObj);
    GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerTextStyleWithResObj(
        nativeNode, &textStyleStruct);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetSelectedTextStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    Local<JSValueRef> fontWeightArg = runtimeCallInfo->GetCallArgRef(NUM_3);
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(NUM_4);
    Local<JSValueRef> fontStyleArg = runtimeCallInfo->GetCallArgRef(NUM_5);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());

    ArkUIPickerTextStyleStruct textStyleStruct;
    Color color;
    RefPtr<ResourceObject> textColorResObj;
    auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
    if (colorArg->IsNull() || colorArg->IsUndefined() ||
        !ArkTSUtils::ParseJsColorAlpha(vm, colorArg, color, textColorResObj, nodeInfo)) {
        color.SetValue(DEFAULT_TIME_PICKER_SELECTED_TEXT_COLOR);
        textStyleStruct.textColorSetByUser = false;
    } else {
        textStyleStruct.textColorSetByUser = true;
    }

    RefPtr<ResourceObject> fontSizeResObj;
    std::string fontSizeStr = ParseFontSize(vm, fontSizeArg, fontSizeResObj);

    std::string weight = ParseFontWeight(vm, fontWeightArg);

    std::string fontFamily;
    RefPtr<ResourceObject> fontFamilyResObj;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily, fontFamilyResObj) ||
        fontFamily.empty()) {
        fontFamily = DEFAULT_ERR_CODE;
    }
    int32_t styleVal = 0;
    if (!fontStyleArg->IsNull() && !fontStyleArg->IsUndefined()) {
        styleVal = fontStyleArg->Int32Value(vm);
    }

    std::string fontInfo = StringUtils::FormatString(
        FORMAT_FONT.c_str(), fontSizeStr.c_str(), weight.c_str(), fontFamily.c_str());

    textStyleStruct.textColor = color.GetValue();
    textStyleStruct.fontStyle = styleVal;
    textStyleStruct.fontInfo = fontInfo.c_str();
    textStyleStruct.fontSizeRawPtr = AceType::RawPtr(fontSizeResObj);
    textStyleStruct.fontFamilyRawPtr = AceType::RawPtr(fontFamilyResObj);
    textStyleStruct.textColorRawPtr = AceType::RawPtr(textColorResObj);
    GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerSelectedTextStyleWithResObj(
        nativeNode, &textStyleStruct);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetDisappearTextStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> colorArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> fontSizeArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    Local<JSValueRef> fontWeightArg = runtimeCallInfo->GetCallArgRef(NUM_3);
    Local<JSValueRef> fontFamilyArg = runtimeCallInfo->GetCallArgRef(NUM_4);
    Local<JSValueRef> fontStyleArg = runtimeCallInfo->GetCallArgRef(NUM_5);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());

    ArkUIPickerTextStyleStruct textStyleStruct;
    Color color;
    RefPtr<ResourceObject> textColorResObj;
    auto nodeInfo = ArkTSUtils::MakeNativeNodeInfo(nativeNode);
    if (colorArg->IsNull() || colorArg->IsUndefined() ||
        !ArkTSUtils::ParseJsColorAlpha(vm, colorArg, color, textColorResObj, nodeInfo)) {
        color.SetValue(DEFAULT_TIME_PICKER_TEXT_COLOR);
        textStyleStruct.textColorSetByUser = false;
    } else {
        textStyleStruct.textColorSetByUser = true;
    }

    RefPtr<ResourceObject> fontSizeResObj;
    std::string fontSizeStr = ParseFontSize(vm, fontSizeArg, fontSizeResObj);

    std::string weight = ParseFontWeight(vm, fontWeightArg);

    std::string fontFamily;
    RefPtr<ResourceObject> fontFamilyResObj;
    if (!ArkTSUtils::ParseJsFontFamiliesToString(vm, fontFamilyArg, fontFamily, fontFamilyResObj) ||
        fontFamily.empty()) {
        fontFamily = DEFAULT_ERR_CODE;
    }
    int32_t styleVal = 0;
    if (!fontStyleArg->IsNull() && !fontStyleArg->IsUndefined()) {
        styleVal = fontStyleArg->Int32Value(vm);
    }

    std::string fontInfo = StringUtils::FormatString(
        FORMAT_FONT.c_str(), fontSizeStr.c_str(), weight.c_str(), fontFamily.c_str());

    textStyleStruct.textColor = color.GetValue();
    textStyleStruct.fontStyle = styleVal;
    textStyleStruct.fontInfo = fontInfo.c_str();
    textStyleStruct.fontSizeRawPtr = AceType::RawPtr(fontSizeResObj);
    textStyleStruct.fontFamilyRawPtr = AceType::RawPtr(fontFamilyResObj);
    textStyleStruct.textColorRawPtr = AceType::RawPtr(textColorResObj);
    GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerDisappearTextStyleWithResObj(
        nativeNode, &textStyleStruct);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetTextStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerTextStyle(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetSelectedTextStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerSelectedTextStyle(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetDisappearTextStyle(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerDisappearTextStyle(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetTimepickerUseMilitaryTime(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> useMilitaryArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (useMilitaryArg->IsBoolean()) {
        bool value = useMilitaryArg->ToBoolean(vm)->Value();
        GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerUseMilitaryTime(nativeNode, value);
    } else {
        GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerUseMilitaryTime(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetTimepickerUseMilitaryTime(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerUseMilitaryTime(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetTimepickerLoop(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> loopArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (loopArg->IsBoolean()) {
        bool value = loopArg->ToBoolean(vm)->Value();
        GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerLoop(nativeNode, value);
    } else {
        GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerLoop(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetTimepickerLoop(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerLoop(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetTimepickerDateTimeOptions(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> firstArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> hourArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    Local<JSValueRef> minuteArg = runtimeCallInfo->GetCallArgRef(NUM_2);
    Local<JSValueRef> secondArg = runtimeCallInfo->GetCallArgRef(NUM_3);
    auto nativeNode = nodePtr(firstArg->ToNativePointer(vm)->Value());
    ZeroPrefixType hourType = ZeroPrefixType::AUTO;
    ZeroPrefixType minuteType = ZeroPrefixType::AUTO;
    ZeroPrefixType secondType = ZeroPrefixType::AUTO;
    std::string hour = "hour";
    if (hourArg->IsString(vm)) {
        std::string hour = hourArg->ToString(vm)->ToString(vm);
        if (hour == "2-digit") {
            hourType = ZeroPrefixType::SHOW;
        } else if (hour == "numeric") {
            hourType = ZeroPrefixType::HIDE;
        }
    }
    std::string minute = "minute";
    if (minuteArg->IsString(vm)) {
        minuteType = ZeroPrefixType::SHOW;
        std::string minute = minuteArg->ToString(vm)->ToString(vm);
        if (minute == "numeric") {
            minuteType = ZeroPrefixType::HIDE;
        }
    }
    std::string second = "second";
    if (secondArg->IsString(vm)) {
        secondType = ZeroPrefixType::SHOW;
        std::string second = secondArg->ToString(vm)->ToString(vm);
        if (second == "numeric") {
            secondType = ZeroPrefixType::HIDE;
        }
    }
    GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerDateTimeOptions(
        nativeNode, static_cast<ArkUI_Int32>(hourType), static_cast<ArkUI_Int32>(minuteType),
        static_cast<ArkUI_Int32>(secondType));
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetTimepickerDateTimeOptions(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerDateTimeOptions(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetTimepickerEnableHapticFeedback(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> enableHapticFeedbackArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (enableHapticFeedbackArg->IsBoolean()) {
        bool value = enableHapticFeedbackArg->ToBoolean(vm)->Value();
        GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerEnableHapticFeedback(nativeNode, value);
    } else {
        GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerEnableHapticFeedback(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetTimepickerEnableHapticFeedback(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerEnableHapticFeedback(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetTimepickerEnableCascade(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0); // 0: index of parameter frameNode
    Local<JSValueRef> enableCascadeArg = runtimeCallInfo->GetCallArgRef(1); // 1: index of parameter enableCascade
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    if (enableCascadeArg->IsBoolean()) {
        bool value = enableCascadeArg->ToBoolean(vm)->Value();
        GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerEnableCascade(nativeNode, value);
    } else {
        GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerEnableCascade(nativeNode);
    }
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetTimepickerEnableCascade(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(0); // 0: index of parameter frameNode
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerEnableCascade(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::SetDigitalCrownSensitivity(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> crownSensitivityArg =
        runtimeCallInfo->GetCallArgRef(NUM_1);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    int32_t crownSensitivity = OHOS::Ace::NG::DEFAULT_CROWNSENSITIVITY;
    if (crownSensitivityArg->IsNumber()) {
        crownSensitivity = crownSensitivityArg->ToNumber(vm)->Value();
    }
    auto modifier = GetArkUINodeModifiers();
    CHECK_NULL_RETURN(modifier, panda::NativePointerRef::New(vm, nullptr));
    modifier->getTimepickerModifier()->setTimePickerDigitalCrownSensitivity(nativeNode, crownSensitivity);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue TimepickerBridge::ResetDigitalCrownSensitivity(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    auto modifier = GetArkUINodeModifiers();
    CHECK_NULL_RETURN(modifier, panda::NativePointerRef::New(vm, nullptr));
    modifier->getTimepickerModifier()->resetTimePickerDigitalCrownSensitivity(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
panda::Local<panda::ObjectRef> CreateTimePickerOnChange(EcmaVM* vm, const BaseEventInfo* info)
{
    const auto* eventInfo = TypeInfoHelper::DynamicCast<DatePickerChangeEvent>(info);
    auto jsonValue = JsonUtil::Create(true);
    CHECK_NULL_RETURN(eventInfo, panda::JSValueRef::Undefined(vm));
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(eventInfo->GetSelectedStr());
    if (!argsPtr) {
        return panda::JSValueRef::Undefined(vm);
    }
    std::vector<std::string> keys = { "year", "month", "day", "hour", "minute", "second" };
    for (auto iter = keys.begin(); iter != keys.end(); iter++) {
        const std::string key = *iter;
        const auto value = argsPtr->GetValue(key);
        if (!value || value->ToString().empty()) {
            continue;
        }
        jsonValue->Put(key.c_str(), value->ToString().c_str());
    }
    Local<JSValueRef> jsValue =
        panda::JSON::Parse(vm, panda::StringRef::NewFromUtf8(vm, jsonValue->ToString().c_str()));
    if (jsValue->IsUndefined()) {
        return panda::JSValueRef::Undefined(vm);
    }
    return jsValue->ToObject(vm);
}
ArkUINativeModuleValue TimepickerBridge::SetTimepickerOnChange(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    uint32_t argsNumber = runtimeCallInfo->GetArgsNumber();
    if (argsNumber != NUM_2) {
        return panda::JSValueRef::Undefined(vm);
    }
    Local<JSValueRef> nativeNodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    Local<JSValueRef> callbackArg = runtimeCallInfo->GetCallArgRef(NUM_1);
    auto nativeNode = nodePtr(nativeNodeArg->ToNativePointer(vm)->Value());
    auto frameNode = reinterpret_cast<FrameNode*>(nativeNode);
    CHECK_NULL_RETURN(frameNode, panda::NativePointerRef::New(vm, nullptr));
    if (callbackArg->IsUndefined() || callbackArg->IsNull() || !callbackArg->IsFunction(vm)) {
        GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerOnChange(nativeNode);
        return panda::JSValueRef::Undefined(vm);
    }
    panda::Local<panda::FunctionRef> func = callbackArg->ToObject(vm);
    std::function<void(const BaseEventInfo*)> callback = [vm, frameNode, func = panda::CopyableGlobal(vm, func)](
                                                             const BaseEventInfo* info) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        PipelineContext::SetCallBackNode(AceType::WeakClaim(frameNode));
        auto dateObj = CreateTimePickerOnChange(vm, info);
        panda::Local<panda::JSValueRef> params[] = { dateObj };
        func->Call(vm, func.ToLocal(), params, PARAM_ARR_LENGTH_1);
    };
    GetArkUINodeModifiers()->getTimepickerModifier()->setTimepickerOnChange(
        nativeNode, reinterpret_cast<void*>(&callback));
    return panda::JSValueRef::Undefined(vm);
}
ArkUINativeModuleValue TimepickerBridge::ResetTimepickerOnChange(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> nodeArg = runtimeCallInfo->GetCallArgRef(NUM_0);
    auto nativeNode = nodePtr(nodeArg->ToNativePointer(vm)->Value());
    GetArkUINodeModifiers()->getTimepickerModifier()->resetTimepickerOnChange(nativeNode);
    return panda::JSValueRef::Undefined(vm);
}
} // namespace OHOS::Ace::NG

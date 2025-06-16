/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#include "frameworks/bridge/declarative_frontend/jsview/dialog/js_alert_dialog.h"

#include <sstream>
#include <string>
#include <vector>

#include "base/log/ace_scoring_log.h"
#include "bridge/declarative_frontend/engine/js_converter.h"
#include "bridge/declarative_frontend/jsview/models/alert_dialog_model_impl.h"
#include "core/common/container.h"
#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/dialog/alert_dialog_model_ng.h"
#include "core/components_ng/pattern/overlay/level_order.h"
#include "frameworks/bridge/common/utils/engine_helper.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"

namespace OHOS::Ace {
std::unique_ptr<AlertDialogModel> AlertDialogModel::instance_ = nullptr;
std::mutex AlertDialogModel::mutex_;
AlertDialogModel* AlertDialogModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::AlertDialogModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::AlertDialogModelNG());
            } else {
                instance_.reset(new Framework::AlertDialogModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace
namespace OHOS::Ace::Framework {
namespace {
const std::vector<DialogAlignment> DIALOG_ALIGNMENT = { DialogAlignment::TOP, DialogAlignment::CENTER,
    DialogAlignment::BOTTOM, DialogAlignment::DEFAULT, DialogAlignment::TOP_START, DialogAlignment::TOP_END,
    DialogAlignment::CENTER_START, DialogAlignment::CENTER_END, DialogAlignment::BOTTOM_START,
    DialogAlignment::BOTTOM_END };
const std::vector<DialogButtonDirection> DIALOG_BUTTONS_DIRECTION = { DialogButtonDirection::AUTO,
    DialogButtonDirection::HORIZONTAL, DialogButtonDirection::VERTICAL };
constexpr int32_t ALERT_DIALOG_VALID_PRIMARY_BUTTON_NUM = 1;
const std::vector<LevelMode> DIALOG_LEVEL_MODE = { LevelMode::OVERLAY, LevelMode::EMBEDDED };
const std::vector<ImmersiveMode> DIALOG_IMMERSIVE_MODE = { ImmersiveMode::DEFAULT, ImmersiveMode::EXTEND};
} // namespace

void SetParseStyle(ButtonInfo& buttonInfo, const int32_t styleValue)
{
    if (styleValue >= static_cast<int32_t>(DialogButtonStyle::DEFAULT) &&
        styleValue <= static_cast<int32_t>(DialogButtonStyle::HIGHTLIGHT)) {
        buttonInfo.dlgButtonStyle = static_cast<DialogButtonStyle>(styleValue);
    }
}

void ParseTextPropsUpdate(const JSRef<JSVal>& jsVal, ButtonInfo& button)
{
    RefPtr<ResourceObject> resObj;
    std::string buttonValue;
    if (SystemProperties::ConfigChangePerform()) {
        RefPtr<ResourceObject> resObj = nullptr;
        if (JSViewAbstract::ParseJsString(jsVal, buttonValue, resObj)) {
            button.text = buttonValue;
        }
        button.resourceTextObj = resObj;
        return;
    }
    if (JSViewAbstract::ParseJsString(jsVal, buttonValue)) {
        button.text = buttonValue;
    }
}

void ParseFontColorPropsUpdate(const JSRef<JSVal>& jsVal, ButtonInfo& button)
{
    Color textColor;
    if (SystemProperties::ConfigChangePerform()) {
        RefPtr<ResourceObject> resObj = nullptr;
        if (JSViewAbstract::ParseJsColor(jsVal, textColor, resObj)) {
            button.textColor = textColor.ColorToString();
        }
        button.resourceFontColorObj = resObj;
        return;
    }
    if (JSViewAbstract::ParseJsColor(jsVal, textColor)) {
        button.textColor = textColor.ColorToString();
    }
}

void ParseBackgroundColorPropsUpdate(const JSRef<JSVal>& jsVal, ButtonInfo& button)
{
    Color color;
    if (SystemProperties::ConfigChangePerform()) {
        RefPtr<ResourceObject> resObj = nullptr;
        if (JSViewAbstract::ParseJsColor(jsVal, color, resObj)) {
            button.backgroundColor = color;
        }
        button.resourceBgColorObj = resObj;
        return;
    }
    if (JSViewAbstract::ParseJsColor(jsVal, color)) {
        button.backgroundColor = color;
    }
}

void ParseButtonInfo(JSRef<JSObject>& objInner, ButtonInfo& buttonInfo)
{
    auto textValue = objInner->GetProperty("value");
    ParseTextPropsUpdate(textValue, buttonInfo);

    // Parse enabled
    auto enabledValue = objInner->GetProperty("enabled");
    if (enabledValue->IsBoolean()) {
        buttonInfo.enabled = enabledValue->ToBoolean();
    }

    // Parse defaultFocus
    auto defaultFocusValue = objInner->GetProperty("defaultFocus");
    if (defaultFocusValue->IsBoolean()) {
        buttonInfo.defaultFocus = defaultFocusValue->ToBoolean();
    }

    // Parse style
    auto style = objInner->GetProperty("style");
    if (style->IsNumber()) {
        SetParseStyle(buttonInfo, style->ToNumber<int32_t>());
    }

    ParseFontColorPropsUpdate(objInner->GetProperty("fontColor"), buttonInfo);
    ParseBackgroundColorPropsUpdate(objInner->GetProperty("backgroundColor"), buttonInfo);
}

void ParseButtonObj(const panda::ecmascript::EcmaVM* vm, DialogProperties& properties, JSRef<JSVal> jsVal,
    const std::string& property, bool isPrimaryButtonValid)
{
    if (!jsVal->IsObject()) {
        return;
    }
    auto objInner = JSRef<JSObject>::Cast(jsVal);
    ButtonInfo buttonInfo;
    ParseButtonInfo(objInner, buttonInfo);
    auto actionValue = objInner->GetProperty("action");
    if (actionValue->IsFunction()) {
        auto frameNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
        auto jsFunc = JSRef<JSFunc>::Cast(actionValue);
        auto func = jsFunc->GetLocalHandle();
        auto eventFunc = [vm, func = panda::CopyableGlobal(vm, func), property, node = frameNode]() {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ACE_SCORING_EVENT("AlertDialog.[" + property + "].onAction");
            auto pipelineContext = PipelineContext::GetCurrentContextSafely();
            CHECK_NULL_VOID(pipelineContext);
            pipelineContext->UpdateCurrentActiveNode(node);
            func->Call(vm, func.ToLocal(), nullptr, 0);
        };
        AlertDialogModel::GetInstance()->SetParseButtonObj(eventFunc, buttonInfo, properties, property);
    }

    if (!buttonInfo.defaultFocus && isPrimaryButtonValid) {
        if (strcmp(property.c_str(), "confirm") == 0 || strcmp(property.c_str(), "primaryButton") == 0) {
            buttonInfo.isPrimary = true;
        } else {
            auto primaryButton = objInner->GetProperty("primary");
            if (primaryButton->IsBoolean()) {
                buttonInfo.isPrimary = primaryButton->ToBoolean();
            }
        }
    }

    if (buttonInfo.IsValid()) {
        properties.buttons.emplace_back(buttonInfo);
    }
}

void ParseButtonArray(const panda::ecmascript::EcmaVM* vm, DialogProperties& properties, JSRef<JSObject> obj,
    const std::string& property)
{
    auto jsVal = obj->GetProperty(property.c_str());
    if (!jsVal->IsArray()) {
        return;
    }
    JSRef<JSArray> array = JSRef<JSArray>::Cast(jsVal);
    size_t length = array->Length();
    if (length <= 0) {
        return;
    }
    int32_t primaryButtonNum = 0;
    bool isPrimaryButtonValid = true;
    for (size_t i = 0; i < length; i++) {
        JSRef<JSVal> buttonItem = array->GetValueAt(i);
        if (!buttonItem->IsObject()) {
            break;
        }
        auto objInner = JSRef<JSObject>::Cast(buttonItem);
        auto primaryButton = objInner->GetProperty("primary");
        if (primaryButton->IsBoolean()) {
            primaryButtonNum += (primaryButton->ToBoolean() ? ALERT_DIALOG_VALID_PRIMARY_BUTTON_NUM : 0);
        }
        if (primaryButtonNum > ALERT_DIALOG_VALID_PRIMARY_BUTTON_NUM) {
            isPrimaryButtonValid = false;
            break;
        }
    }
    for (size_t i = 0; i < length; i++) {
        JSRef<JSVal> buttonItem = array->GetValueAt(i);
        if (!buttonItem->IsObject()) {
            break;
        }
        ParseButtonObj(vm, properties, buttonItem, property + std::to_string(i), isPrimaryButtonValid);
    }
}

void ParseButtons(const panda::ecmascript::EcmaVM* vm, DialogProperties& properties, JSRef<JSObject> obj)
{
    properties.buttons.clear();
    if (obj->GetProperty("confirm")->IsObject()) {
        // Parse confirm.
        auto objInner = obj->GetProperty("confirm");
        ParseButtonObj(vm, properties, objInner, "confirm", true);
    } else if (obj->GetProperty("buttons")->IsArray()) {
        // Parse buttons array.
        ParseButtonArray(vm, properties, obj, "buttons");
    } else {
        // Parse primaryButton and secondaryButton.
        auto objInner = obj->GetProperty("primaryButton");
        ParseButtonObj(vm, properties, objInner, "primaryButton", true);
        objInner = obj->GetProperty("secondaryButton");
        ParseButtonObj(vm, properties, objInner, "secondaryButton", true);
    }

    // Parse buttons direction.
    auto directionValue = obj->GetProperty("buttonDirection");
    if (directionValue->IsNumber()) {
        auto buttonDirection = directionValue->ToNumber<int32_t>();
        if (buttonDirection >= 0 && buttonDirection < static_cast<int32_t>(DIALOG_BUTTONS_DIRECTION.size())) {
            properties.buttonDirection = DIALOG_BUTTONS_DIRECTION[buttonDirection];
        }
    }
}

void ParseDialogTitleAndMessage(DialogProperties& properties, JSRef<JSObject> obj)
{
    auto titleValue = obj->GetProperty("title");
    auto subtitleValue = obj->GetProperty("subtitle");
    auto messageValue = obj->GetProperty("message");
    std::string title;
    std::string subtitle;
    std::string message;
    if (SystemProperties::ConfigChangePerform()) {
        RefPtr<ResourceObject> resTitleObj = nullptr;
        RefPtr<ResourceObject> resSubTitleObj = nullptr;
        RefPtr<ResourceObject> resContentObj = nullptr;
        if (JSViewAbstract::ParseJsString(titleValue, title, resTitleObj)) {
            properties.title = title;
        }
        properties.resourceTitleObj = resTitleObj;
        if (JSViewAbstract::ParseJsString(subtitleValue, subtitle, resSubTitleObj)) {
            properties.subtitle = subtitle;
        }
        properties.resourceSubTitleObj = resSubTitleObj;
        if (JSViewAbstract::ParseJsString(messageValue, message, resContentObj)) {
            properties.content = message;
        }
        properties.resourceContentObj = resContentObj;
        return;
    }
    if (JSAlertDialog::ParseJsString(titleValue, title)) {
        properties.title = title;
    }
    if (JSAlertDialog::ParseJsString(subtitleValue, subtitle)) {
        properties.subtitle = subtitle;
    }
    if (JSAlertDialog::ParseJsString(messageValue, message)) {
        properties.content = message;
    }
}

void ParseAlertShadow(DialogProperties& properties, JSRef<JSObject> obj)
{
    // Parse shadow.
    auto shadowValue = obj->GetProperty("shadow");
    Shadow shadow;
    if ((shadowValue->IsObject() || shadowValue->IsNumber()) && JSAlertDialog::ParseShadowProps(shadowValue, shadow)) {
        properties.shadow = shadow;
    }
}

void ParseAlertBorderWidthAndColor(DialogProperties& properties, JSRef<JSObject> obj)
{
    auto borderWidthValue = obj->GetProperty("borderWidth");
    NG::BorderWidthProperty borderWidth;
    if (JSAlertDialog::ParseBorderWidthProps(borderWidthValue, borderWidth)) {
        properties.borderWidth = borderWidth;
        auto colorValue = obj->GetProperty("borderColor");
        NG::BorderColorProperty borderColor;
        if (JSAlertDialog::ParseBorderColorProps(colorValue, borderColor)) {
            properties.borderColor = borderColor;
        } else {
            borderColor.SetColor(Color::BLACK);
            properties.borderColor = borderColor;
        }
        // Parse border style
        auto styleValue = obj->GetProperty("borderStyle");
        NG::BorderStyleProperty borderStyle;
        if (JSAlertDialog::ParseBorderStyleProps(styleValue, borderStyle)) {
            properties.borderStyle = borderStyle;
        } else {
            properties.borderStyle = NG::BorderStyleProperty(
                { BorderStyle::SOLID, BorderStyle::SOLID, BorderStyle::SOLID, BorderStyle::SOLID });
        }
    }
}

void ParseAlertRadius(DialogProperties& properties, JSRef<JSObject> obj)
{
    auto cornerRadiusValue = obj->GetProperty("cornerRadius");
    NG::BorderRadiusProperty radius;
    if (JSAlertDialog::ParseBorderRadius(cornerRadiusValue, radius)) {
        properties.borderRadius = radius;
    }
}

void UpdateAlertAlignment(DialogAlignment& alignment)
{
    bool isRtl = AceApplicationInfo::GetInstance().IsRightToLeft();
    if (alignment == DialogAlignment::TOP_START) {
        if (isRtl) {
            alignment = DialogAlignment::TOP_END;
        }
    } else if (alignment == DialogAlignment::TOP_END) {
        if (isRtl) {
            alignment = DialogAlignment::TOP_START;
        }
    } else if (alignment == DialogAlignment::CENTER_START) {
        if (isRtl) {
            alignment = DialogAlignment::CENTER_END;
        }
    } else if (alignment == DialogAlignment::CENTER_END) {
        if (isRtl) {
            alignment = DialogAlignment::CENTER_START;
        }
    } else if (alignment == DialogAlignment::BOTTOM_START) {
        if (isRtl) {
            alignment = DialogAlignment::BOTTOM_END;
        }
    } else if (alignment == DialogAlignment::BOTTOM_END) {
        if (isRtl) {
            alignment = DialogAlignment::BOTTOM_START;
        }
    }
}

void ParseAlertAlignment(DialogProperties& properties, JSRef<JSObject> obj)
{
    // Parse alignment
    auto alignmentValue = obj->GetProperty("alignment");
    if (alignmentValue->IsNumber()) {
        auto alignment = alignmentValue->ToNumber<int32_t>();
        if (alignment >= 0 && alignment < static_cast<int32_t>(DIALOG_ALIGNMENT.size())) {
            properties.alignment = DIALOG_ALIGNMENT[alignment];
            UpdateAlertAlignment(properties.alignment);
        }
    }
}

void ParseAlertOffset(DialogProperties& properties, JSRef<JSObject> obj)
{
    auto offsetValue = obj->GetProperty("offset");
    if (!offsetValue->IsObject()) {
        return;
    }
    auto offsetObj = JSRef<JSObject>::Cast(offsetValue);
    CalcDimension dx;
    CalcDimension dy;
    if (!SystemProperties::ConfigChangePerform()) {
        JSAlertDialog::ParseJsDimensionVp(offsetObj->GetProperty("dx"), dx);
        JSAlertDialog::ParseJsDimensionVp(offsetObj->GetProperty("dy"), dy);
        properties.offset = DimensionOffset(dx, dy);
        bool isRtl = AceApplicationInfo::GetInstance().IsRightToLeft();
        Dimension offsetX = isRtl ? properties.offset.GetX() * (-1) : properties.offset.GetX();
        properties.offset.SetX(offsetX);
        return;
    }
    RefPtr<ResourceObject> xResObj = nullptr;
    RefPtr<ResourceObject> yResObj = nullptr;
    if (JSAlertDialog::ParseJsDimensionVp(offsetObj->GetProperty("dx"), dx, xResObj)) {
        properties.offset.SetX(dx);
    }
    if (JSAlertDialog::ParseJsDimensionVp(offsetObj->GetProperty("dy"), dy, yResObj)) {
        properties.offset.SetY(dy);
    }
    DimensionOffset offsetOption;
    if (xResObj) {
        auto&& offsetXUpdateFunc = [](const RefPtr<ResourceObject>& xResObj, DimensionOffset& options) {
            CalcDimension offsetX;
            if (ResourceParseUtils::ParseResDimensionVp(xResObj, offsetX)) {
                Dimension xVal = offsetX;
                options.SetX(xVal);
            }
            bool isRtl = AceApplicationInfo::GetInstance().IsRightToLeft();
            Dimension value = isRtl ? options.GetX() * (-1) : options.GetX();
            options.SetX(value);
        };
        offsetOption.AddResource("dialog.offset.x", xResObj, std::move(offsetXUpdateFunc));
    }
    if (yResObj) {
        auto&& offsetYUpdateFunc = [](const RefPtr<ResourceObject>& yResObj, DimensionOffset& options) {
            CalcDimension offsetY;
            if (ResourceParseUtils::ParseResDimensionVp(yResObj, offsetY)) {
                Dimension yVal = offsetY;
                options.SetY(yVal);
            }
        };
        offsetOption.AddResource("dialog.offset.y", yResObj, std::move(offsetYUpdateFunc));
    }
}

void ParseTextStyle(DialogProperties& properties, JSRef<JSObject> obj)
{
    auto textStyleObj = obj->GetProperty("textStyle");
    if (textStyleObj->IsNull() || !textStyleObj->IsObject()) {
        return;
    }
    auto textStyle = JSRef<JSObject>::Cast(textStyleObj);
    auto args = textStyle->GetProperty("wordBreak");
    int32_t index = 1;
    if (args->IsNumber()) {
        index = args->ToNumber<int32_t>();
    }
    if (index < 0 || index >= static_cast<int32_t>(WORD_BREAK_TYPES.size())) {
        index = 1;
    }
    properties.wordBreak = WORD_BREAK_TYPES[index];
}

void ParseAlertMaskRect(DialogProperties& properties, JSRef<JSObject> obj)
{
    // Parse maskRect.
    auto maskRectValue = obj->GetProperty("maskRect");
    DimensionRect maskRect;
    if (JSViewAbstract::ParseJsDimensionRect(maskRectValue, maskRect)) {
        properties.maskRect = maskRect;
        bool isRtl = AceApplicationInfo::GetInstance().IsRightToLeft();
        auto offset = maskRect.GetOffset();
        Dimension offsetX = isRtl ? offset.GetX() * (-1) : offset.GetX();
        offset.SetX(offsetX);
        properties.maskRect->SetOffset(offset);
    }
}

void ParseAlertDialogLevelMode(DialogProperties& properties, JSRef<JSObject> obj)
{
    auto levelMode = obj->GetProperty("levelMode");
    auto levelUniqueId = obj->GetProperty("levelUniqueId");
    auto immersiveMode = obj->GetProperty("immersiveMode");
    bool showInMainWindow = true;
    if (obj->GetProperty("showInSubWindow")->IsBoolean() && obj->GetProperty("showInSubWindow")->ToBoolean()) {
        showInMainWindow = false;
    }
    if (levelMode->IsNumber() && showInMainWindow) {
        auto mode = levelMode->ToNumber<int32_t>();
        if (mode >= 0 && mode < static_cast<int32_t>(DIALOG_LEVEL_MODE.size())) {
            properties.dialogLevelMode = DIALOG_LEVEL_MODE[mode];
        }
    }
    if (levelUniqueId->IsNumber()) {
        properties.dialogLevelUniqueId = levelUniqueId->ToNumber<int32_t>();
    }
    if (immersiveMode->IsNumber()) {
        auto immersiveVal = immersiveMode->ToNumber<int32_t>();
        if (immersiveVal >= 0 && immersiveVal < static_cast<int32_t>(DIALOG_IMMERSIVE_MODE.size())) {
            properties.dialogImmersiveMode = DIALOG_IMMERSIVE_MODE[immersiveVal];
        }
    }
}

void ParseAlertLevelOrder(DialogProperties& properties, JSRef<JSObject> obj)
{
    if (properties.isShowInSubWindow) {
        return;
    }

    auto levelOrderValue = obj->GetProperty("levelOrder");
    if (!levelOrderValue->IsObject()) {
        return;
    }
    napi_value levelOrderApi = JsConverter::ConvertJsValToNapiValue(levelOrderValue);
    CHECK_NULL_VOID(levelOrderApi);

    auto engine = EngineHelper::GetCurrentEngine();
    CHECK_NULL_VOID(engine);
    NativeEngine* nativeEngine = engine->GetNativeEngine();
    CHECK_NULL_VOID(nativeEngine);
    auto env = reinterpret_cast<napi_env>(nativeEngine);
    NG::LevelOrder* levelOrder = nullptr;
    napi_status status = napi_unwrap(env, levelOrderApi, reinterpret_cast<void**>(&levelOrder));
    if (status != napi_ok || !levelOrder) {
        LOGE("Failed to unwrap LevelOrder.");
        return;
    }

    double order = levelOrder->GetOrder();
    properties.levelOrder = std::make_optional(order);
}

void ParseBackgroundColor(const JSRef<JSVal>& jsValue, DialogProperties& properties)
{
    Color textColor;
    if (SystemProperties::ConfigChangePerform()) {
        RefPtr<ResourceObject> resObj;
        if (JSViewAbstract::ParseJsColor(jsValue, textColor, resObj)) {
            properties.backgroundColor = textColor;
        }
        properties.resourceBgColorObj = resObj;
        return;
    }
    if (JSViewAbstract::ParseJsColor(jsValue, textColor)) {
        properties.backgroundColor = textColor;
    }
}

void JSAlertDialog::Show(const JSCallbackInfo& args)
{
    auto scopedDelegate = EngineHelper::GetCurrentDelegateSafely();
    if (!scopedDelegate) {
        // this case usually means there is no foreground container, need to figure out the reason.
        LOGE("scopedDelegate is null, please check");
        return;
    }

    DialogProperties properties { .type = DialogType::ALERT_DIALOG, .isAlertDialog = true };
    if (args[0]->IsObject()) {
        auto obj = JSRef<JSObject>::Cast(args[0]);
        auto dialogNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
        auto vm = args.GetVm();

        ParseDialogTitleAndMessage(properties, obj);
        ParseButtons(vm, properties, obj);
        ParseAlertShadow(properties, obj);
        ParseAlertBorderWidthAndColor(properties, obj);
        ParseAlertRadius(properties, obj);
        ParseAlertAlignment(properties, obj);
        ParseAlertOffset(properties, obj);
        ParseTextStyle(properties, obj);
        ParseAlertMaskRect(properties, obj);
        ParseAlertDialogLevelMode(properties, obj);

        auto onLanguageChange = [vm, obj, parseContent = ParseDialogTitleAndMessage,
                                    parseButton = ParseButtons, parseShadow = ParseAlertShadow,
                                    parseBorderProps = ParseAlertBorderWidthAndColor,
                                    parseRadius = ParseAlertRadius, parseAlignment = ParseAlertAlignment,
                                    parseOffset = ParseAlertOffset, parseMaskRect = ParseAlertMaskRect,
                                    parseDialogLevelMode = ParseAlertDialogLevelMode,
                                    node = dialogNode](DialogProperties& dialogProps) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            ACE_SCORING_EVENT("AlertDialog.property.onLanguageChange");
            auto pipelineContext = PipelineContext::GetCurrentContextSafely();
            CHECK_NULL_VOID(pipelineContext);
            pipelineContext->UpdateCurrentActiveNode(node);
            parseContent(dialogProps, obj);
            parseButton(vm, dialogProps, obj);
            parseShadow(dialogProps, obj);
            parseBorderProps(dialogProps, obj);
            parseRadius(dialogProps, obj);
            parseAlignment(dialogProps, obj);
            parseOffset(dialogProps, obj);
            parseMaskRect(dialogProps, obj);
            parseDialogLevelMode(dialogProps, obj);
        };
        properties.onLanguageChange = std::move(onLanguageChange);

        // Parses gridCount.
        auto gridCountValue = obj->GetProperty("gridCount");
        if (gridCountValue->IsNumber()) {
            properties.gridCount = gridCountValue->ToNumber<int32_t>();
        }

        // Parse auto autoCancel.
        auto autoCancelValue = obj->GetProperty("autoCancel");
        if (autoCancelValue->IsBoolean()) {
            properties.autoCancel = autoCancelValue->ToBoolean();
        }

        // Parse cancel.
        auto cancelValue = obj->GetProperty("cancel");
        if (cancelValue->IsFunction()) {
            auto jsFunc = JSRef<JSFunc>::Cast(cancelValue);
            auto func = jsFunc->GetLocalHandle();
            auto eventFunc = [vm, func = panda::CopyableGlobal(vm, func), node = dialogNode]() {
                panda::LocalScope pandaScope(vm);
                panda::TryCatch trycatch(vm);
                ACE_SCORING_EVENT("AlertDialog.property.cancel");
                auto pipelineContext = PipelineContext::GetCurrentContextSafely();
                CHECK_NULL_VOID(pipelineContext);
                pipelineContext->UpdateCurrentActiveNode(node);
                func->Call(vm, func.ToLocal(), nullptr, 0);
            };
            AlertDialogModel::GetInstance()->SetOnCancel(eventFunc, properties);
        }

        std::function<void(const int32_t& info, const int32_t& instanceId)> onWillDismissFunc = nullptr;
        ParseDialogCallback(obj, onWillDismissFunc);
        AlertDialogModel::GetInstance()->SetOnWillDismiss(std::move(onWillDismissFunc), properties);

        JSViewAbstract::ParseAppearDialogCallback(args, properties);

        // Parse showInSubWindowValue.
        auto showInSubWindowValue = obj->GetProperty("showInSubWindow");
        if (showInSubWindowValue->IsBoolean()) {
#if defined(PREVIEW)
            LOGW("[Engine Log] Unable to use the SubWindow in the Previewer. Perform this operation on the "
                 "emulator or a real device instead.");
#else
            properties.isShowInSubWindow = showInSubWindowValue->ToBoolean();
#endif
        }

        // Parse isModal.
        auto isModalValue = obj->GetProperty("isModal");
        if (isModalValue->IsBoolean()) {
            LOGI("Parse isModalValue");
            properties.isModal = isModalValue->ToBoolean();
        }

        ParseBackgroundColor(obj->GetProperty("backgroundColor"), properties);
        auto backgroundBlurStyle = obj->GetProperty("backgroundBlurStyle");
        if (backgroundBlurStyle->IsNumber()) {
            auto blurStyle = backgroundBlurStyle->ToNumber<int32_t>();
            if (blurStyle >= static_cast<int>(BlurStyle::NO_MATERIAL) &&
                blurStyle <= static_cast<int>(BlurStyle::COMPONENT_ULTRA_THICK)) {
                properties.backgroundBlurStyle = blurStyle;
            }
        }
        // Parse transition.
        properties.transitionEffect = ParseJsTransitionEffect(args);
        ParseAlertLevelOrder(properties, obj);
        JSViewAbstract::ParseDialogWidthAndHeight(properties, obj);
        JSViewAbstract::SetDialogHoverModeProperties(obj, properties);
        JSViewAbstract::SetDialogBlurStyleOption(obj, properties);
        JSViewAbstract::SetDialogEffectOption(obj, properties);
        AlertDialogModel::GetInstance()->SetShowDialog(properties);
    }
}

void JSAlertDialog::JSBind(BindingTarget globalObj)
{
    JSClass<JSAlertDialog>::Declare("AlertDialog");
    JSClass<JSAlertDialog>::StaticMethod("show", &JSAlertDialog::Show);

    JSClass<JSAlertDialog>::InheritAndBind<JSViewAbstract>(globalObj);
}
} // namespace OHOS::Ace::Framework

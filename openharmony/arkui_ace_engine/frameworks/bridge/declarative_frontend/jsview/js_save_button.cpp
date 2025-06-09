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

#include "frameworks/bridge/declarative_frontend/jsview/js_save_button.h"
#if !defined(PREVIEW) && defined(OHOS_PLATFORM)
#include "interfaces/inner_api/ui_session/ui_session_manager.h"
#endif

#include "bridge/common/utils/utils.h"
#include "bridge/declarative_frontend/jsview/js_utils.h"
#include "core/common/container.h"
#include "core/components/common/properties/text_style.h"
#include "core/components_ng/base/view_abstract_model.h"
#include "core/components_ng/pattern/security_component/save_button/save_button_model_ng.h"
#include "core/components_ng/pattern/security_component/security_component_model_ng.h"
#include "core/components_ng/pattern/security_component/security_component_theme.h"

using OHOS::Ace::NG::SaveButtonModelNG;
using OHOS::Ace::NG::SecurityComponentModelNG;
using OHOS::Ace::NG::SecurityComponentTheme;

namespace OHOS::Ace::Framework {

bool JSSaveButton::ParseComponentStyle(const JSCallbackInfo& info,
    SaveButtonSaveDescription& text, SaveButtonIconStyle& icon, int32_t& bg)
{
    if ((info.Length() < 1) || (!info[0]->IsObject())) {
        return false;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto value = paramObject->GetProperty("text");
    if (value->IsNumber()) {
        text = static_cast<SaveButtonSaveDescription>(value->ToNumber<int32_t>());
        if ((text < SaveButtonSaveDescription::DOWNLOAD) ||
            (text > SaveButtonSaveDescription::MAX_LABEL_TYPE)) {
            return false;
        }
    } else {
        text = SaveButtonSaveDescription::TEXT_NULL;
    }

    value = paramObject->GetProperty("icon");
    if (value->IsNumber()) {
        icon = static_cast<SaveButtonIconStyle>(value->ToNumber<int32_t>());
        if ((icon < SaveButtonIconStyle::ICON_FULL_FILLED) ||
            (icon > SaveButtonIconStyle::ICON_PICTURE)) {
            return false;
        }
    } else {
        icon = SaveButtonIconStyle::ICON_NULL;
    }

    if ((text == SaveButtonSaveDescription::TEXT_NULL) &&
        (icon == SaveButtonIconStyle::ICON_NULL)) {
        return false;
    }

    value = paramObject->GetProperty("buttonType");
    if (value->IsNumber()) {
        bg = value->ToNumber<int32_t>();
        if ((bg != static_cast<int32_t>(ButtonType::NORMAL)) &&
            (bg != static_cast<int32_t>(ButtonType::CIRCLE)) &&
            (bg != static_cast<int32_t>(ButtonType::CAPSULE)) &&
            (bg != static_cast<int32_t>(ButtonType::ROUNDED_RECTANGLE))) {
            return false;
        }
    } else {
        bg = static_cast<int32_t>(ButtonType::CAPSULE);
    }
    return true;
}

void JSSaveButton::Create(const JSCallbackInfo& info)
{
    SaveButtonSaveDescription textDesc;
    SaveButtonIconStyle iconType;
    int32_t backgroundType = 0;
    if (!ParseComponentStyle(info, textDesc, iconType, backgroundType)) {
        SaveButtonModelNG::GetInstance()->Create(
            static_cast<int32_t>(SaveButtonSaveDescription::DOWNLOAD),
            static_cast<int32_t>(SaveButtonIconStyle::ICON_FULL_FILLED),
            static_cast<int32_t>(ButtonType::CAPSULE), false);
    } else {
        SaveButtonModelNG::GetInstance()->Create(static_cast<int32_t>(textDesc),
            static_cast<int32_t>(iconType), backgroundType, false);
    }
}

void JsSaveButtonClickFunction::Execute(GestureEvent& info)
{
    JSRef<JSObject> clickEventParam = JSRef<JSObject>::New();
    Offset globalOffset = info.GetGlobalLocation();
    Offset localOffset = info.GetLocalLocation();
    clickEventParam->SetProperty<double>("screenX", PipelineBase::Px2VpWithCurrentDensity(globalOffset.GetX()));
    clickEventParam->SetProperty<double>("screenY", PipelineBase::Px2VpWithCurrentDensity(globalOffset.GetY()));
    clickEventParam->SetProperty<double>("x", PipelineBase::Px2VpWithCurrentDensity(localOffset.GetX()));
    clickEventParam->SetProperty<double>("y", PipelineBase::Px2VpWithCurrentDensity(localOffset.GetY()));
    clickEventParam->SetProperty<double>("timestamp",
        static_cast<double>(info.GetTimeStamp().time_since_epoch().count()));
    clickEventParam->SetProperty<double>("source", static_cast<int32_t>(info.GetSourceDevice()));
    clickEventParam->SetProperty<double>("pressure", info.GetForce());
    clickEventParam->SetProperty<double>("tiltX", info.GetTiltX().value_or(0.0f));
    clickEventParam->SetProperty<double>("tiltY", info.GetTiltY().value_or(0.0f));
    clickEventParam->SetProperty<double>("sourceTool", static_cast<int32_t>(info.GetSourceTool()));
    auto target = CreateEventTargetObject(info);
    clickEventParam->SetPropertyObject("target", target);
    clickEventParam->SetProperty<int32_t>("targetDisplayId", info.GetTargetDisplayId());

    int32_t res = static_cast<int32_t>(SecurityComponentHandleResult::CLICK_GRANT_FAILED);
    JSRef<JSObject> errorMessage = JSRef<JSObject>::New();
#ifdef SECURITY_COMPONENT_ENABLE
    auto secEventValue = info.GetSecCompHandleEvent();
    if (secEventValue != nullptr) {
        res = secEventValue->GetInt("handleRes", res);
        int32_t code = static_cast<int32_t>(SecurityComponentErrorCode::SUCCESS);
        std::string message;
        if (res == static_cast<int32_t>(SecurityComponentHandleResult::DROP_CLICK)) {
            return;
        }
        code = secEventValue->GetInt("code", code);
        errorMessage->SetProperty<int32_t>("code", code);
        message = secEventValue->GetString("message", message);
        errorMessage->SetProperty<std::string>("message", message);
    }
#endif
    JSRef<JSVal> errorParam = JSRef<JSVal>::Make(ToJSValue(res));
    JSRef<JSVal> params[] = { clickEventParam, errorParam, errorMessage };
    JsFunction::ExecuteJS(3, params); // 3 means three params.
}

void JSSaveButton::JsOnClick(const JSCallbackInfo& info)
{
    if ((info.Length() < 1) || (!info[0]->IsFunction())) {
        return;
    }
    auto jsOnClickFunc = AceType::MakeRefPtr<JsSaveButtonClickFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto frameNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    auto onTap = [execCtx = info.GetExecutionContext(), func = jsOnClickFunc, node = frameNode](GestureEvent& info) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onClick");
        func->Execute(info);
#if !defined(PREVIEW) && defined(OHOS_PLATFORM)
        JSInteractableView::ReportClickEvent(node);
#endif
    };

    double distanceThreshold = std::numeric_limits<double>::infinity();
    if (info.Length() > 1 && info[1]->IsNumber()) {
        distanceThreshold = info[1]->ToNumber<double>();
        distanceThreshold = Dimension(distanceThreshold, DimensionUnit::VP).ConvertToPx();
    }
    NG::ViewAbstract::SetOnClick(std::move(onTap), distanceThreshold);
}

void JSSaveButton::SetIcon(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    std::string bundleName;
    std::string moduleName;
    std::string src;
    auto imageInfo = info[0];
    int32_t resId = 0;
    bool srcValid = ParseJsMediaWithBundleName(imageInfo, src, bundleName, moduleName, resId);
    if (srcValid && (ImageSourceInfo::ResolveURIType(src) == SrcType::RESOURCE)) {
        ImageSourceInfo image(std::make_shared<std::string>(src), bundleName, moduleName);
        SecurityComponentModelNG::SetIcon(image);
    }
}

void JSSaveButton::SetText(const JSCallbackInfo& info)
{
    std::string label;
    if (info.Length() < 1) {
        return;
    }
    bool parseSuccess = ParseJsString(info[0], label);
    if (parseSuccess) {
        if ((info[0]->IsObject() && JSRef<JSObject>::Cast(info[0])->HasProperty("id")) || (info[0]->IsString())) {
            SecurityComponentModelNG::SetText(label);
        }
    }
}

void JSSaveButton::SetIconBorderRadius(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    if (info[0]->IsObject()) {
        std::optional<CalcDimension> topLeft;
        std::optional<CalcDimension> topRight;
        std::optional<CalcDimension> bottomLeft;
        std::optional<CalcDimension> bottomRight;
        JSRef<JSObject> paddingObj = JSRef<JSObject>::Cast(info[0]);

        CalcDimension topLeftDimen;
        if (ParseJsDimensionVp(paddingObj->GetProperty("topLeft"), topLeftDimen)) {
            topLeft = topLeftDimen;
        }
        CalcDimension topRightDimen;
        if (ParseJsDimensionVp(paddingObj->GetProperty("topRight"), topRightDimen)) {
            topRight = topRightDimen;
        }
        CalcDimension bottomLeftDimen;
        if (ParseJsDimensionVp(paddingObj->GetProperty("bottomLeft"), bottomLeftDimen)) {
            bottomLeft = bottomLeftDimen;
        }
        CalcDimension bottomRightDimen;
        if (ParseJsDimensionVp(paddingObj->GetProperty("bottomRight"), bottomRightDimen)) {
            bottomRight = bottomRightDimen;
        }
        
        if (topLeft.has_value() || topRight.has_value() || bottomLeft.has_value() || bottomRight.has_value()) {
            SecurityComponentModelNG::SetIconBorderRadius(topLeft, topRight, bottomLeft, bottomRight);
            return;
        }
    }
    auto theme = GetTheme<SecurityComponentTheme>();
    CHECK_NULL_VOID(theme);

    CalcDimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        SecurityComponentModelNG::SetIconBorderRadius(theme->GetBorderRadius());
    } else {
        SecurityComponentModelNG::SetIconBorderRadius(value);
    }
}

void JSSaveButton::SetStateEffect(const JSCallbackInfo& info)
{
    if ((info.Length() < 1) || (!info[0]->IsBoolean())) {
        return;
    }
    bool stateEffect = info[0]->ToBoolean();
    SecurityComponentModelNG::SetStateEffect(stateEffect);
}

void JSSaveButton::SetTipPosition(const JSCallbackInfo& info)
{
    if ((info.Length() < 1) || (!info[0]->IsNumber())) {
        SecurityComponentModelNG::SetTipPosition(TipPosition::ABOVE_BOTTOM);
        return;
    }
    int32_t value = info[0]->ToNumber<int32_t>();
    if ((value < static_cast<int32_t>(TipPosition::ABOVE_BOTTOM)) ||
        (value > static_cast<int32_t>(TipPosition::BELOW_TOP))) {
        SecurityComponentModelNG::SetTipPosition(TipPosition::ABOVE_BOTTOM);
        return;
    }
    SecurityComponentModelNG::SetTipPosition(static_cast<TipPosition>(value));
}

void JSSaveButton::JSBind(BindingTarget globalObj)
{
    JSClass<JSSaveButton>::Declare("SaveButton");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSaveButton>::StaticMethod("create", &JSSaveButton::Create, opt);
    JSClass<JSSaveButton>::StaticMethod("setIcon", &JSSaveButton::SetIcon);
    JSClass<JSSaveButton>::StaticMethod("setText", &JSSaveButton::SetText);
    JSClass<JSSaveButton>::StaticMethod("iconSize", &JSSecButtonBase::SetIconSize);
    JSClass<JSSaveButton>::StaticMethod("iconBorderRadius", &JSSaveButton::SetIconBorderRadius);
    JSClass<JSSaveButton>::StaticMethod("layoutDirection", &JSSecButtonBase::SetLayoutDirection);
    JSClass<JSSaveButton>::StaticMethod("fontSize", &JSSecButtonBase::SetFontSize);
    JSClass<JSSaveButton>::StaticMethod("fontStyle", &JSSecButtonBase::SetFontStyle);
    JSClass<JSSaveButton>::StaticMethod("iconColor", &JSSecButtonBase::SetIconColor);
    JSClass<JSSaveButton>::StaticMethod("fontWeight", &JSSecButtonBase::SetFontWeight);
    JSClass<JSSaveButton>::StaticMethod("fontFamily", &JSSecButtonBase::SetFontFamily);
    JSClass<JSSaveButton>::StaticMethod("fontColor", &JSSecButtonBase::SetFontColor);
    JSClass<JSSaveButton>::StaticMethod("backgroundColor", &JSSecButtonBase::SetBackgroundColor);
    JSClass<JSSaveButton>::StaticMethod("borderStyle", &JSSecButtonBase::SetBackgroundBorderStyle);
    JSClass<JSSaveButton>::StaticMethod("borderWidth", &JSSecButtonBase::SetBackgroundBorderWidth);
    JSClass<JSSaveButton>::StaticMethod("borderColor", &JSSecButtonBase::SetBackgroundBorderColor);
    JSClass<JSSaveButton>::StaticMethod("borderRadius", &JSSecButtonBase::SetBackgroundBorderRadius);
    JSClass<JSSaveButton>::StaticMethod("padding", &JSSecButtonBase::SetBackgroundPadding);
    JSClass<JSSaveButton>::StaticMethod("textIconSpace", &JSSecButtonBase::SetTextIconSpace);
    JSClass<JSSaveButton>::StaticMethod("align", &JSSecButtonBase::SetAlign);
    JSClass<JSSaveButton>::StaticMethod("onClick", &JSSaveButton::JsOnClick);
    JSClass<JSSaveButton>::StaticMethod("stateEffect", &JSSaveButton::SetStateEffect);
    JSClass<JSSaveButton>::StaticMethod("tipPosition", &JSSaveButton::SetTipPosition);
    JSClass<JSSaveButton>::StaticMethod("key", &JSViewAbstract::JsKey);
    JSClass<JSSaveButton>::StaticMethod("position", &JSViewAbstract::JsPosition);
    JSClass<JSSaveButton>::StaticMethod("markAnchor", &JSViewAbstract::JsMarkAnchor);
    JSClass<JSSaveButton>::StaticMethod("offset", &JSViewAbstract::JsOffset);
    JSClass<JSSaveButton>::StaticMethod("pop", &JSViewAbstract::Pop, opt);
    JSClass<JSSaveButton>::StaticMethod("width", &JSViewAbstract::JsWidth);
    JSClass<JSSaveButton>::StaticMethod("height", &JSViewAbstract::JsHeight);
    JSClass<JSSaveButton>::StaticMethod("size", &JSViewAbstract::JsSize);
    JSClass<JSSaveButton>::StaticMethod("constraintSize", &JSViewAbstract::JsConstraintSize);
    JSClass<JSSaveButton>::StaticMethod("debugLine", &JSViewAbstract::JsDebugLine);
    JSClass<JSSaveButton>::StaticMethod("alignRules", &JSViewAbstract::JsAlignRules);
    JSClass<JSSaveButton>::StaticMethod("id", &JSViewAbstract::JsId);
    JSClass<JSSaveButton>::StaticMethod("chainMode", &JSViewAbstract::JsChainMode);
    JSClass<JSSaveButton>::StaticMethod("maxFontScale", &JSSecButtonBase::SetMaxFontScale);
    JSClass<JSSaveButton>::StaticMethod("minFontScale", &JSSecButtonBase::SetMinFontScale);
    JSClass<JSSaveButton>::StaticMethod("maxLines", &JSSecButtonBase::SetMaxLines);
    JSClass<JSSaveButton>::StaticMethod("maxFontSize", &JSSecButtonBase::SetMaxFontSize);
    JSClass<JSSaveButton>::StaticMethod("minFontSize", &JSSecButtonBase::SetMinFontSize);
    JSClass<JSSaveButton>::StaticMethod("heightAdaptivePolicy", &JSSecButtonBase::SetHeightAdaptivePolicy);
    JSClass<JSSaveButton>::StaticMethod("enabled", &JSViewAbstract::JsEnabled);
    JSClass<JSSaveButton>::Bind<>(globalObj);
}
} // namespace OHOS::Ace::Framework

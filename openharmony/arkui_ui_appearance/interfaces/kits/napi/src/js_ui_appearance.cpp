/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "js_ui_appearance.h"

#include <string>
#include "js_native_api.h"
#include "ui_appearance_log.h"

namespace OHOS {
namespace ArkUi::UiAppearance {
namespace {
static constexpr size_t ARGC_WITH_ONE = 1;
static constexpr size_t ARGC_WITH_TWO = 2;
static constexpr size_t MAX_FONT_SCALE = 5;
static constexpr size_t MIN_FONT_SCALE = 0;
const std::string PERMISSION_ERR_MSG =
    "An attempt was made to update configuration forbidden by permission: ohos.permission.UPDATE_CONFIGURATION.";
const std::string INVALID_ARG_MSG = "The type of 'mode' must be DarkMode.";

static const std::unordered_map<int32_t, std::string> ERROR_CODE_TO_MSG {
    { UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR, "Permission denied. " },
    { UiAppearanceAbilityInterface::ErrCode::INVALID_ARG, "Parameter error. " },
    { UiAppearanceAbilityInterface::ErrCode::SYS_ERR, "Internal error. " },
};

void NapiThrow(napi_env env, const std::string& message, int32_t errCode)
{
    napi_value code = nullptr;
    std::string strCode = std::to_string(errCode);
    napi_create_string_utf8(env, strCode.c_str(), strCode.length(), &code);

    napi_value msg = nullptr;
    auto iter = ERROR_CODE_TO_MSG.find(errCode);
    std::string strMsg = (iter != ERROR_CODE_TO_MSG.end() ? iter->second : "") + message;
    LOGI("napi throw errCode %{public}d, strMsg %{public}s", errCode, strMsg.c_str());
    napi_create_string_utf8(env, strMsg.c_str(), strMsg.length(), &msg);

    napi_value error = nullptr;
    napi_create_error(env, code, msg, &error);
    napi_throw(env, error);
}
} // namespace

void JsUiAppearance::OnExecute(napi_env env, void* data)
{
    LOGI("OnExecute begin.");
    AsyncContext* asyncContext = static_cast<AsyncContext*>(data);
    if (asyncContext == nullptr) {
        NapiThrow(env, "asyncContext is null.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return;
    }
    auto resCode = UiAppearanceAbilityClient::GetInstance()->SetDarkMode(asyncContext->mode);
    asyncContext->status = static_cast<UiAppearanceAbilityInterface::ErrCode>(resCode);
    if (asyncContext->status == UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR) {
        asyncContext->errMsg = PERMISSION_ERR_MSG;
    } else if (asyncContext->status == UiAppearanceAbilityInterface::ErrCode::INVALID_ARG) {
        asyncContext->errMsg = INVALID_ARG_MSG;
    } else {
        asyncContext->errMsg = "";
    }
}

void JsUiAppearance::OnSetFontScale(napi_env env, void* data)
{
    LOGI("OnSetFontScale begin.");
    AsyncContext* asyncContext = static_cast<AsyncContext*>(data);
    if (asyncContext == nullptr) {
        NapiThrow(env, "asyncContext is null.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return;
    }
    int32_t resCode = 0;
    if (asyncContext->jsFontScale <= MIN_FONT_SCALE || asyncContext->jsFontScale > MAX_FONT_SCALE) {
        resCode = UiAppearanceAbilityInterface::ErrCode::INVALID_ARG;
    } else {
        resCode = UiAppearanceAbilityClient::GetInstance()->SetFontScale(asyncContext->fontScale);
    }

    asyncContext->status = static_cast<UiAppearanceAbilityInterface::ErrCode>(resCode);
    if (asyncContext->status == UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR) {
        asyncContext->errMsg = PERMISSION_ERR_MSG;
    } else if (asyncContext->status == UiAppearanceAbilityInterface::ErrCode::INVALID_ARG) {
        asyncContext->errMsg = "fontScale must between 0 and 5";
    } else {
        asyncContext->errMsg = "";
    }
}

void JsUiAppearance::OnSetFontWeightScale(napi_env env, void* data)
{
    LOGI("OnSetFontWeightScale begin.");
    AsyncContext* asyncContext = static_cast<AsyncContext*>(data);
    if (asyncContext == nullptr) {
        NapiThrow(env, "asyncContext is null.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return;
    }
    int32_t resCode = 0;
    if (asyncContext->jsFontWeightScale <= MIN_FONT_SCALE ||
        asyncContext->jsFontWeightScale > MAX_FONT_SCALE) {
        resCode = UiAppearanceAbilityInterface::ErrCode::INVALID_ARG;
    } else {
        resCode = UiAppearanceAbilityClient::GetInstance()
            ->SetFontWeightScale(asyncContext->fontWeightScale);
    }

    asyncContext->status = static_cast<UiAppearanceAbilityInterface::ErrCode>(resCode);
    if (asyncContext->status == UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR) {
        asyncContext->errMsg = PERMISSION_ERR_MSG;
    } else if (asyncContext->status == UiAppearanceAbilityInterface::ErrCode::INVALID_ARG) {
        asyncContext->errMsg = "fontWeightScale must between 0 and 5";
    } else {
        asyncContext->errMsg = "";
    }
}

void JsUiAppearance::OnComplete(napi_env env, napi_status status, void* data)
{
    LOGI("OnComplete begin.");
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        NapiThrow(env, "open handle scope failed.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return;
    }
    AsyncContext* asyncContext = static_cast<AsyncContext*>(data);
    if (asyncContext == nullptr) {
        NapiThrow(env, "asyncContext is null.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return;
    }

    if (asyncContext->status == UiAppearanceAbilityInterface::ErrCode::SUCCEEDED) {
        napi_value result = nullptr;
        napi_get_null(env, &result);
        if (asyncContext->deferred) { // promise
            napi_resolve_deferred(env, asyncContext->deferred, result);
        } else { // AsyncCallback
            napi_value callback = nullptr;
            napi_get_reference_value(env, asyncContext->callbackRef, &callback);
            napi_value ret;
            napi_call_function(env, nullptr, callback, 1, &result, &ret);
        }
    } else {
        napi_value code = nullptr;
        std::string strCode = std::to_string(asyncContext->status);
        napi_create_string_utf8(env, strCode.c_str(), strCode.length(), &code);

        napi_value msg = nullptr;
        auto iter = ERROR_CODE_TO_MSG.find(asyncContext->status);
        std::string strMsg = (iter != ERROR_CODE_TO_MSG.end() ? iter->second : "") + asyncContext->errMsg;
        LOGI("napi throw errCode %{public}d, strMsg %{public}s", asyncContext->status, strMsg.c_str());
        napi_create_string_utf8(env, strMsg.c_str(), strMsg.length(), &msg);

        napi_value error = nullptr;
        napi_create_error(env, code, msg, &error);
        if (asyncContext->deferred) { // promise
            napi_reject_deferred(env, asyncContext->deferred, error);
        } else { // AsyncCallback
            napi_value callback = nullptr;
            napi_get_reference_value(env, asyncContext->callbackRef, &callback);
            napi_value ret;
            napi_call_function(env, nullptr, callback, 1, &error, &ret);
        }
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
    napi_close_handle_scope(env, scope);
}

napi_status JsUiAppearance::CheckArgs(napi_env env, size_t argc, napi_value* argv)
{
    if (argc != ARGC_WITH_ONE && argc != ARGC_WITH_TWO) {
        NapiThrow(
            env, "the number of parameters can only be 1 or 2.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return napi_invalid_arg;
    }

    napi_valuetype valueType = napi_undefined;
    switch (argc) {
        case ARGC_WITH_TWO:
            napi_typeof(env, argv[1], &valueType);
            if (valueType != napi_function) {
                NapiThrow(env, "the second parameter must be a function.",
                    UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
                return napi_invalid_arg;
            }
            [[fallthrough]];
        case ARGC_WITH_ONE:
            napi_typeof(env, argv[0], &valueType);
            if (valueType != napi_number) {
                NapiThrow(
                    env, "the first parameter must be DarkMode.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
                return napi_invalid_arg;
            }
            break;
        default:
            return napi_invalid_arg;
    }
    return napi_ok;
}

napi_status JsUiAppearance::CheckFontScaleArgs(napi_env env, size_t argc, napi_value* argv)
{
    if (argc != ARGC_WITH_ONE && argc != ARGC_WITH_TWO) {
        NapiThrow(
            env, "the number of parameters can only be 1 or 2.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return napi_invalid_arg;
    }

    napi_valuetype valueType = napi_undefined;
    switch (argc) {
        case ARGC_WITH_TWO:
            napi_typeof(env, argv[1], &valueType);
            if (valueType != napi_function) {
                NapiThrow(env, "the second parameter must be a function.",
                    UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
                return napi_invalid_arg;
            }
            [[fallthrough]];
        case ARGC_WITH_ONE:
            napi_typeof(env, argv[0], &valueType);
            if (valueType != napi_number) {
                NapiThrow(
                    env, "the first parameter must be Number.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
                return napi_invalid_arg;
            }
            break;
        default:
            return napi_invalid_arg;
    }
    return napi_ok;
}

UiAppearanceAbilityInterface::DarkMode JsUiAppearance::ConvertJsDarkMode2Enum(int32_t jsVal)
{
    switch (jsVal) {
        case 0:
            return UiAppearanceAbilityInterface::DarkMode::ALWAYS_DARK;
        case 1:
            return UiAppearanceAbilityInterface::DarkMode::ALWAYS_LIGHT;
        default:
            return UiAppearanceAbilityInterface::DarkMode::UNKNOWN;
    }
}

static napi_value JSSetDarkMode(napi_env env, napi_callback_info info)
{
    LOGI("JSSetDarkMode begin.");

    size_t argc = ARGC_WITH_TWO;
    napi_value argv[ARGC_WITH_TWO] = { 0 };
    napi_status napiStatus = napi_ok;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napiStatus = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (napiStatus != napi_ok) {
        NapiThrow(env, "get callback info failed.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }
    napiStatus = JsUiAppearance::CheckArgs(env, argc, argv);
    if (napiStatus != napi_ok) {
        NapiThrow(env, "parameter parsing error.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }

    auto asyncContext = new (std::nothrow) AsyncContext();
    if (asyncContext == nullptr) {
        NapiThrow(env, "create AsyncContext failed.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return result;
    }

    napi_get_value_int32(env, argv[0], &asyncContext->jsSetArg);
    asyncContext->mode = JsUiAppearance::ConvertJsDarkMode2Enum(asyncContext->jsSetArg);
    if (argc == ARGC_WITH_TWO) {
        napi_create_reference(env, argv[1], 1, &asyncContext->callbackRef);
    }
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSSetDarkMode", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, JsUiAppearance::OnExecute, JsUiAppearance::OnComplete,
        reinterpret_cast<void*>(asyncContext), &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);

    return result;
}

static napi_value JSGetDarkMode(napi_env env, napi_callback_info info)
{
    LOGI("JSGetDarkMode begin.");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    size_t argc = 0;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr));
    if (argc != 0) {
        NapiThrow(env, "requires no parameter.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }

    auto mode = UiAppearanceAbilityClient::GetInstance()->GetDarkMode();
    if (mode == UiAppearanceAbilityInterface::ErrCode::SYS_ERR) {
        NapiThrow(env, "get dark-mode failed.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return result;
    }
    if (mode == UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR) {
        NapiThrow(env,
            "An attempt was made to get configuration forbidden by permission: ohos.permission.UPDATE_CONFIGURATION.",
            UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR);
        return result;
    }
    NAPI_CALL(env, napi_create_int32(env, mode, &result));
    return result;
}

static napi_value JSGetFontScale(napi_env env, napi_callback_info info)
{
    LOGI("JSGetFontScale begin.");
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    size_t argc = 0;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr));
    if (argc != 0) {
        NapiThrow(env, "requires no parameter.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }

    std::string fontScale;
    auto ret = UiAppearanceAbilityClient::GetInstance()->GetFontScale(fontScale);
    if (ret == UiAppearanceAbilityInterface::ErrCode::SYS_ERR) {
        NapiThrow(env, "get font-scale failed.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return result;
    }
    if (ret == UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR) {
        NapiThrow(env,
            "An attempt was made to get configuration forbidden by permission: ohos.permission.UPDATE_CONFIGURATION.",
            UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR);
        return result;
    }
    double fontScaleNumber = std::stod(fontScale);
    NAPI_CALL(env, napi_create_double(env, fontScaleNumber, &result));
    return result;
}

static napi_value JSSetFontScale(napi_env env, napi_callback_info info)
{
    LOGI("JSSetFontScale begin.");

    size_t argc = ARGC_WITH_TWO;
    napi_value argv[ARGC_WITH_TWO] = { 0 };
    napi_status napiStatus = napi_ok;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napiStatus = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (napiStatus != napi_ok) {
        NapiThrow(env, "get callback info failed.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }
    napiStatus = JsUiAppearance::CheckFontScaleArgs(env, argc, argv);
    if (napiStatus != napi_ok) {
        NapiThrow(env, "parameter parsing error.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }
    auto asyncContext = new (std::nothrow) AsyncContext();
    if (asyncContext == nullptr) {
        NapiThrow(env, "create AsyncContext failed.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return result;
    }
    napi_get_value_double(env, argv[0], &asyncContext->jsFontScale);

    asyncContext->fontScale = std::to_string(asyncContext->jsFontScale);
    if (argc == ARGC_WITH_TWO) {
        napi_create_reference(env, argv[1], 1, &asyncContext->callbackRef);
    }
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSSetFontScale", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, JsUiAppearance::OnSetFontScale, JsUiAppearance::OnComplete,
        reinterpret_cast<void*>(asyncContext), &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);

    return result;
}

static napi_value JSGetFontWeightScale(napi_env env, napi_callback_info info)
{
    LOGI("JSGetFontWeightScale begin.");
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    size_t argc = 0;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr));
    if (argc != 0) {
        NapiThrow(env, "requires no parameter.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }

    std::string fontWeightScale;
    auto ret = UiAppearanceAbilityClient::GetInstance()->GetFontWeightScale(fontWeightScale);
    if (ret == UiAppearanceAbilityInterface::ErrCode::SYS_ERR) {
        NapiThrow(env, "get font-Weight-scale failed.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return result;
    }
    if (ret == UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR) {
        NapiThrow(env,
            "An attempt was made to get configuration forbidden by permission: ohos.permission.UPDATE_CONFIGURATION.",
            UiAppearanceAbilityInterface::ErrCode::PERMISSION_ERR);
        return result;
    }
    double fontWeightScaleNumber = std::stod(fontWeightScale);
    NAPI_CALL(env, napi_create_double(env, fontWeightScaleNumber, &result));
    return result;
}

static napi_value JSSetFontWeightScale(napi_env env, napi_callback_info info)
{
    LOGI("JSSetFontWeightScale begin.");

    size_t argc = ARGC_WITH_TWO;
    napi_value argv[ARGC_WITH_TWO] = { 0 };
    napi_status napiStatus = napi_ok;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napiStatus = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (napiStatus != napi_ok) {
        NapiThrow(env, "get callback info failed.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }
    napiStatus = JsUiAppearance::CheckFontScaleArgs(env, argc, argv);
    if (napiStatus != napi_ok) {
        NapiThrow(env, "parameter parsing error.", UiAppearanceAbilityInterface::ErrCode::INVALID_ARG);
        return result;
    }
    auto asyncContext = new (std::nothrow) AsyncContext();
    if (asyncContext == nullptr) {
        NapiThrow(env, "create AsyncContext failed.", UiAppearanceAbilityInterface::ErrCode::SYS_ERR);
        return result;
    }
    napi_get_value_double(env, argv[0], &asyncContext->jsFontWeightScale);

    asyncContext->fontWeightScale = std::to_string(asyncContext->jsFontWeightScale);
    if (argc == ARGC_WITH_TWO) {
        napi_create_reference(env, argv[1], 1, &asyncContext->callbackRef);
    }
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSSetFontWeightScale", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, JsUiAppearance::OnSetFontWeightScale, JsUiAppearance::OnComplete,
        reinterpret_cast<void*>(asyncContext), &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);

    return result;
}

EXTERN_C_START
static napi_value UiAppearanceExports(napi_env env, napi_value exports)
{
    napi_value DarkMode = nullptr;
    napi_value alwaysDark = nullptr;
    napi_value alwaysLight = nullptr;
    NAPI_CALL(env, napi_create_int32(env, 0, &alwaysDark));
    NAPI_CALL(env, napi_create_int32(env, 1, &alwaysLight));
    NAPI_CALL(env, napi_create_object(env, &DarkMode));
    NAPI_CALL(env, napi_set_named_property(env, DarkMode, "ALWAYS_DARK", alwaysDark));
    NAPI_CALL(env, napi_set_named_property(env, DarkMode, "ALWAYS_LIGHT", alwaysLight));
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setDarkMode", JSSetDarkMode),
        DECLARE_NAPI_FUNCTION("getDarkMode", JSGetDarkMode),
        DECLARE_NAPI_FUNCTION("getFontScale", JSGetFontScale),
        DECLARE_NAPI_FUNCTION("setFontScale", JSSetFontScale),
        DECLARE_NAPI_FUNCTION("getFontWeightScale", JSGetFontWeightScale),
        DECLARE_NAPI_FUNCTION("setFontWeightScale", JSSetFontWeightScale),
        DECLARE_NAPI_STATIC_PROPERTY("DarkMode", DarkMode),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}
EXTERN_C_END

static napi_module ui_appearance_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = UiAppearanceExports,
    .nm_modname = "uiAppearance", // relative to the module name while import.
    .nm_priv = nullptr,
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void UiAppearanceRegister()
{
    napi_module_register(&ui_appearance_module);
}
} // namespace ArkUi::UiAppearance
} // namespace OHOS

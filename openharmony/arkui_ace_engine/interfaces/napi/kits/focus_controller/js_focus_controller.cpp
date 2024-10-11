/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


#include "interfaces/napi/kits/utils/napi_utils.h"

#include "frameworks/bridge/common/utils/engine_helper.h"

namespace OHOS::Ace::Napi {
namespace {
constexpr size_t STR_BUFFER_SIZE = 1024;
constexpr size_t ARGC_ACTIVATE_PARAMTER = 2;
}

static napi_value JSClearFocus(napi_env env, napi_callback_info info)
{
    auto delegate = EngineHelper::GetCurrentDelegateSafely();
    if (!delegate) {
        return nullptr;
    }
    delegate->ResetFocus();
    return nullptr;
}

static napi_value JSRequestFocus(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data);
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");
    napi_valuetype type = napi_undefined;
    napi_typeof(env, argv, &type);
    NAPI_ASSERT(env, type == napi_string, "the type of arg is not string");
    char outBuffer[STR_BUFFER_SIZE] = { 0 };
    size_t outSize = 0;
    napi_get_value_string_utf8(env, argv, outBuffer, STR_BUFFER_SIZE, &outSize);
    std::string key = std::string(outBuffer);

    napi_value obj = nullptr;
    auto delegate = EngineHelper::GetCurrentDelegateSafely();
    if (!delegate) {
        napi_get_boolean(env, false, &obj);
        return obj;
    }
    auto focusCallback = [env](NG::RequestFocusResult result) {
        switch (result) {
            case NG::RequestFocusResult::NON_FOCUSABLE:
                NapiThrow(env, "This component is not focusable.", ERROR_CODE_NON_FOCUSABLE);
                break;
            case NG::RequestFocusResult::NON_FOCUSABLE_ANCESTOR:
                NapiThrow(env, "This component has unfocusable ancestor.", ERROR_CODE_NON_FOCUSABLE_ANCESTOR);
                break;
            case NG::RequestFocusResult::NON_EXIST:
                NapiThrow(env,
                    "The component doesn't exist, is currently invisible, or has been disabled.",
                    ERROR_CODE_NON_EXIST);
                break;
            default:
                NapiThrow(env, "An internal error occurred.", ERROR_CODE_INTERNAL_ERROR);
                break;
        }
    };
    delegate->SetRequestFocusCallback(focusCallback);
    delegate->RequestFocus(key, true);
    delegate->ResetRequestFocusCallback();
    napi_get_null(env, &obj);
    return obj;
}

static napi_value JSActivate(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ACTIVATE_PARAMTER;
    napi_value argv[ARGC_ACTIVATE_PARAMTER] = { nullptr };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, argc >= 1, "requires at least 1 parameter");
    napi_valuetype type = napi_undefined;
    napi_typeof(env, argv[0], &type);
    NAPI_ASSERT(env, type == napi_boolean, "the type of argv[0] is not bool");
    bool isActive = false;
    napi_get_value_bool(env, argv[0], &isActive);

    bool autoInactive = true;
    if (argc == ARGC_ACTIVATE_PARAMTER) {
        napi_typeof(env, argv[1], &type);
        NAPI_ASSERT(env, type == napi_boolean, "the type of argv[1] is not bool");
        napi_get_value_bool(env, argv[1], &autoInactive);
    }

    napi_value obj = nullptr;
    auto delegate = EngineHelper::GetCurrentDelegateSafely();
    if (!delegate) {
        napi_get_boolean(env, false, &obj);
        return obj;
    }
    delegate->Activate(isActive, autoInactive);
    napi_get_null(env, &obj);
    return obj;
}

static napi_value registerFunc(napi_env env, napi_value exports)
{
    napi_property_descriptor animatorDesc[] = {
        DECLARE_NAPI_FUNCTION("clearFocus", JSClearFocus),
        DECLARE_NAPI_FUNCTION("requestFocus", JSRequestFocus),
        DECLARE_NAPI_FUNCTION("activate", JSActivate),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(animatorDesc) / sizeof(animatorDesc[0]), animatorDesc));
    return exports;
}

static napi_module focusControllerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerFunc,
    .nm_modname = "arkui.focusController",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void FocusControllerRegister()
{
    napi_module_register(&focusControllerModule);
}
} // namespace OHOS::Ace::Napi
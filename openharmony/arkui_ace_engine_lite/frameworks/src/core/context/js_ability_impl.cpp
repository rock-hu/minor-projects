/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "js_ability_impl.h"
#include "ace_event_error_code.h"
#include "ace_log.h"
#include "component.h"
#include "component_utils.h"
#include "fatal_handler.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_profiler.h"
#include "module_manager.h"
#include "presets/localization_module.h"
#include "presets/timer_module.h"
#include "securec.h"
#ifdef _MINI_MULTI_TASKS_
#include "string_util.h"
#endif

namespace OHOS {
namespace ACELite {
#ifdef _MINI_MULTI_TASKS_
namespace {
constexpr char EMPTY_OBJECT_JSON_STRING[] = "{}";
constexpr char ERROR_CODE_KEY[] = "errorCode";
} // namespace
#endif
void JSAbilityImpl::InitEnvironment(const char * const abilityPath, const char * const bundleName, uint16_t token)
{
    if ((abilityPath == nullptr) || strlen(abilityPath) == 0 || (bundleName == nullptr) || strlen(bundleName) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid input parameters");
        return;
    }

    if (isEnvInit_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "already initialized, return");
        return;
    }
    // init engine && js fwk
    JsAppEnvironment *appJsEnv = JsAppEnvironment::GetInstance();
    appContext_ = JsAppContext::GetInstance();
    // check if we should use snapshot mode, do this before everything,
    // but after debugger config is set
    appJsEnv->InitRuntimeMode();
    appContext_->SetCurrentAbilityInfo(abilityPath, bundleName, token);
    appContext_->SetTopJSAbilityImpl(this);
    appJsEnv->InitJsFramework();
    appContext_->LoadApiVersion();

    // initialize js object after engine started up
    abilityModel_ = UNDEFINED;
    nativeElement_ = UNDEFINED;
    isEnvInit_ = true;

    // relocate app.js fullpath
    const char * const appJSFileName = (appJsEnv->IsSnapshotMode()) ? "app.bc" : "app.js";
    char *fileFullPath = RelocateJSSourceFilePath(abilityPath, appJSFileName);
    if (fileFullPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "relocate js file failed");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_APP_ENTRY_MISSING);
        return;
    }

    appContext_->SetAbilityState(TopAbilityState::ABILITY_INITIALIZED);
    START_TRACING(APP_CODE_EVAL);
    MarkAppViewModelEvaling(true);
    abilityModel_ = appContext_->Eval(fileFullPath, strlen(fileFullPath), true); // generate global.$app js object
    MarkAppViewModelEvaling(false);
    STOP_TRACING();

    ace_free(fileFullPath);
    fileFullPath = nullptr;
    router_ = new Router();
    if (router_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc router heap memory failed.");
        return;
    }
}

void JSAbilityImpl::CleanUp()
{
    if (appContext_ == nullptr) {
        return;
    }
    appContext_->SetAbilityState(TopAbilityState::ABILITY_DESTROYING);
    if (!isEnvInit_) {
        return;
    }
    START_TRACING(APP_ON_DESTROY);
    // clean up user's js
    InvokeOnDestroy();
    STOP_TRACING();

    // delete router's resources about page
    if (router_ != nullptr) {
        delete router_;
        router_ = nullptr;
    }
#ifdef _MINI_MULTI_TASKS_
    if (pageInfo_ != nullptr) {
        ACE_FREE(pageInfo_);
        pageInfo_ = nullptr;
    }
#endif
    TimersModule::Clear();
    LocalModule::Clear();

    appContext_->ClearContext();

    ModuleManager::GetInstance()->OnTerminate();
    JsAppEnvironment::GetInstance()->Cleanup();
    isEnvInit_ = false;
    appContext_->SetAbilityState(TopAbilityState::ABILITY_DESTROYED);
    OUTPUT_TRACE();
}

void JSAbilityImpl::DeliverCreate(const char *param)
{
    if (appContext_ == nullptr) {
        return;
    }
    appContext_->SetAbilityState(TopAbilityState::ABILITY_LAUNCHING);
    START_TRACING(APP_ON_CREATE);
    // call InvokeOnCreate
    InvokeOnCreate();
    STOP_TRACING();
    // if we have done the render or not initialized yet, don't call render
    if (rendered_) {
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_APP_RENDER_FAILED);
        return;
    }

#ifndef _MINI_MULTI_TASKS_
    // call render to setup user interface
    jerry_value_t object = UNDEFINED;
    if (param == nullptr) {
        object = jerry_create_object();
        JerrySetStringProperty(object, ROUTER_PAGE_URI, PATH_DEFAULT);
    } else {
        object = jerry_json_parse(reinterpret_cast<const jerry_char_t *>(param), strlen(param));
    }
    if (router_) {
        jerry_release_value(router_->Replace(object, false));
        rendered_ = true;
    }
    jerry_release_value(object);
    appContext_->SetAbilityState(TopAbilityState::ABILITY_LAUNCHDONE);
#endif
}

#ifdef _MINI_MULTI_TASKS_
void JSAbilityImpl::OnRestoreData(AbilitySlite::AbilitySavedData *abilitySavedData)
{
    if (pageInfo_ != nullptr) {
        ACE_FREE(pageInfo_);
        pageInfo_ = nullptr;
    }

    if (abilitySavedData == nullptr) {
        InvokeOnRestoreData(EMPTY_OBJECT_JSON_STRING, AbilitySlite::SavedResultCode::SAVED_RESULT_NO_DATA);
        return;
    }

    // deal user saved data
    char *userData = StringUtil::Malloc(AbilitySlite::SAVED_DATA_LIMIT);
    if (userData == nullptr) {
        InvokeOnRestoreData(EMPTY_OBJECT_JSON_STRING, AbilitySlite::SavedResultCode::SAVED_RESULT_ALLOC_ERROR);
        return;
    }

    uint16_t userDataLen = AbilitySlite::SAVED_DATA_LIMIT;
    AbilitySlite::SavedResultCode userDataRet = abilitySavedData->GetUserSavedData(
        reinterpret_cast<void *>(userData), AbilitySlite::SAVED_DATA_LIMIT, &userDataLen);
    if (userDataRet != AbilitySlite::SavedResultCode::SAVED_RESULT_OK) {
        InvokeOnRestoreData(EMPTY_OBJECT_JSON_STRING, userDataRet);
        ACE_FREE(userData);
        return;
    }

    // deal framework saved data pageInfo
    pageInfo_ = StringUtil::Malloc(AbilitySlite::SAVED_DATA_LIMIT);
    if (pageInfo_ == nullptr) {
        InvokeOnRestoreData(EMPTY_OBJECT_JSON_STRING, AbilitySlite::SavedResultCode::SAVED_RESULT_NO_DATA);
        return;
    }
    uint16_t pageInfoLen = AbilitySlite::SAVED_DATA_LIMIT;
    AbilitySlite::SavedResultCode pageInfoRet = abilitySavedData->GetSavedData(
        reinterpret_cast<void*>(pageInfo_), AbilitySlite::SAVED_DATA_LIMIT, &pageInfoLen);
    if (pageInfoRet != AbilitySlite::SavedResultCode::SAVED_RESULT_OK) {
        ACE_FREE(pageInfo_);
        pageInfo_ = nullptr;
        InvokeOnRestoreData(EMPTY_OBJECT_JSON_STRING, AbilitySlite::SavedResultCode::SAVED_RESULT_NO_DATA);
        return;
    }

    // call js function
    bool success = InvokeOnRestoreData(userData, AbilitySlite::SavedResultCode::SAVED_RESULT_OK);
    ACE_FREE(userData);
    if (!success) {
        // if restore data failed, not restore page
        ACE_FREE(pageInfo_);
        pageInfo_ = nullptr;
        return;
    }
}
#endif

void JSAbilityImpl::Show()
{
    if (appContext_ == nullptr) {
        return;
    }
    if (router_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "no router instance to perform the show request");
        return;
    }
#ifdef _MINI_MULTI_TASKS_
    if (pageInfo_ == nullptr) {
        GotoPage(PATH_DEFAULT);
    } else {
        GotoPage(pageInfo_);
        ACE_FREE(pageInfo_);
        pageInfo_ = nullptr;
    }
#endif
    appContext_->SetAbilityState(TopAbilityState::ABILITY_SHOWING);
    router_->Show();
    FatalHandler::GetInstance().NotifyVisibleStatusChange(true);
    appContext_->SetAbilityState(TopAbilityState::ABILITY_SHOWN);
}

#ifdef _MINI_MULTI_TASKS_
void JSAbilityImpl::OnSaveData(AbilitySlite::AbilitySavedData *abilitySavedData)
{
    if (abilitySavedData == nullptr) {
        return;
    }

    if (IS_UNDEFINED(abilityModel_) || (router_ == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "view model or router is invalid when call user's onSaveData");
        abilitySavedData->SetSavedResultCode(AbilitySlite::SavedResultCode::SAVED_RESULT_NO_DATA);
        return;
    }
    // get js function
    jerry_value_t onSaveFunction = jerryx_get_property_str(abilityModel_, ABILITY_LIFECYCLE_CALLBACK_ON_SAVE_DATA);
    if (IS_UNDEFINED(onSaveFunction)) {
        // user does not set onRestore method
        abilitySavedData->SetSavedResultCode(AbilitySlite::SavedResultCode::SAVED_RESULT_NO_DATA);
        return;
    }

    JSValue jerryUserData = jerry_create_object();
    jerry_value_t args[1] = { jerryUserData };
    CallJSFunctionAutoRelease(onSaveFunction, abilityModel_, args, 1);

    jerry_value_t dataJson = jerry_json_stringify(args[0]);

    char *userData = MallocStringOf(dataJson);
    ReleaseJerryValue(dataJson, jerryUserData, onSaveFunction, VA_ARG_END_FLAG);

    if (userData == nullptr) {
        abilitySavedData->SetSavedResultCode(AbilitySlite::SavedResultCode::SAVED_RESULT_ALLOC_ERROR);
        return;
    }

    AbilitySlite::SavedResultCode retCode =
        abilitySavedData->SetUserSavedData(reinterpret_cast<const void *>(userData), strlen(userData)  + 1);
    ACE_FREE(userData);
    if (retCode != AbilitySlite::SavedResultCode::SAVED_RESULT_OK) {
        abilitySavedData->SetSavedResultCode(retCode);
        return;
    }

    const char* uri = appContext_->GetCurrentUri();
    if (uri == nullptr) {
        retCode =
            abilitySavedData->SetSavedData(reinterpret_cast<const void *>(PATH_DEFAULT), strlen(PATH_DEFAULT) + 1);
    } else {
        retCode = abilitySavedData->SetSavedData(reinterpret_cast<const void *>(uri), strlen(uri) + 1);
    }
    if (retCode != AbilitySlite::SavedResultCode::SAVED_RESULT_OK) {
        abilitySavedData->SetSavedResultCode(retCode);
        return;
    }
    abilitySavedData->SetSavedResultCode(AbilitySlite::SavedResultCode::SAVED_RESULT_OK);
}
#endif

void JSAbilityImpl::Hide() const
{
    if (appContext_ == nullptr) {
        return;
    }
    if (router_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "no router instance to perform the hide request");
        return;
    }
    appContext_->SetAbilityState(TopAbilityState::ABILITY_HIDING);
    router_->Hide();
    FatalHandler::GetInstance().NotifyVisibleStatusChange(false);
    appContext_->SetAbilityState(TopAbilityState::ABILITY_HIDDEN);
}

void JSAbilityImpl::NotifyBackPressed() const
{
    if (appContext_ == nullptr) {
        return;
    }

    InvokeOnBackPressed();

    appContext_->TerminateAbility();
}

void JSAbilityImpl::InvokeOnCreate() const
{
    if (IS_UNDEFINED(abilityModel_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "view model is undefined when call user's init");
        return;
    }
    jerry_value_t onCreateFunction = jerryx_get_property_str(abilityModel_, ABILITY_LIFECYCLE_CALLBACK_ON_CREATE);
    if (IS_UNDEFINED(onCreateFunction)) {
        // user does not set onInit method
        return;
    }
    CallJSFunctionAutoRelease(onCreateFunction, abilityModel_, nullptr, 0);
    jerry_release_value(onCreateFunction);
}

#ifdef _MINI_MULTI_TASKS_
// call js function
bool JSAbilityImpl::InvokeOnRestoreData(const char* userData, AbilitySlite::SavedResultCode retCode) const
{
    // call render to setup user interface
    if (userData == nullptr) {
        return false;
    }
    if (IS_UNDEFINED(abilityModel_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "view model is undefined when call user's onRestoreData");
        return false;
    }

    jerry_value_t args[1];
    args[0] = jerry_json_parse(reinterpret_cast<const jerry_char_t *>(userData), strlen(userData));

    int16_t errorCode = 0;
    switch (retCode) {
        case AbilitySlite::SavedResultCode::SAVED_RESULT_OK:
            errorCode = 0;
            break;
        case AbilitySlite::SavedResultCode::SAVED_RESULT_EXCEED_UPPER_LIMIT:
            errorCode = 1;
            break;
        default:
            errorCode = 2; // 2: default
            break;
    }
    jerry_value_t propName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ERROR_CODE_KEY));
    jerry_value_t propValue = jerry_create_number(errorCode);
    jerry_value_t setResult = jerry_set_property(args[0], propName, propValue);

    jerry_value_t onRestoreDataFunction =
        jerryx_get_property_str(abilityModel_, ABILITY_LIFECYCLE_CALLBACK_ON_RESTORE_DATA);
    if (IS_UNDEFINED(onRestoreDataFunction)) {
        // user does not set onRestore method
        ReleaseJerryValue(propName, propValue, setResult, args[0], VA_ARG_END_FLAG);
        return false;
    }
    CallJSFunctionAutoRelease(onRestoreDataFunction, abilityModel_, args, 1);   // todo lizhiqi args release?
    ReleaseJerryValue(propName, propValue, setResult, args[0], onRestoreDataFunction, VA_ARG_END_FLAG);
    return true;
}
#endif

void JSAbilityImpl::InvokeOnDestroy() const
{
    InvokeMethodWithoutParameter(ABILITY_LIFECYCLE_CALLBACK_ON_DESTROY);
    // release FeatureAbility object
    jerry_release_value(abilityModel_);
}

void JSAbilityImpl::InvokeOnBackPressed() const
{
    InvokeMethodWithoutParameter(BACK_PRESSED_NAME);
}

void JSAbilityImpl::InvokeMethodWithoutParameter(const char * const name) const
{
    if (FatalHandler::GetInstance().IsJSRuntimeFatal()) {
        // can not continue to involve any JS object creating on engine in case runtime fatal
        return;
    }
    if ((name == nullptr) || strlen(name) == 0) {
        return;
    }

    jerry_value_t function = jerryx_get_property_str(abilityModel_, name);
    if (IS_UNDEFINED(function)) {
        // user does not set onBackpress method
        return;
    }
    CallJSFunctionAutoRelease(function, abilityModel_, nullptr, 0);
    jerry_release_value(function);
}

void JSAbilityImpl::MarkAppViewModelEvaling(bool evaling) const
{
    jerry_value_t propNameValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>("__appVing__"));
    jerry_value_t globalObject = jerry_get_global_object();
    jerry_value_t evalingValue = jerry_create_boolean(evaling);
    jerry_release_value(jerry_set_property(globalObject, propNameValue, evalingValue));
    jerry_release_value(evalingValue);
    jerry_release_value(propNameValue);
    jerry_release_value(globalObject);
}

#ifdef _MINI_MULTI_TASKS_
void JSAbilityImpl::GotoPage(const char* uri)
{
    if ((uri == nullptr) || (router_ == nullptr)) {
        return;
    }

    jerry_value_t object = jerry_create_object();
    JerrySetStringProperty(object, ROUTER_PAGE_URI, uri);
    jerry_release_value(router_->Replace(object, false));
    jerry_release_value(object);

    rendered_ = true;
}
#endif
} // namespace ACELite
} // namespace OHOS

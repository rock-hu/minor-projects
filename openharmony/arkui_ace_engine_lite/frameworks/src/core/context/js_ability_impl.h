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

#ifndef OHOS_ACELITE_JS_ABILITY_IMPL_H
#define OHOS_ACELITE_JS_ABILITY_IMPL_H

#ifdef _MINI_MULTI_TASKS_
#include "ability_saved_data.h"
#endif

#include "js_app_context.h"
#include "js_debugger_config.h"
#include "js_router.h"
#include "js_timer_list.h"
#if IS_ENABLED(JS_PROFILER)
#include "js_profiler.h"
#endif
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class JSAbilityImpl final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(JSAbilityImpl);
    /**
     * @fn JSAbilityImpl::JSAbilityImpl()
     *
     * @brief constructor with the ability path parameter.
     */
    JSAbilityImpl()
        : appContext_(nullptr),
          abilityModel_(0),
          nativeElement_(0),
          rendered_(false),
          isEnvInit_(false),
#ifdef _MINI_MULTI_TASKS_
          pageInfo_(nullptr),
#endif
          router_(nullptr)
    {
#if IS_ENABLED(JS_PROFILER)
        // Call GetInstance() to prepare data
        JSProfiler::GetInstance()->PrepareDataBuffer();
#endif
    }

    /**
     * @fn virtual JSAbilityImpl::~JSAbilityImpl()
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~JSAbilityImpl()
    {
#if IS_ENABLED(JS_PROFILER)
        // Call release() to free all performance data
        JSProfiler::GetInstance()->Release();
#endif
    }

    /**
     * @fn JSAbilityRuntime::Init(char* path, uint16_t token)
     *
     * @brief initialize JS executing environment, including JS engine and JS fwk
     */
    void InitEnvironment(const char * const abilityPath, const char * const bundleName, uint16_t token);

    /**
     * @fn JSAbilityRuntime::CleanUp()
     *
     * @brief destroy JS executing environment
     */
    void CleanUp();

    /**
     * @fn JSAbilityRuntime::DeliverCreate();
     * @param param the JS object to init the app start
     * @brief call this function when transfer lifecycle into init
     */
    void DeliverCreate(const char *param = nullptr);

#ifdef _MINI_MULTI_TASKS_
    /**
     * @fn JSAbilityRuntime::OnRestoreData()
     *
     * @brief Called when the user data need to be restored.
    */
    void OnRestoreData(AbilitySlite::AbilitySavedData* abilitySavedData);
#endif

    /**
     * @fn JSAbilityRuntime::Show();
     *
     * @brief move JS application to active state
     */
    void Show();

    /**
     * @fn JSAbilityRuntime::Hide();
     *
     * @brief move the JS application to background state
     */
    void Hide() const;

#ifdef _MINI_MULTI_TASKS_
    /**
     * @fn JSAbilityRuntime::OnSaveData()
     *
     * @brief Called when the user data need to be saved.
    */
    void OnSaveData(AbilitySlite::AbilitySavedData* abilitySavedData);
#endif

    /**
     * @fn JSAbilityRuntime::NotifyBackPressed()
     *
     * @brief call this function when back key pressed
     */
    void NotifyBackPressed() const;

    /**
     * @fn JSAbilityRuntime::GetRouter();
     *
     * @brief call this function when replace new page
     */
    const Router *GetRouter() const
    {
        return router_;
    }

    jerry_value_t GetAppVM() const
    {
        return abilityModel_;
    }

private:
    void InvokeOnCreate() const;
#ifdef _MINI_MULTI_TASKS_
    bool InvokeOnRestoreData(const char* userData, AbilitySlite::SavedResultCode retCode) const;
    void GotoPage(const char* uri);
#endif
    void InvokeOnDestroy() const;
    void InvokeOnBackPressed() const;
    void InvokeMethodWithoutParameter(const char * const name) const;
    void MarkAppViewModelEvaling(bool evaling) const;

    JsAppContext *appContext_;
    jerry_value_t abilityModel_;  // the object evaled from user JS code
    jerry_value_t nativeElement_; // the object returned from render function
    bool rendered_;
    bool isEnvInit_;
#ifdef _MINI_MULTI_TASKS_
    char *pageInfo_;
#endif
    Router *router_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_JS_ABILITY_IMPL_H

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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_UI_EXTENSION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_UI_EXTENSION_H

#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_component/ui_extension_proxy.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_config.h"

namespace OHOS::Ace::Framework {
class JSUIExtension : public JSViewAbstract, public JSInteractableView {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void OnRemoteReady(const JSCallbackInfo& info);
    static void OnReceive(const JSCallbackInfo& info);
    static void OnRelease(const JSCallbackInfo& info);
    static void OnResult(const JSCallbackInfo& info);
    static void OnError(const JSCallbackInfo& info);
    static void OnTerminated(const JSCallbackInfo& info);
    static void OnDrawReady(const JSCallbackInfo& info);
private:
    static void ResolveAreaPlaceholderParams(const JSRef<JSObject>& obj,
        std::map<NG::PlaceholderType, RefPtr<NG::FrameNode>>& placeholderMap);
};

enum class RegisterType {
    SYNC = 0,
    ASYNC,
    UNKNOWN
};

using CallbackFuncPairList = std::list<std::pair<napi_ref, std::function<void(const RefPtr<NG::UIExtensionProxy>&)>>>;

class JSUIExtensionProxy : public Referenced {
public:
    JSUIExtensionProxy() = default;
    ~JSUIExtensionProxy() override = default;
    static void JSBind(BindingTarget globalObj);
    void Send(const JSCallbackInfo& info);
    void SendSync(const JSCallbackInfo& info);
    void On(const JSCallbackInfo& info);
    void Off(const JSCallbackInfo& info);
    void SetProxy(const RefPtr<NG::UIExtensionProxy>& proxy);
    void SetInstanceId(int32_t instanceId);
private:
    static void Constructor(const JSCallbackInfo& info);
    static void Destructor(JSUIExtensionProxy* uiExtensionProxy);

    CallbackFuncPairList::const_iterator FindCbList(napi_env env, napi_value cb,
        CallbackFuncPairList& callbackFuncPairList);
    void AddCallbackToList(napi_env env, napi_value cb, napi_handle_scope scope, RegisterType type,
        const std::function<void(const RefPtr<NG::UIExtensionProxy>&)>&& onFunc);
    void DeleteCallbackFromList(uint32_t argc, napi_env env, napi_value cb, RegisterType type);
    std::list<std::function<void(const RefPtr<NG::UIExtensionProxy>&)>> GetOnFuncList(RegisterType type);
    RegisterType GetRegisterType(const std::string& strType);

    RefPtr<NG::UIExtensionProxy> proxy_;
    int32_t instanceId_ = INSTANCE_ID_UNDEFINED;
    CallbackFuncPairList onSyncOnCallbackList_;
    CallbackFuncPairList onAsyncOnCallbackList_;
    std::mutex callbackLisLock_;
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_UI_EXTENSION_H

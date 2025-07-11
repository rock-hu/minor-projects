/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_REGISTER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_REGISTER_H

#include "frameworks/bridge/declarative_frontend/engine/bindings_defines.h"
#include "frameworks/bridge/declarative_frontend/engine/js_object_template.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view.h"


namespace OHOS::Ace::Framework {

/**
 * register C++ classes that are to be exposed to JS
 */
void JsRegisterViews(BindingTarget globalObj, void* nativeEngine = nullptr, bool isCustomEnvSupported = false);
void JsRegisterFormViews(BindingTarget globalObj, const std::unordered_set<std::string>& formModuleList,
    bool isReload = false, void* nativeEngine = nullptr);
void JsRegisterWorkerViews(BindingTarget globalObj, void* nativeEngine);

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_REGISTER_H

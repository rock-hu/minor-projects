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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FORM_MENU_ITEM_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FORM_MENU_ITEM_H

#include "frameworks/bridge/declarative_frontend/jsview/js_menu_item.h"

namespace OHOS::Ace::Framework {

class JSFormMenuItem : public JSMenuItem {
public:
    static void JSBind(BindingTarget globalObj);
    static void JsOnRequestPublishFormWithSnapshot(const JSCallbackInfo& info);
    static void OnClickParameterCheck(const JSCallbackInfo& info, bool& retFlag);
private:
    static void RequestPublishFormWithSnapshot(JSRef<JSVal> wantValue,
        const std::string& formBindingDataStr, RefPtr<JsFunction> jsCBFunc);
    static inline RefPtr<JsFunction> jsCBFunc_ = nullptr;
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FORM_MENU_ITEM_H
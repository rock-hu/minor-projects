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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LOCATION_BUTTON_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LOCATION_BUTTON_H

#include "bridge/declarative_frontend/engine/bindings.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_sec_button_base.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "core/components_ng/pattern/security_component/location_button/location_button_common.h"
#include "core/event/mouse_event.h"
#include "core/gestures/click_recognizer.h"

namespace OHOS::Ace::Framework {
class JsLocationButtonClickFunction : public JsFunction {
    DECLARE_ACE_TYPE(JsLocationButtonClickFunction, JsFunction)
public:
    explicit JsLocationButtonClickFunction(const JSRef<JSFunc>& jsFunction) :
        JsFunction(JSRef<JSObject>(), jsFunction) {}

    ~JsLocationButtonClickFunction() override {};
    void Execute()
    {
        JsFunction::ExecuteJS();
    };

    void Execute(GestureEvent& info);
};

class JSLocationButton : public JSViewAbstract, public JSInteractableView {
public:
    static void JSBind(BindingTarget globalObj);
    static bool ParseComponentStyle(const JSCallbackInfo& info, LocationButtonLocationDescription& text,
        LocationButtonIconStyle& icon, int32_t& bg);
    static void Create(const JSCallbackInfo& info);
    static void JsOnClick(const JSCallbackInfo& info);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LOCATION_BUTTON_H

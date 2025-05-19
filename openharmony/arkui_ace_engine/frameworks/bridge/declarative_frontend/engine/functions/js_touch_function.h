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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_TOUCH_FUNCTION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_TOUCH_FUNCTION_H

#include "base/geometry/offset.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/core/gestures/raw_recognizer.h"

namespace OHOS::Ace::Framework {

class JsTouchFunction : public virtual JsFunction {
    DECLARE_ACE_TYPE(JsTouchFunction, JsFunction)

public:
    explicit JsTouchFunction(const JSRef<JSFunc>& jsFunction) : JsFunction(JSRef<JSObject>(), jsFunction) {}
    JsTouchFunction(const JSRef<JSObject>& jsObject, const JSRef<JSFunc>& jsFunction) : JsFunction(jsObject, jsFunction)
    {}
    ~JsTouchFunction() override {};

    void Execute() override
    {
        JsFunction::Execute();
    }
    void Execute(TouchEventInfo& info);
    static JSRef<JSObject> CreateTouchInfo(const TouchLocationInfo& touchInfo, TouchEventInfo& info);
    static JSRef<JSObject> CreateJSEventInfo(TouchEventInfo& info);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_TOUCH_FUNCTION_H

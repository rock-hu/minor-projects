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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_ON_AREA_CHANGE_FUNCTION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_ON_AREA_CHANGE_FUNCTION_H

#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/geometry/offset.h"
#include "base/geometry/rect.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/core/gestures/drag_recognizer.h"

namespace OHOS::Ace::Framework {

class JsOnAreaChangeFunction : public JsFunction {
    DECLARE_ACE_TYPE(JsOnAreaChangeFunction, JsFunction)

public:
    explicit JsOnAreaChangeFunction(const JSRef<JSFunc>& jsFunction) : JsFunction(jsFunction) {}
    ~JsOnAreaChangeFunction() override = default;
    void Execute()
    {
        ExecuteJS();
    }
    void Execute(const Rect& oldRect, const Offset& oldOrigin, const Rect& rect, const Offset& origin);
    void Execute(
        const NG::RectF& oldRect, const NG::OffsetF& oldOrigin, const NG::RectF& rect, const NG::OffsetF& origin);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_FUNCTION_JS_ON_AREA_CHANGE_FUNCTION_H
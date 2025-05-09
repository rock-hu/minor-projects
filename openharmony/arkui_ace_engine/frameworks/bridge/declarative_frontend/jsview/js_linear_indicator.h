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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LINEAR_INDICATOR_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LINEAR_INDICATOR_H

#include "core/components_ng/pattern/linear_indicator/linear_indicator_theme.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {
class JSLinearIndicator : public JSViewAbstract, public JSInteractableView {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void SetStyle(const JSCallbackInfo& info);
    static void Loop(const JSCallbackInfo& info);
    static void OnChange(const JSCallbackInfo& info);

    static void JsHeight(const JSCallbackInfo& info);
    static void JsPadding(const JSCallbackInfo& info);
    static void SetPaddingLeft(const JSCallbackInfo& info);
    static void SetPaddingRight(const JSCallbackInfo& info);
    static void JsIgnore(const JSCallbackInfo& info);

private:
    static void SetSpace(const JSRef<JSObject>& paramObject, const RefPtr<NG::LinearIndicatorTheme>& theme);
    static void SetStrokeWidth(const JSRef<JSObject>& paramObject, const RefPtr<NG::LinearIndicatorTheme>& theme);
    static void SetStrokeRadius(const JSRef<JSObject>& paramObject, const RefPtr<NG::LinearIndicatorTheme>& theme);
    static void SetTrackBackgroundColor(
        const JSRef<JSObject>& paramObject, const RefPtr<NG::LinearIndicatorTheme>& theme);
    static void SetTrackColor(const JSRef<JSObject>& paramObject, const RefPtr<NG::LinearIndicatorTheme>& theme);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LINEAR_INDICATOR_H

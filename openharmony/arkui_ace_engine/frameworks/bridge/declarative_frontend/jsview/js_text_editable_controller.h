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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXT_EDITABLE_CONTROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXT_EDITABLE_CONTROLLER_H

#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/engine/js_types.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/core/components_ng/pattern/text_field/text_field_model.h"

namespace OHOS::Ace::Framework {
class JSTextEditableController final : public Referenced {
public:
    JSTextEditableController() = default;
    ~JSTextEditableController() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSTextEditableController* scroller);
    void CaretPosition(int32_t caretPosition);
    void SetTextSelection(const JSCallbackInfo& info);
    void ShowPassword(const JSCallbackInfo& info);
    void HidePassword(const JSCallbackInfo& info);
    void StopEditing();
    void GetTextContentRect(const JSCallbackInfo& info);
    void GetTextContentLinesNum(const JSCallbackInfo& info);
    void GetCaretOffset(const JSCallbackInfo& info);

    void SetController(const RefPtr<TextFieldControllerBase>& controller)
    {
        controllerWeak_ = controller;
    }

private:
    WeakPtr<TextFieldControllerBase> controllerWeak_;
    JSRef<JSObject> CreateRectangle(const Rect& info);
    ACE_DISALLOW_COPY_AND_MOVE(JSTextEditableController);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXT_EDITABLE_CONTROLLER_H

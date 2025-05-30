/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_SCROLL_BAR_JS_SCROLL_BAR_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_SCROLL_BAR_JS_SCROLL_BAR_H

#include <variant>

#include "frameworks/bridge/declarative_frontend/engine/bindings_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "core/components_ng/pattern/scroll_bar/scroll_bar_pattern.h"

namespace OHOS::Ace::Framework {
class JSScrollBar : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void JsSetEnableNestedScroll(const JSCallbackInfo& args);
    static void JsSetScrollBarColor(const JSCallbackInfo& args);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_SCROLL_BAR_JS_SCROLL_BAR_H

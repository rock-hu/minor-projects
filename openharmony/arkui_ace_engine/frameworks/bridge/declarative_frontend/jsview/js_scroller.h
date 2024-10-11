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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SCROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SCROLLER_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings.h"
#include "core/components/scroll/scroll_controller_base.h"
#include "core/components/scroll_bar/scroll_proxy.h"

namespace OHOS::Ace::Framework {

class JSScroller : public Referenced {
public:
    JSScroller() = default;
    ~JSScroller() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSScroller* scroller);

    void ScrollTo(const JSCallbackInfo& args);
    void ScrollEdge(const JSCallbackInfo& args);
    void Fling(const JSCallbackInfo& args);
    void ScrollPage(const JSCallbackInfo& args);
    void CurrentOffset(const JSCallbackInfo& args);
    void ScrollToIndex(const JSCallbackInfo& args);
    void ScrollBy(const JSCallbackInfo& args);
    void IsAtEnd(const JSCallbackInfo& args);
    void GetItemRect(const JSCallbackInfo& args);
    void GetItemIndex(const JSCallbackInfo& args);

    const WeakPtr<ScrollControllerBase>& GetController() const
    {
        return controllerWeak_;
    }

    void SetController(const RefPtr<ScrollControllerBase>& controller)
    {
        if (controller) {
            controller->SetObserver(observer_);
        }
        controllerWeak_ = controller;
    }

    void SetScrollBarProxy(const RefPtr<ScrollProxy>& proxy)
    {
        scrollBarProxyWeak_ = proxy;
    }

    RefPtr<ScrollProxy> GetScrollBarProxy()
    {
        return scrollBarProxyWeak_.Upgrade();
    }

    JSRef<JSObject> CreateRectangle(const Rect& info);

    void SetInstanceId(int32_t instanceId)
    {
        instanceId_ = instanceId;
    }

    int32_t GetInstanceId()
    {
        return instanceId_;
    }

    void SetObserver(const ScrollerObserver& observer)
    {
        observer_ = observer;
        auto controller = controllerWeak_.Upgrade();
        if (controller) {
            controller->SetObserver(observer);
        }
    }

private:
    bool ParseCurveParams(RefPtr<Curve>& curve, const JSRef<JSVal>& jsValue);

    WeakPtr<ScrollControllerBase> controllerWeak_;
    WeakPtr<ScrollProxy> scrollBarProxyWeak_;

    ACE_DISALLOW_COPY_AND_MOVE(JSScroller);

    int32_t instanceId_ = INSTANCE_ID_UNDEFINED;

    ScrollerObserver observer_;
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SCROLLER_H

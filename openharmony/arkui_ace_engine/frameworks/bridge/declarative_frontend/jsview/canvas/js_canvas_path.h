/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CANVAS_PATH_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CANVAS_PATH_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings_defines.h"

namespace OHOS::Ace::Framework {

class JSCanvasPath : public Referenced {
public:
    JSCanvasPath();
    ~JSCanvasPath() override = default;
    static void DestructorInterceptor(void* env, void* controller, void* data) {}
    void JsPath2DSetTransform(const JSCallbackInfo& args);
    void JsPath2DMoveTo(const JSCallbackInfo& args);
    void JsPath2DLineTo(const JSCallbackInfo& args);
    void JsPath2DArc(const JSCallbackInfo& args);
    void JsPath2DArcTo(const JSCallbackInfo& args);
    void JsPath2DQuadraticCurveTo(const JSCallbackInfo& args);
    void JsPath2DBezierCurveTo(const JSCallbackInfo& args);
    void JsPath2DEllipse(const JSCallbackInfo& args);
    void JsPath2DRect(const JSCallbackInfo& args);
    void JsPath2DRoundRect(const JSCallbackInfo& args);
    void JsPath2DClosePath(const JSCallbackInfo& args);
    RefPtr<CanvasPath2D> GetCanvasPath2d() const
    {
        return path2d_;
    }
    void SetCanvasPath2d(RefPtr<CanvasPath2D> path2d)
    {
        path2d_ = path2d;
    }

    void SetUnit(CanvasUnit unit)
    {
        unit_ = unit;
    }

    CanvasUnit GetUnit()
    {
        return unit_;
    }

    double GetDensity()
    {
        double density = PipelineBase::GetCurrentDensity();
        return ((GetUnit() == CanvasUnit::DEFAULT) && !NearZero(density)) ? density : 1.0;
    }

    ACE_DISALLOW_COPY_AND_MOVE(JSCanvasPath);
protected:
    void SetPathSize(const JSCallbackInfo& info);
    size_t lastPathSize_ = 0;
    panda::CopyableGlobal<panda::JSValueRef> pathCmdObj_;
    RefPtr<CanvasPath2D> path2d_;
    CanvasUnit unit_ = CanvasUnit::DEFAULT;
    bool isJudgeSpecialValue_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CANVAS_PATH_H
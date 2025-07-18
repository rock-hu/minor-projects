/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GAUGE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GAUGE_H

#include "core/components_ng/pattern/gauge/gauge_model_ng.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {
class JSGauge : public JSViewAbstract {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);

    static void SetValue(const JSCallbackInfo& info);
    static void SetStartAngle(const JSCallbackInfo& info);
    static void SetEndAngle(const JSCallbackInfo& info);
    static void SetColors(const JSCallbackInfo& info);
    static void SetStrokeWidth(const JSCallbackInfo& info);
    static void SetLabelConfig(const JSCallbackInfo& info);
    static void SetShadowOptions(const JSCallbackInfo& info);
    static void SetDescription(const JSCallbackInfo& info);
    static void SetIndicator(const JSCallbackInfo& info);

private:
    static void SetGradientColors(const JSCallbackInfo& info);
    static void ConvertGradientColor(const JsiRef<JsiValue>& itemParam,
        std::vector<OHOS::Ace::NG::ColorStopArray>& colors, OHOS::Ace::NG::GaugeType& type, bool& useJsLinearGradient,
        bool& useSpecialDefaultIndicator);
    static void ConvertResourceColor(const JsiRef<JsiValue>& itemParam,
        std::vector<OHOS::Ace::NG::ColorStopArray>& colors, bool& useSpecialDefaultIndicator);
    static void SortColorStopOffset(std::vector<NG::ColorStopArray>& colors);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GAUGE_H
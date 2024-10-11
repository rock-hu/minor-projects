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

#include "bridge/cj_frontend/cppview/canvas_gradient.h"

namespace OHOS::Ace::Framework {
NativeCanvasGradient::NativeCanvasGradient(const Gradient& gradient) : FFIData()
{
    gradient_ = gradient;
}

NativeCanvasGradient::~NativeCanvasGradient()
{
    LOGI("Native CanvasGradient Destroyed: %{public}" PRId64, GetID());
}

void NativeCanvasGradient::AddColorStop(double offsetValue, const Color& colorValue)
{
    GradientColor color;
    color.SetColor(colorValue);
    color.SetDimension(offsetValue);
    gradient_.AddColor(color);
}
} // namespace OHOS::Ace::Framework

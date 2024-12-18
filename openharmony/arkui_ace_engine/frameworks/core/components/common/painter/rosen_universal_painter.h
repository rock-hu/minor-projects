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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_ROSEN_UNIVERSAL_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_ROSEN_UNIVERSAL_PAINTER_H

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RosenUniversalPainter {
public:
    RosenUniversalPainter() = default;
    ~RosenUniversalPainter() = default;

    static void DrawHoverBackground(
        RSCanvas* canvas, const Rect& paintRect, uint32_t hoverBackgroundColor, double borderRadius);

    static void DrawRRectBackground(
        RSCanvas* canvas, const RRect& paintRRect, uint32_t backgroundColor, double dipScale);

    static double NormalizeToPx(const Dimension& dimension, double scale);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_ROSEN_UNIVERSAL_PAINTER_H

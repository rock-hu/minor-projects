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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_ROSEN_RENDER_BUTTON_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_ROSEN_RENDER_BUTTON_H

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#include "include/core/SkPoint.h"
#else
#include "core/components_ng/render/drawing.h"
#endif

#include "core/components/button/render_button.h"

namespace OHOS::Ace {

class RosenRenderButton : public RenderButton {
    DECLARE_ACE_TYPE(RosenRenderButton, RenderButton);

public:
    RosenRenderButton() = default;
    ~RosenRenderButton() override = default;

    bool HasEffectiveTransform() const override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void PerformLayout() override;

private:
    Size Measure() override;
    void SyncFocusGeometryProperties();
    void UpdateLayer();
    void PaintButtonAnimation();
    void MeasureButtonSize();
    void MeasureCircle();
    void MeasureCapsule();
    void ResetBoxRadius();
#ifndef USE_ROSEN_DRAWING
    void PaintLayer(SkCanvas* canvas);
    void DrawShape(SkCanvas* canvas, const Offset& offset, bool isStroke = false);
    void DrawArc(SkCanvas* canvas, const Offset& offset);
    void DrawLineProgress(SkCanvas* canvas, const Offset& offset);
    void DrawLineProgressAnimation(SkCanvas* canvas, const Offset& offset);
    void DrawCircleProgress(SkCanvas* canvas, const Offset& offset);
    void DrawDownloadButton(SkCanvas* canvas, const Offset& offset);
    void DrawButton(SkCanvas* canvas, const Offset& offset);
    void ConvertToSkVector(const std::array<Radius, 4>& radii, SkVector* skRadii);
#else
    void PaintLayer(RSCanvas* canvas);
    void DrawShape(RSCanvas* canvas, const Offset& offset, bool isStroke = false);
    void DrawArc(RSCanvas* canvas, const Offset& offset);
    void DrawLineProgress(RSCanvas* canvas, const Offset& offset);
    void DrawLineProgressAnimation(RSCanvas* canvas, const Offset& offset);
    void DrawCircleProgress(RSCanvas* canvas, const Offset& offset);
    void DrawDownloadButton(RSCanvas* canvas, const Offset& offset);
    void DrawButton(RSCanvas* canvas, const Offset& offset);
    void ConvertToVector(const std::array<Radius, 4>& radii, std::vector<RSPoint>& pRadii);
#endif
    void PaintFocus(RenderContext& context, const Offset& offset);
    void PaintPopupFocus(RenderContext& context);
    uint32_t GetStateColor();
    bool NeedClickedColor(const Color& backgroundColor);

#ifndef USE_ROSEN_DRAWING
    SkVector radii_[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
#else
    std::vector<RSPoint> radii_ = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
#endif
    Matrix4 transformLayer_ = Matrix4::CreateIdentity();
    float opacityLayer_ = 1.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_ROSEN_RENDER_BUTTON_H

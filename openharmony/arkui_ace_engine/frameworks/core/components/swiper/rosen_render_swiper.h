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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_ROSEN_RENDER_SWIPER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_ROSEN_RENDER_SWIPER_H

#include "core/components/swiper/render_swiper.h"
#include "core/components_ng/render/drawing.h"

namespace OHOS::Ace {
class RosenRenderSwiper : public RenderSwiper {
    DECLARE_ACE_TYPE(RosenRenderSwiper, RenderSwiper);

public:
    void Update(const RefPtr<Component>& component) override;
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    void UpdateIndicator() override;
    void PaintIndicator(RenderContext& context, const Offset& offset);
    void PaintMask(RenderContext& context, const Offset& offset) const;
    void LayoutDigitalIndicator();
    void CanvasDrawIndicator(RenderContext& context, const Offset& offset);
    void PaintFade(RenderContext& context, const Offset& offset);
    void PaintShadow(RSCanvas* canvas, const Offset& offset);
    IndicatorProperties PrepareIndicatorProperties() const;

    void DrawIndicator(RenderContext& context, const Offset& offset);
    void DrawIndicatorHoverBackground(RenderContext& context, const Offset& offset);
    void DrawIndicatorBackground(RenderContext& context, const Offset& offset);
    void DrawIndicatorItems(RenderContext& context, const Offset& offset);
    void DrawIndicatorFocus(RenderContext& context, const Offset& offset);

    void GetRRect(RSRoundRect& rRect, double& startOffset, double& endOffset, const Offset& offset);
    bool HideIndicatorPoint(int32_t index, const IndicatorOffsetInfo& pointInfo, const Offset& offset);
    void GetIndicatorPointMoveOffset(int32_t index, Offset& animationMove);
    void InitMoveRange();

    RefPtr<RenderText> renderDigitalIndicator_;

    int32_t moveStartIndex_ = 0;
    int32_t moveEndIndex_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_ROSEN_RENDER_SWIPER_H

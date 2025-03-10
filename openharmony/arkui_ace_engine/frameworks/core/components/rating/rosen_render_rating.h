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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_ROSEN_RENDER_RATING_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_ROSEN_RENDER_RATING_H

#include "core/components/rating/render_rating.h"

namespace OHOS::Ace {

class RosenRenderRating final : public RenderRating {
    DECLARE_ACE_TYPE(RosenRenderRating, RenderRating);

public:
    RosenRenderRating() = default;
    ~RosenRenderRating() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void PaintFocus(const Offset& offset, double rrectRadius, const Size& boardSize, RenderContext& context) override;
    void PaintFocusForTABLET(const Offset& offset, double rrectRadius, const Size& boardSize, RenderContext& context);
    void PaintPress(const Offset& offset, double rrectRadius, const Size& boardSize, RenderContext& context);
    void PaintRatingBar(RenderContext& context, RSCanvas* canvas);
    void PaintImageArea(RenderContext& context, const RefPtr<RenderImage>& renderImage, RSCanvas* canvas,
        const ImageAreaProperties& properties);
    void PaintHoverRect(RSCanvas* canvas);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_ROSEN_RENDER_RATING_H

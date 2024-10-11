/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/web/web_paint_method.h"
#include "core/components_ng/render/adapter/rosen_render_surface.h"
namespace OHOS::Ace::NG {
CanvasDrawFunction WebPaintMethod::GetForegroundDrawFunction(PaintWrapper* paintWrapper)
{
    const auto& geometryNode = paintWrapper->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, nullptr);
    auto frameRect = geometryNode->GetFrameRect();
    int32_t componentWidth = frameRect.Width();
    int32_t componentHeight = frameRect.Height();
    TAG_LOGD(AceLogTag::ACE_WEB, "web pattern geometryNode width: %{public}d, height: %{public}d.",
             componentWidth, componentHeight);
    return [weak = WeakClaim(this), width = componentWidth, height = componentHeight](RSCanvas& canvas) {
        auto painter = weak.Upgrade();
        CHECK_NULL_VOID(painter);
        auto surface = DynamicCast<NG::RosenRenderSurface>(painter->renderSuface_);
        if (surface) {
            ACE_SCOPED_TRACE("WebPaintMethod::GetForegroundDrawFunction Web DrawBuffer (width %d, height %d)",
                width, height);
            surface->DrawBuffer(width, height);
        }
    };
}
} // namespace OHOS::Ace::NG
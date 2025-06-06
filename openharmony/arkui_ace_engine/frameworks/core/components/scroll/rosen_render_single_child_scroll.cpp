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

#include "core/components/scroll/rosen_render_single_child_scroll.h"

#include "render_service_client/core/ui/rs_node.h"

#include "core/components/common/painter/rosen_scroll_bar_painter.h"

namespace OHOS::Ace {

void RosenRenderSingleChildScroll::Paint(RenderContext& context, const Offset& offset)
{
    RenderNode::Paint(context, offset);

    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto canvas = renderContext->GetCanvas();
    auto rsNode = renderContext->GetRSNode();
    if (!canvas || !rsNode) {
        LOGE("canvas is null");
        return;
    }
    rsNode->SetPaintOrder(true);
    // Todo: viewPort_ is not adapt for RS.
    rsNode->SetClipToBounds(true);
    if (!viewPort_.IsInfinite()) {
        auto offset = GetPaintOffset();
        rsNode->SetFrame(offset.GetX(), offset.GetY(), viewPort_.Width(), viewPort_.Height());
    }

    // paint custom effect
    if (scrollEffect_) {
        scrollEffect_->Paint(context, viewPort_, offset);
    }

    // paint scrollBar
    if (scrollBar_ && scrollBar_->NeedPaint()) {
        bool needPaint = false;
        if (scrollBar_->IsActive() || scrollBar_->GetDisplayMode() == DisplayMode::ON) {
            scrollBarOpacity_ = UINT8_MAX;
            needPaint = true;
        } else {
            if (scrollBarOpacity_ != 0) {
                needPaint = true;
            }
        }
        if (needPaint) {
            scrollBar_->UpdateScrollBarRegion(offset, GetLayoutSize(), lastOffset_, GetEstimatedHeight());
            RefPtr<RosenScrollBarPainter> scrollBarPainter = AceType::MakeRefPtr<RosenScrollBarPainter>();
            scrollBarPainter->PaintBar(
                canvas, offset, GetPaintRect(), scrollBar_, GetGlobalOffset(), scrollBarOpacity_);
        }
    }

    // Notify scroll bar to update.
    if (scrollBarProxy_) {
        scrollBarProxy_->NotifyScrollBar(AceType::WeakClaim(this));
    }
}

} // namespace OHOS::Ace
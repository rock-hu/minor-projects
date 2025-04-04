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

#include "core/components/scroll/rosen_render_multi_child_scroll.h"

#include "render_service_client/core/ui/rs_node.h"

#include "core/components/common/painter/rosen_scroll_bar_painter.h"

namespace OHOS::Ace {

void RosenRenderMultiChildScroll::Update(const RefPtr<Component>& component)
{
    RenderMultiChildScroll::Update(component);
    auto rsNode = GetRSNode();
    if (rsNode != nullptr) {
        rsNode->SetClipToFrame(true);
    }
}

void RosenRenderMultiChildScroll::Paint(RenderContext& context, const Offset& offset)
{
    // child position changed when layout, need layout again
    if (!NearEqual(GetMainOffset(currentOffset_), offsetBeforeLayout_)) {
        MarkNeedLayout();
        return;
    }
    RenderNode::Paint(context, offset);

    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto canvas = renderContext->GetCanvas();
    auto rsNode = renderContext->GetRSNode();
    if (!canvas || !rsNode) {
        LOGE("Paint canvas is null");
        return;
    }
    rsNode->SetPaintOrder(true);

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
}

} // namespace OHOS::Ace
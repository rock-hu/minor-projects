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

#include "core/pipeline/base/rosen_render_context.h"

#include "core/pipeline/base/render_sub_container.h"
#include "render_service_client/core/ui/rs_canvas_node.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t OVERFLOW_PLATFORM_VERSION = 7;

inline bool ShouldPaint(const RefPtr<RenderNode>& node)
{
    return node != nullptr && node->GetVisible() && !node->GetHidden();
}

} // namespace

RosenRenderContext::~RosenRenderContext()
{
    StopRecordingIfNeeded();
}

void RosenRenderContext::Repaint(const RefPtr<RenderNode>& node)
{
    if (!ShouldPaint(node) || !node->NeedRender() || node->GetRSNode() == nullptr) {
        return;
    }

    auto rsNode = node->GetRSNode();
    auto offset =
        node->GetTransitionPaintRect().GetOffset() -
        Offset(rsNode->GetStagingProperties().GetFrame().x_, rsNode->GetStagingProperties().GetFrame().y_);

    std::string name = AceType::TypeName(node);
    if (name != "RosenRenderForm" && name != "RosenRenderPlugin") {
        InitContext(rsNode, node->GetRectWithShadow(), offset);
        node->RenderWithContext(*this, offset);
        UpdateChildren(rsNode);
    } else {
        node->RenderWithContext(*this, offset);
    }
    StopRecordingIfNeeded();
}

void RosenRenderContext::PaintChild(const RefPtr<RenderNode>& child, const Offset& offset)
{
    if (!ShouldPaint(child)) {
        return;
    }
    auto pipelineContext = child->GetContext().Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext is null.");
        return;
    }
    bool canChildOverflow = pipelineContext->GetMinPlatformVersion() >= OVERFLOW_PLATFORM_VERSION;
    Rect rect = child->GetTransitionPaintRect() + offset;
    if (!(child->IsPaintOutOfParent() || canChildOverflow) && !estimatedRect_.IsIntersectWith(rect)) {
#if defined(PREVIEW)
        child->ClearAccessibilityRect();
#endif
        return;
    }

    auto childRSNode = child->GetRSNode();
    if (childRSNode && childRSNode != rsNode_) {
        childNodes_.emplace_back(childRSNode);
        std::string name = AceType::TypeName(child);
        if (name != "RosenRenderForm" && name != "RosenRenderPlugin") {
            if (child->NeedRender()) {
                RosenRenderContext context;
                auto transparentHole = pipelineContext->GetTransparentHole();
                if (transparentHole.IsValid() && child->GetNeedClip()) {
                    Offset childOffset = rect.GetOffset();
                    Rect hole = transparentHole - childOffset;
                    context.SetClipHole(hole);
                }
                context.Repaint(child);
            } else {
                // No need to repaint, notify to update AccessibilityNode info.
                child->NotifyPaintFinish();
            }
        }
        Offset pos = rect.GetOffset();
        if (name == "RosenRenderPlugin") {
            auto renderPlugin = AceType::DynamicCast<RenderSubContainer>(child);
            if (!renderPlugin) {
                return;
            }
            auto pluginContext = renderPlugin->GetSubPipelineContext();
            if (!pluginContext) {
                return;
            }
            auto density = pipelineContext->GetDensity();
            Offset pluginOffset = {pos.GetX() / density, pos.GetY() / density};
            pluginContext->SetPluginEventOffset(child->GetGlobalOffset());
        }
    } else {
        child->RenderWithContext(*this, rect.GetOffset());
    }
}

void RosenRenderContext::StartRecording()
{
    recordingCanvas_ = new Rosen::Drawing::RecordingCanvas(estimatedRect_.Width(), estimatedRect_.Height());
    if (clipHole_.IsValid()) {
        recordingCanvas_->Save();
        needRestoreHole_ = true;
        recordingCanvas_->ClipRect(
            RSRect(clipHole_.Left(), clipHole_.Top(), clipHole_.Right(), clipHole_.Bottom()),
            RSClipOp::DIFFERENCE, true);
    }
}

void RosenRenderContext::StopRecordingIfNeeded()
{
    auto rsCanvasNode = RSNode::ReinterpretCast<Rosen::RSCanvasNode>(rsNode_);
    if (rosenCanvas_ && rsCanvasNode) {
        rsCanvasNode->FinishRecording();
        rosenCanvas_ = nullptr;
    }

    if (needRestoreHole_) {
        recordingCanvas_->Restore();
        needRestoreHole_ = false;
    }

    if (IsRecording()) {
        delete recordingCanvas_;
        recordingCanvas_ = nullptr;
    }
}

bool RosenRenderContext::IsIntersectWith(const RefPtr<RenderNode>& child, Offset& offset)
{
    if (!ShouldPaint(child)) {
        return false;
    }

    Rect rect = child->GetTransitionPaintRect() + offset;
    if (!estimatedRect_.IsIntersectWith(rect)) {
#if defined(PREVIEW)
        child->ClearAccessibilityRect();
#endif
        return false;
    }

    offset = rect.GetOffset();
    return true;
}

void RosenRenderContext::InitContext(
    const std::shared_ptr<RSNode>& rsNode, const Rect& rect, const Offset& initialOffset)
{
    rsNode_ = rsNode;
    estimatedRect_ = rect + initialOffset;
    if (rsNode_ == nullptr) {
        return;
    }
    childNodes_.clear();
    if (auto rsCanvasNode = rsNode_->ReinterpretCastTo<Rosen::RSCanvasNode>()) {
        rosenCanvas_ = rsCanvasNode->BeginRecording(rsCanvasNode->GetPaintWidth(), rsCanvasNode->GetPaintHeight());
    }
}

RSCanvas* RosenRenderContext::GetCanvas()
{
    // if recording, return recording canvas
    return recordingCanvas_ ? recordingCanvas_ : rosenCanvas_;
}

const std::shared_ptr<RSNode>& RosenRenderContext::GetRSNode()
{
    return rsNode_;
}

std::shared_ptr<RSPicture> RosenRenderContext::FinishRecordingAsPicture()
{
    LOGE("Drawing is not supported");
    return nullptr;
}

std::shared_ptr<RSImage> RosenRenderContext::FinishRecordingAsImage()
{
    LOGE("Drawing is not supported");
    return nullptr;
}

void RosenRenderContext::Restore()
{
    auto canvas = GetCanvas();
    if (canvas != nullptr) {
        canvas->Restore();
    }
}

void RosenRenderContext::UpdateChildren(const std::shared_ptr<RSNode>& rsNode)
{
    std::vector<OHOS::Rosen::NodeId> childNodeIds;
    for (auto& child : childNodes_) {
        if (auto childNode = child.lock()) {
            childNodeIds.emplace_back(childNode->GetId());
        }
    }
    if (childNodeIds != rsNode->GetChildren()) {
        rsNode->ClearChildren();
        for (auto& child : childNodes_) {
            rsNode->AddChild(child.lock());
        }
    }
}
} // namespace OHOS::Ace

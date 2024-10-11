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

#include "core/components_ng/pattern/window_scene/scene/window_node.h"

#include "core/components_ng/pattern/window_scene/scene/window_pattern.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
constexpr float MOUSE_RECT_HOT_VP = 4.0f;
constexpr float TOUCH_RECT_HOT_VP = 20.0f;
constexpr double DEFAULT_HOT_DENSITY = 1.5f;
std::map<int32_t, WeakPtr<WindowNode>> g_windowNodeMap;
}

WindowNode::WindowNode(const std::string& tag,
    int32_t nodeId, int32_t sessionId, const RefPtr<Pattern>& pattern, bool isRoot)
    : FrameNode(tag, nodeId, pattern, isRoot)
{
    sessionId_ = sessionId;
}

WindowNode::~WindowNode()
{
    g_windowNodeMap.erase(sessionId_);
}

RefPtr<WindowNode> WindowNode::GetOrCreateWindowNode(const std::string& tag,
    int32_t nodeId, int32_t sessionId, const std::function<RefPtr<Pattern>(void)>& patternCreator)
{
    auto windowNode = ElementRegister::GetInstance()->GetSpecificItemById<WindowNode>(nodeId);
    if (windowNode) {
        if (windowNode->GetTag() == tag) {
            return windowNode;
        }
        bool removed = ElementRegister::GetInstance()->RemoveItemSilently(nodeId);
        if (!removed) {
            TAG_LOGW(AceLogTag::ACE_WINDOW_SCENE,
                "Remove item silently failed, node id: %{public}d, sessionId: %{public}d", nodeId, sessionId);
        }
        auto parent = windowNode->GetParent();
        if (parent) {
            parent->RemoveChild(windowNode);
        }
    }

    auto iter = g_windowNodeMap.find(sessionId);
    if (iter != g_windowNodeMap.end()) {
        auto node = iter->second.Upgrade();
        if (node) {
            return node;
        }
    }

    auto pattern = patternCreator ? patternCreator() : AceType::MakeRefPtr<Pattern>();
    windowNode = AceType::MakeRefPtr<WindowNode>(tag, nodeId, sessionId, pattern, false);
    windowNode->InitializePatternAndContext();
    bool added = ElementRegister::GetInstance()->AddUINode(windowNode);
    if (!added) {
        TAG_LOGW(AceLogTag::ACE_WINDOW_SCENE, "Add UINode failed, node id: %{public}d, sessionId: %{public}d",
            nodeId, sessionId);
    }
    g_windowNodeMap.emplace(sessionId, WeakPtr<WindowNode>(windowNode));
    return windowNode;
}

void WindowNode::SetParent(const WeakPtr<UINode>& parent)
{
    if (GetParent()) {
        RemoveFromParentCleanly(Claim(this), GetParent());
    }
    UINode::SetParent(parent);
}

bool WindowNode::IsOutOfTouchTestRegion(const PointF& parentLocalPoint, const TouchEvent& touchEvent)
{
    auto pattern = GetPattern<WindowPattern>();
    if (pattern != nullptr) {
        auto hotAreas = pattern->GetHotAreas();
        if (!hotAreas.empty()) {
            auto hotRects = ConvertHotRects(hotAreas);
            for (auto& hotRect : hotRects) {
                if (hotRect.IsInRegion(parentLocalPoint)) {
                    return false;
                }
            }
            return true;
        }
    }
    const auto& rect = GetPaintRectWithTransform();
    const auto& hotRect = ConvertHotRect(rect, static_cast<int32_t>(touchEvent.sourceType));
    if (!hotRect.IsInRegion(parentLocalPoint)) {
        return true;
    }
    return false;
}

std::vector<RectF> WindowNode::GetResponseRegionList(const RectF& rect, int32_t sourceType)
{
    auto pattern = GetPattern<WindowPattern>();
    if (pattern != nullptr) {
        auto hotAreas = pattern->GetHotAreas();
        if (!hotAreas.empty()) {
            return ConvertHotRects(hotAreas);
        }
    }
    std::vector<RectF> responseRegionList;
    responseRegionList.emplace_back(ConvertHotRect(rect, sourceType));
    return responseRegionList;
}

std::vector<RectF> WindowNode::ConvertHotRects(const std::vector<Rosen::Rect>& hotAreas)
{
    std::vector<RectF> responseRegionList;
    for (size_t i = 0; i < hotAreas.size(); i++) {
        float hotX = static_cast<float>(hotAreas[i].posX_);
        float hotY = static_cast<float>(hotAreas[i].posY_);
        float hotWidth = static_cast<float>(hotAreas[i].width_);
        float hotHeight = static_cast<float>(hotAreas[i].height_);
        RectF rectHot(hotX, hotY, hotWidth, hotHeight);
        responseRegionList.emplace_back(rectHot);
    }
    return responseRegionList;
}

RectF WindowNode::ConvertHotRect(const RectF& rect, int32_t sourceType)
{
    auto pattern = GetPattern<WindowPattern>();
    if (pattern != nullptr && pattern->GetSession() != nullptr &&
        (pattern->GetSession()->GetWindowType() != Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
        pattern->GetSession()->GetWindowType() != Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW)) {
        return rect;
    }
    float hotOffsetVp = (sourceType == static_cast<int32_t>(Ace::SourceType::MOUSE)) ?
        MOUSE_RECT_HOT_VP : TOUCH_RECT_HOT_VP;
    auto context = PipelineContext::GetCurrentContext();
    double density = (context != nullptr) ? context->GetDensity() : DEFAULT_HOT_DENSITY;
    float hotOffset = static_cast<float>(hotOffsetVp * density);
    float hotX = rect.GetX() - hotOffset;
    float hotY = rect.GetY() - hotOffset;
    float hotWidth = rect.Width() + hotOffset * 2;
    float hotHeight = rect.Height() + hotOffset * 2;
    RectF rectHot(hotX, hotY, hotWidth, hotHeight);
    return rectHot;
}
} // namespace OHOS::Ace::NG

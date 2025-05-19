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

#include "core/accessibility/accessibility_manager_ng.h"

#include "core/accessibility/accessibility_constants.h"
#include "core/accessibility/accessibility_session_adapter.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {

void AddTouchEventAllFingersInfo(const RefPtr<NG::FrameNode>& node, TouchEventInfo& eventInfo, const TouchEvent& event)
{
    // all fingers collection
    for (const auto& item : event.pointers) {
        float globalX = item.x;
        float globalY = item.y;
        float screenX = item.screenX;
        float screenY = item.screenY;
        PointF localPoint(globalX, globalY);
        NGGestureRecognizer::Transform(localPoint, node, false, false);
        auto localX = static_cast<float>(localPoint.GetX());
        auto localY = static_cast<float>(localPoint.GetY());
        TouchLocationInfo info("onTouch", item.originalId);
        info.SetGlobalLocation(Offset(globalX, globalY));
        info.SetLocalLocation(Offset(localX, localY));
        info.SetScreenLocation(Offset(screenX, screenY));
        info.SetTouchType(event.type);
        info.SetForce(item.force);
        info.SetPressedTime(item.downTime);
        info.SetWidth(item.width);
        info.SetHeight(item.height);
        if (item.tiltX.has_value()) {
            info.SetTiltX(item.tiltX.value());
        }
        if (item.tiltY.has_value()) {
            info.SetTiltY(item.tiltY.value());
        }
        info.SetSourceTool(item.sourceTool);
        eventInfo.AddTouchLocationInfo(std::move(info));
    }
}

void ConvertTouchEvent2TouchEventInfo(const RefPtr<NG::FrameNode>& node, const TouchEvent& event,
    TouchEventInfo& eventInfo)
{
    eventInfo.SetTimeStamp(event.time);
    eventInfo.SetDeviceId(event.deviceId);
    eventInfo.SetPointerEvent(event.GetTouchEventPointerEvent());
    TouchLocationInfo changedInfo("onTouch", event.originalId);
    PointF lastLocalPoint(event.x, event.y);
    NGGestureRecognizer::Transform(lastLocalPoint, node, false, false);
    auto localX = static_cast<float>(lastLocalPoint.GetX());
    auto localY = static_cast<float>(lastLocalPoint.GetY());
    changedInfo.SetLocalLocation(Offset(localX, localY));
    changedInfo.SetGlobalLocation(Offset(event.x, event.y));
    changedInfo.SetScreenLocation(Offset(event.screenX, event.screenY));
    changedInfo.SetTouchType(event.type);
    changedInfo.SetForce(event.force);
    changedInfo.SetPressedTime(event.pressedTime);
    changedInfo.SetWidth(event.width);
    changedInfo.SetHeight(event.height);
    if (event.tiltX.has_value()) {
        changedInfo.SetTiltX(event.tiltX.value());
    }
    if (event.tiltY.has_value()) {
        changedInfo.SetTiltY(event.tiltY.value());
    }
    changedInfo.SetSourceTool(event.sourceTool);
    eventInfo.AddChangedTouchLocationInfo(std::move(changedInfo));

    AddTouchEventAllFingersInfo(node, eventInfo, event);
    eventInfo.SetSourceDevice(event.sourceType);
    eventInfo.SetForce(event.force);
    if (event.tiltX.has_value()) {
        eventInfo.SetTiltX(event.tiltX.value());
    }
    if (event.tiltY.has_value()) {
        eventInfo.SetTiltY(event.tiltY.value());
    }
    if (event.rollAngle.has_value()) {
        eventInfo.SetRollAngle(event.rollAngle.value());
    }
    eventInfo.SetSourceTool(event.sourceTool);
    EventTarget eventTarget;
    eventTarget.id = node->GetInspectorId().value_or("").c_str();
    eventInfo.SetTarget(eventTarget);
}

bool CheckAndSendHoverEnterByAncestor(const RefPtr<NG::FrameNode>& ancestor)
{
    CHECK_NULL_RETURN(ancestor, false);
    auto pipeline = ancestor->GetContext();
    CHECK_NULL_RETURN(pipeline, false);
    // Inter Process is showed as a component with rect like form process,
    // need send hover enter when no component hovered to focus outside
    if (pipeline->IsFormRender() || pipeline->IsJsCard() || pipeline->IsJsPlugin()) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "SendHoverEnterByAncestor");
        ancestor->OnAccessibilityEvent(AccessibilityEventType::HOVER_ENTER_EVENT);
        return true;
    }
    return false;
}
}

bool IsTouchExplorationEnabled(const RefPtr<FrameNode>& root)
{
    CHECK_NULL_RETURN(root, true);
    auto pipeline = root->GetContext();
    CHECK_NULL_RETURN(pipeline, true);
    auto jsAccessibilityManager = pipeline->GetAccessibilityManager();
    CHECK_NULL_RETURN(jsAccessibilityManager, true);
    auto accessibilityWorkMode = jsAccessibilityManager->GenerateAccessibilityWorkMode();
    return accessibilityWorkMode.isTouchExplorationEnabled;
}

void AccessibilityManagerNG::HandleAccessibilityHoverEvent(const RefPtr<FrameNode>& root, const MouseEvent& event)
{
    if (root == nullptr || !AceApplicationInfo::GetInstance().IsAccessibilityEnabled() ||
        !IsTouchExplorationEnabled(root) || event.sourceType != SourceType::MOUSE) {
        return;
    }
    AccessibilityHoverEventType type = AccessibilityHoverEventType::MOVE;
    switch (event.action) {
        case MouseAction::WINDOW_ENTER:
            type = AccessibilityHoverEventType::ENTER;
            break;
        case MouseAction::MOVE:
            type = AccessibilityHoverEventType::MOVE;
            break;
        case MouseAction::WINDOW_LEAVE:
            type = AccessibilityHoverEventType::EXIT;
            break;
        default:
            return;
    }
    PointF point(event.x, event.y);
    HandleHoverEventInnerParam param {SourceType::MOUSE, type, event.time};
    TouchEvent touchEvent;
    HandleAccessibilityHoverEventInner(root, point, param, touchEvent);
}

void AccessibilityManagerNG::HandleAccessibilityHoverEvent(const RefPtr<FrameNode>& root, const TouchEvent& event)
{
    if (root == nullptr ||
        !AceApplicationInfo::GetInstance().IsAccessibilityEnabled() ||
        !IsTouchExplorationEnabled(root) ||
        event.sourceType == SourceType::MOUSE) {
        return;
    }
    if (IsHandlePipelineAccessibilityHoverEnter(root)) {
        return;
    }
    AccessibilityHoverEventType type = AccessibilityHoverEventType::MOVE;
    switch (event.type) {
        case TouchType::HOVER_ENTER:
            type = AccessibilityHoverEventType::ENTER;
            break;
        case TouchType::HOVER_MOVE:
            type = AccessibilityHoverEventType::MOVE;
            break;
        case TouchType::HOVER_EXIT:
            type = AccessibilityHoverEventType::EXIT;
            break;
        default:
            return;
    }
    PointF point(event.x, event.y);
    if (event.pointers.size() > 1 && event.sourceType == SourceType::TOUCH) {
        if (hoverState_.source == SourceType::TOUCH) {
            ResetHoverState();
            return;
        }
    }
    HandleHoverEventInnerParam param {event.sourceType, type, event.time};
    HandleAccessibilityHoverEventInner(root, point, param, event);
}

void AccessibilityManagerNG::HandleAccessibilityHoverEvent(const RefPtr<FrameNode>& root, float pointX, float pointY,
    int32_t sourceType, int32_t eventType, int64_t timeMs)
{
    if (root == nullptr ||
        !AceApplicationInfo::GetInstance().IsAccessibilityEnabled() ||
        !IsTouchExplorationEnabled(root) ||
        eventType < 0 || eventType >= static_cast<int32_t>(AccessibilityHoverEventType::Count)) {
        return;
    }
    PointF point(pointX, pointY);
    TimeStamp time((std::chrono::milliseconds(timeMs)));

    if (IsHandlePipelineAccessibilityHoverEnter(root)) {
        TouchEvent event;
        event.x = pointX;
        event.y = pointY;
        event.sourceType = static_cast<SourceType>(sourceType);
        event.time = time;
        HandlePipelineAccessibilityHoverEnter(root, event, eventType);
    }
    HandleHoverEventInnerParam param {static_cast<SourceType>(sourceType),
        static_cast<AccessibilityHoverEventType>(eventType), time};
    TouchEvent touchEvent;
    HandleAccessibilityHoverEventInner(root, point, param, touchEvent);
}

bool HasTransparentCallback(const RefPtr<NG::FrameNode>& node)
{
    CHECK_NULL_RETURN(node, false);
    auto pipeline = node->GetContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto accessibilityManager = pipeline->GetAccessibilityManager();
    CHECK_NULL_RETURN(accessibilityManager, false);
    return accessibilityManager->IsInHoverTransparentCallbackList(node);
}

bool AccessibilityManagerNG::ExecuteChildNodeHoverTransparentCallback(const RefPtr<FrameNode>& root,
    const TouchEvent& event)
{
    CHECK_NULL_RETURN(root, false);
    auto accessibilityProperty = root->GetAccessibilityProperty<NG::AccessibilityProperty>();
    if (accessibilityProperty != nullptr) {
        auto callback = accessibilityProperty->GetAccessibilityTransparentCallbackFunc();
        if (callback) {
            TouchEventInfo eventInfo("touchEvent");
            ConvertTouchEvent2TouchEventInfo(root, event, eventInfo);
            callback(eventInfo);
        }
    }
    auto children = root->GetFrameChildren();
    for (auto childWeak = children.rbegin(); childWeak != children.rend(); ++childWeak) {
        auto child = childWeak->Upgrade();
        if (child == nullptr) {
            continue;
        }
        ExecuteChildNodeHoverTransparentCallback(child, event);
    }
    return true;
}

bool AccessibilityManagerNG::HandleAccessibilityHoverTransparentCallback(bool transformed,
    const RefPtr<FrameNode>& root,
    const int32_t& currentHoveringId,
    const int32_t& lastHoveringId,
    const TouchEvent& event)
{
    static constexpr int32_t INVALID_NODE_ID = -1;
    if (transformed) {
        return false;
    }
    if ((currentHoveringId == INVALID_NODE_ID) && (lastHoveringId == INVALID_NODE_ID)) {
        return ExecuteChildNodeHoverTransparentCallback(root, event);
    }
    return false;
}

void AccessibilityManagerNG::HandleAccessibilityHoverEventInner(
    const RefPtr<FrameNode>& root,
    const PointF& point,
    HandleHoverEventInnerParam param,
    const TouchEvent& event)
{
    auto sourceType = param.sourceType;
    auto eventType = param.eventType;
    auto time = param.time;
    static constexpr size_t THROTTLE_INTERVAL_HOVER_EVENT = 10;
    uint64_t duration =
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(time - hoverState_.time).count());
    if (!hoverState_.idle) {
        if ((!IsEventTypeChangeDirectHandleHover(eventType)) && (duration < THROTTLE_INTERVAL_HOVER_EVENT)) {
            return;
        }
    }

    static constexpr size_t MIN_SOURCE_CHANGE_GAP_MS = 1000;
    if (sourceType != hoverState_.source && !hoverState_.idle) {
        if (duration < MIN_SOURCE_CHANGE_GAP_MS) {
            return;
        }
        ResetHoverState();
    }

    ACE_SCOPED_TRACE("HandleAccessibilityHoverEventInner");
    if (eventType == AccessibilityHoverEventType::ENTER) {
        ResetHoverState();
    }
    std::vector<WeakPtr<FrameNode>> currentNodesHovering;
    std::vector<RefPtr<FrameNode>> lastNodesHovering;
    std::vector<int32_t> lastNodesHoveringId;
    for (const auto& nodeWeak: hoverState_.nodesHovering) {
        auto node = nodeWeak.Upgrade();
        if (node != nullptr) {
            lastNodesHovering.push_back(node);
            lastNodesHoveringId.push_back(node->GetId());
        }
    }
    if (eventType != AccessibilityHoverEventType::EXIT) {
        std::unique_ptr<AccessibilityProperty::HoverTestDebugTraceInfo> debugInfo = nullptr;
        AccessibilityHoverTestPath path = AccessibilityProperty::HoverTest(point, root, debugInfo);
        for (const auto& node: path) {
            auto id = node->GetId();
            if (std::find(lastNodesHoveringId.begin(), lastNodesHoveringId.end(), id) != lastNodesHoveringId.end() ||
                AccessibilityProperty::IsAccessibilityFocusable(node)) {
                currentNodesHovering.push_back(node);
            }
        }
    }
    auto sendHoverEnter = false;
    static constexpr int32_t INVALID_NODE_ID = -1;
    int32_t lastHoveringId = INVALID_NODE_ID;
    RefPtr<FrameNode> lastHovering = nullptr;
    if (!lastNodesHovering.empty()) {
        lastHovering = lastNodesHovering.back();
        lastHoveringId = lastHovering->GetId();
    }
    int32_t currentHoveringId = INVALID_NODE_ID;
    RefPtr<FrameNode> currentHovering = nullptr;
    if (!currentNodesHovering.empty()) {
        currentHovering = currentNodesHovering.back().Upgrade();
        currentHoveringId = currentHovering->GetId();
    }
    bool transformHover = false;
    if (lastHoveringId != INVALID_NODE_ID && lastHoveringId != currentHoveringId) {
        lastHovering->OnAccessibilityEvent(AccessibilityEventType::HOVER_EXIT_EVENT);
        transformHover = NotifyHoverEventToNodeSession(lastHovering, root, point,
            sourceType, AccessibilityHoverEventType::EXIT, time);
    }
    if (currentHoveringId != INVALID_NODE_ID) {
        if (currentHoveringId != lastHoveringId && (!IgnoreCurrentHoveringNode(currentHovering))) {
            currentHovering->OnAccessibilityEvent(AccessibilityEventType::HOVER_ENTER_EVENT);
            sendHoverEnter = true;
        }
        transformHover = NotifyHoverEventToNodeSession(currentHovering, root, point,
            sourceType, eventType, time);
    }

    if (!sendHoverEnter && (eventType == AccessibilityHoverEventType::ENTER)) {
        // check need send hover enter when no component hovered to focus outside
        transformHover |= CheckAndSendHoverEnterByAncestor(root);
    }

    if (sourceType != SourceType::MOUSE) {
        HandleAccessibilityHoverTransparentCallback(transformHover, root, currentHoveringId, lastHoveringId, event);
    }

    hoverState_.nodesHovering = std::move(currentNodesHovering);
    hoverState_.time = time;
    hoverState_.source = sourceType;
    hoverState_.idle = eventType == AccessibilityHoverEventType::EXIT;
    hoverState_.eventType = eventType;
}

bool AccessibilityManagerNG::IgnoreCurrentHoveringNode(const RefPtr<FrameNode> &node)
{
    auto sessionAdapter = AccessibilitySessionAdapter::GetSessionAdapter(node);
    CHECK_NULL_RETURN(sessionAdapter, false);
    return sessionAdapter->IgnoreHostNode();
}

bool AccessibilityManagerNG::NotifyHoverEventToNodeSession(const RefPtr<FrameNode>& node,
    const RefPtr<FrameNode>& rootNode, const PointF& pointRoot,
    SourceType sourceType, AccessibilityHoverEventType eventType, TimeStamp time)
{
    auto eventHub = node->GetOrCreateEventHub<EventHub>();
    if (!eventHub->IsEnabled()) {
        // If the host component is disabled, do not transfer hover event.
        return false;
    }
    auto sessionAdapter = AccessibilitySessionAdapter::GetSessionAdapter(node);
    CHECK_NULL_RETURN(sessionAdapter, false);
    // mouse event will not be hover and may be transformed by component self through touch event transform
    if ((sourceType == SourceType::MOUSE) && (sessionAdapter->IgnoreTransformMouseEvent())) {
        return false;
    }
    PointF pointNode(pointRoot);
    if (AccessibilityManagerNG::ConvertPointFromAncestorToNode(rootNode, node, pointRoot, pointNode)) {
        sessionAdapter->TransferHoverEvent(pointNode, sourceType, eventType, time);
        return true;
    }
    return false;
}

void AccessibilityManagerNG::ResetHoverState()
{
    hoverState_.idle = true;
    hoverState_.nodesHovering.clear();
}

void AccessibilityManagerNG::HoverTestDebug(const RefPtr<FrameNode>& root, const PointF& point,
    std::string& summary, std::string& detail) const
{
    auto summaryJson = JsonUtil::Create();
    auto detailJson = JsonUtil::Create();
    std::stringstream summaryNodesSearched;
    auto debugInfo = std::make_unique<AccessibilityProperty::HoverTestDebugTraceInfo>();
    AccessibilityHoverTestPath path = AccessibilityProperty::HoverTest(point, root, debugInfo);
    auto summaryPath = JsonUtil::CreateArray();
    auto summarySelected = JsonUtil::CreateArray();

    auto detaiSelectionInfo = JsonUtil::CreateArray();
    size_t numNodesSelected = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        summaryPath->Put(std::to_string(i).c_str(), path[i]->GetAccessibilityId());
        auto detailNodeSelection = JsonUtil::Create();
        if (AccessibilityProperty::IsAccessibilityFocusableDebug(path[i], detailNodeSelection)) {
            summarySelected->Put(std::to_string(numNodesSelected).c_str(), path[i]->GetAccessibilityId());
            ++numNodesSelected;
        }
        detaiSelectionInfo->PutRef(std::move(detailNodeSelection));
    }
    summaryJson->PutRef("path", std::move(summaryPath));
    summaryJson->PutRef("nodesSelected", std::move(summarySelected));

    auto detailSearchInfo = JsonUtil::CreateArray();
    for (size_t i = 0; i < debugInfo->trace.size(); ++i) {
        auto detailNodeSearch = std::move(debugInfo->trace[i]);
        detailSearchInfo->Put(std::to_string(i).c_str(), detailNodeSearch);
    }
    detailJson->PutRef("detailSearch", std::move(detailSearchInfo));
    detailJson->PutRef("detailSelection", std::move(detaiSelectionInfo));
    summary = summaryJson->ToString();
    detail = detailJson->ToString();
}

bool AccessibilityManagerNG::ConvertPointFromAncestorToNode(
    const RefPtr<NG::FrameNode>& ancestor, const RefPtr<NG::FrameNode>& endNode,
    const PointF& pointAncestor, PointF& pointNode)
{
    CHECK_NULL_RETURN(ancestor, false);
    CHECK_NULL_RETURN(endNode, false);
    // revert scale from endNode to ancestor
    std::vector<RefPtr<NG::FrameNode>> path;
    RefPtr<NG::FrameNode> curr = endNode;
    while (curr != nullptr && curr->GetId() != ancestor->GetId()) {
        path.push_back(curr);
        curr = curr->GetAncestorNodeOfFrame(true);
    }
    CHECK_NULL_RETURN(curr, false);
    pointNode = pointAncestor;
    for (auto nodePtr = path.rbegin(); nodePtr != path.rend(); ++nodePtr) {
        auto renderContext = (*nodePtr)->GetRenderContext();
        renderContext->GetPointWithRevert(pointNode);
        auto rect = renderContext->GetPaintRectWithoutTransform();
        pointNode = pointNode - rect.GetOffset();
    }
    return true;
}

bool AccessibilityManagerNG::IsEventTypeChangeDirectHandleHover(AccessibilityHoverEventType eventType)
{
    if ((hoverState_.eventType == AccessibilityHoverEventType::MOVE)
        && (eventType == AccessibilityHoverEventType::EXIT)) {
        return true;
    }
    return false;
}

bool AccessibilityManagerNG::IsHandlePipelineAccessibilityHoverEnter(const RefPtr<NG::FrameNode>& root)
{
    auto pipeline = root->GetContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    CHECK_NULL_RETURN(ngPipeline, false);

    auto container = Container::GetContainer(ngPipeline->GetInstanceId());
    if (container && (container->IsUIExtensionWindow())) {
        return true;
    }
    return false;
}

void AccessibilityManagerNG::HandlePipelineAccessibilityHoverEnter(
    const RefPtr<NG::FrameNode>& root,
    TouchEvent& event,
    int32_t eventType)
{
    CHECK_NULL_VOID(root);
    AccessibilityHoverEventType eventHoverType = static_cast<AccessibilityHoverEventType>(eventType);
    event.type = TouchType::HOVER_MOVE;
    switch (eventHoverType) {
        case AccessibilityHoverEventType::ENTER:
            event.type = TouchType::HOVER_ENTER;
            break;
        case AccessibilityHoverEventType::MOVE:
            event.type = TouchType::HOVER_MOVE;
            break;
        case AccessibilityHoverEventType::EXIT:
            event.type = TouchType::HOVER_EXIT;
            break;
        default:
            break;
    }

    auto pipeline = root->GetContext();
    CHECK_NULL_VOID(pipeline);
    pipeline->OnAccessibilityHoverEvent(event, root);
}
} // namespace OHOS::Ace::NG

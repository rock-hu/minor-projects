/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "js_accessibility_manager.h"

#include "accessibility_system_ability_client.h"
#include "js_third_provider_interaction_operation.h"
#include "perf_monitor_adapter.h"

#include "adapter/ohos/entrance/ace_container.h"
#include "base/log/event_report.h"
#include "core/components_ng/pattern/scrollable/scrollable_utils.h"
#include "core/components_ng/base/frame_node.h"
#include "frameworks/core/accessibility/utils/accessibility_action_function_utils.h"
#include "frameworks/core/components_ng/pattern/ui_extension/platform_container_handler.h"
#include "frameworks/core/components_ng/pattern/overlay/accessibility_focus_paint_node_pattern.h"
#include "frameworks/core/components_ng/pattern/web/transitional_node_info.h"
#include "frameworks/core/components_ng/pattern/web/web_pattern.h"

using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityConfig;

namespace OHOS::Ace::Framework {
namespace {
const char DUMP_ORDER[] = "-accessibility";
const char DUMP_INSPECTOR[] = "-inspector";
const char ACCESSIBILITY_FOCUSED_EVENT[] = "accessibilityfocus";
const char ACCESSIBILITY_CLEAR_FOCUS_EVENT[] = "accessibilityclearfocus";
const char TEXT_CHANGE_EVENT[] = "textchange";
const char PAGE_CHANGE_EVENT[] = "pagechange";
const char SCROLL_END_EVENT[] = "scrollend";
const char SCROLL_START_EVENT[] = "scrollstart";
const char MOUSE_HOVER_ENTER[] = "mousehoverenter";
const char MOUSE_HOVER_EXIT[] = "mousehoverexit";
const char LIST_TAG[] = "List";
const char SIDEBARCONTAINER_TAG[] = "SideBarContainer";
const char STRING_DIR_FORWARD[] = "forward";
const char STRING_DIR_BACKWARD[] = "backward";
constexpr int32_t INVALID_PARENT_ID = -2100000;
constexpr int32_t DEFAULT_PARENT_ID = 2100000;
constexpr int32_t ROOT_STACK_BASE = 1100000;
constexpr int32_t ROOT_DECOR_BASE = 3100000;
constexpr int32_t CARD_NODE_ID_RATION = 10000;
constexpr int32_t CARD_ROOT_NODE_ID_RATION = 1000;
constexpr int32_t CARD_BASE = 100000;
constexpr int32_t DELAY_SEND_EVENT_MILLISECOND = 20;
constexpr uint32_t SUB_TREE_OFFSET_IN_PAGE_ID = 16;
constexpr int32_t MAX_PAGE_ID_WITH_SUB_TREE = (1 << SUB_TREE_OFFSET_IN_PAGE_ID);
constexpr int32_t MIN_NUM = 2;
constexpr size_t MIN_PARAMS_SIZE = 2;
constexpr int64_t INVALID_NODE_ID = -1;

const std::string ACTION_ARGU_SCROLL_STUB = "scrolltype"; // wait for change
const std::string ACTION_DEFAULT_PARAM = "ACCESSIBILITY_ACTION_INVALID";

const std::set<std::string> TAGS_EMBED_COMPONENT = {
    "embeddedObject",
};

const std::map<Accessibility::ActionType, std::function<bool(const AccessibilityActionParam& param)>> ACTIONS = {
    { ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionScrollForward(param.scrollType);
        } },
    { ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionScrollBackward(param.scrollType);
        } },
    { ActionType::ACCESSIBILITY_ACTION_SET_TEXT,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionSetText(param.setTextArgument);
        } },
    { ActionType::ACCESSIBILITY_ACTION_SET_SELECTION,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionSetSelection(param.setSelectionStart,
                                                                      param.setSelectionEnd, param.setSelectionDir);
        } },
    { ActionType::ACCESSIBILITY_ACTION_COPY,
        [](const AccessibilityActionParam& param) { return param.accessibilityProperty->ActActionCopy(); } },
    { ActionType::ACCESSIBILITY_ACTION_CUT,
        [](const AccessibilityActionParam& param) { return param.accessibilityProperty->ActActionCut(); } },
    { ActionType::ACCESSIBILITY_ACTION_PASTE,
        [](const AccessibilityActionParam& param) { return param.accessibilityProperty->ActActionPaste(); } },
    { ActionType::ACCESSIBILITY_ACTION_CLICK,
        [](const AccessibilityActionParam& param) { return param.accessibilityProperty->ActActionClick(); } },
    { ActionType::ACCESSIBILITY_ACTION_LONG_CLICK,
        [](const AccessibilityActionParam& param) { return param.accessibilityProperty->ActActionLongClick(); } },
    { ActionType::ACCESSIBILITY_ACTION_SELECT,
        [](const AccessibilityActionParam& param) { return param.accessibilityProperty->ActActionSelect(); } },
    { ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION,
        [](const AccessibilityActionParam& param) { return param.accessibilityProperty->ActActionClearSelection(); } },
    { ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionMoveText(static_cast<int32_t>(param.moveUnit), true);
        } },
    { ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionMoveText(static_cast<int32_t>(param.moveUnit), false);
        } },
    { ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionSetIndex(static_cast<int32_t>(param.setCursorIndex));
        } },
    { ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK,
        [](const AccessibilityActionParam& param) {
            return param.accessibilityProperty->ActActionExecSubComponent(static_cast<int32_t>(param.spanId));
        } },
};

bool IsDynamicComponent(const RefPtr<NG::UINode>& node)
{
    return node && (node->GetTag() == V2::DYNAMIC_COMPONENT_ETS_TAG);
}

bool IsExtensionComponent(const RefPtr<NG::UINode>& node)
{
    return node && (node->GetTag() == V2::UI_EXTENSION_COMPONENT_ETS_TAG
        || node->GetTag() == V2::EMBEDDED_COMPONENT_ETS_TAG
        || node->GetTag() == V2::ISOLATED_COMPONENT_ETS_TAG);
}

bool IsIsolatedComponent(const RefPtr<NG::UINode>& node)
{
    return node && (node->GetTag() == V2::ISOLATED_COMPONENT_ETS_TAG);
}

bool IsUIExtensionShowPlaceholder(const RefPtr<NG::UINode>& node)
{
    CHECK_NULL_RETURN(node, true);
    if (node->GetTag() == V2::ISOLATED_COMPONENT_ETS_TAG) {
        return false;
    }
#ifdef WINDOW_SCENE_SUPPORTED
    auto pipeline = node->GetContextRefPtr();
    CHECK_NULL_RETURN(pipeline, true);
    auto manager = pipeline->GetUIExtensionManager();
    CHECK_NULL_RETURN(manager, true);
    return manager->IsShowPlaceholder(node->GetId());
#endif
    return true;
}

bool NeedUpdateChildrenOfAccessibilityElementInfo(const RefPtr<NG::UINode>& node)
{
    return !IsDynamicComponent(node);
}

bool CheckChildIsAccessibilityFocus(const RefPtr<NG::UINode> &uiNode, RefPtr<NG::FrameNode> &focusFrameNode)
{
    CHECK_NULL_RETURN(uiNode, false);
    auto nowFrameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
    if (nowFrameNode) {
        auto accessibilityProperty = nowFrameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        if (accessibilityProperty) {
            auto isFocus = accessibilityProperty->GetAccessibilityFocusState();
            if (isFocus) {
                focusFrameNode = nowFrameNode;
                return true;
            }
        }
    }
    auto children = uiNode->GetChildren(true);
    for (const auto& child : children) {
        if (CheckChildIsAccessibilityFocus(child, focusFrameNode)) {
            return true;
        }
    }
    return false;
}

void UpdatePaintNodeRender(const RefPtr<NG::FrameNode>& focusNode)
{
    CHECK_NULL_VOID(focusNode);
    ACE_SCOPED_TRACE("Node[%s] UpdatePaintNodeRender", focusNode->GetTag().c_str());
    auto paintNode = focusNode->GetPaintNode();
    CHECK_NULL_VOID(paintNode);
    auto pattern = paintNode->GetPattern<NG::AccessibilityFocusPaintNodePattern>();
    CHECK_NULL_VOID(pattern);
    pattern->UpdateRenderWithFocusNode();
    auto renderContext = paintNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto accessibilityProperty = focusNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);

    if (accessibilityProperty->IsMatchAccessibilityResponseRegion(false)) {
        auto rectInt = accessibilityProperty->GetAccessibilityResponseRegionRect(false);
        renderContext->UpdateAccessibilityFocusRect(rectInt);
    } else {
        renderContext->UpdateAccessibilityRoundRect();
    }
}

Accessibility::EventType ConvertStrToEventType(const std::string& type)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<Accessibility::EventType> eventTypeMap[] = {
        { ACCESSIBILITY_CLEAR_FOCUS_EVENT, Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT },
        { ACCESSIBILITY_FOCUSED_EVENT, Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT },
        { DOM_CLICK, Accessibility::EventType::TYPE_VIEW_CLICKED_EVENT },
        { DOM_FOCUS, Accessibility::EventType::TYPE_VIEW_FOCUSED_EVENT },
        { DOM_LONG_PRESS, Accessibility::EventType::TYPE_VIEW_LONG_CLICKED_EVENT },
        { MOUSE_HOVER_ENTER, Accessibility::EventType::TYPE_VIEW_HOVER_ENTER_EVENT },
        { MOUSE_HOVER_EXIT, Accessibility::EventType::TYPE_VIEW_HOVER_EXIT_EVENT },
        { PAGE_CHANGE_EVENT, Accessibility::EventType::TYPE_PAGE_STATE_UPDATE },
        { SCROLL_END_EVENT, Accessibility::EventType::TYPE_VIEW_SCROLLED_EVENT },
        { SCROLL_START_EVENT, Accessibility::EventType::TYPE_VIEW_SCROLLED_START },
        { DOM_SELECTED, Accessibility::EventType::TYPE_VIEW_SELECTED_EVENT },
        { TEXT_CHANGE_EVENT, Accessibility::EventType::TYPE_VIEW_TEXT_UPDATE_EVENT },
        { DOM_TOUCH_END, Accessibility::EventType::TYPE_TOUCH_END },
        { DOM_TOUCH_START, Accessibility::EventType::TYPE_TOUCH_BEGIN },
    };
    Accessibility::EventType eventType = Accessibility::EventType::TYPE_VIEW_INVALID;
    int64_t idx = BinarySearchFindIndex(eventTypeMap, ArraySize(eventTypeMap), type.c_str());
    if (idx >= 0) {
        eventType = eventTypeMap[idx].value;
    }
    return eventType;
}

Accessibility::EventType ConvertAceEventType(AccessibilityEventType type)
{
    static const LinearEnumMapNode<AccessibilityEventType, Accessibility::EventType> eventTypeMap[] = {
        { AccessibilityEventType::CLICK, Accessibility::EventType::TYPE_VIEW_CLICKED_EVENT },
        { AccessibilityEventType::LONG_PRESS, Accessibility::EventType::TYPE_VIEW_LONG_CLICKED_EVENT },
        { AccessibilityEventType::SELECTED, Accessibility::EventType::TYPE_VIEW_SELECTED_EVENT },
        { AccessibilityEventType::FOCUS, Accessibility::EventType::TYPE_VIEW_FOCUSED_EVENT },
        { AccessibilityEventType::TEXT_CHANGE, Accessibility::EventType::TYPE_VIEW_TEXT_UPDATE_EVENT },
        { AccessibilityEventType::HOVER_ENTER_EVENT, Accessibility::EventType::TYPE_VIEW_HOVER_ENTER_EVENT },
        { AccessibilityEventType::PAGE_CHANGE, Accessibility::EventType::TYPE_PAGE_STATE_UPDATE },
        { AccessibilityEventType::HOVER_EXIT_EVENT, Accessibility::EventType::TYPE_VIEW_HOVER_EXIT_EVENT },
        { AccessibilityEventType::CHANGE, Accessibility::EventType::TYPE_PAGE_CONTENT_UPDATE },
        { AccessibilityEventType::COMPONENT_CHANGE, Accessibility::EventType::TYPE_VIEW_TEXT_UPDATE_EVENT },
        { AccessibilityEventType::SCROLL_END, Accessibility::EventType::TYPE_VIEW_SCROLLED_EVENT },
        { AccessibilityEventType::TEXT_SELECTION_UPDATE,
            Accessibility::EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT },
        { AccessibilityEventType::ACCESSIBILITY_FOCUSED,
            Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT },
        { AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED,
            Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT },
        { AccessibilityEventType::TEXT_MOVE_UNIT, Accessibility::EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT },
        { AccessibilityEventType::REQUEST_FOCUS, Accessibility::EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY },
        { AccessibilityEventType::SCROLL_START, Accessibility::EventType::TYPE_VIEW_SCROLLED_START },
        { AccessibilityEventType::PAGE_CLOSE, Accessibility::EventType::TYPE_PAGE_CLOSE },
        { AccessibilityEventType::ANNOUNCE_FOR_ACCESSIBILITY,
            Accessibility::EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY },
        { AccessibilityEventType::PAGE_OPEN, Accessibility::EventType::TYPE_PAGE_OPEN },
        { AccessibilityEventType::ELEMENT_INFO_CHANGE, Accessibility::EventType::TYPE_ELEMENT_INFO_CHANGE },
        { AccessibilityEventType::ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT,
            Accessibility::EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT },
        { AccessibilityEventType::SCROLLING_EVENT, Accessibility::EventType::TYPE_VIEW_SCROLLING_EVENT },
    };
    Accessibility::EventType eventType = Accessibility::EventType::TYPE_VIEW_INVALID;
    int64_t idx = BinarySearchFindIndex(eventTypeMap, ArraySize(eventTypeMap), type);
    if (idx >= 0) {
        eventType = eventTypeMap[idx].value;
    }
    return eventType;
}

ActionType ConvertAceAction(AceAction aceAction)
{
    static const ActionTable actionTable[] = {
        { AceAction::ACTION_CLICK, ActionType::ACCESSIBILITY_ACTION_CLICK },
        { AceAction::ACTION_LONG_CLICK, ActionType::ACCESSIBILITY_ACTION_LONG_CLICK },
        { AceAction::ACTION_SCROLL_FORWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD },
        { AceAction::ACTION_SCROLL_BACKWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD },
        { AceAction::ACTION_FOCUS, ActionType::ACCESSIBILITY_ACTION_FOCUS },
        { AceAction::ACTION_CLEAR_FOCUS, ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS },
        { AceAction::ACTION_ACCESSIBILITY_FOCUS, ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS },
        { AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS, ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS },
        { AceAction::ACTION_NEXT_AT_MOVEMENT_GRANULARITY, ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT },
        { AceAction::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY, ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT },
        { AceAction::ACTION_SET_TEXT, ActionType::ACCESSIBILITY_ACTION_SET_TEXT },
        { AceAction::ACTION_COPY, ActionType::ACCESSIBILITY_ACTION_COPY },
        { AceAction::ACTION_PASTE, ActionType::ACCESSIBILITY_ACTION_PASTE },
        { AceAction::ACTION_CUT, ActionType::ACCESSIBILITY_ACTION_CUT },
        { AceAction::ACTION_SELECT, ActionType::ACCESSIBILITY_ACTION_SELECT },
        { AceAction::ACTION_CLEAR_SELECTION, ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION },
        { AceAction::ACTION_SET_SELECTION, ActionType::ACCESSIBILITY_ACTION_SET_SELECTION },
        { AceAction::ACTION_SET_CURSOR_POSITION, ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION },
        { AceAction::ACTION_EXEC_SUB_COMPONENT, ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK },
        { AceAction::ACTION_NEXT_HTML_ITEM, ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM },
        { AceAction::ACTION_PREVIOUS_HTML_ITEM, ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM },
    };
    for (const auto& item : actionTable) {
        if (aceAction == item.aceAction) {
            return item.action;
        }
    }
    return ActionType::ACCESSIBILITY_ACTION_INVALID;
}

inline RangeInfo ConvertAccessibilityValue(const AccessibilityValue& value)
{
    return RangeInfo(value.min, value.max, value.current);
}

int64_t ConvertToCardAccessibilityId(int64_t nodeId, int64_t cardId, int64_t rootNodeId)
{
    // result is integer total ten digits, top five for agp virtualViewId, end five for ace nodeId,
    // for example agp virtualViewId is 32, ace nodeId is 1000001, convert to result is 00032 10001.
    int64_t result = 0;
    if (nodeId == rootNodeId + ROOT_STACK_BASE) {
        // for example agp virtualViewId is 32 root node is 2100000, convert to result is 00032 21000.
        result = cardId * CARD_BASE + (static_cast<int64_t>(nodeId / CARD_BASE)) * CARD_ROOT_NODE_ID_RATION +
                 nodeId % CARD_BASE;
    } else {
        result = cardId * CARD_BASE + (static_cast<int64_t>(nodeId / DOM_ROOT_NODE_ID_BASE)) * CARD_NODE_ID_RATION +
                 nodeId % DOM_ROOT_NODE_ID_BASE;
    }
    return result;
}
void CheckInvalidNodeParentID(const RefPtr<AccessibilityNode>& node, AccessibilityElementInfo& nodeInfo,
    const RefPtr<JsAccessibilityManager>& manager, int& leftTopX, int& leftTopY, int& rightBottomX, int& rightBottomY)
{
    if (node->GetParentId() == -1) {
        const auto& children = node->GetChildList();
        if (!children.empty()) {
            auto lastChildNode = manager->GetAccessibilityNodeById(children.back()->GetNodeId());
            if (lastChildNode) {
                rightBottomX = leftTopX + static_cast<int>(lastChildNode->GetWidth());
                rightBottomY = leftTopY + static_cast<int>(lastChildNode->GetHeight());
                Accessibility::Rect bounds(leftTopX, leftTopY, rightBottomX, rightBottomY);
                nodeInfo.SetRectInScreen(bounds);
            }
        }
        nodeInfo.SetParent(INVALID_PARENT_ID);
    }
    if (node->GetNodeId() == 0) {
        nodeInfo.SetParent(INVALID_PARENT_ID);
    }
}
void UpdateAccessibilityNodeInfo(const RefPtr<AccessibilityNode>& node, AccessibilityElementInfo& nodeInfo,
    const RefPtr<JsAccessibilityManager>& manager, int windowId)
{
    int leftTopX = static_cast<int>(node->GetLeft()) + manager->GetWindowLeft(node->GetWindowId());
    int leftTopY = static_cast<int>(node->GetTop()) + manager->GetWindowTop(node->GetWindowId());
    int rightBottomX = leftTopX + static_cast<int>(node->GetWidth());
    int rightBottomY = leftTopY + static_cast<int>(node->GetHeight());
    if (manager->isOhosHostCard()) {
        int64_t id = ConvertToCardAccessibilityId(node->GetNodeId(), manager->GetCardId(), manager->GetRootNodeId());
        nodeInfo.SetAccessibilityId(id);
        if (node->GetParentId() == -1) {
            nodeInfo.SetParent(-1);
        } else {
            nodeInfo.SetParent(
                ConvertToCardAccessibilityId(node->GetParentId(), manager->GetCardId(), manager->GetRootNodeId()));
        }
        leftTopX = static_cast<int>(node->GetLeft() + manager->GetCardOffset().GetX());
        leftTopY = static_cast<int>(node->GetTop() + manager->GetCardOffset().GetY());
        rightBottomX = leftTopX + static_cast<int>(node->GetWidth());
        rightBottomY = leftTopY + static_cast<int>(node->GetHeight());
        Accessibility::Rect bounds(leftTopX, leftTopY, rightBottomX, rightBottomY);
        nodeInfo.SetRectInScreen(bounds);
    } else {
        if (node->GetTag() == SIDEBARCONTAINER_TAG) {
            Rect sideBarRect = node->GetRect();
            for (const auto& childNode : node->GetChildList()) {
                sideBarRect = sideBarRect.CombineRect(childNode->GetRect());
            }
            leftTopX = static_cast<int>(sideBarRect.Left()) + manager->GetWindowLeft(node->GetWindowId());
            leftTopY = static_cast<int>(sideBarRect.Top()) + manager->GetWindowTop(node->GetWindowId());
            rightBottomX = static_cast<int>(sideBarRect.Right()) + manager->GetWindowLeft(node->GetWindowId());
            rightBottomY = static_cast<int>(sideBarRect.Bottom()) + manager->GetWindowTop(node->GetWindowId());
        }
        Accessibility::Rect bounds(leftTopX, leftTopY, rightBottomX, rightBottomY);
        nodeInfo.SetRectInScreen(bounds);
        nodeInfo.SetComponentId(static_cast<int64_t>(node->GetNodeId()));
        nodeInfo.SetParent(static_cast<int64_t>(node->GetParentId()));
    }
    CheckInvalidNodeParentID(node, nodeInfo, manager, leftTopX, leftTopY, rightBottomX, rightBottomY);
    nodeInfo.SetPagePath(manager->GetPagePath());
    nodeInfo.SetWindowId(windowId);
    nodeInfo.SetChecked(node->GetCheckedState());
    nodeInfo.SetEnabled(node->GetEnabledState());
    nodeInfo.SetFocused(node->GetFocusedState());
    nodeInfo.SetSelected(node->GetSelectedState());
    nodeInfo.SetCheckable(node->GetCheckableState());
    nodeInfo.SetClickable(node->GetClickableState());
    nodeInfo.SetFocusable(node->GetFocusableState());
    nodeInfo.SetScrollable(node->GetScrollableState());
    nodeInfo.SetLongClickable(node->GetLongClickableState());
    nodeInfo.SetEditable(node->GetEditable());
    nodeInfo.SetPluraLineSupported(node->GetIsMultiLine());
    nodeInfo.SetPassword(node->GetIsPassword());
    nodeInfo.SetTextLengthLimit(node->GetMaxTextLength());
    nodeInfo.SetSelectedBegin(node->GetTextSelectionStart());
    nodeInfo.SetSelectedEnd(node->GetTextSelectionEnd());
    nodeInfo.SetVisible(node->GetShown() && node->GetVisible());
    nodeInfo.SetHint(node->GetHintText());
    std::string accessibilityLabel = node->GetAccessibilityLabel();
    nodeInfo.SetLabeled(atol(accessibilityLabel.c_str()));
    nodeInfo.SetError(node->GetErrorText());
    nodeInfo.SetComponentResourceId(node->GetJsComponentId());
    nodeInfo.SetInspectorKey(node->GetJsComponentId());
    RangeInfo rangeInfo = ConvertAccessibilityValue(node->GetAccessibilityValue());
    nodeInfo.SetRange(rangeInfo);
    nodeInfo.SetInputType(static_cast<int>(node->GetTextInputType()));
    nodeInfo.SetComponentType(node->GetTag());
    GridInfo gridInfo(
        node->GetCollectionInfo().rows, node->GetCollectionInfo().columns, (nodeInfo.IsPluraLineSupported() ? 0 : 1));
    nodeInfo.SetGrid(gridInfo);
    nodeInfo.SetAccessibilityFocus(node->GetAccessibilityFocusedState());
    nodeInfo.SetPageId(node->GetPageId());

    int32_t row = node->GetCollectionItemInfo().row;
    int32_t column = node->GetCollectionItemInfo().column;
    GridItemInfo gridItemInfo(row, row, column, column, false, nodeInfo.IsSelected());
    nodeInfo.SetGridItem(gridItemInfo);
    nodeInfo.SetBundleName(Container::CurrentBundleName());

    if (node->GetTag() == LIST_TAG) {
        nodeInfo.SetItemCounts(node->GetListItemCounts());
        nodeInfo.SetBeginIndex(node->GetListBeginIndex());
        nodeInfo.SetEndIndex(node->GetListEndIndex());
    }
    if (node->GetIsPassword()) {
        std::string strStar(node->GetText().size(), '*');
        nodeInfo.SetContent(strStar);
    } else {
        nodeInfo.SetContent(node->GetText());
    }

    auto supportAceActions = node->GetSupportAction();
    std::vector<ActionType> actions(supportAceActions.size());

    for (auto it = supportAceActions.begin(); it != supportAceActions.end(); ++it) {
        AccessibleAction action(ConvertAceAction(*it), "ace");
        nodeInfo.AddAction(action);
    }

    if (node->GetImportantForAccessibility() == NG::AccessibilityProperty::Level::YES_STR) {
        actions.emplace_back(ActionType::ACCESSIBILITY_ACTION_FOCUS);
        nodeInfo.SetCheckable(true);
    } else if (node->GetImportantForAccessibility() == NG::AccessibilityProperty::Level::NO_STR ||
               node->GetImportantForAccessibility() == NG::AccessibilityProperty::Level::NO_HIDE_DESCENDANTS) {
        nodeInfo.SetVisible(false);
    }
    manager->UpdateNodeChildIds(node);
    for (const auto& child : node->GetChildIds()) {
        nodeInfo.AddChild(child);
    }

#ifdef ACE_DEBUG
    std::string actionForLog;
    for (const auto& action : supportAceActions) {
        if (!actionForLog.empty()) {
            actionForLog.append(",");
        }
        actionForLog.append(std::to_string(static_cast<int32_t>(action)));
    }
#endif
}

void UpdateCacheInfo(std::list<AccessibilityElementInfo>& infos, uint32_t mode, const RefPtr<AccessibilityNode>& node,
    const RefPtr<JsAccessibilityManager>& jsAccessibilityManager, int windowId)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "elementId: %{public}d, mode: %{public}d", node->GetNodeId(), mode);
    // parent
    uint32_t umode = mode;
    if (umode & static_cast<uint32_t>(PREFETCH_PREDECESSORS)) {
        if (node->GetParentId() != -1 && node->GetParentId() != DEFAULT_PARENT_ID) {
            AccessibilityElementInfo parentNodeInfo;
            UpdateAccessibilityNodeInfo(node->GetParentNode(), parentNodeInfo, jsAccessibilityManager, windowId);
            infos.emplace_back(parentNodeInfo);
        }
    }
    // sister/brothers
    if (umode & static_cast<uint32_t>(PREFETCH_SIBLINGS)) {
        if (node->GetParentId() != -1 && node->GetParentId() != DEFAULT_PARENT_ID) {
            for (const auto& item : node->GetParentNode()->GetChildList()) {
                if (node->GetNodeId() != item->GetNodeId()) {
                    AccessibilityElementInfo siblingNodeInfo;
                    UpdateAccessibilityNodeInfo(item, siblingNodeInfo, jsAccessibilityManager, windowId);
                    infos.emplace_back(siblingNodeInfo);
                }
            }
        }
    }
    // children
    if (umode & static_cast<uint32_t>(PREFETCH_CHILDREN)) {
        for (const auto& item : node->GetChildList()) {
            AccessibilityElementInfo childNodeInfo;
            UpdateAccessibilityNodeInfo(item, childNodeInfo, jsAccessibilityManager, windowId);
            infos.emplace_back(childNodeInfo);
        }
    }

    // get all children
    if (umode & static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN)) {
        std::list<RefPtr<AccessibilityNode>> children;
        for (const auto& item : node->GetChildList()) {
            children.emplace_back(item);
        }

        while (!children.empty()) {
            auto parent = children.front();
            children.pop_front();
            AccessibilityElementInfo childNodeInfo;
            UpdateAccessibilityNodeInfo(parent, childNodeInfo, jsAccessibilityManager, windowId);
            infos.push_back(childNodeInfo);
            for (const auto& item : parent->GetChildList()) {
                children.emplace_back(item);
            }
        }
    }
}

void SortAccessibilityInfosByBreadth(std::list<AccessibilityElementInfo>& infos,
    std::list<int64_t>& accessibilityIdQueue, std::list<AccessibilityElementInfo>& output)
{
    while (!accessibilityIdQueue.empty()) {
        auto accessibilityId = accessibilityIdQueue.front();
        accessibilityIdQueue.pop_front();
        for (std::list<AccessibilityElementInfo>::iterator info = infos.begin(); info != infos.end(); ++info) {
            if (accessibilityId != info->GetAccessibilityId()) {
                continue;
            }
            for (auto& child : info->GetChildIds()) {
                accessibilityIdQueue.emplace_back(child);
            }
            output.emplace_back(*info);
            infos.erase(info);
            break;
        }
        SortAccessibilityInfosByBreadth(infos, accessibilityIdQueue, output);
    }
}

void SortExtensionAccessibilityInfo(std::list<AccessibilityElementInfo>& infos, int64_t rootAccessibilityId)
{
    auto input = infos;
    infos.clear();
    std::list<int64_t> accessibilityIdQueue;
    accessibilityIdQueue.emplace_back(rootAccessibilityId);
    SortAccessibilityInfosByBreadth(input, accessibilityIdQueue, infos);
}

void ConvertExtensionAccessibilityId(AccessibilityElementInfo& info, const RefPtr<NG::FrameNode>& extensionNode,
    int64_t uiExtensionOffset, AccessibilityElementInfo& parentInfo)
{
    auto extensionAbilityId =
            extensionNode->WrapExtensionAbilityId(uiExtensionOffset, info.GetAccessibilityId());
    info.SetAccessibilityId(extensionAbilityId);
    auto parentNodeId =
        extensionNode->WrapExtensionAbilityId(uiExtensionOffset, info.GetParentNodeId());
    info.SetParent(parentNodeId);
    auto childIds = info.GetChildIds();
    for (auto& child : childIds) {
        info.RemoveChild(child);
        info.AddChild(extensionNode->WrapExtensionAbilityId(uiExtensionOffset, child));
    }
    if (V2::ROOT_ETS_TAG == info.GetComponentType()) {
        for (auto& child : info.GetChildIds()) {
            parentInfo.AddChild(child);
        }
    }
}

void ConvertExtensionAccessibilityNodeId(std::list<AccessibilityElementInfo>& infos,
    const RefPtr<NG::FrameNode>& extensionNode, int64_t uiExtensionOffset,
    AccessibilityElementInfo& parentInfo)
{
    CHECK_NULL_VOID(extensionNode);
    for (auto& accessibilityElementInfo : infos) {
        ConvertExtensionAccessibilityId(accessibilityElementInfo, extensionNode, uiExtensionOffset, parentInfo);
    }
    for (auto& accessibilityElementInfo : infos) {
        if (std::find(parentInfo.GetChildIds().begin(), parentInfo.GetChildIds().end(),
            accessibilityElementInfo.GetAccessibilityId()) != parentInfo.GetChildIds().end()) {
            accessibilityElementInfo.SetParent(extensionNode->GetAccessibilityId());
        }
    }
}

inline std::string BoolToString(bool tag)
{
    return tag ? "true" : "false";
}

std::string ConvertInputTypeToString(AceTextCategory type)
{
    switch (type) {
        case AceTextCategory::INPUT_TYPE_DEFAULT:
            return "INPUT_TYPE_DEFAULT";
        case AceTextCategory::INPUT_TYPE_TEXT:
            return "INPUT_TYPE_TEXT";
        case AceTextCategory::INPUT_TYPE_EMAIL:
            return "INPUT_TYPE_EMAIL";
        case AceTextCategory::INPUT_TYPE_DATE:
            return "INPUT_TYPE_DATE";
        case AceTextCategory::INPUT_TYPE_TIME:
            return "INPUT_TYPE_TIME";
        case AceTextCategory::INPUT_TYPE_NUMBER:
            return "INPUT_TYPE_NUMBER";
        case AceTextCategory::INPUT_TYPE_PASSWORD:
            return "INPUT_TYPE_PASSWORD";
        case AceTextCategory::INPUT_TYPE_PHONENUMBER:
            return "INPUT_TYPE_PHONENUMBER";
        case AceTextCategory::INPUT_TYPE_USER_NAME:
            return "INPUT_TYPE_USER_NAME";
        case AceTextCategory::INPUT_TYPE_NEW_PASSWORD:
            return "INPUT_TYPE_NEW_PASSWORD";
        default:
            return "illegal input type";
    }
}

bool FindAccessibilityFocus(const RefPtr<AccessibilityNode>& node, RefPtr<AccessibilityNode>& resultNode)
{
    CHECK_NULL_RETURN(node, false);
    if (node->GetAccessibilityFocusedState()) {
        resultNode = node;
        return true;
    }
    if (!node->GetChildList().empty()) {
        for (const auto& item : node->GetChildList()) {
            if (resultNode != nullptr) {
                return true;
            }
            if (FindAccessibilityFocus(item, resultNode)) {
                return true;
            }
        }
    }

    return false;
}

void FindFocusedExtensionElementInfoNG(const SearchParameter& searchParam,
    const RefPtr<NG::FrameNode>& node, Accessibility::AccessibilityElementInfo& info)
{
    if (NG::UI_EXTENSION_OFFSET_MIN < (searchParam.uiExtensionOffset + 1)) {
        node->FindFocusedExtensionElementInfoNG(searchParam.nodeId, searchParam.mode,
            searchParam.uiExtensionOffset / NG::UI_EXTENSION_ID_FACTOR, info);
    } else {
        info.SetValidElement(false);
    }
}

void SetUiExtensionAbilityParentIdForFocus(const RefPtr<NG::UINode>& uiExtensionNode,
    const int64_t uiExtensionOffset, Accessibility::AccessibilityElementInfo& info)
{
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiExtensionNode);
    if (frameNode) {
        auto parentId = info.GetParentNodeId();
        AccessibilityElementInfo parentInfo;
        ConvertExtensionAccessibilityId(info, frameNode, uiExtensionOffset, parentInfo);
        if (parentId == NG::UI_EXTENSION_ROOT_ID) {
            info.SetParent(frameNode->GetAccessibilityId());
        }
    }
}

RefPtr<NG::FrameNode> GetFramenodeByAccessibilityId(const RefPtr<NG::FrameNode>& root, int64_t id);

RefPtr<NG::FrameNode> FindAccessibilityFocus(const RefPtr<NG::UINode>& node,
    int32_t focusType, Accessibility::AccessibilityElementInfo& info,
    const int64_t uiExtensionOffset, const RefPtr<PipelineBase>& context, int64_t currentFocusNodeId)
{
    CHECK_NULL_RETURN(node, nullptr);
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    if (frameNode) {
        if (frameNode->GetRenderContext()->GetAccessibilityFocus().value_or(false)) {
            auto node = GetFramenodeByAccessibilityId(frameNode, currentFocusNodeId);
            std::string finalNodeId = node ? std::to_string(node->GetAccessibilityId()) : "nullptr";
            TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
                "currentFocusNodeId: %{public}" PRId64 ", result accessiblityId: %s",
                currentFocusNodeId, finalNodeId.c_str());
            return node;
        }
    }
    if (node->GetChildren(true).empty()) {
        return nullptr;
    }
    for (const auto& child : node->GetChildren(true)) {
        auto extensionNode = AceType::DynamicCast<NG::FrameNode>(child);
        if (IsIsolatedComponent(child) && extensionNode &&
            !IsUIExtensionShowPlaceholder(extensionNode) &&
            (extensionNode->GetUiExtensionId() > NG::UI_EXTENSION_UNKNOW_ID) &&
            (((extensionNode->GetUiExtensionId() <= NG::UI_EXTENSION_ID_FIRST_MAX) &&
            (NG::UI_EXTENSION_OFFSET_MAX == uiExtensionOffset)) ||
            (extensionNode->GetUiExtensionId() <= NG::UI_EXTENSION_ID_OTHER_MAX))) {
            SearchParameter transferSearchParam {NG::UI_EXTENSION_ROOT_ID, "", focusType, uiExtensionOffset};
            OHOS::Ace::Framework::FindFocusedExtensionElementInfoNG(
                transferSearchParam, extensionNode, info);
            if (info.GetAccessibilityId() < 0) {
                continue;
            }
            SetUiExtensionAbilityParentIdForFocus(extensionNode, uiExtensionOffset, info);
            return extensionNode;
        }
        auto result = FindAccessibilityFocus(child, focusType, info, uiExtensionOffset, context, currentFocusNodeId);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

bool FindInputFocus(const RefPtr<AccessibilityNode>& node, RefPtr<AccessibilityNode>& resultNode)
{
    CHECK_NULL_RETURN(node, false);
    if (!node->GetFocusedState() && (node->GetParentId() != -1)) {
        return false;
    }
    if (node->GetFocusedState()) {
        resultNode = node;
    }
    if (!node->GetChildList().empty()) {
        for (const auto& item : node->GetChildList()) {
            if (FindInputFocus(item, resultNode)) {
                return true;
            }
        }
    }
    return node->GetFocusedState();
}

RefPtr<NG::FrameNode> FindInputFocus(const RefPtr<NG::UINode>& node, int32_t focusType,
    Accessibility::AccessibilityElementInfo& info, const int64_t uiExtensionOffset,
    const RefPtr<PipelineBase>& context)
{
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    CHECK_NULL_RETURN(frameNode, nullptr);
    if (!(frameNode->GetFocusHub() ? frameNode->GetFocusHub()->IsCurrentFocus() : false)) {
        return nullptr;
    }
    if (frameNode->GetFocusHub()->IsChild()) {
        if (frameNode->IsInternal()) {
            return frameNode->GetFocusParent();
        }
        return frameNode;
    }
    auto focusHub = frameNode->GetFocusHub();
    RefPtr<NG::FrameNode> target;
    focusHub->AnyChildFocusHub([&target, &info, context, focusType, uiExtensionOffset](
                                   const RefPtr<NG::FocusHub>& focusChild) {
        auto extensionNode = focusChild->GetFrameNode();
        if ((extensionNode && IsIsolatedComponent(extensionNode)) &&
            !IsUIExtensionShowPlaceholder(extensionNode) &&
            (extensionNode->GetUiExtensionId() > NG::UI_EXTENSION_UNKNOW_ID) &&
            (((extensionNode->GetUiExtensionId() <= NG::UI_EXTENSION_ID_FIRST_MAX) &&
            (NG::UI_EXTENSION_OFFSET_MAX == uiExtensionOffset)) ||
            (extensionNode->GetUiExtensionId() <= NG::UI_EXTENSION_ID_OTHER_MAX))) {
            SearchParameter transferSearchParam {NG::UI_EXTENSION_ROOT_ID, "", focusType, uiExtensionOffset};
            OHOS::Ace::Framework::FindFocusedExtensionElementInfoNG(
                transferSearchParam, extensionNode, info);
            if (info.GetAccessibilityId() < 0) {
                return false;
            }
            SetUiExtensionAbilityParentIdForFocus(extensionNode, uiExtensionOffset, info);
            target = extensionNode;
            return true;
        }
        target = FindInputFocus(focusChild->GetFrameNode(), focusType, info, uiExtensionOffset, context);
        return target ? true : false;
    });
    return target;
}

void FindText(
    const RefPtr<AccessibilityNode>& node, const std::string& text, std::list<RefPtr<AccessibilityNode>>& nodeList)
{
    CHECK_NULL_VOID(node);
    if (node->GetText().find(text) != std::string::npos) {
        nodeList.push_back(node);
    }
    if (!node->GetChildList().empty()) {
        for (const auto& child : node->GetChildList()) {
            FindText(child, text, nodeList);
        }
    }
}

void FindText(const RefPtr<NG::UINode>& node, const std::string& text, std::list<RefPtr<NG::FrameNode>>& nodeList)
{
    CHECK_NULL_VOID(node);

    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    if (frameNode && !frameNode->IsInternal()) {
        if (frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>()->GetGroupText().find(text) !=
            std::string::npos) {
            nodeList.push_back(frameNode);
        }
    }

    if (!node->GetChildren(true).empty()) {
        for (const auto& child : node->GetChildren(true)) {
            FindText(child, text, nodeList);
        }
    }
}

bool FindFrameNodeByAccessibilityId(int64_t id, const std::list<RefPtr<NG::UINode>>& children,
    std::queue<RefPtr<NG::UINode>>& nodes, RefPtr<NG::FrameNode>& result)
{
    for (const auto& child : children) {
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(child);
        if (frameNode != nullptr && !frameNode->CheckAccessibilityLevelNo()) {
            if (frameNode->GetAccessibilityId() == id) {
                result = AceType::DynamicCast<NG::FrameNode>(child);
                return true;
            }
        }
        nodes.push(child);
    }
    return false;
}

RefPtr<NG::FrameNode> GetFramenodeByAccessibilityId(const RefPtr<NG::FrameNode>& root, int64_t id)
{
    CHECK_NULL_RETURN(root, nullptr);
    if (root->GetAccessibilityId() == id) {
        return root;
    }
    std::queue<RefPtr<NG::UINode>> nodes;
    nodes.push(root);
    RefPtr<NG::FrameNode> frameNodeResult = nullptr;

    while (!nodes.empty()) {
        auto current = nodes.front();
        nodes.pop();
        if (current->HasVirtualNodeAccessibilityProperty()) {
            auto fnode = AceType::DynamicCast<NG::FrameNode>(current);
            auto property = fnode->GetAccessibilityProperty<NG::AccessibilityProperty>();
            const auto& children = std::list<RefPtr<NG::UINode>> { property->GetAccessibilityVirtualNode() };
            if (FindFrameNodeByAccessibilityId(id, children, nodes, frameNodeResult)) {
                return frameNodeResult;
            }
        } else {
            const auto& children = current->GetChildren(true);
            if (FindFrameNodeByAccessibilityId(id, children, nodes, frameNodeResult)) {
                return frameNodeResult;
            }
        }
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(current);
        if (!frameNode) {
            continue;
        }
        auto overlayNode = frameNode->GetOverlayNode();
        if (overlayNode) {
            const auto& children = std::list<RefPtr<NG::UINode>> { overlayNode };
            if (FindFrameNodeByAccessibilityId(id, children, nodes, frameNodeResult)) {
                return frameNodeResult;
            }
        }
    }
    return nullptr;
}

using FindCondition = std::function<bool(const RefPtr<NG::FrameNode>&)>;

bool FindFrameNodeByCondition(const std::list<RefPtr<NG::UINode>>& children,
    std::queue<RefPtr<NG::UINode>>& nodes, RefPtr<NG::FrameNode>& result, FindCondition condition)
{
    for (const auto& child : children) {
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(child);
        if (frameNode != nullptr && !frameNode->CheckAccessibilityLevelNo()) {
            if (condition(frameNode)) {
                result = frameNode;
                return true;
            }
        }
        nodes.push(child);
    }
    return false;
}

RefPtr<NG::FrameNode> GetFramenodeByCondition(const RefPtr<NG::FrameNode>& root, FindCondition condition)
{
    CHECK_NULL_RETURN(root, nullptr);

    std::queue<RefPtr<NG::UINode>> nodes;
    nodes.push(root);
    RefPtr<NG::FrameNode> frameNodeResult = nullptr;

    while (!nodes.empty()) {
        auto current = nodes.front();
        nodes.pop();
        if (current->HasVirtualNodeAccessibilityProperty()) {
            auto fnode = AceType::DynamicCast<NG::FrameNode>(current);
            auto property = fnode->GetAccessibilityProperty<NG::AccessibilityProperty>();
            const auto& children = std::list<RefPtr<NG::UINode>> { property->GetAccessibilityVirtualNode() };
            if (FindFrameNodeByCondition(children, nodes, frameNodeResult, condition)) {
                return frameNodeResult;
            }
        } else {
            const auto& children = current->GetChildren(true);
            if (FindFrameNodeByCondition(children, nodes, frameNodeResult, condition)) {
                return frameNodeResult;
            }
        }
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(current);
        if (!frameNode) {
            continue;
        }
        auto overlayNode = frameNode->GetOverlayNode();
        if (overlayNode) {
            const auto& children = std::list<RefPtr<NG::UINode>> { overlayNode };
            if (FindFrameNodeByCondition(children, nodes, frameNodeResult, condition)) {
                return frameNodeResult;
            }
        }
    }
    return nullptr;
}

void GetFrameNodeParent(const RefPtr<NG::UINode>& uiNode, RefPtr<NG::FrameNode>& parent)
{
    if (AceType::InstanceOf<NG::FrameNode>(uiNode)) {
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
        if (!frameNode->IsInternal()) {
            parent = frameNode;
            return;
        }
    }
    CHECK_NULL_VOID(uiNode);
    auto parentNode = uiNode->GetParent();
    GetFrameNodeParent(parentNode, parent);
}

bool NeedChangeTreeId(int64_t elementId, int32_t targetTreeId)
{
    if (elementId == INVALID_PARENT_ID) {
        return true;
    }
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);
    if (splitTreeId != 0 &&
        splitTreeId != AccessibilityElementInfo::UNDEFINED_TREE_ID &&
        splitTreeId != targetTreeId) {
        return false;
    }
    return true;
}

std::string GetSurfaceIdByEmbedNode(const RefPtr<NG::FrameNode>& node)
{
    auto surfaceId = ElementRegister::GetInstance()->GetSurfaceIdByEmbedNode(AceType::RawPtr(node));
    return std::to_string(surfaceId);
}

WeakPtr<NG::FrameNode> GetEmbedNodeBySurfaceId(const std::string& surfaceId)
{
    std::stringstream ss(surfaceId);
    uint64_t id;
    if (ss >> id) {
        return ElementRegister::GetInstance()->GetEmbedNodeBySurfaceId(id);
    }
    return nullptr;
}

bool CheckFrameNodeByAccessibilityLevel(const RefPtr<NG::FrameNode>& frameNode, bool isParent)
{
    return true;
}

bool IsInPageNodes(int32_t pageId, const std::vector<RefPtr<NG::FrameNode>>& pageNodes)
{
    for (const auto& page : pageNodes) {
        CHECK_NULL_CONTINUE(page);
        if (pageId == page->GetPageId()) {
            return true;
        }
    }
    return false;
}

bool CheckAndSetAccessibilityVisible(const RefPtr<NG::FrameNode>& node, bool isReduceMode = false)
{
    if (!isReduceMode) {
        return true;
    }
    auto parentNode = node->GetParentFrameNode();
    if (!parentNode) {
        if (node->GetTag() != V2::PAGE_ETS_TAG) {
            node->SetAccessibilityVisible(node->IsActive() && node->IsVisible());
        }
    } else {
        if (node->GetTag() == V2::PAGE_ETS_TAG) {
            return node->GetAccessibilityVisible();
        }
        auto nodeAccessibilityVisible = node->IsActive() && node->IsVisible() && parentNode->GetAccessibilityVisible();
        node->SetAccessibilityVisible(nodeAccessibilityVisible);
    }
    return node->GetAccessibilityVisible();
}

bool IsValidEmbedTarget(const RefPtr<NG::FrameNode>& frameNode, const CommonProperty& commonProperty)
{
    return commonProperty.checkEmbedNode &&
           frameNode && commonProperty.checkGetFunc &&
           (commonProperty.checkGetFunc(frameNode) != INVALID_NODE_ID);
}

void GetFrameNodeChildren(
    const RefPtr<NG::UINode>& uiNode,
    std::vector<std::pair<int64_t, int32_t>>& childrenIdInfo,
    const CommonProperty& commonProperty)
{
    CHECK_NULL_VOID(uiNode);
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
    if (IsValidEmbedTarget(frameNode, commonProperty)) {
        return;
    }

    if (AceType::InstanceOf<NG::FrameNode>(uiNode)) {
        if (!frameNode->IsFirstVirtualNode()) {
            CHECK_NULL_VOID(frameNode->IsActive());
        }
        if (uiNode->GetTag() == "stage") {
        } else if (uiNode->GetTag() == "page") {
            if (!IsInPageNodes(uiNode->GetPageId(), commonProperty.pageNodes)) {
                return;
            }
        } else if (!frameNode->IsInternal() || frameNode->IsFirstVirtualNode()) {
            if (CheckFrameNodeByAccessibilityLevel(frameNode, false) &&
                CheckAndSetAccessibilityVisible(frameNode, commonProperty.isReduceMode)) {
                auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
                childrenIdInfo.emplace_back(
                    std::make_pair(frameNode->GetAccessibilityId(),
                        accessibilityProperty ? accessibilityProperty->GetAccessibilityZIndex() : -1));
                return;
            }
        }
    }

    if (frameNode) {
        auto overlayNode = frameNode->GetOverlayNode();
        if (overlayNode) {
            GetFrameNodeChildren(overlayNode, childrenIdInfo, commonProperty);
        }
    }

    if (AceType::InstanceOf<NG::FrameNode>(uiNode)) {
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
        auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        auto uiVirtualNode = accessibilityProperty->GetAccessibilityVirtualNode();
        if (uiVirtualNode != nullptr) {
            auto virtualNode = AceType::DynamicCast<NG::FrameNode>(uiVirtualNode);
            if (virtualNode != nullptr) {
                GetFrameNodeChildren(virtualNode, childrenIdInfo, commonProperty);
                return;
            }
        }
    }

    for (const auto& frameChild : uiNode->GetChildren(true)) {
        GetFrameNodeChildren(frameChild, childrenIdInfo, commonProperty);
    }
}

void GetFrameNodeChildren(
    const RefPtr<NG::UINode>& uiNode,
    std::list<RefPtr<NG::FrameNode>>& children,
    const CommonProperty& commonProperty)
{
    CHECK_NULL_VOID(uiNode);
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
    if (IsValidEmbedTarget(frameNode, commonProperty)) {
        return;
    }

    if (frameNode) {
        CHECK_NULL_VOID(frameNode->IsActive());
        if (uiNode->GetTag() == "page") {
            if (!commonProperty.pageNodes.empty() && !IsInPageNodes(uiNode->GetPageId(), commonProperty.pageNodes)) {
                return;
            }
        } else if (!frameNode->IsInternal() && uiNode->GetTag() != "stage") {
            if (CheckFrameNodeByAccessibilityLevel(frameNode, false) &&
                CheckAndSetAccessibilityVisible(frameNode, commonProperty.isReduceMode)) {
                children.emplace_back(frameNode);
                return;
            }
        }

        auto overlayNode = frameNode->GetOverlayNode();
        if (overlayNode) {
            GetFrameNodeChildren(overlayNode, children, commonProperty);
        }

        auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        auto uiVirtualNode = accessibilityProperty->GetAccessibilityVirtualNode();
        if (uiVirtualNode != nullptr) {
            auto virtualNode = AceType::DynamicCast<NG::FrameNode>(uiVirtualNode);
            if (virtualNode != nullptr) {
                GetFrameNodeChildren(virtualNode, children, commonProperty);
                return;
            }
        }
    }

    for (const auto& frameChild : uiNode->GetChildren(true)) {
        GetFrameNodeChildren(frameChild, children, commonProperty);
    }
}

bool GetNodeAccessibilityVisible(const RefPtr<NG::FrameNode>& frameNode, bool isAllAncestorAccessibilityVisible,
                                 bool clipVisible)
{
    if (frameNode->IsFirstVirtualNode()) {
        return frameNode->IsVisible() && isAllAncestorAccessibilityVisible && clipVisible;
    } else {
        return frameNode->IsActive() && frameNode->IsVisible() && isAllAncestorAccessibilityVisible && clipVisible;
    }
}

bool ProcessParentFrameNode(
    const RefPtr<NG::UINode>& parent, std::string& parentPath, bool& isAllAncestorAccessibilityVisible)
{
    auto parentFrameNode = AceType::DynamicCast<NG::FrameNode>(parent);
    if (parentFrameNode->CheckAccessibilityLevelNo()) {
        return false;
    }

    parentPath += "Parent ID: " + std::to_string(parent->GetAccessibilityId()) +
                  " IsActive: " + std::to_string(parentFrameNode->IsActive()) +
                  " IsVisible: " + std::to_string(parentFrameNode->IsVisible()) +
                  " AccessibilityVisible: " + std::to_string(parentFrameNode->GetAccessibilityVisible()) +
                  " Parent Tag: " + parent->GetTag() + " | ";

    if (parent->GetTag() == V2::PAGE_ETS_TAG) {
        isAllAncestorAccessibilityVisible = parentFrameNode->GetAccessibilityVisible();
    } else if (parentFrameNode->IsFirstVirtualNode()) {
        isAllAncestorAccessibilityVisible = parentFrameNode->IsVisible();
    } else {
        isAllAncestorAccessibilityVisible = parentFrameNode->IsActive() && parentFrameNode->IsVisible();
    }

    return !isAllAncestorAccessibilityVisible;
}

RefPtr<NG::UINode> GetInitialParent(const RefPtr<NG::UINode>& uiNode)
{
    if (AceType::InstanceOf<NG::FrameNode>(uiNode)) {
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
        if (frameNode->IsFirstVirtualNode()) {
            auto weakNode = frameNode->GetVirtualNodeParent();
            return weakNode.Upgrade();
        } else {
            return uiNode->GetParent();
        }
    }
    return nullptr;
}

void UpdateAccessibilityVisibleToRoot(const RefPtr<NG::UINode>& uiNode)
{
    RefPtr<NG::UINode> parent = GetInitialParent(uiNode);
    bool isAllAncestorAccessibilityVisible = true;
    bool clipVisible = true;
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
    CHECK_NULL_VOID(frameNode);
    OHOS::Ace::NG::RectF frameRect;
    OHOS::Ace::NG::RectF visibleInnerRect;
    OHOS::Ace::NG::RectF visibleRect;
    frameNode->SetIsCalculateInnerVisibleRectClip(true);
    frameNode->GetVisibleRectWithClip(visibleRect, visibleInnerRect, frameRect);
    bool isClipCheckSkip = NearEqual(visibleRect.Width(), 0.0) && NearEqual(visibleRect.Height(), 0.0) &&
                           NearEqual(visibleInnerRect.Width(), 0.0) && NearEqual(visibleInnerRect.Height(), 0.0);
    clipVisible = (GreatNotEqual(visibleInnerRect.Width(), 0.0) && GreatNotEqual(visibleInnerRect.Height(), 0.0)) ||
                  isClipCheckSkip;
    auto renderContext = frameNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto rect = renderContext->GetPaintRectWithoutTransform();
    if (NearEqual(rect.Width(), 0.0) && NearEqual(rect.Height(), 0.0)) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "Component %{public}s is not visible: width and height are both zero.",
                 std::to_string(frameNode->GetAccessibilityId()).c_str());
        clipVisible = false;
    }
    std::string parentPath;
    while (parent) {
        if (AceType::InstanceOf<NG::FrameNode>(parent)) {
            if (ProcessParentFrameNode(parent, parentPath, isAllAncestorAccessibilityVisible)) {
                break;
            }
        }
        parent = parent->GetParent();
    }
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "Complete parent path:current id %{public}" PRId64 " %{public}s",
        frameNode->GetAccessibilityId(), parentPath.c_str());

    bool nodeAccessibilityVisible =
        GetNodeAccessibilityVisible(frameNode, isAllAncestorAccessibilityVisible, clipVisible);
    if (!nodeAccessibilityVisible) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "Element %{public}" PRId64 " is invisible. isActive %{public}d, isVisible %{public}d"
            " isAllAncestorAccessibilityVisible:%{public}d clipVisible:%{public}d",
            frameNode->GetAccessibilityId(), frameNode->IsActive(), frameNode->IsVisible(),
            isAllAncestorAccessibilityVisible, clipVisible);
    }

    if (frameNode->GetTag() != V2::PAGE_ETS_TAG) {
        frameNode->SetAccessibilityVisible(nodeAccessibilityVisible);
    }
}

void SetRootAccessibilityVisible(const RefPtr<NG::UINode>& uiNode, AccessibilityElementInfo& nodeInfo)
{
    UpdateAccessibilityVisibleToRoot(uiNode);
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(uiNode);
    CHECK_NULL_VOID(frameNode);
    nodeInfo.SetAccessibilityVisible(frameNode->GetAccessibilityVisible());
}

void SetRootAccessibilityNextFocusId(const RefPtr<NG::UINode>& currentNode, const RefPtr<NG::FrameNode>& rootNode,
                                     AccessibilityElementInfo& nodeInfo)
{
    auto currentFrameNode = AceType::DynamicCast<NG::FrameNode>(currentNode);
    CHECK_NULL_VOID(currentFrameNode);
    auto accessibilityProperty = currentFrameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (accessibilityProperty->GetAccessibilityNextFocusInspectorKey() == "") {
        return;
    }

    FindCondition condition = [accessibilityProperty](const RefPtr<NG::FrameNode>& node) {
        return node->GetInspectorId() == accessibilityProperty->GetAccessibilityNextFocusInspectorKey();
    };
    auto nextNode = GetFramenodeByCondition(rootNode, condition);
    CHECK_NULL_VOID(nextNode);
    auto elementId = nextNode->GetAccessibilityId();
    if (nodeInfo.GetBelongTreeId() > 0) {
        AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(nodeInfo.GetBelongTreeId(), elementId);
    }
    nodeInfo.SetAccessibilityNextFocusId(elementId);
}

void SetRootAccessibilityPreFocusId(const RefPtr<NG::UINode>& currentNode, const RefPtr<NG::FrameNode>& rootNode,
                                    AccessibilityElementInfo& nodeInfo,
                                    const std::map<std::string, int64_t>& nextFocusIdMap)
{
    auto currentFrameNode = AceType::DynamicCast<NG::FrameNode>(currentNode);
    CHECK_NULL_VOID(currentFrameNode);

    auto currentInspectorId = currentFrameNode->GetInspectorId().value_or("");
    if (currentInspectorId == "") {
        return;
    }
    auto it = nextFocusIdMap.find(currentInspectorId);
    if (it != nextFocusIdMap.end()) {
        int64_t preAccessibilityId = it->second;
        nodeInfo.SetAccessibilityPreviousFocusId(preAccessibilityId);
    }
}

void RemoveEntriesWithPreAccessibilityId(std::map<std::string, int64_t>& nextFocusMap, int64_t preAccessibilityId)
{
    for (auto mapIt = nextFocusMap.begin(); mapIt != nextFocusMap.end();) {
        if (mapIt->second == preAccessibilityId) {
            mapIt = nextFocusMap.erase(mapIt);
        } else {
            ++mapIt;
        }
    }
}

void HandleExistingContext(std::map<int32_t, std::map<std::string, int64_t>>::iterator& it,
                           const std::string& nextFocusInspectorKey, int64_t preAccessibilityId)
{
    if (nextFocusInspectorKey.empty()) {
        RemoveEntriesWithPreAccessibilityId(it->second, preAccessibilityId);
    } else {
        auto nextFocusMap = it->second;
        it->second[nextFocusInspectorKey] = preAccessibilityId;
    }
}

int64_t GetParentId(const RefPtr<NG::UINode>& uiNode)
{
    if (AceType::InstanceOf<NG::FrameNode>(uiNode)) {
        if (AceType::DynamicCast<NG::FrameNode>(uiNode)->IsFirstVirtualNode()) {
            auto weakNode = AceType::DynamicCast<NG::FrameNode>(uiNode)->GetVirtualNodeParent();
            auto refNode = weakNode.Upgrade();
            return refNode == nullptr ? INVALID_PARENT_ID : refNode->GetAccessibilityId();
        }
    }
    auto parent = uiNode->GetParent();
    while (parent) {
        if (AceType::InstanceOf<NG::FrameNode>(parent)) {
            if ((parent->GetTag() == V2::PAGE_ETS_TAG) || (parent->GetTag() == V2::STAGE_ETS_TAG) ||
                AceType::DynamicCast<NG::FrameNode>(parent)->CheckAccessibilityLevelNo()) {
                parent = parent->GetParent();
                continue;
            }
            return parent->GetAccessibilityId();
        }
        parent = parent->GetParent();
    }
    return INVALID_PARENT_ID;
}

void FillElementInfo(int64_t elementId, AccessibilityElementInfo& elementInfo, const RefPtr<PipelineBase>& context,
    const RefPtr<JsAccessibilityManager>& jsAccessibilityManager, const FillEventInfoParam& param)
{
    int64_t elementIdUnwrap = elementId;
    int64_t uiextensionId = 0;
    std::list<AccessibilityElementInfo> elementInfos;
    int32_t mode = 0;
    CHECK_NULL_VOID(jsAccessibilityManager);
#ifdef WINDOW_SCENE_SUPPORTED
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    if (uiExtensionManager->IsWrapExtensionAbilityId(elementId)) {
        auto unWrapIdPair = uiExtensionManager->UnWrapExtensionAbilityId(
            NG::UI_EXTENSION_OFFSET_MAX, elementIdUnwrap);
        elementIdUnwrap = unWrapIdPair.second;
        uiextensionId = unWrapIdPair.first;
    }
#endif
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
        "start to search elementId: %{public}" PRId64, elementIdUnwrap);
    jsAccessibilityManager->SearchElementInfoByAccessibilityIdNG(
        elementIdUnwrap, mode, elementInfos, context, NG::UI_EXTENSION_OFFSET_MAX);
    if (elementInfos.empty()) {
        LOGE("Element infos is empty. Find element infos failed.");
        return;
    }
    elementInfo = elementInfos.front();
    if (uiextensionId > 0) {
        elementIdUnwrap = (uiextensionId * NG::UI_EXTENSION_OFFSET_MAX) + elementInfo.GetAccessibilityId();
        TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY, "elementIdUnwrap: %{public}" PRId64, elementIdUnwrap);
    }
    elementInfo.SetAccessibilityId(elementIdUnwrap);
    elementInfo.SetWindowId(param.windowId);
    jsAccessibilityManager->UpdateElementInfoInnerWindowId(elementInfo, elementId);
}

void FillEventInfo(const RefPtr<NG::FrameNode>& node,
                   AccessibilityEventInfo& eventInfo,
                   const RefPtr<PipelineBase>& context,
                   const RefPtr<JsAccessibilityManager>& jsAccessibilityManager,
                   const FillEventInfoParam& param)
{
    CHECK_NULL_VOID(node);
    eventInfo.SetComponentType(node->GetTag());
    eventInfo.SetPageId(node->GetPageId());
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    eventInfo.SetItemCounts(accessibilityProperty->GetCollectionItemCounts());
    eventInfo.SetBeginIndex(accessibilityProperty->GetBeginIndex());
    eventInfo.SetEndIndex(accessibilityProperty->GetEndIndex());
    AccessibilityElementInfo elementInfo;
    FillElementInfo(param.elementId, elementInfo, context, jsAccessibilityManager, param);
    if (param.stackNodeId != -1) {
        int64_t stackNodeId = param.stackNodeId;
        AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(elementInfo.GetBelongTreeId(), stackNodeId);
        elementInfo.SetNavDestinationId(stackNodeId);
    }
    eventInfo.AddContent(elementInfo.GetContent());
    eventInfo.SetElementInfo(elementInfo);
}
#ifdef WEB_SUPPORTED

void FillWebElementInfo(int64_t elementId, AccessibilityElementInfo& elementInfo,
                        const RefPtr<PipelineBase>& context,
                        const RefPtr<JsAccessibilityManager>& jsAccessibilityManager,
                        const FillEventInfoParam& param,
                        const RefPtr<NG::WebPattern>& webPattern)
{
    std::list<AccessibilityElementInfo> elementInfos;
    int32_t mode = 0;
    CHECK_NULL_VOID(jsAccessibilityManager);
    jsAccessibilityManager->SearchWebElementInfoByAccessibilityIdNG(
        elementId, mode, elementInfos, context, webPattern);
    if (elementInfos.empty()) {
        LOGE("Element infos is empty. Find element infos failed.");
        return;
    }
    elementInfo = elementInfos.front();
    elementInfo.SetWindowId(param.windowId);
}

void FillWebEventInfo(AccessibilityEventInfo& eventInfo,
                      const RefPtr<PipelineBase>& context,
                      const RefPtr<JsAccessibilityManager>& jsAccessibilityManager,
                      const FillEventInfoParam& param,
                      const RefPtr<NG::WebPattern>& webPattern)
{
    CHECK_NULL_VOID(webPattern);
    auto webNode = webPattern->GetHost();
    CHECK_NULL_VOID(webNode);
    eventInfo.SetPageId(webNode->GetPageId());
    AccessibilityElementInfo elementInfo;
    FillWebElementInfo(param.elementId, elementInfo, context, jsAccessibilityManager, param, webPattern);
    eventInfo.SetComponentType(elementInfo.GetComponentType());
    eventInfo.AddContent(elementInfo.GetContent());
    eventInfo.SetItemCounts(elementInfo.GetItemCounts());
    eventInfo.SetBeginIndex(elementInfo.GetBeginIndex());
    eventInfo.SetEndIndex(elementInfo.GetEndIndex());
    elementInfo.SetNavDestinationId(param.stackNodeId);
    eventInfo.SetElementInfo(elementInfo);
}
#endif

void FillEventInfo(const RefPtr<AccessibilityNode>& node, AccessibilityEventInfo& eventInfo)
{
    eventInfo.SetComponentType(node->GetTag());
    if (node->GetTag() == LIST_TAG) {
        eventInfo.SetItemCounts(node->GetListItemCounts());
        eventInfo.SetBeginIndex(node->GetListBeginIndex());
        eventInfo.SetEndIndex(node->GetListEndIndex());
    }
    eventInfo.SetPageId(node->GetPageId());
    eventInfo.AddContent(node->GetText());
    eventInfo.SetLatestContent(node->GetText());
}

inline bool IsPopupSupported(const RefPtr<NG::PipelineContext>& pipeline, int64_t nodeId)
{
    CHECK_NULL_RETURN(pipeline, false);
    auto overlayManager = pipeline->GetOverlayManager();
    if (overlayManager) {
        return overlayManager->HasPopupInfo(nodeId);
    }
    return false;
}

void SetAccessibilityFocusAction(AccessibilityElementInfo& nodeInfo, const char* tag)
{
    if (nodeInfo.HasAccessibilityFocus()) {
        AccessibleAction action(ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, tag);
        nodeInfo.AddAction(action);
    } else {
        AccessibleAction action(ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, tag);
        nodeInfo.AddAction(action);
    }
}

void UpdateSupportAction(const RefPtr<NG::FrameNode>& node, AccessibilityElementInfo& nodeInfo)
{
    CHECK_NULL_VOID(node);
    if (nodeInfo.IsFocusable()) {
        if (nodeInfo.IsFocused()) {
            AccessibleAction action(ACCESSIBILITY_ACTION_CLEAR_FOCUS, "ace");
            nodeInfo.AddAction(action);
        } else {
            AccessibleAction action(ACCESSIBILITY_ACTION_FOCUS, "ace");
            nodeInfo.AddAction(action);
        }
    }

    auto eventHub = node->GetOrCreateEventHub<NG::EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto gestureEventHub = eventHub->GetGestureEventHub();
    CHECK_NULL_VOID(gestureEventHub);
    nodeInfo.SetClickable(gestureEventHub->IsAccessibilityClickable());
    if (gestureEventHub->IsAccessibilityClickable()) {
        AccessibleAction action(ACCESSIBILITY_ACTION_CLICK, "ace");
        nodeInfo.AddAction(action);
    }
    nodeInfo.SetLongClickable(gestureEventHub->IsAccessibilityLongClickable());
    if (gestureEventHub->IsAccessibilityLongClickable()) {
        AccessibleAction action(ACCESSIBILITY_ACTION_LONG_CLICK, "ace");
        nodeInfo.AddAction(action);
    }
}

void UpdateUserAccessibilityElementInfo(
    const RefPtr<NG::AccessibilityProperty>& accessibilityProperty, AccessibilityElementInfo& nodeInfo)
{
    CHECK_NULL_VOID(accessibilityProperty);
    if (accessibilityProperty->HasUserDisabled()) {
        nodeInfo.SetEnabled(!accessibilityProperty->IsUserDisabled());
    }
    if (accessibilityProperty->HasUserCheckedType()) {
        nodeInfo.SetChecked(accessibilityProperty->GetUserCheckedType());
    } else {
        nodeInfo.SetChecked(accessibilityProperty->IsChecked());
    }
    if (accessibilityProperty->HasUserSelected()) {
        nodeInfo.SetSelected(accessibilityProperty->IsUserSelected());
    } else {
        nodeInfo.SetSelected(accessibilityProperty->IsSelected());
    }

    if (nodeInfo.IsEnabled()) {
        if (accessibilityProperty->HasUserCheckable()) {
            nodeInfo.SetCheckable(accessibilityProperty->IsUserCheckable());
        } else {
            nodeInfo.SetCheckable(accessibilityProperty->IsCheckable());
        }
    }
}

bool IsUserCheckedOrSelected(const RefPtr<NG::FrameNode> frameNode)
{
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, false);
    if (accessibilityProperty->HasUserCheckedType() || accessibilityProperty->HasUserSelected()) {
        return true;
    }
    return false;
}

void UpdateAccessibilityTextValueInfo(
    RefPtr<NG::AccessibilityProperty>& accessibilityProperty, AccessibilityElementInfo& nodeInfo)
{
    if (accessibilityProperty->HasUserTextValue()) {
        nodeInfo.SetContent(accessibilityProperty->GetUserTextValue());
    } else {
        nodeInfo.SetContent(accessibilityProperty->GetGroupText());
    }

    if (!accessibilityProperty->HasUserTextValue() && accessibilityProperty->GetAccessibilityText().empty() &&
        accessibilityProperty->IsAccessibilityGroup() && accessibilityProperty->IsAccessibilityTextPreferred()) {
        nodeInfo.SetAccessibilityText(accessibilityProperty->GetGroupPreferAccessibilityText());
    } else {
        nodeInfo.SetAccessibilityText(accessibilityProperty->GetAccessibilityText());
    }
    nodeInfo.SetOriginalText(accessibilityProperty->GetText());
}

void UpdateElementInfoPageIdWithTreeId(Accessibility::AccessibilityElementInfo& info, int32_t treeId)
{
    int32_t pageId = info.GetPageId();
    if ((pageId >= MAX_PAGE_ID_WITH_SUB_TREE) || (pageId < 0)) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "pageId %{public}d cannot set tree id", pageId);
    } else {
        uint32_t unsignedPageId = static_cast<uint32_t>(pageId);
        uint32_t unsignedTreeId = static_cast<uint32_t>(treeId);
        info.SetPageId((unsignedTreeId << SUB_TREE_OFFSET_IN_PAGE_ID) | unsignedPageId);
    }
}

bool ScrollByOffsetToParent(const RefPtr<NG::FrameNode>& curFrameNode, const RefPtr<NG::FrameNode>& parentFrameNode)
{
    CHECK_NULL_RETURN(curFrameNode, false);
    CHECK_NULL_RETURN(parentFrameNode, false);
    auto parentPattern = parentFrameNode->GetPattern<NG::ScrollablePattern>();
    CHECK_NULL_RETURN(parentPattern, false);

    auto scrollAbility = parentPattern->GetScrollOffsetAbility();
    auto scrollFunc = scrollAbility.scrollFunc;
    auto scrollAxis = scrollAbility.axis;
    if (!scrollFunc || scrollAxis == Axis::NONE) {
        return false;
    }
    if (parentFrameNode->GetTag() == V2::SCROLL_ETS_TAG) {
        return false;
    }
    NG::MoveOffsetParam param {
        scrollAxis == Axis::VERTICAL,
        scrollAbility.contentStartOffset,
        scrollAbility.contentEndOffset,
        true
    };
    auto moveOffset = NG::ScrollableUtils::GetMoveOffset(parentFrameNode, curFrameNode, param);
    if (!NearZero(moveOffset)) {
        TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY, "Scroll offset: %{public}f on %{public}s/%{public}d, axis: %{public}d",
            moveOffset, parentFrameNode->GetTag().c_str(), parentFrameNode->GetId(), scrollAxis);
        auto ret = scrollFunc(parentPattern->IsReverse() ? -moveOffset : moveOffset);
        auto pipeline = NG::PipelineContext::GetCurrentContextSafelyWithCheck();
        if (pipeline) {
            pipeline->FlushUITasks();
        }
        return ret;
    }
    return false;
}

bool ScrollByOffset(const RefPtr<NG::FrameNode>& curFrameNode)
{
    CHECK_NULL_RETURN(curFrameNode, false);
    bool ret = false;
    auto parentFrameNode = curFrameNode->GetParentFrameNode();

    while (parentFrameNode) {
        auto accessibilityProperty = parentFrameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        if (accessibilityProperty && !accessibilityProperty->IsUserScrollTriggerable()) {
            return false;
        }
        if (ScrollByOffsetToParent(curFrameNode, parentFrameNode)) {
            ret = true;
        }
        parentFrameNode = parentFrameNode->GetParentFrameNode();
    }
    return ret;
}

void ProcessFocusScroll(const RefPtr<NG::FrameNode>& curFrameNode, RefPtr<NG::PipelineContext>& context)
{
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [node = AceType::WeakClaim(AceType::RawPtr(curFrameNode))] {
            auto focusNode = node.Upgrade();
            CHECK_NULL_VOID(focusNode);
            auto accessibilityProperty = focusNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
            CHECK_NULL_VOID(accessibilityProperty);
            if (accessibilityProperty->GetAccessibilityFocusState()) {
                ScrollByOffset(focusNode);
            }
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityProcessFocusScroll");
}

int32_t GetLastPageId(const RefPtr<NG::PipelineContext>& ngPipeline)
{
    CHECK_NULL_RETURN(ngPipeline, -1);
    auto stageManager = ngPipeline->GetStageManager();
    CHECK_NULL_RETURN(stageManager, -1);
    auto page = stageManager->GetLastPageWithTransition();
    CHECK_NULL_RETURN(page, -1);
    return page->GetPageId();
}

void AddHasRegisteredHover(const RefPtr<NG::FrameNode>& node, ExtraElementInfo& extraElementInfo)
{
    CHECK_NULL_VOID(node);
    auto eventHub = node->GetOrCreateEventHub<NG::EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto inputEventHub = eventHub->GetInputEventHub();
    CHECK_NULL_VOID(inputEventHub);
    if (inputEventHub->HasAccessibilityHoverEvent()) {
        extraElementInfo.SetExtraElementInfo("hasRegisteredHover", true);
    }
}

void UpdateWebEmbedParent(std::list<AccessibilityElementInfo>& infos,
    const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty)
{
    if (infos.empty() || !commonProperty.checkGetFunc) {
        return;
    }
    auto webEmbedNodeId = commonProperty.checkGetFunc(node);
    CHECK_EQUAL_VOID(webEmbedNodeId, INVALID_NODE_ID);
    auto& frontElementInfo = infos.front();
    frontElementInfo.SetParent(webEmbedNodeId);
}
} // namespace

bool JsAccessibilityManager::IsTagInEmbedComponent(const std::string& tag)
{
    return TAGS_EMBED_COMPONENT.find(tag) != TAGS_EMBED_COMPONENT.end();
}

void JsAccessibilityManager::UpdateAccessibilityElementInfo(
    const RefPtr<NG::FrameNode>& node, AccessibilityElementInfo& nodeInfo)
{
    CHECK_NULL_VOID(node);
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (accessibilityProperty->HasAccessibilityRole()) {
        nodeInfo.SetComponentType(accessibilityProperty->GetAccessibilityRole());
    }
    if (accessibilityProperty->HasAccessibilityCustomRole()) {
        nodeInfo.SetCustomComponentType(accessibilityProperty->GetAccessibilityCustomRole());
    }

    UpdateAccessibilityTextValueInfo(accessibilityProperty, nodeInfo);

    RangeInfo rangeInfo = ConvertAccessibilityValue(accessibilityProperty->GetAccessibilityValue());
    if (accessibilityProperty->HasUserRangeCurrentValue()) {
        rangeInfo.SetCurrent(accessibilityProperty->GetUserRangeCurrentValue());
    }
    if (accessibilityProperty->HasUserRangeMinValue()) {
        rangeInfo.SetMin(accessibilityProperty->GetUserRangeMinValue());
    }
    if (accessibilityProperty->HasUserRangeMaxValue()) {
        rangeInfo.SetMax(accessibilityProperty->GetUserRangeMaxValue());
    }
    nodeInfo.SetRange(rangeInfo);
    if (accessibilityProperty->HasSubComponent()) {
        std::vector<SubComponentInfo> subComponentInfos;
        accessibilityProperty->GetSubComponentInfo(subComponentInfos);
        for (const auto& subComponent : subComponentInfos) {
            nodeInfo.AddSpan(SpanInfo(subComponent.spanId, subComponent.spanText,
                subComponent.accessibilityText, subComponent.accessibilityDescription,
                subComponent.accessibilityLevel));
        }
    }
    nodeInfo.SetAccessibilityNextFocusInspectorKey(accessibilityProperty->GetAccessibilityNextFocusInspectorKey());
    nodeInfo.SetAccessibilityScrollable(accessibilityProperty->IsUserScrollTriggerable());
    nodeInfo.SetHint(accessibilityProperty->GetHintText());
    nodeInfo.SetAccessibilityGroup(accessibilityProperty->IsAccessibilityGroup());
    nodeInfo.SetAccessibilityLevel(accessibilityProperty->GetAccessibilityLevel());
    nodeInfo.SetTextType(accessibilityProperty->GetTextType());
    nodeInfo.SetTextLengthLimit(accessibilityProperty->GetTextLengthLimit());
    nodeInfo.SetOffset(accessibilityProperty->GetScrollOffSet());
    auto context = node->GetRenderContext();
    if (context != nullptr) {
        nodeInfo.SetZIndex(context->GetZIndex().value_or(0));
        nodeInfo.SetOpacity(context->GetOpacity().value_or(1));
        nodeInfo.SetBackgroundColor(context->GetBackgroundColor().value_or(Color::TRANSPARENT).ToString());
        nodeInfo.SetBackgroundImage(context->GetBackgroundImage().value_or(ImageSourceInfo("")).ToString());
        if (context->GetForeground() != nullptr) {
            nodeInfo.SetBlur(context->GetForeground()->propBlurRadius.value_or(Dimension(0)).ToString());
        }
    }
    auto eventHub = node->GetOrCreateEventHub<NG::EventHub>();
    if (eventHub != nullptr) {
        nodeInfo.SetHitTestBehavior(NG::GestureEventHub::GetHitTestModeStr(eventHub->GetGestureEventHub()));
    }

    UpdateUserAccessibilityElementInfo(accessibilityProperty, nodeInfo);

    nodeInfo.SetPassword(accessibilityProperty->IsPassword());
    nodeInfo.SetPluraLineSupported(accessibilityProperty->IsMultiLine());
    nodeInfo.SetHinting(accessibilityProperty->IsHint());
    nodeInfo.SetDescriptionInfo(accessibilityProperty->GetAccessibilityDescription());
    if (accessibilityProperty->HasUserCurrentValue()) {
        nodeInfo.SetCurrentIndex(accessibilityProperty->GetUserCurrentValue());
    } else {
        nodeInfo.SetCurrentIndex(accessibilityProperty->GetCurrentIndex());
    }
    if (accessibilityProperty->HasUserMinValue()) {
        nodeInfo.SetBeginIndex(accessibilityProperty->GetUserMinValue());
    } else {
        nodeInfo.SetBeginIndex(accessibilityProperty->GetBeginIndex());
    }
    if (accessibilityProperty->HasUserMaxValue()) {
        nodeInfo.SetEndIndex(accessibilityProperty->GetUserMaxValue());
    } else {
        nodeInfo.SetEndIndex(accessibilityProperty->GetEndIndex());
    }
    auto tag = node->GetTag();
    if (tag == V2::TOAST_ETS_TAG || tag == V2::POPUP_ETS_TAG || tag == V2::DIALOG_ETS_TAG ||
        tag == V2::ACTION_SHEET_DIALOG_ETS_TAG || tag == V2::ALERT_DIALOG_ETS_TAG || tag == V2::MENU_ETS_TAG ||
        tag == "SelectMenu") {
        nodeInfo.SetLiveRegion(1);
    }
    nodeInfo.SetContentInvalid(accessibilityProperty->GetContentInvalid());
    nodeInfo.SetError(accessibilityProperty->GetErrorText());
    nodeInfo.SetSelectedBegin(accessibilityProperty->GetTextSelectionStart());
    nodeInfo.SetSelectedEnd(accessibilityProperty->GetTextSelectionEnd());
    nodeInfo.SetInputType(static_cast<int>(accessibilityProperty->GetTextInputType()));
    nodeInfo.SetItemCounts(accessibilityProperty->GetCollectionItemCounts());
    nodeInfo.SetChildTreeIdAndWinId(
        accessibilityProperty->GetChildTreeId(), accessibilityProperty->GetChildWindowId());
    if (nodeInfo.GetComponentType() == "FormComponent") {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "elementId: %{public}" PRId64 ", Set: %{public}d %{public}d",
            nodeInfo.GetAccessibilityId(), accessibilityProperty->GetChildTreeId(),
            accessibilityProperty->GetChildWindowId());
    }
    if (nodeInfo.GetWindowId() == static_cast<int32_t>(windowId_)) {
        nodeInfo.SetBelongTreeId(treeId_);
        nodeInfo.SetParentWindowId(parentWindowId_);
    } else {
        nodeInfo.SetBelongTreeId(0);
        nodeInfo.SetParentWindowId(0);
    }

    GridInfo gridInfo(accessibilityProperty->GetCollectionInfo().rows,
        accessibilityProperty->GetCollectionInfo().columns, accessibilityProperty->GetCollectionInfo().selectMode);
    nodeInfo.SetGrid(gridInfo);
    ExtraElementInfo extraElementInfo {};
    accessibilityProperty->GetAllExtraElementInfo(extraElementInfo);
    AddHasRegisteredHover(node, extraElementInfo);
    if (node->IsAccessibilityVirtualNode()) {
        auto parentUinode = node->GetVirtualNodeParent().Upgrade();
        if (parentUinode) {
            auto parentFrame = AceType::DynamicCast<NG::FrameNode>(parentUinode);
            AddHasRegisteredHover(parentFrame, extraElementInfo);
        }
    }
    nodeInfo.SetExtraElement(extraElementInfo);

    int32_t row = accessibilityProperty->GetCollectionItemInfo().row;
    int32_t column = accessibilityProperty->GetCollectionItemInfo().column;
    int32_t rowSpan = accessibilityProperty->GetCollectionItemInfo().rowSpan;
    int32_t columnSpan = accessibilityProperty->GetCollectionItemInfo().columnSpan;
    bool heading = accessibilityProperty->GetCollectionItemInfo().heading;
    GridItemInfo gridItemInfo(row, rowSpan, column, columnSpan, heading, nodeInfo.IsSelected());
    nodeInfo.SetGridItem(gridItemInfo);

    SetAccessibilityFocusAction(nodeInfo, "ace");
    if (nodeInfo.IsEnabled()) {
        nodeInfo.SetScrollable(accessibilityProperty->IsScrollable());
        nodeInfo.SetEditable(accessibilityProperty->IsEditable());
        nodeInfo.SetDeletable(accessibilityProperty->IsDeletable());
        UpdateSupportAction(node, nodeInfo);
        accessibilityProperty->ResetSupportAction();
        auto supportAceActions = accessibilityProperty->GetSupportAction();
        for (auto it = supportAceActions.begin(); it != supportAceActions.end(); ++it) {
            AccessibleAction action(ConvertAceAction(*it), "ace");
            nodeInfo.AddAction(action);
        }
    }
}
#ifdef WEB_SUPPORTED

void JsAccessibilityManager::UpdateWebAccessibilityElementInfo(
    const std::shared_ptr<NG::TransitionalNodeInfo>& node, AccessibilityElementInfo& nodeInfo, int32_t treeId)
{
    CHECK_NULL_VOID(node);
    nodeInfo.SetContent(node->GetContent());
    nodeInfo.SetOriginalText(node->GetContent());
    nodeInfo.SetAccessibilityText(node->GetContent());
    RangeInfo rangeInfo(node->GetRangeInfoMin(), node->GetRangeInfoMax(), node->GetRangeInfoCurrent());
    nodeInfo.SetRange(rangeInfo);
    nodeInfo.SetHint(node->GetHint());
    nodeInfo.SetHinting(node->GetIsHinting());
    nodeInfo.SetDescriptionInfo(node->GetDescriptionInfo());
    nodeInfo.SetChecked(node->GetIsChecked());
    nodeInfo.SetSelected(node->GetIsSelected());
    nodeInfo.SetPassword(node->GetIsPassword());
    nodeInfo.SetPluraLineSupported(node->GetIsPluralLineSupported());
    nodeInfo.SetLiveRegion(node->GetLiveRegion());
    nodeInfo.SetContentInvalid(node->GetIsContentInvalid());
    nodeInfo.SetError(node->GetError());
    nodeInfo.SetSelectedBegin(node->GetSelectionStart());
    nodeInfo.SetSelectedEnd(node->GetSelectionEnd());
    nodeInfo.SetInputType(node->GetInputType());
    nodeInfo.SetItemCounts(node->GetItemCounts());

    nodeInfo.SetBelongTreeId(treeId);
    nodeInfo.SetParentWindowId(parentWebWindowId_);
    GridInfo gridInfo(node->GetGridRows(), node->GetGridColumns(), node->GetGridSelectedMode());
    nodeInfo.SetGrid(gridInfo);

    int32_t row = node->GetGridItemRow();
    int32_t column = node->GetGridItemColumn();
    int32_t rowSpan = node->GetGridItemRowSpan();
    int32_t columnSpan = node->GetGridItemColumnSpan();
    bool heading = node->GetIsHeading();
    GridItemInfo gridItemInfo(row, rowSpan, column, columnSpan, heading, nodeInfo.IsSelected());
    nodeInfo.SetGridItem(gridItemInfo);

    SetAccessibilityFocusAction(nodeInfo, "web");
    nodeInfo.SetCheckable(node->GetIsCheckable());
    nodeInfo.SetScrollable(node->GetIsScrollable());
    nodeInfo.SetEditable(node->GetIsEditable());
    nodeInfo.SetDeletable(node->GetIsDeletable());
    nodeInfo.SetClickable(node->GetIsClickable());
    auto supportAceActions = node->GetActions();
    for (auto it = supportAceActions.begin(); it != supportAceActions.end(); ++it) {
        AccessibleAction action(ConvertAceAction(static_cast<AceAction>(*it)), "web");
        nodeInfo.AddAction(action);
    }
    nodeInfo.SetAccessibilityGroup(node->GetIsAccessibilityGroup());
    ExtraElementInfo extraElementInfo {};
    extraElementInfo.SetExtraElementInfo("componentTypeDescription", node->GetComponentTypeDescription());
    extraElementInfo.SetExtraElementInfo("CheckboxGroupSelectedStatus", node->GetCheckboxGroupSelectedStatus());
    extraElementInfo.SetExtraElementInfo("expandedState", node->GetExpandedState());
    nodeInfo.SetExtraElement(extraElementInfo);
}

#endif
namespace {
void UpdateChildrenOfAccessibilityElementInfo(
    const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty, AccessibilityElementInfo& nodeInfo)
{
    if (!NeedUpdateChildrenOfAccessibilityElementInfo(node)) {
        TAG_LOGW(AceLogTag::ACE_ACCESSIBILITY, "Node %{public}s not need update children"
            " of accessibilityElementInfo", node->GetTag().c_str());
        return;
    }
    if (!IsExtensionComponent(node) || IsUIExtensionShowPlaceholder(node)) {
        std::vector<std::pair<int64_t, int32_t>> childrenIdInfo;
        for (const auto& item : node->GetChildren(true)) {
            GetFrameNodeChildren(item, childrenIdInfo, commonProperty);
        }

        auto overlayNode = node->GetOverlayNode();
        if (overlayNode != nullptr) {
            GetFrameNodeChildren(overlayNode, childrenIdInfo, commonProperty);
        }

        auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
        auto uiVirtualNode = accessibilityProperty->GetAccessibilityVirtualNode();
        if (uiVirtualNode != nullptr) {
            auto virtualNode = AceType::DynamicCast<NG::FrameNode>(uiVirtualNode);
            if (virtualNode != nullptr) {
                childrenIdInfo.clear();
                GetFrameNodeChildren(virtualNode, childrenIdInfo, commonProperty);
            }
        }
        std::sort(childrenIdInfo.begin(), childrenIdInfo.end(),
            [](const auto&zIndexA, const auto&zIndexB) { return zIndexA.second < zIndexB.second; });
        for (const auto& childrenIdPair : childrenIdInfo) {
            nodeInfo.AddChild(childrenIdPair.first);
        }
    }
}

}

void JsAccessibilityManager::UpdateVirtualNodeChildAccessibilityElementInfo(
    const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty,
    AccessibilityElementInfo& nodeParentInfo, AccessibilityElementInfo& nodeInfo,
    const RefPtr<NG::PipelineContext>& ngPipeline)
{
    CHECK_NULL_VOID(node);
    nodeInfo.SetParent(GetParentId(node));
    UpdateChildrenOfAccessibilityElementInfo(node, commonProperty, nodeInfo);

    nodeInfo.SetAccessibilityId(node->GetAccessibilityId());
    nodeInfo.SetComponentType(node->GetTag());
    nodeInfo.SetUniqueId(node->GetId());

    nodeInfo.SetEnabled(node->GetFocusHub() ? node->GetFocusHub()->IsEnabled() : true);
    nodeInfo.SetFocused(node->GetFocusHub() ? node->GetFocusHub()->IsCurrentFocus() : false);
    nodeInfo.SetAccessibilityFocus(node->GetRenderContext()->GetAccessibilityFocus().value_or(false));
    nodeInfo.SetInspectorKey(node->GetInspectorId().value_or(""));
    nodeInfo.SetVisible(node->IsVisible());
    if (node->IsVisible()) {
        auto rect = node->GetVirtualNodeTransformRectRelativeToWindow();
        auto left = static_cast<int32_t>(rect.Left() + commonProperty.windowLeft);
        auto top = static_cast<int32_t>(rect.Top() + commonProperty.windowTop);
        auto right = static_cast<int32_t>(rect.Right() + commonProperty.windowLeft);
        auto bottom = static_cast<int32_t>(rect.Bottom() + commonProperty.windowTop);
        Accessibility::Rect bounds { left, top, right, bottom };
        nodeInfo.SetRectInScreen(bounds);
    }
    nodeInfo.SetWindowId(commonProperty.windowId);
    nodeInfo.SetInnerWindowId(commonProperty.innerWindowId);
    nodeInfo.SetPageId(node->GetPageId());
    nodeInfo.SetPagePath(
        GetPagePathInPageNodes(nodeInfo.GetPageId(), commonProperty.pageNodes, commonProperty.pagePaths));
    nodeInfo.SetBundleName(Container::CurrentBundleName());

    if (nodeInfo.IsEnabled()) {
        nodeInfo.SetFocusable(node->GetFocusHub() ? node->GetFocusHub()->IsFocusable() : false);
        nodeInfo.SetPopupSupported(IsPopupSupported(ngPipeline, node->GetId()));
    }
    nodeInfo.SetComponentResourceId(node->GetInspectorId().value_or(""));
    UpdateAccessibilityElementInfo(node, nodeInfo);
}

void JsAccessibilityManager::UpdateVirtualNodeAccessibilityElementInfo(
    const RefPtr<NG::FrameNode>& parent, const RefPtr<NG::FrameNode>& node,
    const CommonProperty& commonProperty, AccessibilityElementInfo& nodeInfo,
    const RefPtr<NG::PipelineContext>& ngPipeline)
{
    CHECK_NULL_VOID(parent);
    CHECK_NULL_VOID(node);
    nodeInfo.SetParent(GetParentId(node));
    UpdateChildrenOfAccessibilityElementInfo(node, commonProperty, nodeInfo);

    nodeInfo.SetAccessibilityId(node->GetAccessibilityId());
    nodeInfo.SetComponentType(node->GetTag());
    nodeInfo.SetUniqueId(node->GetId());

    nodeInfo.SetEnabled(node->GetFocusHub() ? node->GetFocusHub()->IsEnabled() : true);
    nodeInfo.SetFocused(node->GetFocusHub() ? node->GetFocusHub()->IsCurrentFocus() : false);
    nodeInfo.SetAccessibilityFocus(node->GetRenderContext()->GetAccessibilityFocus().value_or(false));
    nodeInfo.SetInspectorKey(node->GetInspectorId().value_or(""));
    nodeInfo.SetVisible(node->IsVisible());
    if (node->IsVisible()) {
        auto virtualNodeRect = node->GetTransformRectRelativeToWindow();
        auto parentRect = parent->GetTransformRectRelativeToWindow();
        auto left = static_cast<int32_t>(parentRect.Left() + commonProperty.windowLeft);
        auto top = static_cast<int32_t>(parentRect.Top() + commonProperty.windowTop);
        auto right = static_cast<int32_t>(parentRect.Left() + virtualNodeRect.Width() + commonProperty.windowLeft);
        if (virtualNodeRect.Width() > (parentRect.Right() - parentRect.Left())) {
            right = static_cast<int32_t>(parentRect.Right() + commonProperty.windowLeft);
        }
        auto bottom = static_cast<int32_t>(parentRect.Top() + virtualNodeRect.Height() + commonProperty.windowTop);
        if (virtualNodeRect.Height() > (parentRect.Bottom() - parentRect.Top())) {
            bottom = static_cast<int32_t>(parentRect.Bottom() + commonProperty.windowTop);
        }
        Accessibility::Rect bounds { left, top, right, bottom };
        nodeInfo.SetRectInScreen(bounds);
    }
    nodeInfo.SetWindowId(commonProperty.windowId);
    nodeInfo.SetInnerWindowId(commonProperty.innerWindowId);
    nodeInfo.SetPageId(node->GetPageId());
    nodeInfo.SetPagePath(
        GetPagePathInPageNodes(nodeInfo.GetPageId(), commonProperty.pageNodes, commonProperty.pagePaths));
    nodeInfo.SetBundleName(Container::CurrentBundleName());

    if (nodeInfo.IsEnabled()) {
        nodeInfo.SetFocusable(node->GetFocusHub() ? node->GetFocusHub()->IsFocusable() : false);
        nodeInfo.SetPopupSupported(IsPopupSupported(ngPipeline, node->GetId()));
    }
    nodeInfo.SetComponentResourceId(node->GetInspectorId().value_or(""));
    UpdateAccessibilityElementInfo(node, nodeInfo);
}

namespace {
void GetPositionToWindowWithTransform(NG::OffsetF& offset, NG::OffsetF& offsetBottom, const RefPtr<NG::FrameNode>& node)
{
    CHECK_NULL_VOID(node);
    auto context = node->GetRenderContext();
    CHECK_NULL_VOID(context);
    auto rect = context->GetPaintRectWithoutTransform();

    offset = rect.GetOffset();
    offsetBottom = NG::OffsetF(rect.GetX() + rect.Width(), rect.GetY() + rect.Height());

    NG::PointF leftTopPointNode(offset.GetX(), offset.GetY());
    NG::PointF rightBottomPointNode(rect.GetX() + rect.Width(), rect.GetY() + rect.Height());
    context->GetPointTransformRotate(leftTopPointNode);
    context->GetPointTransformRotate(rightBottomPointNode);
    auto parent = node->GetAncestorNodeOfFrame(true);
    while (parent) {
        // if get the window boundary (window scene), not calculate transform
        if (!parent->IsWindowBoundary()) {
            auto parentRenderContext = parent->GetRenderContext();
            if (!parentRenderContext) {
                parent = parent->GetAncestorNodeOfFrame(true);
                continue;
            }
            auto parentOffset = parentRenderContext->GetPaintRectWithoutTransform().GetOffset();
            NG::PointF leftTopPointAfterAddOffset(parentOffset.GetX() + leftTopPointNode.GetX(),
                                            parentOffset.GetY() + leftTopPointNode.GetY());
            NG::PointF rightBottomPointAfterAddOffset(parentOffset.GetX() + rightBottomPointNode.GetX(),
                                                parentOffset.GetY() + rightBottomPointNode.GetY());

            auto parentTransformMat = parentRenderContext->GetMatrixWithTransformRotate();
            Point leftTop(leftTopPointAfterAddOffset.GetX(), leftTopPointAfterAddOffset.GetY());
            Point rightBottom(rightBottomPointAfterAddOffset.GetX(), rightBottomPointAfterAddOffset.GetY());
            auto leftTopPoint = parentTransformMat * leftTop;
            auto rightBottomPoint = parentTransformMat * rightBottom;
            leftTopPointNode.SetX(leftTopPoint.GetX());
            leftTopPointNode.SetY(leftTopPoint.GetY());
            rightBottomPointNode.SetX(rightBottomPoint.GetX());
            rightBottomPointNode.SetY(rightBottomPoint.GetY());
        }
        parent = parent->GetAncestorNodeOfFrame(true);
    }
    offset.SetX(leftTopPointNode.GetX());
    offset.SetY(leftTopPointNode.GetY());
    offsetBottom.SetX(rightBottomPointNode.GetX());
    offsetBottom.SetY(rightBottomPointNode.GetY());
}

NG::RectF GetFinalRealRect(const RefPtr<NG::FrameNode>& node)
{
    if ((node->GetTag() == V2::WINDOW_SCENE_ETS_TAG) && node->IsWindowBoundary()) {
        auto renderContext = node->GetRenderContext();
        CHECK_NULL_RETURN(renderContext, NG::RectF());
        auto rect = renderContext->GetPaintRectWithoutTransform();
        // commonproperty will contain the offset and scale of the window scene
        return {0, 0, rect.Width(), rect.Height()};
    }

    NG::OffsetF offset;
    NG::OffsetF offsetBottom;
    GetPositionToWindowWithTransform(offset, offsetBottom, node);
    return {
        LessNotEqual(offset.GetX(), offsetBottom.GetX()) ? offset.GetX() : offsetBottom.GetX(),
        LessNotEqual(offset.GetY(), offsetBottom.GetY()) ? offset.GetY() : offsetBottom.GetY(),
        LessNotEqual(offset.GetX(), offsetBottom.GetX())
            ? offsetBottom.GetX() - offset.GetX()
            : offset.GetX() - offsetBottom.GetX(),
        LessNotEqual(offset.GetY(), offsetBottom.GetY())
            ? offsetBottom.GetY() - offset.GetY()
            : offset.GetY() - offsetBottom.GetY()
        };
}

void SetRectInScreen(const RefPtr<NG::FrameNode>& node, AccessibilityElementInfo& nodeInfo,
    const CommonProperty& commonProperty)
{
    if (node->IsAccessibilityVirtualNode()) {
        auto rect = node->GetVirtualNodeTransformRectRelativeToWindow();
        auto left = static_cast<int32_t>(rect.Left() + commonProperty.windowLeft);
        auto top = static_cast<int32_t>(rect.Top() + commonProperty.windowTop);
        auto right = static_cast<int32_t>(rect.Right() + commonProperty.windowLeft);
        auto bottom = static_cast<int32_t>(rect.Bottom() + commonProperty.windowTop);
        Accessibility::Rect bounds { left, top, right, bottom };
        nodeInfo.SetRectInScreen(bounds);
    } else if (node->IsVisible()) {
        auto rect = GetFinalRealRect(node);
        auto rotateTransformData = commonProperty.rotateTransform;
        auto currentDegree = rotateTransformData.rotateDegree;
        if (!NearZero(currentDegree, 0)) {
            AccessibilityRect rotateRect(rect.GetX(), rect.GetY(), rect.Width(), rect.Height());
            rotateRect.Rotate(rotateTransformData.innerCenterX, rotateTransformData.innerCenterY, currentDegree);
            rotateRect.ApplyTransformation(rotateTransformData, commonProperty.scaleX, commonProperty.scaleY);
            Accessibility::Rect bounds { static_cast<int32_t>(rotateRect.GetX()),
                static_cast<int32_t>(rotateRect.GetY()),
                static_cast<int32_t>(rotateRect.GetX() + rotateRect.GetWidth()),
                static_cast<int32_t>(rotateRect.GetY() + rotateRect.GetHeight()) };
            nodeInfo.SetRectInScreen(bounds);
            return;
        }
        if (!NearZero(commonProperty.scaleX, 0) && !NearZero(commonProperty.scaleY, 0)) {
            rect.SetRect(rect.GetX() * commonProperty.scaleX, rect.GetY() * commonProperty.scaleY,
                rect.Width() * commonProperty.scaleX, rect.Height() * commonProperty.scaleY);
        }
        auto left = static_cast<int32_t>(rect.Left() + commonProperty.windowLeft);
        auto top = static_cast<int32_t>(rect.Top() + commonProperty.windowTop);
        auto right = static_cast<int32_t>(rect.Right() + commonProperty.windowLeft);
        auto bottom = static_cast<int32_t>(rect.Bottom() + commonProperty.windowTop);
        Accessibility::Rect bounds { left, top, right, bottom };
        nodeInfo.SetRectInScreen(bounds);
    }
}
}

void JsAccessibilityManager::UpdateAccessibilityVisible(
    const RefPtr<NG::FrameNode>& node, AccessibilityElementInfo& nodeInfo)
{
    auto parentNode = node->GetParentFrameNode();
    UpdateElementInfoTreeId(nodeInfo);

    if (!parentNode) {
        if (node->GetTag() != V2::PAGE_ETS_TAG) {
            node->SetAccessibilityVisible(node->IsActive() && node->IsVisible());
        }
    } else {
        if (node->GetTag() == V2::PAGE_ETS_TAG) {
            nodeInfo.SetAccessibilityVisible(node->IsActive() && node->IsVisible() && node->GetAccessibilityVisible() &&
                                             parentNode->GetAccessibilityVisible());
            return;
        }
        auto nodeAccessibilityVisible = node->IsActive() && node->IsVisible() && parentNode->GetAccessibilityVisible();
        node->SetAccessibilityVisible(nodeAccessibilityVisible);
        if (!nodeAccessibilityVisible) {
            TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
                "Element %{public}" PRId64 " is invisible. node isActive %{public}d, node isVisible %{public}d"
                "parent accessibilityVisible:%{public}d.parent id %{public}" PRId64,
                nodeInfo.GetAccessibilityId(), node->IsActive(), node->IsVisible(),
                parentNode->GetAccessibilityVisible(), parentNode->GetAccessibilityId());
        }
    }
    nodeInfo.SetAccessibilityVisible(node->GetAccessibilityVisible());
}

void JsAccessibilityManager::UpdateAccessibilityElementInfo(
    const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty,
    AccessibilityElementInfo& nodeInfo, const RefPtr<NG::PipelineContext>& ngPipeline)
{
    CHECK_NULL_VOID(node);
    nodeInfo.SetParent(GetParentId(node));
    UpdateChildrenOfAccessibilityElementInfo(node, commonProperty, nodeInfo);
    nodeInfo.SetAccessibilityId(node->GetAccessibilityId());
    nodeInfo.SetComponentType(node->GetTag());
    nodeInfo.SetUniqueId(node->GetId());

    nodeInfo.SetEnabled(node->GetFocusHub() ? node->GetFocusHub()->IsEnabled() : true);
    nodeInfo.SetFocused(node->GetFocusHub() ? node->GetFocusHub()->IsCurrentFocus() : false);
    nodeInfo.SetAccessibilityFocus(node->GetRenderContext()->GetAccessibilityFocus().value_or(false));
    nodeInfo.SetClip(node->GetRenderContext()->GetClipEdge().value_or(false));
    nodeInfo.SetInspectorKey(node->GetInspectorId().value_or(""));
    nodeInfo.SetVisible(node->IsVisible());
    nodeInfo.SetIsActive(node->IsActive());
    SetRectInScreen(node, nodeInfo, commonProperty);
    nodeInfo.SetWindowId(commonProperty.windowId);
    nodeInfo.SetInnerWindowId(commonProperty.innerWindowId);
    // is abnormal that pageId equals to 0, use last pageId to fix pageId
    if (node->GetPageId()) {
        nodeInfo.SetPageId(node->GetPageId());
    } else {
        nodeInfo.SetPageId(GetLastPageId(ngPipeline));
    }
    nodeInfo.SetPagePath(
        GetPagePathInPageNodes(nodeInfo.GetPageId(), commonProperty.pageNodes, commonProperty.pagePaths));
    nodeInfo.SetBundleName(Container::CurrentBundleName());

    if (nodeInfo.IsEnabled()) {
        nodeInfo.SetFocusable(node->GetFocusHub() ? node->GetFocusHub()->IsFocusable() : false);
        nodeInfo.SetPopupSupported(IsPopupSupported(ngPipeline, node->GetId()));
    }
    nodeInfo.SetComponentResourceId(node->GetInspectorId().value_or(""));
    UpdateAccessibilityElementInfo(node, nodeInfo);
    UpdateAccessibilityVisible(node, nodeInfo);
}
#ifdef WEB_SUPPORTED

void JsAccessibilityManager::WebSetScreenRect(const std::shared_ptr<NG::TransitionalNodeInfo>& node,
    const CommonProperty& commonProperty, AccessibilityElementInfo& nodeInfo)
{
    auto rotateTransformData = commonProperty.rotateTransform;
    auto currentDegree = rotateTransformData.rotateDegree;
    if (!NearZero(currentDegree, 0)) {
        AccessibilityRect rotateRect(node->GetRectX(), node->GetRectY(),
            node->GetRectWidth(), node->GetRectHeight());
        rotateRect.Rotate(rotateTransformData.innerCenterX, rotateTransformData.innerCenterY, currentDegree);
        rotateRect.ApplyTransformation(rotateTransformData, commonProperty.scaleX, commonProperty.scaleY);
        Accessibility::Rect bounds { static_cast<int32_t>(std::floor(rotateRect.GetX())),
            static_cast<int32_t>(std::floor(rotateRect.GetY())),
            static_cast<int32_t>(std::floor(rotateRect.GetX() + rotateRect.GetWidth())),
            static_cast<int32_t>(std::floor(rotateRect.GetY() + rotateRect.GetHeight())) };
        nodeInfo.SetRectInScreen(bounds);
    } else {
        NG::RectT<int32_t> rectInt {
            node->GetRectX(),
            node->GetRectY(),
            node->GetRectWidth(),
            node->GetRectHeight()
        };
        if (!NearZero(commonProperty.scaleX, 1.0f) || !NearZero(commonProperty.scaleY, 1.0f)) {
            rectInt.SetRect(static_cast<int32_t>(std::floor(rectInt.GetX() * commonProperty.scaleX)),
                static_cast<int32_t>(std::floor(rectInt.GetY() * commonProperty.scaleY)),
                static_cast<int32_t>(std::floor(rectInt.Width() * commonProperty.scaleX)),
                static_cast<int32_t>(std::floor(rectInt.Height() * commonProperty.scaleY)));
        }
        auto left = static_cast<int32_t>(rectInt.Left() + commonProperty.windowLeft);
        auto top = static_cast<int32_t>(rectInt.Top() + commonProperty.windowTop);
        auto right = static_cast<int32_t>(rectInt.Right() + commonProperty.windowLeft);
        auto bottom = static_cast<int32_t>(rectInt.Bottom() + commonProperty.windowTop);
        Accessibility::Rect bounds { left, top, right, bottom };
        nodeInfo.SetRectInScreen(bounds);
    }
}

void JsAccessibilityManager::UpdateWebAccessibilityElementInfo(
    const std::shared_ptr<NG::TransitionalNodeInfo>& node, const CommonProperty& commonProperty,
    AccessibilityElementInfo& nodeInfo, const RefPtr<NG::WebPattern>& webPattern)
{
    if (node->GetParentId() == -1) {
        nodeInfo.SetParent(INVALID_PARENT_ID);
    } else {
        nodeInfo.SetParent(node->GetParentId());
    }
    for (const auto& child : node->GetChildIds()) {
        nodeInfo.AddChild(child);
    }

    nodeInfo.SetAccessibilityId(node->GetAccessibilityId());
    nodeInfo.SetComponentType(node->GetComponentType());
    if (IsTagInEmbedComponent(nodeInfo.GetComponentType())) {
        nodeInfo.SetEnabled(true);
    } else {
        nodeInfo.SetEnabled(node->GetIsEnabled());
    }
    nodeInfo.SetFocused(node->GetIsFocused());
    nodeInfo.SetAccessibilityFocus(node->GetIsAccessibilityFocus());
    nodeInfo.SetVisible(node->GetIsVisible());

    if (node->GetIsVisible()) {
        CHECK_NULL_VOID(webPattern);
        auto webNode = webPattern->GetHost();
        CHECK_NULL_VOID(webNode);
        WebSetScreenRect(node, commonProperty, nodeInfo);
    }

    nodeInfo.SetWindowId(commonProperty.windowId);
    nodeInfo.SetPageId(node->GetPageId());
    nodeInfo.SetPagePath(
        GetPagePathInPageNodes(nodeInfo.GetPageId(), commonProperty.pageNodes, commonProperty.pagePaths));
    nodeInfo.SetBundleName(Container::CurrentBundleName());

    if (nodeInfo.IsEnabled()) {
        nodeInfo.SetFocusable(node->GetIsFocusable());
        nodeInfo.SetPopupSupported(node->GetIsPopupSupported());
    }
    CHECK_NULL_VOID(webPattern);
    UpdateWebAccessibilityElementInfo(node, nodeInfo, webPattern->GetTreeId());
}

#endif
namespace {
std::list<AccessibilityElementInfo> SearchExtensionElementInfoByAccessibilityIdNG(
    int64_t elementId, int32_t mode, const RefPtr<NG::FrameNode>& node,  int64_t offset)
{
    std::list<AccessibilityElementInfo> extensionElementInfo;
    if (NG::UI_EXTENSION_OFFSET_MIN < (offset + 1)) {
        node->SearchExtensionElementInfoByAccessibilityIdNG(elementId, mode,
            offset / NG::UI_EXTENSION_ID_FACTOR, extensionElementInfo);
    }
    return extensionElementInfo;
}

void UpdateUiExtensionParentIdForFocus(const RefPtr<NG::FrameNode>& rootNode, const int64_t uiExtensionOffset,
    Accessibility::AccessibilityElementInfo& info)
{
    if ((uiExtensionOffset != NG::UI_EXTENSION_OFFSET_MAX) && (info.GetComponentType() != V2::ROOT_ETS_TAG) &&
        (info.GetParentNodeId() == rootNode->GetAccessibilityId())) {
            info.SetParent(NG::UI_EXTENSION_ROOT_ID);
    }
}

void GetChildrenFromFrameNode(
    const RefPtr<NG::FrameNode>& node,
    std::list<RefPtr<NG::FrameNode>>& children,
    const CommonProperty& commonProperty)
{
    std::list<RefPtr<NG::FrameNode>> frameNodeChildren;
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    auto uiVirtualNode = accessibilityProperty->GetAccessibilityVirtualNode();
    if (uiVirtualNode != nullptr) {
        auto virtualNode = AceType::DynamicCast<NG::FrameNode>(uiVirtualNode);
        if (virtualNode != nullptr) {
            GetFrameNodeChildren(virtualNode, frameNodeChildren, commonProperty);
        }
    } else {
        for (const auto& item : node->GetChildren(true)) {
            GetFrameNodeChildren(item, frameNodeChildren, commonProperty);
        }

        auto overlayNode = node->GetOverlayNode();
        if (overlayNode != nullptr) {
            GetFrameNodeChildren(overlayNode, frameNodeChildren, commonProperty);
        }
    }
    while (!frameNodeChildren.empty()) {
        children.emplace_back(frameNodeChildren.front());
        frameNodeChildren.pop_front();
    }
}

RefPtr<NG::FrameNode> GetLastChildFrameNode(const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty)
{
    std::list<RefPtr<NG::FrameNode>> children { node };
    RefPtr<NG::FrameNode> checkNode;

    while (!children.empty()) {
        checkNode = children.back();
        children.clear();
        GetChildrenFromFrameNode(checkNode, children, commonProperty);
    }
    return checkNode;
}
}

void JsAccessibilityManager::UpdateVirtualNodeInfo(std::list<AccessibilityElementInfo>& infos,
    AccessibilityElementInfo& nodeInfo, const RefPtr<NG::UINode>& uiVirtualNode, const CommonProperty& commonProperty,
    const RefPtr<NG::PipelineContext>& ngPipeline)
{
    auto frameParentNode = AceType::DynamicCast<NG::FrameNode>(uiVirtualNode);
    CHECK_NULL_VOID(frameParentNode);
    std::list<RefPtr<NG::FrameNode>> children;

    GetChildrenFromFrameNode(frameParentNode, children, commonProperty);
    for (const auto& frameNodeChild : children) {
        AccessibilityElementInfo virtualInfo;
        UpdateVirtualNodeChildAccessibilityElementInfo(frameNodeChild, commonProperty,
            nodeInfo, virtualInfo, ngPipeline);
        virtualInfo.SetParent(uiVirtualNode->GetAccessibilityId());
        nodeInfo.AddChild(frameNodeChild->GetAccessibilityId());
        UpdateVirtualNodeInfo(infos, virtualInfo, frameNodeChild, commonProperty, ngPipeline);
        infos.push_back(virtualInfo);
    }
}

namespace {
void SearchExtensionElementInfoNG(const SearchParameter& searchParam,
    const RefPtr<NG::FrameNode>& node, std::list<Accessibility::AccessibilityElementInfo>& infos,
    Accessibility::AccessibilityElementInfo& parentInfo)
{
    auto extensionElementInfos = SearchExtensionElementInfoByAccessibilityIdNG(
        searchParam.nodeId, searchParam.mode, node, searchParam.uiExtensionOffset);
    if (!extensionElementInfos.empty()) {
        auto rootParentId = extensionElementInfos.front().GetParentNodeId();
        ConvertExtensionAccessibilityNodeId(extensionElementInfos, node, searchParam.uiExtensionOffset, parentInfo);
        if (rootParentId == NG::UI_EXTENSION_ROOT_ID) {
            extensionElementInfos.front().SetParent(node->GetAccessibilityId());
        }
        if (parentInfo.GetComponentType() == V2::ISOLATED_COMPONENT_ETS_TAG) {
            auto windowId = parentInfo.GetWindowId();
            for (auto& info : extensionElementInfos) {
                info.SetWindowId(windowId);
            }
        }
        for (auto& info : extensionElementInfos) {
            infos.push_back(info);
        }
    }
}
}

void JsAccessibilityManager::UpdateChildrenNodeInCache(std::list<AccessibilityElementInfo>& infos,
    const CommonProperty& commonProperty, const RefPtr<NG::PipelineContext>& ngPipeline,
    const SearchParameter& searchParam, std::list<RefPtr<NG::FrameNode>>& children)
{
    while (!children.empty()) {
        RefPtr<NG::FrameNode> frameNodeParent = children.front();
        children.pop_front();
        AccessibilityElementInfo nodeInfo;
        auto accessibilityProperty = frameNodeParent->GetAccessibilityProperty<NG::AccessibilityProperty>();
        auto uiVirtualNode = accessibilityProperty->GetAccessibilityVirtualNode();
        if (CheckAndSetAccessibilityVisible(frameNodeParent, commonProperty.isReduceMode)) {
            UpdateAccessibilityElementInfo(frameNodeParent, commonProperty, nodeInfo, ngPipeline);
        }
        if (uiVirtualNode != nullptr) {
            auto virtualNode = AceType::DynamicCast<NG::FrameNode>(uiVirtualNode);
            if (virtualNode == nullptr) {
                continue;
            }
            AccessibilityElementInfo virtualInfo;
            UpdateVirtualNodeAccessibilityElementInfo(
                frameNodeParent, virtualNode, commonProperty, virtualInfo, ngPipeline);
            virtualInfo.SetParent(frameNodeParent->GetAccessibilityId());
            auto childIds = nodeInfo.GetChildIds();
            for (auto& child : childIds) {
                nodeInfo.RemoveChild(child);
            }
            nodeInfo.AddChild(virtualNode->GetAccessibilityId());
            auto uiParentNode = AceType::DynamicCast<NG::UINode>(frameNodeParent);
            if (!uiVirtualNode->GetChildren(true).empty()) {
                UpdateVirtualNodeInfo(infos, virtualInfo, uiVirtualNode, commonProperty, ngPipeline);
            }
            infos.push_back(virtualInfo);
            infos.push_back(nodeInfo);
            continue;
        }
        if (!IsExtensionComponent(frameNodeParent) || IsUIExtensionShowPlaceholder(frameNodeParent)) {
            infos.push_back(nodeInfo);
            GetChildrenFromFrameNode(frameNodeParent, children, commonProperty);
            continue;
        }
        if (!((frameNodeParent->GetUiExtensionId() > NG::UI_EXTENSION_UNKNOW_ID) &&
            (((frameNodeParent->GetUiExtensionId() <= NG::UI_EXTENSION_ID_FIRST_MAX) &&
            (NG::UI_EXTENSION_OFFSET_MAX == searchParam.uiExtensionOffset)) ||
            (frameNodeParent->GetUiExtensionId() <= NG::UI_EXTENSION_ID_OTHER_MAX)))) {
            continue;
        }
        auto transferParam = searchParam;
        transferParam.nodeId = NG::UI_EXTENSION_ROOT_ID;
        SearchExtensionElementInfoNG(transferParam, frameNodeParent, infos, nodeInfo);
        infos.push_back(nodeInfo);
    }
}

void JsAccessibilityManager::UpdateCacheInfoNG(std::list<AccessibilityElementInfo>& infos,
    const RefPtr<NG::FrameNode>& node, CommonProperty& commonProperty,
    const RefPtr<NG::PipelineContext>& ngPipeline, const SearchParameter& searchParam)
{
    uint32_t umode = searchParam.mode;
    std::list<RefPtr<NG::FrameNode>> children;
    // get all children
    if ((umode & (static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN) |
                  static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN_REDUCED))) == 0) {
        return;
    }
    commonProperty.isReduceMode = umode & static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN_REDUCED);
    GetChildrenFromFrameNode(node, children, commonProperty);
    UpdateChildrenNodeInCache(infos, commonProperty, ngPipeline, searchParam, children);
}

void JsAccessibilityManager::UpdateEventWhiteList(const std::vector<uint32_t>& eventList)
{
    eventWhiteList_ = eventList;
}

namespace {
bool CanAccessibilityFocusedNG(const RefPtr<NG::FrameNode>& node)
{
    CHECK_NULL_RETURN(node, false);
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, false);
    auto level = accessibilityProperty->GetAccessibilityLevel();
    return !node->IsRootNode() &&
           node->GetLayoutProperty()->GetVisibilityValue(VisibleType::VISIBLE) == VisibleType::VISIBLE &&
           level != NG::AccessibilityProperty::Level::NO_STR &&
           level != NG::AccessibilityProperty::Level::NO_HIDE_DESCENDANTS;
}
// focus move search
void AddFocusableNode(std::list<RefPtr<NG::FrameNode>>& nodeList, const RefPtr<NG::FrameNode>& node)
{
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    auto level = accessibilityProperty->GetAccessibilityLevel();
    if (CanAccessibilityFocusedNG(node)) {
        nodeList.emplace_back(node);
    }

    if (!accessibilityProperty->IsAccessibilityGroup() &&
        level != NG::AccessibilityProperty::Level::NO_HIDE_DESCENDANTS) {
        std::list<RefPtr<NG::FrameNode>> children;
        CommonProperty commonProperty;
        commonProperty.pageNodes.clear(); // empty means search all page
        commonProperty.pagePaths.clear();
        for (const auto& child : node->GetChildren(true)) {
            GetFrameNodeChildren(child, children, commonProperty);
        }

        for (const auto& child : children) {
            AddFocusableNode(nodeList, child);
        }
    }
}

// execute action
bool RequestFocus(RefPtr<NG::FrameNode>& frameNode)
{
    auto focusHub = frameNode->GetFocusHub();
    CHECK_NULL_RETURN(focusHub, false);
    return focusHub->RequestFocusImmediately();
}

bool LostFocus(const RefPtr<NG::FrameNode>& frameNode)
{
    CHECK_NULL_RETURN(frameNode, false);
    auto focusHub = frameNode->GetFocusHub();
    CHECK_NULL_RETURN(focusHub, false);
    focusHub->LostFocus();
    return true;
}

void HandleWillClickAccept(const RefPtr<NG::FrameNode>& frameNode)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<NG::EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto gestureEventHub = eventHub->GetGestureEventHub();
    CHECK_NULL_VOID(gestureEventHub);
    auto gestureEventInfo = gestureEventHub->GetGestureEventInfo();
    auto clickInfo = gestureEventHub->GetClickInfo();
    NG::UIObserverHandler::GetInstance().NotifyWillClick(gestureEventInfo, clickInfo, frameNode);
}

void HandleDidClickAccept(const RefPtr<NG::FrameNode>& frameNode)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetOrCreateEventHub<NG::EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto gestureEventHub = eventHub->GetGestureEventHub();
    CHECK_NULL_VOID(gestureEventHub);
    auto gestureEventInfo = gestureEventHub->GetGestureEventInfo();
    auto clickInfo = gestureEventHub->GetClickInfo();
    NG::UIObserverHandler::GetInstance().NotifyDidClick(gestureEventInfo, clickInfo, frameNode);
}

bool ActClick(RefPtr<NG::FrameNode>& frameNode, const NG::SecCompEnhanceEvent& secEvent)
{
    if (NG::AccessibilityFunctionUtils::HandleClickBySecComponent(frameNode, secEvent)) {
        // notify child action happened to parent
        NG::AccessibilityFunctionUtils::HandleNotifyChildAction(frameNode, NotifyChildActionType::ACTION_CLICK);
        return true;
    }

    auto interceptResult =
        NG::AccessibilityFunctionUtils::HandleAccessibilityActionIntercept(
            frameNode, AccessibilityInterfaceAction::ACCESSIBILITY_CLICK);
    if (interceptResult == AccessibilityActionInterceptResult::ACTION_INTERCEPT) {
        return true;
    }
    auto eventHub = frameNode->GetOrCreateEventHub<NG::EventHub>();
    CHECK_NULL_RETURN(eventHub, false);
    auto gesture = eventHub->GetGestureEventHub();
    CHECK_NULL_RETURN(gesture, false);
    HandleWillClickAccept(frameNode);
    bool result = gesture->ActClick();
    HandleDidClickAccept(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, result);
    auto accessibilityAction = ACTIONS.find(ACCESSIBILITY_ACTION_CLICK);
    if (accessibilityAction != ACTIONS.end()) {
        AccessibilityActionParam param;
        param.accessibilityProperty = accessibilityProperty;
        result |= accessibilityAction->second(param);
    }

    // notify child action happened to parent
    NG::AccessibilityFunctionUtils::HandleNotifyChildAction(frameNode, NotifyChildActionType::ACTION_CLICK);
    return result;
}

bool ActLongClick(RefPtr<NG::FrameNode>& frameNode)
{
    auto gesture = frameNode->GetOrCreateEventHub<NG::EventHub>()->GetGestureEventHub();
    CHECK_NULL_RETURN(gesture, false);
    return gesture->ActLongClick();
}

void CreateOrUpdateAccessibilityFocusPaint(const RefPtr<NG::FrameNode>& focusFrameNode)
{
    CHECK_NULL_VOID(focusFrameNode);
    ACE_SCOPED_TRACE("CreateOrUpdateAccessibilityFocusPaint: targetNode[%s].", focusFrameNode->GetTag().c_str());
    auto paintNode = NG::FrameNode::CreateFrameNode(V2::ACCESSIBILITY_FOCUS_PAINT_NODE_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<NG::AccessibilityFocusPaintNodePattern>(focusFrameNode));
    CHECK_NULL_VOID(paintNode);
    auto pattern = paintNode->GetPattern<NG::AccessibilityFocusPaintNodePattern>();
    CHECK_NULL_VOID(pattern);
    pattern->UpdateFocusNode(focusFrameNode);
    auto renderContext = paintNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->OnZIndexUpdate(INT32_MAX);
    renderContext->ResetAccessibilityFocusRect();
    renderContext->UpdateAccessibilityFocus(true);

    auto pipeline = focusFrameNode->GetContextRefPtr();
    CHECK_NULL_VOID(pipeline);
    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);

    auto rootNode = AceType::DynamicCast<NG::FrameNode>(overlayManager->FindWindowScene(focusFrameNode));
    CHECK_NULL_VOID(rootNode);
    pattern->UpdateRootNode(rootNode);
    rootNode->SetFocusPaintNode(paintNode);
    rootNode->MarkNeedSyncRenderTree();
    rootNode->RebuildRenderContextTree();
}

void RemoveAccessibilityFocusPaint(const RefPtr<NG::FrameNode>& focusFrameNode)
{
    CHECK_NULL_VOID(focusFrameNode);
    ACE_SCOPED_TRACE("Node[%s] RemoveAccessibilityFocusPaint", focusFrameNode->GetTag().c_str());
    auto paintNode = focusFrameNode->GetPaintNode();
    CHECK_NULL_VOID(paintNode);
    focusFrameNode->SetPaintNode(nullptr);
    auto renderContext = paintNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateAccessibilityFocus(false);
    auto pattern = paintNode->GetPattern<NG::AccessibilityFocusPaintNodePattern>();
    CHECK_NULL_VOID(pattern);
    auto rootNode = AceType::DynamicCast<NG::FrameNode>(pattern->GetRootNode().Upgrade());
    CHECK_NULL_VOID(rootNode);
    rootNode->SetFocusPaintNode(nullptr);
    rootNode->MarkNeedSyncRenderTree();
    rootNode->RebuildRenderContextTree();
}

void PaintAccessibilityFocusNode(const RefPtr<NG::FrameNode>& focusNode, bool focus)
{
    CHECK_NULL_VOID(focusNode);
    if (focusNode->IsDrawFocusOnTop()) {
        if (focus) {
            CreateOrUpdateAccessibilityFocusPaint(focusNode);
            UpdatePaintNodeRender(focusNode);
        } else {
            RemoveAccessibilityFocusPaint(focusNode);
        }
    } else {
        auto context = focusNode->GetRenderContext();
        CHECK_NULL_VOID(context);
        context->UpdateAccessibilityFocus(focus);
    }
}

void ClearAccessibilityFocus(const RefPtr<NG::FrameNode>& root, int64_t focusNodeId)
{
    auto oldFocusNode = GetFramenodeByAccessibilityId(root, focusNodeId);
    CHECK_NULL_VOID(oldFocusNode);
    if (oldFocusNode->IsDrawFocusOnTop()) {
        PaintAccessibilityFocusNode(oldFocusNode, false);
    } else {
        oldFocusNode->GetRenderContext()->UpdateAccessibilityFocus(false);
    }
}

NG::RectT<int32_t> GetFrameNodeRectInt(const RefPtr<NG::FrameNode>& frameNode)
{
    auto rect = frameNode->GetTransformRectRelativeToWindow();
    NG::RectT<int32_t> rectInt {
        static_cast<int32_t>(std::floor(rect.Left())),
        static_cast<int32_t>(std::floor(rect.Top())),
        static_cast<int32_t>(std::floor(rect.Width())),
        static_cast<int32_t>(std::floor(rect.Height()))
    };
    return rectInt;
}

void UpdateAccessibilityFocusRect(const RefPtr<NG::FrameNode>& frameNode,
    RefPtr<NG::RenderContext>& renderContext,
    bool isAccessibilityVirtualNode)
{
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(renderContext);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (accessibilityProperty->IsMatchAccessibilityResponseRegion(isAccessibilityVirtualNode)) {
        auto rectInt = accessibilityProperty->GetAccessibilityResponseRegionRect(isAccessibilityVirtualNode);
        renderContext->UpdateAccessibilityFocusRect(rectInt);
        if (isAccessibilityVirtualNode) {
            renderContext->UpdateAccessibilityFocus(true, frameNode->GetAccessibilityId());
        } else {
            PaintAccessibilityFocusNode(frameNode, true);
        }
    } else {
        if (isAccessibilityVirtualNode) {
            renderContext->UpdateAccessibilityFocusRect(GetFrameNodeRectInt(frameNode));
            renderContext->UpdateAccessibilityFocus(true, frameNode->GetAccessibilityId());
        } else {
            renderContext->ResetAccessibilityFocusRect();
            PaintAccessibilityFocusNode(frameNode, true);
        }
    }
}

void ClearVirtualNodeAccessibilityFocus(const RefPtr<NG::FrameNode>& root, int64_t currentFocusVirtualNodeParentId)
{
    CHECK_EQUAL_VOID(currentFocusVirtualNodeParentId, -1);
    auto currentFocusVirtualNodeParentNode = GetFramenodeByAccessibilityId(root, currentFocusVirtualNodeParentId);
    CHECK_NULL_VOID(currentFocusVirtualNodeParentNode);
    auto renderContext = currentFocusVirtualNodeParentNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateAccessibilityFocus(false);
}

void SetParentAccessibilityId(const RefPtr<NG::FrameNode>& frameNode, AccessibilityFocusInfo& focusInfo)
{
    auto parentUinode = frameNode->GetVirtualNodeParent().Upgrade();
    CHECK_NULL_VOID(parentUinode);
    auto parentFrame = AceType::DynamicCast<NG::FrameNode>(parentUinode);
    CHECK_NULL_VOID(parentFrame);
    focusInfo.currentFocusVirtualNodeParentId = parentFrame->GetAccessibilityId();
}

bool ActAccessibilityFocus(int64_t elementId, const RefPtr<NG::FrameNode>& frameNode,
    RefPtr<NG::PipelineContext>& context, AccessibilityFocusInfo& focusInfo, bool isNeedClear)
{
    CHECK_NULL_RETURN(frameNode, false);
    bool isAccessibilityVirtualNode = frameNode->IsAccessibilityVirtualNode();
    RefPtr<NG::RenderContext> renderContext = nullptr;
    if (isAccessibilityVirtualNode) {
        auto parentUinode = frameNode->GetVirtualNodeParent().Upgrade();
        CHECK_NULL_RETURN(parentUinode, false);
        auto parentFrame = AceType::DynamicCast<NG::FrameNode>(parentUinode);
        CHECK_NULL_RETURN(parentFrame, false);
        renderContext = parentFrame->GetRenderContext();
    } else {
        renderContext = frameNode->GetRenderContext();
    }
    CHECK_NULL_RETURN(renderContext, false);
    if (isNeedClear) {
        if (elementId != focusInfo.currentFocusNodeId) {
            return false;
        }
        if (isAccessibilityVirtualNode) {
            renderContext->UpdateAccessibilityFocus(false);
            focusInfo.currentFocusVirtualNodeParentId = -1;
        } else {
            PaintAccessibilityFocusNode(frameNode, false);
        }
        focusInfo.currentFocusNodeId = -1;
        auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        CHECK_NULL_RETURN(accessibilityProperty, true);
        accessibilityProperty->OnAccessibilityFocusCallback(false);
        accessibilityProperty->SetAccessibilityFocusState(false);
        return true;
    }
    CHECK_EQUAL_RETURN(elementId, focusInfo.currentFocusNodeId, false);
    ClearAccessibilityFocus(context->GetRootElement(), focusInfo.currentFocusNodeId);
    if (focusInfo.currentFocusNodeId != focusInfo.currentFocusVirtualNodeParentId) {
        ClearVirtualNodeAccessibilityFocus(context->GetRootElement(), focusInfo.currentFocusVirtualNodeParentId);
    }
    UpdateAccessibilityFocusRect(frameNode, renderContext, isAccessibilityVirtualNode);
    focusInfo.currentFocusNodeId = frameNode->GetAccessibilityId();
    if (isAccessibilityVirtualNode) {
        SetParentAccessibilityId(frameNode, focusInfo);
    }
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, false);
    accessibilityProperty->OnAccessibilityFocusCallback(true);
    accessibilityProperty->SetAccessibilityFocusState(true);
    ProcessFocusScroll(frameNode, context);
    return true;
}

inline std::string GetSupportAction(const std::unordered_set<AceAction>& supportAceActions)
{
    std::string actionForDump;
    for (const auto& action : supportAceActions) {
        if (!actionForDump.empty()) {
            actionForDump.append(",");
        }
        actionForDump.append(std::to_string(static_cast<int32_t>(action)));
    }
    return actionForDump;
}

static std::string ConvertActionTypeToString(ActionType action)
{
    static const ActionStrTable actionStrTable[] = {
        { ActionType::ACCESSIBILITY_ACTION_FOCUS, "ACCESSIBILITY_ACTION_FOCUS" },
        { ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS, "ACCESSIBILITY_ACTION_CLEAR_FOCUS" },
        { ActionType::ACCESSIBILITY_ACTION_SELECT, "ACCESSIBILITY_ACTION_SELECT" },
        { ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION, "ACCESSIBILITY_ACTION_CLEAR_SELECTION" },
        { ActionType::ACCESSIBILITY_ACTION_CLICK, "ACCESSIBILITY_ACTION_CLICK" },
        { ActionType::ACCESSIBILITY_ACTION_LONG_CLICK, "ACCESSIBILITY_ACTION_LONG_CLICK" },
        { ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, "ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS" },
        { ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS,
            "ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS" },
        { ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD, "ACCESSIBILITY_ACTION_SCROLL_FORWARD" },
        { ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD, "ACCESSIBILITY_ACTION_SCROLL_BACKWARD" },
        { ActionType::ACCESSIBILITY_ACTION_COPY, "ACCESSIBILITY_ACTION_COPY" },
        { ActionType::ACCESSIBILITY_ACTION_PASTE, "ACCESSIBILITY_ACTION_PASTE" },
        { ActionType::ACCESSIBILITY_ACTION_CUT, "ACCESSIBILITY_ACTION_CUT" },
        { ActionType::ACCESSIBILITY_ACTION_SET_SELECTION, "ACCESSIBILITY_ACTION_SET_SELECTION" },
        { ActionType::ACCESSIBILITY_ACTION_SET_TEXT, "ACCESSIBILITY_ACTION_SET_TEXT" },
        { ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT, "ACCESSIBILITY_ACTION_NEXT_TEXT" },
        { ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT, "ACCESSIBILITY_ACTION_PREVIOUS_TEXT" },
        { ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION, "ACCESSIBILITY_ACTION_SET_CURSOR_POSITION" },
        { ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK, "ACCESSIBILITY_ACTION_SPAN_CLICK" },
        { ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM, "ACCESSIBILITY_ACTION_NEXT_HTML_ITEM" },
        { ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM, "ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM" },
    };
    for (const auto& item : actionStrTable) {
        if (action == item.action) {
            return item.actionStr;
        }
    }
    return ACTION_DEFAULT_PARAM;
}

static AceAction ConvertAccessibilityAction(ActionType accessibilityAction)
{
    static const ActionTable actionTable[] = {
        { AceAction::ACTION_CLICK, ActionType::ACCESSIBILITY_ACTION_CLICK },
        { AceAction::ACTION_LONG_CLICK, ActionType::ACCESSIBILITY_ACTION_LONG_CLICK },
        { AceAction::ACTION_SCROLL_FORWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD },
        { AceAction::ACTION_SCROLL_BACKWARD, ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD },
        { AceAction::ACTION_FOCUS, ActionType::ACCESSIBILITY_ACTION_FOCUS },
        { AceAction::ACTION_CLEAR_FOCUS, ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS },
        { AceAction::ACTION_ACCESSIBILITY_FOCUS, ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS },
        { AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS, ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS },
        { AceAction::ACTION_NEXT_AT_MOVEMENT_GRANULARITY, ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT },
        { AceAction::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY, ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT },
        { AceAction::ACTION_SET_TEXT, ActionType::ACCESSIBILITY_ACTION_SET_TEXT },
        { AceAction::ACTION_COPY, ActionType::ACCESSIBILITY_ACTION_COPY },
        { AceAction::ACTION_PASTE, ActionType::ACCESSIBILITY_ACTION_PASTE },
        { AceAction::ACTION_CUT, ActionType::ACCESSIBILITY_ACTION_CUT },
        { AceAction::ACTION_SELECT, ActionType::ACCESSIBILITY_ACTION_SELECT },
        { AceAction::ACTION_CLEAR_SELECTION, ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION },
        { AceAction::ACTION_SET_SELECTION, ActionType::ACCESSIBILITY_ACTION_SET_SELECTION },
        { AceAction::ACTION_SET_CURSOR_POSITION, ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION },
        { AceAction::ACTION_EXEC_SUB_COMPONENT, ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK },
        { AceAction::ACTION_NEXT_HTML_ITEM, ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM },
        { AceAction::ACTION_PREVIOUS_HTML_ITEM, ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM },
    };
    for (const auto& item : actionTable) {
        if (accessibilityAction == item.action) {
            return item.aceAction;
        }
    }
    return AceAction::ACTION_NONE;
}

static void DumpSupportActionNG(const AccessibilityElementInfo& nodeInfo)
{
    DumpLog::GetInstance().AddDesc(
        "support action instructions: use command to make application components perform corresponding action");
    DumpLog::GetInstance().AddDesc(
        "use support action command: aa dump -i [AbilityRecord] -c -inspector [AccessibilityId] [AceAction]");
    std::string actionForDump;
    for (const auto& action : nodeInfo.GetActionList()) {
        if (!actionForDump.empty()) {
            actionForDump.append(",");
        }
        actionForDump.append(ConvertActionTypeToString(action.GetActionType()));
        actionForDump.append(": ");
        actionForDump.append(std::to_string(static_cast<int32_t>(ConvertAccessibilityAction(action.GetActionType()))));
    }
    DumpLog::GetInstance().AddDesc("support action: ", actionForDump);
}

static void DumpGridInfoNG(const AccessibilityElementInfo& nodeInfo)
{
    DumpLog::GetInstance().AddDesc("grid info rows: ", nodeInfo.GetGrid().GetRowCount());
    DumpLog::GetInstance().AddDesc("grid info columns: ", nodeInfo.GetGrid().GetColumnCount());
    DumpLog::GetInstance().AddDesc("grid info select mode: ", nodeInfo.GetGrid().GetSelectionMode());
    DumpLog::GetInstance().AddDesc("grid item info, row: ", nodeInfo.GetGridItem().GetRowIndex());
    DumpLog::GetInstance().AddDesc("grid item info, column: ", nodeInfo.GetGridItem().GetColumnIndex());
    DumpLog::GetInstance().AddDesc("grid item info, rowSpan: ", nodeInfo.GetGridItem().GetRowSpan());
    DumpLog::GetInstance().AddDesc("grid item info, columnSpan: ", nodeInfo.GetGridItem().GetColumnSpan());
    DumpLog::GetInstance().AddDesc("grid item info, is heading: ", nodeInfo.GetGridItem().IsHeading());
    DumpLog::GetInstance().AddDesc("grid item info, selected: ", nodeInfo.GetGridItem().IsSelected());
}

inline void DumpContentListNG(const AccessibilityElementInfo& nodeInfo)
{
    std::vector<std::string> contentList;
    nodeInfo.GetContentList(contentList);
    std::string contents;
    for (const auto& content : contentList) {
        if (!contents.empty()) {
            contents.append(",");
        }
        contents.append(content);
    }
    DumpLog::GetInstance().AddDesc("content list: ", contents);
}

static void DumpExtraElementInfoNG(const AccessibilityElementInfo& nodeInfo)
{
    ExtraElementInfo extraElementInfo = nodeInfo.GetExtraElement();
    if (!extraElementInfo.GetExtraElementInfoValueStr().empty()) {
        for (auto i = extraElementInfo.GetExtraElementInfoValueStr().begin();
             i != extraElementInfo.GetExtraElementInfoValueStr().end(); ++i) {
            DumpLog::GetInstance().AddDesc("extra element info: ", i->first, i->second);
        }
    }
    if (!extraElementInfo.GetExtraElementInfoValueInt().empty()) {
        for (auto i = extraElementInfo.GetExtraElementInfoValueInt().begin();
             i != extraElementInfo.GetExtraElementInfoValueInt().end(); ++i) {
            DumpLog::GetInstance().AddDesc("extra element info: ", i->first, i->second);
        }
    }
}

static void UpdateSpanList(std::vector<SpanInfo>& spansInfosList, std::string& spans)
{
    for (const auto& span : spansInfosList) {
        if (!spans.empty()) {
            spans.append(",");
        }
        spans.append("\n\t span id: ");
        spans.append(std::to_string(span.GetSpanId()));
        spans.append(", span text: ");
        spans.append(span.GetSpanText());
        spans.append(", accessibility text: ");
        spans.append(span.GetAccessibilityText());
        spans.append(", accessibility description: ");
        spans.append(span.GetAccessibilityDescription());
        spans.append(", accessibility level: ");
        spans.append(span.GetAccessibilityLevel());
    }
}

inline void DumpSpanListNG(const AccessibilityElementInfo& nodeInfo)
{
    std::string spans;
    std::vector<SpanInfo> spansInfosList = nodeInfo.GetSpanList();
    std::size_t spanCount = spansInfosList.size();
    UpdateSpanList(spansInfosList, spans);
    DumpLog::GetInstance().AddDesc("span list count: ", static_cast<int32_t>(spanCount));
    DumpLog::GetInstance().AddDesc("span list: ", spans);
}

inline std::string ChildrenToString(const std::vector<int64_t>& children, int32_t treeId)
{
    std::string ids;
    for (auto child : children) {
        if (!ids.empty()) {
            ids.append(",");
        }
        int64_t childId = child;
        AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, childId);
        ids.append(std::to_string(childId));
    }
    return ids;
}

inline void DumpRectNG(const Accessibility::Rect& rect)
{
    DumpLog::GetInstance().AddDesc(
        "width: ", std::to_string(rect.GetRightBottomXScreenPostion() - rect.GetLeftTopXScreenPostion()));
    DumpLog::GetInstance().AddDesc(
        "height: ", std::to_string(rect.GetRightBottomYScreenPostion() - rect.GetLeftTopYScreenPostion()));
    DumpLog::GetInstance().AddDesc("left: ", std::to_string(rect.GetLeftTopXScreenPostion()));
    DumpLog::GetInstance().AddDesc("top: ", std::to_string(rect.GetLeftTopYScreenPostion()));
    DumpLog::GetInstance().AddDesc("right: ", std::to_string(rect.GetRightBottomXScreenPostion()));
    DumpLog::GetInstance().AddDesc("bottom: ", std::to_string(rect.GetRightBottomYScreenPostion()));
}

void GenerateAccessibilityEventInfo(const AccessibilityEvent& accessibilityEvent, AccessibilityEventInfo& eventInfo)
{
    Accessibility::EventType type = Accessibility::EventType::TYPE_VIEW_INVALID;
    if (accessibilityEvent.type != AccessibilityEventType::UNKNOWN) {
        type = ConvertAceEventType(accessibilityEvent.type);
    } else {
        type = ConvertStrToEventType(accessibilityEvent.eventType);
    }

    if (type == Accessibility::EventType::TYPE_VIEW_INVALID) {
        return;
    }

    eventInfo.SetTimeStamp(GetMicroTickCount());
    eventInfo.SetBeforeText(accessibilityEvent.beforeText);
    eventInfo.SetLatestContent(accessibilityEvent.latestContent);
    eventInfo.SetTextAnnouncedForAccessibility(accessibilityEvent.textAnnouncedForAccessibility);
    eventInfo.SetWindowChangeTypes(static_cast<Accessibility::WindowUpdateType>(accessibilityEvent.windowChangeTypes));
    eventInfo.SetWindowContentChangeTypes(
        static_cast<Accessibility::WindowsContentChangeTypes>(accessibilityEvent.windowContentChangeTypes));
    eventInfo.SetSource(accessibilityEvent.nodeId);
    eventInfo.SetEventType(type);
    eventInfo.SetCurrentIndex(static_cast<int>(accessibilityEvent.currentItemIndex));
    eventInfo.SetItemCounts(static_cast<int>(accessibilityEvent.itemCount));
    eventInfo.SetBundleName(Container::CurrentBundleName());
    eventInfo.SetBeginIndex(accessibilityEvent.startIndex);
    eventInfo.SetEndIndex(accessibilityEvent.endIndex);
    if (accessibilityEvent.extraEventInfo.size() > 0) {
        ExtraEventInfo extraEventInfo;
        for (const auto& info : accessibilityEvent.extraEventInfo) {
            auto ret = extraEventInfo.SetExtraEventInfo(info.first, info.second);
            TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "The result of SetExtraEventInfo:%{public}d, keyStrLen:%{public}d",
                ret, static_cast<int>(info.first.length()));
        }
        eventInfo.SetExtraEvent(extraEventInfo);
    }
}
} // namespace

void JsAccessibilityManager::DumpAccessibilityPropertyNG(const AccessibilityElementInfo& nodeInfo)
{
    DumpLog::GetInstance().AddDesc("checked: ", BoolToString(nodeInfo.IsChecked()));
    DumpLog::GetInstance().AddDesc("selected: ", BoolToString(nodeInfo.IsSelected()));
    DumpLog::GetInstance().AddDesc("checkable: ", BoolToString(nodeInfo.IsCheckable()));
    DumpLog::GetInstance().AddDesc("scrollable: ", BoolToString(nodeInfo.IsScrollable()));
    DumpLog::GetInstance().AddDesc("accessibility hint: ", BoolToString(nodeInfo.IsGivingHint()));
    DumpLog::GetInstance().AddDesc("hint text: ", nodeInfo.GetHint());
    DumpLog::GetInstance().AddDesc("error text: ", nodeInfo.GetError());
    DumpLog::GetInstance().AddDesc("max text length: ", nodeInfo.GetTextLengthLimit());
    DumpLog::GetInstance().AddDesc("scroll offset: ", nodeInfo.GetOffset());
    DumpLog::GetInstance().AddDesc("text selection start: ", nodeInfo.GetSelectedBegin());
    DumpLog::GetInstance().AddDesc("text selection end: ", nodeInfo.GetSelectedEnd());
    DumpLog::GetInstance().AddDesc("is multi line: ", BoolToString(nodeInfo.IsPluraLineSupported()));
    DumpLog::GetInstance().AddDesc("is password: ", BoolToString(nodeInfo.IsPassword()));
    DumpLog::GetInstance().AddDesc(
        "text input type: ", ConvertInputTypeToString(static_cast<AceTextCategory>(nodeInfo.GetInputType())));
    DumpGridInfoNG(nodeInfo);
    DumpLog::GetInstance().AddDesc("min value: ", nodeInfo.GetRange().GetMin());
    DumpLog::GetInstance().AddDesc("max value: ", nodeInfo.GetRange().GetMax());
    DumpLog::GetInstance().AddDesc("current value: ", nodeInfo.GetRange().GetCurrent());
    DumpLog::GetInstance().AddDesc("current index: ", nodeInfo.GetCurrentIndex());
    DumpLog::GetInstance().AddDesc("begin index: ", nodeInfo.GetBeginIndex());
    DumpLog::GetInstance().AddDesc("end index: ", nodeInfo.GetEndIndex());
    DumpLog::GetInstance().AddDesc("collection item counts: ", nodeInfo.GetItemCounts());
    DumpLog::GetInstance().AddDesc("editable: ", BoolToString(nodeInfo.IsEditable()));
    DumpLog::GetInstance().AddDesc("is essential: ", BoolToString(nodeInfo.IsEssential()));
    DumpLog::GetInstance().AddDesc("deletable: ", nodeInfo.IsDeletable());
    DumpLog::GetInstance().AddDesc("live region: ", nodeInfo.GetLiveRegion());
    DumpLog::GetInstance().AddDesc("content description: ", nodeInfo.GetDescriptionInfo());
    DumpLog::GetInstance().AddDesc("content invalid: ", BoolToString(nodeInfo.GetContentInvalid()));
    DumpLog::GetInstance().AddDesc("accessibility label: ", nodeInfo.GetLabeledAccessibilityId());
    DumpLog::GetInstance().AddDesc("isActive: ", nodeInfo.GetIsActive());
    DumpLog::GetInstance().AddDesc("accessibilityVisible: ", nodeInfo.GetAccessibilityVisible());
    DumpLog::GetInstance().AddDesc("accessibilityNextFocusInspectorKey: ",
                                   nodeInfo.GetAccessibilityNextFocusInspectorKey());
    DumpLog::GetInstance().AddDesc("accessibilityScrollTriggerable: ",
                                   nodeInfo.GetAccessibilityScrollable());
    DumpLog::GetInstance().AddDesc("accessibilityNextFocusId: ", nodeInfo.GetAccessibilityNextFocusId());
    DumpLog::GetInstance().AddDesc("accessibilityPreviousFocusId: ", nodeInfo.GetAccessibilityPreviousFocusId());
    DumpLog::GetInstance().AddDesc("clip: ", nodeInfo.GetClip());
    DumpExtraElementInfoNG(nodeInfo);
    DumpLog::GetInstance().AddDesc(
        "trigger action: ", static_cast<int32_t>(ConvertAccessibilityAction(nodeInfo.GetTriggerAction())));
    DumpLog::GetInstance().AddDesc("text move step: " + std::to_string(nodeInfo.GetTextMovementStep()));
    DumpSpanListNG(nodeInfo);
    DumpSupportActionNG(nodeInfo);
    DumpContentListNG(nodeInfo);
    DumpLog::GetInstance().AddDesc("latest content: ", nodeInfo.GetLatestContent());
}

void JsAccessibilityManager::DumpCommonPropertyNG(const AccessibilityElementInfo& nodeInfo, int32_t treeId)
{
    int64_t elementId = nodeInfo.GetAccessibilityId();
    AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, elementId);
    DumpLog::GetInstance().AddDesc("ID: ", elementId);
    DumpLog::GetInstance().AddDesc("UniqueID: ", nodeInfo.GetUniqueId());
    int64_t parentId = nodeInfo.GetParentNodeId();
    AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, parentId);
    DumpLog::GetInstance().AddDesc("parent ID: ", parentId);
    DumpLog::GetInstance().AddDesc("child IDs: ", ChildrenToString(nodeInfo.GetChildIds(), treeId));
    DumpLog::GetInstance().AddDesc("component type: ", nodeInfo.GetComponentType());
    DumpLog::GetInstance().AddDesc("accessibilityCustomRole: " + nodeInfo.GetCustomComponentType());
    DumpLog::GetInstance().AddDesc("text: ", nodeInfo.GetContent());
    DumpLog::GetInstance().AddDesc("originText: ", nodeInfo.GetOriginalText());
    DumpLog::GetInstance().AddDesc("window id: " + std::to_string(nodeInfo.GetWindowId()));
    DumpRectNG(nodeInfo.GetRectInScreen());

    DumpLog::GetInstance().AddDesc("enabled: ", BoolToString(nodeInfo.IsEnabled()));
    DumpLog::GetInstance().AddDesc("focusable: ", BoolToString(nodeInfo.IsFocusable()));
    DumpLog::GetInstance().AddDesc("focused: ", BoolToString(nodeInfo.IsFocused()));
    DumpLog::GetInstance().AddDesc("visible: ", BoolToString(nodeInfo.IsVisible()));
    DumpLog::GetInstance().AddDesc("accessibility focused: ", BoolToString(nodeInfo.HasAccessibilityFocus()));
    DumpLog::GetInstance().AddDesc("accessibilityText: " + nodeInfo.GetAccessibilityText());
    DumpLog::GetInstance().AddDesc("accessibilityGroup: " + BoolToString(nodeInfo.GetAccessibilityGroup()));
    DumpLog::GetInstance().AddDesc("accessibilityLevel: " + nodeInfo.GetAccessibilityLevel());
    DumpLog::GetInstance().AddDesc("accessibilityDescription: " + nodeInfo.GetDescriptionInfo());
    DumpLog::GetInstance().AddDesc("hitTestBehavior: " + nodeInfo.GetHitTestBehavior());

    DumpLog::GetInstance().AddDesc("inspector key: ", nodeInfo.GetInspectorKey());
    DumpLog::GetInstance().AddDesc("bundle name: ", nodeInfo.GetBundleName());
    DumpLog::GetInstance().AddDesc("page id: " + std::to_string(nodeInfo.GetPageId()));
    DumpLog::GetInstance().AddDesc("page path: ", nodeInfo.GetPagePath());
    DumpLog::GetInstance().AddDesc("is valid element: ", BoolToString(nodeInfo.IsValidElement()));
    DumpLog::GetInstance().AddDesc("resource name: ", nodeInfo.GetComponentResourceId());

    DumpLog::GetInstance().AddDesc("clickable: ", BoolToString(nodeInfo.IsClickable()));
    DumpLog::GetInstance().AddDesc("long clickable: ", BoolToString(nodeInfo.IsLongClickable()));
    DumpLog::GetInstance().AddDesc("popup supported: ", BoolToString(nodeInfo.IsPopupSupported()));
    DumpLog::GetInstance().AddDesc("zindex: ", std::to_string(nodeInfo.GetZIndex()));
}

void JsAccessibilityManager::UpdateVirtualNodeFocus()
{
    auto frameNode = lastFrameNode_.Upgrade();
    CHECK_NULL_VOID(frameNode);
    if (!frameNode->IsAccessibilityVirtualNode()) {
        return;
    }
    RefPtr<NG::RenderContext> renderContext;
    if (frameNode->GetAccessibilityId() == currentFocusNodeId_) {
        auto parentUinode = frameNode->GetVirtualNodeParent().Upgrade();
        CHECK_NULL_VOID(parentUinode);
        auto parentFrame = AceType::DynamicCast<NG::FrameNode>(parentUinode);
        CHECK_NULL_VOID(parentFrame);
        renderContext = parentFrame->GetRenderContext();
        CHECK_NULL_VOID(renderContext);
        renderContext->UpdateAccessibilityFocus(false);
        auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        CHECK_NULL_VOID(accessibilityProperty);
        if (accessibilityProperty->IsMatchAccessibilityResponseRegion(true)) {
            auto rectInt = accessibilityProperty->GetAccessibilityResponseRegionRect(true);
            renderContext->UpdateAccessibilityFocusRect(rectInt);
        } else {
            renderContext->UpdateAccessibilityFocusRect(GetFrameNodeRectInt(frameNode));
        }
        renderContext->UpdateAccessibilityFocus(true, frameNode->GetAccessibilityId());
        accessibilityProperty->SetAccessibilityFocusState(true);
    }
}

JsAccessibilityManager::~JsAccessibilityManager()
{
    UnsubscribeStateObserver(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);
    UnsubscribeStateObserver(AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED);
    UnsubscribeStateObserver(AccessibilityStateEventType::EVENT_CONFIG_EVENT_CHANGED);

    DeregisterInteractionOperation();
}
void JsAccessibilityManager::ToastAccessibilityConfigObserver::OnConfigChanged(
    const AccessibilityConfig::CONFIG_ID id, const AccessibilityConfig::ConfigValue& value)
{
    TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY, "accessibility content timeout changed:%{public}u", value.contentTimeout);
    AceApplicationInfo::GetInstance().SetBarrierfreeDuration((int32_t)value.contentTimeout);
}

bool JsAccessibilityManager::SubscribeToastObserver()
{
    if (!toastObserver_) {
        toastObserver_ = std::make_shared<ToastAccessibilityConfigObserver>();
    }
    CHECK_NULL_RETURN(toastObserver_, false);
    auto& config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool isSuccess = config.InitializeContext();
    if (!isSuccess) {
        return false;
    }
    config.SubscribeConfigObserver(CONFIG_CONTENT_TIMEOUT, toastObserver_);
    return true;
}

bool JsAccessibilityManager::UnsubscribeToastObserver()
{
    CHECK_NULL_RETURN(toastObserver_, false);
    auto& config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool isSuccess = config.InitializeContext();
    if (!isSuccess) {
        return false;
    }
    config.UnsubscribeConfigObserver(CONFIG_CONTENT_TIMEOUT, toastObserver_);
    return true;
}

bool JsAccessibilityManager::SubscribeStateObserver(uint32_t eventType)
{
    if (!stateObserver_[eventType]) {
        stateObserver_[eventType] = std::make_shared<JsAccessibilityStateObserver>();
    }

    stateObserver_[eventType]->SetAccessibilityManager(WeakClaim(this));
    stateObserver_[eventType]->SetPipeline(context_);
    stateObserver_[eventType]->SetEventType(eventType);

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(instance, false);
    Accessibility::RetError ret = instance->SubscribeStateObserver(stateObserver_[eventType], eventType);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, " the result of SubscribeStateObserver:%{public}d, eventType:%{public}u",
        ret, eventType);
    return ret == RET_OK;
}

bool JsAccessibilityManager::UnsubscribeStateObserver(uint32_t eventType)
{
    CHECK_NULL_RETURN(stateObserver_[eventType], false);
    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(instance, false);
    Accessibility::RetError ret = instance->UnsubscribeStateObserver(stateObserver_[eventType], eventType);
    return ret == RET_OK;
}

AccessibilityWorkMode JsAccessibilityManager::GenerateAccessibilityWorkMode()
{
    AccessibilityWorkMode accessibilityWorkMode;
    auto client = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(client, accessibilityWorkMode);
    auto ret = client->IsTouchExplorationEnabled(accessibilityWorkMode.isTouchExplorationEnabled);
    if (ret != RET_OK) {
        accessibilityWorkMode.isTouchExplorationEnabled = true;
    }
    return accessibilityWorkMode;
}

void JsAccessibilityManager::UpdateAccessibilityNextFocusIdMap(int32_t containerId,
                                                               const std::string& nextFocusInspectorKey,
                                                               int64_t preAccessibilityId)
{
    std::lock_guard<std::mutex> lock(nextFocusMapWithSubWindowMutex_);

    auto it = nextFocusMapWithSubWindow_.find(containerId);
    if (it != nextFocusMapWithSubWindow_.end()) {
        HandleExistingContext(it, nextFocusInspectorKey, preAccessibilityId);
    } else {
        if (!nextFocusInspectorKey.empty()) {
            nextFocusMapWithSubWindow_.emplace(
                containerId, std::map<std::string, int64_t> {{ nextFocusInspectorKey, preAccessibilityId }});
        }
    }
}

void JsAccessibilityManager::InitializeCallback()
{
    if (IsRegister()) {
        return;
    }

    auto pipelineContext = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(pipelineContext);
    windowId_ = pipelineContext->GetWindowId();

    auto client = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_VOID(client);
    bool isEnabled = false;
    client->IsEnabled(isEnabled);
    AceApplicationInfo::GetInstance().SetAccessibilityEnabled(isEnabled);
    bool isScreenReadEnabled = false;
    client->IsScreenReaderEnabled(isScreenReadEnabled);
    AceApplicationInfo::GetInstance().SetAccessibilityScreenReadEnabled(isScreenReadEnabled);

    std::vector<uint32_t> needEvents;
    client->SearchNeedEvents(needEvents);
    UpdateEventWhiteList(needEvents);

    SubscribeStateObserver(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);
    SubscribeStateObserver(AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED);
    SubscribeStateObserver(AccessibilityStateEventType::EVENT_CONFIG_EVENT_CHANGED);

    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    if (container != nullptr && container->IsDynamicRender()) {
        RegisterDynamicRenderGetParentRectHandler();
        return;
    }

    if (pipelineContext->IsFormRender() || pipelineContext->IsJsCard() || pipelineContext->IsJsPlugin()) {
        return;
    }

    if (container != nullptr && container->IsUIExtensionWindow()) {
        pipelineContext->AddUIExtensionCallbackEvent(OHOS::Ace::NG::UIExtCallbackEventId::ON_UEA_ACCESSIBILITY_READY);
        RegisterUIExtBusinessConsumeCallback();
        RegisterGetParentRectHandler();
        if (container->GetUIContentType() == UIContentType::PREVIEW_UI_EXTENSION) {
            SetIsIgnoreAllAction(true);
        }
        return;
    }

    if (isEnabled) {
        RegisterInteractionOperation(windowId_);
    }
}

void JsAccessibilityManager::RegisterGetParentRectHandler()
{
    auto accessibilityGetParentRect = [weak = WeakClaim(this)](AccessibilityParentRectInfo& parentRectInfo) {
        auto jsAccessibilityManager = weak.Upgrade();
        CHECK_NULL_VOID(jsAccessibilityManager);
        auto uecRectInfo = jsAccessibilityManager->GetUECAccessibilityParentRectInfo();
        if (uecRectInfo.isChanged) {
            parentRectInfo.left = uecRectInfo.left;
            parentRectInfo.top = uecRectInfo.top;
            parentRectInfo.scaleX = uecRectInfo.scaleX;
            parentRectInfo.scaleY = uecRectInfo.scaleY;
            parentRectInfo.rotateTransform = uecRectInfo.rotateTransform;
        } else {
            auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(jsAccessibilityManager->context_.Upgrade());
            CHECK_NULL_VOID(ngPipeline);
            parentRectInfo.left = jsAccessibilityManager->GetWindowLeft(ngPipeline->GetWindowId());
            parentRectInfo.top = jsAccessibilityManager->GetWindowTop(ngPipeline->GetWindowId());
            auto container = Container::CurrentSafely();
            if (container) {
                auto windowScale = container->GetWindowScale();
                parentRectInfo.scaleX = windowScale;
                parentRectInfo.scaleY = windowScale;
            }
        }
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "Get host rect [scaleX:%{public}f, scaleY:%{public}f].", parentRectInfo.scaleX, parentRectInfo.scaleY);
    };
    SetAccessibilityGetParentRectHandler(accessibilityGetParentRect);
}

void JsAccessibilityManager::RegisterUIExtBusinessConsumeCallback()
{
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context_.Upgrade());
    CHECK_NULL_VOID(ngPipeline);
    auto uiExtManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtManager);
    auto updateAccessibilityParentRectCallback = [weak = WeakClaim(this)](const AAFwk::Want& data) -> int32_t {
        auto jsAccessibilityManager = weak.Upgrade();
        CHECK_NULL_RETURN(jsAccessibilityManager, -1);
        AccessibilityParentRectInfo info;
        info.left = data.GetIntParam("left", 0);
        info.top = data.GetIntParam("top", 0);
        info.scaleX = data.GetFloatParam("scaleX", 1.0f);
        info.scaleY = data.GetFloatParam("scaleY", 1.0f);
        RotateTransform rotateTransform(data.GetIntParam("rotateDegree", 0), data.GetIntParam("centerX", 0),
            data.GetIntParam("centerY", 0), data.GetIntParam("innerCenterX", 0),
            data.GetIntParam("innerCenterY", 0));
        info.rotateTransform = rotateTransform;
        info.isChanged = true;
        jsAccessibilityManager->UpdateUECAccessibilityParentRectInfo(info);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "Update UIExt Accessiblity [scaleX:%{public}f, scaleY:%{public}f].", info.scaleX, info.scaleY);
        // go on transfer info to next uiextension
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(jsAccessibilityManager->context_.Upgrade());
        CHECK_NULL_RETURN(ngPipeline, -1);
        auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
        CHECK_NULL_RETURN(uiExtensionManager, -1);
        uiExtensionManager->TransferAccessibilityRectInfo();
        return 0;
    };
    uiExtManager->RegisterBusinessDataConsumeCallback(
        Ace::NG::UIContentBusinessCode::TRANSFORM_PARAM, updateAccessibilityParentRectCallback);
}

AccessibilityParentRectInfo JsAccessibilityManager::GetUECAccessibilityParentRectInfo() const
{
    return uecRectInfo_;
}

void JsAccessibilityManager::UpdateUECAccessibilityParentRectInfo(const AccessibilityParentRectInfo& info)
{
    uecRectInfo_ = info;
}

void JsAccessibilityManager::RegisterDynamicRenderGetParentRectHandler()
{
    auto accessibilityGetParentRect = [weak = WeakClaim(this)](AccessibilityParentRectInfo& parentRectInfo) {
        auto jsAccessibilityManager = weak.Upgrade();
        CHECK_NULL_VOID(jsAccessibilityManager);
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(jsAccessibilityManager->context_.Upgrade());
        CHECK_NULL_VOID(ngPipeline);
        auto container = Platform::AceContainer::GetContainer(ngPipeline->GetInstanceId());
        CHECK_NULL_VOID(container);
        auto containerHandler = container->GetContainerHandler();
        CHECK_NULL_VOID(containerHandler);

        HandlerData data = {
            .actionCode = static_cast<int32_t>(NG::DynamicContainerHandleAction::ACCESSIBILITY_GET_RECT) };
        HandlerReply reply;

        containerHandler->SendDataToHost(data, reply);

        parentRectInfo.left = reply.GetParam<int32_t>("left", 0);
        parentRectInfo.top = reply.GetParam<int32_t>("top", 0);
        parentRectInfo.scaleX = reply.GetParam<float>("scaleX", 1.0f);
        parentRectInfo.scaleY = reply.GetParam<float>("scaleY", 1.0f);
        RotateTransform rotateData(reply.GetParam<int32_t>("rotateDegree", 0),
                                   reply.GetParam<int32_t>("centerX", 0),
                                   reply.GetParam<int32_t>("centerY", 0),
                                   reply.GetParam<int32_t>("innerCenterX", 0),
                                   reply.GetParam<int32_t>("innerCenterY", 0));
        parentRectInfo.rotateTransform = rotateData;
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "Get DC host rect [left:%{public}d, top:%{public}d, scaleX:%{public}f, scaleY:%{public}f].",
            parentRectInfo.left, parentRectInfo.top, parentRectInfo.scaleX, parentRectInfo.scaleY);
    };
    SetAccessibilityGetParentRectHandler(accessibilityGetParentRect);
}


namespace {
    const char FULL_SILENT[] = "FULL_SILENT";

    bool IsPageEvent(const Accessibility::EventType& eventType)
    {
        return eventType && (eventType == Accessibility::EventType::TYPE_PAGE_STATE_UPDATE
            || eventType == Accessibility::EventType::TYPE_PAGE_CONTENT_UPDATE
            || eventType == Accessibility::EventType::TYPE_PAGE_OPEN
            || eventType == Accessibility::EventType::TYPE_PAGE_CLOSE);
    }

    bool IsRootOrPageComponent(const std::string& componentType)
    {
        return !componentType.empty() && (componentType == V2::ROOT_ETS_TAG || componentType == V2::PAGE_ETS_TAG);
    }

    bool CheckSendAccessibilityEventByPageMode(
        const std::string& pageMode, const std::string& componentType, const int32_t pageId)
    {
        if (pageMode == FULL_SILENT) {
            TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
                "current pageMode:%{public}s, drop page event", pageMode.c_str());
            return false;
        }
        if (IsRootOrPageComponent(componentType) && pageId <= 1) {
            TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
                "current pageMode:%{public}s componentType:%{public}s pageId:%{public}d, drop page event",
                pageMode.c_str(), componentType.c_str(), pageId);
            return false;
        }
        return true;
    }

    bool CheckExtensionComponentReadyByPageId(
        const int32_t pageId, const std::vector<std::pair<WeakPtr<NG::FrameNode>, bool>>& nodeVec)
    {
        for (const auto& [node, status] : nodeVec) {
            auto frameNode = node.Upgrade();
            if (frameNode && (frameNode->GetPageId() == pageId && !status)) {
                return false;
            }
        }
        return true;
    }

    bool IsDelNode(const WeakPtr<NG::FrameNode>& node)
    {
        auto frameNode = node.Upgrade();
        CHECK_NULL_RETURN(frameNode, true);
        auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        CHECK_NULL_RETURN(accessibilityProperty, true);
        return !accessibilityProperty->HasAccessibilitySamePage();
    }

    void UpdateExtensionComponentStatusVec(std::vector<std::pair<WeakPtr<NG::FrameNode>, bool>>& nodeVec)
    {
        for (auto it = nodeVec.begin(); it != nodeVec.end();) {
            if (IsDelNode(it->first)) {
                it = nodeVec.erase(it);
            } else {
                ++it;
            }
        }
    }

    void ClearDefaultFocusList(std::list<WeakPtr<NG::FrameNode>>& nodeList)
    {
        for (auto it = nodeList.begin(); it != nodeList.end();) {
            auto node = it->Upgrade();
            if (!node) {
                it = nodeList.erase(it);
            } else {
                ++it;
            }
        }
    }

    Accessibility::EventType GetEventTypeByAccessibilityEvent(const AccessibilityEvent& accessibilityEvent)
    {
        Accessibility::EventType type = Accessibility::EventType::TYPE_VIEW_INVALID;
        if (accessibilityEvent.type != AccessibilityEventType::UNKNOWN) {
            type = ConvertAceEventType(accessibilityEvent.type);
        } else {
            type = ConvertStrToEventType(accessibilityEvent.eventType);
        }
        return type;
    }
}

bool JsAccessibilityManager::CachePageEventByController(
    const AccessibilityEvent& accessibilityEvent,
    const std::string& componentType,
    int32_t pageId,
    int32_t containerId)
{
    auto accessibilityWorkMode = GenerateAccessibilityWorkMode();
    if (!accessibilityWorkMode.isTouchExplorationEnabled) {
        return false;
    }

    AccessibilityEvent event = accessibilityEvent;
    event.componentType = componentType;
    auto eventType = GetEventTypeByAccessibilityEvent(event);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
        "cache page event from pageEventController, componentType:%{public}s event:%{public}d nodeId:%{public}"
        PRId64, event.componentType.c_str(), eventType, event.nodeId);
    pageController_.AddAccessibilityEvent(containerId, pageId, event);
    return true;
}

bool JsAccessibilityManager::IsSendAccessibilityEvent(const AccessibilityEvent& accessibilityEvent)
{
    if (!IsPageEvent(GetEventTypeByAccessibilityEvent(accessibilityEvent))) {
        return true;
    }
    GetInfoByNodeId infoOfNode;
    auto pipelineContext = GetPipelineContext().Upgrade();
    if (!pipelineContext) {
        return IsSendAccessibilityEventForHost(accessibilityEvent, infoOfNode.componentType, infoOfNode.pageId);
    }

    GetComponentTypeAndPageIdByNodeId(accessibilityEvent.nodeId, pipelineContext, infoOfNode);
    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    if (container != nullptr && container->IsUIExtensionWindow()) {
        if (!IsSendAccessibilityEventForUEA(accessibilityEvent, infoOfNode.componentType, infoOfNode.pageId)) {
            return false;
        }
        pageController_.Update();
        if (!pageController_.CheckEmpty(infoOfNode.nodeInstanceId)) {
            auto cached = CachePageEventByController(
                accessibilityEvent, infoOfNode.componentType, infoOfNode.pageId, infoOfNode.nodeInstanceId);
            return !cached;
        }
    }
    return IsSendAccessibilityEventForHost(accessibilityEvent, infoOfNode.componentType, infoOfNode.pageId);
}

void JsAccessibilityManager::ReleaseCacheEvent()
{
    if (eventQueue_.empty()) {
        return;
    }
    while (!eventQueue_.empty()) {
        auto event = eventQueue_.front();
        SendAccessibilityAsyncEvent(event);
        eventQueue_.pop();
    }
}

bool JsAccessibilityManager::IsSendAccessibilityEventForUEA(
    const AccessibilityEvent& accessibilityEvent, const std::string& componentType, const int32_t pageId)
{
    if (!pageMode_.has_value()) {
        eventQueue_.push(accessibilityEvent);
        return false;
    }
    const auto& pageMode = pageMode_.value();
    if (pageMode.empty()) {
        if (treeId_ == -1) {
            cacheEventVec_.push_back(accessibilityEvent);
            return false;
        }
        return true;
    }
    if (!CheckSendAccessibilityEventByPageMode(pageMode, componentType, pageId)) {
        return false;
    }
    if (treeId_ == -1) {
        cacheEventVec_.push_back(accessibilityEvent);
        return false;
    }
    return true;
}

bool JsAccessibilityManager::IsSendAccessibilityEventForHost(
    const AccessibilityEvent& accessibilityEvent, const std::string& componentType, const int32_t pageId)
{
    UpdateExtensionComponentStatusVec(extensionComponentStatusVec_);
    ClearDefaultFocusList(defaultFocusList_);
    RefPtr<NG::FrameNode> findeNode;
    auto ngPipeline = FindPipelineByElementId(accessibilityEvent.nodeId, findeNode);

    pageController_.Update();
    bool isPageEventControllerEmpty = ngPipeline ? pageController_.CheckEmpty(ngPipeline->GetInstanceId()) : true;
    if (extensionComponentStatusVec_.empty() && isPageEventControllerEmpty) {
        return true;
    }

    for (const auto& [node, status] : extensionComponentStatusVec_) {
        auto frameNode = node.Upgrade();
        CHECK_NULL_CONTINUE(frameNode);
        auto nodePageId = frameNode->GetPageId();
        if (!pageIdEventMap_.count(nodePageId)) {
            pageIdEventMap_[nodePageId] = std::nullopt;
        }
    }
    if (!CheckExtensionComponentReadyByPageId(pageId, extensionComponentStatusVec_)) {
        if (pageIdEventMap_.count(pageId) && pageIdEventMap_[pageId].has_value()) {
            auto event = pageIdEventMap_[pageId].value();
            auto eventType = GetEventTypeByAccessibilityEvent(event);
            TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
                "override the event, componentType:%{public}s event:%{public}d nodeId:%{public}" PRId64,
                event.componentType.c_str(), eventType, event.nodeId);
        }
        AccessibilityEvent event = accessibilityEvent;
        event.componentType = componentType;
        pageIdEventMap_[pageId] = event;
        return false;
    } else if (!isPageEventControllerEmpty) {
        auto containerId = ngPipeline ? ngPipeline->GetInstanceId() : 0;
        auto cached = CachePageEventByController(accessibilityEvent, componentType, pageId, containerId);
        return !cached;
    }
    return true;
}

void JsAccessibilityManager::GetComponentTypeAndPageIdByNodeId(
    const int64_t nodeId,
    const RefPtr<PipelineBase>& context,
    GetInfoByNodeId& infoOfNode)
{
    CHECK_NULL_VOID(context);
    if (AceType::InstanceOf<NG::PipelineContext>(context)) {
        RefPtr<NG::FrameNode> node;
        auto nodePipeline = FindPipelineByElementId(nodeId, node);
        CHECK_NULL_VOID(node);
        infoOfNode.componentType = node->GetTag();
        infoOfNode.pageId = node->GetPageId();
        infoOfNode.nodeInstanceId = nodePipeline ? nodePipeline->GetInstanceId() : -1;
    } else {
        auto node = GetAccessibilityNodeFromPage(nodeId);
        CHECK_NULL_VOID(node);
        infoOfNode.componentType = node->GetTag();
        infoOfNode.pageId = node->GetPageId();
        infoOfNode.nodeInstanceId = -1;
    }
}

void JsAccessibilityManager::SendCacheAccessibilityEvent(int32_t instanceId)
{
    auto container = Platform::AceContainer::GetContainer(instanceId);
    if (container == nullptr || !container->IsUIExtensionWindow()) {
        return;
    }

    if (!cacheEventVec_.empty()) {
        for (const auto& event : cacheEventVec_) {
            SendAccessibilityAsyncEventInner(event);
        }
        cacheEventVec_.clear();
    }
}

void JsAccessibilityManager::SendCacheAccessibilityEventForHost(const int32_t pageId)
{
    if (!CheckExtensionComponentReadyByPageId(pageId, extensionComponentStatusVec_)) {
        return;
    }

    if (pageIdEventMap_.count(pageId) && pageIdEventMap_[pageId].has_value()) {
        auto event = pageIdEventMap_[pageId].value();
        SendAccessibilityAsyncEventInner(event);
        pageIdEventMap_[pageId] = std::nullopt;
    }
}

bool JsAccessibilityManager::CheckPageEventValidInCache(int32_t containerId)
{
    return pageController_.HasAnyAccessibilityEvent(containerId);
}

bool JsAccessibilityManager::CheckPageEventByPageInCache(int32_t containerId, int32_t pageId)
{
    return pageController_.HasAccessibilityEvent(containerId, pageId);
}

void JsAccessibilityManager::ReleaseAccessibilityEventList(
    const std::list<std::pair<int32_t, AccessibilityEvent>>& eventList)
{
    for (auto& pair : eventList) {
        auto eventType = GetEventTypeByAccessibilityEvent(pair.second);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "release page event from pageEventController, componentType:%{public}s event:%{public}d nodeId:%{public}"
            PRId64, pair.second.componentType.c_str(), eventType, pair.second.nodeId);
        SendAccessibilityAsyncEventInner(pair.second);
    }
}

void JsAccessibilityManager::ReleaseAllCacheAccessibilityEvent(int32_t containerId)
{
    std::list<std::pair<int32_t, AccessibilityEvent>> eventList;
    pageController_.ReleaseAllAccessibilityEvent(containerId, eventList);
    ReleaseAccessibilityEventList(eventList);
}

void JsAccessibilityManager::ReleaseCacheAccessibilityEvent(int32_t containerId, int32_t pageId)
{
    std::list<std::pair<int32_t, AccessibilityEvent>> eventList;
    pageController_.ReleaseAccessibilityEvent(containerId, pageId, eventList);
    ReleaseAccessibilityEventList(eventList);
    if ((pageId == 0) || (pageId == -1)) {
        ReleaseAllCacheAccessibilityEvent(containerId);
    }
}

void JsAccessibilityManager::ReleasePageEvent(const RefPtr<NG::FrameNode>& node, bool deleteController, bool releaseAll)
{
    CHECK_NULL_VOID(node);
    auto pipeline = node->GetContextRefPtr();
    CHECK_NULL_VOID(pipeline);
    auto containerId = pipeline->GetInstanceId();
    if (pageController_.CheckNode(node, deleteController)) {
        ReleaseCacheAccessibilityEvent(containerId, node->GetPageId());
    }

    if (releaseAll) {
        pageController_.DeleteInstanceNodeAll(node);
        ReleaseAllCacheAccessibilityEvent(containerId);
    }
}

void JsAccessibilityManager::AddToPageEventController(const RefPtr<NG::FrameNode>& node)
{
    pageController_.Add(node);
}

bool JsAccessibilityManager::CheckPageEventCached(const RefPtr<NG::FrameNode>& node, bool onlyCurrentPage)
{
    CHECK_NULL_RETURN(node, false);
    auto pipeline = node->GetContextRefPtr();
    CHECK_NULL_RETURN(pipeline, false);
    auto containerId = pipeline->GetInstanceId();

    if (onlyCurrentPage == false) {
        auto ret = CheckPageEventValidInCache(containerId);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "CheckPageEventCached not onlyCurrentPage return %{public}d containerId %{public}d control Id :%{public}"
            PRId64, ret, containerId, node->GetAccessibilityId());
        return ret;
    }

    auto pageId = node->GetPageId();
    if ((pageId == 0) || (pageId == -1)) {
        auto ret = CheckPageEventValidInCache(containerId);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "CheckPageEventCached pageId %{public}d return %{public}d containerId %{public}d control Id :%{public}"
            PRId64, pageId, ret, containerId, node->GetAccessibilityId());
        return ret;
    }
    auto ret = CheckPageEventByPageInCache(containerId, pageId);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
        "CheckPageEventCached pageId %{public}d return %{public}d containerId %{public}d control Id :%{public}"
        PRId64, pageId, ret, containerId, node->GetAccessibilityId());
    return ret;
}


void JsAccessibilityManager::AddFrameNodeToUecStatusVec(const RefPtr<NG::FrameNode>& node)
{
    extensionComponentStatusVec_.emplace_back(WeakPtr(node), false);
}

void JsAccessibilityManager::AddFrameNodeToDefaultFocusList(const RefPtr<NG::FrameNode>& node, bool isFocus)
{
    if (isFocus) {
        AddDefaultFocusNode(node);
        return;
    }
    EraseDefaultFocusNode(node);
}

void JsAccessibilityManager::AddDefaultFocusNode(const RefPtr<NG::FrameNode>& defaultFocusNode)
{
    CHECK_NULL_VOID(defaultFocusNode);
    auto nodeId = defaultFocusNode->GetId();
    for (const auto& node : defaultFocusList_) {
        auto frameNode = node.Upgrade();
        if (frameNode && (frameNode->GetId() == nodeId)) {
            return;
        }
    }
    TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
        "add frameNode to defaultFocusList, nodeId: %{public}d.", nodeId);
    defaultFocusList_.push_back(WeakPtr(defaultFocusNode));
}

void JsAccessibilityManager::EraseDefaultFocusNode(const RefPtr<NG::FrameNode>& defaultFocusNode)
{
    CHECK_NULL_VOID(defaultFocusNode);
    auto nodeId = defaultFocusNode->GetId();
    for (auto it = defaultFocusList_.begin(); it != defaultFocusList_.end();) {
        auto node = it->Upgrade();
        if (node && (node->GetId() == nodeId)) {
            it = defaultFocusList_.erase(it);
            TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
                "erase frameNode from defaultFocusList, nodeId: %{public}d.", nodeId);
            return;
        } else {
            ++it;
        }
    }
}

void JsAccessibilityManager::AddHoverTransparentCallback(const RefPtr<NG::FrameNode>& node)
{
    hoverTransparentCallbackController_.AddToHoverTransparentCallbackList(node);
}

bool JsAccessibilityManager::CheckHoverTransparentCallbackListEmpty(int32_t containerId)
{
    return hoverTransparentCallbackController_.CheckHoverTransparentCallbackListEmpty(containerId);
}

int64_t JsAccessibilityManager::CheckAndGetEmbedFrameNode(const RefPtr<NG::FrameNode>& node)
{
    auto surfaceId = GetSurfaceIdByEmbedNode(node);
    if (surfaceId == "0") {
        return INVALID_NODE_ID;
    }
#ifdef WEB_SUPPORTED
    return GetWebAccessibilityIdBySurfaceId(surfaceId);
#else
    return INVALID_NODE_ID;
#endif
}

void JsAccessibilityManager::RegisterUIExtGetPageModeCallback(RefPtr<NG::UIExtensionManager>& uiExtManager)
{
    CHECK_NULL_VOID(uiExtManager);
    auto callback = [weak = WeakClaim(this)](const AAFwk::Want& data) -> int32_t {
        if (!data.HasParameter("pageMode")) {
            return -1;
        }
        auto accessibilityManager = weak.Upgrade();
        CHECK_NULL_RETURN(accessibilityManager, -1);

        auto pageMode = data.GetStringParam("pageMode");
        TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
            "host send pageMode to uea, pageMode: %{public}s.", pageMode.c_str());
        accessibilityManager->UpdatePageMode(pageMode);
        accessibilityManager->ReleaseCacheEvent();
        return 0;
    };
    uiExtManager->RegisterBusinessDataConsumeCallback(NG::UIContentBusinessCode::SEND_PAGE_MODE_TO_UEA, callback);
}

void JsAccessibilityManager::UpdateFrameNodeState(int32_t nodeId)
{
    for (auto& [node, status] : extensionComponentStatusVec_) {
        auto frameNode = node.Upgrade();
        if (frameNode && (frameNode->GetId() == nodeId)) {
            status = true;
            SendCacheAccessibilityEventForHost(frameNode->GetPageId());
            return;
        }
    }
}

bool JsAccessibilityManager::SendAccessibilitySyncEvent(
    const AccessibilityEvent& accessibilityEvent, AccessibilityEventInfo eventInfo)
{
    if (!IsRegister()) {
        return false;
    }
    auto client = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(client, false);
    bool isEnabled = false;
    client->IsEnabled(isEnabled);
    if (!isEnabled) {
        return false;
    }
    AccessibilityElementInfo info = eventInfo.GetElementInfo();
    int64_t elementId = eventInfo.GetAccessibilityId();
    if (info.GetBelongTreeId() > 0) {
        AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(info.GetBelongTreeId(), elementId);
    }
    eventInfo.SetSource(elementId);
    UpdateElementInfoTreeId(info);
    eventInfo.SetElementInfo(info);
    eventInfo.SetPageId(info.GetPageId());
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
        "send accessibility componentType:%{public}s event:%{public}d accessibilityId:%{public}" PRId64,
        eventInfo.GetComponentType().c_str(), eventInfo.GetEventType(), eventInfo.GetAccessibilityId());
    if (IsTagInEmbedComponent(eventInfo.GetComponentType()) &&
        eventInfo.GetEventType() == TYPE_VIEW_HOVER_ENTER_EVENT) {
        TAG_LOGI(AceLogTag::ACE_WEB, "dont hover on embeddedObject without same layer rendering");
        return false;
    }
    return client->SendEvent(eventInfo);
}

bool JsAccessibilityManager::TransferAccessibilityAsyncEvent(
    const AccessibilityEventInfo& eventInfo, int64_t uiExtensionOffset)
{
#ifdef WINDOW_SCENE_SUPPORTED
    auto client = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(client, false);
    bool isEnabled = false;
    client->IsEnabled(isEnabled);
    if (!isEnabled) {
        return false;
    }

    auto pipeline = context_.Upgrade();
    CHECK_NULL_RETURN(pipeline, false);
    RefPtr<NG::PipelineContext> ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    CHECK_NULL_RETURN(ngPipeline, false);
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_RETURN(uiExtensionManager, false);
    auto container = Container::GetContainer(ngPipeline->GetInstanceId());
    bool isDynamicRender = container && container->IsDynamicRender() &&
        container->GetUIContentType() == UIContentType::ISOLATED_COMPONENT;
    if (!IsRegister() && !isDynamicRender) {
        return false;
    }
    AccessibilityEventInfo eventInfoNew = eventInfo;
    if (isDynamicRender) {
        auto focusedContainer = Container::GetFocused();
        if (focusedContainer) {
            eventInfoNew.SetWindowId(focusedContainer->GetWindowId());
        }
    }
    eventInfoNew.SetSource(uiExtensionOffset * GetUiextensionId() + eventInfo.GetViewId());
    AccessibilityElementInfo elementInfo;
    FillElementInfo(eventInfoNew.GetAccessibilityId(), elementInfo, pipeline, Claim(this),
        FillEventInfoParam { eventInfoNew.GetAccessibilityId(), -1,
            static_cast<int32_t>(eventInfoNew.GetWindowId()) });
    eventInfoNew.SetElementInfo(elementInfo);
    TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY, "send accessibility event:%{public}d accessibilityId:%{public}" PRId64,
        eventInfoNew.GetEventType(), eventInfoNew.GetAccessibilityId());
    return client->SendEvent(eventInfoNew);
#endif
    return false;
}

void JsAccessibilityManager::SendExtensionAccessibilityEvent(
    const AccessibilityEventInfo& eventInfo, int64_t uiExtensionOffset)
{
    TransferAccessibilityAsyncEvent(eventInfo, uiExtensionOffset);
}

void JsAccessibilityManager::FillEventInfoWithNode(
    const RefPtr<NG::FrameNode>& node,
    AccessibilityEventInfo& eventInfo,
    const RefPtr<NG::PipelineContext>& context,
    int64_t elementId)
{
    CHECK_NULL_VOID(node);
    eventInfo.SetComponentType(node->GetTag());
    eventInfo.SetPageId(node->GetPageId());
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    eventInfo.SetItemCounts(accessibilityProperty->GetCollectionItemCounts());
    eventInfo.SetBeginIndex(accessibilityProperty->GetBeginIndex());
    eventInfo.SetEndIndex(accessibilityProperty->GetEndIndex());
    AccessibilityElementInfo elementInfo;

    CommonProperty commonProperty;
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    GenerateCommonProperty(context, commonProperty, mainContext, node);
    UpdateAccessibilityElementInfo(node, commonProperty, elementInfo, context);
    elementInfo.SetWindowId(eventInfo.GetWindowId());
    eventInfo.AddContent(elementInfo.GetContent());
    eventInfo.SetElementInfo(elementInfo);
}

int64_t JsAccessibilityManager::GetDelayTimeBeforeSendEvent(
    const AccessibilityEvent& accessibilityEvent,
    const RefPtr<AceType>& node)
{
    if (accessibilityEvent.type != AccessibilityEventType::CLICK) {
        return 0;
    }

    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    if (frameNode) {
        if (IsUserCheckedOrSelected(frameNode)) {
            return DELAY_SEND_EVENT_MILLISECOND;
        }
    } else {
        auto context = GetPipelineContext().Upgrade();
        if (!AceType::InstanceOf<NG::PipelineContext>(context)) {
            return 0;
        }
        RefPtr<NG::FrameNode> findeNode;
        auto ngPipeline = FindPipelineByElementId(accessibilityEvent.nodeId, findeNode);
        if ((findeNode) && IsUserCheckedOrSelected(findeNode)) {
            return DELAY_SEND_EVENT_MILLISECOND;
        }
    }

    return 0;
}

bool JsAccessibilityManager::IsEventIgnoredByWorkMode(const AccessibilityEvent& accessibilityEvent)
{
    auto accessibilityWorkMode = GenerateAccessibilityWorkMode();
    if (!accessibilityWorkMode.isTouchExplorationEnabled) {
        switch (accessibilityEvent.type) {
            case AccessibilityEventType::ELEMENT_INFO_CHANGE:
            case AccessibilityEventType::TEXT_CHANGE:
            case AccessibilityEventType::FOCUS:
            case AccessibilityEventType::SCROLLING_EVENT:
                return true;
            default:
                return false;
        }
    }
    return false;
}

void JsAccessibilityManager::SendEventToAccessibilityWithNode(
    const AccessibilityEvent& accessibilityEvent, const RefPtr<AceType>& node, const RefPtr<PipelineBase>& context)
{
    auto eventType = GetEventTypeByAccessibilityEvent(accessibilityEvent);
    if (!CheckWhiteList(eventType)) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "send accessibility check whitelist failed event:%{public}d" PRId64, eventType);
        return;
    }

    if (IsEventIgnoredByWorkMode(accessibilityEvent) || !IsSendAccessibilityEvent(accessibilityEvent)) {
        return;
    }

    auto delayTime = GetDelayTimeBeforeSendEvent(accessibilityEvent, node);
    if ((delayTime > 0) && context) {
        context->GetTaskExecutor()->PostDelayedTask(
            [weak = WeakClaim(this), accessibilityEvent, node, context] {
                auto jsAccessibilityManager = weak.Upgrade();
                CHECK_NULL_VOID(jsAccessibilityManager);
                jsAccessibilityManager->SendEventToAccessibilityWithNodeInner(accessibilityEvent, node, context);
            },
            TaskExecutor::TaskType::UI, delayTime, "ArkUIAccessibilitySendSyncEventWithDelay");
        return;
    }
    SendEventToAccessibilityWithNodeInner(accessibilityEvent, node, context);
}

void JsAccessibilityManager::SendEventToAccessibilityWithNodeInner(
    const AccessibilityEvent& accessibilityEvent, const RefPtr<AceType>& node, const RefPtr<PipelineBase>& context)
{
    ACE_SCOPED_TRACE("SendAccessibilityAsyncEvent");
    CHECK_NULL_VOID(node);
    CHECK_NULL_VOID(context);
    int32_t windowId = static_cast<int32_t>(context->GetRealHostWindowId());
    if (windowId == 0) {
        return;
    }
    if (!AceType::InstanceOf<NG::FrameNode>(node)) {
        return;
    }
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    CHECK_NULL_VOID(frameNode);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);

    if ((!frameNode->IsActive()) || frameNode->CheckAccessibilityLevelNo()) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "node: %{public}" PRId64 ", is not active or level is no",
            frameNode->GetAccessibilityId());
        return;
    }

    AccessibilityEventInfo eventInfo;

    if (accessibilityEvent.type != AccessibilityEventType::PAGE_CHANGE || accessibilityEvent.windowId == 0) {
        eventInfo.SetWindowId(windowId);
    } else {
        eventInfo.SetWindowId(accessibilityEvent.windowId);
    }
    FillEventInfoWithNode(frameNode, eventInfo, ngPipeline, accessibilityEvent.nodeId);
    if ((ngPipeline != nullptr) && (ngPipeline->IsFormRender())) {
        eventInfo.SetWindowId(static_cast<int32_t>(GetWindowId()));
    }
    GenerateAccessibilityEventInfo(accessibilityEvent, eventInfo);

    auto container = Container::GetContainer(context->GetInstanceId());
    if (container && container->IsDynamicRender() &&
        container->GetUIContentType() == UIContentType::ISOLATED_COMPONENT) {
        SendExtensionAccessibilityEvent(eventInfo, NG::UI_EXTENSION_UNKNOW_ID);
    } else {
        context->GetTaskExecutor()->PostTask(
            [weak = WeakClaim(this), accessibilityEvent, eventInfo] {
                auto jsAccessibilityManager = weak.Upgrade();
                CHECK_NULL_VOID(jsAccessibilityManager);
                jsAccessibilityManager->SendAccessibilitySyncEvent(accessibilityEvent, eventInfo);
            },
            TaskExecutor::TaskType::BACKGROUND, "ArkUIAccessibilitySendSyncEvent");
    }
}

void GetRealEventWindowId(
    const AccessibilityEvent& accessibilityEvent, const RefPtr<NG::PipelineContext>& ngPipeline, uint32_t& windowId)
{
    if ((accessibilityEvent.type == AccessibilityEventType::PAGE_CHANGE && accessibilityEvent.windowId != 0) ||
        accessibilityEvent.windowChangeTypes == WINDOW_UPDATE_ADDED) {
        windowId = accessibilityEvent.windowId;
        return;
    }
    if ((ngPipeline != nullptr) && (ngPipeline->IsFormRender())) {
        return;
    }
    windowId = ngPipeline->GetRealHostWindowId();
}

void JsAccessibilityManager::SendAccessibilityAsyncEvent(const AccessibilityEvent& accessibilityEvent)
{
    auto eventType = GetEventTypeByAccessibilityEvent(accessibilityEvent);
    if (!CheckWhiteList(eventType)) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "send accessibility check whitelist failed event:%{public}d" PRId64, eventType);
        return;
    }
    if (IsEventIgnoredByWorkMode(accessibilityEvent) || !IsSendAccessibilityEvent(accessibilityEvent)) {
        return;
    }

    auto delayTime = GetDelayTimeBeforeSendEvent(accessibilityEvent, nullptr);
    if (delayTime > 0) {
        auto context = GetPipelineContext().Upgrade();
        if (context) {
            context->GetTaskExecutor()->PostDelayedTask(
                [weak = WeakClaim(this), accessibilityEvent] {
                    auto jsAccessibilityManager = weak.Upgrade();
                    CHECK_NULL_VOID(jsAccessibilityManager);
                    jsAccessibilityManager->SendAccessibilityAsyncEventInner(accessibilityEvent);
                },
                TaskExecutor::TaskType::UI, delayTime, "ArkUIAccessibilitySendSyncEventWithDelay");
        }
        return;
    }
    SendAccessibilityAsyncEventInner(accessibilityEvent);
}

void JsAccessibilityManager::SendAccessibilityAsyncEventInner(const AccessibilityEvent& accessibilityEvent)
{
    ACE_ACCESS_SCOPED_TRACE("SendAccessibilityAsyncEvent");
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    int32_t windowId = static_cast<int32_t>(context->GetRealHostWindowId());
    if (windowId == 0) {
        return;
    }
    RefPtr<NG::PipelineContext> ngPipeline;
    AccessibilityEventInfo eventInfo;
    uint32_t realWindowId = GetWindowId();
    if (AceType::InstanceOf<NG::PipelineContext>(context)) {
        RefPtr<NG::FrameNode> node;
        ngPipeline = FindPipelineByElementId(accessibilityEvent.nodeId, node);
        CHECK_NULL_VOID(ngPipeline);
        CHECK_NULL_VOID(node);
        GetRealEventWindowId(accessibilityEvent, ngPipeline, realWindowId);
        FillEventInfo(node, eventInfo, ngPipeline, Claim(this),
            FillEventInfoParam {
                accessibilityEvent.nodeId, accessibilityEvent.stackNodeId, realWindowId });
        eventInfo.SetWindowId(realWindowId);
    } else {
        ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
        auto node = GetAccessibilityNodeFromPage(accessibilityEvent.nodeId);
        CHECK_NULL_VOID(node);
        FillEventInfo(node, eventInfo);
        eventInfo.SetWindowId(windowId);
    }
    if (accessibilityEvent.type == AccessibilityEventType::PAGE_CHANGE && accessibilityEvent.windowId != 0) {
        eventInfo.SetWindowId(accessibilityEvent.windowId);
    }
    if ((ngPipeline != nullptr) && (ngPipeline->IsFormRender())) {
        eventInfo.SetWindowId(static_cast<int32_t>(GetWindowId()));
    }

    GenerateAccessibilityEventInfo(accessibilityEvent, eventInfo);

    auto container = Container::GetContainer(context->GetInstanceId());
    if (container && container->IsDynamicRender() &&
        container->GetUIContentType() == UIContentType::ISOLATED_COMPONENT) {
        SendExtensionAccessibilityEvent(eventInfo, NG::UI_EXTENSION_OFFSET_MAX);
    } else {
        context->GetTaskExecutor()->PostTask(
            [weak = WeakClaim(this), accessibilityEvent, eventInfo] {
                auto jsAccessibilityManager = weak.Upgrade();
                CHECK_NULL_VOID(jsAccessibilityManager);
                jsAccessibilityManager->SendAccessibilitySyncEvent(accessibilityEvent, eventInfo);
            },
            TaskExecutor::TaskType::BACKGROUND, "ArkUIAccessibilitySendSyncEvent");
    }
}

#ifdef WEB_SUPPORTED

void JsAccessibilityManager::SendWebAccessibilityAsyncEvent(
    const AccessibilityEvent& accessibilityEvent, const RefPtr<NG::WebPattern>& webPattern)
{
    ACE_ACCESS_SCOPED_TRACE("SendWebAccessibilityAsyncEvent");
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    int32_t windowId = static_cast<int32_t>(context->GetRealHostWindowId());
    if (windowId == 0) {
        return;
    }

    AccessibilityEventInfo eventInfo;
    RefPtr<NG::PipelineContext> ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context_.Upgrade());
    CHECK_NULL_VOID(ngPipeline);
    FillWebEventInfo(eventInfo, ngPipeline, Claim(this),
        FillEventInfoParam {
                accessibilityEvent.nodeId, accessibilityEvent.stackNodeId, ngPipeline->GetRealHostWindowId() },
        webPattern);
    eventInfo.SetWindowId(ngPipeline->GetRealHostWindowId());

    if (accessibilityEvent.type == AccessibilityEventType::PAGE_CHANGE && accessibilityEvent.windowId != 0) {
        eventInfo.SetWindowId(accessibilityEvent.windowId);
    }

    GenerateAccessibilityEventInfo(accessibilityEvent, eventInfo);

    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), accessibilityEvent, eventInfo] {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            jsAccessibilityManager->SendAccessibilitySyncEvent(accessibilityEvent, eventInfo);
        },
        TaskExecutor::TaskType::BACKGROUND, "ArkUIAccessibilitySendSyncEvent");
}
#endif

void JsAccessibilityManager::UpdateNodeChildIds(const RefPtr<AccessibilityNode>& node)
{
    CHECK_NULL_VOID(node);
    node->ActionUpdateIds();
    const auto& children = node->GetChildList();
    std::vector<int32_t> childrenVec;
    auto cardId = GetCardId();
    auto rootNodeId = GetRootNodeId();

    // get last stack children to barrier free service.
    if ((node->GetNodeId() == GetRootNodeId() + ROOT_STACK_BASE) && !children.empty() && !IsDeclarative()) {
        auto lastChildNodeId = children.back()->GetNodeId();
        if (isOhosHostCard()) {
            childrenVec.emplace_back(ConvertToCardAccessibilityId(lastChildNodeId, cardId, rootNodeId));
        } else {
            childrenVec.emplace_back(lastChildNodeId);
            for (const auto& child : children) {
                if (child->GetNodeId() == ROOT_DECOR_BASE - 1) {
                    childrenVec.emplace_back(child->GetNodeId());
                    break;
                }
            }
        }
    } else {
        childrenVec.resize(children.size());
        if (isOhosHostCard()) {
            std::transform(children.begin(), children.end(), childrenVec.begin(),
                [cardId, rootNodeId](const RefPtr<AccessibilityNode>& child) {
                    return ConvertToCardAccessibilityId(child->GetNodeId(), cardId, rootNodeId);
                });
        } else {
            std::transform(children.begin(), children.end(), childrenVec.begin(),
                [](const RefPtr<AccessibilityNode>& child) { return child->GetNodeId(); });
        }
    }
    node->SetChildIds(childrenVec);
}

void JsAccessibilityManager::ProcessParameters(
    ActionType op, const std::vector<std::string>& params, std::map<std::string, std::string>& paramsMap)
{
    if (op == ActionType::ACCESSIBILITY_ACTION_SET_TEXT) {
        if (params.size() == (EVENT_DUMP_ACTION_PARAM_INDEX + 1)) {
            paramsMap = { { ACTION_ARGU_SET_TEXT, params[EVENT_DUMP_ACTION_PARAM_INDEX] } };
        }
    }

    if (op == ActionType::ACCESSIBILITY_ACTION_SET_SELECTION) {
        paramsMap[ACTION_ARGU_SELECT_TEXT_START] = "-1";
        paramsMap[ACTION_ARGU_SELECT_TEXT_END] = "-1";
        paramsMap[ACTION_ARGU_SELECT_TEXT_INFORWARD] = STRING_DIR_BACKWARD;
        if (params.size() > EVENT_DUMP_PARAM_LENGTH_LOWER) {
            paramsMap[ACTION_ARGU_SELECT_TEXT_START] = params[EVENT_DUMP_ACTION_PARAM_INDEX];
        }
        if (params.size() > EVENT_DUMP_PARAM_LENGTH_LOWER + 1) {
            paramsMap[ACTION_ARGU_SELECT_TEXT_END] = params[EVENT_DUMP_ACTION_PARAM_INDEX + 1];
        }
        // 2 means params number Offset
        if (params.size() > EVENT_DUMP_PARAM_LENGTH_LOWER + 2) {
            // 2 means params number Offset
            paramsMap[ACTION_ARGU_SELECT_TEXT_INFORWARD] = params[EVENT_DUMP_ACTION_PARAM_INDEX + 2];
        }
    }

    if (op == ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT || op == ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT) {
        if (params.size() == EVENT_DUMP_PARAM_LENGTH_UPPER) {
            paramsMap[ACTION_ARGU_MOVE_UNIT] = std::to_string(TextMoveUnit::STEP_CHARACTER);
        }
        paramsMap[ACTION_ARGU_MOVE_UNIT] = std::to_string(TextMoveUnit::STEP_CHARACTER);
    }

    if (op == ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION) {
        paramsMap[ACTION_ARGU_SET_OFFSET] = params[EVENT_DUMP_ACTION_PARAM_INDEX];
    }

    if ((op == ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD) ||
        (op == ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD)) {
        if (params.size() > EVENT_DUMP_PARAM_LENGTH_LOWER) {
            paramsMap = { { ACTION_ARGU_SCROLL_STUB, params[EVENT_DUMP_ACTION_PARAM_INDEX] } };
        }
    }

    if (op == ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK) {
        if (params.size() > EVENT_DUMP_PARAM_LENGTH_LOWER) {
            paramsMap = { { ACTION_ARGU_SPAN_ID, params[EVENT_DUMP_ACTION_PARAM_INDEX] } };
        }
    }
}

bool TransferExecuteAction(int64_t elementId, const RefPtr<NG::FrameNode>& node,
    const std::map<std::string, std::string>& actionArguments,
    ActionType action, int64_t uiExtensionOffset)
{
    bool isExecuted = false;
    if ((uiExtensionOffset + 1) > NG::UI_EXTENSION_OFFSET_MIN) {
        isExecuted = node->TransferExecuteAction(
            elementId, actionArguments, static_cast<int>(action),
            uiExtensionOffset / NG::UI_EXTENSION_ID_FACTOR);
    }
    return isExecuted;
}

RefPtr<NG::PipelineContext> JsAccessibilityManager::GetPipelineByWindowId(uint32_t windowId)
{
    auto mainPipeline = AceType::DynamicCast<NG::PipelineContext>(context_.Upgrade());
    if (mainPipeline!= nullptr && mainPipeline->GetWindowId() == windowId) {
        return mainPipeline;
    }
    for (auto subPipelineWeak : GetSubPipelineContexts()) {
        auto subContextNG = AceType::DynamicCast<NG::PipelineContext>(subPipelineWeak.Upgrade());
        if (subContextNG!= nullptr && subContextNG->GetWindowId() == windowId) {
            return subContextNG;
        }
    }
    if (GetWindowId() == windowId) {
        return mainPipeline;
    }
    return nullptr;
}

// DFX related
namespace {
enum class InjectActionType : uint32_t {
    UNDEFINED_ACTION = 0,
    NOTIFY_CHILD_ACTION = 1,
    SECURITY_CLICK_ACTION = 2,
};

bool CheckAndGetEventTestArgument(std::vector<std::string>::const_iterator start,
    const std::vector<std::string>& params, DumpInfoArgument& argument)
{
    auto arg = start;
    argument.mode = DumpMode::EVENT_TEST;
    constexpr int32_t NUM_EVENT_DIMENSION = 2;
    if (std::distance(arg, params.end()) <= NUM_EVENT_DIMENSION) {
        DumpLog::GetInstance().Print(std::string("Error: --event-test is used to send event with node ") +
            "need elementId and eventId, e.g. '--event-test ${elementId} ${eventId}'!");
        return false;
    }
    ++arg;
    argument.nodeId = StringUtils::StringToLongInt(*arg);
    ++arg;
    argument.eventId = StringUtils::StringToInt(*arg);
    return true;
}

bool CheckAndGetHoverTestArgument(std::vector<std::string>::const_iterator start,
    const std::vector<std::string>& params, DumpInfoArgument& argument)
{
    auto arg = start;
    argument.mode = DumpMode::HOVER_TEST;
    static constexpr int32_t NUM_POINT_DIMENSION = 2;
    if (std::distance(arg, params.end()) <= NUM_POINT_DIMENSION) {
        DumpLog::GetInstance().Print(std::string("Error: --hover-test is used to get nodes at a point ") +
            "relative to the root node, e.g. '--hover-test ${x} ${y}'!");
        return false;
    }
    ++arg;
    argument.pointX = StringUtils::StringToInt(*arg);
    ++arg;
    argument.pointY = StringUtils::StringToInt(*arg);
    return true;
}

bool DumpProcessInjectActionParameters(
    const std::vector<std::string>& params,
    int64_t& nodeId,
    int32_t& result,
    InjectActionType& actionType)
{
    constexpr int32_t NUM_PARAMETERS_DIMENSION = 1;
    if (params.size() < 1) {
        return false;
    }

    for (auto arg = params.begin() + 1; arg != params.end(); ++arg) {
        if (*arg == "--inject-action") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            nodeId = StringUtils::StringToLongInt(*arg);
        } else if (*arg == "--NotifyChildAction") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            result = StringUtils::StringToInt(*arg);
            actionType = InjectActionType::NOTIFY_CHILD_ACTION;
            return true;
        }  else if (*arg == "--SecurityClickAction") {
            actionType = InjectActionType::SECURITY_CLICK_ACTION;
            return true;
        }
    }
    return false;
}

bool DumpProcessEmbedSearchParameters(
    const std::vector<std::string>& params,
    int64_t& nodeId,
    SearchSurfaceIdType& searchType)
{
    constexpr int32_t NUM_PARAMETERS_DIMENSION = 2;
    if (params.size() < 1) {
        return false;
    }

    for (auto arg = params.begin() + 1; arg != params.end(); ++arg) {
        // Process the embed node parameters and dump its subtree info.
        if (*arg == "--embed-search") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            nodeId = StringUtils::StringToLongInt(*arg);
            ++arg;
            searchType = static_cast<SearchSurfaceIdType>(StringUtils::StringToInt(*arg));
            return true;
        }
    }
    return false;
}

bool DumpProcessEmbedHoverParameters(
    const std::vector<std::string>& params,
    int64_t& nodeId,
    int32_t& x,
    int32_t& y)
{
    constexpr int32_t NUM_PARAMETERS_DIMENSION = 3;
    if (params.size() < 1) {
        return false;
    }

    for (auto arg = params.begin() + 1; arg != params.end(); ++arg) {
        // Process the embed node parameters and hover test.
        if (*arg == "--embed-hover") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            nodeId = StringUtils::StringToLongInt(*arg);
            ++arg;
            x = StringUtils::StringToInt(*arg);
            ++arg;
            y = StringUtils::StringToInt(*arg);
            return true;
        }
    }
    return false;
}

bool DumpProcessSetCheckListParameters(
    const std::vector<std::string>& params,
    std::vector<uint32_t>& vec)
{
    if (params.size() < MIN_NUM) {
        return false;
    }
    auto arg = params.begin() + 1;
    if (*arg != "--set-whitelist") {
        return false;
    }
    for (++arg; arg != params.end();) {
        vec.emplace_back(StringUtils::StringToUint(*arg++));
    }
    return true;
}

bool DumpProcessGetCheckListParameters(
    const std::vector<std::string>& params)
{
    if (params.size() < MIN_NUM) {
        return false;
    }
    auto arg = params.begin() + 1;
    if (*arg != "--get-whitelist") {
        return false;
    }
    return true;
}

bool DumpProcessSpecificSearchParameters(
    const std::vector<std::string>& params,
    int64_t& nodeId,
    std::string& propertyTarget,
    int32_t& propertyType)
{
    constexpr int32_t NUM_PARAMETERS_DIMENSION = 3;
    if (params.size() < MIN_PARAMS_SIZE) {
        return false;
    }

    for (auto arg = params.begin() + 1; arg != params.end(); ++arg) {
        if (*arg == "--specific-search") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            nodeId = StringUtils::StringToLongInt(*arg);
            ++arg;
            propertyTarget = *arg;
            ++arg;
            propertyType = StringUtils::StringToInt(*arg);
            return true;
        }
    }
    return false;
}
} // DFX related

void JsAccessibilityManager::DumpInjectActionTest(const std::vector<std::string>& params)
{
    int64_t nodeId = 0;
    int32_t result = 0;
    InjectActionType actionType = InjectActionType::UNDEFINED_ACTION;
 
    if (!DumpProcessInjectActionParameters(params, nodeId, result, actionType)) {
        return;
    }

    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    RefPtr<NG::PipelineContext> ngPipeline;

    RefPtr<NG::FrameNode> frameNode;
    ngPipeline = FindPipelineByElementId(nodeId, frameNode);
    CHECK_NULL_VOID(ngPipeline);
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (actionType == InjectActionType::NOTIFY_CHILD_ACTION) {
        accessibilityProperty->SetNotifyChildAction([nodeId, result] (
            const RefPtr<NG::FrameNode>& node,
            NotifyChildActionType childActionType) {
                TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY, "onNotifyChildAction callback: nodeid %{public}" \
                    PRId64 " result %{public}d", nodeId, result);
                return static_cast<AccessibilityActionResult>(result);
        });
    }
    if (actionType == InjectActionType::SECURITY_CLICK_ACTION) {
        accessibilityProperty->SetSecurityClickAction (
            [nodeId] (const NG::SecCompEnhanceEvent& event) {
                std::string hmac(event.dataBuffer.begin(), event.dataBuffer.end());
                uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    event.time.time_since_epoch()).count();
                TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "onSecurityClickAction callback: nodeid %{public}" \
                    PRId64 " hmac %{public}s timesttamp %{public}" PRId64, nodeId, hmac.c_str(), time);
            }
        );
    }
    DumpLog::GetInstance().Print(std::string("Result: inject action done"));
}

void JsAccessibilityManager::DumpEmbedSearchTest(const std::vector<std::string>& params)
{
    int64_t nodeId = 0;
    SearchSurfaceIdType searchType = SearchSurfaceIdType::SEARCH_ALL;

    if (!DumpProcessEmbedSearchParameters(params, nodeId, searchType)) {
        return;
    }

    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    RefPtr<NG::PipelineContext> ngPipeline;

    RefPtr<NG::FrameNode> frameNode;
    ngPipeline = FindPipelineByElementId(nodeId, frameNode);
    CHECK_NULL_VOID(ngPipeline);
    CHECK_NULL_VOID(frameNode);
    auto surfaceId = GetSurfaceIdByEmbedNode(frameNode);
    if (surfaceId.empty()) {
        DumpLog::GetInstance().Print(std::string("Result: embed search not have surfaceId"));
        return;
    }
    std::list<AccessibilityElementInfo> infos;

    SearchElementInfoBySurfaceId(surfaceId, ngPipeline->GetWindowId(), searchType, infos);
    if (infos.empty()) {
        DumpLog::GetInstance().Print(std::string("Result: embed search empty"));
        return;
    }
    auto accessibilityId = infos.front().GetAccessibilityId();
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(
        accessibilityId, splitElementId, splitTreeId);

    for (auto& info : infos) {
        DumpCommonPropertyNG(info, splitTreeId);
    }
    DumpLog::GetInstance().Print(0, surfaceId, infos.size());
    DumpLog::GetInstance().Print(std::string("Result: embed search done"));
}

void JsAccessibilityManager::DumpEmbedHoverTestNG(const std::vector<std::string>& params, uint32_t windowId)
{
    int64_t rootId = INVALID_NODE_ID;
    int32_t x = 0;
    int32_t y = 0;
    if (!DumpProcessEmbedHoverParameters(params, rootId, x, y)) {
        return;
    }

    auto pipeline = GetPipelineByWindowId(windowId);
    CHECK_NULL_VOID(pipeline);
    auto accessibilityManagerNG = pipeline->GetAccessibilityManagerNG();
    CHECK_NULL_VOID(accessibilityManagerNG);
    auto pipelineRoot = pipeline->GetRootElement();
    RefPtr<NG::FrameNode> root = nullptr;
    if (rootId == -1) {
        root = pipelineRoot;
    } else {
        root = GetFramenodeByAccessibilityId(pipelineRoot, rootId);
    }
    CHECK_NULL_VOID(root);

    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    CommonProperty commonProperty;
    GenerateCommonProperty(pipeline, commonProperty, mainContext, root);
    AccessibilityElementInfo elementInfo;
    UpdateAccessibilityElementInfo(root, commonProperty, elementInfo, pipeline);
    x -= elementInfo.GetRectInScreen().GetLeftTopXScreenPostion();
    y -= elementInfo.GetRectInScreen().GetLeftTopYScreenPostion();

    DumpLog::GetInstance().Print("Window ID: " + std::to_string(windowId));
    DumpLog::GetInstance().Print("Root ID: " + std::to_string(root->GetAccessibilityId()));
    NG::PointF hoverPoint(x, y);
    DumpLog::GetInstance().Print("Hover Point: " + hoverPoint.ToString());
    auto surfaceId = GetSurfaceIdByEmbedNode(root);
    if (surfaceId.empty()) {
        DumpLog::GetInstance().Print(std::string("Result: embed hover test not have surfaceId"));
        return;
    }
    DumpLog::GetInstance().Print("surfaceId: " + surfaceId);

    TimeStamp time;
    NG::HandleHoverEventParam param = {
        hoverPoint, SourceType::TOUCH, NG::AccessibilityHoverEventType::ENTER, time, true };
    accessibilityManagerNG->HandleAccessibilityHoverEventBySurfaceId(surfaceId, param);
}

void JsAccessibilityManager::DumpSetCheckListTest(const std::vector<std::string>& params)
{
    std::vector<uint32_t> vec;
    if (!DumpProcessSetCheckListParameters(params, vec)) {
        return;
    }
    UpdateEventWhiteList(vec);
    DumpLog::GetInstance().Print("white list size: " + std::to_string(eventWhiteList_.size()));
    DumpLog::GetInstance().Print(std::string("Result: Set White List Done"));
}

void JsAccessibilityManager::DumpGetCheckListTest(const std::vector<std::string>& params)
{
    if (!DumpProcessGetCheckListParameters(params)) {
        return;
    }
    DumpLog::GetInstance().Print("white list size: " + std::to_string(eventWhiteList_.size()));
    for (const auto& event : eventWhiteList_) {
        DumpLog::GetInstance().Print("white list No: "+ std::to_string(event));
    }
    DumpLog::GetInstance().Print(std::string("Result: Get White List Done"));
}

void JsAccessibilityManager::DumpSpecificPropertySearchTest(const std::vector<std::string>& params, uint32_t windowId)
{
    int64_t nodeId = INVALID_NODE_ID;
    std::string propertyTarget;
    int32_t propertyType = 0;
    if (!DumpProcessSpecificSearchParameters(params, nodeId, propertyTarget, propertyType)) {
        return;
    }

    if (propertyType != SEARCH_TYPE::CUSTOMID) {
        return;
    }
    auto pipeline = GetPipelineByWindowId(windowId);
    CHECK_NULL_VOID(pipeline);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    CHECK_NULL_VOID(ngPipeline);

    std::list<AccessibilityElementInfo> infos;
    std::list<AccessibilityElementInfo> treeInfos;
    SpecificPropertyParam propertyParam {
        .propertyTarget = propertyTarget, .propertyType = static_cast<SEARCH_TYPE>(propertyType)};
    SearchElementInfoByCustomIdNG(nodeId, propertyParam.propertyTarget, infos, treeInfos, ngPipeline);
    DumpLog::GetInstance().Print(std::string("customId search start"));
    DumpLog::GetInstance().Print("info size: " + std::to_string(infos.size()));
    for (const auto& info : infos) {
        DumpLog::GetInstance().Print("info Element ID: " + std::to_string(info.GetAccessibilityId()));
    }
    DumpLog::GetInstance().Print("treeInfos size: " + std::to_string(treeInfos.size()));
    for (const auto& treeInfo : treeInfos) {
        DumpLog::GetInstance().Print("treeInfo Element ID " + std::to_string(treeInfo.GetAccessibilityId()));
    }
    DumpLog::GetInstance().Print(std::string("Result: customId search done"));
}

void JsAccessibilityManager::DumpTreeNG(bool useWindowId, uint32_t windowId, int64_t rootId, bool isDumpSimplify)
{
    if (!useWindowId && rootId == -1) {
        // used to adapt old function
        DumpTree(0, 0);
        return;
    }

    auto pipeline = GetPipelineByWindowId(windowId);
    if (pipeline == nullptr) {
        DumpLog::GetInstance().Print("Error: pipeline is not found!");
        return;
    }
    auto rootNode = pipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    CommonProperty commonProperty;
    auto mainPipeline = context_.Upgrade();
    CHECK_NULL_VOID(mainPipeline);
    GenerateCommonProperty(pipeline, commonProperty, mainPipeline, rootNode);
    auto nodeId = rootId == -1 ? rootNode->GetAccessibilityId() : rootId;
    DumpTreeNG(rootNode, 0, nodeId, commonProperty, isDumpSimplify);
}

void JsAccessibilityManager::DumpHoverTestNG(uint32_t windowId, int64_t rootId, int32_t x, int32_t y, bool verbose)
{
    auto pipeline = GetPipelineByWindowId(windowId);
    CHECK_NULL_VOID(pipeline);
    auto accessibilityManagerNG = pipeline->GetAccessibilityManagerNG();
    CHECK_NULL_VOID(accessibilityManagerNG);
    auto pipelineRoot = pipeline->GetRootElement();
    RefPtr<NG::FrameNode> root = nullptr;
    if (rootId == -1) {
        root = pipelineRoot;
    } else {
        root = GetFramenodeByAccessibilityId(pipelineRoot, rootId);
    }
    CHECK_NULL_VOID(root);

    DumpLog::GetInstance().Print("Window ID: " + std::to_string(windowId));
    DumpLog::GetInstance().Print("Root ID: " + std::to_string(root->GetAccessibilityId()));
    NG::PointF hoverPoint(x, y);
    DumpLog::GetInstance().Print("Hover Point: " + hoverPoint.ToString());

    std::string summary;
    std::string detail;
    accessibilityManagerNG->HoverTestDebug(root, hoverPoint, summary, detail);
    DumpLog::GetInstance().Print(summary);
    if (verbose) {
        DumpLog::GetInstance().Print(detail);
    }
}

bool JsAccessibilityManager::DumpProcessEventParameters(
    AccessibilityEvent& event, const std::vector<std::string>& params)
{
    constexpr int32_t NUM_PARAMETERS_DIMENSION = 1;
    if (params.size() < 1) {
        return false;
    }
    for (auto arg = params.begin() + 1; arg != params.end(); ++arg) {
        if (*arg == "--stackNodeId") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            event.stackNodeId = StringUtils::StringToLongInt(*arg);
        } else if (*arg == "--beforeText") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            event.beforeText = *arg;
        } else if (*arg == "--latestContent") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            event.latestContent = *arg;
        } else if (*arg == "--textAnnounced") {
            if (std::distance(arg, params.end()) <= NUM_PARAMETERS_DIMENSION) {
                DumpLog::GetInstance().Print(std::string("Error: parameters need with data"));
                return false;
            }
            ++arg;
            event.textAnnouncedForAccessibility = *arg;
        }
    }
    return true;
}

void JsAccessibilityManager::DumpSendEventTest(int64_t nodeId, int32_t eventId, const std::vector<std::string>& params)
{
    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    RefPtr<NG::PipelineContext> ngPipeline;

    RefPtr<NG::FrameNode> frameNode;
    ngPipeline = FindPipelineByElementId(nodeId, frameNode);
    CHECK_NULL_VOID(ngPipeline);
    CHECK_NULL_VOID(frameNode);

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = nodeId;
    accessibilityEvent.windowChangeTypes = WindowUpdateType::WINDOW_UPDATE_ACTIVE;
    accessibilityEvent.type = static_cast<AccessibilityEventType>(eventId);
    if (DumpProcessEventParameters(accessibilityEvent, params)) {
        SendEventToAccessibilityWithNode(accessibilityEvent, frameNode, ngPipeline);
    }
}

bool JsAccessibilityManager::CheckDumpInfoParams(const std::vector<std::string> &params)
{
    if (params.size() < 1) {
        return false;
    }
    const std::string firstParam = params[0];
    if (firstParam.compare("-inspector") != 0 && firstParam.compare("-accessibility") != 0 &&
        firstParam.compare("-simplify") != 0) {
        return false;
    }
    return true;
}

bool JsAccessibilityManager::GetDumpInfoArgument(const std::vector<std::string>& params, DumpInfoArgument& argument)
{
    if (params.size() < 1) {
        return false;
    }
    argument.isDumpSimplify = params[0].compare("-simplify") == 0;
    bool isArgument = DumpInfoParams(params, argument);
    return isArgument;
}

bool JsAccessibilityManager::DumpInfoParams(const std::vector<std::string>& params, DumpInfoArgument& argument)
{
    for (auto arg = params.begin() + 1; arg != params.end(); ++arg) {
        if (*arg == "-w") {
            argument.useWindowId = true;
        } else if (*arg == "--root") {
            ++arg;
            if (arg == params.end()) {
                DumpLog::GetInstance().Print(std::string("Error: --root is used to set the root node, ") +
                    "e.g. '--root ${AccessibilityId}'!");
                return false;
            }
            argument.rootId = StringUtils::StringToLongInt(*arg);
        } else if (*arg == "--hover-test") {
            if (!CheckAndGetHoverTestArgument(arg, params, argument)) {
                return false;
            }
        } else if (*arg == "--event-test") {
            return CheckAndGetEventTestArgument(arg, params, argument);
        } else if (*arg == "--inject-action") {
            argument.mode = DumpMode::INJECT_ACTION_TEST;
            break;
        } else if (*arg == "--embed-search") {
            argument.mode = DumpMode::EMBED_SEARCH_TEST;
            break;
        } else if (*arg == "--embed-hover") {
            argument.mode = DumpMode::EMBED_HOVER_TEST;
            break;
        } else if (*arg == "--set-whitelist") {
            argument.mode = DumpMode::SET_CHECKLIST_TEST;
            break;
        } else if (*arg == "--get-whitelist") {
            argument.mode = DumpMode::GET_CHECKLIST_TEST;
            break;
        } else if (*arg == "--specific-search") {
            argument.mode = DumpMode::SPECIFIC_SEARCH_TEST;
            break;
        } else if (*arg == "-v") {
            argument.verbose = true;
        } else if (*arg == "-json") {
            argument.mode = DumpMode::TREE;
        } else {
            if (HandleNodeModeParam(*arg, argument)) {
                break;
            }
        }
    }
    return true;
}

bool JsAccessibilityManager::HandleNodeModeParam(const std::string& param, DumpInfoArgument& argument)
{
    if (argument.mode == DumpMode::NODE) {
        argument.mode = DumpMode::HANDLE_EVENT;
        return true;
    } else {
        argument.mode = DumpMode::NODE;
        argument.nodeId = StringUtils::StringToLongInt(param);
        return false;
    }
}

void JsAccessibilityManager::OnDumpInfoNG(const std::vector<std::string>& params, uint32_t windowId, bool hasJson)
{
    if (!CheckDumpInfoParams(params)) {
        DumpLog::GetInstance().Print("Error: invalid arguments!");
        return;
    }
    DumpInfoArgument argument;
    if (!GetDumpInfoArgument(params, argument)) {
        return;
    }
    std::vector<std::string> info;
    bool isChildElement = CheckIsChildElement(argument.nodeId, params, info, argument.mode, argument.rootId);
    if (isChildElement) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "dump child element: %{public}" PRId64, argument.nodeId);
        return;
    }
    ChooseDumpEvent(params, argument, windowId, hasJson);
}

void JsAccessibilityManager::ChooseDumpEvent(const std::vector<std::string>& params,
    DumpInfoArgument& argument, uint32_t windowId, bool hasJson)
{
    switch (argument.mode) {
        case DumpMode::TREE:
            isUseJson_ = hasJson;
            DumpTreeNG(argument.useWindowId, windowId, argument.rootId, argument.isDumpSimplify);
            break;
        case DumpMode::NODE:
            DumpPropertyNG(argument.nodeId);
            break;
        case DumpMode::HANDLE_EVENT:
            DumpHandleEvent(params);
            break;
        case DumpMode::HOVER_TEST:
            DumpHoverTestNG(windowId, argument.rootId, argument.pointX, argument.pointY, argument.verbose);
            break;
        case DumpMode::EVENT_TEST:
            DumpSendEventTest(argument.nodeId, argument.eventId, params);
            break;
        case DumpMode::INJECT_ACTION_TEST:
            DumpInjectActionTest(params);
            break;
        case DumpMode::EMBED_SEARCH_TEST:
            DumpEmbedSearchTest(params);
            break;
        case DumpMode::EMBED_HOVER_TEST:
            DumpEmbedHoverTestNG(params, windowId);
            break;
        case DumpMode::SET_CHECKLIST_TEST:
            DumpSetCheckListTest(params);
            break;
        case DumpMode::GET_CHECKLIST_TEST:
            DumpGetCheckListTest(params);
            break;
        case DumpMode::SPECIFIC_SEARCH_TEST:
            DumpSpecificPropertySearchTest(params, windowId);
            break;
        default:
            DumpLog::GetInstance().Print("Error: invalid arguments!");
            break;
    }
}

bool JsAccessibilityManager::CheckDumpHandleEventParams(const std::vector<std::string>& params)
{
    if (params.size() > EVENT_DUMP_PARAM_LENGTH_UPPER + 1) {
        DumpLog::GetInstance().Print("Error: params length is illegal!");
        return false;
    }
    if (params[EVENT_DUMP_ORDER_INDEX] != DUMP_ORDER && params[EVENT_DUMP_ORDER_INDEX] != DUMP_INSPECTOR) {
        DumpLog::GetInstance().Print("Error: Unrecognized dump command for accessibility!");
        return false;
    }
    return true;
}

bool JsAccessibilityManager::CheckGetActionIdAndOp(
    const std::vector<std::string>& params,
    int64_t& actionAccessibilityId,
    ActionType& actionOp)
{
    if (EVENT_DUMP_ID_INDEX > params.size()) {
        return false;
    }
    if (EVENT_DUMP_ACTION_INDEX != std::clamp(EVENT_DUMP_ACTION_INDEX, EVENT_DUMP_ID_INDEX, params.size())) {
        return false;
    }
    actionAccessibilityId = StringUtils::StringToLongInt(params[EVENT_DUMP_ID_INDEX]);
    auto action = static_cast<AceAction>(StringUtils::StringToInt(params[EVENT_DUMP_ACTION_INDEX]));
    actionOp = ConvertAceAction(action);
    if ((actionOp != ActionType::ACCESSIBILITY_ACTION_SET_SELECTION) &&
        (params.size() > EVENT_DUMP_PARAM_LENGTH_UPPER + 1)) {
        return false;
    }
    if (actionOp == ActionType::ACCESSIBILITY_ACTION_INVALID) {
        return false;
    }
    return true;
}

void JsAccessibilityManager::DumpHandleEvent(const std::vector<std::string>& params)
{
    if (!CheckDumpHandleEventParams(params)) {
        return;
    }
    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    int64_t nodeId = StringUtils::StringToLongInt(params[EVENT_DUMP_ID_INDEX]);

    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(nodeId, splitElementId, splitTreeId);
    nodeId = splitElementId;

    auto action = static_cast<AceAction>(StringUtils::StringToInt(params[EVENT_DUMP_ACTION_INDEX]));
    auto op = ConvertAceAction(action);
    if ((op != ActionType::ACCESSIBILITY_ACTION_SET_SELECTION) && (params.size() > EVENT_DUMP_PARAM_LENGTH_UPPER + 1)) {
        return DumpLog::GetInstance().Print("Error: params is illegal!");
    }
    std::map<std::string, std::string> paramsMap;
    ProcessParameters(op, params, paramsMap);
    if (AceType::InstanceOf<NG::PipelineContext>(pipeline)) {
        RefPtr<NG::FrameNode> node;
#ifdef WINDOW_SCENE_SUPPORTED
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
        auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
        CHECK_NULL_VOID(uiExtensionManager);
        if (uiExtensionManager->IsWrapExtensionAbilityId(nodeId)) {
            ExecuteActionNG(nodeId, paramsMap, op, ngPipeline, NG::UI_EXTENSION_OFFSET_MAX);
            return;
        }
#endif
        pipeline = FindPipelineByElementId(nodeId, node);
        CHECK_NULL_VOID(pipeline);
        pipeline->GetTaskExecutor()->PostTask(
            [weak = WeakClaim(this), op, nodeId, paramsMap]() {
                auto jsAccessibilityManager = weak.Upgrade();
                CHECK_NULL_VOID(jsAccessibilityManager);
                RefPtr<NG::FrameNode> node;
                auto pipeline = jsAccessibilityManager->FindPipelineByElementId(nodeId, node);
                CHECK_NULL_VOID(pipeline);
                jsAccessibilityManager->ExecuteActionNG(nodeId, paramsMap, op, pipeline, NG::UI_EXTENSION_OFFSET_MAX);
            },
            TaskExecutor::TaskType::UI, "ArkUIAccessibilityExecuteAction");
        return;
    }
    auto node = GetAccessibilityNodeFromPage(nodeId);
    CHECK_NULL_VOID(node);
    pipeline->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), op, node, paramsMap]() {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            auto pipeline = jsAccessibilityManager->context_.Upgrade();
            CHECK_NULL_VOID(pipeline);
            jsAccessibilityManager->AccessibilityActionEvent(
                op, paramsMap, node, AceType::DynamicCast<PipelineContext>(pipeline));
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityActionEvent");
}

void JsAccessibilityManager::DumpProperty(const RefPtr<AccessibilityNode>& node)
{
    const auto& supportAceActions = node->GetSupportAction();
    const auto& charValue = node->GetChartValue();

    DumpLog::GetInstance().AddDesc("ID: ", node->GetNodeId());
    DumpLog::GetInstance().AddDesc("parent ID: ", node->GetParentId());
    DumpLog::GetInstance().AddDesc("child IDs: ", GetNodeChildIds(node));
    DumpLog::GetInstance().AddDesc("component type: ", node->GetTag());
    DumpLog::GetInstance().AddDesc("input type: ", node->GetInputType());
    DumpLog::GetInstance().AddDesc("text: ", node->GetText());
    DumpLog::GetInstance().AddDesc("width: ", node->GetWidth());
    DumpLog::GetInstance().AddDesc("height: ", node->GetHeight());
    DumpLog::GetInstance().AddDesc("left: ", node->GetLeft() + GetCardOffset().GetX());
    DumpLog::GetInstance().AddDesc("top: ", node->GetTop() + GetCardOffset().GetY());
    DumpLog::GetInstance().AddDesc("enabled: ", BoolToString(node->GetEnabledState()));
    DumpLog::GetInstance().AddDesc("checked: ", BoolToString(node->GetCheckedState()));
    DumpLog::GetInstance().AddDesc("selected: ", BoolToString(node->GetSelectedState()));
    DumpLog::GetInstance().AddDesc("focusable: ", BoolToString(node->GetFocusableState()));
    DumpLog::GetInstance().AddDesc("focused: ", BoolToString(node->GetFocusedState()));
    DumpLog::GetInstance().AddDesc("checkable: ", BoolToString(node->GetCheckableState()));
    DumpLog::GetInstance().AddDesc("clickable: ", BoolToString(node->GetClickableState()));
    DumpLog::GetInstance().AddDesc("long clickable: ", BoolToString(node->GetLongClickableState()));
    DumpLog::GetInstance().AddDesc("scrollable: ", BoolToString(node->GetScrollableState()));
    DumpLog::GetInstance().AddDesc("editable: ", BoolToString(node->GetEditable()));
    DumpLog::GetInstance().AddDesc("hint text: ", node->GetHintText());
    DumpLog::GetInstance().AddDesc("error text: ", node->GetErrorText());
    DumpLog::GetInstance().AddDesc("js component id: ", node->GetJsComponentId());
    DumpLog::GetInstance().AddDesc("accessibility label: ", node->GetAccessibilityLabel());
    DumpLog::GetInstance().AddDesc("accessibility hint: ", node->GetAccessibilityHint());
    DumpLog::GetInstance().AddDesc("max text length: ", node->GetMaxTextLength());
    DumpLog::GetInstance().AddDesc("text selection start: ", node->GetTextSelectionStart());
    DumpLog::GetInstance().AddDesc("text selection end: ", node->GetTextSelectionEnd());
    DumpLog::GetInstance().AddDesc("is multi line: ", BoolToString(node->GetIsMultiLine()));
    DumpLog::GetInstance().AddDesc("is password", BoolToString(node->GetIsPassword()));
    DumpLog::GetInstance().AddDesc("text input type: ", ConvertInputTypeToString(node->GetTextInputType()));
    DumpLog::GetInstance().AddDesc("min value: ", node->GetAccessibilityValue().min);
    DumpLog::GetInstance().AddDesc("max value: ", node->GetAccessibilityValue().max);
    DumpLog::GetInstance().AddDesc("current value: ", node->GetAccessibilityValue().current);
    DumpLog::GetInstance().AddDesc("collection info rows: ", node->GetCollectionInfo().rows);
    DumpLog::GetInstance().AddDesc("collection info columns: ", node->GetCollectionInfo().columns);
    DumpLog::GetInstance().AddDesc("collection item info, row: ", node->GetCollectionItemInfo().row);
    DumpLog::GetInstance().AddDesc("collection item info, column: ", node->GetCollectionItemInfo().column);
    DumpLog::GetInstance().AddDesc("chart has value: ", BoolToString(charValue && !charValue->empty()));
    DumpLog::GetInstance().AddDesc("accessibilityGroup: ", BoolToString(node->GetAccessible()));
    DumpLog::GetInstance().AddDesc("accessibilityImportance: ", node->GetImportantForAccessibility());
    DumpLog::GetInstance().AddDesc("support action: ", GetSupportAction(supportAceActions));
    DumpLog::GetInstance().Print(0, node->GetTag(), node->GetChildList().size());
}

void JsAccessibilityManager::DumpPropertyInfo(
    const RefPtr<NG::FrameNode>& frameNode, AccessibilityElementInfo& nodeInfo)
{
    DumpCommonPropertyNG(nodeInfo, treeId_);
    DumpAccessibilityPropertyNG(nodeInfo);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    if (accessibilityProperty) {
        DumpLog::GetInstance().AddDesc("offset: ", accessibilityProperty->GetScrollOffSet());
        DumpLog::GetInstance().AddDesc("childTreeId: ", accessibilityProperty->GetChildTreeId());
        DumpLog::GetInstance().AddDesc("childWindowId: ", accessibilityProperty->GetChildWindowId());
    }

    DumpLog::GetInstance().AddDesc("surfaceId: ", GetSurfaceIdByEmbedNode(frameNode));
    DumpLog::GetInstance().Print(0, nodeInfo.GetComponentType(), nodeInfo.GetChildCount());
}

void JsAccessibilityManager::DumpPropertyNG(int64_t nodeID)
{
    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    RefPtr<NG::PipelineContext> ngPipeline;

#ifdef WINDOW_SCENE_SUPPORTED
    ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    std::list<AccessibilityElementInfo> extensionElementInfos;
    if (uiExtensionManager->IsWrapExtensionAbilityId(nodeID)) {
        SearchElementInfoByAccessibilityIdNG(
            nodeID, PREFETCH_RECURSIVE_CHILDREN, extensionElementInfos, ngPipeline, NG::UI_EXTENSION_OFFSET_MAX);
        for (auto& extensionElementInfo : extensionElementInfos) {
            if (nodeID == extensionElementInfo.GetAccessibilityId()) {
                DumpCommonPropertyNG(extensionElementInfo, treeId_);
                DumpAccessibilityPropertyNG(extensionElementInfo);
                DumpLog::GetInstance().Print(
                    0, extensionElementInfo.GetComponentType(), extensionElementInfo.GetChildCount());
                return;
            }
        }
    }
#endif

    RefPtr<NG::FrameNode> frameNode;
    ngPipeline = FindPipelineByElementId(nodeID, frameNode);
    CHECK_NULL_VOID(ngPipeline);
    CHECK_NULL_VOID(frameNode);

    CommonProperty commonProperty;
    GenerateCommonProperty(ngPipeline, commonProperty, pipeline, frameNode);
    commonProperty.checkEmbedNode = false;
    AccessibilityElementInfo nodeInfo;
    UpdateAccessibilityElementInfo(frameNode, commonProperty, nodeInfo, ngPipeline);
    SetRootAccessibilityVisible(frameNode, nodeInfo);
    SetRootAccessibilityNextFocusId(frameNode, ngPipeline->GetRootElement(), nodeInfo);
    SetRootAccessibilityPreFocusId(frameNode, ngPipeline->GetRootElement(), nodeInfo,
                                   nextFocusMapWithSubWindow_[pipeline->GetInstanceId()]);
    if (IsExtensionComponent(frameNode) && !IsUIExtensionShowPlaceholder(frameNode)) {
        SearchParameter param {-1, "", PREFETCH_RECURSIVE_CHILDREN, NG::UI_EXTENSION_OFFSET_MAX};
        std::list<AccessibilityElementInfo> extensionElementInfos;
        SearchExtensionElementInfoNG(param, frameNode, extensionElementInfos, nodeInfo);
    }
    DumpPropertyInfo(frameNode, nodeInfo);
}

void JsAccessibilityManager::DumpProperty(const std::vector<std::string>& params)
{
    CHECK_NULL_VOID(DumpLog::GetInstance().GetDumpFile());
    if (params.empty()) {
        DumpLog::GetInstance().Print("Error: params cannot be empty!");
        return;
    }
    if (params.size() != PROPERTY_DUMP_PARAM_LENGTH) {
        DumpLog::GetInstance().Print("Error: params length is illegal!");
        return;
    }
    if (params[0] != DUMP_ORDER && params[0] != DUMP_INSPECTOR) {
        DumpLog::GetInstance().Print("Error: not accessibility dump order!");
        return;
    }

    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);

    if (!AceType::InstanceOf<NG::PipelineContext>(pipeline)) {
        auto node = GetAccessibilityNodeFromPage(StringUtils::StringToLongInt(params[1]));
        if (!node) {
            DumpLog::GetInstance().Print("Error: can't find node with ID " + params[1]);
            return;
        }
        DumpProperty(node);
    } else {
        DumpPropertyNG(StringUtils::StringToLongInt(params[1]));
    }
}

static void DumpAccessibilityElementInfosTreeNG(
    std::list<AccessibilityElementInfo>& infos, int32_t depth, int64_t accessibilityId, bool isRoot)
{
    AccessibilityElementInfo accessibilityInfo;
    for (auto& info : infos) {
        if (accessibilityId == info.GetAccessibilityId()) {
            accessibilityInfo = info;
            break;
        }
    }
    if (!isRoot) {
        DumpLog::GetInstance().AddDesc("ID: " + std::to_string(accessibilityInfo.GetAccessibilityId()));
        DumpLog::GetInstance().AddDesc("compid: " + accessibilityInfo.GetInspectorKey());
        DumpLog::GetInstance().AddDesc("text: " + accessibilityInfo.GetContent());
        DumpLog::GetInstance().AddDesc("accessibilityText: " + accessibilityInfo.GetContent());
        DumpLog::GetInstance().AddDesc("accessibilityGroup: ");
        DumpLog::GetInstance().AddDesc("accessibilityLevel: ");
        DumpLog::GetInstance().AddDesc("accessibilityCustomRole: " + accessibilityInfo.GetCustomComponentType());
        DumpLog::GetInstance().AddDesc("top: " +
            std::to_string(accessibilityInfo.GetRectInScreen().GetLeftTopYScreenPostion()));
        DumpLog::GetInstance().AddDesc("left: " +
            std::to_string(accessibilityInfo.GetRectInScreen().GetLeftTopXScreenPostion()));
        DumpLog::GetInstance().AddDesc("width: " + std::to_string(
            accessibilityInfo.GetRectInScreen().GetRightBottomXScreenPostion() -
            accessibilityInfo.GetRectInScreen().GetLeftTopXScreenPostion()));
        DumpLog::GetInstance().AddDesc("height: " + std::to_string(
            accessibilityInfo.GetRectInScreen().GetRightBottomYScreenPostion() -
            accessibilityInfo.GetRectInScreen().GetLeftTopYScreenPostion()));
        DumpLog::GetInstance().AddDesc("visible: " + std::to_string(accessibilityInfo.IsVisible()));
        DumpLog::GetInstance().AddDesc(
            "clickable: " + std::to_string(accessibilityInfo.IsClickable()));
        DumpLog::GetInstance().AddDesc("longclickable: " +
            std::to_string(accessibilityInfo.IsLongClickable()));
        DumpLog::GetInstance().AddDesc("checkable: " + std::to_string(accessibilityInfo.IsCheckable()));
        DumpLog::GetInstance().AddDesc("scrollable: " + std::to_string(accessibilityInfo.IsScrollable()));
        DumpLog::GetInstance().AddDesc("checked: " + std::to_string(accessibilityInfo.IsChecked()));
        DumpLog::GetInstance().AddDesc("hint: " + accessibilityInfo.GetHint());
        DumpLog::GetInstance().Print(depth, accessibilityInfo.GetComponentType(), accessibilityInfo.GetChildCount());
        depth ++;
    }
    for (auto child : accessibilityInfo.GetChildIds()) {
        DumpAccessibilityElementInfosTreeNG(infos, depth, child, false);
    }
}

static void DumpTreeNodeInfoNG(
    const RefPtr<NG::FrameNode>& node, int32_t depth, const CommonProperty& commonProperty, int32_t childSize)
{
    NG::RectF rect = node->GetTransformRectRelativeToWindow(true);
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    DumpLog::GetInstance().AddDesc("ID: " + std::to_string(node->GetAccessibilityId()));
    DumpLog::GetInstance().AddDesc("compid: " + node->GetInspectorId().value_or(""));
    if (accessibilityProperty) {
        DumpLog::GetInstance().AddDesc("text: " + accessibilityProperty->GetGroupText());
        DumpLog::GetInstance().AddDesc("accessibilityText: " + accessibilityProperty->GetAccessibilityText());
        DumpLog::GetInstance().AddDesc(
            "accessibilityGroup: " + std::to_string(accessibilityProperty->IsAccessibilityGroup()));
        DumpLog::GetInstance().AddDesc("accessibilityLevel: " + accessibilityProperty->GetAccessibilityLevel());
        DumpLog::GetInstance().AddDesc(
            "accessibilityCustomRole: " + accessibilityProperty->GetAccessibilityCustomRole());
    }
    DumpLog::GetInstance().AddDesc("top: " + std::to_string(rect.Top() + commonProperty.windowTop));
    DumpLog::GetInstance().AddDesc("left: " + std::to_string(rect.Left() + commonProperty.windowLeft));
    DumpLog::GetInstance().AddDesc("width: " + std::to_string(rect.Width()));
    DumpLog::GetInstance().AddDesc("height: " + std::to_string(rect.Height()));
    DumpLog::GetInstance().AddDesc("visible: " + std::to_string(node->IsVisible()));
    auto eventHub = node->GetOrCreateEventHub<NG::EventHub>();
    if (eventHub) {
        auto gestureEventHub = eventHub->GetGestureEventHub();
        DumpLog::GetInstance().AddDesc(
            "clickable: " + std::to_string(gestureEventHub ? gestureEventHub->IsAccessibilityClickable() : false));
        DumpLog::GetInstance().AddDesc("longclickable: " +
            std::to_string(gestureEventHub ? gestureEventHub->IsAccessibilityLongClickable() : false));
    }
    if (accessibilityProperty) {
        DumpLog::GetInstance().AddDesc("checkable: " + std::to_string(accessibilityProperty->IsCheckable()));
        DumpLog::GetInstance().AddDesc("scrollable: " + std::to_string(accessibilityProperty->IsScrollable()));
        DumpLog::GetInstance().AddDesc("checked: " + std::to_string(accessibilityProperty->IsChecked()));
        DumpLog::GetInstance().AddDesc("hint: " + accessibilityProperty->GetHintText());
        DumpLog::GetInstance().AddDesc("childTree: " + std::to_string(accessibilityProperty->GetChildTreeId()));
    }
    DumpLog::GetInstance().Print(depth, node->GetTag(), childSize);
}

void JsAccessibilityManager::DumpTreeNodeSafeAreaInfoNg(const RefPtr<NG::FrameNode>& node)
{
    auto layoutProperty = node->GetLayoutProperty();
    if (layoutProperty) {
        auto&& opts = layoutProperty->GetSafeAreaExpandOpts();
        if (opts && opts->type != NG::SAFE_AREA_TYPE_NONE && opts->edges != NG::SAFE_AREA_EDGE_NONE) {
            DumpLog::GetInstance().AddDesc(opts->ToString());
        }
        if (layoutProperty->GetSafeAreaInsets()) {
            DumpLog::GetInstance().AddDesc(layoutProperty->GetSafeAreaInsets()->ToString());
        }
    }
    if (node->SelfOrParentExpansive()) {
        auto geometryNode = node->GetGeometryNode();
        if (geometryNode) {
            auto rect = geometryNode->GetSelfAdjust();
            auto parentRect = geometryNode->GetParentAdjust();
            bool isDefaultSize = NearZero(rect.GetX(), 0.0) && NearZero(rect.GetY(), 0.0) &&
                                 NearZero(rect.Width(), 0.0) && NearZero(rect.Height(), 0.0);
            bool isParentDefaultSize = NearZero(parentRect.GetX(), 0.0) && NearZero(parentRect.GetY(), 0.0) &&
                                       NearZero(parentRect.Width(), 0.0) && NearZero(parentRect.Height(), 0.0);
            if (!isDefaultSize && !isParentDefaultSize) {
                DumpLog::GetInstance().AddDesc(std::string("selfAdjust: ")
                                                   .append(rect.ToString().c_str())
                                                   .append(",parentAdjust")
                                                   .append(parentRect.ToString().c_str()));
            }
        }
    }
    CHECK_NULL_VOID(node->GetTag() == V2::PAGE_ETS_TAG);
    auto pipeline = node->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto manager = pipeline->GetSafeAreaManager();
    CHECK_NULL_VOID(manager);
    if (!manager->IsIgnoreSafeArea() && !manager->IsNeedAvoidWindow() && !manager->IsFullScreen() &&
        !manager->KeyboardSafeAreaEnabled() && !manager->GetUseCutout()) {
        DumpLog::GetInstance().AddDesc(
            std::string("ignoreSafeArea: ")
                .append(std::to_string(manager->IsIgnoreSafeArea()))
                .append(std::string(", isNeedAvoidWindow: ").c_str())
                .append(std::to_string(manager->IsNeedAvoidWindow()))
                .append(std::string(", IisFullScreen: ").c_str())
                .append(std::to_string(manager->IsFullScreen()))
                .append(std::string(", isKeyboardAvoidMode: ").c_str())
                .append(std::to_string(static_cast<int32_t>(manager->GetKeyBoardAvoidMode())))
                .append(std::string(", isUseCutout: ").c_str())
                .append(std::to_string(manager->GetUseCutout())));
    }
}

void JsAccessibilityManager::DumpPadding(const std::unique_ptr<NG::PaddingProperty>& padding, std::string label)
{
    NG::CalcLength defaultValue = NG::CalcLength(Dimension(0));
    auto left = padding->left.value_or(defaultValue).GetDimension().Value();
    auto right = padding->right.value_or(defaultValue).GetDimension().Value();
    auto top = padding->top.value_or(defaultValue).GetDimension().Value();
    auto bottom = padding->bottom.value_or(defaultValue).GetDimension().Value();
    if (!NearZero(left, 0.0) && !NearZero(right, 0.0) && !NearZero(top, 0.0) && !NearZero(bottom, 0.0)) {
        DumpLog::GetInstance().AddDesc(label.append(padding->ToString().c_str()));
    }
}

void JsAccessibilityManager::DumpBorder(const std::unique_ptr<NG::BorderWidthProperty>& border, std::string label)
{
    Dimension defaultValue(0);
    auto left = border->leftDimen.value_or(defaultValue).Value();
    auto right = border->rightDimen.value_or(defaultValue).Value();
    auto top = border->topDimen.value_or(defaultValue).Value();
    auto bottom = border->bottomDimen.value_or(defaultValue).Value();
    if (!NearZero(left, 0.0) && !NearZero(right, 0.0) && !NearZero(top, 0.0) && !NearZero(bottom, 0.0)) {
        DumpLog::GetInstance().AddDesc(label.append(border->ToString().c_str()));
    }
}

void JsAccessibilityManager::DumpTreeNodeCommonInfoNg(
    const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty)
{
    DumpLog::GetInstance().AddDesc("ID: " + std::to_string(node->GetAccessibilityId()));
    auto renderContext = node->GetRenderContext();
    if (renderContext) {
        auto backgroundColor = renderContext->GetBackgroundColor();
        if (backgroundColor && backgroundColor->ColorToString().compare("#00000000") != 0) {
            DumpLog::GetInstance().AddDesc("BackgroundColor: " + backgroundColor->ColorToString());
        }
        DumpLog::GetInstance().AddDesc(std::string("PaintRectWithoutTransform: ")
                                           .append(renderContext->GetPaintRectWithoutTransform().ToString()));
    }
    NG::RectF rect = node->GetTransformRectRelativeToWindow(true);
    auto top = rect.Top() + commonProperty.windowTop;
    auto left = rect.Left() + commonProperty.windowLeft;
    if (!NearZero(top, 0.0) && !NearZero(left, 0.0)) {
        DumpLog::GetInstance().AddDesc("top: " + std::to_string(top));
        DumpLog::GetInstance().AddDesc("left: " + std::to_string(left));
    }
    DumpLog::GetInstance().AddDesc("width: " + std::to_string(rect.Width()));
    DumpLog::GetInstance().AddDesc("height: " + std::to_string(rect.Height()));
    auto layoutProperty = node->GetLayoutProperty();
    if (layoutProperty) {
        if (!node->IsVisible() && layoutProperty->GetVisibility().has_value()) {
            DumpLog::GetInstance().AddDesc(
                "visible: " + std::to_string(static_cast<int32_t>(layoutProperty->GetVisibility().value())));
        }
        auto& padding = layoutProperty->GetPaddingProperty();
        if (padding) {
            DumpPadding(padding, std::string("Padding: "));
        }
        auto& safeAreaPadding = layoutProperty->GetSafeAreaPaddingProperty();
        if (safeAreaPadding) {
            DumpPadding(safeAreaPadding, std::string("SafeAreaPadding: "));
        }
        auto& margin = layoutProperty->GetMarginProperty();
        if (margin) {
            DumpPadding(margin, std::string("Margin: "));
        }
        auto& border = layoutProperty->GetBorderWidthProperty();
        if (border) {
            DumpBorder(border, std::string("Border: "));
        }
        auto layoutRect = layoutProperty->GetLayoutRect();
        if (layoutRect) {
            DumpLog::GetInstance().AddDesc(std::string("LayoutRect: ").append(layoutRect.value().ToString().c_str()));
        }
    }
}

void JsAccessibilityManager::DumpTreeNodeSimplifyInfoNG(
    const RefPtr<NG::FrameNode>& node, int32_t depth, const CommonProperty& commonProperty, int32_t childSize)
{
    DumpTreeNodeCommonInfoNg(node, commonProperty);
    DumpTreeNodeSafeAreaInfoNg(node);
    DumpLog::GetInstance().Print(depth, node->GetTag(), childSize);
}

void JsAccessibilityManager::DumpTreeAccessibilityNodeNG(const RefPtr<NG::UINode>& uiNodeParent, int32_t depth,
    int64_t nodeID, const CommonProperty& commonProperty)
{
    CHECK_NULL_VOID(uiNodeParent);
    auto virtualFrameNode = AceType::DynamicCast<NG::FrameNode>(uiNodeParent);
    auto uiNodeChildren = uiNodeParent->GetChildren(true);
    auto vNode = GetFramenodeByAccessibilityId(virtualFrameNode, nodeID);
    if (!vNode) {
        if (uiNodeChildren.size() == 0) {
            return;
        }
    }
    std::vector<std::pair<int64_t, int32_t>> childrenIdInfo;
    for (const auto& item : uiNodeChildren) {
        GetFrameNodeChildren(item, childrenIdInfo, commonProperty);
    }
    if (vNode != nullptr) {
        DumpTreeNodeInfoNG(vNode, depth + 1, commonProperty, childrenIdInfo.size());
    }
    for (const auto& item : uiNodeChildren) {
        DumpTreeAccessibilityNodeNG(item, depth + 1, item->GetAccessibilityId(), commonProperty);
    }
}

void JsAccessibilityManager::DumpTreeNG(const RefPtr<NG::FrameNode>& parent, int32_t depth,
    int64_t nodeID, const CommonProperty& commonProperty, bool isDumpSimplify)
{
    auto node = GetFramenodeByAccessibilityId(parent, nodeID);
    if (!node) {
        DumpLog::GetInstance().Print("Error: failed to get accessibility node with ID " + std::to_string(nodeID));
        return;
    }
    if (!node->IsActive()) {
        return;
    }
    std::vector<std::pair<int64_t, int32_t>> childrenIdInfo;
    for (const auto& item : node->GetChildren(true)) {
        GetFrameNodeChildren(item, childrenIdInfo, commonProperty);
    }

    auto overlayNode = node->GetOverlayNode();
    if (overlayNode) {
        GetFrameNodeChildren(overlayNode, childrenIdInfo, commonProperty);
    }

    if (isDumpSimplify) {
        DumpTreeNodeSimplifyInfoNG(node, depth, commonProperty, childrenIdInfo.size());
    } else if (!isUseJson_) {
        DumpTreeNodeInfoNG(node, depth, commonProperty, childrenIdInfo.size());
    } else {
        DumpTreeNodeInfoInJson(node, depth, commonProperty, childrenIdInfo.size());
    }
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    auto uiVirtualNode = accessibilityProperty->GetAccessibilityVirtualNode();
    bool hasVirtualNode = false;
    if (uiVirtualNode != nullptr) {
        auto virtualNode = AceType::DynamicCast<NG::FrameNode>(uiVirtualNode);
        CHECK_NULL_VOID(virtualNode);
        hasVirtualNode = true;
        DumpTreeAccessibilityNodeNG(uiVirtualNode, depth+1, virtualNode->GetAccessibilityId(), commonProperty);
    }
    if (IsExtensionComponent(node) && !IsUIExtensionShowPlaceholder(node)) {
        std::list<AccessibilityElementInfo> extensionElementInfos;
        auto pipeline = context_.Upgrade();
        CHECK_NULL_VOID(pipeline);
        SearchElementInfoByAccessibilityIdNG(
            node->GetAccessibilityId(), PREFETCH_RECURSIVE_CHILDREN, extensionElementInfos,
            pipeline, NG::UI_EXTENSION_OFFSET_MAX);
        if (!extensionElementInfos.empty()) {
            DumpAccessibilityElementInfosTreeNG(extensionElementInfos, depth + 1, node->GetAccessibilityId(), true);
        }
    }
    if (!hasVirtualNode) {
        for (const auto& childIdPair : childrenIdInfo) {
            DumpTreeNG(node, depth + 1, childIdPair.first, commonProperty, isDumpSimplify);
        }
    }
}

void JsAccessibilityManager::DumpTree(int32_t depth, int64_t nodeID, bool isDumpSimplify)
{
    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    if (!AceType::InstanceOf<NG::PipelineContext>(pipeline)) {
        AccessibilityNodeManager::DumpTree(depth, nodeID, isDumpSimplify);
    } else {
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
        auto rootNode = ngPipeline->GetRootElement();
        CHECK_NULL_VOID(rootNode);
        nodeID = rootNode->GetAccessibilityId();
        CommonProperty commonProperty;
        GenerateCommonProperty(ngPipeline, commonProperty, pipeline, rootNode);
        DumpTreeNG(rootNode, depth, nodeID, commonProperty, isDumpSimplify);
        for (const auto& subContext : GetSubPipelineContexts()) {
            auto subPipeline = subContext.Upgrade();
            ngPipeline = AceType::DynamicCast<NG::PipelineContext>(subPipeline);
            CHECK_NULL_CONTINUE(ngPipeline);
            rootNode = ngPipeline->GetRootElement();
            CHECK_NULL_CONTINUE(rootNode);
            nodeID = rootNode->GetAccessibilityId();
            commonProperty.windowId = static_cast<int32_t>(ngPipeline->GetWindowId());
            commonProperty.windowLeft = GetWindowLeft(ngPipeline->GetWindowId());
            commonProperty.windowTop = GetWindowTop(ngPipeline->GetWindowId());
            commonProperty.pageNodes.clear(); // empty means search all page
            commonProperty.pagePaths.clear();
            DumpTreeNG(rootNode, depth, nodeID, commonProperty, isDumpSimplify);
        }
    }
}

void JsAccessibilityManager::SetCardViewParams(const std::string& key, bool focus)
{
    callbackKey_ = key;
    if (!callbackKey_.empty()) {
        InitializeCallback();
    }
}

void JsAccessibilityManager::UpdateViewScale()
{
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    float scaleX = 1.0;
    float scaleY = 1.0;
    if (context->GetViewScale(scaleX, scaleY)) {
        scaleX_ = scaleX;
        scaleY_ = scaleY;
    }
}

void JsAccessibilityManager::HandleComponentPostBinding()
{
    for (auto targetIter = nodeWithTargetMap_.begin(); targetIter != nodeWithTargetMap_.end();) {
        auto nodeWithTarget = targetIter->second.Upgrade();
        if (nodeWithTarget) {
            if (nodeWithTarget->GetTag() == ACCESSIBILITY_TAG_POPUP) {
                auto idNodeIter = nodeWithIdMap_.find(targetIter->first);
                if (idNodeIter != nodeWithIdMap_.end()) {
                    auto nodeWithId = idNodeIter->second.Upgrade();
                    if (nodeWithId) {
                        nodeWithId->SetAccessibilityHint(nodeWithTarget->GetText());
                    } else {
                        nodeWithIdMap_.erase(idNodeIter);
                    }
                }
            }
            ++targetIter;
        } else {
            // clear the disabled node in the maps
            targetIter = nodeWithTargetMap_.erase(targetIter);
        }
    }

    // clear the disabled node in the maps
    for (auto idItem = nodeWithIdMap_.begin(); idItem != nodeWithIdMap_.end();) {
        if (!idItem->second.Upgrade()) {
            idItem = nodeWithIdMap_.erase(idItem);
        } else {
            ++idItem;
        }
    }
}

RefPtr<AccessibilityNodeManager> AccessibilityNodeManager::Create()
{
    return AceType::MakeRefPtr<JsAccessibilityManager>();
}

RefPtr<PipelineBase> JsAccessibilityManager::GetPipelineByWindowId(const int32_t windowId)
{
    auto context = context_.Upgrade();
    if (AceType::InstanceOf<NG::PipelineContext>(context)) {
        CHECK_NULL_RETURN(context, nullptr);
        if (context->GetWindowId() == static_cast<uint32_t>(windowId)) {
            return context;
        }
        if (GetWindowId() == static_cast<uint32_t>(windowId)) {
            return context;
        }
        for (auto& subContext : GetSubPipelineContexts()) {
            context = subContext.Upgrade();
            CHECK_NULL_CONTINUE(context);
            if (context->GetWindowId() == static_cast<uint32_t>(windowId)) {
                return context;
            }
        }
        return nullptr;
    } else {
        return context;
    }
}

RetError JsAccessibilityManager::JsInteractionOperation::SearchElementInfoByAccessibilityId(const int64_t elementId,
    const int32_t requestId, AccessibilityElementOperatorCallback& callback, const int32_t mode)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "ArkUI search by id: %{public}" PRId64 ", mode: %{public}d",
        elementId, mode);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        std::list<AccessibilityElementInfo> infos;
        callback.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "SetSearchElementInfoByAccessibilityIdResult, requestId: %{public}d",
            requestId);
        return RET_OK;
    }
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (!context) {
        std::list<AccessibilityElementInfo> infos;
        callback.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "SetSearchElementInfoByAccessibilityIdResult, requestId: %{public}d",
            requestId);
        return RET_OK;
    }
    auto windowId = windowId_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, requestId, &callback, mode, windowId]() {
            auto jsAccessibilityManager = weak.Upgrade();
            if (!jsAccessibilityManager) {
                std::list<AccessibilityElementInfo> infos;
                callback.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
                TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
                         "SetSearchElementInfoByAccessibilityIdResult, requestId: %{public}d", requestId);
                return;
            }
            ACE_SCOPED_TRACE("SearchElementInfoByAccessibilityId");
            jsAccessibilityManager->SearchElementInfoByAccessibilityId(
                splitElementId, requestId, callback, mode, windowId);
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilitySearchElementInfoById");
    return RET_OK;
}

void JsAccessibilityManager::JsInteractionOperation::HandleSearchElementInfoCallback(
    AccessibilityElementOperatorCallback& callback, int32_t requestId)
{
    std::list<AccessibilityElementInfo> infos;
    std::list<AccessibilityElementInfo> treeInfos;
    callback.SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
        "SetSearchElementInfoBySpecificPropertyResult, requestId: %{public}d", requestId);
}

void JsAccessibilityManager::JsInteractionOperation::SearchElementInfoBySpecificProperty(const int64_t elementId,
    const SpecificPropertyParam &param, const int32_t requestId, AccessibilityElementOperatorCallback &callback)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "ArkUI search by id: %{public}" PRId64 ", target: %{public}s,"
        "type: %{public}d", elementId, param.propertyTarget.c_str(), param.propertyType);

    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        std::list<AccessibilityElementInfo> infos;
        std::list<AccessibilityElementInfo> treeInfos;
        callback.SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "SetSearchElementInfoBySpecificPropertyResult, requestId: %{public}d",
            requestId);
        return;
    }

    auto windowId = windowId_;
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (!context) {
        std::list<AccessibilityElementInfo> infos;
        std::list<AccessibilityElementInfo> treeInfos;
        callback.SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "SetSearchElementInfoBySpecificPropertyResult, requestId: %{public}d",
            requestId);
        return;
    }
    auto executor = context->GetTaskExecutor();
    if (!executor) {
        HandleSearchElementInfoCallback(callback, requestId);
        return;
    }

    executor->PostTask(
        [weak = GetHandler(), splitElementId, param, requestId, &callback, windowId]() {
            auto jsAccessibilityManager = weak.Upgrade();
            if (!jsAccessibilityManager) {
                std::list<AccessibilityElementInfo> infos;
                std::list<AccessibilityElementInfo> treeInfos;
                callback.SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
                TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
                    "SetSearchElementInfoBySpecificPropertyResult, requestId: %{public}d", requestId);
                return;
            }
            ACE_SCOPED_TRACE("SearchElementInfoBySpecificProperty");
            jsAccessibilityManager->SearchElementInfoBySpecificProperty(
                splitElementId, param, requestId, callback, windowId);
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilitySearchElementInfoByProperty");
    return;
}

#ifdef WEB_SUPPORTED

RetError JsAccessibilityManager::WebInteractionOperation::SearchElementInfoByAccessibilityId(const int64_t elementId,
    const int32_t requestId, Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t mode)
{
    uint32_t realMode = mode;
    if (realMode & static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN_REDUCED)) {
        realMode &= ~static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN_REDUCED);
        realMode |= static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN);
    }
    TAG_LOGD(AceLogTag::ACE_WEB, "search by id: %{public}" PRId64 ", mode: %{public}d",
        elementId, mode);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_RETURN(jsAccessibilityManager, RET_OK);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_RETURN(context, RET_OK);
    auto windowId = windowId_;
    auto web = webPattern_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, requestId, &callback, realMode, windowId, web]() {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            auto webPattern = web.Upgrade();
            CHECK_NULL_VOID(webPattern);
            ACE_SCOPED_TRACE("SearchWebElementInfoByAccessibilityId");
            jsAccessibilityManager->SearchWebElementInfoByAccessibilityId(
                splitElementId, requestId, callback, realMode, windowId, webPattern);
        },
        TaskExecutor::TaskType::UI, "ArkWebAccessibilitySearchElementInfoById");
    return RET_OK;
}

void JsAccessibilityManager::WebInteractionOperation::SearchElementInfoBySpecificProperty(
    const int64_t elementId, const SpecificPropertyParam &param, const int32_t requestId,
    AccessibilityElementOperatorCallback &callback)
{
    std::list<AccessibilityElementInfo> infos;
    std::list<AccessibilityElementInfo> treeInfos;
    callback.SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
}

void JsAccessibilityManager::WebInteractionOperation::SearchElementInfosByText(const int64_t elementId,
    const std::string& text, const int32_t requestId, Accessibility::AccessibilityElementOperatorCallback& callback)
{
}

void JsAccessibilityManager::WebInteractionOperation::SearchDefaultFocusByWindowId(const int32_t windowId,
    const int32_t requestId, Accessibility::AccessibilityElementOperatorCallback &callback, const int32_t pageId)
{
}

void JsAccessibilityManager::WebInteractionOperation::FindFocusedElementInfo(const int64_t elementId,
    const int32_t focusType, const int32_t requestId, Accessibility::AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "find focus: %{public}" PRId64 ", focusType: %{public}d",
        elementId, focusType);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto windowId = windowId_;
    auto web = webPattern_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, focusType, requestId, &callback, windowId, web]() {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            auto webPattern = web.Upgrade();
            CHECK_NULL_VOID(webPattern);
            ACE_SCOPED_TRACE("FindWebFocusedElementInfo");
            jsAccessibilityManager->FindWebFocusedElementInfo(
                splitElementId, focusType, requestId, callback, windowId, webPattern);
        },
        TaskExecutor::TaskType::UI, "ArkWebFindFocusedElementInfo");
}

void JsAccessibilityManager::WebInteractionOperation::FocusMoveSearch(const int64_t elementId, const int32_t direction,
    const int32_t requestId, Accessibility::AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "move search: %{public}" PRId64 ", direction: %{public}d",
        elementId, direction);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto windowId = windowId_;
    auto web = webPattern_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, direction, requestId, &callback, windowId, web] {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            auto webPattern = web.Upgrade();
            CHECK_NULL_VOID(webPattern);
            ACE_SCOPED_TRACE("FocusMoveSearch");
            jsAccessibilityManager->WebFocusMoveSearch(splitElementId, direction, requestId, callback,
                windowId, webPattern);
        },
        TaskExecutor::TaskType::UI, "ArkWebFocusMoveSearch");
}

void JsAccessibilityManager::WebInteractionOperation::ExecuteAction(const int64_t elementId, const int32_t action,
    const std::map<std::string, std::string>& actionArguments, const int32_t requestId,
    Accessibility::AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "elementId: %{public}" PRId64 ", action: %{public}d", elementId, action);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto actionInfo = static_cast<ActionType>(action);
    ActionParam param { actionInfo, actionArguments };
    auto windowId = windowId_;
    auto web = webPattern_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, param, requestId, &callback, windowId, web] {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            auto webPattern = web.Upgrade();
            CHECK_NULL_VOID(webPattern);
            ACE_SCOPED_TRACE("ExecuteAction");
            jsAccessibilityManager->ExecuteWebAction(splitElementId, param, requestId, callback, windowId, webPattern);
        },
        TaskExecutor::TaskType::UI, "ArkWebAccessibilityExecuteAction");
}

void JsAccessibilityManager::WebInteractionOperation::ClearFocus() {}

void JsAccessibilityManager::WebInteractionOperation::SetChildTreeIdAndWinId(const int64_t nodeId, const int32_t treeId,
    const int32_t childWindowId) {}

void JsAccessibilityManager::WebInteractionOperation::SetBelongTreeId(const int32_t treeId) {}

void JsAccessibilityManager::WebInteractionOperation::GetCursorPosition(
    const int64_t elementId, const int32_t requestId, AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "GetCursorPosition id: %{public}" PRId64, elementId);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto web = webPattern_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, requestId, &callback, web]() {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            auto webPattern = web.Upgrade();
            CHECK_NULL_VOID(webPattern);
            ACE_SCOPED_TRACE("GetWebCursorPosition");
            jsAccessibilityManager->GetWebCursorPosition(splitElementId, requestId, callback, webPattern);
        },

        TaskExecutor::TaskType::UI, "GetWebCursorPosition");
}

void JsAccessibilityManager::WebInteractionOperation::OutsideTouch() {}
#endif

void JsAccessibilityManager::SearchElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
    AccessibilityElementOperatorCallback& callback, const int32_t mode, const int32_t windowId)
{
    // set perf monitor
    HiviewDFX::PerfMonitorAdapter::GetInstance().OnSceneChanged(HiviewDFX::ACCESSIBLE_FEATURE, true);
    std::list<AccessibilityElementInfo> infos;

    auto pipeline = GetPipelineByWindowId(windowId);
    if (pipeline) {
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
        if (ngPipeline) {
            SearchElementInfoByAccessibilityIdNG(elementId, mode, infos, pipeline, NG::UI_EXTENSION_OFFSET_MAX);
            TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
                "SearchElementInfoByAccessibilityIdNG info size: %{public}zu, elementId: %{public}" PRId64,
                infos.size(), elementId);
            SetSearchElementInfoByAccessibilityIdResult(callback, std::move(infos), requestId);
            return;
        }
    }

    int64_t nodeId = elementId;
    if (elementId == -1) {
        nodeId = 0;
    }
    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage(nodeId);
    if (!node) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "SearchElementInfoByAccessibilityIdNG info size: %{public}zu, elementId: %{public}" PRId64,
            infos.size(), elementId);
        SetSearchElementInfoByAccessibilityIdResult(callback, std::move(infos), requestId);
        return;
    }

    AccessibilityElementInfo nodeInfo;
    UpdateAccessibilityNodeInfo(node, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_);
    infos.push_back(nodeInfo);
    // cache parent/siblings/children infos
    UpdateCacheInfo(infos, mode, node, jsAccessibilityManager, jsAccessibilityManager->windowId_);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
        "SearchElementInfoByAccessibilityIdNG info size: %{public}zu, elementId: %{public}" PRId64,
        infos.size(), elementId);
    SetSearchElementInfoByAccessibilityIdResult(callback, std::move(infos), requestId);
}

void JsAccessibilityManager::SearchElementInfoByAccessibilityIdNG(int64_t elementId, int32_t mode,
    std::list<AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context, int64_t uiExtensionOffset)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "elementId: %{public}" PRId64 ", treeId: %{public}d, mode: %{public}d",
        elementId, treeId_, mode);
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);

    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);

    AccessibilityElementInfo nodeInfo;
    int64_t nodeId = elementId;
    if (elementId == -1) {
        nodeId = rootNode->GetAccessibilityId();
    }

#ifdef WINDOW_SCENE_SUPPORTED
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    if (uiExtensionManager->IsWrapExtensionAbilityId(nodeId)) {
        SearchParameter param {nodeId, "", mode, uiExtensionOffset};
        return SearchExtensionElementInfoByAccessibilityIdNG(param, rootNode, infos, context, ngPipeline);
    }
#endif

    auto node = GetFramenodeByAccessibilityId(rootNode, nodeId);
    CHECK_NULL_VOID(node);
    CommonProperty commonProperty;
    GenerateCommonProperty(ngPipeline, commonProperty, mainContext, node);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
        "windowId: %{public}d, windowLeft: %{public}d, "
        "windowTop: %{public}d",
        commonProperty.windowId, commonProperty.windowLeft, commonProperty.windowTop);
    UpdateAccessibilityElementInfo(node, commonProperty, nodeInfo, ngPipeline);
    SetRootAccessibilityVisible(node, nodeInfo);
    SetRootAccessibilityNextFocusId(node, rootNode, nodeInfo);
    SetRootAccessibilityPreFocusId(node, rootNode, nodeInfo,
                                   nextFocusMapWithSubWindow_[context->GetInstanceId()]);
    if (IsExtensionComponent(node) && !IsUIExtensionShowPlaceholder(node)) {
        SearchParameter param {-1, "", mode, uiExtensionOffset};
        SearchExtensionElementInfoNG(param, node, infos, nodeInfo);
    }
    infos.push_back(nodeInfo);
    SearchParameter param {nodeId, "", mode, uiExtensionOffset};
    UpdateCacheInfoNG(infos, node, commonProperty, ngPipeline, param);
    SortExtensionAccessibilityInfo(infos, nodeInfo.GetAccessibilityId());
    if ((infos.size() > 0) && (uiExtensionOffset != NG::UI_EXTENSION_OFFSET_MAX) &&
        (infos.front().GetComponentType() != V2::ROOT_ETS_TAG) &&
        (infos.front().GetParentNodeId() == rootNode->GetAccessibilityId())) {
            infos.front().SetParent(NG::UI_EXTENSION_ROOT_ID);
    }
    UpdateWebEmbedParent(infos, node, commonProperty);
}

bool JsAccessibilityManager::SetAccessibilityCustomId(RefPtr<NG::FrameNode> checkNode, const std::string &customId,
    CommonProperty &commonProperty, std::list<AccessibilityElementInfo> &infos, const RefPtr<PipelineBase>& context)
{
    if (checkNode->GetInspectorId().value_or("") == customId) {
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
        CHECK_NULL_RETURN(ngPipeline, false);
        auto rootNode = ngPipeline->GetRootElement();
        CHECK_NULL_RETURN(rootNode, false);
        AccessibilityElementInfo nodeInfo;
        UpdateAccessibilityElementInfo(checkNode, commonProperty, nodeInfo, ngPipeline);
        SetRootAccessibilityVisible(checkNode, nodeInfo);
        SetRootAccessibilityNextFocusId(checkNode, rootNode, nodeInfo);
        SetRootAccessibilityPreFocusId(checkNode, rootNode, nodeInfo,
            nextFocusMapWithSubWindow_[context->GetInstanceId()]);
        infos.emplace_back(nodeInfo);
        return true;
    }
    return false;
}

bool JsAccessibilityManager::FindUIExtensionAccessibilityElement(const RefPtr<NG::FrameNode>& checkNode,
    const std::string &customId, const CommonProperty& commonProperty,
    std::list<AccessibilityElementInfo> &infos, const RefPtr<PipelineBase>& context)
{
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_RETURN(ngPipeline, false);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_RETURN(rootNode, false);
    if (!((checkNode->GetUiExtensionId() > NG::UI_EXTENSION_UNKNOW_ID) &&
            (checkNode->GetUiExtensionId() <= NG::UI_EXTENSION_ID_FIRST_MAX))) {
        return false;
    }

    SearchParameter searchParam {
        .nodeId = NG::UI_EXTENSION_ROOT_ID,
        .mode = static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN),
        .uiExtensionOffset = NG::UI_EXTENSION_OFFSET_MAX,
    };

    AccessibilityElementInfo parentInfo;
    UpdateAccessibilityElementInfo(checkNode, commonProperty, parentInfo, ngPipeline);
    SetRootAccessibilityVisible(checkNode, parentInfo);
    SetRootAccessibilityNextFocusId(checkNode, rootNode, parentInfo);
    SetRootAccessibilityPreFocusId(checkNode, rootNode, parentInfo,
        nextFocusMapWithSubWindow_[ngPipeline->GetInstanceId()]);

    std::list<Accessibility::AccessibilityElementInfo> extensionElementInfos;
    NG::SearchExtensionElementInfoNG(searchParam, checkNode, extensionElementInfos, parentInfo);

    for (auto& ueaInfo : extensionElementInfos) {
        if (ueaInfo.GetInspectorKey() == customId) {
            infos.emplace_back(ueaInfo);
            break;
        }
    }
    if (!infos.empty()) {
        return true;
    }
    return false;
}

void JsAccessibilityManager::SearchElementInfoByCustomIdNG(const int64_t elementId, const std::string &customId,
    std::list<AccessibilityElementInfo> &infos, std::list<AccessibilityElementInfo> &treeInfos,
    const RefPtr<PipelineBase>& context)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "elementId: %{public}" PRId64 ", customId: %{public}s",
        elementId, customId.c_str());
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);

    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);

    CommonProperty commonProperty;
    GenerateCommonProperty(ngPipeline, commonProperty, mainContext, rootNode);
    int64_t nodeId = elementId;
    if (elementId == -1) {
        nodeId = rootNode->GetAccessibilityId();
    }
    auto node = GetFramenodeByAccessibilityId(rootNode, nodeId);
    CHECK_NULL_VOID(node);
    std::list<RefPtr<NG::FrameNode>> children { node };
    while (!children.empty()) {
        auto checkNode = children.front();
        children.pop_front();
        if (SetAccessibilityCustomId(checkNode, customId, commonProperty, infos, context)) {
            treeInfos.clear();
            break;
        }
#ifdef WINDOW_SCENE_SUPPORTED
        auto isFind = FindUIExtensionAccessibilityElement(checkNode, customId, commonProperty, infos, context);
        if (isFind) {
            treeInfos.clear();
            break;
        }
#endif
        auto accessibilityProperty = checkNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        if (accessibilityProperty && accessibilityProperty->GetChildTreeId() != -1) {
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityElementInfo(checkNode, commonProperty, nodeInfo, ngPipeline);
            SetRootAccessibilityVisible(checkNode, nodeInfo);
            SetRootAccessibilityNextFocusId(checkNode, rootNode, nodeInfo);
            SetRootAccessibilityPreFocusId(checkNode, rootNode, nodeInfo,
                nextFocusMapWithSubWindow_[context->GetInstanceId()]);
            treeInfos.emplace_back(nodeInfo);
        }
        GetChildrenFromFrameNode(checkNode, children, commonProperty);
    }
}

void JsAccessibilityManager::SearchExtensionElementInfoByAccessibilityIdNG(const SearchParameter& searchParam,
    const RefPtr<NG::FrameNode>& node, std::list<Accessibility::AccessibilityElementInfo>& infos,
    const RefPtr<PipelineBase>& context, const RefPtr<NG::PipelineContext>& ngPipeline)
{
#ifdef WINDOW_SCENE_SUPPORTED
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    auto unWrapIdPair = uiExtensionManager->UnWrapExtensionAbilityId(searchParam.uiExtensionOffset, searchParam.nodeId);
    int64_t childWrapId = unWrapIdPair.second;
    int64_t uiExtensionId = unWrapIdPair.first;
    auto uiExtensionNode = FindNodeFromRootByExtensionId(node, uiExtensionId);
    CHECK_NULL_VOID(uiExtensionNode);
    SearchParameter param {childWrapId, "", searchParam.mode, searchParam.uiExtensionOffset};
    AccessibilityElementInfo nodeInfo;
    CommonProperty commonProperty;
    GenerateCommonProperty(ngPipeline, commonProperty, mainContext, uiExtensionNode);
    UpdateAccessibilityElementInfo(uiExtensionNode, commonProperty, nodeInfo, ngPipeline);
    SearchExtensionElementInfoNG(param, uiExtensionNode, infos, nodeInfo);
#endif
}

std::list<AccessibilityElementInfo> JsAccessibilityManager::SearchElementInfosByTextNG(
    int64_t elementId, const std::string& text, const RefPtr<NG::FrameNode>& node, int64_t offset)
{
    std::list<AccessibilityElementInfo> extensionElementInfo;
    if (NG::UI_EXTENSION_OFFSET_MIN < (offset + 1)) {
        node->SearchElementInfosByTextNG(elementId, text, offset, extensionElementInfo);
    }
    return extensionElementInfo;
}

void JsAccessibilityManager::SearchElementInfosByTextNG(const SearchParameter& searchParam,
    const RefPtr<NG::FrameNode>& node, std::list<Accessibility::AccessibilityElementInfo>& infos,
    const RefPtr<PipelineBase>& context, const RefPtr<NG::PipelineContext>& ngPipeline)
{
#ifdef WINDOW_SCENE_SUPPORTED
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    auto unWrapIdPair = uiExtensionManager->UnWrapExtensionAbilityId(searchParam.uiExtensionOffset, searchParam.nodeId);
    int64_t childWrapId = unWrapIdPair.second;
    int64_t uiExtensionId = unWrapIdPair.first;
    std::list<AccessibilityElementInfo> extensionElementInfos;
    AccessibilityElementInfo nodeInfo;
    auto uiExtensionNode = FindNodeFromRootByExtensionId(node, uiExtensionId);
    CHECK_NULL_VOID(uiExtensionNode);

    extensionElementInfos = SearchElementInfosByTextNG(
        childWrapId, searchParam.text, uiExtensionNode, searchParam.uiExtensionOffset / NG::UI_EXTENSION_ID_FACTOR);
    if (extensionElementInfos.empty()) {
        return;
    }
    CommonProperty commonProperty;
    GenerateCommonProperty(ngPipeline, commonProperty, mainContext, uiExtensionNode);
    UpdateAccessibilityElementInfo(uiExtensionNode, commonProperty, nodeInfo, ngPipeline);
    SetRootAccessibilityVisible(node, nodeInfo);

    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    SetRootAccessibilityNextFocusId(node, rootNode, nodeInfo);
    SetRootAccessibilityPreFocusId(node, rootNode, nodeInfo,
                                   nextFocusMapWithSubWindow_[context->GetInstanceId()]);
    ConvertExtensionAccessibilityNodeId(extensionElementInfos, uiExtensionNode,
        searchParam.uiExtensionOffset, nodeInfo);
    for (auto& info : extensionElementInfos) {
        infos.emplace_back(info);
    }
#endif
}

void JsAccessibilityManager::SearchElementInfoBySpecificProperty(const int64_t elementId,
    const SpecificPropertyParam &param, const int32_t requestId,
    AccessibilityElementOperatorCallback &callback, const int32_t windowId)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "elementId: %{public}" PRId64 ", propType: %{public}d,"
        "propTarget: %{public}s", elementId, param.propertyType, param.propertyTarget.c_str());
    std::list<AccessibilityElementInfo> infos;
    std::list<AccessibilityElementInfo> treeInfos;

    auto pipeline = GetPipelineByWindowId(windowId);
    if (pipeline) {
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
        if (ngPipeline) {
            if (param.propertyType == SEARCH_TYPE::CUSTOMID) {
                SearchElementInfoByCustomIdNG(elementId, param.propertyTarget, infos, treeInfos, pipeline);
            }
        } else {
            SearchAccessibilityNodeBySpecificProperty(elementId, param, requestId, callback, windowId);
        }
    }

    SetSearchElementInfoByCustomIdResult(callback, infos, treeInfos, requestId);
    return;
}

RefPtr<NG::FrameNode> JsAccessibilityManager::FindNodeFromRootByExtensionId(
    const RefPtr<NG::FrameNode>& root, const int64_t uiExtensionId)
{
    CHECK_NULL_RETURN(root, nullptr);
    std::queue<RefPtr<NG::UINode>> nodes;
    nodes.push(root);
    RefPtr<NG::FrameNode> frameNode;
    while (!nodes.empty()) {
        auto current = nodes.front();
        nodes.pop();
        frameNode = AceType::DynamicCast<NG::FrameNode>(current);
        if (IsExtensionComponent(frameNode) && !IsUIExtensionShowPlaceholder(frameNode) &&
            (uiExtensionId == frameNode->GetUiExtensionId())) {
            return frameNode;
        }
        const auto& children = current->GetChildren(true);
        for (const auto& child : children) {
            nodes.push(child);
        }
    }
    return nullptr;
}

void JsAccessibilityManager::SearchElementInfosByTextNG(int64_t elementId, const std::string& text,
    std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
    int64_t uiExtensionOffset)
{
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
#ifdef WINDOW_SCENE_SUPPORTED
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    if (uiExtensionManager->IsWrapExtensionAbilityId(elementId)) {
        SearchParameter param {elementId, text, 0, uiExtensionOffset};
        SearchElementInfosByTextNG(param, rootNode, infos, context, ngPipeline);
        return;
    }
#endif
    if (elementId == NG::UI_EXTENSION_ROOT_ID) {
        elementId = rootNode->GetAccessibilityId();
    }
    auto node = GetFramenodeByAccessibilityId(rootNode, elementId);
    CHECK_NULL_VOID(node);
    CommonProperty commonProperty;
    GenerateCommonProperty(ngPipeline, commonProperty, mainContext, node);
    commonProperty.checkEmbedNode = false;
    nlohmann::json textJson = nlohmann::json::parse(text, nullptr, false);
    if (textJson.is_null() || textJson.is_discarded() || !textJson.contains("type")) {
        return;
    }
    if (textJson["type"] == "textType") {
        SearchParameter param {0, text, 0, uiExtensionOffset};
        FindTextByTextHint(node, infos, ngPipeline, commonProperty, param);
        return;
    }
    if (!textJson.contains("value")) {
        return;
    }
    SearchParameter param {0, textJson["value"], 0, uiExtensionOffset};
    FindText(node, infos, ngPipeline, commonProperty, param);
}

void JsAccessibilityManager::JsInteractionOperation::SearchElementInfosByText(const int64_t elementId,
    const std::string& text, const int32_t requestId, AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "search by text: %{public}" PRId64 ", text: %{public}s",
        elementId, text.c_str());
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    if (text.empty()) {
        return;
    }
    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto windowId = windowId_;
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [weak = GetHandler(), splitElementId, text, requestId, &callback, windowId]() {
                auto jsAccessibilityManager = weak.Upgrade();
                CHECK_NULL_VOID(jsAccessibilityManager);
                ACE_SCOPED_TRACE("SearchElementInfosByText");
                jsAccessibilityManager->SearchElementInfosByText(
                    splitElementId, text, requestId, callback, windowId);
            },
            TaskExecutor::TaskType::UI, "ArkUIAccessibilitySearchElementInfoByText");
    }
}

void JsAccessibilityManager::SearchElementInfosByText(const int64_t elementId, const std::string& text,
    const int32_t requestId, AccessibilityElementOperatorCallback& callback, const int32_t windowId)
{
    if (text.empty()) {
        return;
    }

    if (elementId == -1) {
        return;
    }

    std::list<AccessibilityElementInfo> infos;

    auto pipeline = GetPipelineByWindowId(windowId);
    if (pipeline) {
        if (AceType::InstanceOf<NG::PipelineContext>(pipeline)) {
            SearchElementInfosByTextNG(elementId, text, infos, pipeline, NG::UI_EXTENSION_OFFSET_MAX);
            SetSearchElementInfoByTextResult(callback, std::move(infos), requestId);
            return;
        }
    }

    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    int64_t nodeId = elementId;
    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage(nodeId);
    CHECK_NULL_VOID(node);
    std::list<RefPtr<AccessibilityNode>> nodeList;
    OHOS::Ace::Framework::FindText(node, text, nodeList);
    if (!nodeList.empty()) {
        for (const auto& node : nodeList) {
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityNodeInfo(node, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_);
            infos.emplace_back(nodeInfo);
        }
    }

    SetSearchElementInfoByTextResult(callback, std::move(infos), requestId);
}

void JsAccessibilityManager::JsInteractionOperation::SearchDefaultFocusByWindowId(const int32_t windowId,
    const int32_t requestId, AccessibilityElementOperatorCallback& callback, const int32_t pageId)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "search by windowId: %{public}d, pageId: %{public}d.", windowId, pageId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), windowId, pageId, requestId, &callback]() {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            ACE_SCOPED_TRACE("SearchDefaultFocusByWindowId");
            jsAccessibilityManager->SearchDefaultFocusByWindowId(
                windowId, pageId, requestId, callback);
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilitySearchDefaultFocusByWindowId");
}

void JsAccessibilityManager::SearchDefaultFocusByWindowId(const int32_t windowId, int32_t pageId,
    const int32_t requestId, AccessibilityElementOperatorCallback& callback)
{
    std::list<AccessibilityElementInfo> infos;

    auto pipeline = GetPipelineByWindowId(windowId);
    if (pipeline) {
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
        if (ngPipeline) {
            SearchDefaultFocusByWindowIdNG(pageId, infos, pipeline);
            SetSearchDefaultFocusByWindowIdResult(callback, std::move(infos), requestId);
            return;
        }
    }

    auto defaultFocusNodeList = GetDefaultFocusList();
    for (const auto& defaultFocusNode : defaultFocusNodeList) {
        auto frameNode = defaultFocusNode.Upgrade();
        if (frameNode && frameNode->GetPageId() == pageId) {
            auto node = GetAccessibilityNodeFromPage(frameNode->GetAccessibilityId());
            CHECK_NULL_CONTINUE(node);
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityNodeInfo(node, nodeInfo, Claim(this), windowId_);
            infos.emplace_back(nodeInfo);
        }
    }
    SetSearchDefaultFocusByWindowIdResult(callback, std::move(infos), requestId);
}

void JsAccessibilityManager::SearchDefaultFocusByWindowIdNG(const int32_t pageId,
    std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context)
{
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);

    for (const auto& defaultFocusNode : defaultFocusList_) {
        auto node = defaultFocusNode.Upgrade();
        if (node && node->GetPageId() == pageId) {
            AccessibilityElementInfo nodeInfo;

            CommonProperty commonProperty;
            GenerateCommonProperty(ngPipeline, commonProperty, mainContext, node);
            UpdateAccessibilityElementInfo(node, commonProperty, nodeInfo, ngPipeline);
            infos.emplace_back(nodeInfo);
        }
    }
}

void JsAccessibilityManager::JsInteractionOperation::FindFocusedElementInfo(const int64_t elementId,
    const int32_t focusType, const int32_t requestId, AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "find focus: %{public}" PRId64 ", focusType: %{public}d",
        elementId, focusType);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto windowId = windowId_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, focusType, requestId, &callback, windowId]() {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            ACE_SCOPED_TRACE("FindFocusedElementInfo");
            jsAccessibilityManager->FindFocusedElementInfo(splitElementId, focusType, requestId, callback, windowId);
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityFindFocusedElementInfo");
}

void JsAccessibilityManager::FindFocusedElementInfo(const int64_t elementId, const int32_t focusType,
    const int32_t requestId, AccessibilityElementOperatorCallback& callback, const int32_t windowId)
{
    AccessibilityElementInfo nodeInfo;
    if (focusType != FOCUS_TYPE_INPUT && focusType != FOCUS_TYPE_ACCESSIBILITY) {
        nodeInfo.SetValidElement(false);
        SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
        return;
    }

    auto context = GetPipelineByWindowId(windowId);
    if (!context) {
        SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
        return;
    }

    if (AceType::InstanceOf<NG::PipelineContext>(context)) {
        FindFocusedElementInfoNG(elementId, focusType, nodeInfo, context, NG::UI_EXTENSION_OFFSET_MAX);
        SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
        return;
    }

    int64_t nodeId = static_cast<int64_t>(elementId);
    if (elementId == -1) {
        nodeId = 0;
    }

    auto node = GetAccessibilityNodeFromPage(nodeId);
    if (!node) {
        nodeInfo.SetValidElement(false);
        SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
        return;
    }

    RefPtr<AccessibilityNode> resultNode = nullptr;
    bool status = false;
    if (focusType == FOCUS_TYPE_ACCESSIBILITY) {
        status = FindAccessibilityFocus(node, resultNode);
    }
    if (focusType == FOCUS_TYPE_INPUT) {
        status = FindInputFocus(node, resultNode);
    }

    if ((status) && (resultNode != nullptr)) {
        UpdateAccessibilityNodeInfo(resultNode, nodeInfo, Claim(this), windowId_);
    }

    SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
}

void JsAccessibilityManager::FindFocusedElementInfoNG(int64_t elementId, int32_t focusType,
    Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
    const int64_t uiExtensionOffset)
{
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
#ifdef WINDOW_SCENE_SUPPORTED
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    if (uiExtensionManager->IsWrapExtensionAbilityId(elementId)) {
        SearchParameter param {elementId, "", focusType, uiExtensionOffset};
        return FindFocusedExtensionElementInfoNG(param, info, context, rootNode);
    }
#endif
    int64_t nodeId = elementId;
    if (elementId == -1) {
        nodeId = rootNode->GetAccessibilityId();
    }
    auto node = GetFramenodeByAccessibilityId(rootNode, nodeId);
    if (!node) {
        return info.SetValidElement(false);
    }
    if (IsExtensionComponent(node) && !IsUIExtensionShowPlaceholder(node)) {
        SearchParameter transferSearchParam {NG::UI_EXTENSION_ROOT_ID, "", focusType, uiExtensionOffset};
        OHOS::Ace::Framework::FindFocusedExtensionElementInfoNG(transferSearchParam, node, info);
        return SetUiExtensionAbilityParentIdForFocus(node, uiExtensionOffset, info);
    }
    RefPtr<NG::FrameNode> resultNode;
    if (focusType == FOCUS_TYPE_ACCESSIBILITY) {
        resultNode = FindAccessibilityFocus(node, focusType, info, uiExtensionOffset, context, currentFocusNodeId_);
    }
    if (focusType == FOCUS_TYPE_INPUT) {
        resultNode = FindInputFocus(node, focusType, info, uiExtensionOffset, context);
    }
    if ((!resultNode) || (IsExtensionComponent(resultNode) && !IsUIExtensionShowPlaceholder(resultNode))) {
        return;
    }
    CommonProperty commonProperty;
    GenerateCommonProperty(ngPipeline, commonProperty, mainContext, resultNode);
    UpdateAccessibilityElementInfo(resultNode, commonProperty, info, ngPipeline);
    SetRootAccessibilityVisible(resultNode, info);
    SetRootAccessibilityNextFocusId(resultNode, rootNode, info);
    SetRootAccessibilityPreFocusId(resultNode, rootNode, info,
        nextFocusMapWithSubWindow_[context->GetInstanceId()]);
    UpdateUiExtensionParentIdForFocus(rootNode, uiExtensionOffset, info);
}

void JsAccessibilityManager::FindFocusedExtensionElementInfoNG(const SearchParameter& searchParam,
    Accessibility::AccessibilityElementInfo& info,
    const RefPtr<PipelineBase>& context, const RefPtr<NG::FrameNode>& root)
{
#ifdef WINDOW_SCENE_SUPPORTED
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    auto elementIdPair = uiExtensionManager->UnWrapExtensionAbilityId(searchParam.uiExtensionOffset,
        searchParam.nodeId);
    auto uiExtensionNode = uiExtensionManager->GetFocusUiExtensionNode();
    CHECK_NULL_VOID(uiExtensionNode);
    SearchParameter transferSearchParam {elementIdPair.second, "",
        searchParam.mode, searchParam.uiExtensionOffset};
    OHOS::Ace::Framework::FindFocusedExtensionElementInfoNG(transferSearchParam, uiExtensionNode, info);
    SetUiExtensionAbilityParentIdForFocus(uiExtensionNode, searchParam.uiExtensionOffset, info);
#endif
}

RefPtr<NG::FrameNode> JsAccessibilityManager::FindNodeFromPipeline(
    const WeakPtr<PipelineBase>& context, const int64_t elementId)
{
    auto pipeline = context.Upgrade();
    CHECK_NULL_RETURN(pipeline, nullptr);

    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    CHECK_NULL_RETURN(ngPipeline, nullptr);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_RETURN(rootNode, nullptr);

    int64_t nodeId = elementId;
    // accessibility use -1 for first search to get root node
    if (elementId == -1) {
        nodeId = rootNode->GetAccessibilityId();
    }

    auto node = GetFramenodeByAccessibilityId(rootNode, nodeId);
    if (node) {
        return node;
    }
    return nullptr;
}

RefPtr<NG::PipelineContext> JsAccessibilityManager::FindPipelineByElementId(
    const int64_t elementId, RefPtr<NG::FrameNode>& node)
{
    node = FindNodeFromPipeline(context_, elementId);
    if (node) {
        auto context = AceType::DynamicCast<NG::PipelineContext>(context_.Upgrade());
        return context;
    }
    for (auto subContext : GetSubPipelineContexts()) {
        node = FindNodeFromPipeline(subContext, elementId);
        if (node) {
            auto context = AceType::DynamicCast<NG::PipelineContext>(subContext.Upgrade());
            return context;
        }
    }
    return nullptr;
}

void JsAccessibilityManager::JsInteractionOperation::ExecuteAction(const int64_t elementId, const int32_t action,
    const std::map<std::string, std::string>& actionArguments, const int32_t requestId,
    AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "ExecuteAction elementId: %{public}" PRId64 ", action: %{public}d",
             elementId, action);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    if (!jsAccessibilityManager) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "ExecuteAction failed, requestId: %{public}d", requestId);
        callback.SetExecuteActionResult(false, requestId);
        return;
    }
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    if (!context) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "ExecuteAction failed, requestId: %{public}d", requestId);
        callback.SetExecuteActionResult(false, requestId);
        return;
    }
    auto actionInfo = static_cast<ActionType>(action);
    ActionParam param { actionInfo, actionArguments };
    auto windowId = windowId_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, param, requestId, &callback, windowId] {
            auto jsAccessibilityManager = weak.Upgrade();
            if (!jsAccessibilityManager) {
                TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "ExecuteAction failed, requestId: %{public}d", requestId);
                callback.SetExecuteActionResult(false, requestId);
                return;
            }
            ACE_SCOPED_TRACE("ExecuteAction");
            jsAccessibilityManager->ExecuteAction(splitElementId, param, requestId, callback, windowId);
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityExecuteAction");
}

bool JsAccessibilityManager::AccessibilityActionEvent(const ActionType& action,
    const std::map<std::string, std::string>& actionArguments, const RefPtr<AccessibilityNode>& node,
    const RefPtr<PipelineContext>& context)
{
    if (!node || !context) {
        return false;
    }
    ContainerScope scope(context->GetInstanceId());
    switch (action) {
        case ActionType::ACCESSIBILITY_ACTION_CLICK: {
            node->SetClicked(true);
            if (!node->GetClickEventMarker().IsEmpty()) {
#ifndef NG_BUILD
                context->SendEventToFrontend(node->GetClickEventMarker());
#endif
                node->ActionClick();
                return true;
            }
            return node->ActionClick();
        }
        case ActionType::ACCESSIBILITY_ACTION_LONG_CLICK: {
            if (!node->GetLongPressEventMarker().IsEmpty()) {
#ifndef NG_BUILD
                context->SendEventToFrontend(node->GetLongPressEventMarker());
#endif
                node->ActionLongClick();
                return true;
            }
            return node->ActionLongClick();
        }
        case ActionType::ACCESSIBILITY_ACTION_SET_TEXT: {
            if (!node->GetSetTextEventMarker().IsEmpty()) {
#ifndef NG_BUILD
                context->SendEventToFrontend(node->GetSetTextEventMarker());
#endif
                node->ActionSetText(actionArguments.find(ACTION_ARGU_SET_TEXT)->second);
                return true;
            }
            return node->ActionSetText(actionArguments.find(ACTION_ARGU_SET_TEXT)->second);
        }
        case ActionType::ACCESSIBILITY_ACTION_FOCUS: {
#ifndef NG_BUILD
            context->AccessibilityRequestFocus(std::to_string(node->GetNodeId()));
#endif
            if (!node->GetFocusEventMarker().IsEmpty()) {
#ifndef NG_BUILD
                context->SendEventToFrontend(node->GetFocusEventMarker());
#endif
                node->ActionFocus();
                return true;
            }
            return node->ActionFocus();
        }
        case ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS: {
            return RequestAccessibilityFocus(node);
        }
        case ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS: {
            return ClearAccessibilityFocus(node);
        }
        case ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD:
            return node->ActionScrollForward();
        case ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD:
            return node->ActionScrollBackward();
        default:
            return false;
    }
}

void JsAccessibilityManager::SendActionEvent(const Accessibility::ActionType& action, int64_t nodeId)
{
    static std::unordered_map<Accessibility::ActionType, std::string> actionToStr {
        { Accessibility::ActionType::ACCESSIBILITY_ACTION_CLICK, DOM_CLICK },
        { Accessibility::ActionType::ACCESSIBILITY_ACTION_LONG_CLICK, DOM_LONG_PRESS },
        { Accessibility::ActionType::ACCESSIBILITY_ACTION_FOCUS, DOM_FOCUS },
        { Accessibility::ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, ACCESSIBILITY_FOCUSED_EVENT },
        { Accessibility::ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, ACCESSIBILITY_CLEAR_FOCUS_EVENT },
        { Accessibility::ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD, SCROLL_END_EVENT },
        { Accessibility::ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD, SCROLL_END_EVENT },
    };
    if (actionToStr.find(action) == actionToStr.end()) {
        return;
    }
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.eventType = actionToStr[action];
    accessibilityEvent.nodeId = static_cast<int64_t>(nodeId);
    SendAccessibilityAsyncEvent(accessibilityEvent);
}

namespace {
void stringToLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](char &c) {
        return std::tolower(c);
    });
}

bool conversionDirection(std::string dir)
{
    stringToLower(dir);
    if (dir.compare(STRING_DIR_FORWARD) == 0) {
        return true;
    }

    return false;
}

int32_t getArgumentByKey(const std::map<std::string, std::string>& actionArguments, const std::string& checkKey)
{
    auto iter = actionArguments.find(checkKey);
    int32_t argument = -1; // -1:default value
    if (iter != actionArguments.end()) {
        std::stringstream strArguments;
        strArguments << iter->second;
        strArguments >> argument;
    }
    return argument;
}

uint64_t GetUnsignedLongArgumentByKey(const std::map<std::string, std::string>& actionArguments,
    const std::string& checkKey)
{
    auto iter = actionArguments.find(checkKey);
    uint64_t argument = 0; // 0:default value
    if (iter != actionArguments.end()) {
        std::stringstream strArguments;
        strArguments << iter->second;
        strArguments >> argument;
    }
    return argument;
}

std::string GetStrArgumentByKey(const std::map<std::string, std::string>& actionArguments, const std::string& checkKey)
{
    std::string result;
    auto iter = actionArguments.find(checkKey);
    if (iter != actionArguments.end()) {
        result = iter->second;
    }
    return result;
}

AccessibilityScrollType findAccessibilityScrollType(int32_t accessibilityScrollTypeValue)
{
    switch (accessibilityScrollTypeValue) {
        case 0:
            return AccessibilityScrollType::SCROLL_HALF;
        case 1:
            return AccessibilityScrollType::SCROLL_FULL;
        default:
            return AccessibilityScrollType::SCROLL_DEFAULT;
    }
}

NG::SecCompEnhanceEvent GetAccessibilitySecCompEnhanceEvent(const std::map<std::string, std::string>& actionArguments)
{
    NG::SecCompEnhanceEvent secEvent;
    auto hmac = GetStrArgumentByKey(actionArguments, ACTION_ARGU_CLICK_ENHANCE_DATA);
    if (hmac.empty()) {
        return secEvent;
    }
    auto time = GetUnsignedLongArgumentByKey(actionArguments, ACTION_ARGU_CLICK_TIMESTAMP);
    secEvent.dataBuffer.assign(hmac.begin(), hmac.end());
    std::chrono::microseconds microseconds(time);
    secEvent.time = TimeStamp { microseconds };
    return secEvent;
}

AccessibilityScrollType getAccessibilityScrollType(const std::map<std::string, std::string>& actionArguments,
    const std::string& checkKey)
{
    auto argument = getArgumentByKey(actionArguments, checkKey);
    return findAccessibilityScrollType(argument);
}


bool ActAccessibilityAction(Accessibility::ActionType action, const std::map<std::string, std::string>& actionArguments,
    RefPtr<NG::AccessibilityProperty> accessibilityProperty)
{
    AccessibilityActionParam param;
    if (action == ActionType::ACCESSIBILITY_ACTION_SET_SELECTION) {
        std::string dir = STRING_DIR_BACKWARD;
        auto iter = actionArguments.find(ACTION_ARGU_SELECT_TEXT_INFORWARD);
        if (iter != actionArguments.end()) {
            dir = iter->second;
        }
        param.setSelectionStart = getArgumentByKey(actionArguments, ACTION_ARGU_SELECT_TEXT_START);
        param.setSelectionEnd = getArgumentByKey(actionArguments, ACTION_ARGU_SELECT_TEXT_END);
        param.setSelectionDir = conversionDirection(dir);
    }
    if (action == ActionType::ACCESSIBILITY_ACTION_SET_TEXT) {
        auto iter = actionArguments.find(ACTION_ARGU_SET_TEXT);
        if (iter != actionArguments.end()) {
            param.setTextArgument = iter->second;
        }
    }
    if (action == ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT ||
        action == ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT) {
        int moveUnit = TextMoveUnit::STEP_CHARACTER;
        auto iter = actionArguments.find(ACTION_ARGU_MOVE_UNIT);
        if (iter != actionArguments.end()) {
            std::stringstream str_moveUnit;
            str_moveUnit << iter->second;
            str_moveUnit >> moveUnit;
        }
        param.moveUnit = static_cast<TextMoveUnit>(moveUnit);
    }
    if (action == ActionType::ACCESSIBILITY_ACTION_SET_CURSOR_POSITION) {
        param.setCursorIndex = getArgumentByKey(actionArguments, ACTION_ARGU_SET_OFFSET);
    }
    if ((action == ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD) ||
        (action == ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD)) {
        param.scrollType = getAccessibilityScrollType(actionArguments, ACTION_ARGU_SCROLL_STUB);
    }
    if (action == ActionType::ACCESSIBILITY_ACTION_SPAN_CLICK) {
        param.spanId = getArgumentByKey(actionArguments, ACTION_ARGU_SPAN_ID);
    }
    auto accessibiltyAction = ACTIONS.find(action);
    if (accessibiltyAction != ACTIONS.end()) {
        param.accessibilityProperty = accessibilityProperty;
        return accessibiltyAction->second(param);
    }
    return false;
}
}

bool JsAccessibilityManager::ExecuteExtensionActionNG(int64_t elementId,
    const std::map<std::string, std::string>& actionArguments, int32_t action, const RefPtr<PipelineBase>& context,
    int64_t uiExtensionOffset)
{
    return ExecuteActionNG(
        elementId, actionArguments, static_cast<ActionType>(action), context, uiExtensionOffset);
}

bool JsAccessibilityManager::ExecuteActionNG(int64_t elementId,
    const std::map<std::string, std::string>& actionArguments, ActionType action, const RefPtr<PipelineBase>& context,
    int64_t uiExtensionOffset)
{
    if (GetIsIgnoreAllAction() && !(action == ActionType::ACCESSIBILITY_ACTION_FOCUS ||
                                    action == ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS)) {
        return false;
    }
    bool result = false;
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_RETURN(ngPipeline, result);
#ifdef WINDOW_SCENE_SUPPORTED
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_RETURN(uiExtensionManager, result);
    if (uiExtensionManager->IsWrapExtensionAbilityId(elementId)) {
        auto unWrapIdPair = uiExtensionManager->UnWrapExtensionAbilityId(uiExtensionOffset, elementId);
        int64_t childWrapId = unWrapIdPair.second;
        int64_t uiExtensionId = unWrapIdPair.first;
        auto rootNode = ngPipeline->GetRootElement();
        CHECK_NULL_RETURN(rootNode, result);
        auto uiExtensionNode = FindNodeFromRootByExtensionId(rootNode, uiExtensionId);
        CHECK_NULL_RETURN(uiExtensionNode, result);
        return OHOS::Ace::Framework::TransferExecuteAction(
            childWrapId, uiExtensionNode, actionArguments, action, uiExtensionOffset);
    }
#endif
    ContainerScope instance(ngPipeline->GetInstanceId());
    auto frameNode = GetFramenodeByAccessibilityId(ngPipeline->GetRootElement(), elementId);

    if (!frameNode && elementId == lastElementId_) {
        frameNode = lastFrameNode_.Upgrade();
    }


    CHECK_NULL_RETURN(frameNode, result);

    auto enabled = frameNode->GetFocusHub() ? frameNode->GetFocusHub()->IsEnabled() : true;
    if (!enabled && action != ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS &&
        action != ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS) {
        return result;
    }
    result = ConvertActionTypeToBoolen(action, frameNode, elementId, ngPipeline, actionArguments);
    if (!result) {
        auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        CHECK_NULL_RETURN(accessibilityProperty, false);
        result = ActAccessibilityAction(action, actionArguments, accessibilityProperty);
        if (!result) {
            EventReport::ReportAccessibilityFailEvent(ConvertActionTypeToString(action));
        }
    }
    return result;
}

bool JsAccessibilityManager::ConvertActionTypeToBoolen(ActionType action, RefPtr<NG::FrameNode>& frameNode,
    int64_t elementId, RefPtr<NG::PipelineContext>& context, const std::map<std::string, std::string>& actionArguments)
{
    bool result = false;
    NG::SecCompEnhanceEvent secEvent;
    switch (action) {
        case ActionType::ACCESSIBILITY_ACTION_FOCUS: {
            result = RequestFocus(frameNode);
            break;
        }
        case ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS: {
            result = LostFocus(frameNode);
            break;
        }
        case ActionType::ACCESSIBILITY_ACTION_CLICK: {
            secEvent = GetAccessibilitySecCompEnhanceEvent(actionArguments);
            result = ActClick(frameNode, secEvent);
            break;
        }
        case ActionType::ACCESSIBILITY_ACTION_LONG_CLICK: {
            result = ActLongClick(frameNode);
            break;
        }
        case ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS: {
            SaveLast(elementId, frameNode);
            SaveCurrentFocusNodeSize(frameNode);
            AccessibilityFocusInfo focusInfo{ currentFocusNodeId_, currentFocusVirtualNodeParentId_ };
            result = ActAccessibilityFocus(elementId, frameNode, context, focusInfo, false);
            currentFocusNodeId_ = focusInfo.currentFocusNodeId;
            currentFocusVirtualNodeParentId_ = focusInfo.currentFocusVirtualNodeParentId;
            break;
        }
        case ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS: {
            SaveLast(elementId, frameNode);
            AccessibilityFocusInfo focusInfo{ currentFocusNodeId_, currentFocusVirtualNodeParentId_ };
            result = ActAccessibilityFocus(elementId, frameNode, context, focusInfo, true);
            currentFocusNodeId_ = focusInfo.currentFocusNodeId;
            currentFocusVirtualNodeParentId_ = focusInfo.currentFocusVirtualNodeParentId;
            break;
        }
        default:
            break;
    }
    return result;
}

void JsAccessibilityManager::ExecuteAction(const int64_t elementId, const ActionParam& param, const int32_t requestId,
    AccessibilityElementOperatorCallback& callback, const int32_t windowId)
{
    auto action = param.action;
    auto actionArguments = param.actionArguments;

    bool actionResult = false;
    auto context = GetPipelineByWindowId(windowId);
    if (!context) {
        SetExecuteActionResult(callback, actionResult, requestId);
        return;
    }

    if (AceType::InstanceOf<NG::PipelineContext>(context)) {
        actionResult = ExecuteActionNG(elementId, actionArguments, action, context, NG::UI_EXTENSION_OFFSET_MAX);
    } else {
        auto node = GetAccessibilityNodeFromPage(elementId);
        if (!node) {
            SetExecuteActionResult(callback, false, requestId);
            return;
        }

        actionResult =
            AccessibilityActionEvent(action, actionArguments, node, AceType::DynamicCast<PipelineContext>(context));
    }
    SetExecuteActionResult(callback, actionResult, requestId);
    if (actionResult && AceType::InstanceOf<PipelineContext>(context)) {
        SendActionEvent(action, elementId);
    }
}

void JsAccessibilityManager::JsInteractionOperation::GetCursorPosition(const int64_t elementId,
    const int32_t requestId, AccessibilityElementOperatorCallback &callback)
{
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    RefPtr<NG::FrameNode> node;
    auto ngPipeline = jsAccessibilityManager->FindPipelineByElementId(splitElementId, node);
    CHECK_NULL_VOID(ngPipeline);
#ifdef WINDOW_SCENE_SUPPORTED
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    if (uiExtensionManager->IsWrapExtensionAbilityId(splitElementId)) {
        auto unWrapIdPair = uiExtensionManager->UnWrapExtensionAbilityId(NG::UI_EXTENSION_OFFSET_MAX, elementId);
        int64_t uiExtensionId = unWrapIdPair.first;
        auto rootNode = ngPipeline->GetRootElement();
        CHECK_NULL_VOID(rootNode);
        auto uiExtensionNode = jsAccessibilityManager->FindNodeFromRootByExtensionId(rootNode, uiExtensionId);
        CHECK_NULL_VOID(uiExtensionNode);
        auto accessibilityProperty = uiExtensionNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
        CHECK_NULL_VOID(accessibilityProperty);
        auto callNumber = accessibilityProperty->ActActionGetIndex();
        callback.SetCursorPositionResult(callNumber, requestId);
        return;
    }
#endif
    auto frameNode = GetFramenodeByAccessibilityId(ngPipeline->GetRootElement(), splitElementId);
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    auto callNumber = accessibilityProperty->ActActionGetIndex();
    callback.SetCursorPositionResult(callNumber, requestId);
}

void JsAccessibilityManager::JsInteractionOperation::ClearFocus()
{
    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler()] {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            ACE_SCOPED_TRACE("ClearCurrentFocus");
            jsAccessibilityManager->ClearCurrentFocus();
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityClearCurrentFocus");
}

void JsAccessibilityManager::JsInteractionOperation::OutsideTouch() {}
#ifdef WEB_SUPPORTED

std::shared_ptr<NG::TransitionalNodeInfo> GetChildrenFromWebNode(
    int64_t nodeId, std::list<int64_t>& children,
    const RefPtr<NG::PipelineContext>& ngPipeline, const RefPtr<NG::WebPattern>& webPattern)
{
    std::shared_ptr<NG::TransitionalNodeInfo> node = nullptr;
    std::list<int64_t> webNodeChildren;
    if (AceApplicationInfo::GetInstance().IsAccessibilityEnabled()) {
        int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
        int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
        AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(nodeId, splitElementId, splitTreeId);
        node = webPattern->GetTransitionalNodeById(splitElementId);
        CHECK_NULL_RETURN(node, nullptr);
        auto treeId = webPattern->GetTreeId();
        for (auto& childIdConst : node->GetChildIds()) {
            int64_t mutableChildId = childIdConst;
            AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, mutableChildId);
            webNodeChildren.emplace_back(mutableChildId);
        }
    }
    while (!webNodeChildren.empty()) {
        children.emplace_back(webNodeChildren.front());
        webNodeChildren.pop_front();
    }
    return node;
}

void JsAccessibilityManager::SearchWebElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
    AccessibilityElementOperatorCallback& callback, const int32_t mode, const int32_t windowId,
    const RefPtr<NG::WebPattern>& webPattern)
{
    std::list<AccessibilityElementInfo> infos;

    auto pipeline = GetPipelineByWindowId(windowId);
    CHECK_NULL_VOID(pipeline);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    CHECK_NULL_VOID(ngPipeline);

    if (!ngPipeline->GetOnFocus() && (SystemProperties::GetDeviceType() == DeviceType::TWO_IN_ONE)) {
        TAG_LOGD(AceLogTag::ACE_WEB,
            "SearchWebElementInfo GetOnFocus, elementId: %{public}" PRId64
            ", requestId: %{public}d, mode: %{public}d, windowId: %{public}d",
            elementId, requestId, mode, windowId);
        SetSearchElementInfoByAccessibilityIdResult(callback, std::move(infos), requestId, true);
        return;
    }
    CHECK_NULL_VOID(webPattern);

    if (elementId == -1) {
        auto webNode = webPattern->GetHost();
        AccessibilityElementInfo webInfo;
        SetRootAccessibilityVisible(webNode, webInfo);
        if (!webInfo.GetAccessibilityVisible()) {
            TAG_LOGD(AceLogTag::ACE_WEB,
                "SearchWebElementinfo accessibility visible false, elementId: %{public}" PRId64
                ", requestId: %{public}d, mode: %{public}d, windowId: %{public}d",
                elementId, requestId, mode, windowId);
            SetSearchElementInfoByAccessibilityIdResult(callback, std::move(infos), requestId, true);
            return;
        }
    }

    SearchWebElementInfoByAccessibilityIdNG(elementId, mode, infos, ngPipeline, webPattern);
    SetSearchElementInfoByAccessibilityIdResult(callback, std::move(infos), requestId, true);
}

void JsAccessibilityManager::SearchWebElementInfoByAccessibilityIdNG(int64_t elementId, int32_t mode,
    std::list<AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
    const RefPtr<NG::WebPattern>& webPattern)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "elementId: %{public}" PRId64 ", treeId: %{public}d, mode: %{public}d",
        elementId, treeId_, mode);
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);

    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);

    AccessibilityElementInfo nodeInfo;

    CHECK_NULL_VOID(webPattern);
    auto webNode = webPattern->GetHost();
    CHECK_NULL_VOID(webNode);
    CommonProperty commonProperty;
    GenerateCommonPropertyForWeb(ngPipeline, commonProperty, mainContext, webNode);

    auto node = webPattern->GetTransitionalNodeById(elementId);
    CHECK_NULL_VOID(node);
    UpdateWebAccessibilityElementInfo(node, commonProperty, nodeInfo, webPattern);
    nodeInfo.SetAccessibilityVisible(webPattern->GetAccessibilityVisible(elementId));
    infos.push_back(nodeInfo);
    SearchParameter param {elementId, "", mode, 0};
    UpdateWebCacheInfo(infos, elementId, commonProperty, ngPipeline, param, webPattern);
}

void JsAccessibilityManager::FindWebFocusedElementInfo(const int64_t elementId, const int32_t focusType,
    const int32_t requestId, AccessibilityElementOperatorCallback& callback, const int32_t windowId,
    const RefPtr<NG::WebPattern>& webPattern)
{
    AccessibilityElementInfo nodeInfo;
    if (focusType != FOCUS_TYPE_INPUT && focusType != FOCUS_TYPE_ACCESSIBILITY) {
        nodeInfo.SetValidElement(false);
        SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
        return;
    }

    auto context = GetPipelineByWindowId(windowId);
    if (!context || AceType::InstanceOf<NG::PipelineContext>(context)) {
        nodeInfo.SetValidElement(false);
        SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
        return;
    }

    FindWebFocusedElementInfoNG(elementId, focusType, nodeInfo, context, webPattern);
    SetFindFocusedElementInfoResult(callback, nodeInfo, requestId);
}

void JsAccessibilityManager::FindWebFocusedElementInfoNG(int64_t elementId, int32_t focusType,
    Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
    const RefPtr<NG::WebPattern>& webPattern)
{
    CHECK_NULL_VOID(webPattern);
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto node = webPattern->GetFocusedAccessibilityNode(elementId, focusType == FOCUS_TYPE_ACCESSIBILITY);
    if (!node) {
        info.SetValidElement(false);
        return;
    }
    CHECK_NULL_VOID(node);

    auto webNode = webPattern->GetHost();
    CHECK_NULL_VOID(webNode);
    CommonProperty commonProperty;
    GenerateCommonPropertyForWeb(ngPipeline, commonProperty, mainContext, webNode);
    UpdateWebAccessibilityElementInfo(node, commonProperty, info, webPattern);
}

void JsAccessibilityManager::WebFocusMoveSearch(const int64_t elementId, const int32_t direction,
    const int32_t requestId, Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId,
    const RefPtr<NG::WebPattern>& webPattern)
{
    AccessibilityElementInfo nodeInfo;
    auto context = GetPipelineByWindowId(windowId);
    if (!context) {
        nodeInfo.SetValidElement(false);
        SetFocusMoveSearchResult(callback, nodeInfo, requestId);
        return;
    }

    WebFocusMoveSearchNG(elementId, direction, nodeInfo, context, webPattern);
    WebFocusMoveSearchByComponent(nodeInfo, webPattern, direction, context);
    SetFocusMoveSearchResult(callback, nodeInfo, requestId);
}

void JsAccessibilityManager::WebFocusMoveSearchByComponent(AccessibilityElementInfo& nodeInfo,
    const RefPtr<NG::WebPattern>& webPattern, const int32_t direction, RefPtr<PipelineBase> context)
{
    if (!IsTagInEmbedComponent(nodeInfo.GetComponentType())) {
        return;
    }
    int64_t accessibilityId = nodeInfo.GetAccessibilityId();
    CHECK_NULL_VOID(webPattern);
    std::shared_ptr<NG::TransitionalNodeInfo> transitionalNodeInfo =
        webPattern->GetTransitionalNodeById(accessibilityId);
    CHECK_NULL_VOID(transitionalNodeInfo);
    std::string surfaceId = webPattern->GetSurfaceIdByHtmlElementId(transitionalNodeInfo->GetHtmlElementId());
    if (surfaceId == "") {
        return;
    }
    std::list<AccessibilityElementInfo> embedNodeTreeInfo;
    int32_t windowId = nodeInfo.GetWindowId();
    SearchSurfaceIdRet searchSurfaceIdRet = SearchSurfaceIdRet::NO_MATCH_NODE;
    if (direction == FocusMoveDirection::FORWARD) {
        searchSurfaceIdRet =
            SearchElementInfoBySurfaceId(surfaceId, windowId, SearchSurfaceIdType::SEARCH_HEAD, embedNodeTreeInfo);
    } else if (direction == FocusMoveDirection::BACKWARD) {
        searchSurfaceIdRet =
            SearchElementInfoBySurfaceId(surfaceId, windowId, SearchSurfaceIdType::SEARCH_TAIL, embedNodeTreeInfo);
    }
    if (searchSurfaceIdRet != SearchSurfaceIdRet::SEARCH_SUCCESS || embedNodeTreeInfo.empty()) {
        WebFocusMoveSearchNG(accessibilityId, direction, nodeInfo, context, webPattern);
    } else {
        nodeInfo = embedNodeTreeInfo.front();
    }
}

void JsAccessibilityManager::WebFocusMoveSearchNG(int64_t elementId, int32_t direction,
    AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
    const RefPtr<NG::WebPattern>& webPattern)
{
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);

    CHECK_NULL_VOID(webPattern);
    auto webNode = webPattern->GetHost();
    CHECK_NULL_VOID(webNode);
    CommonProperty commonProperty;
    GenerateCommonPropertyForWeb(ngPipeline, commonProperty, mainContext, webNode);

    auto node = webPattern->GetAccessibilityNodeByFocusMove(elementId, direction);
    if (node) {
        UpdateWebAccessibilityElementInfo(node, commonProperty, info, webPattern);
    } else {
        auto webNode = webPattern->GetHost();
        CHECK_NULL_VOID(webNode);
        UpdateAccessibilityElementInfo(webNode, commonProperty, info, ngPipeline);
    }
}

bool JsAccessibilityManager::ExecuteWebActionNG(int64_t elementId, ActionType action,
    const std::map<std::string, std::string>& actionArguments, const RefPtr<NG::WebPattern>& webPattern)
{
    CHECK_NULL_RETURN(webPattern, false);
    return webPattern->ExecuteAction(elementId, ConvertAccessibilityAction(action), actionArguments);
}

void JsAccessibilityManager::ExecuteWebAction(const int64_t elementId, const ActionParam& param,
    const int32_t requestId, AccessibilityElementOperatorCallback& callback, const int32_t windowId,
    const RefPtr<NG::WebPattern>& webPattern)
{
    auto action = param.action;
    auto actionArguments = param.actionArguments;

    bool actionResult = false;
    auto context = GetPipelineByWindowId(windowId);
    if (!context || !AceType::InstanceOf<NG::PipelineContext>(context)) {
        SetExecuteActionResult(callback, actionResult, requestId);
        return;
    }

    actionResult = ExecuteWebActionNG(elementId, action, actionArguments, webPattern);
    SetExecuteActionResult(callback, actionResult, requestId);
}

bool JsAccessibilityManager::RegisterWebInteractionOperationAsChildTree(int64_t accessibilityId,
    const WeakPtr<NG::WebPattern>& webPattern)
{
    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(instance, false);
    auto pipelineContext = GetPipelineContext().Upgrade();
    CHECK_NULL_RETURN(pipelineContext, false);
    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    if (container != nullptr && container->IsUIExtensionWindow()) {
        windowId_ = pipelineContext->GetRealHostWindowId();
    }

    AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId_, accessibilityId);

    uint32_t windowId = GetWindowId();
    auto interactionOperation = std::make_shared<WebInteractionOperation>(windowId);
    interactionOperation->SetHandler(WeakClaim(this));
    interactionOperation->SetWebPattern(webPattern);
    auto pattern = webPattern.Upgrade();
    CHECK_NULL_RETURN(pattern, false);
    Accessibility::Registration registration {
        .windowId = static_cast<int32_t>(windowId),
        .parentWindowId = static_cast<int32_t>(windowId),
        .parentTreeId = treeId_,
        .elementId = accessibilityId,
    };
    parentWebWindowId_ = windowId;
    TAG_LOGI(AceLogTag::ACE_WEB, "windowId: %{public}u, parentWindowId: %{public}u, "
        "parentTreeId: %{public}d, elementId %{public}" PRId64,
        windowId, windowId, treeId_, accessibilityId);
    Accessibility::RetError retReg = instance->RegisterElementOperator(registration, interactionOperation);
    TAG_LOGI(AceLogTag::ACE_WEB, "RegisterWebInteractionOperationAsChildTree result: %{public}d", retReg);
    return retReg == RET_OK;
}

bool JsAccessibilityManager::DeregisterWebInteractionOperationAsChildTree(int32_t treeId)
{
    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(instance, false);
    uint32_t windowId = GetWindowId();
    Accessibility::RetError retReg = instance->DeregisterElementOperator(windowId, treeId);
    return retReg == RET_OK;
}

void JsAccessibilityManager::UpdateWebCacheInfo(std::list<AccessibilityElementInfo>& infos,
    int64_t nodeId, const CommonProperty& commonProperty, const RefPtr<NG::PipelineContext>& ngPipeline,
    const SearchParameter& searchParam, const RefPtr<NG::WebPattern>& webPattern)
{
    uint32_t umode = searchParam.mode;
    std::unordered_map<int64_t, AccessibilityElementInfo> allEmbedNodeTreeInfosMap;
    std::list<int64_t> children;
    // get all children
    if (!(umode & static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN))) {
        return;
    }
    GetChildrenFromWebNode(nodeId, children, ngPipeline, webPattern);
    // The application needs to insert node information in the order of BFS
    while (!children.empty()) {
        int64_t parent = children.front();
        children.pop_front();
        AccessibilityElementInfo nodeInfo;
        if (GetChildrenFromEmbedNode(nodeInfo, parent, children, allEmbedNodeTreeInfosMap)) {
            PushElementsIntoInfos(nodeInfo, infos, webPattern, children, allEmbedNodeTreeInfosMap);
            continue;
        }

        auto node = GetChildrenFromWebNode(parent, children, ngPipeline, webPattern);
        if (node) {
            UpdateWebAccessibilityElementInfo(node, commonProperty, nodeInfo, webPattern);
            PushElementsIntoInfos(nodeInfo, infos, webPattern, children, allEmbedNodeTreeInfosMap);
        }
    }
}

bool JsAccessibilityManager::GetChildrenFromEmbedNode(AccessibilityElementInfo& nodeInfo, int64_t nodeId,
    std::list<int64_t>& children, std::unordered_map<int64_t, AccessibilityElementInfo>& allEmbedNodeTreeInfosMap)
{
    auto it = allEmbedNodeTreeInfosMap.find(nodeId);
    if (it == allEmbedNodeTreeInfosMap.end()) {
        return false;
    }
    nodeInfo = it->second;
    allEmbedNodeTreeInfosMap.erase(it);

    for (auto& childId : nodeInfo.GetChildIds()) {
        children.emplace_back(childId);
    }
    return true;
}

void JsAccessibilityManager::PushElementsIntoInfos(AccessibilityElementInfo& nodeInfo,
    std::list<AccessibilityElementInfo>& infos, const RefPtr<NG::WebPattern>& webPattern, std::list<int64_t>& children,
    std::unordered_map<int64_t, AccessibilityElementInfo>& allEmbedNodeTreeInfosMap)
{
    if (!IsTagInEmbedComponent(nodeInfo.GetComponentType())) {
        infos.push_back(nodeInfo);
        return;
    }
    int64_t accessibilityId = nodeInfo.GetAccessibilityId();
    CHECK_NULL_VOID(webPattern);
    std::shared_ptr<NG::TransitionalNodeInfo> node = webPattern->GetTransitionalNodeById(accessibilityId);
    CHECK_NULL_VOID(node);
    std::string surfaceId = webPattern->GetSurfaceIdByHtmlElementId(node->GetHtmlElementId());
    if (surfaceId == "") {
        infos.push_back(nodeInfo);
        return;
    }
    std::list<AccessibilityElementInfo> embedNodeTreeInfo;
    int32_t windowId = nodeInfo.GetWindowId();
    SearchSurfaceIdRet searchSurfaceIdRet =
        SearchElementInfoBySurfaceId(surfaceId, windowId, SearchSurfaceIdType::SEARCH_ALL, embedNodeTreeInfo);
    if (searchSurfaceIdRet != SearchSurfaceIdRet::SEARCH_SUCCESS) {
        infos.push_back(nodeInfo);
        return;
    }
    AccessibilityElementInfo& root = embedNodeTreeInfo.front();
    AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(webPattern->GetTreeId(), accessibilityId);
    root.SetParent(accessibilityId);
    nodeInfo.AddChild(root.GetAccessibilityId());
    infos.push_back(nodeInfo);
    children.emplace_back(root.GetAccessibilityId());
    for (const auto& info : embedNodeTreeInfo) {
        allEmbedNodeTreeInfosMap[info.GetAccessibilityId()] = info;
    }
}

int64_t JsAccessibilityManager::GetWebAccessibilityIdBySurfaceId(const std::string& surfaceId)
{
    WeakPtr<NG::WebPattern> weakWebPattern = GetWebPatternBySurfaceId(surfaceId);
    if (weakWebPattern.Invalid()) {
        TAG_LOGI(
            AceLogTag::ACE_WEB, "JsAccessibilityManager GetWebAccessibilityIdBySurfaceId weakWebPattern is Invalid");
        return INVALID_NODE_ID;
    }
    auto webPattern = weakWebPattern.Upgrade();
    CHECK_NULL_RETURN(webPattern, INVALID_NODE_ID);
    int64_t webAccessibilityId = webPattern->GetWebAccessibilityIdBySurfaceId(surfaceId);
    AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(webPattern->GetTreeId(), webAccessibilityId);
    TAG_LOGD(AceLogTag::ACE_WEB,
        "JsAccessibilityManager GetWebAccessibilityIdBySurfaceId return webAccessibilityId: %{public}" PRId64,
        webAccessibilityId);
    return webAccessibilityId;
}
#endif //WEB_SUPPORTED

bool JsAccessibilityManager::RegisterInteractionOperationAsChildTree(
    const Registration& registration)
{
    bool ret = false;
    switch (registration.operatorType) {
        case OperatorType::JS_THIRD_PROVIDER:
            ret = RegisterThirdProviderInteractionOperationAsChildTree(registration);
            break;
        default:
            TAG_LOGW(AceLogTag::ACE_ACCESSIBILITY,
                "RegisterInteractionOperationAsChildTree operatorType: %{public}d",
                static_cast<int32_t>(registration.operatorType));
    }
    return ret;
}

bool JsAccessibilityManager::RegisterThirdProviderInteractionOperationAsChildTree(
    const Registration& registration)
{
    std::shared_ptr<AccessibilitySystemAbilityClient> instance =
        AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(instance, false);
    Accessibility::Registration innerRegistration {
        .windowId = static_cast<int32_t>(registration.windowId),
        .parentWindowId = static_cast<int32_t>(registration.parentWindowId),
        .parentTreeId = registration.parentTreeId,
        .elementId = registration.elementId,
    };

    auto provider = registration.accessibilityProvider.Upgrade();
    CHECK_NULL_RETURN(provider, false);
    auto interactionOperation = std::make_shared<JsThirdProviderInteractionOperation>(
        registration.accessibilityProvider, WeakClaim(this), registration.hostNode);
    provider->SendThirdAccessibilityProvider(interactionOperation);
    interactionOperation->Initialize();
    RegisterJsThirdProviderInteractionOperation(registration.elementId,
        interactionOperation);
    Accessibility::RetError retReg = instance->RegisterElementOperator(
        innerRegistration, interactionOperation);
    TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
        "RegisterWebInteractionOperationAsChildTree result: %{public}d", retReg);
    return retReg == RET_OK;
}

bool JsAccessibilityManager::DeregisterInteractionOperationAsChildTree(
    uint32_t windowId, int32_t treeId)
{
    std::shared_ptr<AccessibilitySystemAbilityClient> instance =
        AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(instance, false);
    Accessibility::RetError retReg = instance->DeregisterElementOperator(windowId, treeId);
    return retReg == RET_OK;
}

int JsAccessibilityManager::RegisterInteractionOperation(int windowId)
{
    if (IsRegister()) {
        return 0;
    }

    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "RegisterInteractionOperation windowId: %{public}u", windowId);
    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_RETURN(instance, -1);
    auto interactionOperation = std::make_shared<JsInteractionOperation>(windowId);
    interactionOperation->SetHandler(WeakClaim(this));
    Accessibility::RetError retReg = instance->RegisterElementOperator(windowId, interactionOperation);
    RefPtr<PipelineBase> context;
    for (const auto& subContext : GetSubPipelineContexts()) {
        context = subContext.Upgrade();
        CHECK_NULL_CONTINUE(context);
        TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY, "RegisterSubPipeline windowId: %{public}u", context->GetWindowId());
        interactionOperation = std::make_shared<JsInteractionOperation>(context->GetWindowId());
        interactionOperation->SetHandler(WeakClaim(this));
        auto retResult = instance->RegisterElementOperator(context->GetWindowId(), interactionOperation);
        retReg = retResult == RET_OK ? retReg : retResult;
    }
    Register(retReg == RET_OK);
    if (retReg == RET_OK) {
        NotifyChildTreeOnRegister(treeId_);
    }

    return retReg;
}

void JsAccessibilityManager::RegisterSubWindowInteractionOperation(int windowId)
{
    if (!AceApplicationInfo::GetInstance().IsAccessibilityEnabled() || !IsRegister()) {
        return;
    }

    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_VOID(instance);
    auto interactionOperation = std::make_shared<JsInteractionOperation>(windowId);
    interactionOperation->SetHandler(WeakClaim(this));
    instance->RegisterElementOperator(windowId, interactionOperation);
}

void JsAccessibilityManager::DeregisterInteractionOperation()
{
    if (!IsRegister()) {
        return;
    }
    int windowId = static_cast<int>(GetWindowId());

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_VOID(instance);
    Register(false);
    if (currentFocusNodeId_ != -1 && lastElementId_ != -1) {
        auto focusNode = lastFrameNode_.Upgrade();
        PaintAccessibilityFocusNode(focusNode, false);
    }
    lastFrameNode_.Reset();
    lastElementId_ = -1;
    currentFocusNodeId_ = -1;
    if (parentWindowId_ == 0) {
        instance->DeregisterElementOperator(windowId);
    } else {
        instance->DeregisterElementOperator(windowId, treeId_);
        parentElementId_ = INVALID_PARENT_ID;
        parentTreeId_ = 0;
        parentWindowId_ = 0;
    }

    RefPtr<PipelineBase> context;
    for (const auto& subContext : GetSubPipelineContexts()) {
        context = subContext.Upgrade();
        CHECK_NULL_CONTINUE(context);
        instance->DeregisterElementOperator(context->GetWindowId());
    }
    NotifyChildTreeOnDeregister();

    CHECK_EQUAL_VOID(currentFocusVirtualNodeParentId_, -1);
    auto pipelineContext = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(pipelineContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipelineContext);
    CHECK_NULL_VOID(ngPipeline);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    ClearVirtualNodeAccessibilityFocus(rootNode, currentFocusVirtualNodeParentId_);
}

void JsAccessibilityManager::RegisterAccessibilityChildTreeCallback(
    int64_t elementId, const std::shared_ptr<AccessibilityChildTreeCallback> &callback)
{
    std::lock_guard<std::mutex> lock(childTreeCallbackMapMutex_);
    childTreeCallbackMap_[elementId] = callback;
}

void JsAccessibilityManager::DeregisterAccessibilityChildTreeCallback(int64_t elementId)
{
    std::lock_guard<std::mutex> lock(childTreeCallbackMapMutex_);
    childTreeCallbackMap_.erase(elementId);
}

void JsAccessibilityManager::RegisterAccessibilitySAObserverCallback(
    int64_t elementId, const std::shared_ptr<AccessibilitySAObserverCallback> &callback)
{
    std::lock_guard<std::mutex> lock(componentSACallbackMutex_);
    componentSACallbackMap_[elementId] = callback;
}

void JsAccessibilityManager::DeregisterAccessibilitySAObserverCallback(int64_t elementId)
{
    std::lock_guard<std::mutex> lock(componentSACallbackMutex_);
    componentSACallbackMap_.erase(elementId);
}

void JsAccessibilityManager::NotifyAccessibilitySAStateChange(bool state)
{
    std::lock_guard<std::mutex> lock(componentSACallbackMutex_);
    for (auto &item : componentSACallbackMap_) {
        if (item.second == nullptr) {
            continue;
        }
        item.second->OnState(state);
    }
}

void JsAccessibilityManager::NotifyChildTreeOnRegister(int32_t treeId)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "NotifyChildTreeOnRegister size: %{public}zu", childTreeCallbackMap_.size());
    std::lock_guard<std::mutex> lock(childTreeCallbackMapMutex_);
    for (auto &item : childTreeCallbackMap_) {
        if (item.second == nullptr) {
            continue;
        }
        item.second->OnRegister(GetWindowId(), treeId);
    }
}

void JsAccessibilityManager::NotifyChildTreeOnDeregister()
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "NotifyChildTreeOnDeregister size: %{public}zu",
        childTreeCallbackMap_.size());
    std::lock_guard<std::mutex> lock(childTreeCallbackMapMutex_);
    for (auto &item : childTreeCallbackMap_) {
        if (item.second == nullptr) {
            continue;
        }
        item.second->OnDeregister();
    }
}

void JsAccessibilityManager::NotifySetChildTreeIdAndWinId(
    int64_t elementId, const int32_t treeId, const int32_t childWindowId)
{
    std::lock_guard<std::mutex> lock(childTreeCallbackMapMutex_);
    auto findResult = childTreeCallbackMap_.find(elementId);
    if (findResult == childTreeCallbackMap_.end()) {
        TAG_LOGW(AceLogTag::ACE_ACCESSIBILITY, "node: %{public}" PRId64 " not found, treeId: %{public}d", elementId,
            treeId);
        return;
    }
    auto callback = findResult->second;
    CHECK_NULL_VOID(callback);
    callback->SetChildTreeId(treeId);
    callback->OnSetChildTree(childWindowId, treeId);
}

bool JsAccessibilityManager::CheckIsChildElement(
    int64_t &elementId,
    const std::vector<std::string> &params,
    std::vector<std::string> &info,
    DumpMode mode,
    int64_t &rootId)
{
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;

    if (mode == DumpMode::TREE) {
        AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(rootId, splitElementId, splitTreeId);
        if (splitTreeId <= 0 || splitTreeId == treeId_) {
            rootId = splitElementId;
            return false;
        }
    } else {
        if (elementId <= 0) {
            return false;
        }

        AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);
        if (splitTreeId <= 0 || splitTreeId == treeId_) {
            elementId = splitElementId;
            return false;
        }
    }

    std::lock_guard<std::mutex> lock(childTreeCallbackMapMutex_);
    for (const auto &item : childTreeCallbackMap_) {
        if (item.second == nullptr) {
            continue;
        }
        if (item.second->GetChildTreeId() != splitTreeId) {
            continue;
        }
        item.second->OnDumpChildInfo(params, info);
        for (const auto &childInfo : info) {
            DumpLog::GetInstance().Print(childInfo.c_str());
        }
        return true;
    }
    return false;
}

bool JsAccessibilityManager::NeedRegisterChildTree(
    uint32_t parentWindowId, int32_t parentTreeId, int64_t parentElementId)
{
    if (!IsRegister()) {
        return true;
    }

    if (parentWindowId_ == parentWindowId && parentTreeId_ == parentTreeId && parentElementId_ == parentElementId) {
        return false;
    }
    DeregisterInteractionOperationAsChildTree();
    return true;
}

void JsAccessibilityManager::RegisterInteractionOperationAsChildTree(
    uint32_t parentWindowId, int32_t parentTreeId, int64_t parentElementId)
{
    AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(parentTreeId, parentElementId);
    if (!NeedRegisterChildTree(parentWindowId, parentTreeId, parentElementId)) {
        return;
    }

    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_VOID(instance);
    auto pipelineContext = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(pipelineContext);
    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    if (container != nullptr && container->IsUIExtensionWindow()) {
        windowId_ = pipelineContext->GetRealHostWindowId();
    } else if (pipelineContext->IsFormRender()) {
        windowId_ = parentWindowId;
    }

    uint32_t windowId = GetWindowId();
    auto interactionOperation = std::make_shared<JsInteractionOperation>(windowId);
    interactionOperation->SetHandler(WeakClaim(this));
    Accessibility::Registration registration {
        .windowId = static_cast<int32_t>(windowId),
        .parentWindowId = static_cast<int32_t>(parentWindowId),
        .parentTreeId = parentTreeId,
        .elementId = parentElementId,
    };
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "windowId: %{public}u, parentWindowId: %{public}u, "
                                           "parentTreeId: %{public}d, %{public}" PRId64,
                                           windowId, parentWindowId, parentTreeId, parentElementId);
    Accessibility::RetError retReg = instance->RegisterElementOperator(registration, interactionOperation);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "RegisterElementOperator result: %{public}d", retReg);
    Register(retReg == RET_OK);
    AceApplicationInfo::GetInstance().SetAccessibilityEnabled(retReg == RET_OK);
    bool isScreenReadEnabled = false;
    instance->IsScreenReaderEnabled(isScreenReadEnabled);
    AceApplicationInfo::GetInstance().SetAccessibilityScreenReadEnabled(isScreenReadEnabled);
    parentElementId_ = parentElementId;
    parentTreeId_ = parentTreeId;
    parentWindowId_ = parentWindowId;

    for (const auto& subContext : GetSubPipelineContexts()) {
        auto context = subContext.Upgrade();
        CHECK_NULL_CONTINUE(context);
        interactionOperation = std::make_shared<JsInteractionOperation>(context->GetWindowId());
        interactionOperation->SetHandler(WeakClaim(this));
        instance->RegisterElementOperator(context->GetWindowId(), interactionOperation);
    }
}

void JsAccessibilityManager::SetAccessibilityGetParentRectHandler(std::function<void(int32_t &, int32_t &)> &&callback)
{
    getParentRectHandler_ = std::move(callback);
}

void JsAccessibilityManager::SetAccessibilityGetParentRectHandler(
    std::function<void(AccessibilityParentRectInfo &)> &&callback)
{
    getParentRectHandlerNew_ = std::move(callback);
}

void JsAccessibilityManager::DeregisterInteractionOperationAsChildTree()
{
    if (!IsRegister()) {
        return;
    }
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "deregister accessibility childTree");

    std::shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    CHECK_NULL_VOID(instance);
    uint32_t windowId = GetWindowId();
    Register(false);
    currentFocusNodeId_ = -1;
    instance->DeregisterElementOperator(windowId, treeId_);
    AceApplicationInfo::GetInstance().SetAccessibilityEnabled(false);
    AceApplicationInfo::GetInstance().SetAccessibilityScreenReadEnabled(false);
    parentElementId_ = INVALID_PARENT_ID;
    parentTreeId_ = 0;
    parentWindowId_ = 0;
    NotifyChildTreeOnDeregister();

    RefPtr<PipelineBase> context;
    for (const auto& subContext : GetSubPipelineContexts()) {
        context = subContext.Upgrade();
        CHECK_NULL_CONTINUE(context);
        instance->DeregisterElementOperator(context->GetWindowId());
    }
}

void JsAccessibilityManager::SendUecOnTreeEvent(int64_t splitElementId)
{
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    auto frameNode = GetFramenodeByAccessibilityId(rootNode, splitElementId);
    CHECK_NULL_VOID(frameNode);

    if ((!frameNode) || (IsExtensionComponent(frameNode) && !IsUIExtensionShowPlaceholder(frameNode))) {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "Register element is uec: %{public}s ", frameNode->GetTag().c_str());
        rootNode->OnAccessibilityEvent(AccessibilityEventType::CHANGE);
    } else {
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "Register element is not uec: %{public}s", frameNode->GetTag().c_str());
    }
}
void JsAccessibilityManager::JsInteractionOperation::SetChildTreeIdAndWinId(
    const int64_t nodeId, const int32_t treeId, const int32_t childWindowId)
{
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(nodeId, splitElementId, splitTreeId);
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "SetChildTreeId node: %{public}" PRId64 " treeId: %{public}d",
        splitElementId, treeId);
    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, treeId, childWindowId] {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            ACE_SCOPED_TRACE("SetChildTreeIdAndWinId");
            jsAccessibilityManager->NotifySetChildTreeIdAndWinId(splitElementId, treeId, childWindowId);
        },
        TaskExecutor::TaskType::UI, "ArkUISetChildTreeIdAndWinId");
}

void JsAccessibilityManager::JsInteractionOperation::SetBelongTreeId(const int32_t treeId)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "SetBelongTreeId treeId: %{public}d", treeId);
    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    jsAccessibilityManager->treeId_ = treeId;
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    jsAccessibilityManager->SendCacheAccessibilityEvent(context->GetInstanceId());
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), treeId] {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            ACE_SCOPED_TRACE("SetBelongTreeId");
            jsAccessibilityManager->NotifyChildTreeOnRegister(treeId);
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityClearCurrentFocus");
}

void JsAccessibilityManager::UpdateElementInfoTreeId(Accessibility::AccessibilityElementInfo& info)
{
    int32_t treeId = info.GetBelongTreeId();
    if (treeId <= 0) {
        return;
    }

    int64_t elementId = info.GetAccessibilityId();
    AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, elementId);
    info.SetAccessibilityId(elementId);

    int64_t parentId = info.GetParentNodeId();
    if ((parentId != INVALID_PARENT_ID) && NeedChangeTreeId(parentId, treeId)) {
        AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, parentId);
        info.SetParent(parentId);
    }

    UpdateElementInfoPageIdWithTreeId(info, treeId);

    std::vector<int64_t> childIds = info.GetChildIds();
    for (int64_t child : childIds) {
        if (NeedChangeTreeId(child, treeId)) {
            info.RemoveChild(child);
            AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, child);
            info.AddChild(child);
        }
    }
}

void JsAccessibilityManager::UpdateElementInfosTreeId(
    std::list<Accessibility::AccessibilityElementInfo>& infos, bool checkEmbed)
{
    int32_t rootTreeId = INVALID_NODE_ID;
    for (auto& item : infos) {
        int32_t treeId = item.GetBelongTreeId();
        if (treeId <= 0) {
            continue;
        }
        if (rootTreeId == INVALID_NODE_ID) {
            rootTreeId = treeId;
        }

        int64_t elementId = item.GetAccessibilityId();
        AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, elementId);
        item.SetAccessibilityId(elementId);

        if (checkEmbed && (rootTreeId != INVALID_NODE_ID) && (rootTreeId != treeId)) {
            continue;
        }

        int64_t parentId = item.GetParentNodeId();
        if (parentId != INVALID_PARENT_ID) {
            if (NeedChangeTreeId(parentId, treeId)) {
                AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, parentId);
                item.SetParent(parentId);
            }
        }

        UpdateElementInfoPageIdWithTreeId(item, treeId);
        if (checkEmbed && IsTagInEmbedComponent(item.GetComponentType())) {
            continue;
        }

        std::vector<int64_t> childIds = item.GetChildIds();
        for (int64_t child : childIds) {
            if (NeedChangeTreeId(child, treeId)) {
                item.RemoveChild(child);
                AccessibilitySystemAbilityClient::SetSplicElementIdTreeId(treeId, child);
                item.AddChild(child);
            }
        }
    }
}

void JsAccessibilityManager::SetPipelineContext(const RefPtr<PipelineBase>& context)
{
    context_ = context;
    if (stateObserver_[AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED] != nullptr) {
        stateObserver_[AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED]->SetPipeline(context_);
    }
}

void JsAccessibilityManager::JsAccessibilityStateObserver::OnStateChanged(const bool state)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "accessibility state changed:%{public}d, event type:%{public}u", state,
        eventType_);

    // Do not upgrade jsAccessibilityManager on async thread, destructor will cause freeze
    auto pipelineRef = pipeline_.Upgrade();
    CHECK_NULL_VOID(pipelineRef);
    pipelineRef->GetTaskExecutor()->PostTask(
        [weak = accessibilityManager_, state, eventType = eventType_]() {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);

            if (eventType == AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED) {
                auto client = AccessibilitySystemAbilityClient::GetInstance();
                if (client) {
                    std::vector<uint32_t> needEvents;
                    client->SearchNeedEvents(needEvents);
                    jsAccessibilityManager->UpdateEventWhiteList(needEvents);
                }
                auto pipelineRef = jsAccessibilityManager->GetPipelineContext().Upgrade();
                if (jsAccessibilityManager->ShouldSkipAccessibilityStateChange(pipelineRef)) {
                    return;
                }

                if (state) {
                    jsAccessibilityManager->RegisterInteractionOperation(jsAccessibilityManager->GetWindowId());
                } else {
                    jsAccessibilityManager->DeregisterInteractionOperation();
                }
                AceApplicationInfo::GetInstance().SetAccessibilityEnabled(state);
                jsAccessibilityManager->NotifyAccessibilitySAStateChange(state);
            } else if (eventType == AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED) {
                jsAccessibilityManager->isScreenReaderEnabledInitialized_ = true;
                jsAccessibilityManager->isScreenReaderEnabled_ = state;
                AceApplicationInfo::GetInstance().SetAccessibilityScreenReadEnabled(state);
            } else if (eventType == AccessibilityStateEventType::EVENT_CONFIG_EVENT_CHANGED) {
                std::vector<uint32_t> needEvents;
                auto client = AccessibilitySystemAbilityClient::GetInstance();
                CHECK_NULL_VOID(client);
                client->SearchNeedEvents(needEvents);
                jsAccessibilityManager->UpdateEventWhiteList(needEvents);
            }
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityStateChanged");
}

void JsAccessibilityManager::JsInteractionOperation::FocusMoveSearch(
    int64_t elementId, const int32_t direction, const int32_t requestId, AccessibilityElementOperatorCallback& callback)
{
    TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "move search: %{public}" PRId64 ", direction: %{public}d",
        elementId, direction);
    int64_t splitElementId = AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID;
    int32_t splitTreeId = AccessibilityElementInfo::UNDEFINED_TREE_ID;
    AccessibilitySystemAbilityClient::GetTreeIdAndElementIdBySplitElementId(elementId, splitElementId, splitTreeId);

    auto jsAccessibilityManager = GetHandler().Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto context = jsAccessibilityManager->GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    auto windowId = windowId_;
    context->GetTaskExecutor()->PostTask(
        [weak = GetHandler(), splitElementId, direction, requestId, &callback, windowId] {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            ACE_SCOPED_TRACE("FocusMoveSearch");
            jsAccessibilityManager->FocusMoveSearch(splitElementId, direction, requestId, callback, windowId);
        },
        TaskExecutor::TaskType::UI, "ArkUIAccessibilityFocusMoveSearch");
}

bool JsAccessibilityManager::ShouldSkipAccessibilityStateChange(const RefPtr<PipelineBase>& pipelineRef)
{
    CHECK_NULL_RETURN(pipelineRef, false);
    auto pipelineContext = AceType::DynamicCast<NG::PipelineContext>(pipelineRef);
    CHECK_NULL_RETURN(pipelineContext, false);
    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    if (container != nullptr && container->IsDynamicRender()) {
        return true;
    }
    if (pipelineContext->IsFormRender() || pipelineContext->IsJsCard() || pipelineContext->IsJsPlugin()) {
        return true;
    }
    if (container != nullptr && container->IsUIExtensionWindow()) {
        return true;
    }
    return false;
}

void JsAccessibilityManager::FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
    Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId)
{
    AccessibilityElementInfo nodeInfo;
    auto context = GetPipelineByWindowId(windowId);
    if (!context) {
        nodeInfo.SetValidElement(false);
        SetFocusMoveSearchResult(callback, nodeInfo, requestId);
        return;
    }

    if (AceType::InstanceOf<NG::PipelineContext>(context)) {
        FocusMoveSearchNG(elementId, direction, nodeInfo, context, NG::UI_EXTENSION_OFFSET_MAX);
        SetFocusMoveSearchResult(callback, nodeInfo, requestId);
        return;
    }

    auto node = GetAccessibilityNodeFromPage(elementId);
    if (!node) {
        nodeInfo.SetValidElement(false);
        SetFocusMoveSearchResult(callback, nodeInfo, requestId);
        return;
    }

    // get root node.
    auto rootNode = node;
    while (rootNode->GetParentNode()) {
        rootNode = rootNode->GetParentNode();
        if (!rootNode->GetParentNode()) {
            break;
        }
    }

    std::list<RefPtr<AccessibilityNode>> nodeList;
    AddFocusableNode(nodeList, rootNode);
    RefPtr<AccessibilityNode> resultNode;

    switch (direction) {
        case FocusMoveDirection::FORWARD:
        case FocusMoveDirection::BACKWARD:
            // forward and backward
            resultNode = FindNodeInRelativeDirection(nodeList, node, direction);
            break;
        case FocusMoveDirection::UP:
        case FocusMoveDirection::DOWN:
        case FocusMoveDirection::LEFT:
        case FocusMoveDirection::RIGHT:
            // up, down, left and right
            resultNode = FindNodeInAbsoluteDirection(nodeList, node, direction);
            break;
        default:
            break;
    }

    if (resultNode) {
        auto jsAccessibilityManager = Claim(this);
        UpdateAccessibilityNodeInfo(resultNode, nodeInfo, jsAccessibilityManager, windowId_);
    }

    SetFocusMoveSearchResult(callback, nodeInfo, requestId);
}

void JsAccessibilityManager::AddFocusableNode(
    std::list<RefPtr<AccessibilityNode>>& nodeList, const RefPtr<AccessibilityNode>& node)
{
    const std::string importance = node->GetImportantForAccessibility();
    if (CanAccessibilityFocused(node)) {
        nodeList.push_back(node);
    }
    if (!node->GetAccessible() && importance != "no-hide-descendants") {
        for (auto& child : node->GetChildList()) {
            AddFocusableNode(nodeList, child);
        }
    }
}

bool JsAccessibilityManager::CanAccessibilityFocused(const RefPtr<AccessibilityNode>& node)
{
    return node != nullptr && !node->IsRootNode() && node->GetVisible() &&
           node->GetImportantForAccessibility() != "no" &&
           node->GetImportantForAccessibility() != "no-hide-descendants";
}

RefPtr<AccessibilityNode> JsAccessibilityManager::FindNodeInRelativeDirection(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction)
{
    switch (direction) {
        case FocusMoveDirection::FORWARD:
            return GetNextFocusableNode(nodeList, node);
        case FocusMoveDirection::BACKWARD:
            return GetPreviousFocusableNode(nodeList, node);
        default:
            break;
    }

    return nullptr;
}

RefPtr<AccessibilityNode> JsAccessibilityManager::FindNodeInAbsoluteDirection(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction)
{
    auto tempBest = node->GetRect();
    auto nodeRect = node->GetRect();

    switch (direction) {
        case FocusMoveDirection::LEFT:
            tempBest.SetLeft(node->GetLeft() + node->GetWidth() + 1);
            break;
        case FocusMoveDirection::RIGHT:
            tempBest.SetLeft(node->GetLeft() - node->GetWidth() - 1);
            break;
        case FocusMoveDirection::UP:
            tempBest.SetTop(node->GetTop() + node->GetHeight() + 1);
            break;
        case FocusMoveDirection::DOWN:
            tempBest.SetTop(node->GetTop() - node->GetHeight() - 1);
            break;
        default:
            break;
    }

    RefPtr<AccessibilityNode> nearestNode = nullptr;
    for (auto nodeItem = nodeList.begin(); nodeItem != nodeList.end(); nodeItem++) {
        if ((*nodeItem)->GetNodeId() == node->GetNodeId() || (*nodeItem)->IsRootNode()) {
            continue;
        }
        auto itemRect = (*nodeItem)->GetRect();
        if (CheckBetterRect(nodeRect, direction, itemRect, tempBest)) {
            tempBest = itemRect;
            nearestNode = (*nodeItem);
        }
    }

    return nearestNode;
}

RefPtr<AccessibilityNode> JsAccessibilityManager::GetNextFocusableNode(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node)
{
    auto nodeItem = nodeList.begin();
    for (; nodeItem != nodeList.end(); nodeItem++) {
        if ((*nodeItem)->GetNodeId() == node->GetNodeId()) {
            break;
        }
    }

    if (nodeItem != nodeList.end() && ++nodeItem != nodeList.end()) {
        return (*nodeItem);
    }
    if (!nodeList.empty()) {
        return (*nodeList.begin());
    }

    return nullptr;
}

RefPtr<AccessibilityNode> JsAccessibilityManager::GetPreviousFocusableNode(
    const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node)
{
    auto nodeItem = nodeList.rbegin();
    for (; nodeItem != nodeList.rend(); nodeItem++) {
        if ((*nodeItem)->GetNodeId() == node->GetNodeId()) {
            break;
        }
    }

    if (nodeItem != nodeList.rend() && ++nodeItem != nodeList.rend()) {
        return (*nodeItem);
    }

    if (!nodeList.empty()) {
        return (*nodeList.rbegin());
    }
    return nullptr;
}

bool JsAccessibilityManager::RequestAccessibilityFocus(const RefPtr<AccessibilityNode>& node)
{
    auto requestNodeId = node->GetNodeId();
    if (currentFocusNodeId_ == requestNodeId) {
        return false;
    }

    ClearCurrentFocus();
    currentFocusNodeId_ = requestNodeId;
    node->SetAccessibilityFocusedState(true);
    return node->ActionAccessibilityFocus(true);
}

bool JsAccessibilityManager::ClearAccessibilityFocus(const RefPtr<AccessibilityNode>& node)
{
    auto requestNodeId = node->GetNodeId();
    if (currentFocusNodeId_ != requestNodeId) {
        return false;
    }

    currentFocusNodeId_ = -1;
    node->SetAccessibilityFocusedState(false);
    return node->ActionAccessibilityFocus(false);
}

bool JsAccessibilityManager::ClearCurrentFocus()
{
    auto currentFocusNode = GetAccessibilityNodeFromPage(currentFocusNodeId_);
    CHECK_NULL_RETURN(currentFocusNode, false);
    currentFocusNodeId_ = -1;
    currentFocusNode->SetFocusedState(false);
    currentFocusNode->SetAccessibilityFocusedState(false);
    return currentFocusNode->ActionAccessibilityFocus(false);
}

void FocusExtensionElementMoveSearchNG(const SearchParameter& searchParam,
    const RefPtr<NG::FrameNode>& node, Accessibility::AccessibilityElementInfo& info)
{
    if (NG::UI_EXTENSION_OFFSET_MIN < (searchParam.uiExtensionOffset + 1)) {
        node->FocusMoveSearchNG(searchParam.nodeId, searchParam.mode,
            searchParam.uiExtensionOffset / NG::UI_EXTENSION_ID_FACTOR, info);
    } else {
        info.SetValidElement(false);
    }
}

void GetExtensionNextFocusableNode(const AccessibilityElementInfo& focusElement,
    const std::list<AccessibilityElementInfo>& nodeList, AccessibilityElementInfo& node)
{
    auto nodeItem = nodeList.begin();
    for (; nodeItem != nodeList.end(); nodeItem++) {
        if ((*nodeItem).GetAccessibilityId() == focusElement.GetAccessibilityId()) {
            break;
        }
    }

    if (nodeItem != nodeList.end() && ++nodeItem != nodeList.end()) {
        node = (*nodeItem);
    }
    if (!nodeList.empty()) {
        node = (*nodeList.begin());
    }
}

void GetExtensionPreviousFocusableNode(const AccessibilityElementInfo& focusElement,
    const std::list<AccessibilityElementInfo>& nodeList, AccessibilityElementInfo& node)
{
    auto nodeItem = nodeList.rbegin();
    for (; nodeItem != nodeList.rend(); nodeItem++) {
        if ((*nodeItem).GetAccessibilityId() == focusElement.GetAccessibilityId()) {
            break;
        }
    }

    if (nodeItem != nodeList.rend() && ++nodeItem != nodeList.rend()) {
        node = (*nodeItem);
    }

    if (!nodeList.empty()) {
        node = (*nodeList.rbegin());
    }
}

void FindExtensionNodeInAbsoluteDirection(const AccessibilityElementInfo& focusElement,
    const std::list<AccessibilityElementInfo>& nodeList, AccessibilityElementInfo& node, int32_t direction)
{
    auto left = focusElement.GetRectInScreen().GetLeftTopXScreenPostion();
    auto top = focusElement.GetRectInScreen().GetLeftTopYScreenPostion();
    auto width = focusElement.GetRectInScreen().GetRightBottomXScreenPostion() -
        focusElement.GetRectInScreen().GetLeftTopXScreenPostion();
    auto height = focusElement.GetRectInScreen().GetRightBottomYScreenPostion() -
        focusElement.GetRectInScreen().GetLeftTopYScreenPostion();
    Rect rect(left, top, width, height);
    Rect tempBest(left, top, width, height);
    auto nodeRect = tempBest;
    switch (direction) {
        case FocusMoveDirection::LEFT:
            tempBest.SetLeft(left + width + 1);
            break;
        case FocusMoveDirection::RIGHT:
            tempBest.SetLeft(left - width - 1);
            break;
        case FocusMoveDirection::UP:
            tempBest.SetTop(top + height + 1);
            break;
        case FocusMoveDirection::DOWN:
            tempBest.SetTop(top - height - 1);
            break;
        default:
            break;
    }
    for (const auto& nodeItem : nodeList) {
        if (nodeItem.GetAccessibilityId() == focusElement.GetAccessibilityId() ||
            V2::ROOT_ETS_TAG == nodeItem.GetComponentType()) {
            continue;
        }
        left = nodeItem.GetRectInScreen().GetLeftTopXScreenPostion();
        top = nodeItem.GetRectInScreen().GetLeftTopYScreenPostion();
        width = nodeItem.GetRectInScreen().GetRightBottomXScreenPostion() -
            nodeItem.GetRectInScreen().GetLeftTopXScreenPostion();
        height = nodeItem.GetRectInScreen().GetRightBottomYScreenPostion() -
            nodeItem.GetRectInScreen().GetLeftTopYScreenPostion();
        Rect itemRect(left, top, width, height);
        if (CheckBetterRect(nodeRect, direction, itemRect, tempBest)) {
            tempBest = itemRect;
            node = nodeItem;
        }
    }
}

void FindExtensionNodeInRelativeDirection(const AccessibilityElementInfo& focusElement,
    const std::list<AccessibilityElementInfo>& nodeList, AccessibilityElementInfo& node, int direction)
{
    switch (direction) {
        case FocusMoveDirection::FORWARD:
            GetExtensionNextFocusableNode(focusElement, nodeList, node);
            break;
        case FocusMoveDirection::BACKWARD:
            GetExtensionPreviousFocusableNode(focusElement, nodeList, node);
            break;
        default:
            break;
    }
}

void FilterAccessibilityElementByFocusable(std::list<AccessibilityElementInfo>& elementList,
    AccessibilityElementInfo& focusElement, int64_t elementId)
{
    auto input = elementList;
    elementList.clear();
    std::set<int64_t> filterIds;
    for (auto& element : input) {
        if (filterIds.find(element.GetParentNodeId()) != filterIds.end()) {
            filterIds.insert(element.GetAccessibilityId());
            continue;
        }
        auto width = element.GetRectInScreen().GetRightBottomXScreenPostion() -
            element.GetRectInScreen().GetLeftTopXScreenPostion();
        auto height = element.GetRectInScreen().GetRightBottomYScreenPostion() -
            element.GetRectInScreen().GetLeftTopYScreenPostion();
        if (width == 0 || height == 0) {
            filterIds.insert(element.GetAccessibilityId());
            continue;
        }
        elementList.emplace_back(element);
        if (element.GetAccessibilityId() == elementId) {
            focusElement = element;
        }
    }
}

void JsAccessibilityManager::GetResultOfFocusMoveSearchNG(
    int64_t elementId, int32_t direction, AccessibilityElementInfo& info)
{
    auto pipeline = context_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    std::list<AccessibilityElementInfo> nodeList;
    SearchElementInfoByAccessibilityIdNG(NG::UI_EXTENSION_ROOT_ID,
        PREFETCH_RECURSIVE_CHILDREN, nodeList, pipeline, NG::UI_EXTENSION_OFFSET_MAX);
    AccessibilityElementInfo focusElement;
    FilterAccessibilityElementByFocusable(nodeList, focusElement, elementId);
    switch (direction) {
        case FocusMoveDirection::FORWARD:
        case FocusMoveDirection::BACKWARD:
            Framework::FindExtensionNodeInRelativeDirection(focusElement, nodeList, info, direction);
            break;
        case FocusMoveDirection::UP:
        case FocusMoveDirection::DOWN:
        case FocusMoveDirection::LEFT:
        case FocusMoveDirection::RIGHT:
            Framework::FindExtensionNodeInAbsoluteDirection(focusElement, nodeList, info, direction);
            break;
        default:
            break;
    }
}

void JsAccessibilityManager::FocusMoveSearchNG(int64_t elementId, int32_t direction,
    Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
    const int64_t uiExtensionOffset)
{
    auto mainContext = context_.Upgrade();
    CHECK_NULL_VOID(mainContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto rootNode = ngPipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    auto node = GetFramenodeByAccessibilityId(rootNode, elementId);
    info.SetValidElement(false);
    GetResultOfFocusMoveSearchNG(elementId, direction, info);
}

void JsAccessibilityManager::FocusExtensionElementMoveSearchNG(const SearchParameter& searchParam,
    Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
    const RefPtr<NG::FrameNode>& root, RefPtr<NG::FrameNode>& outputExtensionNode)
{
#ifdef WINDOW_SCENE_SUPPORTED
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtensionManager);
    auto elementIdPair =
        uiExtensionManager->UnWrapExtensionAbilityId(searchParam.uiExtensionOffset, searchParam.nodeId);
    outputExtensionNode = uiExtensionManager->GetFocusUiExtensionNode();
    CHECK_NULL_VOID(outputExtensionNode);
    SearchParameter transferSearchParam {elementIdPair.second, "",
        searchParam.mode, searchParam.uiExtensionOffset};
    OHOS::Ace::Framework::FocusExtensionElementMoveSearchNG(transferSearchParam, outputExtensionNode, info);
    AccessibilityElementInfo parentInfo;
    ConvertExtensionAccessibilityId(info, outputExtensionNode, searchParam.uiExtensionOffset, parentInfo);
#endif
}

// AccessibilitySystemAbilityClient will release callback after DeregisterElementOperator
void JsAccessibilityManager::SetSearchElementInfoByAccessibilityIdResult(AccessibilityElementOperatorCallback& callback,
    std::list<AccessibilityElementInfo>&& infos, const int32_t requestId, bool checkEmbed)
{
    if (!IsRegister()) {
        return;
    }
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), infos = std::move(infos), &callback, requestId, checkEmbed] () mutable {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "winId: %{public}d, treeId: %{public}d, reqId: %{public}d",
                jsAccessibilityManager->windowId_, jsAccessibilityManager->treeId_, requestId);
            if (!jsAccessibilityManager->IsRegister()) {
                return;
            }
            jsAccessibilityManager->UpdateElementInfosTreeId(infos, checkEmbed);
            callback.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
        }, TaskExecutor::TaskType::BACKGROUND, "ArkUIAccessibilitySetSearchElementInfoById");
}

void JsAccessibilityManager::SetSearchElementInfoByCustomIdResult(AccessibilityElementOperatorCallback& callback,
    std::list<AccessibilityElementInfo> &infos, const std::list<AccessibilityElementInfo> &treeInfos,
    const int32_t requestId)
{
    if (!IsRegister()) {
        return;
    }
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), infos = std::move(infos), treeInfos = std::move(treeInfos),
            &callback, requestId] () mutable {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY, "winId: %{public}d, treeId: %{public}d, reqId: %{public}d",
                jsAccessibilityManager->windowId_, jsAccessibilityManager->treeId_, requestId);
            if (!jsAccessibilityManager->IsRegister()) {
                return;
            }
            jsAccessibilityManager->UpdateElementInfosTreeId(infos);
            callback.SetSearchElementInfoBySpecificPropertyResult(infos, treeInfos, requestId);
        }, TaskExecutor::TaskType::BACKGROUND, "ArkUIAccessibilitySetSearchElementInfoById");
    return;
}

void JsAccessibilityManager::SetSearchElementInfoByTextResult(AccessibilityElementOperatorCallback& callback,
    std::list<AccessibilityElementInfo>&& infos, const int32_t requestId)
{
    if (!IsRegister()) {
        return;
    }
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), infos = std::move(infos), &callback, requestId] () mutable {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            if (!jsAccessibilityManager->IsRegister()) {
                return;
            }
            jsAccessibilityManager->UpdateElementInfosTreeId(infos);
            callback.SetSearchElementInfoByTextResult(infos, requestId);
        }, TaskExecutor::TaskType::BACKGROUND, "ArkUIAccessibilitySetSearchElementInfoByText");
}

void JsAccessibilityManager::SetSearchDefaultFocusByWindowIdResult(AccessibilityElementOperatorCallback& callback,
    std::list<AccessibilityElementInfo>&& infos, const int32_t requestId)
{
    if (!IsRegister()) {
        return;
    }
    auto context = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), infos = std::move(infos), &callback, requestId] () mutable {
            auto jsAccessibilityManager = weak.Upgrade();
            CHECK_NULL_VOID(jsAccessibilityManager);
            if (!jsAccessibilityManager->IsRegister()) {
                return;
            }
            jsAccessibilityManager->UpdateElementInfosTreeId(infos);
            callback.SetSearchDefaultFocusByWindowIdResult(infos, requestId);
        }, TaskExecutor::TaskType::BACKGROUND, "ArkUIAccessibilitySetSearchDefaultFocusByWindowId");
}

void JsAccessibilityManager::SetFindFocusedElementInfoResult(
    AccessibilityElementOperatorCallback& callback, AccessibilityElementInfo& info, const int32_t requestId)
{
    if (IsRegister()) {
        UpdateElementInfoTreeId(info);
        callback.SetFindFocusedElementInfoResult(info, requestId);
    }
}

void JsAccessibilityManager::SetFocusMoveSearchResult(
    AccessibilityElementOperatorCallback& callback, AccessibilityElementInfo& info, const int32_t requestId)
{
    if (IsRegister()) {
        UpdateElementInfoTreeId(info);
        callback.SetFocusMoveSearchResult(info, requestId);
    }
}

void JsAccessibilityManager::SetFocusMoveResultWithNode(
    const WeakPtr<NG::FrameNode>& hostNode, AccessibilityElementOperatorCallback& callback, const int32_t requestId)
{
    if (!IsRegister()) {
        return;
    }
    AccessibilityElementInfo nodeInfo;
    auto host = hostNode.Upgrade();
    if (host) {
        auto context = host->GetContextRefPtr();
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
        CommonProperty commonProperty;
        GenerateCommonProperty(ngPipeline, commonProperty, ngPipeline, host);
        UpdateAccessibilityElementInfo(host, commonProperty, nodeInfo, ngPipeline);
    } else {
        nodeInfo.SetValidElement(false);
    }
    TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY,
            "SetFocusMoveResultWithNode : %{public}" PRId64, nodeInfo.GetAccessibilityId());
    SetFocusMoveSearchResult(callback, nodeInfo, requestId);
}

void JsAccessibilityManager::SetExecuteActionResult(
    AccessibilityElementOperatorCallback& callback, const bool succeeded, const int32_t requestId)
{
    if (IsRegister()) {
        callback.SetExecuteActionResult(succeeded, requestId);
    }
}

std::string JsAccessibilityManager::GetPagePath()
{
    auto context = context_.Upgrade();
    CHECK_NULL_RETURN(context, "");
    auto frontend = context->GetFrontend();
    CHECK_NULL_RETURN(frontend, "");
    ContainerScope scope(context->GetInstanceId());
    return frontend->GetPagePath();
}

bool JsAccessibilityManager::IsUpdateWindowSceneInfo(const RefPtr<NG::FrameNode>& node,
    NG::WindowSceneInfo& windowSceneInfo)
{
    CHECK_NULL_RETURN(node, false);
    // update windowScene node commonProperty left, top position and get scale data
    auto parent = node->GetAncestorNodeOfFrame(false);
    if (node->GetTag() == V2::WINDOW_SCENE_ETS_TAG) {
        parent = node;
    }

    while (parent) {
        // window scene with windowboundary property, is the same as window
        if ((parent->GetTag() != V2::WINDOW_SCENE_ETS_TAG) || (!parent->IsWindowBoundary())) {
            parent = parent->GetAncestorNodeOfFrame(false);
            continue;
        }
        auto accessibilityProperty = parent->GetAccessibilityProperty<NG::AccessibilityProperty>();
        if (accessibilityProperty) {
            accessibilityProperty->GetWindowScenePosition(windowSceneInfo);
        }
        TAG_LOGD(AceLogTag::ACE_ACCESSIBILITY,
            "windowScene nodeId: %{public}" PRId64
            ", left: %{public}d, top: %{public}d, windowSceneScale: [%{public}f, %{public}f], "
            "innerWindowId:%{public}d, type: %{public}d",
            parent->GetAccessibilityId(), windowSceneInfo.left, windowSceneInfo.top,
            windowSceneInfo.scaleX, windowSceneInfo.scaleY, windowSceneInfo.innerWindowId,
            parent->GetWindowPatternType());
        return true;
    }
    return false;
}

void JsAccessibilityManager::UpdateElementInfoInnerWindowId(
    Accessibility::AccessibilityElementInfo& info, const int64_t& elementId)
{
    auto node = FindNodeFromPipeline(context_, elementId);
    if (node) {
        NG::WindowSceneInfo windowSceneInfo;
        IsUpdateWindowSceneInfo(node, windowSceneInfo);
        info.SetInnerWindowId(windowSceneInfo.innerWindowId);
    }
}

const std::string JsAccessibilityManager::GetPagePathInPageNodes(
    int32_t pageId,
    const std::vector<RefPtr<NG::FrameNode>>& pageNodes,
    const std::vector<std::string> pagePaths)
{
    if (pagePaths.empty()) {
        return GetPagePath();
    }
    if (pagePaths.size() != pageNodes.size()) {
        return GetPagePath();
    }
    for (std::size_t i = 0; i < pageNodes.size(); ++i) {
        CHECK_NULL_CONTINUE(pageNodes[i]);
        if (pageId != pageNodes[i]->GetPageId()) {
            continue;
        }
        return pagePaths[i];
    }
    return GetPagePath();
}

void JsAccessibilityManager::GetCurrentWindowPages(
    const RefPtr<NG::PipelineContext>& ngPipeline,
    std::vector<RefPtr<NG::FrameNode>>& pageNodes,
    std::vector<std::string>& pagePaths)
{
    CHECK_NULL_VOID(ngPipeline);
    auto stageManager = ngPipeline->GetStageManager();
    CHECK_NULL_VOID(stageManager);
    if (stageManager->IsSplitMode()) {
        pageNodes = stageManager->GetTopPagesWithTransition();
        pagePaths = stageManager->GetTopPagePaths();
        if (pageNodes.empty()) {
            TAG_LOGI(AceLogTag::ACE_ACCESSIBILITY, "Split mode enable, cannot get page");
        }
        return;
    }
    auto page = stageManager->GetLastPageWithTransition();
    CHECK_NULL_VOID(page);
    pageNodes.push_back(page);
    pagePaths.push_back(GetPagePath());
}

void JsAccessibilityManager::UpdateWindowInfo(AccessibilityWindowInfo& windowInfo, const RefPtr<PipelineBase>& context)
{
    CHECK_NULL_VOID(context);
    auto container = Platform::AceContainer::GetContainer(context->GetInstanceId());
    CHECK_NULL_VOID(container);
    auto singleHandTransform = container->GetSingleHandTransform();
    windowInfo.top = windowInfo.top * singleHandTransform.scaleY_ + singleHandTransform.y_;
    windowInfo.left = windowInfo.left * singleHandTransform.scaleX_ + singleHandTransform.x_;
    windowInfo.scaleX *= singleHandTransform.scaleX_;
    windowInfo.scaleY *= singleHandTransform.scaleY_;
}

AccessibilityWindowInfo JsAccessibilityManager::GenerateWindowInfo(const RefPtr<NG::FrameNode>& node,
    const RefPtr<PipelineBase>& context)
{
    AccessibilityWindowInfo windowInfo;
    NG::WindowSceneInfo windowSceneInfo;
    if (IsUpdateWindowSceneInfo(node, windowSceneInfo)) {
        windowInfo.left = windowSceneInfo.left;
        windowInfo.top = windowSceneInfo.top;
        windowInfo.scaleX = windowSceneInfo.scaleX;
        windowInfo.scaleY = windowSceneInfo.scaleY;
        windowInfo.innerWindowId = windowSceneInfo.innerWindowId;
        return windowInfo;
    }
    RefPtr<NG::PipelineContext> ngPipeline;
    if (context) {
        ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    } else {
        ngPipeline = AceType::DynamicCast<NG::PipelineContext>(GetPipelineContext().Upgrade());
    }
    CHECK_NULL_RETURN(ngPipeline, windowInfo);
    auto container = Platform::AceContainer::GetContainer(ngPipeline->GetInstanceId());
    if (container && !container->IsSubWindow()) {
        // subwindow by subpipeline, donot use getParentRectHandler when it is registered in mainpipeline
        if (getParentRectHandler_) {
            getParentRectHandler_(windowInfo.top, windowInfo.left);
            return windowInfo;
        } else if (getParentRectHandlerNew_) {
            if (IsReentrantLimit()) {
                return windowInfo;
            }
            SetReentrantLimit(true);
            AccessibilityParentRectInfo rectInfo;
            getParentRectHandlerNew_(rectInfo);
            windowInfo.top = rectInfo.top;
            windowInfo.left = rectInfo.left;
            windowInfo.scaleX = rectInfo.scaleX;
            windowInfo.scaleY = rectInfo.scaleY;
            windowInfo.rotateTransform = rectInfo.rotateTransform;
            SetReentrantLimit(false);
            return windowInfo;
        }
    }
    windowInfo.left = GetWindowLeft(ngPipeline->GetWindowId());
    windowInfo.top = GetWindowTop(ngPipeline->GetWindowId());
    if (container) {
        auto windowScale = container->GetWindowScale();
        windowInfo.scaleX = windowScale;
        windowInfo.scaleY = windowScale;
    }
    UpdateWindowInfo(windowInfo, ngPipeline);

    return windowInfo;
}

void JsAccessibilityManager::GenerateCommonProperty(const RefPtr<PipelineBase>& context, CommonProperty& output,
    const RefPtr<PipelineBase>& mainContext, const RefPtr<NG::FrameNode>& node)
{
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(context);
    CHECK_NULL_VOID(ngPipeline);
    if (!ngPipeline->IsFormRender()) {
        output.windowId = static_cast<int32_t>(ngPipeline->GetRealHostWindowId());
    } else {
        output.windowId = static_cast<int32_t>(GetWindowId());
    }

    auto windowInfo = GenerateWindowInfo(node, context);
    output.windowLeft = windowInfo.left;
    output.windowTop = windowInfo.top;
    scaleX_ = windowInfo.scaleX;
    scaleY_ = windowInfo.scaleY;
    output.scaleX = windowInfo.scaleX;
    output.scaleY = windowInfo.scaleY;
    output.innerWindowId = windowInfo.innerWindowId;
    output.rotateTransform = windowInfo.rotateTransform;
    // handle this time page info
    GetCurrentWindowPages(ngPipeline, output.pageNodes, output.pagePaths);
    if (context->GetWindowId() != mainContext->GetWindowId()) {
        output.pageNodes.clear();
        output.pagePaths.clear();
    }

    output.checkGetFunc = [weak = WeakClaim(this)](const RefPtr<NG::FrameNode>& node) -> int64_t {
        auto jsAccessibilityManager = weak.Upgrade();
        CHECK_NULL_RETURN(jsAccessibilityManager, INVALID_NODE_ID);
        return jsAccessibilityManager->CheckAndGetEmbedFrameNode(node);
    };
}

void JsAccessibilityManager::GenerateCommonPropertyForWeb(const RefPtr<PipelineBase>& context, CommonProperty& output,
    const RefPtr<PipelineBase>& mainContext, const RefPtr<NG::FrameNode>& node)
{
    GenerateCommonProperty(context, output, mainContext, node);
    auto parentRectInfo = GetTransformRectInfoRelativeToWindow(node, context);
    output.windowLeft = parentRectInfo.left;
    output.windowTop = parentRectInfo.top;
    output.rotateTransform = parentRectInfo.rotateTransform;
    output.scaleX = parentRectInfo.scaleX;
    output.scaleY = parentRectInfo.scaleY;
}

void JsAccessibilityManager::FindText(const RefPtr<NG::UINode>& node,
    std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<NG::PipelineContext>& context,
    const CommonProperty& commonProperty, const SearchParameter& searchParam)
{
    CHECK_NULL_VOID(node);
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    if (frameNode && !frameNode->IsInternal()) {
        if (frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>()->GetGroupText().find(
            searchParam.text) != std::string::npos) {
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityElementInfo(frameNode, commonProperty, nodeInfo, context);
            infos.emplace_back(nodeInfo);
        }
    }
    if (IsExtensionComponent(frameNode) && !IsUIExtensionShowPlaceholder(frameNode)) {
        auto infosByIPC = SearchElementInfosByTextNG(NG::UI_EXTENSION_ROOT_ID, searchParam.text,
            frameNode, searchParam.uiExtensionOffset / NG::UI_EXTENSION_ID_FACTOR);
        if (!infosByIPC.empty()) {
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityElementInfo(frameNode, commonProperty, nodeInfo, context);
            ConvertExtensionAccessibilityNodeId(infosByIPC, frameNode, searchParam.uiExtensionOffset, nodeInfo);
            for (auto& info : infosByIPC) {
                infos.emplace_back(info);
            }
        }
    }
    if (!node->GetChildren(true).empty()) {
        for (const auto& child : node->GetChildren(true)) {
            FindText(child, infos, context, commonProperty, searchParam);
        }
    }
}

void JsAccessibilityManager::FindTextByTextHint(const RefPtr<NG::UINode>& node,
    std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<NG::PipelineContext>& context,
    const CommonProperty& commonProperty, const SearchParameter& searchParam)
{
    CHECK_NULL_VOID(node);
    auto frameNode = AceType::DynamicCast<NG::FrameNode>(node);
    if (frameNode && !frameNode->IsInternal()) {
        std::string text = searchParam.text;
        nlohmann::json textJson = nlohmann::json::parse(text, nullptr, false);
        std::string value = "";
        if (!textJson.is_null() && !textJson.is_discarded() && textJson.contains("value")) {
            value = textJson["value"];
        }
        std::string textType = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>()->GetTextType();
        nlohmann::json textTypeJson = nlohmann::json::parse(textType, nullptr, false);
        if (!textTypeJson.is_null() && !textTypeJson.is_discarded() &&
            textTypeJson.contains("type") && textTypeJson["type"] == value) {
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityElementInfo(frameNode, commonProperty, nodeInfo, context);
            infos.emplace_back(nodeInfo);
        }
    }
    if (IsIsolatedComponent(frameNode)) {
        auto infosByIPC = SearchElementInfosByTextNG(NG::UI_EXTENSION_ROOT_ID, searchParam.text,
            frameNode, searchParam.uiExtensionOffset / NG::UI_EXTENSION_ID_FACTOR);
        if (!infosByIPC.empty()) {
            AccessibilityElementInfo nodeInfo;
            UpdateAccessibilityElementInfo(frameNode, commonProperty, nodeInfo, context);
            ConvertExtensionAccessibilityNodeId(infosByIPC, frameNode, searchParam.uiExtensionOffset, nodeInfo);
            for (auto& info : infosByIPC) {
                infos.emplace_back(info);
            }
        }
    }
    if (!node->GetChildren(true).empty()) {
        for (const auto& child : node->GetChildren(true)) {
            FindTextByTextHint(child, infos, context, commonProperty, searchParam);
        }
    }
}

void JsAccessibilityManager::CreateNodeInfoJson(const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty,
    std::unique_ptr<JsonValue>& json, int32_t childSize)
{
    CHECK_NULL_VOID(node);
    auto child = JsonUtil::Create(true);
    child->Put("childSize", childSize);
    child->Put("ID", node->GetAccessibilityId());
    child->Put("compid", node->GetInspectorId().value_or("").c_str());
    auto accessibilityProperty = node->GetAccessibilityProperty<NG::AccessibilityProperty>();
    if (accessibilityProperty) {
        child->Put("text", accessibilityProperty->GetGroupText().c_str());
        child->Put("accessibilityText", accessibilityProperty->GetAccessibilityText().c_str());
        child->Put("accessibilityGroup", accessibilityProperty->IsAccessibilityGroup());
        child->Put("accessibilityLevel", accessibilityProperty->GetAccessibilityLevel().c_str());
    }
    NG::RectF rect = node->GetTransformRectRelativeToWindow(true);
    child->Put("top", rect.Top() + commonProperty.windowTop);
    child->Put("left", rect.Left() + commonProperty.windowLeft);
    child->Put("width", rect.Width());
    child->Put("height", rect.Height());
    child->Put("visible", node->IsVisible());
    auto eventHub = node->GetOrCreateEventHub<NG::EventHub>();
    if (eventHub) {
        auto gestureEventHub = eventHub->GetGestureEventHub();
        child->Put("clickable", gestureEventHub ? gestureEventHub->IsAccessibilityClickable() : false);
        child->Put("longclickable", gestureEventHub ? gestureEventHub->IsAccessibilityLongClickable() : false);
    }
    if (accessibilityProperty) {
        child->Put("checkable", accessibilityProperty->IsCheckable());
        child->Put("scrollable", accessibilityProperty->IsScrollable());
        child->Put("checked", accessibilityProperty->IsChecked());
        child->Put("hint", accessibilityProperty->GetHintText().c_str());
        child->Put("childTree", accessibilityProperty->GetChildTreeId());
    }
    std::string tag =
        node->GetTag() == "root" ? "root" : node->GetTag() + "_" + std::to_string(node->GetAccessibilityId());
    json->Put(tag.c_str(), child);
}

void JsAccessibilityManager::DumpTreeNodeInfoInJson(
    const RefPtr<NG::FrameNode>& node, int32_t depth, const CommonProperty& commonProperty, int32_t childSize)
{
    auto json = JsonUtil::Create(true);
    CreateNodeInfoJson(node, commonProperty, json, childSize);
    std::string content = DumpLog::GetInstance().FormatDumpInfo(json->ToString(), depth);
    std::string prefix = DumpLog::GetInstance().GetPrefix(depth);
    std::string fulljson = prefix.append(content);
    DumpLog::GetInstance().PrintJson(fulljson);
}


void JsAccessibilityManager::TransferThirdProviderHoverEvent(
    const WeakPtr<NG::FrameNode>& hostNode, const NG::PointF& point, SourceType source,
    NG::AccessibilityHoverEventType eventType, TimeStamp time)
{
    auto pipelineContext = GetPipelineContext().Upgrade();
    CHECK_NULL_VOID(pipelineContext);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipelineContext);
    CHECK_NULL_VOID(ngPipeline);
    auto frameNode = hostNode.Upgrade();
    CHECK_NULL_VOID(frameNode);
    AccessibilityHoverForThirdConfig config;
    config.hostElementId = frameNode->GetAccessibilityId();
    config.point = point;
    config.sourceType = source;
    config.eventType = eventType;
    config.time = time;
    config.context = ngPipeline;
    HandleAccessibilityHoverForThird(config);
}

bool JsAccessibilityManager::OnDumpChildInfoForThird(
    int64_t hostElementId,
    const std::vector<std::string>& params,
    std::vector<std::string>& info)
{
    return OnDumpChildInfoForThirdRecursive(hostElementId, params, info, WeakClaim(this));
}

#ifdef WEB_SUPPORTED
void JsAccessibilityManager::GetWebCursorPosition(const int64_t elementId, const int32_t requestId,
    AccessibilityElementOperatorCallback& callback, const RefPtr<NG::WebPattern>& webPattern)
{
    CHECK_NULL_VOID(webPattern);
    auto node = webPattern->GetTransitionalNodeById(elementId);
    CHECK_NULL_VOID(node);

    callback.SetCursorPositionResult(node->GetSelectionStart(), requestId);
}
#endif // WEB_SUPPORTED

void JsAccessibilityManager::FireAccessibilityEventCallback(uint32_t eventId, int64_t parameter)
{
    auto eventType = static_cast<AccessibilityCallbackEventId>(eventId);
    AccessibilityEvent event;
    switch (eventType) {
        case AccessibilityCallbackEventId::ON_LOAD_PAGE:
            event.nodeId = parameter;
            event.windowChangeTypes = WindowUpdateType::WINDOW_UPDATE_ACTIVE;
            event.type = AccessibilityEventType::CHANGE;
            SendAccessibilityAsyncEvent(event);
            break;
        default:
            break;
    }
}

int32_t JsAccessibilityManager::GetTransformDegreeRelativeToWindow(const RefPtr<NG::FrameNode>& node, bool excludeSelf)
{
    int32_t rotateDegree = 0;
    auto context = node->GetRenderContext();
    if (context && !excludeSelf) {
        rotateDegree = context->GetRotateDegree();
    }
    auto parent = node->GetAncestorNodeOfFrame(true);
    while (parent) {
        if (parent->IsWindowBoundary()) {
            break;
        }
        auto contextParent = parent->GetRenderContext();
        if (contextParent) {
            rotateDegree += contextParent->GetRotateDegree();
        }
        parent = parent->GetAncestorNodeOfFrame(true);
    }
    return ((rotateDegree % FULL_ANGLE + FULL_ANGLE) % FULL_ANGLE);
}

AccessibilityParentRectInfo JsAccessibilityManager::GetTransformRectInfoRelativeToWindow(
    const RefPtr<NG::FrameNode>& node, const RefPtr<PipelineBase>& context)
{
    AccessibilityParentRectInfo rectInfo;
    CHECK_NULL_RETURN(node, rectInfo);
    auto finalScale = node->GetTransformScaleRelativeToWindow();
    rectInfo.scaleX = finalScale.x;
    rectInfo.scaleY = finalScale.y;
    CHECK_NULL_RETURN(context, rectInfo);
    auto windowInfo = GenerateWindowInfo(node, context);
    auto rectFinal = GetFinalRealRect(node);
    RotateTransform rotateData;
    RotateTransform windowRotateData = windowInfo.rotateTransform;
    rotateData.rotateDegree = GetTransformDegreeRelativeToWindow(node);
    AccessibilityRect rotateRect(rectFinal.GetX(), rectFinal.GetY(),
        rectFinal.Width(), rectFinal.Height());
    if (windowRotateData.rotateDegree) {
        rotateRect.Rotate(windowRotateData.innerCenterX, windowRotateData.innerCenterY,
            windowRotateData.rotateDegree);
        rotateRect.ApplyTransformation(windowRotateData, windowInfo.scaleX, windowInfo.scaleY);
        rotateData.rotateDegree += windowRotateData.rotateDegree;
    } else {
        RotateTransform roateDataTemp(0, windowInfo.left, windowInfo.top, 0, 0);
        rotateRect.ApplyTransformation(roateDataTemp, windowInfo.scaleX, windowInfo.scaleY);
    }
    rectInfo.left = rotateRect.GetX();
    rectInfo.top = rotateRect.GetY();
    rectInfo.scaleX *= windowInfo.scaleX;
    rectInfo.scaleY *= windowInfo.scaleY;
    if (rotateData.rotateDegree) {
        rotateData.centerX = static_cast<int32_t>(rotateRect.GetWidth()) * 0.5f + rotateRect.GetX();
        rotateData.centerY = static_cast<int32_t>(rotateRect.GetHeight()) * 0.5f + rotateRect.GetY();
        auto renderContext = node->GetRenderContext();
        CHECK_NULL_RETURN(renderContext, rectInfo);
        auto rectOrigin = renderContext->GetPaintRectWithoutTransform();
        rotateData.innerCenterX = rectOrigin.Width() * 0.5f;
        rotateData.innerCenterY = rectOrigin.Height() * 0.5f;
    }
    rectInfo.rotateTransform = rotateData;
    return rectInfo;
}

void JsAccessibilityManager::UpdateAccessibilityNodeRect(const RefPtr<NG::FrameNode>& frameNode)
{
    auto accessibilityWorkMode = GenerateAccessibilityWorkMode();
    if (!accessibilityWorkMode.isTouchExplorationEnabled) {
        return;
    }
    CHECK_NULL_VOID(frameNode);
    auto renderContext = frameNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    auto isFocus = accessibilityProperty->GetAccessibilityFocusState();
    if (isFocus && !frameNode->IsAccessibilityVirtualNode() && !frameNode->IsDrawFocusOnTop()) {
        if (accessibilityProperty->IsMatchAccessibilityResponseRegion(false)) {
            auto rectInt = accessibilityProperty->GetAccessibilityResponseRegionRect(false);
            renderContext->UpdateAccessibilityFocusRect(rectInt);
        } else {
            renderContext->UpdateAccessibilityRoundRect();
        }
    } else {
        RefPtr<NG::FrameNode> focusFrameNode = nullptr;
        auto checkflag = CheckChildIsAccessibilityFocus(frameNode, focusFrameNode);
        if (checkflag && focusFrameNode && focusFrameNode->IsDrawFocusOnTop()) {
            UpdatePaintNodeRender(focusFrameNode);
        }
    }
}

void JsAccessibilityManager::OnAccessbibilityDetachFromMainTree(const RefPtr<NG::FrameNode>& focusNode)
{
    auto paintNode = focusNode->GetPaintNode();
    CHECK_NULL_VOID(paintNode);
    auto paintNodePattern = AceType::DynamicCast<NG::AccessibilityFocusPaintNodePattern>(paintNode->GetPattern());
    CHECK_NULL_VOID(paintNodePattern);
    auto currentFocusNode = paintNodePattern->GetFocusNode().Upgrade();
    CHECK_NULL_VOID(currentFocusNode);
    if (currentFocusNode->GetId() == focusNode->GetId()) {
        paintNodePattern->OnDetachFromFocusNode();
        RemoveAccessibilityFocusPaint(focusNode);
    }
}

bool JsAccessibilityManager::CheckAccessibilityVisible(const RefPtr<NG::FrameNode>& node)
{
    UpdateAccessibilityVisibleToRoot(node);
    CHECK_NULL_RETURN(node, true);
    return node->GetAccessibilityVisible();
}

bool JsAccessibilityManager::IsScreenReaderEnabled()
{
    if (!isScreenReaderEnabledInitialized_) {
        auto client = AccessibilitySystemAbilityClient::GetInstance();
        if (client) {
            client->IsScreenReaderEnabled(isScreenReaderEnabled_);
            isScreenReaderEnabledInitialized_ = true;
        }
    }
    return isScreenReaderEnabled_;
}

SearchSurfaceIdRet JsAccessibilityManager::SearchElementInfoBySurfaceId(
    const std::string& surfaceId, const int32_t windowId,
    const SearchSurfaceIdType searchType, std::list<AccessibilityElementInfo>& infos)
{
    auto pipeline = GetPipelineByWindowId(windowId);
    CHECK_NULL_RETURN(pipeline, SearchSurfaceIdRet::NO_MATCH_NODE);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    CHECK_NULL_RETURN(ngPipeline, SearchSurfaceIdRet::NO_MATCH_NODE);

    auto node = GetEmbedNodeBySurfaceId(surfaceId).Upgrade();
    CHECK_NULL_RETURN(node, SearchSurfaceIdRet::NO_MATCH_NODE);

    if (searchType != SearchSurfaceIdType::SEARCH_TAIL) {
        auto mode = searchType == SearchSurfaceIdType::SEARCH_ALL ?
            static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN) : 0;
        SearchElementInfoByAccessibilityIdNG(
            node->GetAccessibilityId(), mode, infos, pipeline, NG::UI_EXTENSION_OFFSET_MAX);
        UpdateElementInfosTreeId(infos);
        return SearchSurfaceIdRet::SEARCH_SUCCESS;
    }

    auto mainContext = context_.Upgrade();
    CHECK_NULL_RETURN(mainContext, SearchSurfaceIdRet::NO_MATCH_NODE);
    CommonProperty commonProperty;
    GenerateCommonProperty(pipeline, commonProperty, mainContext, node);
    UpdateAccessibilityVisibleToRoot(node);
    commonProperty.checkEmbedNode = false;
    auto lastNode = GetLastChildFrameNode(node, commonProperty);
    CHECK_NULL_RETURN(lastNode, SearchSurfaceIdRet::NO_MATCH_NODE);

    AccessibilityElementInfo elementInfo;
    UpdateAccessibilityElementInfo(lastNode, commonProperty, elementInfo, ngPipeline);
    SetRootAccessibilityVisible(lastNode, elementInfo);
    SetRootAccessibilityNextFocusId(lastNode, node, elementInfo);
    SetRootAccessibilityPreFocusId(lastNode, node, elementInfo, nextFocusMapWithSubWindow_[pipeline->GetInstanceId()]);
    infos.push_back(elementInfo);
    UpdateElementInfosTreeId(infos);
    return SearchSurfaceIdRet::SEARCH_SUCCESS;
}

WeakPtr<NG::WebPattern> JsAccessibilityManager::GetWebPatternBySurfaceId(const std::string& surfaceId)
{
    std::lock_guard<std::mutex> lock(webPatternMapMutex_);
    auto it = webPatternMap_.find(surfaceId);
    if (it != webPatternMap_.end()) {
        return it->second;
    }
    return WeakPtr<NG::WebPattern>();
}

void JsAccessibilityManager::SetWebPatternBySurfaceId(const std::string& surfaceId, WeakPtr<NG::WebPattern> pattern)
{
    std::lock_guard<std::mutex> lock(webPatternMapMutex_);
    webPatternMap_[surfaceId] = pattern;
}

void JsAccessibilityManager::RemoveWebPatternBySurfaceId(const std::string& surfaceId)
{
    if (!surfaceId.empty()) {
        std::lock_guard<std::mutex> lock(webPatternMapMutex_);
        auto it = webPatternMap_.find(surfaceId);
        if (it != webPatternMap_.end()) {
            webPatternMap_.erase(it);
        }
    }
}

bool JsAccessibilityManager::CheckWhiteList(const uint32_t& eventType)
{
    auto whiteList = eventWhiteList_;
    if (whiteList.size() == 0) {
        return true;
    }

    if (whiteList.size() == 1) {
        if (whiteList[0] == Accessibility::EventType::TYPE_VIEW_INVALID) {
            return false;
        } else if (whiteList[0] == Accessibility::EventType::TYPES_ALL_MASK) {
            return true;
        }
    }
    return std::find(whiteList.begin(), whiteList.end(), eventType) != whiteList.end();
}

void JsAccessibilityManager::SearchAccessibilityNodeBySpecificProperty(const int64_t elementId,
    const SpecificPropertyParam &param, const int32_t requestId,
    AccessibilityElementOperatorCallback &callback, const int32_t windowId)
{
    std::list<AccessibilityElementInfo> infos;
    std::list<AccessibilityElementInfo> treeInfos;

    if (param.propertyType != SEARCH_TYPE::CUSTOMID) {
        SetSearchElementInfoByCustomIdResult(callback, infos, treeInfos, requestId);
        return;
    }

    auto pipeline = GetPipelineByWindowId(windowId);
    auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    if (!ngPipeline) {
        SetSearchElementInfoByCustomIdResult(callback, infos, treeInfos, requestId);
        return;
    }
    auto rootNode = ngPipeline->GetRootElement();

    int64_t nodeId = elementId;
    if (elementId == -1) {
        nodeId = rootNode->GetAccessibilityId();
    }
    auto weak = WeakClaim(this);
    auto jsAccessibilityManager = weak.Upgrade();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage(nodeId);
    if (!node) {
        SetSearchElementInfoByCustomIdResult(callback, infos, treeInfos, requestId);
        return;
    }
    std::list<AccessibilityElementInfo> nodeInfos;
    AccessibilityElementInfo nodeInfo;
    UpdateAccessibilityNodeInfo(node, nodeInfo, jsAccessibilityManager, jsAccessibilityManager->windowId_);
    nodeInfos.push_back(nodeInfo);
    // cache parent/siblings/children infos
    UpdateCacheInfo(
        infos,
        static_cast<uint32_t>(PREFETCH_RECURSIVE_CHILDREN),
        node,
        jsAccessibilityManager,
        jsAccessibilityManager->windowId_);

    for (auto& nodeInfo : nodeInfos) {
        if (nodeInfo.GetInspectorKey() == param.propertyTarget) {
            infos.emplace_back(nodeInfo);
            break;
        }
    }
    SetSearchElementInfoByCustomIdResult(callback, infos, treeInfos, requestId);
    return;
}
} // namespace OHOS::Ace::Framework

/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_ACCESSIBILITY_JS_ACCESSIBILITY_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_ACCESSIBILITY_JS_ACCESSIBILITY_MANAGER_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "accessibility_config.h"
#include "accessibility_element_operator.h"
#include "accessibility_event_info.h"
#include "accessibility_state_event.h"

#include "core/accessibility/accessibility_manager.h"
#include "core/accessibility/accessibility_utils.h"
#include "frameworks/bridge/common/accessibility/accessibility_node_manager.h"

#include "js_third_accessibility_hover_ng.h"

namespace OHOS::NWeb {
class NWebAccessibilityNodeInfo;
} // namespace OHOS::NWeb::NWebAccessibilityNodeInfo

namespace OHOS::Ace::NG {
    class TransitionalNodeInfo;
}

namespace OHOS::Ace::Framework {

struct SearchParameter {
    int64_t nodeId;
    std::string text;
    int32_t mode;
    int64_t uiExtensionOffset;
};

struct CommonProperty {
    int32_t innerWindowId = -1;
    int32_t windowId = 0;
    int32_t windowLeft = 0;
    int32_t windowTop = 0;
    int32_t pageId = 0;
    std::string pagePath;
};

struct ActionTable {
    AceAction aceAction;
    ActionType action;
};

struct ActionStrTable {
    ActionType action;
    std::string actionStr;
};

struct FillEventInfoParam {
    int64_t elementId = 0;
    int64_t stackNodeId = 0;
    uint32_t windowId = 0;
};

struct AccessibilityActionParam {
    RefPtr<NG::AccessibilityProperty> accessibilityProperty;
    std::string setTextArgument = "";
    int32_t setSelectionStart = -1;
    int32_t setSelectionEnd = -1;
    bool setSelectionDir = false;
    int32_t setCursorIndex = -1;
    TextMoveUnit moveUnit = TextMoveUnit::STEP_CHARACTER;
    AccessibilityScrollType scrollType = AccessibilityScrollType::SCROLL_DEFAULT;
    int32_t spanId = -1;
};

struct ActionParam {
    Accessibility::ActionType action;
    std::map<std::string, std::string> actionArguments;
};

enum class DumpMode {
    TREE,
    NODE,
    HANDLE_EVENT,
    HOVER_TEST
};

class JsAccessibilityManager : public AccessibilityNodeManager,
    public AccessibilityHoverManagerForThirdNG {
    DECLARE_ACE_TYPE(JsAccessibilityManager, AccessibilityNodeManager);

public:
    JsAccessibilityManager() = default;
    ~JsAccessibilityManager() override;

    // JsAccessibilityManager overrides functions.
    void InitializeCallback() override;
    void SendAccessibilityAsyncEvent(const AccessibilityEvent& accessibilityEvent) override;
    void UpdateVirtualNodeFocus() override;
    void SetCardViewParams(const std::string& key, bool focus) override;
    void HandleComponentPostBinding() override;
    void RegisterSubWindowInteractionOperation(int windowId) override;
    void SetPipelineContext(const RefPtr<PipelineBase>& context) override;
    void UpdateElementInfosTreeId(std::list<Accessibility::AccessibilityElementInfo>& infos);
    void UpdateElementInfoTreeId(Accessibility::AccessibilityElementInfo& info);

    void UpdateViewScale();

    float GetScaleX() const
    {
        return scaleX_;
    }

    float GetScaleY() const
    {
        return scaleY_;
    }

    uint32_t GetWindowId() const
    {
        return windowId_;
    }

    void SetWindowId(uint32_t windowId)
    {
        windowId_ = windowId;
    }

    void SaveLast(const int64_t elementId, const RefPtr<NG::FrameNode>& node)
    {
        lastElementId_ = elementId;
        lastFrameNode_ = node;
    }

    void SaveCurrentFocusNodeSize(const RefPtr<NG::FrameNode>& currentFocusNode)
    {
        if (currentFocusNode->IsAccessibilityVirtualNode()) {
            auto oldGeometryNode = currentFocusNode->GetGeometryNode();
            CHECK_NULL_VOID(oldGeometryNode);
            oldGeometrySize_ = oldGeometryNode->GetFrameSize();
        }
    }

    bool SubscribeToastObserver();
    bool UnsubscribeToastObserver();
    bool SubscribeStateObserver(int eventType);
    bool UnsubscribeStateObserver(int eventType);
    int RegisterInteractionOperation(int windowId);
    void DeregisterInteractionOperation();
    bool SendAccessibilitySyncEvent(
        const AccessibilityEvent& accessibilityEvent, Accessibility::AccessibilityEventInfo eventInfo);
    bool TransferAccessibilityAsyncEvent(
        const Accessibility::AccessibilityEventInfo& eventInfo,
        int64_t uiExtensionOffset) override;
    void SendExtensionAccessibilityEvent(
        const Accessibility::AccessibilityEventInfo& eventInfo,
        int64_t uiExtensionOffset) override;
    void SearchElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t mode, const int32_t windowId);
    void SearchElementInfosByText(const int64_t elementId, const std::string& text, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId);
    void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId);
    void FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId);
    bool IsUpdateWindowSceneInfo(const RefPtr<NG::FrameNode>& node, NG::WindowSceneInfo& windowSceneInfo);
    void UpdateElementInfoInnerWindowId(Accessibility::AccessibilityElementInfo& info, const int64_t& elementId);
    void ExecuteAction(const int64_t accessibilityId, const ActionParam& param, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId);
    bool ClearCurrentFocus();
    void UpdateNodeChildIds(const RefPtr<AccessibilityNode>& node);
    void SendActionEvent(const Accessibility::ActionType& action, int64_t nodeId);
    void SearchElementInfoByAccessibilityIdNG(int64_t elementId, int32_t mode,
        std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
        const int64_t uiExtensionOffset = 0) override;
    void SearchElementInfosByTextNG(int64_t elementId, const std::string& text,
        std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
        const int64_t uiExtensionOffset = 0) override;
    void FindFocusedElementInfoNG(int64_t elementId, int32_t focusType, Accessibility::AccessibilityElementInfo& info,
        const RefPtr<PipelineBase>& context, const int64_t uiExtensionOffset = 0) override;
    void FocusMoveSearchNG(int64_t elementId, int32_t direction, Accessibility::AccessibilityElementInfo& info,
        const RefPtr<PipelineBase>& context, const int64_t uiExtensionOffset = 0) override;
    bool ExecuteExtensionActionNG(int64_t elementId, const std::map<std::string, std::string>& actionArguments,
        int32_t action, const RefPtr<PipelineBase>& context, int64_t uiExtensionOffset) override;
#ifdef WEB_SUPPORTED
    void SendWebAccessibilityAsyncEvent(const AccessibilityEvent& accessibilityEvent,
        const RefPtr<NG::WebPattern>& webPattern) override;
    void SearchWebElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t mode, const int32_t windowId,
        const RefPtr<NG::WebPattern>& webPattern);
    void SearchWebElementInfoByAccessibilityIdNG(int64_t elementId, int32_t mode,
        std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
        const RefPtr<NG::WebPattern>& webPattern);
    void FindWebFocusedElementInfo(const int64_t elementId, const int32_t focusType,
        const int32_t requestId, Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId,
        const RefPtr<NG::WebPattern>& webPattern);
    void FindWebFocusedElementInfoNG(int64_t elementId, int32_t focusType,
        Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
        const RefPtr<NG::WebPattern>& webPattern);
    void WebFocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId,
        const RefPtr<NG::WebPattern>& webPattern);
    void WebFocusMoveSearchNG(int64_t elementId, int32_t direction,
        Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
        const RefPtr<NG::WebPattern>& webPattern);
    void ExecuteWebAction(const int64_t elementId, const ActionParam& param, const int32_t requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t windowId,
        const RefPtr<NG::WebPattern>& webPattern);
    bool ExecuteWebActionNG(int64_t elementId, Accessibility::ActionType action,
        const std::map<std::string, std::string>& actionArguments, const RefPtr<NG::WebPattern>& webPattern);

    bool DeregisterWebInteractionOperationAsChildTree(int32_t treeID) override;
    bool RegisterWebInteractionOperationAsChildTree(int64_t accessibilityId,
        const WeakPtr<NG::WebPattern>& webPattern) override;
    void GetWebCursorPosition(const int64_t elementId, const int32_t requestId,
        AccessibilityElementOperatorCallback& callback, const RefPtr<NG::WebPattern>& webPattern);
#endif //WEB_SUPPORTED
    void GetResultOfFocusMoveSearchNG(
        int64_t elementId, int32_t direction, Accessibility::AccessibilityElementInfo& info);

    std::string GetPagePath();

    void RegisterAccessibilityChildTreeCallback(
        int64_t elementId, const std::shared_ptr<AccessibilityChildTreeCallback> &callback) override;

    void DeregisterAccessibilityChildTreeCallback(int64_t elementId) override;

    void RegisterAccessibilitySAObserverCallback(
        int64_t elementId, const std::shared_ptr<AccessibilitySAObserverCallback> &callback) override;

    void DeregisterAccessibilitySAObserverCallback(int64_t elementId) override;

    void RegisterInteractionOperationAsChildTree(uint32_t parentWindowId, int32_t parentTreeId,
        int64_t parentElementId) override;
    void SetAccessibilityGetParentRectHandler(std::function<void(int32_t &, int32_t &)> &&callback) override;
    void SetAccessibilityGetParentRectHandler(
        std::function<void(AccessibilityParentRectInfo &)> &&callback) override;
    void DeregisterInteractionOperationAsChildTree() override;
    void SendEventToAccessibilityWithNode(const AccessibilityEvent& accessibilityEvent,
        const RefPtr<AceType>& node, const RefPtr<PipelineBase>& context) override;

    bool RegisterInteractionOperationAsChildTree(const Registration& registration) override;
    bool DeregisterInteractionOperationAsChildTree(uint32_t windowId, int32_t treeId) override;

    void TransferThirdProviderHoverEvent(
        const WeakPtr<NG::FrameNode>& hostNode, const NG::PointF& point, SourceType source,
        NG::AccessibilityHoverEventType eventType, TimeStamp time) override;

    void DumpAccessibilityPropertyNG(const AccessibilityElementInfo& nodeInfo);
    void DumpCommonPropertyNG(const AccessibilityElementInfo& nodeInfo, int32_t treeId);
    bool OnDumpChildInfoForThird(
        int64_t hostElementId,
        const std::vector<std::string>& params,
        std::vector<std::string>& info) override;
    void ProcessParameters(
        ActionType op, const std::vector<std::string>& params, std::map<std::string, std::string>& paramsMap);
    bool CheckDumpHandleEventParams(const std::vector<std::string> &params);
    bool CheckGetActionIdAndOp(
        const std::vector<std::string>& params,
        int64_t& actionAccessibilityId,
        ActionType& actionOp);

    void FireAccessibilityEventCallback(uint32_t eventId, int64_t parameter) override;

protected:
    void OnDumpInfoNG(const std::vector<std::string>& params, uint32_t windowId, bool hasJson = false) override;
    void DumpHandleEvent(const std::vector<std::string>& params) override;
    void DumpProperty(const std::vector<std::string>& params) override;
    void DumpTree(int32_t depth, int64_t nodeID, bool isDumpSimplify = false) override;

private:
    static constexpr int32_t INVALID_PARENT_ID = -2100000;

    class JsInteractionOperation : public Accessibility::AccessibilityElementOperator {
    public:
        explicit JsInteractionOperation(int32_t windowId) : windowId_(windowId) {}
        virtual ~JsInteractionOperation() = default;
        // Accessibility override.
        void SearchElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t mode) override;
        void SearchElementInfosByText(const int64_t elementId, const std::string& text, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void ExecuteAction(const int64_t elementId, const int32_t action,
            const std::map<std::string, std::string>& actionArguments, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void ClearFocus() override;
        void OutsideTouch() override;
        void GetCursorPosition(const int64_t elementId, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback &callback) override;
        void SetChildTreeIdAndWinId(const int64_t nodeId, const int32_t treeId, const int32_t childWindowId) override;
        void SetBelongTreeId(const int32_t treeId) override;

        void SetHandler(const WeakPtr<JsAccessibilityManager>& js)
        {
            js_ = js;
        }

        const WeakPtr<JsAccessibilityManager>& GetHandler() const
        {
            return js_;
        }

    private:
        WeakPtr<JsAccessibilityManager> js_;
        uint32_t windowId_ = 0;
    };
#ifdef WEB_SUPPORTED

    class WebInteractionOperation : public Accessibility::AccessibilityElementOperator {
    public:
        explicit WebInteractionOperation(int32_t windowId) : windowId_(windowId) {}
        virtual ~WebInteractionOperation() = default;
        // Accessibility override.
        void SearchElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback, const int32_t mode) override;
        void SearchElementInfosByText(const int64_t elementId, const std::string& text, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void ExecuteAction(const int64_t elementId, const int32_t action,
            const std::map<std::string, std::string>& actionArguments, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void ClearFocus() override;
        void OutsideTouch() override;
        void GetCursorPosition(const int64_t elementId, const int32_t requestId,
            Accessibility::AccessibilityElementOperatorCallback &callback) override;
        void SetChildTreeIdAndWinId(const int64_t nodeId, const int32_t treeId, const int32_t childWindowId) override;
        void SetBelongTreeId(const int32_t treeId) override;

        void SetHandler(const WeakPtr<JsAccessibilityManager>& js)
        {
            js_ = js;
        }

        const WeakPtr<JsAccessibilityManager>& GetHandler() const
        {
            return js_;
        }

        void SetWebPattern(const WeakPtr<NG::WebPattern>& webPattern)
        {
            webPattern_ = webPattern;
        }

        const WeakPtr<NG::WebPattern>& GetWebPattern() const
        {
            return webPattern_;
        }

    private:
        WeakPtr<JsAccessibilityManager> js_;
        uint32_t windowId_ = 0;
        WeakPtr<NG::WebPattern> webPattern_;
    };
#endif // WEB_SUPPORTED

    class ToastAccessibilityConfigObserver : public AccessibilityConfig::AccessibilityConfigObserver {
    public:
        ToastAccessibilityConfigObserver() = default;
        void OnConfigChanged(
            const AccessibilityConfig::CONFIG_ID id, const AccessibilityConfig::ConfigValue& value) override;
    };

    class JsAccessibilityStateObserver : public Accessibility::AccessibilityStateObserver {
    public:
        void OnStateChanged(const bool state) override;
        void SetAccessibilityManager(const WeakPtr<JsAccessibilityManager>& accessibilityManager)
        {
            accessibilityManager_ = accessibilityManager;
        }

        void SetPipeline(const WeakPtr<PipelineBase>& pipeline)
        {
            pipeline_ = pipeline;
        }

    private:
        // Do not upgrade accessibilityManager_ on async thread, destructor might cause freeze
        WeakPtr<JsAccessibilityManager> accessibilityManager_;
        WeakPtr<PipelineBase> pipeline_;
    };

    bool AccessibilityActionEvent(const Accessibility::ActionType& action,
        const std::map<std::string, std::string>& actionArguments, const RefPtr<AccessibilityNode>& node,
        const RefPtr<PipelineContext>& context);
    bool RequestAccessibilityFocus(const RefPtr<AccessibilityNode>& node);

    bool ClearAccessibilityFocus(const RefPtr<AccessibilityNode>& node);

    void AddFocusableNode(std::list<RefPtr<AccessibilityNode>>& nodeList, const RefPtr<AccessibilityNode>& node);
    bool CanAccessibilityFocused(const RefPtr<AccessibilityNode>& node);
    RefPtr<AccessibilityNode> FindNodeInRelativeDirection(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction);
    RefPtr<AccessibilityNode> FindNodeInAbsoluteDirection(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction);
    RefPtr<AccessibilityNode> GetNextFocusableNode(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node);
    RefPtr<AccessibilityNode> GetPreviousFocusableNode(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node);

    bool ExecuteActionNG(int64_t elementId, const std::map<std::string, std::string>& actionArguments,
        Accessibility::ActionType action, const RefPtr<PipelineBase>& context, int64_t uiExtensionOffset);
    bool ConvertActionTypeToBoolen(Accessibility::ActionType action, RefPtr<NG::FrameNode>& frameNode,
        int64_t elementId, RefPtr<NG::PipelineContext>& context);
    void SetSearchElementInfoByAccessibilityIdResult(Accessibility::AccessibilityElementOperatorCallback& callback,
        std::list<Accessibility::AccessibilityElementInfo>&& infos, const int32_t requestId);

    void SetSearchElementInfoByTextResult(Accessibility::AccessibilityElementOperatorCallback& callback,
        std::list<Accessibility::AccessibilityElementInfo>&& infos, const int32_t requestId);

    void SetFindFocusedElementInfoResult(Accessibility::AccessibilityElementOperatorCallback& callback,
        Accessibility::AccessibilityElementInfo& info, const int32_t requestId);

    void SetFocusMoveSearchResult(Accessibility::AccessibilityElementOperatorCallback& callback,
        Accessibility::AccessibilityElementInfo& info, const int32_t requestId);

    void SetExecuteActionResult(
        Accessibility::AccessibilityElementOperatorCallback& callback, const bool succeeded, const int32_t requestId);

    void SearchExtensionElementInfoByAccessibilityIdNG(const SearchParameter& searchParam,
        const RefPtr<NG::FrameNode>& node, std::list<Accessibility::AccessibilityElementInfo>& infos,
        const RefPtr<PipelineBase>& context, const RefPtr<NG::PipelineContext>& ngPipeline);
    void SearchElementInfosByTextNG(const SearchParameter& searchParam, const RefPtr<NG::FrameNode>& node,
        std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
        const RefPtr<NG::PipelineContext>& ngPipeline);
    std::list<Accessibility::AccessibilityElementInfo> SearchElementInfosByTextNG(
        int64_t elementId, const std::string& text, const RefPtr<NG::FrameNode>& node, int64_t offset);
    void FindFocusedExtensionElementInfoNG(const SearchParameter& searchParam,
        Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
        const RefPtr<NG::FrameNode>& root);
    void FocusExtensionElementMoveSearchNG(const SearchParameter& searchParam,
        Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
        const RefPtr<NG::FrameNode>& root, RefPtr<NG::FrameNode>& outputExtensionNode);
    void TransferExecuteAction(int64_t elementId, RefPtr<NG::FrameNode>& node,
        const std::map<std::string, std::string>& actionArguments,
        Accessibility::ActionType& action, int64_t uiExtensionOffset);
    RefPtr<NG::FrameNode> FindNodeFromRootByExtensionId(const RefPtr<NG::FrameNode>& root, const int64_t uiExtensionId);
    bool RegisterThirdProviderInteractionOperationAsChildTree(const Registration& registration);

    void DumpProperty(const RefPtr<AccessibilityNode>& node);
    void DumpPropertyNG(int64_t nodeID);
    void DumpHoverTestNG(uint32_t windowId, int64_t nodeID, int32_t x, int32_t y, bool verbose);
    RefPtr<NG::PipelineContext> FindPipelineByElementId(const int64_t elementId, RefPtr<NG::FrameNode>& node);
    RefPtr<NG::FrameNode> FindNodeFromPipeline(const WeakPtr<PipelineBase>& context, const int64_t elementId);
    RefPtr<PipelineBase> GetPipelineByWindowId(const int32_t windowId);

    RefPtr<NG::PipelineContext> GetPipelineByWindowId(uint32_t windowId);
    void DumpTreeNG(bool useWindowId, uint32_t windowId, int64_t rootId, bool isDumpSimplify = false);
    void DumpTreeNG(const RefPtr<NG::FrameNode>& parent, int32_t depth,
        int64_t nodeID, const CommonProperty& commonProperty, bool isDumpSimplify = false);
    void DumpTreeNodeSafeAreaInfoNg(const RefPtr<NG::FrameNode>& node);
    void DumpPadding(const std::unique_ptr<NG::PaddingProperty>& padding, std::string label);
    void DumpBorder(const std::unique_ptr<NG::BorderWidthProperty>& border, std::string label);
    void DumpTreeNodeCommonInfoNg(const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty);
    void DumpTreeNodeSimplifyInfoNG(
        const RefPtr<NG::FrameNode>& node, int32_t depth, const CommonProperty& commonProperty, int32_t childSize);
    void DumpTreeAccessibilityNodeNG(const RefPtr<NG::UINode>& uiNodeParent,
        int32_t depth, int64_t nodeID, const CommonProperty& commonProperty);
    bool CheckDumpInfoParams(const std::vector<std::string> &params);
    void GenerateCommonProperty(const RefPtr<PipelineBase>& context, CommonProperty& output,
        const RefPtr<PipelineBase>& mainContext, const RefPtr<NG::FrameNode>& node = nullptr);

    void FindText(const RefPtr<NG::UINode>& node, std::list<Accessibility::AccessibilityElementInfo>& infos,
        const RefPtr<NG::PipelineContext>& context,
        const CommonProperty& commonProperty, const SearchParameter& searchParam);

    void FindTextByTextHint(const RefPtr<NG::UINode>& node, std::list<Accessibility::AccessibilityElementInfo>& infos,
        const RefPtr<NG::PipelineContext>& context,
        const CommonProperty& commonProperty, const SearchParameter& searchParam);

    void UpdateAccessibilityElementInfo(
        const RefPtr<NG::FrameNode>& node, Accessibility::AccessibilityElementInfo& nodeInfo);

    void UpdateAccessibilityVisible(
        const RefPtr<NG::FrameNode>& node, AccessibilityElementInfo& nodeInfo);

    void UpdateVirtualNodeInfo(std::list<Accessibility::AccessibilityElementInfo>& infos,
        Accessibility::AccessibilityElementInfo& nodeInfo,
        const RefPtr<NG::UINode>& uiVirtualNode, const CommonProperty& commonProperty,
        const RefPtr<NG::PipelineContext>& ngPipeline);

    void UpdateVirtualNodeChildAccessibilityElementInfo(
        const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty,
        Accessibility::AccessibilityElementInfo& nodeParentInfo, Accessibility::AccessibilityElementInfo& nodeInfo,
        const RefPtr<NG::PipelineContext>& ngPipeline);

    void UpdateVirtualNodeAccessibilityElementInfo(
        const RefPtr<NG::FrameNode>& parent, const RefPtr<NG::FrameNode>& node,
        const CommonProperty& commonProperty, Accessibility::AccessibilityElementInfo& nodeInfo,
        const RefPtr<NG::PipelineContext>& ngPipeline);

    void UpdateAccessibilityElementInfo(
        const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty,
        Accessibility::AccessibilityElementInfo& nodeInfo, const RefPtr<NG::PipelineContext>& ngPipeline);

    void UpdateCacheInfoNG(std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<NG::FrameNode>& node,
        const CommonProperty& commonProperty, const RefPtr<NG::PipelineContext>& ngPipeline,
        const SearchParameter& searchParam);
#ifdef WEB_SUPPORTED

    void UpdateWebAccessibilityElementInfo(const std::shared_ptr<NG::TransitionalNodeInfo>& node,
        Accessibility::AccessibilityElementInfo& nodeInfo, int32_t treeId);

    void UpdateWebAccessibilityElementInfo(const std::shared_ptr<NG::TransitionalNodeInfo>& node,
        const CommonProperty& commonProperty, Accessibility::AccessibilityElementInfo& nodeInfo,
        const RefPtr<NG::WebPattern>& webPattern);

    void UpdateWebCacheInfo(std::list<Accessibility::AccessibilityElementInfo>& infos, int64_t nodeId,
        const CommonProperty& commonProperty, const RefPtr<NG::PipelineContext>& ngPipeline,
        const SearchParameter& searchParam, const RefPtr<NG::WebPattern>& webPattern);
#endif //WEB_SUPPORTED
    void NotifyChildTreeOnRegister(int32_t treeId);

    void NotifyChildTreeOnDeregister();

    void SendUecOnTreeEvent(int64_t splitElementId);

    void NotifySetChildTreeIdAndWinId(int64_t elementId, const int32_t treeId, const int32_t childWindowId);

    bool CheckIsChildElement(
        int64_t &elementId,
        const std::vector<std::string>& params,
        std::vector<std::string>& info,
        DumpMode mode,
        int64_t &rootId);

    bool NeedRegisterChildTree(uint32_t parentWindowId, int32_t parentTreeId, int64_t parentElementId);

    void FillEventInfoWithNode(
        const RefPtr<NG::FrameNode>& node,
        Accessibility::AccessibilityEventInfo& eventInfo,
        const RefPtr<NG::PipelineContext>& context,
        int64_t elementId);

    void NotifyAccessibilitySAStateChange(bool state);
    void DumpTreeNodeInfoInJson(
        const RefPtr<NG::FrameNode>& node, int32_t depth, const CommonProperty& commonProperty, int32_t childSize);
    void CreateNodeInfoJson(const RefPtr<NG::FrameNode>& node, const CommonProperty& commonProperty,
        std::unique_ptr<JsonValue>& json, int32_t childSize);

    void SendEventToAccessibilityWithNodeInner(const AccessibilityEvent& accessibilityEvent,
        const RefPtr<AceType>& node, const RefPtr<PipelineBase>& context);
    void SendAccessibilityAsyncEventInner(const AccessibilityEvent& accessibilityEvent);
    int64_t GetDelayTimeBeforeSendEvent(const AccessibilityEvent& accessibilityEvent, const RefPtr<AceType>& node);
    void UpdateChildrenNodeInCache(std::list<AccessibilityElementInfo>& infos,
        const CommonProperty& commonProperty, const RefPtr<NG::PipelineContext>& ngPipeline,
        const SearchParameter& searchParam, std::list<RefPtr<NG::FrameNode>>& children);
    std::string callbackKey_;
    uint32_t windowId_ = 0;
    std::shared_ptr<JsAccessibilityStateObserver> stateObserver_ = nullptr;
    std::shared_ptr<ToastAccessibilityConfigObserver> toastObserver_ = nullptr;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    int64_t currentFocusNodeId_ = -1;

    int64_t lastElementId_ = -1;
    WeakPtr<NG::FrameNode> lastFrameNode_;
    NG::SizeF oldGeometrySize_;
    mutable std::mutex childTreeCallbackMapMutex_;
    std::unordered_map<int64_t, std::shared_ptr<AccessibilityChildTreeCallback>> childTreeCallbackMap_;
    mutable std::mutex componentSACallbackMutex_;
    std::unordered_map<int64_t, std::shared_ptr<AccessibilitySAObserverCallback>> componentSACallbackMap_;
    int64_t parentElementId_ = INVALID_PARENT_ID;
    uint32_t parentWindowId_ = 0;
    int32_t parentTreeId_ = 0;
    uint32_t parentWebWindowId_ = 0;
    std::function<void(int32_t&, int32_t&)> getParentRectHandler_;
    std::function<void(AccessibilityParentRectInfo&)> getParentRectHandlerNew_;
    bool isUseJson_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_ACCESSIBILITY_JS_ACCESSIBILITY_MANAGER_H

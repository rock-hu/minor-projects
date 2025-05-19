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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_MANAGER_H

#include "base/geometry/ng/point_t.h"
#include "base/memory/ace_type.h"
#include "core/accessibility/accessibility_node.h"
#include "core/accessibility/accessibility_constants.h"
#include "core/accessibility/accessibility_provider.h"
#include "core/accessibility/accessibility_utils.h"

namespace OHOS::Accessibility {
class AccessibilityElementInfo;
class AccessibilityEventInfo;
class AccessibilityElementOperator;
} // namespace OHOS::Accessibility

namespace OHOS::Ace::NG {
class WebPattern;
class FrameNode;
class UIExtensionManager;
} // namespace OHOS::Ace::NG

namespace OHOS::Ace {

constexpr int32_t ZERO_ANGLE = 0;
constexpr int32_t QUARTER_ANGLE = 90;
constexpr int32_t HALF_ANGLE = 180;
constexpr int32_t THREE_QUARTER_ANGLE = 270;
constexpr int32_t FULL_ANGLE = 360;

struct RotateTransform {
    int32_t rotateDegree = 0;  // final rotate degree of parent interface
    int32_t centerX = 0;       // center X of parent interface relative to real window
    int32_t centerY = 0;       // center Y of parent interface relative to real window
    int32_t innerCenterX = 0;  // relative center X of parent interface relative without transform
    int32_t innerCenterY = 0;  // relative center Y of parent interface relative without transform

    RotateTransform(int32_t degree = 0, int32_t x = 0, int32_t y = 0, int32_t innerX = 0, int32_t innerY = 0)
        : rotateDegree(degree), centerX(x), centerY(y), innerCenterX(innerX), innerCenterY(innerY) {}
};

class ComposedElement;

struct AccessibilityEvent {
    int64_t nodeId = 0;
    int64_t stackNodeId = -1;
    uint32_t windowId = 0;
    WindowsContentChangeTypes windowContentChangeTypes = CONTENT_CHANGE_TYPE_INVALID;
    WindowUpdateType windowChangeTypes = WINDOW_UPDATE_INVALID;
    std::string eventType;
    std::string componentType;
    std::string beforeText;
    std::string latestContent;
    std::string textAnnouncedForAccessibility;
    double currentItemIndex = 0.0;
    double itemCount = 0.0;
    AccessibilityEventType type = AccessibilityEventType::UNKNOWN;
    int32_t startIndex = 0;
    int32_t endIndex = 0;
};

enum class OperatorType {
    UNDEFINE = 1,
    JS_UIEXTENSION,
    JS_WEB,
    JS_THIRD_PROVIDER,
};

struct Registration {
    uint32_t windowId = 0;
    uint32_t parentWindowId = 0;
    int32_t parentTreeId = 0;
    int64_t elementId = 0;
    OperatorType operatorType = OperatorType::UNDEFINE;
    WeakPtr<NG::FrameNode> hostNode;
    WeakPtr<AccessibilityProvider> accessibilityProvider;
};

enum class AccessibilityVersion {
    JS_VERSION = 1,
    JS_DECLARATIVE_VERSION,
};

struct AccessibilityParentRectInfo {
    int32_t left = 0;
    int32_t top = 0;
    float scaleX = 1.0f;       // scale of parent interface
    float scaleY = 1.0f;       // scale of parent interface
    RotateTransform rotateTransform;
    bool isChanged = false;    // only for uiextension, true means uec transfered translate params to uiextension
};

struct AccessibilityWorkMode {
    bool isTouchExplorationEnabled = true;
};

struct AccessibilityWindowInfo {
    int32_t left = 0;
    int32_t top = 0;
    int32_t innerWindowId = -1;
    float_t scaleX = 1.0f;
    float_t scaleY = 1.0f;
    RotateTransform rotateTransform;
};

enum class AccessibilityCallbackEventId : uint32_t {
    ON_LOAD_PAGE = 0,
    ON_SHOW = 1,
    ON_HIDE = 2,
    ON_SEND_ELEMENT_INFO_CHANGE = 3,
};

struct AccessibilityCallbackEvent {
    AccessibilityCallbackEventId eventId;
    int64_t parameter;
    AccessibilityCallbackEvent(AccessibilityCallbackEventId id, int64_t para) : eventId(id), parameter(para) {}
    bool operator < (const AccessibilityCallbackEvent& other) const
    {
        return std::tie(eventId, parameter) < std::tie(other.eventId, other.parameter);
    }
};

class AccessibilitySAObserverCallback {
public:
    explicit AccessibilitySAObserverCallback(int64_t accessibilityId) : accessibilityId_(accessibilityId)
    {}
    virtual ~AccessibilitySAObserverCallback() = default;
    virtual bool OnState(bool state) = 0;

    int64_t GetAccessibilityId() const
    {
        return accessibilityId_;
    }
private:
    int64_t accessibilityId_ = -1;
};

class AccessibilityChildTreeCallback {
public:
    explicit AccessibilityChildTreeCallback(int64_t accessibilityId) : accessibilityId_(accessibilityId)
    {}
    virtual ~AccessibilityChildTreeCallback() = default;
    virtual bool OnRegister(uint32_t windowId, int32_t treeId) = 0;
    virtual bool OnDeregister() = 0;
    virtual bool OnSetChildTree(int32_t childWindowId, int32_t childTreeId) = 0;
    virtual bool OnDumpChildInfo(const std::vector<std::string>& params, std::vector<std::string>& info) = 0;
    virtual void OnClearRegisterFlag() = 0;
    int32_t GetChildTreeId() const
    {
        return childTreeId_;
    }
    void SetChildTreeId(int32_t childTreeId)
    {
        childTreeId_ = childTreeId;
    }
    int64_t GetAccessibilityId() const
    {
        return accessibilityId_;
    }

private:
    int32_t childTreeId_ = 0;
    int64_t accessibilityId_ = -1;
};

using VisibleRatioCallback = std::function<void(bool, double)>;
class AccessibilityManager : public AceType {
    DECLARE_ACE_TYPE(AccessibilityManager, AceType);

public:
    AccessibilityManager() = default;
    ~AccessibilityManager() override = default;

    virtual void SendAccessibilityAsyncEvent(const AccessibilityEvent& accessibilityEvent) = 0;
    virtual void SendWebAccessibilityAsyncEvent(const AccessibilityEvent& accessibilityEvent,
        const RefPtr<NG::WebPattern>& webPattern) {}
    virtual bool IsScreenReaderEnabled()
    {
        return false;
    }
    virtual void UpdateVirtualNodeFocus() = 0;
    virtual int64_t GenerateNextAccessibilityId() = 0;
    virtual RefPtr<AccessibilityNode> CreateSpecializedNode(
        const std::string& tag, int32_t nodeId, int32_t parentNodeId) = 0;
    virtual RefPtr<AccessibilityNode> CreateAccessibilityNode(
        const std::string& tag, int32_t nodeId, int32_t parentNodeId, int32_t itemIndex) = 0;
    virtual RefPtr<AccessibilityNode> GetAccessibilityNodeById(NodeId nodeId) const = 0;
    virtual std::string GetInspectorNodeById(NodeId nodeId) const = 0;
    virtual void RemoveAccessibilityNodes(RefPtr<AccessibilityNode>& node) = 0;
    virtual void RemoveAccessibilityNodeById(NodeId nodeId) = 0;
    virtual void ClearPageAccessibilityNodes(int32_t pageId) = 0;
    virtual void SetRootNodeId(int32_t nodeId) = 0;
    virtual void TrySaveTargetAndIdNode(
        const std::string& id, const std::string& target, const RefPtr<AccessibilityNode>& node) = 0;
    virtual void HandleComponentPostBinding() = 0;
    virtual void OnDumpInfo(const std::vector<std::string>& params) = 0;
    virtual void OnDumpInfoNG(const std::vector<std::string>& params, uint32_t windowId, bool hasJson = false) = 0;
    virtual void SetCardViewPosition(int id, float offsetX, float offsetY) = 0;
    virtual void SetCardViewParams(const std::string& key, bool focus) = 0;
    virtual void SetSupportAction(uint32_t action, bool isEnable) = 0;
    virtual void ClearNodeRectInfo(RefPtr<AccessibilityNode>& node, bool isPopDialog) = 0;
    virtual void AddComposedElement(const std::string& key, const RefPtr<ComposedElement>& node) = 0;
    virtual void RemoveComposedElementById(const std::string& key) = 0;
    virtual WeakPtr<ComposedElement> GetComposedElementFromPage(NodeId nodeId) = 0;
    virtual void TriggerVisibleChangeEvent() = 0;
    virtual void AddVisibleChangeNode(NodeId nodeId, double ratio, VisibleRatioCallback callback) = 0;
    virtual void RemoveVisibleChangeNode(NodeId nodeId) = 0;
    virtual bool IsVisibleChangeNodeExists(NodeId nodeId) = 0;
    virtual void UpdateEventTarget(NodeId id, BaseEventInfo& info) = 0;
    virtual void SetWindowPos(int32_t left, int32_t top, int32_t windowId) = 0;
    virtual AccessibilityParentRectInfo GetTransformRectInfoRelativeToWindow(
        const RefPtr<NG::FrameNode>& node, const RefPtr<PipelineBase>& context) { return {}; }
#ifdef WINDOW_SCENE_SUPPORTED
    virtual void SearchElementInfoByAccessibilityIdNG(int64_t elementId, int32_t mode,
        std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
        int64_t uiExtensionOffset) = 0;
    virtual void SearchElementInfosByTextNG(int64_t elementId, const std::string& text,
        std::list<Accessibility::AccessibilityElementInfo>& infos, const RefPtr<PipelineBase>& context,
        const int64_t uiExtensionOffset = 0) = 0;
    virtual void FindFocusedElementInfoNG(int64_t elementId, int32_t focusType,
        Accessibility::AccessibilityElementInfo& info, const RefPtr<PipelineBase>& context,
        const int64_t uiExtensionOffset = 0) = 0;
    virtual void FocusMoveSearchNG(int64_t elementId, int32_t direction, Accessibility::AccessibilityElementInfo& info,
        const RefPtr<PipelineBase>& context, const int64_t uiExtensionOffset = 0) = 0;
    virtual bool ExecuteExtensionActionNG(int64_t elementId, const std::map<std::string, std::string>& actionArguments,
        int32_t action, const RefPtr<PipelineBase>& context, int64_t uiExtensionOffset) = 0;
    virtual bool TransferAccessibilityAsyncEvent(
        const Accessibility::AccessibilityEventInfo& eventInfo, int64_t uiExtensionOffset)
    {
        return false;
    }
    virtual void SendExtensionAccessibilityEvent(
        const Accessibility::AccessibilityEventInfo& eventInfo, int64_t uiExtensionOffset) {}
#endif
#ifdef WEB_SUPPORTED
    virtual bool RegisterWebInteractionOperationAsChildTree(int64_t accessibilityId,
        const WeakPtr<NG::WebPattern>& webPattern)
    {
        return false;
    }
    virtual bool DeregisterWebInteractionOperationAsChildTree(int32_t treeId)
    {
        return false;
    }
#endif
    void SetVersion(AccessibilityVersion version)
    {
        version_ = version;
    }
    AccessibilityVersion GetVersion()
    {
        return version_;
    }

    virtual void RegisterAccessibilityChildTreeCallback(
        int64_t elementId, const std::shared_ptr<AccessibilityChildTreeCallback> &callback) {};

    virtual void DeregisterAccessibilityChildTreeCallback(int64_t elementId) {};

    virtual void RegisterInteractionOperationAsChildTree(
        uint32_t parentWindowId, int32_t parentTreeId, int64_t parentElementId) {};
    virtual void SetAccessibilityGetParentRectHandler(std::function<void(int32_t &, int32_t &)> &&callback) {};
    virtual void SetAccessibilityGetParentRectHandler(
        std::function<void(AccessibilityParentRectInfo &)> &&callback) {};
    virtual void DeregisterInteractionOperationAsChildTree() {};
    virtual void SendEventToAccessibilityWithNode(const AccessibilityEvent& accessibilityEvent,
        const RefPtr<AceType>& node, const RefPtr<PipelineBase>& context) {};

    virtual void AddFrameNodeToUecStatusVec(const RefPtr<NG::FrameNode>& node) {};
    virtual void AddFrameNodeToDefaultFocusList(const RefPtr<NG::FrameNode>& node, bool isFocus) {};

    virtual void RegisterUIExtGetPageModeCallback(RefPtr<NG::UIExtensionManager>& uiExtManager) {};
    virtual void UpdateFrameNodeState(int32_t nodeId) {};

    virtual void UpdatePageMode(const std::string& pageMode) {};

    virtual void RegisterAccessibilitySAObserverCallback(
        int64_t elementId, const std::shared_ptr<AccessibilitySAObserverCallback> &callback) {};

    virtual void DeregisterAccessibilitySAObserverCallback(int64_t elementId) {};

    virtual bool RegisterInteractionOperationAsChildTree(
        const Registration& registration) { return false; };
    virtual bool DeregisterInteractionOperationAsChildTree(
        uint32_t windowId, int32_t treeId) { return false; };

    virtual void TransferThirdProviderHoverEvent(
        const WeakPtr<NG::FrameNode>& hostNode, const NG::PointF &point, SourceType source,
        NG::AccessibilityHoverEventType eventType, TimeStamp time) {};

    virtual bool OnDumpChildInfoForThird(
        int64_t hostElementId, const std::vector<std::string> &params, std::vector<std::string> &info)
    {
        return false;
    }

    virtual void FireAccessibilityEventCallback(uint32_t eventId, int64_t parameter) {}

    virtual void UpdateAccessibilityNextFocusIdMap(int32_t containerId,
                                                   const std::string& nextFocusInspectorKey, int64_t preAccessibilityId)
    {
    }

    bool IsRegister()
    {
        return isReg_;
    }

    void Register(bool state)
    {
        isReg_ = state;
    }

    int32_t GetTreeId() const
    {
        return treeId_;
    }

    void SetUiextensionId(int64_t uiExtensionId)
    {
        uiExtensionId_  = uiExtensionId;
    }

    int64_t GetUiextensionId() const
    {
        return uiExtensionId_;
    }

    virtual AccessibilityWindowInfo GenerateWindowInfo(const RefPtr<NG::FrameNode>& node,
        const RefPtr<PipelineBase>& context)
    {
        return AccessibilityWindowInfo();
    }

    virtual void UpdateWindowInfo(AccessibilityWindowInfo& windowInfo, const RefPtr<PipelineBase>& context) {}
    virtual void UpdateAccessibilityNodeRect(const RefPtr<NG::FrameNode>& frameNode) {}
    virtual void OnAccessbibilityDetachFromMainTree(const RefPtr<NG::FrameNode>& frameNode) {}
    virtual int32_t GetTransformDegreeRelativeToWindow(const RefPtr<NG::FrameNode>& node, bool excludeSelf = false)
    {
        return 0;
    }

    virtual AccessibilityWorkMode GenerateAccessibilityWorkMode()
    {
        return AccessibilityWorkMode();
    }

    virtual void ReleasePageEvent(
        const RefPtr<NG::FrameNode>& node,
        bool deleteController = true,
        bool releaseAll = false) {}
    virtual void AddToPageEventController(const RefPtr<NG::FrameNode>& node) {}
    virtual bool CheckPageEventCached(const RefPtr<NG::FrameNode>& node, bool onlyCurrentPage) {return false;}
    virtual bool CheckAccessibilityVisible(const RefPtr<NG::FrameNode>& node) {return true;}

    virtual void AddHoverTransparentCallback(const RefPtr<NG::FrameNode>& node) {};
    virtual bool IsInHoverTransparentCallbackList(const RefPtr<NG::FrameNode>& node) { return false; };

protected:
    int32_t treeId_ = 0;

private:
    AccessibilityVersion version_ = AccessibilityVersion::JS_VERSION;
    bool isReg_ = false;
    int64_t uiExtensionId_ = 0;
};

class AccessibilityRect {
private:
    float x_;
    float y_;
    float width_;
    float height_;

    void calculateNewCenter(float cx, float cy, float rectCx, float rectCy,
        int angle, float& newCx, float& newCy)
    {
        int addDouble = 2;
        switch (angle) {
            case QUARTER_ANGLE:
                newCx = cx - (rectCy - cy);
                newCy = cy + (rectCx - cx);
                break;
            case HALF_ANGLE:
                newCx = addDouble * cx - rectCx;
                newCy = addDouble * cy - rectCy;
                break;
            case THREE_QUARTER_ANGLE:
                newCx = cx + (rectCy - cy);
                newCy = cy - (rectCx - cx);
                break;
        }
    }
public:
    AccessibilityRect(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f)
        : x_(x), y_(y), width_(width), height_(height) {}
    float GetX() const
    {
        return x_;
    }

    float GetY() const
    {
        return y_;
    }

    float GetWidth() const
    {
        return width_;
    }

    float GetHeight() const
    {
        return height_;
    }

    void SetPosition(float x, float y)
    {
        x_ = x;
        y_ = y;
    }

    void SetSize(float width, float height)
    {
        width_ = width;
        height_ = height;
    }

    void Rotate(float rotateCenterX, float rotateCenterY, int angle)
    {
        if (angle > FULL_ANGLE) {
            angle %= FULL_ANGLE;
        }
        if (angle == ZERO_ANGLE) {
            return;
        }
        const float rectCenterX = x_ + width_ * 0.5f;
        const float rectCenterY = y_ + height_ * 0.5f;
        float newCenterX;
        float newCenterY;
        calculateNewCenter(rotateCenterX, rotateCenterY, rectCenterX, rectCenterY, angle, newCenterX, newCenterY);
        float newWidth = width_;
        float newHeight = height_;
        if (angle == QUARTER_ANGLE || angle == THREE_QUARTER_ANGLE) {
            newWidth = height_;
            newHeight = width_;
        }
        x_ = newCenterX - newWidth * 0.5f;
        y_ = newCenterY - newHeight * 0.5f;
        width_ = newWidth;
        height_ = newHeight;
    }

    void Rotate(int angle)
    {
        const float centerX = x_ + width_ * 0.5f;
        const float centerY = y_ + height_ * 0.5f;
        Rotate(centerX, centerY, angle);
    }

    void ApplyTransformation(const RotateTransform& rotateTransform, const float& scaleX, const float& scaleY)
    {
        x_ = (x_ - rotateTransform.innerCenterX) * scaleX + rotateTransform.centerX;
        y_ = (y_ - rotateTransform.innerCenterY) * scaleY + rotateTransform.centerY;
        width_ *= scaleX;
        height_ *= scaleY;
    }
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_MANAGER_H

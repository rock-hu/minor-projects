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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_FRAME_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_FRAME_NODE_H

#include <functional>
#include <list>
#include <utility>
#include <mutex>

#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/point_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/geometry/ng/vector.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/thread/cancelable_callback.h"
#include "base/thread/task_executor.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/accessibility/accessibility_utils.h"
#include "core/common/recorder/exposure_processor.h"
#include "core/common/resource/resource_configuration.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/base/extension_handler.h"
#include "core/components_ng/base/frame_scene_status.h"
#include "core/components_ng/base/geometry_node.h"
#include "core/components_ng/base/modifier.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/event/focus_hub.h"
#include "core/components_ng/event/gesture_event_hub.h"
#include "core/components_ng/event/input_event_hub.h"
#include "core/components_ng/event/target_component.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/property/accessibility_property.h"
#include "core/components_ng/property/flex_property.h"
#include "core/components_ng/property/layout_constraint.h"
#include "core/components_ng/property/property.h"
#include "core/components_ng/render/paint_property.h"
#include "core/components_ng/render/paint_wrapper.h"
#include "core/components_ng/render/render_context.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/components_v2/inspector/inspector_node.h"

namespace OHOS::Accessibility {
class AccessibilityElementInfo;
class AccessibilityEventInfo;
} // namespace OHOS::Accessibility

namespace OHOS::Ace::Kit {
class FrameNode;
}

namespace OHOS::Ace::NG {
class InspectorFilter;
class PipelineContext;
class Pattern;
class StateModifyTask;
class UITask;
struct DirtySwapConfig;

struct CacheVisibleRectResult {
    OffsetF windowOffset = OffsetF();
    RectF visibleRect = RectF();
    RectF innerVisibleRect = RectF();
    VectorF cumulativeScale = {1.0f, 1.0f};
    RectF frameRect = RectF();
    RectF innerBoundaryRect = RectF();
};

struct CacheMatrixInfo {
    Matrix4 revertMatrix = Matrix4::CreateIdentity();
    Matrix4 localMatrix = Matrix4::CreateIdentity();
    RectF paintRectWithTransform;
};

// FrameNode will display rendering region in the screen.
class ACE_FORCE_EXPORT FrameNode : public UINode, public LayoutWrapper {
    DECLARE_ACE_TYPE(FrameNode, UINode, LayoutWrapper);

private:
    class FrameProxy;

public:
    // create a new child element with new element tree.
    static RefPtr<FrameNode> CreateFrameNodeWithTree(
        const std::string& tag, int32_t nodeId, const RefPtr<Pattern>& pattern);

    static RefPtr<FrameNode> GetOrCreateFrameNode(
        const std::string& tag, int32_t nodeId, const std::function<RefPtr<Pattern>(void)>& patternCreator);

    static RefPtr<FrameNode> GetOrCreateCommonNode(const std::string& tag, int32_t nodeId, bool isLayoutNode,
        const std::function<RefPtr<Pattern>(void)>& patternCreator);

    // create a new element with new pattern.
    static RefPtr<FrameNode> CreateFrameNode(
        const std::string& tag, int32_t nodeId, const RefPtr<Pattern>& pattern, bool isRoot = false);

    static RefPtr<FrameNode> CreateCommonNode(const std::string& tag, int32_t nodeId, bool isLayoutNode,
        const RefPtr<Pattern>& pattern, bool isRoot = false);

    // get element with nodeId from node map.
    static RefPtr<FrameNode> GetFrameNode(const std::string& tag, int32_t nodeId);

    static void ProcessOffscreenNode(const RefPtr<FrameNode>& node, bool needRemainActive = false);
    // avoid use creator function, use CreateFrameNode

    FrameNode(const std::string& tag, int32_t nodeId, const RefPtr<Pattern>& pattern,
        bool isRoot = false, bool isLayoutNode = false);

    ~FrameNode() override;

    int32_t FrameCount() const override
    {
        return 1;
    }

    int32_t CurrentFrameCount() const override
    {
        return 1;
    }

    void SetCheckboxFlag(const bool checkboxFlag)
    {
        checkboxFlag_ = checkboxFlag;
    }

    bool GetCheckboxFlag() const
    {
        return checkboxFlag_;
    }

    void SetDisallowDropForcedly(bool isDisallowDropForcedly)
    {
        isDisallowDropForcedly_ = isDisallowDropForcedly;
    }

    bool GetDisallowDropForcedly() const
    {
        return isDisallowDropForcedly_;
    }

    void OnInspectorIdUpdate(const std::string& id) override;

    void OnAutoEventParamUpdate(const std::string& value) override;

    void UpdateGeometryTransition() override;

    struct ZIndexComparator {
        bool operator()(const WeakPtr<FrameNode>& weakLeft, const WeakPtr<FrameNode>& weakRight) const
        {
            auto left = weakLeft.Upgrade();
            auto right = weakRight.Upgrade();
            if (left && right) {
                return left->GetRenderContext()->GetZIndexValue(ZINDEX_DEFAULT_VALUE) <
                       right->GetRenderContext()->GetZIndexValue(ZINDEX_DEFAULT_VALUE);
            }
            return false;
        }
    };

    const std::multiset<WeakPtr<FrameNode>, ZIndexComparator>& GetFrameChildren() const
    {
        return frameChildren_;
    }

    void InitializePatternAndContext();

    virtual void MarkModifyDone();

    void MarkDirtyNode(PropertyChangeFlag extraFlag = PROPERTY_UPDATE_NORMAL) override;

    void ProcessFreezeNode();

    void OnFreezeStateChange() override;

    void ProcessPropertyDiff()
    {
        if (isPropertyDiffMarked_) {
            MarkModifyDone();
            MarkDirtyNode();
            isPropertyDiffMarked_ = false;
        }
    }

    void FlushUpdateAndMarkDirty() override;

    void MarkNeedFrameFlushDirty(PropertyChangeFlag extraFlag = PROPERTY_UPDATE_NORMAL) override
    {
        MarkDirtyNode(extraFlag);
    }

    [[deprecated]] void OnMountToParentDone();

    void AfterMountToParent() override;

    bool GetIsLayoutNode();

    bool GetIsFind();

    void SetIsFind(bool isFind);

    void GetOneDepthVisibleFrame(std::list<RefPtr<FrameNode>>& children);

    void GetOneDepthVisibleFrameWithOffset(std::list<RefPtr<FrameNode>>& children, OffsetF& offset);

    void UpdateLayoutConstraint(const MeasureProperty& calcLayoutConstraint);

    RefPtr<LayoutWrapperNode> CreateLayoutWrapper(bool forceMeasure = false, bool forceLayout = false) override;

    RefPtr<LayoutWrapperNode> UpdateLayoutWrapper(
        RefPtr<LayoutWrapperNode> layoutWrapper, bool forceMeasure = false, bool forceLayout = false);

    void CreateLayoutTask(bool forceUseMainThread = false);

    std::optional<UITask> CreateRenderTask(bool forceUseMainThread = false);

    void SwapDirtyLayoutWrapperOnMainThread(const RefPtr<LayoutWrapper>& dirty);

    // Clear the user callback.
    void ClearUserOnAreaChange();

    void SetOnAreaChangeCallback(OnAreaChangedFunc&& callback);

    void TriggerOnAreaChangeCallback(uint64_t nanoTimestamp);

    void OnConfigurationUpdate(const ConfigurationChange& configurationChange) override;

    void SetVisibleAreaUserCallback(const std::vector<double>& ratios, const VisibleCallbackInfo& callback)
    {
        eventHub_->SetVisibleAreaRatiosAndCallback(callback, ratios, true);
    }

    void CleanVisibleAreaUserCallback(bool isApproximate = false);

    void SetVisibleAreaInnerCallback(const std::vector<double>& ratios, const VisibleCallbackInfo& callback,
        bool isCalculateInnerClip = false)
    {
        isCalculateInnerVisibleRectClip_ = isCalculateInnerClip;
        eventHub_->SetVisibleAreaRatiosAndCallback(callback, ratios, false);
    }

    void SetIsCalculateInnerVisibleRectClip(bool isCalculateInnerClip = true)
    {
        isCalculateInnerVisibleRectClip_ = isCalculateInnerClip;
    }

    void SetIsCalculateInnerClip(bool isCalculateInnerClip = false)
    {
        isCalculateInnerVisibleRectClip_ = isCalculateInnerClip;
    }

    void CleanVisibleAreaInnerCallback()
    {
        eventHub_->CleanVisibleAreaCallback(false);
    }

    void TriggerVisibleAreaChangeCallback(uint64_t timestamp, bool forceDisappear = false);

    void SetOnSizeChangeCallback(OnSizeChangedFunc&& callback);

    void AddInnerOnSizeChangeCallback(int32_t id, OnSizeChangedFunc&& callback);

    void SetJSFrameNodeOnSizeChangeCallback(OnSizeChangedFunc&& callback);

    void TriggerOnSizeChangeCallback();

    void SetGeometryNode(const RefPtr<GeometryNode>& node);

    const RefPtr<RenderContext>& GetRenderContext() const
    {
        return renderContext_;
    }

    const RefPtr<Pattern>& GetPattern() const;

    template<typename T>
    T* GetPatternPtr() const
    {
        return reinterpret_cast<T*>(RawPtr(pattern_));
    }

    template<typename T>
    RefPtr<T> GetPattern() const
    {
        return DynamicCast<T>(pattern_);
    }

    template<typename T>
    RefPtr<T> GetAccessibilityProperty() const
    {
        return DynamicCast<T>(accessibilityProperty_);
    }

    template<typename T>
    T* GetLayoutPropertyPtr() const
    {
        return reinterpret_cast<T*>(RawPtr(layoutProperty_));
    }

    template<typename T>
    RefPtr<T> GetLayoutProperty() const
    {
        return DynamicCast<T>(layoutProperty_);
    }

    template<typename T>
    T* GetPaintPropertyPtr() const
    {
        return reinterpret_cast<T*>(RawPtr(paintProperty_));
    }

    template<typename T>
    RefPtr<T> GetPaintProperty() const
    {
        return DynamicCast<T>(paintProperty_);
    }

    template<typename T>
    RefPtr<T> GetEventHub() const
    {
        return DynamicCast<T>(eventHub_);
    }

    RefPtr<GestureEventHub> GetOrCreateGestureEventHub() const
    {
        return eventHub_->GetOrCreateGestureEventHub();
    }

    RefPtr<InputEventHub> GetOrCreateInputEventHub() const
    {
        return eventHub_->GetOrCreateInputEventHub();
    }

    RefPtr<FocusHub> GetOrCreateFocusHub() const;

    const RefPtr<FocusHub>& GetFocusHub() const
    {
        return eventHub_->GetFocusHub();
    }

    bool HasVirtualNodeAccessibilityProperty() override
    {
        if (accessibilityProperty_ && accessibilityProperty_->GetAccessibilityVirtualNodePtr()) {
            return true;
        }
        return false;
    }

    FocusType GetFocusType() const
    {
        FocusType type = FocusType::DISABLE;
        auto focusHub = GetFocusHub();
        if (focusHub) {
            type = focusHub->GetFocusType();
        }
        return type;
    }

    void PostIdleTask(std::function<void(int64_t deadline, bool canUseLongPredictTask)>&& task);

    void AddJudgeToTargetComponent(RefPtr<TargetComponent>& targetComponent);

    // If return true, will prevent TouchTest Bubbling to parent and brother nodes.
    HitTestResult TouchTest(const PointF& globalPoint, const PointF& parentLocalPoint, const PointF& parentRevertPoint,
        TouchRestrict& touchRestrict, TouchTestResult& result, int32_t touchId, ResponseLinkResult& responseLinkResult,
        bool isDispatch = false) override;

    HitTestResult MouseTest(const PointF& globalPoint, const PointF& parentLocalPoint, MouseTestResult& onMouseResult,
        MouseTestResult& onHoverResult, RefPtr<FrameNode>& hoverNode) override;

    HitTestResult AxisTest(const PointF &globalPoint, const PointF &parentLocalPoint, const PointF &parentRevertPoint,
        TouchRestrict &touchRestrict, AxisTestResult &axisResult) override;

    void CollectSelfAxisResult(const PointF& globalPoint, const PointF& localPoint, bool& consumed,
        const PointF& parentRevertPoint, AxisTestResult& axisResult, bool& preventBubbling, HitTestResult& testResult,
        TouchRestrict& touchRestrict);

    void AnimateHoverEffect(bool isHovered) const;

    bool IsAtomicNode() const override;

    void MarkNeedSyncRenderTree(bool needRebuild = false) override;

    void RebuildRenderContextTree() override;

    bool IsContextTransparent() override;

    bool IsTrimMemRecycle() const
    {
        return isTrimMemRecycle_;
    }

    void SetTrimMemRecycle(bool isTrimMemRecycle)
    {
        isTrimMemRecycle_ = isTrimMemRecycle;
    }

    bool IsVisible() const
    {
        return layoutProperty_->GetVisibility().value_or(VisibleType::VISIBLE) == VisibleType::VISIBLE;
    }

    bool IsPrivacySensitive() const
    {
        return isPrivacySensitive_;
    }

    void SetPrivacySensitive(bool flag)
    {
        isPrivacySensitive_ = flag;
    }

    void ChangeSensitiveStyle(bool isSensitive);

    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;

    void ToTreeJson(std::unique_ptr<JsonValue>& json, const InspectorConfig& config) const override;

    void FromJson(const std::unique_ptr<JsonValue>& json) override;

    RefPtr<FrameNode> GetAncestorNodeOfFrame(bool checkBoundary = false) const;

    std::string& GetNodeName()
    {
        return nodeName_;
    }

    void SetNodeName(std::string& nodeName)
    {
        nodeName_ = nodeName;
    }

    void OnWindowShow() override;

    void OnWindowHide() override;

    void OnWindowFocused() override;

    void OnWindowUnfocused() override;

    void OnWindowSizeChanged(int32_t width, int32_t height, WindowSizeChangeReason type) override;

    void OnNotifyMemoryLevel(int32_t level) override;

    // call by recycle framework.
    void OnRecycle() override;
    void OnReuse() override;

    OffsetF GetOffsetRelativeToWindow() const;

    OffsetF GetPositionToScreen();

    OffsetF GetPositionToParentWithTransform() const;

    OffsetF GetPositionToScreenWithTransform();

    OffsetF GetPositionToWindowWithTransform(bool fromBottom = false) const;

    OffsetF GetTransformRelativeOffset() const;

    VectorF GetTransformScaleRelativeToWindow() const;

    RectF GetTransformRectRelativeToWindow() const;

    OffsetF GetPaintRectOffset(bool excludeSelf = false) const;

    OffsetF GetPaintRectOffsetNG(bool excludeSelf = false) const;

    bool GetRectPointToParentWithTransform(std::vector<Point>& pointList, const RefPtr<FrameNode>& parent) const;

    RectF GetPaintRectToWindowWithTransform();

    std::pair<OffsetF, bool> GetPaintRectGlobalOffsetWithTranslate(bool excludeSelf = false) const;

    OffsetF GetPaintRectOffsetToStage() const;

    RectF GetPaintRectWithTransform() const;

    VectorF GetTransformScale() const;

    void AdjustGridOffset();

    bool IsInternal() const
    {
        return isInternal_;
    }

    void SetInternal()
    {
        isInternal_ = true;
    }

    int32_t GetAllDepthChildrenCount();

    void OnAccessibilityEvent(
        AccessibilityEventType eventType, WindowsContentChangeTypes windowsContentChangeType =
                                              WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_INVALID) const;

    void OnAccessibilityEventForVirtualNode(AccessibilityEventType eventType, int64_t accessibilityId);

    void OnAccessibilityEvent(
        AccessibilityEventType eventType, std::string beforeText, std::string latestContent);

    void OnAccessibilityEvent(
        AccessibilityEventType eventType, int64_t stackNodeId, WindowsContentChangeTypes windowsContentChangeType);

    void OnAccessibilityEvent(
        AccessibilityEventType eventType, std::string textAnnouncedForAccessibility);
    void MarkNeedRenderOnly();

    void OnDetachFromMainTree(bool recursive, PipelineContext* context) override;
    void OnAttachToMainTree(bool recursive) override;
    void OnAttachToBuilderNode(NodeStatus nodeStatus) override;
    bool RenderCustomChild(int64_t deadline) override;
    void TryVisibleChangeOnDescendant(VisibleType preVisibility, VisibleType currentVisibility) override;
    void NotifyVisibleChange(VisibleType preVisibility, VisibleType currentVisibility);

    void PushDestroyCallbackWithTag(std::function<void()>&& callback, std::string tag)
    {
        destroyCallbacksMap_[tag] = callback;
    }

    void SetColorModeUpdateCallback(const std::function<void()>&& callback)
    {
        colorModeUpdateCallback_ = callback;
    }

    void SetNDKColorModeUpdateCallback(const std::function<void(int32_t)>&& callback)
    {
        std::unique_lock<std::shared_mutex> lock(colorModeCallbackMutex_);
        ndkColorModeUpdateCallback_ = callback;
        colorMode_ = SystemProperties::GetColorMode();
    }

    void SetNDKFontUpdateCallback(const std::function<void(float, float)>&& callback)
    {
        std::unique_lock<std::shared_mutex> lock(fontSizeCallbackMutex_);
        ndkFontUpdateCallback_ = callback;
    }

    void FireColorNDKCallback();
    void FireFontNDKCallback(const ConfigurationChange& configurationChange);

    bool MarkRemoving() override;

    void AddHotZoneRect(const DimensionRect& hotZoneRect) const;
    void RemoveLastHotZoneRect() const;

    virtual bool IsOutOfTouchTestRegion(const PointF& parentLocalPoint, const TouchEvent& touchEvent,
        std::vector<RectF>* regionList = nullptr);

    bool IsLayoutDirtyMarked() const
    {
        return isLayoutDirtyMarked_;
    }

    void SetLayoutDirtyMarked(bool marked)
    {
        isLayoutDirtyMarked_ = marked;
    }

    bool HasPositionProp() const
    {
        CHECK_NULL_RETURN(renderContext_, false);
        return renderContext_->HasPosition() || renderContext_->HasOffset() || renderContext_->HasPositionEdges() ||
               renderContext_->HasOffsetEdges() || renderContext_->HasAnchor();
    }

    // The function is only used for fast preview.
    void FastPreviewUpdateChildDone() override
    {
        OnMountToParentDone();
    }

    bool IsExclusiveEventForChild() const
    {
        return exclusiveEventForChild_;
    }

    void SetExclusiveEventForChild(bool exclusiveEventForChild)
    {
        exclusiveEventForChild_ = exclusiveEventForChild;
    }

    void SetDraggable(bool draggable)
    {
        draggable_ = draggable;
        userSet_ = true;
        customerSet_ = false;
    }

    void SetCustomerDraggable(bool draggable)
    {
        draggable_ = draggable;
        userSet_ = true;
        customerSet_ = true;
    }

    void SetDragPreviewOptions(const DragPreviewOption& previewOption, bool isResetOptions = true)
    {
        if (isResetOptions) {
            previewOption_ = previewOption;
        } else {
            auto options = previewOption_.options;
            previewOption_ = previewOption;
            previewOption_.options = options;
        }
        previewOption_.onApply = std::move(previewOption.onApply);
    }

    void SetOptionsAfterApplied(const OptionsAfterApplied& optionsAfterApplied)
    {
        previewOption_.options = optionsAfterApplied;
    }

    DragPreviewOption GetDragPreviewOption() const
    {
        return previewOption_;
    }

    void SetBackgroundFunction(std::function<RefPtr<UINode>()>&& buildFunc)
    {
        builderFunc_ = std::move(buildFunc);
        backgroundNode_ = nullptr;
    }

    bool IsDraggable() const
    {
        return draggable_;
    }

    bool IsLayoutComplete() const
    {
        return isLayoutComplete_;
    }

    bool IsUserSet() const
    {
        return userSet_;
    }

    bool IsCustomerSet() const
    {
        return customerSet_;
    }

    float GetPreGrayedOpacity() const
    {
        return preOpacity_;
    }

    void SetPreGrayedOpacity(float preOpacity)
    {
        preOpacity_ = preOpacity;
    }

    void SetAllowDrop(const std::set<std::string>& allowDrop)
    {
        allowDrop_ = allowDrop;
    }

    const std::set<std::string>& GetAllowDrop() const
    {
        return allowDrop_;
    }

    void SetDrawModifier(const RefPtr<NG::DrawModifier>& drawModifier)
    {
        if (!extensionHandler_) {
            extensionHandler_ = MakeRefPtr<ExtensionHandler>();
            extensionHandler_->AttachFrameNode(this);
        }
        extensionHandler_->SetDrawModifier(drawModifier);
    }

    bool IsSupportDrawModifier();

    void SetDragPreview(const NG::DragDropInfo& info)
    {
        dragPreviewInfo_ = info;
    }

    const DragDropInfo& GetDragPreview() const
    {
        return dragPreviewInfo_;
    }

    void SetOverlayNode(const RefPtr<FrameNode>& overlayNode)
    {
        overlayNode_ = overlayNode;
    }

    RefPtr<FrameNode> GetOverlayNode() const
    {
        return overlayNode_;
    }

    RefPtr<FrameNode> FindChildByPosition(float x, float y);
    // some developer use translate to make Grid drag animation, using old function can't find accurate child.
    // new function will ignore child's position and translate properties.
    RefPtr<FrameNode> FindChildByPositionWithoutChildTransform(float x, float y);

    RefPtr<NodeAnimatablePropertyBase> GetAnimatablePropertyFloat(const std::string& propertyName) const;
    static RefPtr<FrameNode> FindChildByName(const RefPtr<FrameNode>& parentNode, const std::string& nodeName);
    void CreateAnimatablePropertyFloat(const std::string& propertyName, float value,
        const std::function<void(float)>& onCallbackEvent, const PropertyUnit& propertyType = PropertyUnit::UNKNOWN);
    void DeleteAnimatablePropertyFloat(const std::string& propertyName);
    void UpdateAnimatablePropertyFloat(const std::string& propertyName, float value);
    void CreateAnimatableArithmeticProperty(const std::string& propertyName, RefPtr<CustomAnimatableArithmetic>& value,
        std::function<void(const RefPtr<CustomAnimatableArithmetic>&)>& onCallbackEvent);
    void UpdateAnimatableArithmeticProperty(const std::string& propertyName, RefPtr<CustomAnimatableArithmetic>& value);

    void SetHitTestMode(HitTestMode mode);
    HitTestMode GetHitTestMode() const override;

    TouchResult GetOnChildTouchTestRet(const std::vector<TouchTestInfo>& touchInfo);
    OnChildTouchTestFunc GetOnTouchTestFunc();
    void CollectTouchInfos(
        const PointF& globalPoint, const PointF& parentRevertPoint, std::vector<TouchTestInfo>& touchInfos);
    RefPtr<FrameNode> GetDispatchFrameNode(const TouchResult& touchRes);

    std::string ProvideRestoreInfo();

    static std::vector<RefPtr<FrameNode>> GetNodesById(const std::unordered_set<int32_t>& set);
    static std::vector<FrameNode*> GetNodesPtrById(const std::unordered_set<int32_t>& set);

    double GetPreviewScaleVal() const;

    bool IsPreviewNeedScale() const;

    void SetViewPort(RectF viewPort)
    {
        viewPort_ = viewPort;
    }

    std::optional<RectF> GetSelfViewPort() const
    {
        return viewPort_;
    }

    std::optional<RectF> GetViewPort() const;

    // Frame Rate Controller(FRC) decides FrameRateRange by scene, speed and scene status
    // speed is measured by millimeter/second
    void AddFRCSceneInfo(const std::string& scene, float speed, SceneStatus status);

    OffsetF GetParentGlobalOffsetDuringLayout() const;
    void OnSetCacheCount(int32_t cacheCount, const std::optional<LayoutConstraintF>& itemConstraint) override {};

    // layoutwrapper function override
    const RefPtr<LayoutAlgorithmWrapper>& GetLayoutAlgorithm(bool needReset = false) override;

    void Measure(const std::optional<LayoutConstraintF>& parentConstraint) override;

    // Called to perform layout children.
    void Layout() override;

    int32_t GetTotalChildCount() const override
    {
        return UINode::TotalChildCount();
    }

    int32_t GetTotalChildCountWithoutExpanded() const
    {
        return UINode::CurrentFrameCount();
    }

    const RefPtr<GeometryNode>& GetGeometryNode() const override
    {
        return geometryNode_;
    }

    void SetLayoutProperty(const RefPtr<LayoutProperty>& layoutProperty)
    {
        layoutProperty_ = layoutProperty;
        layoutProperty_->SetHost(WeakClaim(this));
    }

    const RefPtr<LayoutProperty>& GetLayoutProperty() const override
    {
        return layoutProperty_;
    }

    RefPtr<LayoutWrapper> GetOrCreateChildByIndex(
        uint32_t index, bool addToRenderTree = true, bool isCache = false) override;
    RefPtr<LayoutWrapper> GetChildByIndex(uint32_t index, bool isCache = false) override;

    FrameNode* GetFrameNodeChildByIndex(uint32_t index, bool isCache = false, bool isExpand = true);
    /**
     * @brief Get the index of Child among all FrameNode children of [this].
     * Handles intermediate SyntaxNodes like LazyForEach.
     *
     * @param child pointer to the Child FrameNode.
     * @return index of Child, or -1 if not found.
     */
    int32_t GetChildTrueIndex(const RefPtr<LayoutWrapper>& child) const;
    uint32_t GetChildTrueTotalCount() const;
    ChildrenListWithGuard GetAllChildrenWithBuild(bool addToRenderTree = true) override;
    void RemoveChildInRenderTree(uint32_t index) override;
    void RemoveAllChildInRenderTree() override;
    void DoRemoveChildInRenderTree(uint32_t index, bool isAll) override;
    void SetActiveChildRange(
        int32_t start, int32_t end, int32_t cacheStart = 0, int32_t cacheEnd = 0, bool showCached = false) override;
    void SetActiveChildRange(const std::optional<ActiveChildSets>& activeChildSets,
        const std::optional<ActiveChildRange>& activeChildRange = std::nullopt) override;
    void DoSetActiveChildRange(
        int32_t start, int32_t end, int32_t cacheStart, int32_t cacheEnd, bool showCache = false) override;
    void RecycleItemsByIndex(int32_t start, int32_t end) override;
    const std::string& GetHostTag() const override
    {
        return GetTag();
    }

    void UpdateFocusState();
    bool SelfOrParentExpansive();
    bool SelfExpansive();
    bool SelfExpansiveToKeyboard();
    bool ParentExpansive();

    bool IsActive() const override
    {
        return isActive_;
    }

    void SetActive(bool active = true, bool needRebuildRenderContext = false) override;

    bool GetAccessibilityVisible() const
    {
        return accessibilityVisible_;
    }

    void SetAccessibilityVisible(const bool accessibilityVisible)
    {
        accessibilityVisible_ = accessibilityVisible;
    }

    bool IsOutOfLayout() const override
    {
        return renderContext_->HasPosition() || renderContext_->HasPositionEdges();
    }
    void ProcessSafeAreaPadding();

    bool SkipMeasureContent() const override;
    float GetBaselineDistance() const override;
    void SetCacheCount(
        int32_t cacheCount = 0, const std::optional<LayoutConstraintF>& itemConstraint = std::nullopt) override;

    void SyncGeometryNode(bool needSyncRsNode, const DirtySwapConfig& config);
    RefPtr<UINode> GetFrameChildByIndex(
        uint32_t index, bool needBuild, bool isCache = false, bool addToRenderTree = false) override;
    RefPtr<UINode> GetFrameChildByIndexWithoutExpanded(uint32_t index) override;
    bool CheckNeedForceMeasureAndLayout() override;

    bool SetParentLayoutConstraint(const SizeF& size) const override;
    void ForceSyncGeometryNode();

    template<typename T>
    RefPtr<T> FindFocusChildNodeOfClass()
    {
        const auto& children = GetChildren();
        for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
            auto& child = *iter;
            auto target = DynamicCast<FrameNode>(child->FindChildNodeOfClass<T>());
            if (target) {
                auto focusEvent = target->eventHub_->GetFocusHub();
                if (focusEvent && focusEvent->IsCurrentFocus()) {
                    return AceType::DynamicCast<T>(target);
                }
            }
        }

        if (AceType::InstanceOf<T>(this)) {
            auto target = DynamicCast<FrameNode>(this);
            if (target) {
                auto focusEvent = target->eventHub_->GetFocusHub();
                if (focusEvent && focusEvent->IsCurrentFocus()) {
                    return Claim(AceType::DynamicCast<T>(this));
                }
            }
        }
        return nullptr;
    }

    virtual std::vector<RectF> GetResponseRegionList(const RectF& rect, int32_t sourceType);

    bool IsFirstBuilding() const
    {
        return isFirstBuilding_;
    }

    void MarkBuildDone()
    {
        isFirstBuilding_ = false;
    }

    Matrix4 GetLocalMatrix() const
    {
        return localMat_;
    }
    OffsetF GetOffsetInScreen();
    OffsetF GetOffsetInSubwindow(const OffsetF& subwindowOffset);
    RefPtr<PixelMap> GetPixelMap();
    RefPtr<FrameNode> GetPageNode();
    RefPtr<FrameNode> GetFirstAutoFillContainerNode();
    RefPtr<FrameNode> GetNodeContainer();
    RefPtr<ContentModifier> GetContentModifier();

    ExtensionHandler* GetExtensionHandler() const
    {
        return RawPtr(extensionHandler_);
    }

    void SetExtensionHandler(const RefPtr<ExtensionHandler>& handler)
    {
        extensionHandler_ = handler;
        if (extensionHandler_) {
            extensionHandler_->AttachFrameNode(this);
        }
    }

    void NotifyFillRequestSuccess(RefPtr<ViewDataWrap> viewDataWrap,
        RefPtr<PageNodeInfoWrap> nodeWrap, AceAutoFillType autoFillType);
    void NotifyFillRequestFailed(int32_t errCode, const std::string& fillContent = "", bool isPopup = false);

    int32_t GetUiExtensionId();
    int64_t WrapExtensionAbilityId(int64_t extensionOffset, int64_t abilityId);
    void SearchExtensionElementInfoByAccessibilityIdNG(
        int64_t elementId, int32_t mode, int64_t offset, std::list<Accessibility::AccessibilityElementInfo>& output);
    void SearchElementInfosByTextNG(int64_t elementId, const std::string& text, int64_t offset,
        std::list<Accessibility::AccessibilityElementInfo>& output);
    void FindFocusedExtensionElementInfoNG(
        int64_t elementId, int32_t focusType, int64_t offset, Accessibility::AccessibilityElementInfo& output);
    void FocusMoveSearchNG(
        int64_t elementId, int32_t direction, int64_t offset, Accessibility::AccessibilityElementInfo& output);
    bool TransferExecuteAction(
        int64_t elementId, const std::map<std::string, std::string>& actionArguments, int32_t action, int64_t offset);
    std::vector<RectF> GetResponseRegionListForRecognizer(int32_t sourceType);

    std::vector<RectF> GetResponseRegionListForTouch(const RectF& rect);

    void GetResponseRegionListByTraversal(std::vector<RectF>& responseRegionList);

    bool InResponseRegionList(const PointF& parentLocalPoint, const std::vector<RectF>& responseRegionList);

    bool GetMonopolizeEvents() const;

    bool IsWindowBoundary() const
    {
        return isWindowBoundary_;
    }

    void SetWindowBoundary(bool isWindowBoundary = true)
    {
        isWindowBoundary_ = isWindowBoundary;
    }

    void SetIsMeasureBoundary(bool isMeasureBoundary)
    {
        isMeasureBoundary_ = isMeasureBoundary;
    }

    void InitLastArea();

    OffsetF CalculateCachedTransformRelativeOffset(uint64_t nanoTimestamp);

    void PaintDebugBoundary(bool flag) override;
    RectF GetRectWithRender();
    bool CheckAncestorPageShow();

    void SetRemoveCustomProperties(std::function<void()> func)
    {
        if (!removeCustomProperties_) {
            removeCustomProperties_ = func;
        }
    }

    void GetVisibleRect(RectF& visibleRect, RectF& frameRect) const;
    void GetVisibleRectWithClip(RectF& visibleRect, RectF& visibleInnerRect, RectF& frameRect) const;

    void AttachContext(PipelineContext* context, bool recursive = false) override;
    void DetachContext(bool recursive = false) override;

    void SetExposureProcessor(const RefPtr<Recorder::ExposureProcessor>& processor);

    bool GetIsGeometryTransitionIn() const
    {
        return isGeometryTransitionIn_;
    }

    void SetIsGeometryTransitionIn(bool isGeometryTransitionIn)
    {
        isGeometryTransitionIn_ = isGeometryTransitionIn;
    }

    void SetGeometryTransitionInRecursive(bool isGeometryTransitionIn) override
    {
        SetIsGeometryTransitionIn(isGeometryTransitionIn);
        UINode::SetGeometryTransitionInRecursive(isGeometryTransitionIn);
    }
    static std::pair<float, float> ContextPositionConvertToPX(
        const RefPtr<RenderContext>& context, const SizeF& percentReference);

    // Notified by render context when any transform attributes updated,
    // this flag will be used to refresh the transform matrix cache if it's dirty
    void NotifyTransformInfoChanged()
    {
        isTransformNotChanged_ = false;
    }

    void AddPredictLayoutNode(const RefPtr<FrameNode>& node)
    {
        predictLayoutNode_.emplace_back(node);
    }

    bool CheckAccessibilityLevelNo() const {
        return false;
    }

    void HasAccessibilityVirtualNode(bool hasAccessibilityVirtualNode)
    {
        hasAccessibilityVirtualNode_ = hasAccessibilityVirtualNode;
    }

    void ProcessAccessibilityVirtualNode();

    void UpdateAccessibilityNodeRect();

    RectF GetVirtualNodeTransformRectRelativeToWindow()
    {
        auto parentUinode = GetVirtualNodeParent().Upgrade();
        CHECK_NULL_RETURN(parentUinode, RectF {});
        auto parentFrame = AceType::DynamicCast<FrameNode>(parentUinode);
        CHECK_NULL_RETURN(parentFrame, RectF {});
        auto parentRect = parentFrame->GetTransformRectRelativeToWindow();
        auto currentRect = GetTransformRectRelativeToWindow();
        currentRect.SetTop(currentRect.Top() + parentRect.Top());
        currentRect.SetLeft(currentRect.Left() + parentRect.Left());
        return currentRect;
    }

    void SetIsUseTransitionAnimator(bool isUseTransitionAnimator)
    {
        isUseTransitionAnimator_ = isUseTransitionAnimator;
    }

    bool GetIsUseTransitionAnimator()
    {
        return isUseTransitionAnimator_;
    }

    // this method will check the cache state and return the cached revert matrix preferentially,
    // but the caller can pass in true to forcible refresh the cache
    CacheMatrixInfo& GetOrRefreshMatrixFromCache(bool forceRefresh = false);

    // apply the matrix to the given point specified by dst
    static void MapPointTo(PointF& dst, Matrix4& matrix);
    void SetSuggestOpIncMarked(bool flag);
    bool GetSuggestOpIncMarked();
    void SetCanSuggestOpInc(bool flag);
    bool GetCanSuggestOpInc();
    void SetApplicationRenderGroupMarked(bool flag);
    bool GetApplicationRenderGroupMarked();
    void SetSuggestOpIncActivatedOnce();
    bool GetSuggestOpIncActivatedOnce();
    bool MarkSuggestOpIncGroup(bool suggest, bool calc);
    void SetOpIncGroupCheckedThrough(bool flag);
    bool GetOpIncGroupCheckedThrough();
    void SetOpIncCheckedOnce();
    bool GetOpIncCheckedOnce();
    void MarkAndCheckNewOpIncNode();
    ChildrenListWithGuard GetAllChildren();
    OPINC_TYPE_E FindSuggestOpIncNode(std::string& path, const SizeF& boundary, int32_t depth);
    void GetInspectorValue() override;
    void NotifyWebPattern(bool isRegister) override;

    FrameNodeChangeInfoFlag GetChangeInfoFlag()
    {
        return changeInfoFlag_;
    }

    void ClearSubtreeLayoutAlgorithm(bool includeSelf = true, bool clearEntireTree = false) override;

    void ClearChangeInfoFlag()
    {
        changeInfoFlag_ = FRAME_NODE_CHANGE_INFO_NONE;
    }

    void OnSyncGeometryFrameFinish(const RectF& paintRect);
    void AddFrameNodeChangeInfoFlag(FrameNodeChangeInfoFlag changeFlag = FRAME_NODE_CHANGE_INFO_NONE);
    void RegisterNodeChangeListener();
    void UnregisterNodeChangeListener();
    void ProcessFrameNodeChangeFlag();
    void OnNodeTransformInfoUpdate(bool changed);
    void OnNodeTransitionInfoUpdate();
    uint32_t GetWindowPatternType() const;

    void ResetLayoutAlgorithm()
    {
        layoutAlgorithm_.Reset();
    }

    bool HasLayoutAlgorithm()
    {
        return layoutAlgorithm_ != nullptr;
    }

    bool GetDragHitTestBlock() const
    {
        return dragHitTestBlock_;
    }

    void SetDragHitTestBlock(bool dragHitTestBlock)
    {
        dragHitTestBlock_ = dragHitTestBlock;
    }

    void NotifyChange(int32_t changeIdx, int32_t count, int64_t id, NotificationType notificationType) override;

    void ChildrenUpdatedFrom(int32_t index);
    int32_t GetChildrenUpdated() const
    {
        return childrenUpdatedFrom_;
    }

    void SetJSCustomProperty(std::function<bool()> func, std::function<std::string(const std::string&)> getFunc);
    bool GetJSCustomProperty(const std::string& key, std::string& value);
    bool GetCapiCustomProperty(const std::string& key, std::string& value);

    void AddCustomProperty(const std::string& key, const std::string& value) override;
    void RemoveCustomProperty(const std::string& key) override;

    void AddExtraCustomProperty(const std::string& key, void* extraData);
    void* GetExtraCustomProperty(const std::string& key) const;
    void RemoveExtraCustomProperty(const std::string& key);
    bool GetCustomPropertyByKey(const std::string& key, std::string& value);
    void AddNodeDestroyCallback(const std::string& callbackKey, std::function<void()>&& callback);
    void RemoveNodeDestroyCallback(const std::string& callbackKey);
    void FireOnExtraNodeDestroyCallback();

    LayoutConstraintF GetLayoutConstraint() const;

    WeakPtr<TargetComponent> GetTargetComponent() const
    {
        return targetComponent_;
    }

    void SetExposeInnerGestureFlag(bool exposeInnerGestureFlag)
    {
        exposeInnerGestureFlag_ = exposeInnerGestureFlag;
    }

    bool GetExposeInnerGestureFlag() const
    {
        return exposeInnerGestureFlag_;
    }

    RefPtr<UINode> GetCurrentPageRootNode() override;

    std::list<RefPtr<FrameNode>> GetActiveChildren();

    void MarkDirtyWithOnProChange(PropertyChangeFlag extraFlag);
    void OnPropertyChangeMeasure() const;

    void SetKitNode(const RefPtr<Kit::FrameNode>& node);

    void SetVisibleAreaChangeTriggerReason(VisibleAreaChangeTriggerReason triggerReason)
    {
        if (visibleAreaChangeTriggerReason_ != triggerReason) {
            visibleAreaChangeTriggerReason_ = triggerReason;
        }
    }

    void OnThemeScopeUpdate(int32_t themeScopeId) override;

    OffsetF CalculateOffsetRelativeToWindow(uint64_t nanoTimestamp, bool logFlag = false);

    bool IsDebugInspectorId();

    RectF GetLastFrameRect() const
    {
        RectF rect;
        return lastFrameRect_ ? *lastFrameRect_ : rect;
    }
    void SetLastFrameRect(const RectF& lastFrameRect)
    {
        *lastFrameRect_ = lastFrameRect;
    }
    OffsetF GetLastParentOffsetToWindow() const
    {
        OffsetF offset;
        return lastParentOffsetToWindow_ ? *lastParentOffsetToWindow_ : offset;
    }
    void SetLastParentOffsetToWindow(const OffsetF& lastParentOffsetToWindow)
    {
        *lastParentOffsetToWindow_ = lastParentOffsetToWindow;
    }
    std::shared_ptr<OffsetF>& GetLastHostParentOffsetToWindow()
    {
        return lastHostParentOffsetToWindow_;
    }

protected:
    void DumpInfo() override;
    std::unordered_map<std::string, std::function<void()>> destroyCallbacksMap_;
    void DumpInfo(std::unique_ptr<JsonValue>& json) override;
    void DumpSimplifyInfo(std::unique_ptr<JsonValue>& json) override;

private:
    void MarkDirtyNode(
        bool isMeasureBoundary, bool isRenderBoundary, PropertyChangeFlag extraFlag = PROPERTY_UPDATE_NORMAL);
    OPINC_TYPE_E IsOpIncValidNode(const SizeF& boundary, int32_t childNumber = 0);
    static int GetValidLeafChildNumber(const RefPtr<FrameNode>& host, int32_t thresh);
    void MarkNeedRender(bool isRenderBoundary);
    bool IsNeedRequestParentMeasure() const;
    void UpdateLayoutPropertyFlag() override;
    void ForceUpdateLayoutPropertyFlag(PropertyChangeFlag propertyChangeFlag) override;
    void AdjustParentLayoutFlag(PropertyChangeFlag& flag) override;
    /**
     * @brief try to mark Parent dirty with flag PROPERTY_UPDATE_BY_CHILD_REQUEST.
     *
     * @return true if Parent is successfully marked dirty.
     */
    virtual bool RequestParentDirty();

    void UpdateChildrenLayoutWrapper(const RefPtr<LayoutWrapperNode>& self, bool forceMeasure, bool forceLayout);
    void AdjustLayoutWrapperTree(const RefPtr<LayoutWrapperNode>& parent, bool forceMeasure, bool forceLayout) override;

    OffsetF GetParentGlobalOffset() const;

    RefPtr<PaintWrapper> CreatePaintWrapper();
    void LayoutOverlay();

    void OnGenerateOneDepthVisibleFrame(std::list<RefPtr<FrameNode>>& visibleList) override;
    void OnGenerateOneDepthVisibleFrameWithTransition(std::list<RefPtr<FrameNode>>& visibleList) override;
    void OnGenerateOneDepthVisibleFrameWithOffset(
        std::list<RefPtr<FrameNode>>& visibleList, OffsetF& offset) override;
    void OnGenerateOneDepthAllFrame(std::list<RefPtr<FrameNode>>& allList) override;

    bool IsMeasureBoundary();
    bool IsRenderBoundary();

    bool OnRemoveFromParent(bool allowTransition) override;
    bool RemoveImmediately() const override;

    bool IsPaintRectWithTransformValid();

    // dump self info.
    void DumpDragInfo();
    void DumpOverlayInfo();
    void DumpCommonInfo();
    void DumpCommonInfo(std::unique_ptr<JsonValue>& json);
    void DumpSimplifyCommonInfo(std::unique_ptr<JsonValue>& json);
    void DumpSimplifySafeAreaInfo(std::unique_ptr<JsonValue>& json);
    void DumpSimplifyOverlayInfo(std::unique_ptr<JsonValue>& json);
    void DumpBorder(const std::unique_ptr<NG::BorderWidthProperty>& border, std::string label,
        std::unique_ptr<JsonValue>& json);
    void DumpPadding(const std::unique_ptr<NG::PaddingProperty>& border, std::string label,
        std::unique_ptr<JsonValue>& json);
    void DumpOverlayInfo(std::unique_ptr<JsonValue>& json);
    void DumpDragInfo(std::unique_ptr<JsonValue>& json);
    void DumpAlignRulesInfo(std::unique_ptr<JsonValue>& json);
    void DumpSafeAreaInfo(std::unique_ptr<JsonValue>& json);
    void DumpExtensionHandlerInfo(std::unique_ptr<JsonValue>& json);
    void DumpOnSizeChangeInfo(std::unique_ptr<JsonValue>& json);
    void BuildLayoutInfo(std::unique_ptr<JsonValue>& json);

    void DumpSafeAreaInfo();
    void DumpAlignRulesInfo();
    void DumpExtensionHandlerInfo();
    void DumpAdvanceInfo() override;
    void DumpAdvanceInfo(std::unique_ptr<JsonValue>& json) override;
    void DumpViewDataPageNode(RefPtr<ViewDataWrap> viewDataWrap, bool needsRecordData = false) override;
    void DumpOnSizeChangeInfo();
    bool CheckAutoSave() override;
    void MouseToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;
    void TouchToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;
    void GeometryNodeToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;

    bool GetTouchable() const;
    bool OnLayoutFinish(bool& needSyncRsNode, DirtySwapConfig& config);

    void ProcessVisibleAreaChangeEvent(const RectF& visibleRect, const RectF& frameRect,
        const std::vector<double>& visibleAreaRatios, VisibleCallbackInfo& visibleAreaCallback, bool isUser);
    void ProcessAllVisibleCallback(const std::vector<double>& visibleAreaUserRatios,
        VisibleCallbackInfo& visibleAreaUserCallback, double currentVisibleRatio,
        double lastVisibleRatio, bool isThrottled = false, bool isInner = false);
    void ProcessThrottledVisibleCallback();
    bool IsFrameDisappear() const;
    bool IsFrameDisappear(uint64_t timestamp);
    bool IsFrameAncestorDisappear(uint64_t timestamp);
    void ThrottledVisibleTask();

    void OnPixelRoundFinish(const SizeF& pixelGridRoundSize);

    double CalculateCurrentVisibleRatio(const RectF& visibleRect, const RectF& renderRect);

    // set costom background layoutConstraint
    void SetBackgroundLayoutConstraint(const RefPtr<FrameNode>& customNode);

    void GetPercentSensitive();
    void UpdatePercentSensitive();

    void AddFrameNodeSnapshot(bool isHit, int32_t parentId, std::vector<RectF> responseRegionList, EventTreeType type);

    int32_t GetNodeExpectedRate();

    void RecordExposureInner();

    const std::pair<uint64_t, OffsetF>& GetCachedGlobalOffset() const;

    void SetCachedGlobalOffset(const std::pair<uint64_t, OffsetF>& timestampOffset);

    const std::pair<uint64_t, OffsetF>& GetCachedTransformRelativeOffset() const;

    void SetCachedTransformRelativeOffset(const std::pair<uint64_t, OffsetF>& timestampOffset);

    HitTestMode TriggerOnTouchIntercept(const TouchEvent& touchEvent);

    void TriggerShouldParallelInnerWith(
        const ResponseLinkResult& currentRecognizers, const ResponseLinkResult& responseLinkRecognizers);

    void TriggerRsProfilerNodeMountCallbackIfExist();

    void AddTouchEventAllFingersInfo(TouchEventInfo& event, const TouchEvent& touchEvent);

    RectF ApplyFrameNodeTranformToRect(const RectF& rect, const RefPtr<FrameNode>& parent) const;

    CacheVisibleRectResult GetCacheVisibleRect(uint64_t timestamp, bool logFlag = false);

    CacheVisibleRectResult CalculateCacheVisibleRect(CacheVisibleRectResult& parentCacheVisibleRect,
        const RefPtr<FrameNode>& parentUi, RectF& rectToParent, VectorF scale, uint64_t timestamp);

    void NotifyConfigurationChangeNdk(const ConfigurationChange& configurationChange);

    bool AllowVisibleAreaCheck() const;

    bool ProcessMouseTestHit(const PointF& globalPoint, const PointF& localPoint,
    TouchRestrict& touchRestrict, TouchTestResult& newComingTargets);

    void ResetPredictNodes();

    bool isTrimMemRecycle_ = false;
    // sort in ZIndex.
    std::multiset<WeakPtr<FrameNode>, ZIndexComparator> frameChildren_;
    RefPtr<GeometryNode> geometryNode_ = MakeRefPtr<GeometryNode>();

    std::function<void()> colorModeUpdateCallback_;
    std::function<void(int32_t)> ndkColorModeUpdateCallback_;
    std::function<void(float, float)> ndkFontUpdateCallback_;
    RefPtr<AccessibilityProperty> accessibilityProperty_;
    bool hasAccessibilityVirtualNode_ = false;
    RefPtr<LayoutProperty> layoutProperty_;
    RefPtr<PaintProperty> paintProperty_;
    RefPtr<RenderContext> renderContext_ = RenderContext::Create();
    RefPtr<EventHub> eventHub_;
    RefPtr<Pattern> pattern_;

    RefPtr<ExtensionHandler> extensionHandler_;

    RefPtr<FrameNode> backgroundNode_;
    std::function<RefPtr<UINode>()> builderFunc_;
    std::unique_ptr<RectF> lastFrameRect_;
    std::unique_ptr<OffsetF> lastParentOffsetToWindow_;
    std::shared_ptr<OffsetF> lastHostParentOffsetToWindow_;
    std::unique_ptr<RectF> lastFrameNodeRect_;
    std::set<std::string> allowDrop_;
    const static std::set<std::string> layoutTags_;
    std::function<void()> removeCustomProperties_;
    std::function<std::string(const std::string& key)> getCustomProperty_;
    std::optional<RectF> viewPort_;
    NG::DragDropInfo dragPreviewInfo_;

    RefPtr<LayoutAlgorithmWrapper> layoutAlgorithm_;
    RefPtr<GeometryNode> oldGeometryNode_;
    std::optional<bool> skipMeasureContent_;
    std::unique_ptr<FrameProxy> frameProxy_;
    WeakPtr<TargetComponent> targetComponent_;

    bool needSyncRenderTree_ = false;

    bool isPropertyDiffMarked_ = false;
    bool isLayoutDirtyMarked_ = false;
    bool isRenderDirtyMarked_ = false;
    bool isMeasureBoundary_ = false;
    bool hasPendingRequest_ = false;
    bool isPrivacySensitive_ = false;

    // for container, this flag controls only the last child in touch area is consuming event.
    bool exclusiveEventForChild_ = false;
    bool isActive_ = false;
    bool accessibilityVisible_ = true;
    bool isResponseRegion_ = false;
    bool isLayoutComplete_ = false;
    bool isFirstBuilding_ = true;

    double lastVisibleRatio_ = 0.0;
    double lastInnerVisibleRatio_ = 0.0;
    double lastVisibleCallbackRatio_ = 0.0;
    double lastInnerVisibleCallbackRatio_ = 0.0;
    double lastThrottledVisibleRatio_ = 0.0;
    double lastThrottledVisibleCbRatio_ = 0.0;
    int64_t lastThrottledTriggerTime_ = 0;
    bool throttledCallbackOnTheWay_ = false;

    // internal node such as Text in Button CreateWithLabel
    // should not seen by preview inspector or accessibility
    bool isInternal_ = false;

    std::string nodeName_;

    ColorMode colorMode_ = ColorMode::LIGHT;

    bool draggable_ = false;
    bool userSet_ = false;
    bool customerSet_ = false;
    bool isWindowBoundary_ = false;
    uint8_t suggestOpIncByte_ = 0;
    uint64_t getCacheNanoTime_ = 0;
    RectF prePaintRect_;

    std::map<std::string, RefPtr<NodeAnimatablePropertyBase>> nodeAnimatablePropertyMap_;
    Matrix4 localMat_ = Matrix4::CreateIdentity();
    // this is just used for the hit test process of event handling, do not used for other purpose
    CacheMatrixInfo cacheMatrixInfo_;
    // control the localMat_ and localRevertMatrix_ available or not, set to false when any transform info is set
    bool isTransformNotChanged_ = false;
    bool isFind_ = false;

    bool isRestoreInfoUsed_ = false;
    bool checkboxFlag_ = false;
    bool isDisallowDropForcedly_ = false;
    bool isGeometryTransitionIn_ = false;
    bool isLayoutNode_ = false;
    bool isCalculateInnerVisibleRectClip_ = false;
    bool dragHitTestBlock_ = false;

    bool isUseTransitionAnimator_ = false;

    bool exposeInnerGestureFlag_ = false;

    RefPtr<FrameNode> overlayNode_;

    std::unordered_map<std::string, int32_t> sceneRateMap_;

    DragPreviewOption previewOption_ { true, false, false, false, false, false, true, false, { .isShowBadge = true } };

    std::unordered_map<std::string, std::string> customPropertyMap_;

    std::unordered_map<std::string, void*> extraCustomPropertyMap_;

    std::map<std::string, std::function<void()>> destroyCallbacks_;

    RefPtr<Recorder::ExposureProcessor> exposureProcessor_;

    std::pair<uint64_t, OffsetF> cachedGlobalOffset_ = { 0, OffsetF() };
    std::pair<uint64_t, OffsetF> cachedTransformRelativeOffset_ = { 0, OffsetF() };
    std::pair<uint64_t, bool> cachedIsFrameDisappear_ = { 0, false };
    std::pair<uint64_t, CacheVisibleRectResult> cachedVisibleRectResult_ = { 0, CacheVisibleRectResult() };

    struct onSizeChangeDumpInfo {
        int64_t onSizeChangeTimeStamp;
        RectF lastFrameRect;
        RectF currFrameRect;
    };
    std::vector<onSizeChangeDumpInfo> onSizeChangeDumpInfos;
    std::list<WeakPtr<FrameNode>> predictLayoutNode_;
    FrameNodeChangeInfoFlag changeInfoFlag_ = FRAME_NODE_CHANGE_INFO_NONE;
    std::optional<RectF> syncedFramePaintRect_;

    int32_t childrenUpdatedFrom_ = -1;
    VisibleAreaChangeTriggerReason visibleAreaChangeTriggerReason_ = VisibleAreaChangeTriggerReason::IDLE;
    float preOpacity_ = 1.0f;

    friend class RosenRenderContext;
    friend class RenderContext;
    friend class Pattern;
    mutable std::shared_mutex fontSizeCallbackMutex_;
    mutable std::shared_mutex colorModeCallbackMutex_;

    RefPtr<Kit::FrameNode> kitNode_;
    ACE_DISALLOW_COPY_AND_MOVE(FrameNode);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_FRAME_NODE_H

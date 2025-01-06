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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_BUBBLE_BUBBLE_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_BUBBLE_BUBBLE_PATTERN_H

#include <optional>

#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/size_t.h"
#include "base/memory/referenced.h"
#include "core/components/common/properties/popup_param.h"
#include "core/components/popup/popup_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/event/focus_hub.h"
#include "core/components_ng/manager/focus/focus_view.h"
#include "core/components_ng/pattern/bubble/bubble_accessibility_property.h"
#include "core/components_ng/pattern/bubble/bubble_event_hub.h"
#include "core/components_ng/pattern/bubble/bubble_layout_algorithm.h"
#include "core/components_ng/pattern/bubble/bubble_layout_property.h"
#include "core/components_ng/pattern/bubble/bubble_paint_method.h"
#include "core/components_ng/pattern/bubble/bubble_render_property.h"
#include "core/components_ng/pattern/overlay/popup_base_pattern.h"

namespace OHOS::Ace::NG {

enum class TransitionStatus {
    INVISIABLE,
    ENTERING,
    NORMAL,
    EXITING,
};

enum class DismissReason {
    BACK_PRESSED = 0,
    TOUCH_OUTSIDE,
    CLOSE_BUTTON,
};

class BubblePattern : public PopupBasePattern, public FocusView {
    DECLARE_ACE_TYPE(BubblePattern, PopupBasePattern, FocusView);

public:
    BubblePattern() = default;
    BubblePattern(int32_t id, const std::string& tag) : targetNodeId_(id), targetTag_(tag) {}
    ~BubblePattern() override = default;

    bool IsAtomicNode() const override
    {
        return false;
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        auto bubbleMethod = AceType::MakeRefPtr<BubblePaintMethod>();
        bubbleMethod->SetArrowPosition(arrowPosition_);
        bubbleMethod->SetChildOffset(childOffset_);
        bubbleMethod->SetChildSize(childSize_);
        bubbleMethod->SetShowArrow(showArrow_);
        bubbleMethod->SetClipPath(clipPath_);
        bubbleMethod->SetClipFrameNode(clipFrameNode_);
        bubbleMethod->SetArrowOffsetsFromClip(arrowOffsetsFromClip_);
        bubbleMethod->SetArrowWidth(arrowWidth_);
        bubbleMethod->SetArrowHeight(arrowHeight_);
        bubbleMethod->SetBorder(border_);
        bubbleMethod->SetArrowBuildPlacement(arrowBuildPlacement_);
        auto pipeline = PipelineBase::GetCurrentContext();
        CHECK_NULL_RETURN(pipeline, bubbleMethod);
        auto theme = pipeline->GetTheme<PopupTheme>();
        CHECK_NULL_RETURN(theme, bubbleMethod);
        bubbleMethod->SetOuterBorderWidth(theme->GetPopupOuterBorderWidth());
        bubbleMethod->SetInnerBorderWidth(theme->GetPopupInnerBorderWidth());
        return bubbleMethod;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        auto bubbleProp = AceType::MakeRefPtr<BubbleLayoutProperty>();
        bubbleProp->UpdatePropertyChangeFlag(PROPERTY_UPDATE_MEASURE);
        return bubbleProp;
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<BubbleLayoutAlgorithm>(targetNodeId_, targetTag_, targetOffset_, targetSize_);
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<BubbleRenderProperty>();
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<BubbleEventHub>();
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<BubbleAccessibilityProperty>();
    }

    OffsetF GetChildOffset()
    {
        return childOffset_;
    }

    SizeF GetChildSize()
    {
        return childSize_;
    }

    FocusPattern GetFocusPattern() const override
    {
        return { FocusType::SCOPE, true };
    }

    std::list<int32_t> GetRouteOfFirstScope() override
    {
        return { 0, 0, 0 };
    }

    void OnWindowHide() override;
    void OnWindowSizeChanged(int32_t width, int32_t height, WindowSizeChangeReason type) override;
    void StartEnteringTransitionEffects(const RefPtr<FrameNode>& popupNode, const std::function<void()>& finish);
    void StartExitingTransitionEffects(const RefPtr<FrameNode>& popupNode, const std::function<void()>& finish);
    void StartEnteringAnimation(std::function<void()> finish);
    void StartExitingAnimation(std::function<void()> finish);
    bool IsOnShow();
    bool IsExiting();
    void OnColorConfigurationUpdate() override;
    void UpdateBubbleText();
    void UpdateText(const RefPtr<UINode>& node, const RefPtr<PopupTheme>& popupTheme);
    void AddPipelineCallBack();
    void UpdateAgingTextSize();
    void DumpInfo() override;

    void SetMessageColor(bool isSetMessageColor)
    {
        isSetMessageColor_ = isSetMessageColor;
    }

    void SetMessageNode(RefPtr<FrameNode> messageNode)
    {
        messageNode_ = messageNode;
    }

    void SetCustomPopupTag(bool isCustomPopup)
    {
        isCustomPopup_ = isCustomPopup;
    }

    void SetTransitionStatus(TransitionStatus transitionStatus)
    {
        transitionStatus_ = transitionStatus;
    }

    TransitionStatus GetTransitionStatus() const
    {
        return transitionStatus_;
    }

    void SetInteractiveDismiss(bool interactiveDismiss)
    {
        interactiveDismiss_ = interactiveDismiss;
    }

    bool GetInteractiveDismiss()
    {
        if (interactiveDismiss_) {
            return true;
        }
        return false;
    }

    void UpdateOnWillDismiss(const std::function<void(int32_t)>&& onWillDismiss)
    {
        onWillDismiss_ = std::move(onWillDismiss);
    }

    bool HasOnWillDismiss()
    {
        if (onWillDismiss_) {
            return true;
        }
        return false;
    }

    void CallOnWillDismiss(int32_t reason)
    {
        if (onWillDismiss_) {
            onWillDismiss_(reason);
        }
    }

    void SetEnableHoverMode(bool enableHoverMode)
    {
        enableHoverMode_ = enableHoverMode;
    }

    bool GetEnableHoverMode() const
    {
        return enableHoverMode_;
    }
    
    void SetHasTransition(bool hasTransition)
    {
        hasTransition_ = hasTransition;
    }

    bool GetHasTransition() const
    {
        return hasTransition_;
    }

    void ResetFocusState()
    {
        FocusViewDidShow(nullptr);
        SetIsViewRootScopeFocused(true);
        SetIsViewHasFocused(false);
    }

    Rect GetHostWindowRect() const
    {
        return hostWindowRect_;
    }

    void RegisterDoubleBindCallback(const std::function<void(const std::string&)>& callback)
    {
        doubleBindCallback_ = callback;
    }

    void CallDoubleBindCallback(const std::string& value)
    {
        if (doubleBindCallback_) {
            doubleBindCallback_(value);
        }
    }

    void SetPopupParam(const RefPtr<PopupParam>& popupParam)
    {
        popupParam_ = popupParam;
    }

    const RefPtr<PopupParam>& GetPopupParam() const
    {
        return popupParam_;
    }

    void SetCustomNode(const WeakPtr<UINode>& customNode)
    {
        customNode_ = customNode;
    }

    const RefPtr<UINode> GetCustomNode() const
    {
        return customNode_.Upgrade();
    }

protected:
    void OnDetachFromFrameNode(FrameNode* frameNode) override;

    bool AvoidKeyboard() const override
    {
        return false;
    }

    bool AvoidBottom() const override
    {
        return false;
    }

private:
    void OnModifyDone() override;
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool skipLayout) override;

    RefPtr<FrameNode> GetButtonRowNode();
    RefPtr<PopupTheme> GetPopupTheme();
    void InitTouchEvent();
    void HandleTouchEvent(const TouchEventInfo& info);
    void HandleTouchDown(const Offset& clickPosition);
    void RegisterButtonOnHover();
    void RegisterButtonOnTouch();
    void ButtonOnHover(bool isHover, const RefPtr<NG::FrameNode>& buttonNode);
    void ButtonOnPress(const TouchEventInfo& info, const RefPtr<NG::FrameNode>& buttonNode);
    void PopBubble();
    void Animation(
        RefPtr<RenderContext>& renderContext, const Color& endColor, int32_t duration, const RefPtr<Curve>& curve);

    OffsetT<Dimension> GetInvisibleOffset();
    RefPtr<RenderContext> GetRenderContext();
    void ResetToInvisible();
    bool PostTask(const TaskExecutor::Task& task, const std::string& name);
    void StartOffsetEnteringAnimation();
    void StartAlphaEnteringAnimation(std::function<void()> finish);
    void StartOffsetExitingAnimation();
    void StartAlphaExitingAnimation(std::function<void()> finish);

    int32_t targetNodeId_ = -1;
    std::string targetTag_;

    RefPtr<TouchEventImpl> touchEvent_;
    bool mouseEventInitFlag_ = false;
    bool touchEventInitFlag_ = false;
    bool isHover_ = false;
    bool interactiveDismiss_ = true;
    std::function<void(int32_t)> onWillDismiss_;
    OffsetF childOffset_;
    OffsetF arrowPosition_;
    SizeF childSize_;
    RectF touchRegion_;
    Rect hostWindowRect_;
    BubbleDumpInfo dumpInfo_;
    // top right bottom left
    std::vector<float> arrowOffsetByClips_ = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::optional<Placement> arrowPlacement_;
    std::vector<std::vector<float>> arrowOffsetsFromClip_
        = { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} };
    float arrowWidth_ = Dimension(16.0_vp).ConvertToPx();
    float arrowHeight_ = Dimension(8.0_vp).ConvertToPx();
    Placement arrowBuildPlacement_ = Placement::BOTTOM;

    bool showArrow_ = false;
    ColorMode colorMode_ = ColorMode::COLOR_MODE_UNDEFINED;
    bool isSetMessageColor_ = false;
    Border border_;

    TransitionStatus transitionStatus_ = TransitionStatus::INVISIABLE;

    bool delayShow_ = false;
    std::function<void()> finish_;

    std::optional<OffsetF> targetOffset_;
    std::optional<SizeF> targetSize_;

    bool isCustomPopup_ = false;
    RefPtr<FrameNode> messageNode_;

    std::string clipPath_;
    RefPtr<FrameNode> clipFrameNode_;
    ACE_DISALLOW_COPY_AND_MOVE(BubblePattern);

    bool hasTransition_ = false;
    bool hasOnAreaChange_ = false;
    bool enableHoverMode_ = false;
    int32_t halfFoldHoverCallbackId_ = -1;
    std::function<void(const std::string&)> onStateChangeCallback_ = nullptr;
    std::function<void(const std::string&)> doubleBindCallback_ = nullptr;
    RefPtr<PopupParam> popupParam_ = nullptr;
    WeakPtr<UINode> customNode_ = nullptr;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_BUBBLE_BUBBLE_PATTERN_H

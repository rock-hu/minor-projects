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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_PATTERN_H

#include <cstddef>
#include <optional>

#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/slider/slider_content_modifier.h"
#include "core/components_ng/pattern/slider/slider_event_hub.h"
#include "core/components_ng/pattern/slider/slider_layout_algorithm.h"
#include "core/components_ng/pattern/slider/slider_layout_property.h"
#include "core/components_ng/pattern/slider/slider_model_ng.h"
#include "core/components_ng/pattern/slider/slider_paint_method.h"
#include "core/components_ng/pattern/slider/slider_paint_property.h"
#include "core/components_ng/pattern/slider/slider_custom_content_options.h"

namespace OHOS::Ace::NG {
class SliderPattern : public Pattern {
    DECLARE_ACE_TYPE(SliderPattern, Pattern);

public:
    SliderPattern() = default;
    ~SliderPattern() override = default;

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        if (!IsSliderVisible()) {
            return nullptr;
        }
        auto paintParameters = UpdateContentParameters();
        if (!sliderContentModifier_) {
            sliderContentModifier_ = AceType::MakeRefPtr<SliderContentModifier>(
                paintParameters,
                [weak = WeakClaim(this)](float x) {
                    auto pattern = weak.Upgrade();
                    CHECK_NULL_VOID(pattern);
                    pattern->UpdateImagePositionX(x);
                },
                [weak = WeakClaim(this)](float y) {
                    auto pattern = weak.Upgrade();
                    CHECK_NULL_VOID(pattern);
                    pattern->UpdateImagePositionY(y);
                });
            sliderContentModifier_->SetHost(GetHost());
        }
        InitAccessibilityVirtualNodeTask();
        sliderContentModifier_->SetUseContentModifier(UseContentModifier());
        auto overlayGlobalOffset = CalculateGlobalSafeOffset();
        std::pair<OffsetF, float> BubbleVertex = GetBubbleVertexPosition(circleCenter_, trackThickness_, blockSize_);
        SliderPaintMethod::TipParameters tipParameters { bubbleFlag_, BubbleVertex.first, overlayGlobalOffset };

        auto textDirection = TextDirection::AUTO;
        auto layoutProperty = GetLayoutProperty<SliderLayoutProperty>();
        if (layoutProperty) {
            textDirection = layoutProperty->GetLayoutDirection();
        }
        if ((HasPrefix() || HasSuffix()) && !contentModifierNode_ && !endsInitFlag_) {
            endsInitFlag_ = true;
            InitSliderEndsState();
        }
        return MakeRefPtr<SliderPaintMethod>(sliderContentModifier_, paintParameters, sliderLength_, borderBlank_,
            sliderTipModifier_, tipParameters, textDirection);
    }

    void InitSliderEndsState()
    {
        CHECK_NULL_VOID(sliderContentModifier_);
        if (HasPrefix()) {
            CHECK_NULL_VOID(prefixNodeStack_);
            prefixSize_ = prefixNodeStack_->GetGeometryNode()->GetFrameSize();
            sliderContentModifier_->SetHasPrefix(true);
        }
        if (HasSuffix()) {
            CHECK_NULL_VOID(suffixNodeStack_);
            suffixSize_ = suffixNodeStack_->GetGeometryNode()->GetFrameSize();
            sliderContentModifier_->SetHasSuffix(true);
        }
        InitSliderEnds();
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<SliderLayoutProperty>();
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<SliderPaintProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<SliderLayoutAlgorithm>();
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override;

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<SliderEventHub>();
    }

    FocusPattern GetFocusPattern() const override
    {
        return { FocusType::NODE, true, FocusStyleType::CUSTOM_REGION };
    }

    const OffsetF& GetBlockCenter() const
    {
        return circleCenter_;
    }

    std::optional<OffsetF> GetAnimatableBlockCenter() const
    {
        if (sliderContentModifier_ != nullptr) {
            auto blockCenter = sliderContentModifier_->GetBlockCenter();
            return OffsetF(blockCenter.GetX(), blockCenter.GetY());
        }
        return std::nullopt;
    }

    float GetValueRatio() const
    {
        return valueRatio_;
    }

    std::string ProvideRestoreInfo() override;
    void OnRestoreInfo(const std::string& restoreInfo) override;
    OffsetF CalculateGlobalSafeOffset();
    void UpdateValue(float value);
    void OnWindowSizeChanged(int32_t width, int32_t height, WindowSizeChangeReason type) override;

    void SetBuilderFunc(SliderMakeCallback&& makeFunc)
    {
        if (makeFunc == nullptr) {
            makeFunc_ = std::nullopt;
            OnModifyDone();
            return;
        }
        makeFunc_ = std::move(makeFunc);
        if (sliderContentModifier_) {
            sliderContentModifier_->SetUseContentModifier(true);
        }
    }

    bool UseContentModifier()
    {
        return contentModifierNode_ != nullptr;
    }

    void SetEnableHapticFeedback(bool value)
    {
        isEnableHaptic_ = value;
    }

    bool GetEnableHapticFeedback() const
    {
        return isEnableHaptic_;
    }

    bool HasPrefix() const
    {
        if (contentModifierNode_) {
            return false;
        }
        return prefix_.Upgrade() != nullptr;
    };

    bool HasSuffix() const
    {
        if (contentModifierNode_) {
            return false;
        }
        return suffix_.Upgrade() != nullptr;
    };

    void SetSliderValue(double value, int32_t mode);
    void InitSliderEnds();
    void InitAccessibilityVirtualNodeTask();
    void SetIsAccessibilityOn(bool value)
    {
        isAccessibilityOn_ = value;
    }
    void PlayHapticFeedback(bool isShowSteps);
    bool OnThemeScopeUpdate(int32_t themeScopeId) override;
    void DumpInfo() override;

    void InitPrefixSuffixRow();
    void SetPrefix(const RefPtr<NG::UINode>& prefix, const NG::SliderPrefixOptions& options);
    void SetSuffix(const RefPtr<NG::UINode>& suffix, const NG::SliderSuffixOptions& options);
    void ResetPrefix();
    void ResetSuffix();
    void UpdatePrefixPosition();
    void UpdateSuffixPosition();
    void UpdateEndsIsShowStepsPosition(
        PointF& EndsPosition, PointF& block, SizeF& endsSize, float outsetoffset, bool side);
    void UpdateEndsNotShowStepsPosition(
        PointF& EndsPosition, PointF& block, SizeF& endsSize, float noneOffset, float outsetOffset);

#ifdef SUPPORT_DIGITAL_CROWN
    void SetDigitalCrownSensitivity(CrownSensitivity sensitivity)
    {
        crownSensitivity_ = sensitivity;
    }

    CrownSensitivity GetDigitalCrownSensitivity()
    {
        return crownSensitivity_;
    }
#endif
    void SetBubbleFlag(bool flag)
    {
        bubbleFlag_ = flag;
    }
    
    RefPtr<SliderContentModifier> GetSliderContentModifier() const
    {
        return sliderContentModifier_;
    }

    bool IsSliderVisible();
    SliderContentModifier::Parameters UpdateContentParameters();
    std::pair<OffsetF, float> GetBubbleVertexPosition(
        const OffsetF& blockCenter, float trackThickness, const SizeF& blockSize);
    const SizeF& GetBlockSize() const
    {
        return blockSize_;
    }

    float GetTrackThickness() const
    {
        return trackThickness_;
    }

    const bool& GetBubbleFlag() const
    {
        return bubbleFlag_;
    }
    void CreateTipToMountRoot();
    void RemoveTipFromRoot();
    void RefreshTipNode();
    void OnFinishEventTipSize();
    void CalculateOffset();
    void MountToNavigation(RefPtr<FrameNode>& tipNode);

private:
    void OnAttachToFrameNode() override;
    void OnDetachFromFrameNode(FrameNode* frameNode) override;
    void OnModifyDone() override;
    void CalcSliderValue();
    void CancelExceptionValue(float& min, float& max, float& step);
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool skipLayout) override;
    bool UpdateParameters();
    void CreateParagraphFunc();
    void CreateParagraphAndLayout(
        const TextStyle& textStyle, const std::string& content, const LayoutConstraintF& contentConstraint);
    bool CreateParagraph(const TextStyle& textStyle, std::string content);
    void UpdateCircleCenterOffset();
    void UpdateTipsValue();
    void UpdateBubbleSizeAndLayout();
    void UpdateBubble();
    void InitializeBubble();
    void UpdatePaintRect(RefPtr<SliderTheme> theme, SliderModel::SliderMode& sliderMode, RoundRect& paintRect,
        const RectF& rect, float rectRadius);

    bool AtMousePanArea(const Offset& offsetInFrame);
    bool AtTouchPanArea(const Offset& offsetInFrame);
    bool AtPanArea(const Offset& offset, const SourceType& sourceType);

    void UpdateMarkDirtyNode(const PropertyChangeFlag& Flag);
    Axis GetDirection() const;

    void InitClickEvent(const RefPtr<GestureEventHub>& gestureHub);
    void InitTouchEvent(const RefPtr<GestureEventHub>& gestureHub);
    void HandleTouchEvent(const TouchEventInfo& info);
    void HandleTouchDown(const Offset& location, SourceType sourceType);
    void HandleTouchUp(const Offset& location, SourceType sourceType);
    void InitMouseEvent(const RefPtr<InputEventHub>& inputEventHub);
    void HandleMouseEvent(const MouseInfo& info);
    void HandleHoverEvent(bool isHover);
    void HandleEnabled();
    void InitPanEvent(const RefPtr<GestureEventHub>& gestureHub);
    void HandlingGestureStart(const GestureEvent& info);
    void HandlingGestureEvent(const GestureEvent& info);
    void HandledGestureEvent();

    void UpdateValueByLocalLocation(const std::optional<Offset>& localLocation);
    void FireChangeEvent(int32_t mode);

    void InitOnKeyEvent(const RefPtr<FocusHub>& focusHub);
    void GetInnerFocusPaintRect(RoundRect& paintRect);
    void GetOutsetInnerFocusPaintRect(RoundRect& paintRect);
    void GetInsetAndNoneInnerFocusPaintRect(RoundRect& paintRect);
    bool OnKeyEvent(const KeyEvent& event);
    void PaintFocusState();
    bool MoveStep(int32_t stepCount);
#ifdef SUPPORT_DIGITAL_CROWN
    void InitDigitalCrownEvent(const RefPtr<FocusHub>& focusHub)
    {
        auto pipeline = GetContext();
        CHECK_NULL_VOID(pipeline);
        auto sliderTheme = pipeline->GetTheme<SliderTheme>();
        CHECK_NULL_VOID(sliderTheme);
        crownDisplayControlRatio_ = sliderTheme->GetCrownDisplayControlRatio();

        auto onCrownEvent = [weak = WeakClaim(this)](const CrownEvent& event) -> bool {
            auto pattern = weak.Upgrade();
            CHECK_NULL_RETURN(pattern, false);
            pattern->HandleCrownEvent(event);
            return true;
        };
        focusHub->SetOnCrownEventInternal(std::move(onCrownEvent));
    }
    void HandleCrownEvent(const CrownEvent& event)
    {
        TAG_LOGD(AceLogTag::ACE_SELECT_COMPONENT,
            "slider HandleCrownEvent event.action %{public}d event.degree %{public}f",
            event.action, event.degree);
        double mainDelta = GetCrownRotatePx(event);
        switch (event.action) {
            case CrownAction::UPDATE:
                if (!isHandleCrownActionBegin_) {
                    isHandleCrownActionBegin_ = true;
                    crownMovingLength_ = valueRatio_ * sliderLength_;
                    crownEventNum_ = 0;
                    reachBoundary_ = false;
                    HandleCrownAction(mainDelta);
                    timeStampPre_ = GetCurrentTimestamp();
                    UpdateMarkDirtyNode(PROPERTY_UPDATE_RENDER);
                    FireChangeEvent(SliderChangeMode::Begin);
                    OpenTranslateAnimation(SliderStatus::MOVE);
                } else {
                    HandleCrownAction(mainDelta);
                    StartVibrateFeedback();
                    UpdateMarkDirtyNode(PROPERTY_UPDATE_RENDER);
                    FireChangeEvent(SliderChangeMode::Moving);
                    OpenTranslateAnimation(SliderStatus::MOVE);
                }
                break;
            case CrownAction::END:
                isHandleCrownActionBegin_ = false;
                bubbleFlag_ = false;
                UpdateMarkDirtyNode(PROPERTY_UPDATE_RENDER);
                FireChangeEvent(SliderChangeMode::End);
                CloseTranslateAnimation();
                break;
            case CrownAction::BEGIN:
            default:
                break;
        }
    }
    double GetCrownRotatePx(const CrownEvent& event) const;
    void HandleCrownAction(double mainDelta);
    void StartVibrateFeedback();
#endif
    void RegisterVisibleAreaChange();
    void OnWindowHide() override;
    void OnWindowShow() override;
    void StartAnimation();
    void StopAnimation();

    void OpenTranslateAnimation(SliderStatus status);
    void CloseTranslateAnimation();

    void GetSelectPosition(SliderContentModifier::Parameters& parameters, float centerWidth, const OffsetF& offset);
    void GetBackgroundPosition(SliderContentModifier::Parameters& parameters, float centerWidth, const OffsetF& offset);
    void GetCirclePosition(SliderContentModifier::Parameters& parameters, float centerWidth, const OffsetF& offset);
    void UpdateBlock();
    void LayoutImageNode();
    void UpdateImagePositionX(float centerX);
    void UpdateImagePositionY(float centerY);

    void SetAccessibilityAction();
    void UpdateTipState();
    void OnIsFocusActiveUpdate(bool isFocusActive);
    void AddIsFocusActiveUpdateEvent();
    void RemoveIsFocusActiveUpdateEvent();
    bool isMinResponseExceed(const std::optional<Offset>& localLocation);
    void FireBuilder();
    RefPtr<FrameNode> BuildContentModifierNode();
    float GetValueInValidRange(const RefPtr<SliderPaintProperty>& paintProperty, float value, float min, float max);
    void UpdateToValidValue();
    void InitSliderAccessibilityEnabledRegister();
    void AccessibilityVirtualNodeRenderTask();
    bool CheckCreateAccessibilityVirtualNode();
    void InitAccessibilityHoverEvent();
    bool InitAccessibilityVirtualNode();
    void ModifyAccessibilityVirtualNode();
    void AddStepPointsAccessibilityVirtualNode();
    void UpdateStepAccessibilityVirtualNode();
    void UpdateParentNodeSize();
    std::string GetPointAccessibilityTxt(uint32_t pointIndex, float stepRatio, float min, float max);
    uint32_t GetCurrentStepIndex();
    int32_t GetOffsetStepIndex(uint32_t index);
    SizeF GetStepPointAccessibilityVirtualNodeSize();
    void UpdateStepPointsAccessibilityVirtualNodeSelected();
    void SetStepPointsAccessibilityVirtualNodeEvent(
        const RefPtr<FrameNode>& pointNode, uint32_t index, bool isClickAbled, bool reverse, bool isDisabledDesc);
    void SetStepPointAccessibilityVirtualNode(const RefPtr<FrameNode>& pointNode, const SizeF& size,
        const PointF& point, const std::string& txt, uint32_t index);
    void SendAccessibilityValueEvent(int32_t mode);
    void ClearSliderVirtualNode();
    void InitOrRefreshSlipFactor();
    RefPtr<PanEvent> CreatePanEvent();

    std::optional<SliderMakeCallback> makeFunc_;
    RefPtr<FrameNode> contentModifierNode_;
    void SetSkipGestureEvents()
    {
        skipGestureEvents_ = true;
    }
    void ResetSkipGestureEvents()
    {
        skipGestureEvents_ = false;
    }
    bool IsSkipGestureEvents()
    {
        return skipGestureEvents_;
    }
    void DumpSubInfo(RefPtr<SliderPaintProperty> paintProperty);

    Axis direction_ = Axis::HORIZONTAL;
    enum SliderChangeMode { Begin = 0, Moving = 1, End = 2, Click = 3 };
    float value_ = 0.0f;
    float minResponse_ = 0.0f;
    bool skipGestureEvents_ = false;
    float minResponseStartValue_ = value_;
    bool isMinResponseExceedFlag_ = false;
    SourceType eventSourceDevice_ = SourceType::NONE;
    Offset eventLocalLocation_ {};
    bool showTips_ = false;
    bool hotFlag_ = false; // whether the mouse is hovering over the slider
    bool valueChangeFlag_ = false;
    bool mouseHoverFlag_ = false;
    bool mousePressedFlag_ = false;
    bool axisFlag_ = false; // Wheel operation flag
    bool focusFlag_ = false;
    bool panMoveFlag_ = false;
    bool hasVisibleChangeRegistered_ = false;
    bool isVisibleArea_ = true;
    bool isShow_ = true;
    SliderModelNG::SliderInteraction sliderInteractionMode_ = SliderModelNG::SliderInteraction::SLIDE_AND_CLICK;
    bool allowDragEvents_ = true;
    int32_t fingerId_ = -1;
    std::optional<Offset> lastTouchLocation_ = std::nullopt;

    float valueRatio_ = 0.0f;
    float sliderLength_ = 0.0f;
    float borderBlank_ = 0.0f;
    float hotBlockShadowWidth_ = 0.0f;
    double axisOffset_ = 0.0;
    OffsetF circleCenter_ = { 0.0f, 0.0f }; // Relative to the content area

    float trackThickness_ = 0.0f;
    SizeF blockHotSize_;
    SizeF blockSize_;
#ifdef SUPPORT_DIGITAL_CROWN
    CrownSensitivity crownSensitivity_ = CrownSensitivity::MEDIUM;
    double crownDisplayControlRatio_ = 1.0;
    double crownMovingLength_ = 0.0;
    int32_t crownEventNum_ = 0;
    bool reachBoundary_ = false;
    int64_t timeStampCur_ = 0;
    int64_t timeStampPre_ = 0;
    bool isHandleCrownActionBegin_ = false;
#endif

    RefPtr<TouchEventImpl> touchEvent_;
    RefPtr<ClickEvent> clickListener_;
    RefPtr<PanEvent> panEvent_;
    RefPtr<InputEvent> mouseEvent_;
    RefPtr<InputEvent> hoverEvent_;
    RefPtr<FrameNode> parentPrefixSuffixNode_;
    std::vector<PointF> stepPoints_;
    PointF blockStart_ = { 0.0f, 0.0f };
    PointF blockEnd_ = { 0.0f, 0.0f };
    PointF prefixPosition_ = { 0.0f, 0.0f };
    PointF suffixPosition_ = { 0.0f, 0.0f };
    SizeF prefixSize_ = { 0.0f, 0.0f };
    SizeF suffixSize_ = { 0.0f, 0.0f };
    RSRect trackRect_;
    bool isShowSteps_ = false;
    bool side_ = true;
    bool endsInitFlag_ = false;
    float outsetOffset_ = 0.0f;

    NG::SliderPrefixOptions prefixAccessibilityoptions_;
    NG::SliderSuffixOptions suffixAccessibilityoptions_;
    WeakPtr<UINode> prefix_;
    WeakPtr<UINode> suffix_;
    RefPtr<FrameNode> prefixNodeStack_;
    RefPtr<FrameNode> suffixNodeStack_;

    RefPtr<SliderContentModifier> sliderContentModifier_;
    bool isTouchUpFlag_ = false;

    // tip Parameters
    bool bubbleFlag_ = false;
    RefPtr<SliderTipModifier> sliderTipModifier_;

    RefPtr<FrameNode> imageFrameNode_;
    std::function<void(bool)> isFocusActiveUpdateEvent_;
    bool isFocusActive_ = false;
    SliderModel::SliderMode sliderMode_ = SliderModel::SliderMode::OUTSET;
    bool isAccessibilityOn_ = AceApplicationInfo::GetInstance().IsAccessibilityEnabled();

    std::shared_ptr<AccessibilitySAObserverCallback> accessibilitySAObserverCallback_;
    RefPtr<FrameNode> parentAccessibilityNode_;
    std::vector<RefPtr<FrameNode>> pointAccessibilityNodeVec_;
    std::vector<GestureEventFunc> pointAccessibilityNodeEventVec_;
    bool isInitAccessibilityVirtualNode_ = false;
    uint64_t lastSendPostValueTime_ = 0;
    float accessibilityValue_ = 0.0f;
    bool isEnableHaptic_ = true;
    double slipfactor_ = 0;
    RefPtr<FrameNode> sliderTipNode_ = nullptr;
    RefPtr<UINode> navigationNode_ = nullptr;
    double xLastSlider_ = 0.0f;
    double yLastSlider_ = 0.0f;
    ACE_DISALLOW_COPY_AND_MOVE(SliderPattern);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_PATTERN_H

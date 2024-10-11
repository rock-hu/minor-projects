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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TABS_TAB_BAR_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TABS_TAB_BAR_PATTERN_H

#include <optional>
#include <unordered_map>

#include "base/geometry/axis.h"
#include "base/memory/referenced.h"
#include "core/components/common/layout/constants.h"
#include "core/components/swiper/swiper_controller.h"
#include "core/components/tab_bar/tab_theme.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/swiper/swiper_model.h"
#include "core/components_ng/pattern/tabs/tab_bar_accessibility_property.h"
#include "core/components_ng/pattern/tabs/tab_bar_layout_algorithm.h"
#include "core/components_ng/pattern/tabs/tab_bar_layout_property.h"
#include "core/components_ng/pattern/tabs/tab_bar_paint_method.h"
#include "core/components_ng/pattern/tabs/tab_bar_paint_property.h"
#include "core/components_ng/pattern/tabs/tab_content_model.h"
#include "core/event/mouse_event.h"
#include "core/components_ng/pattern/tabs/tab_content_transition_proxy.h"
#include "frameworks/core/components/focus_animation/focus_animation_theme.h"
#include "frameworks/core/components_ng/event/focus_hub.h"

namespace OHOS::Ace::NG {
class InspectorFilter;

const auto TabBarPhysicalCurve = AceType::MakeRefPtr<InterpolatingSpring>(-1.0f, 1.0f, 228.0f, 30.f);

using TabBarBuilderFunc = std::function<void()>;
class TabBarParam : public virtual Referenced {
public:
    TabBarParam(const std::string& textParam, const std::string& iconParam, TabBarBuilderFunc&& builderParam)
        : text_(textParam), icon_(iconParam), builder_(std::move(builderParam)) {};

    const std::string& GetIcon() const
    {
        return icon_;
    }

    void SetIcon(const std::string& icon)
    {
        icon_ = icon;
    }

    const std::string& GetText() const
    {
        return text_;
    }

    void SetText(const std::string& text)
    {
        text_ = text;
    }

    const std::optional<TabBarSymbol>& GetSymbol() const
    {
        return symbol_;
    }

    void SetSymbol(const std::optional<TabBarSymbol>& symbol)
    {
        symbol_ = symbol;
    }

    bool HasBuilder() const
    {
        return builder_ != nullptr;
    }

    void SetBuilder(TabBarBuilderFunc&& builderParam)
    {
        builder_ = std::move(builderParam);
    }

    void ExecuteBuilder() const
    {
        if (builder_ != nullptr) {
            builder_();
        }
    }

    void SetTabBarStyle(TabBarStyle tabBarStyle)
    {
        tabBarStyle_ = tabBarStyle;
    }

    TabBarStyle GetTabBarStyle() const
    {
        return tabBarStyle_;
    }

    void SetCustomNode(FrameNode* node)
    {
        node_ = node;
    }

private:
    std::string text_;
    std::string icon_;
    std::optional<TabBarSymbol> symbol_;
    TabBarBuilderFunc builder_;
    TabBarStyle tabBarStyle_ = TabBarStyle::NOSTYLE;
    FrameNode* node_ = nullptr;
};

enum class AnimationType {
    PRESS = 0,
    HOVER,
    HOVERTOPRESS,
};

class TabBarPattern : public Pattern {
    DECLARE_ACE_TYPE(TabBarPattern, Pattern);

public:
    explicit TabBarPattern(const RefPtr<SwiperController>& swiperController);
    ~TabBarPattern() override = default;

    bool IsAtomicNode() const override
    {
        return false;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<TabBarLayoutProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        auto layoutAlgorithm = MakeRefPtr<TabBarLayoutAlgorithm>();
        layoutAlgorithm->SetCurrentDelta(currentDelta_);
        layoutAlgorithm->SetTabBarStyle(tabBarStyle_);
        if (targetIndex_) {
            layoutAlgorithm->SetTargetIndex(targetIndex_);
        } else if (jumpIndex_) {
            layoutAlgorithm->SetJumpIndex(jumpIndex_);
        }
        layoutAlgorithm->SetVisibleItemPosition(visibleItemPosition_);
        layoutAlgorithm->SetCanOverScroll(canOverScroll_);
        layoutAlgorithm->SetLastFontScale(lastFontScale_);
        return layoutAlgorithm;
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<TabBarPaintProperty>();
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override;

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<TabBarAccessibilityProperty>();
    }

    FocusPattern GetFocusPattern() const override
    {
        FocusPaintParam focusPaintParams;
        auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
        CHECK_NULL_RETURN(pipeline, FocusPattern());
        auto focusTheme = pipeline->GetTheme<FocusAnimationTheme>();
        CHECK_NULL_RETURN(focusTheme, FocusPattern());
        auto tabTheme = pipeline->GetTheme<TabTheme>();
        CHECK_NULL_RETURN(tabTheme, FocusPattern());
        focusPaintParams.SetPaintWidth(tabTheme->GetActiveIndicatorWidth());
        focusPaintParams.SetPaintColor(focusTheme->GetColor());
        return { FocusType::NODE, true, FocusStyleType::CUSTOM_REGION, focusPaintParams };
    }

    void SetIndicator(int32_t indicator)
    {
        indicator_ = indicator;
    }

    void OnTabBarIndexChange(int32_t index);

    void UpdateCurrentOffset(float offset);

    void UpdateIndicator(int32_t indicator);

    void UpdateTextColorAndFontWeight(int32_t indicator);

    void UpdateImageColor(int32_t indicator);

    void UpdateSymbolStats(int32_t index, int32_t preIndex);

    void UpdateSymbolEffect(int32_t index);

    void UpdateSubTabBoard(int32_t index);

    SelectedMode GetSelectedMode() const;

    void AddTabBarItemType(int32_t tabBarItemId, bool isBuilder)
    {
        tabBarType_.emplace(std::make_pair(tabBarItemId, isBuilder));
    }

    bool IsContainsBuilder();

    void SetAnimationDuration(int32_t animationDuration)
    {
        animationDuration_ = animationDuration;
    }

    void SetTouching(bool isTouching)
    {
        touching_ = isTouching;
    }

    bool IsTouching() const
    {
        return touching_;
    }

    void SetTabBarStyle(TabBarStyle tabBarStyle)
    {
        tabBarStyle_ = tabBarStyle;
        InitLongPressAndDragEvent();
    }

    TabBarStyle GetTabBarStyle() const
    {
        return tabBarStyle_;
    }

    void TriggerTranslateAnimation(int32_t currentIndex, int32_t targetIndex);

    void HandleBottomTabBarChange(int32_t index);

    bool GetChangeByClick() const
    {
        return changeByClick_;
    }

    void SetChangeByClick(bool changeByClick)
    {
        changeByClick_ = changeByClick;
    }

    bool GetClickRepeat() const
    {
        return clickRepeat_;
    }

    void SetClickRepeat(bool clickRepeat)
    {
        clickRepeat_ = clickRepeat;
    }

    void SetSelectedMode(SelectedMode selectedMode, uint32_t position)
    {
        if (selectedModes_.size() <= position) {
            selectedModes_.emplace_back(selectedMode);
        } else {
            selectedModes_[position] = selectedMode;
        }
    }

    void SetIndicatorStyle(const IndicatorStyle& indicatorStyle, uint32_t position)
    {
        if (indicatorStyles_.size() <= position) {
            indicatorStyles_.emplace_back(indicatorStyle);
        } else {
            indicatorStyles_[position] = indicatorStyle;
        }
    }

    void SetTabBarStyle(TabBarStyle tabBarStyle, uint32_t position)
    {
        if (tabBarStyles_.size() <= position) {
            tabBarStyles_.emplace_back(tabBarStyle);
        } else {
            tabBarStyles_[position] = tabBarStyle;
        }
    }

    void SetBottomTabBarStyle(const BottomTabBarStyle& bottomTabBarStyle, uint32_t position)
    {
        if (bottomTabBarStyles_.size() <= position) {
            bottomTabBarStyles_.emplace_back(bottomTabBarStyle);
        } else {
            bottomTabBarStyles_[position] = bottomTabBarStyle;
        }
    }

    void SetLabelStyle(int32_t tabBarItemId, const LabelStyle& labelStyle)
    {
        labelStyles_[tabBarItemId] = labelStyle;
    }

    void SetIconStyle(const IconStyle& iconStyle, uint32_t position)
    {
        if (iconStyles_.size() <= position) {
            iconStyles_.emplace_back(iconStyle);
        } else {
            iconStyles_[position] = iconStyle;
        }
    }

    std::vector<IconStyle> GetIconStyle()
    {
        return iconStyles_;
    }

    void SetSymbol(const TabBarSymbol& symbol, uint32_t position)
    {
        if (symbolArray_.size() <= position) {
            symbolArray_.emplace_back(symbol);
        } else {
            symbolArray_[position] = symbol;
        }
    }

    std::vector<TabBarSymbol> GetSymbol()
    {
        return symbolArray_;
    }

    bool IsMaskAnimationByCreate()
    {
        return isMaskAnimationByCreate_;
    }

    void SetMaskAnimationByCreate(bool isMaskAnimationByCreate)
    {
        isMaskAnimationByCreate_ = isMaskAnimationByCreate;
    }

    bool IsMaskAnimationExecuted()
    {
        return isMaskAnimationExecuted_;
    }

    void SetMaskAnimationExecuted(bool isMaskAnimationExecuted)
    {
        isMaskAnimationExecuted_ = isMaskAnimationExecuted;
    }

    void SetImageColorOnIndex(int32_t index)
    {
        imageColorOnIndex_ = index;
    }

    std::optional<int32_t> GetImageColorOnIndex()
    {
        return imageColorOnIndex_;
    }

    int32_t GetIndicator()
    {
        return indicator_;
    }

    bool IsAtTop() const;

    bool IsAtBottom() const;
    std::string ProvideRestoreInfo() override;
    void OnRestoreInfo(const std::string& restoreInfo) override;

    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;
    void FromJson(const std::unique_ptr<JsonValue>& json) override;

    void SetFirstFocus(bool isFirstFocus)
    {
        isFirstFocus_ = isFirstFocus;
    }

    void ResetIndicatorAnimationState()
    {
        isAnimating_ = false;
        animationTargetIndex_.reset();
    }

    bool GetTouchingSwiper() const
    {
        return isTouchingSwiper_;
    }

    TabBarStyle GetTabBarStyle(uint32_t position) const
    {
        if (position < 0 || position >= tabBarStyles_.size()) {
            return TabBarStyle::NOSTYLE;
        }
        return tabBarStyles_[position];
    }

    const BottomTabBarStyle& GetBottomTabBarStyle(uint32_t position) const
    {
        if (position < 0 || position >= bottomTabBarStyles_.size()) {
            return bottomTabBarStyle_;
        }
        return bottomTabBarStyles_[position];
    }

    LabelStyle GetBottomTabLabelStyle(int32_t tabBarItemId) const
    {
        auto iter = labelStyles_.find(tabBarItemId);
        if (iter == labelStyles_.end()) {
            LabelStyle labelStyle{};
            return labelStyle;
        }
        return iter->second;
    }

    void DumpAdvanceInfo() override;
    void DumpAdvanceInfo(std::unique_ptr<JsonValue>& json) override;
    void SetRegionInfo(std::unique_ptr<JsonValue>& json);

    std::optional<int32_t> GetAnimationDuration()
    {
        return animationDuration_;
    }

    bool GetTabContentWillChangeFlag()
    {
        return tabContentWillChangeFlag_;
    }

    void ResetTabContentWillChangeFlag()
    {
        tabContentWillChangeFlag_ = false;
    }

    void UpdateAnimationDuration();

    bool HasSurfaceChangedCallback()
    {
        return surfaceChangedCallbackId_.has_value();
    }

    void UpdateSurfaceChangedCallbackId(int32_t id)
    {
        surfaceChangedCallbackId_ = id;
    }

    bool ContentWillChange(int32_t comingIndex);
    bool ContentWillChange(int32_t currentIndex, int32_t comingIndex);

    void AddTabBarItemClickEvent(const RefPtr<FrameNode>& tabBarItem);

    void RemoveTabBarItemInfo(int32_t tabBarItemId)
    {
        clickEvents_.erase(tabBarItemId);
        labelStyles_.erase(tabBarItemId);
    }

    std::optional<float> GetThirdLargeFontHeight()
    {
        return thirdLargeFontHeight_;
    }

    void SetThirdLargeFontHeight(std::optional<float> thirdLargeFontHeight)
    {
        if (thirdLargeFontHeight.has_value()) {
            thirdLargeFontHeight_ = thirdLargeFontHeight;
        } else {
            thirdLargeFontHeight_.reset();
        }
    }

private:
    void OnModifyDone() override;
    void OnAttachToFrameNode() override;
    void OnDetachFromFrameNode(FrameNode* node) override;
    void BeforeCreateLayoutWrapper() override;
    void InitSurfaceChangedCallback();
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;

    void InitLongPressEvent(const RefPtr<GestureEventHub>& gestureHub);
    void InitDragEvent(const RefPtr<GestureEventHub>& gestureHub);
    void InitScrollable(const RefPtr<GestureEventHub>& gestureHub);
    void InitTouch(const RefPtr<GestureEventHub>& gestureHub);
    void InitHoverEvent();
    void InitMouseEvent();

    void HandleMouseEvent(const MouseInfo& info);
    void HandleHoverEvent(bool isHover);
    void HandleHoverOnEvent(int32_t index);
    void HandleMoveAway(int32_t index);
    void InitOnKeyEvent(const RefPtr<FocusHub>& focusHub);
    bool OnKeyEvent(const KeyEvent& event);
    bool OnKeyEventWithoutClick(const KeyEvent& event);
    bool OnKeyEventWithoutClick(const RefPtr<FrameNode>& host, const KeyEvent& event);
    void HandleLongPressEvent(const GestureEvent& info);
    void ShowDialogWithNode(int32_t index);
    void CloseDialog();
    void InitLongPressAndDragEvent();
    void HandleClick(const GestureEvent& info, int32_t index);
    void ClickTo(const RefPtr<FrameNode>& host, int32_t index);
    void HandleTouchEvent(const TouchLocationInfo& info);
    void HandleSubTabBarClick(const RefPtr<TabBarLayoutProperty>& layoutProperty, int32_t index);
    void HandleBottomTabBarClick(int32_t selectedIndex, int32_t unselectedIndex);
    void ChangeMask(int32_t index, float imageSize, const OffsetF& originalMaskOffset, float opacity,
        float radiusRatio, bool isSelected);
    void PlayMaskAnimation(float selectedImageSize, const OffsetF& originalSelectedMaskOffset, int32_t selectedIndex,
        float unselectedImageSize, const OffsetF& originalUnselectedMaskOffset, int32_t unselectedIndex);
    static void MaskAnimationFinish(const RefPtr<FrameNode>& host, int32_t selectedIndex, bool isSelected);
    void GetBottomTabBarImageSizeAndOffset(const std::vector<int32_t>& selectedIndexes,
        int32_t maskIndex, float& selectedImageSize, float& unselectedImageSize, OffsetF& originalSelectedMaskOffset,
        OffsetF& originalUnselectedMaskOffset);
    void UpdateBottomTabBarImageColor(const std::vector<int32_t>& selectedIndexes, int32_t maskIndex);
    void UpdateSymbolApply(const RefPtr<NG::FrameNode>& symbolNode, RefPtr<TextLayoutProperty>& symbolProperty,
        int32_t index, std::string type);
    bool CheckSvg(int32_t index) const;

    void HandleTouchDown(int32_t index);
    void HandleTouchUp(int32_t index);
    int32_t CalculateSelectedIndex(const Offset& info);

    void PlayPressAnimation(int32_t index, const Color& pressColor, AnimationType animationType);
    void PlayTabBarTranslateAnimation(AnimationOption option, float targetCurrentOffset);
    void PlayIndicatorTranslateAnimation(AnimationOption option, RectF originalPaintRect, RectF targetPaintRect,
        float targetOffset);
    void CreateIndicatorTranslateProperty(const RefPtr<FrameNode>& host, const std::string& propertyName);
    void StopTranslateAnimation();
    float CalculateTargetOffset(int32_t targetIndex);
    void UpdateIndicatorCurrentOffset(float offset);

    void GetInnerFocusPaintRect(RoundRect& paintRect);
    void PaintFocusState(bool needMarkDirty = true);
    void FocusIndexChange(int32_t index);
    void UpdateGradientRegions(bool needMarkDirty = true);

    float GetSpace(int32_t indicator);
    float CalculateFrontChildrenMainSize(int32_t indicator);
    float CalculateBackChildrenMainSize(int32_t indicator);
    void SetEdgeEffect(const RefPtr<GestureEventHub>& gestureHub);
    void SetEdgeEffectCallback(const RefPtr<ScrollEdgeEffect>& scrollEffect);
    bool IsOutOfBoundary();
    void SetAccessibilityAction();
    void TabBarClickEvent(int32_t index) const;
    void OnCustomContentTransition(int32_t fromIndex, int32_t toIndex);
    void ApplyTurnPageRateToIndicator(float turnPageRate);
    bool CheckSwiperDisable() const;
    void SetSwiperCurve(const RefPtr<Curve>& curve) const;
    void InitTurnPageRateEvent();
    void GetIndicatorStyle(IndicatorStyle& indicatorStyle, OffsetF& indicatorOffset);
    void CalculateIndicatorStyle(
        int32_t startIndex, int32_t nextIndex, IndicatorStyle& indicatorStyle, OffsetF& indicatorOffset);
    Color GetTabBarBackgroundColor() const;
    float GetLeftPadding() const;
    void HandleBottomTabBarAnimation(int32_t index);
    void UpdatePaintIndicator(int32_t indicator, bool needMarkDirty);
    std::pair<float, float> GetOverScrollInfo(const SizeF& size);
    void RemoveTabBarEventCallback();
    void AddTabBarEventCallback();
    void AddMaskItemClickEvent();
    bool CanScroll() const;
    bool ParseTabsIsRtl();
    bool IsValidIndex(int32_t index);
    int32_t GetLoopIndex(int32_t originalIndex) const;

    void StartShowTabBar(int32_t delay);
    void StopShowTabBar();
    void InitTabBarProperty();
    void UpdateTabBarHiddenRatio(float ratio);
    void SetTabBarTranslate(const TranslateOptions& options);
    void SetTabBarOpacity(float opacity);

    RefPtr<NodeAnimatablePropertyFloat> showTabBarProperty_;
    bool isTabBarShowing_ = false;

    std::map<int32_t, RefPtr<ClickEvent>> clickEvents_;
    RefPtr<LongPressEvent> longPressEvent_;
    RefPtr<TouchEventImpl> touchEvent_;
    RefPtr<ScrollableEvent> scrollableEvent_;
    RefPtr<InputEvent> mouseEvent_;
    RefPtr<InputEvent> hoverEvent_;
    RefPtr<SwiperController> swiperController_;
    RefPtr<ScrollEdgeEffect> scrollEffect_;
    RefPtr<FrameNode> dialogNode_;
    RefPtr<DragEvent> dragEvent_;
    AnimationStartEventPtr animationStartEvent_;
    AnimationEndEventPtr animationEndEvent_;

    float bigScale_ = 0.0f;
    float largeScale_ = 0.0f;
    float maxScale_ = 0.0f;
    int32_t indicator_ = 0;
    int32_t focusIndicator_ = 0;
    Axis axis_ = Axis::HORIZONTAL;
    std::unordered_map<int32_t, bool> tabBarType_;
    std::optional<int32_t> animationDuration_;

    std::shared_ptr<AnimationUtils::Animation> tabbarIndicatorAnimation_;
    std::shared_ptr<AnimationUtils::Animation> translateAnimation_;
    std::shared_ptr<AnimationUtils::Animation> maskAnimation_;

    bool indicatorAnimationIsRunning_ = false;
    bool translateAnimationIsRunning_ = false;

    bool isRTL_ = false;

    bool touching_ = false; // whether the item is in touching
    bool isHover_ = false;
    bool isMaskAnimationByCreate_ = false;
    bool isMaskAnimationExecuted_ = false;
    bool tabContentWillChangeFlag_ = false;
    std::optional<int32_t> imageColorOnIndex_;
    std::optional<int32_t> touchingIndex_;
    std::optional<int32_t> hoverIndex_;
    std::optional<int32_t> moveIndex_;
    TabBarStyle tabBarStyle_ = TabBarStyle::NOSTYLE;
    float currentIndicatorOffset_ = 0.0f;
    std::vector<SelectedMode> selectedModes_;
    std::vector<IndicatorStyle> indicatorStyles_;
    std::vector<TabBarStyle> tabBarStyles_;
    std::unordered_map<int32_t, LabelStyle> labelStyles_;
    std::vector<IconStyle> iconStyles_;
    std::vector<TabBarSymbol> symbolArray_;
    bool isFirstFocus_ = true;
    bool isTouchingSwiper_ = false;
    float indicatorStartPos_ = 0.0f;
    float indicatorEndPos_ = 0.0f;
    float turnPageRate_ = 0.0f;
    int32_t swiperStartIndex_ = 0;
    std::vector<BottomTabBarStyle> bottomTabBarStyles_;
    BottomTabBarStyle bottomTabBarStyle_;

    RefPtr<TabBarModifier> tabBarModifier_;
    std::vector<bool> gradientRegions_ = {false, false, false, false};
    bool isAnimating_ = false;
    bool changeByClick_ = false;
    bool clickRepeat_ = false;
    float scrollMargin_ = 0.0f;
    bool isFirstLayout_ = true;
    std::optional<int32_t> animationTargetIndex_;
    std::optional<int32_t> surfaceChangedCallbackId_;
    std::optional<WindowSizeChangeReason> windowSizeChangeReason_;
    std::pair<double, double> prevRootSize_;

    std::optional<int32_t> jumpIndex_;
    std::optional<int32_t> targetIndex_;
    float currentDelta_ = 0.0f;
    float currentOffset_ = 0.0f;
    std::map<int32_t, ItemInfo> visibleItemPosition_;
    bool canOverScroll_ = false;
    float lastFontScale_ = 0.0f;
    std::optional<float> thirdLargeFontHeight_;
    ACE_DISALLOW_COPY_AND_MOVE(TabBarPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TABS_TAB_BAR_PATTERN_H

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "core/components_ng/pattern/picker_utils/picker_column_pattern.h"

#include "base/utils/measure_util.h"
#include "core/components_ng/pattern/picker_utils/toss_animation_controller.h"
#include "core/components_ng/pattern/text/text_pattern.h"
namespace OHOS::Ace::NG {
namespace {

const float MOVE_DISTANCE = 5.0f;
const int32_t OPTION_COUNT_PHONE_LANDSCAPE = 3;
constexpr int32_t HOVER_ANIMATION_DURATION = 250;
constexpr int32_t PRESS_ANIMATION_DURATION = 100;
constexpr int32_t CLICK_ANIMATION_DURATION = 300;
constexpr int32_t HOT_ZONE_HEIGHT_CANDIDATE = 2;
constexpr int32_t HOT_ZONE_HEIGHT_DISAPPEAR = 4;
constexpr int32_t MIDDLE_CHILD_INDEX = 2;
constexpr float FONT_SIZE_PERCENT = 1.0f;
constexpr float FONT_SIZE_PERCENT_50 = 0.5f;
constexpr int32_t HALF_NUMBER = 2;
constexpr char PICKER_DRAG_SCENE[] = "picker_drag_scene";
constexpr char MEASURE_SIZE_STRING[] = "TEST";
const Dimension FONT_SIZE = Dimension(2.0);
const Dimension FOCUS_SIZE = Dimension(1.0);
} // namespace

void PickerColumnPattern::OnAttachToFrameNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContextRefPtr();
    CHECK_NULL_VOID(context);
    auto pickerTheme = context->GetTheme<PickerTheme>();
    CHECK_NULL_VOID(pickerTheme);
    auto hub = host->GetOrCreateEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    tossAnimationController_->SetPipelineContext(context);
    tossAnimationController_->SetColumn(AceType::WeakClaim(this));
    jumpInterval_ = pickerTheme->GetJumpInterval().ConvertToPx();
    CreateAnimation();
    InitPanEvent(gestureHub);
    host->GetRenderContext()->SetClipToFrame(true);
    InitHapticController(host);
    RegisterWindowStateChangedCallback();
}

void PickerColumnPattern::OnDetachFromFrameNode(FrameNode* frameNode)
{
    isTossPlaying_ = false;
    if (hapticController_) {
        hapticController_->Stop();
    }
    UnregisterWindowStateChangedCallback(frameNode);
}

void PickerColumnPattern::RegisterWindowStateChangedCallback()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    pipeline->AddWindowStateChangedCallback(host->GetId());
}

void PickerColumnPattern::UnregisterWindowStateChangedCallback(FrameNode* frameNode)
{
    CHECK_NULL_VOID(frameNode);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    pipeline->RemoveWindowStateChangedCallback(frameNode->GetId());
}

void PickerColumnPattern::OnWindowHide()
{
    isShow_ = false;
    if (hapticController_) {
        hapticController_->Stop();
    }
}

void PickerColumnPattern::OnWindowShow()
{
    isShow_ = true;
}

void PickerColumnPattern::StopHaptic()
{
    stopHaptic_ = true;
}

void PickerColumnPattern::ParseTouchListener()
{
    auto touchCallback = [weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (info.GetTouches().empty()) {
            return;
        }
        if (info.GetTouches().front().GetTouchType() == TouchType::DOWN) {
            pattern->SetLocalDownDistance(info.GetTouches().front().GetLocalLocation().GetDistance());
            pattern->OnTouchDown();
        }
        if (info.GetTouches().front().GetTouchType() == TouchType::UP ||
            info.GetTouches().front().GetTouchType() == TouchType::CANCEL) {
            pattern->OnTouchUp();
            pattern->SetLocalDownDistance(0.0f);
        }
        if (info.GetTouches().front().GetTouchType() == TouchType::MOVE) {
            if (std::abs(info.GetTouches().front().GetLocalLocation().GetDistance() - pattern->GetLocalDownDistance()) >
                MOVE_DISTANCE) {
                pattern->OnTouchUp();
            }
        }
    };
    touchListener_ = MakeRefPtr<TouchEventImpl>(std::move(touchCallback));
}

void PickerColumnPattern::ParseMouseEvent()
{
    auto mouseTask = [weak = WeakClaim(this)](bool isHover) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleMouseEvent(isHover);
    };
    mouseEvent_ = MakeRefPtr<InputEvent>(std::move(mouseTask));
}

void PickerColumnPattern::InitMouseAndPressEvent()
{
    if (mouseEvent_ || touchListener_) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto columnEventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(columnEventHub);
    RefPtr<TouchEventImpl> touchListener = CreateItemTouchEventListener();
    CHECK_NULL_VOID(touchListener);
    auto columnGesture = columnEventHub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(columnGesture);
    columnGesture->AddTouchEvent(touchListener);
    auto childSize = static_cast<int32_t>(host->GetChildren().size());
    RefPtr<FrameNode> middleChild = nullptr;
    auto midSize = childSize / 2;
    middleChild = DynamicCast<FrameNode>(host->GetChildAtIndex(midSize));
    CHECK_NULL_VOID(middleChild);
    auto eventHub = middleChild->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto inputHub = eventHub->GetOrCreateInputEventHub();
    ParseMouseEvent();
    inputHub->AddOnHoverEvent(mouseEvent_);
    auto gesture = middleChild->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gesture);
    ParseTouchListener();
    gesture->AddTouchEvent(touchListener_);
    for (int32_t i = 0; i < childSize; i++) {
        RefPtr<FrameNode> childNode = DynamicCast<FrameNode>(host->GetChildAtIndex(i));
        CHECK_NULL_VOID(childNode);
        RefPtr<PickerEventParam> param = MakeRefPtr<PickerEventParam>();
        param->instance_ = childNode;
        param->itemIndex_ = i;
        param->itemTotalCounts_ = childSize;
        auto eventHub = childNode->GetEventHub<EventHub>();
        CHECK_NULL_VOID(eventHub);
        if (i != midSize) {
            RefPtr<ClickEvent> clickListener = CreateItemClickEventListener(param);
            CHECK_NULL_VOID(clickListener);
            auto gesture = eventHub->GetOrCreateGestureEventHub();
            CHECK_NULL_VOID(gesture);
            gesture->AddClickEvent(clickListener);
        }
    }
}

RefPtr<TouchEventImpl> PickerColumnPattern::CreateItemTouchEventListener()
{
    auto toss = GetToss();
    CHECK_NULL_RETURN(toss, nullptr);
    auto touchCallback = [weak = WeakClaim(this), toss](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (info.GetSourceTool() == SourceTool::MOUSE) {
            pattern->stopHaptic_ = true;
        } else {
            pattern->stopHaptic_ = false;
        }
        auto isToss = pattern->GetTossStatus();
        if (info.GetTouches().empty()) {
            return;
        }
        if (info.GetTouches().front().GetTouchType() == TouchType::DOWN) {
            if (isToss == true) {
                pattern->touchBreak_ = true;
                pattern->animationBreak_ = true;
                pattern->clickBreak_ = true;
                auto TossEndPosition = toss->GetTossEndPosition();
                pattern->SetYLast(TossEndPosition);
                toss->StopTossAnimation();
            } else {
                pattern->animationBreak_ = false;
                pattern->clickBreak_ = false;
            }
        }
        if (info.GetTouches().front().GetTouchType() == TouchType::UP) {
            pattern->touchBreak_ = false;
            if (pattern->animationBreak_ == true) {
                pattern->PlayRestAnimation();
                pattern->yOffset_ = 0.0;
            }
        }
    };
    auto listener = MakeRefPtr<TouchEventImpl>(std::move(touchCallback));
    return listener;
}

void PickerColumnPattern::HandleMouseEvent(bool isHover)
{
    if (isHover) {
        hovered_ = true;
        PlayHoverAnimation(GetButtonHoverColor());
    } else {
        hovered_ = false;
        PlayHoverAnimation(GetButtonBgColor());
    }
}

void PickerColumnPattern::OnTouchDown()
{
    SetSelectedMark();
    PlayPressAnimation(GetButtonPressColor());
}

void PickerColumnPattern::OnTouchUp()
{
    if (hovered_) {
        PlayPressAnimation(GetButtonHoverColor());
    } else {
        PlayPressAnimation(GetButtonBgColor());
    }
}

void PickerColumnPattern::SetButtonBackgroundColor(const Color& pressColor)
{
    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();
    CHECK_NULL_VOID(pickerTheme);
    CHECK_EQUAL_VOID(pickerTheme->IsCircleDial(), true);

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto blend = host->GetParent();
    CHECK_NULL_VOID(blend);
    auto stack = blend->GetParent();
    CHECK_NULL_VOID(stack);
    auto buttonNode = DynamicCast<FrameNode>(stack->GetFirstChild());
    auto renderContext = buttonNode->GetRenderContext();
    renderContext->UpdateBackgroundColor(pressColor);
    buttonNode->MarkModifyDone();
    buttonNode->MarkDirtyNode();
}

void PickerColumnPattern::PlayPressAnimation(const Color& pressColor)
{
    AnimationOption option = AnimationOption();
    option.SetDuration(PRESS_ANIMATION_DURATION);
    option.SetCurve(Curves::SHARP);
    option.SetFillMode(FillMode::FORWARDS);
    AnimationUtils::Animate(option, [weak = AceType::WeakClaim(this), pressColor]() {
        auto picker = weak.Upgrade();
        CHECK_NULL_VOID(picker);
        picker->SetButtonBackgroundColor(pressColor);
    });
}

void PickerColumnPattern::PlayHoverAnimation(const Color& color)
{
    AnimationOption option = AnimationOption();
    option.SetDuration(HOVER_ANIMATION_DURATION);
    option.SetCurve(Curves::FRICTION);
    option.SetFillMode(FillMode::FORWARDS);
    AnimationUtils::Animate(option, [weak = AceType::WeakClaim(this), color]() {
        auto picker = weak.Upgrade();
        CHECK_NULL_VOID(picker);
        picker->SetButtonBackgroundColor(color);
    });
}

bool PickerColumnPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    bool isChange =
        config.frameSizeChange || config.frameOffsetChange || config.contentSizeChange || config.contentOffsetChange;

    CHECK_NULL_RETURN(isChange, false);
    CHECK_NULL_RETURN(dirty, false);
    auto geometryNode = dirty->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, false);
    auto offset = geometryNode->GetFrameOffset();
    auto size = geometryNode->GetFrameSize();
    if (!NearEqual(offset, offset_) || !NearEqual(size, size_)) {
        offset_ = offset;
        size_ = size;
        AddHotZoneRectToText();
    }
    return true;
}

void PickerColumnPattern::UpdateTextPropertiesLinear(bool isDown, double scale)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    uint32_t showCount = GetShowCount();
    auto child = host->GetChildren();
    auto iter = child.begin();
    if (child.size() != showCount) {
        return;
    }
    for (uint32_t index = 0; index < showCount; index++) {
        auto textNode = DynamicCast<FrameNode>(*iter);
        CHECK_NULL_VOID(textNode);
        auto textPattern = textNode->GetPattern<TextPattern>();
        CHECK_NULL_VOID(textPattern);
        RefPtr<TextLayoutProperty> textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        TextPropertiesLinearAnimation(textLayoutProperty, index, showCount, isDown, scale);
        iter++;
    }
}

Dimension PickerColumnPattern::LinearFontSize(
    const Dimension& startFontSize, const Dimension& endFontSize, double percent)
{
    if (percent > FONT_SIZE_PERCENT) {
        return startFontSize + (endFontSize - startFontSize);
    } else {
        return startFontSize + (endFontSize - startFontSize) * percent;
    }
}

void PickerColumnPattern::HandleAccessibilityTextChange()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto blendNode = DynamicCast<FrameNode>(host->GetParent());
    CHECK_NULL_VOID(blendNode);
    auto accessibilityProperty = blendNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetUserTextValue(GetCurrentOption());
    accessibilityProperty->SetAccessibilityText(GetCurrentOption());
    blendNode->OnAccessibilityEvent(AccessibilityEventType::TEXT_CHANGE);
}

bool PickerColumnPattern::InnerHandleScroll(bool isDown, bool isUpatePropertiesOnly, bool isUpdateAnimationProperties)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto pattern = host->GetPattern<PickerColumnPattern>();
    CHECK_NULL_RETURN(pattern, false);
    auto totalOptionCount = pattern->GetOptionCount();
    CHECK_NULL_RETURN(totalOptionCount, false);

    if (!GetCanLoopFromLayoutProperty() && ((isDown && currentIndex_ == totalOptionCount - 1) ||
        (!isDown && currentIndex_ == 0))) {
        return false;
    }

    uint32_t currentIndex = GetCurrentIndex();
    if (isDown) {
        currentIndex = (totalOptionCount + currentIndex + 1) % totalOptionCount; // index add one
    } else {
        auto totalCountAndIndex = totalOptionCount + currentIndex;
        currentIndex = (totalCountAndIndex ? totalCountAndIndex - 1 : 0) % totalOptionCount; // index reduce one
    }
    SetCurrentIndex(currentIndex);
    if (hapticController_ && isEnableHaptic_ && !stopHaptic_) {
        hapticController_->PlayOnce();
    }
    FlushCurrentOptions(isDown, isUpatePropertiesOnly, isUpdateAnimationProperties, isTossPlaying_);
    HandleChangeCallback(isDown, true);
    HandleEventCallback(true);

    HandleAccessibilityTextChange();
    return true;
}

void PickerColumnPattern::InitPanEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    CHECK_NULL_VOID(!panEvent_);
    auto actionStartTask = [weak = WeakClaim(this)](const GestureEvent& event) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (event.GetInputEventType() == InputEventType::AXIS && event.GetSourceTool() == SourceTool::MOUSE) {
            return;
        }
        pattern->HandleDragStart(event);
    };
    auto actionUpdateTask = [weak = WeakClaim(this)](const GestureEvent& event) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->SetMainVelocity(event.GetMainVelocity());
        pattern->HandleDragMove(event);
    };
    auto actionEndTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (info.GetInputEventType() == InputEventType::AXIS && info.GetSourceTool() == SourceTool::MOUSE) {
            return;
        }
        pattern->SetMainVelocity(info.GetMainVelocity());
        pattern->HandleDragEnd();
    };
    auto actionCancelTask = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragEnd();
    };
    PanDirection panDirection;
    panDirection.type = PanDirection::VERTICAL;
    panEvent_ = MakeRefPtr<PanEvent>(
        std::move(actionStartTask), std::move(actionUpdateTask), std::move(actionEndTask), std::move(actionCancelTask));
    PanDistanceMap distanceMap = { { SourceTool::UNKNOWN, DEFAULT_PAN_DISTANCE.ConvertToPx() },
        { SourceTool::PEN, DEFAULT_PEN_PAN_DISTANCE.ConvertToPx() } };
    gestureHub->AddPanEvent(panEvent_, panDirection, DEFAULT_PAN_FINGER, distanceMap);
}

void PickerColumnPattern::HandleDragStart(const GestureEvent& event)
{
    SetSelectedMark();
    CHECK_NULL_VOID(GetHost());
    CHECK_NULL_VOID(GetToss());
    auto toss = GetToss(); // todo
    auto offsetY = event.GetGlobalPoint().GetY();
    toss->SetStart(offsetY);
    yLast_ = offsetY;
    pressed_ = true;
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    frameNode->AddFRCSceneInfo(PICKER_DRAG_SCENE, event.GetMainVelocity(), SceneStatus::START);
}

void PickerColumnPattern::HandleDragMove(const GestureEvent& event)
{
    if (event.GetInputEventType() == InputEventType::AXIS && event.GetSourceTool() == SourceTool::MOUSE &&
        CanMove(LessNotEqual(event.GetDelta().GetY(), 0.0))) {
        InnerHandleScroll(LessNotEqual(event.GetDelta().GetY(), 0.0), true);
        return;
    }
    animationBreak_ = false;
    CHECK_NULL_VOID(pressed_);
    CHECK_NULL_VOID(GetHost());
    CHECK_NULL_VOID(GetToss());
    auto toss = GetToss();
    auto offsetY =
        event.GetGlobalPoint().GetY() + (event.GetInputEventType() == InputEventType::AXIS ? event.GetOffsetY() : 0.0);
    if (NearEqual(offsetY, yLast_, 1.0)) { // if changing less than 1.0, no need to handle
        if (hapticController_) {
            hapticController_->Stop();
        }
        return;
    }
    toss->SetEnd(offsetY);
    UpdateColumnChildPosition(offsetY);
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    frameNode->AddFRCSceneInfo(PICKER_DRAG_SCENE, event.GetMainVelocity(), SceneStatus::RUNNING);
}

void PickerColumnPattern::HandleDragEnd()
{
    if (hapticController_) {
        hapticController_->Stop();
        isHapticPlayOnce_ = false;
    }
    pressed_ = false;
    CHECK_NULL_VOID(GetToss());
    auto toss = GetToss();
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    if (!NotLoopOptions() && toss->Play()) {
        isTossPlaying_ = true;
        frameNode->AddFRCSceneInfo(PICKER_DRAG_SCENE, mainVelocity_, SceneStatus::END);
        // AccessibilityEventType::SCROLL_END
        return;
    }
    yOffset_ = 0.0;
    yLast_ = 0.0;
    if (!animationCreated_) {
        ScrollOption(0.0);
        return;
    }
    PickerScrollDirection dir = scrollDelta_ > 0.0 ? PickerScrollDirection::DOWN : PickerScrollDirection::UP;
    uint32_t middleIndex = GetShowCount() / MIDDLE_CHILD_INDEX;
    auto shiftDistance = (dir == PickerScrollDirection::UP) ? optionProperties_[middleIndex].prevDistance
                                                            : optionProperties_[middleIndex].nextDistance;
    auto shiftThreshold = shiftDistance / MIDDLE_CHILD_INDEX;
    if (!isTossPlaying_ && GreatOrEqual(static_cast<double>(std::abs(scrollDelta_)), std::abs(shiftThreshold))) {
        InnerHandleScroll(LessNotEqual(scrollDelta_, 0.0), true, false);
        scrollDelta_ = scrollDelta_ - std::abs(shiftDistance) * (dir == PickerScrollDirection::UP ? -1 : 1);
    }
    CreateAnimation(scrollDelta_, 0.0);
    frameNode->AddFRCSceneInfo(PICKER_DRAG_SCENE, mainVelocity_, SceneStatus::END);
}

void PickerColumnPattern::CreateAnimation()
{
    CHECK_NULL_VOID(!animationCreated_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto propertyCallback = [weak = AceType::WeakClaim(this)](float value) {
        auto column = weak.Upgrade();
        CHECK_NULL_VOID(column);
        column->ScrollOption(value);
    };
    scrollProperty_ = AceType::MakeRefPtr<NodeAnimatablePropertyFloat>(0.0, std::move(propertyCallback));
    renderContext->AttachNodeAnimatableProperty(scrollProperty_);

    auto aroundClickCallback = [weak = AceType::WeakClaim(this)](float value) {
        auto column = weak.Upgrade();
        CHECK_NULL_VOID(column);
        if (value > 0) {
            column->UpdateColumnChildPosition(std::ceil(value));
        } else {
            column->UpdateColumnChildPosition(std::floor(value));
        }
    };
    aroundClickProperty_ = AceType::MakeRefPtr<NodeAnimatablePropertyFloat>(0.0, std::move(aroundClickCallback));
    renderContext->AttachNodeAnimatableProperty(aroundClickProperty_);
    animationCreated_ = true;
}

void PickerColumnPattern::CreateAnimation(double from, double to)
{
    AnimationOption option;
    option.SetCurve(Curves::FAST_OUT_SLOW_IN);
    option.SetDuration(CLICK_ANIMATION_DURATION);
    scrollProperty_->Set(from);
    AnimationUtils::Animate(option, [weak = AceType::WeakClaim(this), to]() {
        auto column = weak.Upgrade();
        CHECK_NULL_VOID(column);
        column->scrollProperty_->Set(to);
    });
}

void PickerColumnPattern::ScrollOption(double delta, bool isJump)
{
    scrollDelta_ = delta;
    auto midIndex = GetShowCount() / 2;
    PickerScrollDirection dir = GreatNotEqual(delta, 0.0) ? PickerScrollDirection::DOWN : PickerScrollDirection::UP;
    auto shiftDistance = (dir == PickerScrollDirection::UP) ? optionProperties_[midIndex].prevDistance
                                                            : optionProperties_[midIndex].nextDistance;
    HandleEnterSelectedArea(scrollDelta_, shiftDistance, dir);
    distancePercent_ = delta / shiftDistance;
    auto textLinearPercent = 0.0;
    textLinearPercent = (std::abs(delta)) / (optionProperties_[midIndex].height);
    UpdateTextPropertiesLinear(LessNotEqual(delta, 0.0), textLinearPercent);
    CalcAlgorithmOffset(dir, distancePercent_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
}

void PickerColumnPattern::UpdateDisappearTextProperties(const RefPtr<PickerTheme>& pickerTheme,
    const RefPtr<TextLayoutProperty>& textLayoutProperty, const RefPtr<PickerLayoutProperty>& pickerLayoutProperty)
{
    auto normalOptionSize = pickerTheme->GetOptionStyle(false, false).GetFontSize();
    textLayoutProperty->UpdateTextColor(pickerLayoutProperty->GetDisappearColor().value_or(
        pickerTheme->GetOptionStyle(false, false).GetTextColor()));
    if (pickerLayoutProperty->HasDisappearFontSize()) {
        textLayoutProperty->UpdateFontSize(pickerLayoutProperty->GetDisappearFontSize().value());
    } else {
        textLayoutProperty->UpdateAdaptMaxFontSize(normalOptionSize);
        textLayoutProperty->UpdateAdaptMinFontSize(pickerTheme->GetOptionStyle(false, false).GetAdaptMinFontSize());
    }
    textLayoutProperty->UpdateFontWeight(pickerLayoutProperty->GetDisappearWeight().value_or(
        pickerTheme->GetOptionStyle(false, false).GetFontWeight()));
    textLayoutProperty->UpdateFontFamily(pickerLayoutProperty->GetDisappearFontFamily().value_or(
        pickerTheme->GetOptionStyle(false, false).GetFontFamilies()));
    textLayoutProperty->UpdateItalicFontStyle(pickerLayoutProperty->GetDisappearFontStyle().value_or(
        pickerTheme->GetOptionStyle(false, false).GetFontStyle()));
}

void PickerColumnPattern::UpdateCandidateTextProperties(const RefPtr<PickerTheme>& pickerTheme,
    const RefPtr<TextLayoutProperty>& textLayoutProperty, const RefPtr<PickerLayoutProperty>& pickerLayoutProperty)
{
    auto focusOptionSize = pickerTheme->GetOptionStyle(false, false).GetFontSize() + FONT_SIZE;
    textLayoutProperty->UpdateTextColor(
        pickerLayoutProperty->GetColor().value_or(pickerTheme->GetOptionStyle(false, false).GetTextColor()));
    if (pickerLayoutProperty->HasFontSize()) {
        textLayoutProperty->UpdateFontSize(pickerLayoutProperty->GetFontSize().value());
    } else {
        textLayoutProperty->UpdateAdaptMaxFontSize(focusOptionSize);
        textLayoutProperty->UpdateAdaptMinFontSize(
            pickerTheme->GetOptionStyle(true, false).GetAdaptMinFontSize() - FOCUS_SIZE);
    }
    textLayoutProperty->UpdateFontWeight(
        pickerLayoutProperty->GetWeight().value_or(pickerTheme->GetOptionStyle(false, false).GetFontWeight()));
    textLayoutProperty->UpdateFontFamily(
        pickerLayoutProperty->GetFontFamily().value_or(pickerTheme->GetOptionStyle(false, false).GetFontFamilies()));
    textLayoutProperty->UpdateItalicFontStyle(
        pickerLayoutProperty->GetFontStyle().value_or(pickerTheme->GetOptionStyle(false, false).GetFontStyle()));
}

void PickerColumnPattern::UpdateSelectedTextProperties(const RefPtr<PickerTheme>& pickerTheme,
    const RefPtr<TextLayoutProperty>& textLayoutProperty, const RefPtr<PickerLayoutProperty>& pickerLayoutProperty)
{
    auto selectedOptionSize = pickerTheme->GetOptionStyle(true, false).GetFontSize();
    if (pickerTheme->IsCircleDial() && !isUserSetSelectColor_) {
        if (selectedMarkPaint_) {
            textLayoutProperty->UpdateTextColor(pickerTheme->GetOptionStyle(true, true).GetTextColor());
        } else {
            textLayoutProperty->UpdateTextColor(pickerTheme->GetOptionStyle(false, false).GetTextColor());
        }
    } else {
        textLayoutProperty->UpdateTextColor(
            pickerLayoutProperty->GetSelectedColor().value_or(pickerTheme->GetOptionStyle(true, false).GetTextColor()));
    }

    if (pickerLayoutProperty->HasSelectedFontSize()) {
        textLayoutProperty->UpdateFontSize(pickerLayoutProperty->GetSelectedFontSize().value());
    } else {
        textLayoutProperty->UpdateAdaptMaxFontSize(selectedOptionSize);
        textLayoutProperty->UpdateAdaptMinFontSize(pickerTheme->GetOptionStyle(true, false).GetAdaptMinFontSize());
    }
    textLayoutProperty->UpdateFontWeight(
        pickerLayoutProperty->GetSelectedWeight().value_or(pickerTheme->GetOptionStyle(true, false).GetFontWeight()));
    SelectedWeight_ =
        pickerLayoutProperty->GetSelectedWeight().value_or(pickerTheme->GetOptionStyle(true, false).GetFontWeight());
    textLayoutProperty->UpdateFontFamily(pickerLayoutProperty->GetSelectedFontFamily().value_or(
        pickerTheme->GetOptionStyle(true, false).GetFontFamilies()));
    textLayoutProperty->UpdateItalicFontStyle(
        pickerLayoutProperty->GetSelectedFontStyle().value_or(pickerTheme->GetOptionStyle(true, false).GetFontStyle()));
}

void PickerColumnPattern::ResetAlgorithmOffset()
{
    algorithmOffset_.clear();
    uint32_t counts = GetShowCount();
    for (uint32_t i = 0; i < counts; i++) {
        algorithmOffset_.emplace_back(0.0f);
    }
}

void PickerColumnPattern::CalcAlgorithmOffset(PickerScrollDirection dir, double distancePercent)
{
    algorithmOffset_.clear();
    uint32_t counts = GetShowCount();
    for (uint32_t i = 0; i < counts; i++) {
        auto distance =
            (dir == PickerScrollDirection::UP) ? optionProperties_[i].prevDistance : optionProperties_[i].nextDistance;
        algorithmOffset_.emplace_back(static_cast<int32_t>(distance * distancePercent));
    }
}

void PickerColumnPattern::SetOptionShiftDistance()
{
    uint32_t itemCounts = 0;
    CHECK_EQUAL_VOID(GetOptionItemCount(itemCounts), false);
    bool isLanscape = IsLanscape(itemCounts);
    for (uint32_t i = 0; i < itemCounts; i++) {
        PickerOptionProperty& prop = optionProperties_[i];
        if (isLanscape) {
            prop.prevDistance = GetShiftDistanceForLandscape(i, PickerScrollDirection::UP);
            prop.nextDistance = GetShiftDistanceForLandscape(i, PickerScrollDirection::DOWN);
        } else {
            prop.prevDistance = GetShiftDistance(i, PickerScrollDirection::UP);
            prop.nextDistance = GetShiftDistance(i, PickerScrollDirection::DOWN);
        }
    }
}

void PickerColumnPattern::UpdateToss(double offsetY)
{
    UpdateColumnChildPosition(offsetY);
}

void PickerColumnPattern::UpdateFinishToss(double offsetY)
{
    int32_t dragDelta = offsetY - yLast_;
    if (!CanMove(LessNotEqual(dragDelta, 0))) {
        return;
    }
    auto midIndex = GetShowCount() / 2;
    PickerScrollDirection dir = dragDelta > 0.0 ? PickerScrollDirection::DOWN : PickerScrollDirection::UP;
    auto shiftDistance = (dir == PickerScrollDirection::UP) ? optionProperties_[midIndex].prevDistance
                                                            : optionProperties_[midIndex].nextDistance;
    ScrollOption(shiftDistance);
}

void PickerColumnPattern::TossStoped()
{
    yOffset_ = 0.0;
    yLast_ = 0.0;
    ScrollOption(0.0);
}

void PickerColumnPattern::ShiftOptionProp(RefPtr<FrameNode> curNode, RefPtr<FrameNode> shiftNode)
{
    RefPtr<TextPattern> curPattern = curNode->GetPattern<TextPattern>();
    CHECK_NULL_VOID(curPattern);
    RefPtr<TextLayoutProperty> curLayoutProperty = curPattern->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(curLayoutProperty);

    RefPtr<TextPattern> shiftPattern = shiftNode->GetPattern<TextPattern>();
    CHECK_NULL_VOID(shiftPattern);
    RefPtr<TextLayoutProperty> shiftLayoutProperty = shiftPattern->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(shiftLayoutProperty);
    curLayoutProperty->UpdateFontWeight(shiftLayoutProperty->GetFontWeight().value_or(FontWeight::W100));
}

void PickerColumnPattern::SetAccessibilityAction()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto blendNode = DynamicCast<FrameNode>(host->GetParent());
    CHECK_NULL_VOID(blendNode);
    auto stackNode = DynamicCast<FrameNode>(blendNode->GetParent());
    CHECK_NULL_VOID(stackNode);
    auto parentNode = DynamicCast<FrameNode>(stackNode->GetParent());
    CHECK_NULL_VOID(parentNode);
    auto accessibilityProperty = blendNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityGroup(true);
    accessibilityProperty->SetAccessibilityCustomRole(parentNode->GetTag());
    accessibilityProperty->SetUserTextValue(GetCurrentOption());
    accessibilityProperty->SetAccessibilityText(GetCurrentOption());

    accessibilityProperty->SetSpecificSupportActionCallback(
        [weakPtr = WeakClaim(this), accessibilityPtr = WeakClaim(RawPtr(accessibilityProperty))]() {
        const auto& pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        const auto& accessibilityProperty = accessibilityPtr.Upgrade();
        CHECK_NULL_VOID(accessibilityProperty);
        if (pattern->CanMove(true)) {
            accessibilityProperty->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
        }
        if (pattern->CanMove(false)) {
            accessibilityProperty->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
        }
    });

    accessibilityProperty->SetActionScrollForward([weakPtr = WeakClaim(this)]() {
        const auto& pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (!pattern->CanMove(true)) {
            return;
        }
        CHECK_NULL_VOID(pattern->animationCreated_);
        pattern->InnerHandleScroll(true);
        pattern->CreateAnimation(0.0 - pattern->jumpInterval_, 0.0);
        // AccessibilityEventType::SCROLL_END
    });

    accessibilityProperty->SetActionScrollBackward([weakPtr = WeakClaim(this)]() {
        const auto& pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (!pattern->CanMove(false)) {
            return;
        }
        CHECK_NULL_VOID(pattern->animationCreated_);
        pattern->InnerHandleScroll(false);
        pattern->CreateAnimation(pattern->jumpInterval_, 0.0);
        // AccessibilityEventType::SCROLL_END
    });
}

RefPtr<ClickEvent> PickerColumnPattern::CreateItemClickEventListener(RefPtr<PickerEventParam> param)
{
    auto clickEventHandler = [param, weak = WeakClaim(this)](const GestureEvent& /* info */) {
        auto pattern = weak.Upgrade();
        pattern->OnAroundButtonClick(param);
    };
    auto listener = AceType::MakeRefPtr<NG::ClickEvent>(clickEventHandler);
    return listener;
}

void PickerColumnPattern::OnAroundButtonClick(RefPtr<PickerEventParam> param)
{
    if (clickBreak_) {
        return;
    }
    int32_t middleIndex = static_cast<int32_t>(GetShowCount()) / 2;
    int32_t step = param->itemIndex_ - middleIndex;
    if (step != 0) {
        if (animation_) {
            AnimationUtils::StopAnimation(animation_);
            yLast_ = 0.0;
            yOffset_ = 0.0;
        }
        if (hapticController_) {
            isHapticPlayOnce_ = true;
            hapticController_->Stop();
        }
        auto distance =
            (step > 0 ? optionProperties_[middleIndex].prevDistance : optionProperties_[middleIndex].nextDistance) *
            std::abs(step);

        AnimationOption option;
        option.SetCurve(Curves::FAST_OUT_SLOW_IN);
        option.SetDuration(CLICK_ANIMATION_DURATION);
        aroundClickProperty_->Set(0.0);
        animation_ = AnimationUtils::StartAnimation(option, [weak = AceType::WeakClaim(this), step, distance]() {
            auto column = weak.Upgrade();
            CHECK_NULL_VOID(column);
            column->aroundClickProperty_->Set(step > 0 ? 0.0 - std::abs(distance) : std::abs(distance));
        });
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto pipeline = host->GetContext();
        CHECK_NULL_VOID(pipeline);
        pipeline->RequestFrame();
    }

    SetSelectedMark();
}

void PickerColumnPattern::TossAnimationStoped()
{
    isTossPlaying_ = false;
    if (hapticController_) {
        hapticController_->Stop();
    }
    yLast_ = 0.0;
}

float PickerColumnPattern::GetShiftDistance(uint32_t index, PickerScrollDirection dir)
{
    uint32_t optionCounts = 0;
    CHECK_EQUAL_RETURN(GetOptionItemCount(optionCounts), false, 0.0f);
    uint32_t nextIndex = 0;
    float distance = 0.0f;
    float val = 0.0f;
    auto isDown = dir == PickerScrollDirection::DOWN;
    if (optionCounts == 0) {
        return distance;
    }
    if (isDown) {
        nextIndex = (optionCounts + index + 1) % optionCounts; // index add one
    } else {
        nextIndex = (optionCounts + index - 1) % optionCounts; // index reduce one
    }
    switch (static_cast<PickerOptionIndex>(index)) {
        case PickerOptionIndex::COLUMN_INDEX_0: // first
            distance = (dir == PickerScrollDirection::DOWN) ? optionProperties_[index].height
                                                            : (0.0f - optionProperties_[index].height);
            break;
        case PickerOptionIndex::COLUMN_INDEX_1:
            distance = (dir == PickerScrollDirection::DOWN) ? optionProperties_[index].height
                                                            : (0.0f - optionProperties_[index].height);
            break;
        case PickerOptionIndex::COLUMN_INDEX_2:
            if (dir == PickerScrollDirection::UP) {
                distance = -optionProperties_[nextIndex].height;
            } else {
                val = optionProperties_[index].height +
                      (optionProperties_[nextIndex].height - optionProperties_[nextIndex].fontheight) /
                          MIDDLE_CHILD_INDEX;
                distance = std::ceil(val);
            }
            break;
        case PickerOptionIndex::COLUMN_INDEX_3:
            val = (optionProperties_[index].height - optionProperties_[nextIndex].fontheight) / MIDDLE_CHILD_INDEX +
                  optionProperties_[nextIndex].height;
            distance = (dir == PickerScrollDirection::DOWN) ? val : (0.0f - val);
            distance = std::floor(distance);
            break;
        case PickerOptionIndex::COLUMN_INDEX_4:
            if (dir == PickerScrollDirection::DOWN) {
                distance = optionProperties_[nextIndex].height;
            } else {
                val = optionProperties_[index].height +
                      (optionProperties_[nextIndex].height - optionProperties_[nextIndex].fontheight) /
                          MIDDLE_CHILD_INDEX;
                distance = std::ceil(0.0f - val);
            }
            break;
        case PickerOptionIndex::COLUMN_INDEX_5:
            distance = (dir == PickerScrollDirection::DOWN) ? optionProperties_[index].height
                                                            : (0.0f - optionProperties_[index].height);
            break;
        case PickerOptionIndex::COLUMN_INDEX_6: // last
            distance = (dir == PickerScrollDirection::DOWN) ? optionProperties_[index].height
                                                            : (0.0f - optionProperties_[index].height);
            break;
        default:
            break;
    }
    return distance;
}

float PickerColumnPattern::GetShiftDistanceForLandscape(uint32_t index, PickerScrollDirection dir)
{
    uint32_t optionCounts = 0;
    CHECK_EQUAL_RETURN(GetOptionItemCount(optionCounts), false, 0.0f);
    uint32_t nextIndex = 0;
    float distance = 0.0f;
    float val = 0.0f;
    auto isDown = dir == PickerScrollDirection::DOWN;
    if (optionCounts == 0) {
        return distance;
    }
    if (isDown) {
        nextIndex = (optionCounts + index + 1) % optionCounts; // index add one
    } else {
        nextIndex = (optionCounts + index - 1) % optionCounts; // index reduce one
    }
    switch (static_cast<PickerOptionIndex>(index)) {
        case PickerOptionIndex::COLUMN_INDEX_0: // first
            if (dir == PickerScrollDirection::UP) {
                distance = 0.0f - optionProperties_[index].height;
            } else {
                distance = optionProperties_[index].height +
                           (optionProperties_[nextIndex].height - optionProperties_[nextIndex].fontheight) /
                               MIDDLE_CHILD_INDEX;
            }
            break;
        case PickerOptionIndex::COLUMN_INDEX_1:
            val = (optionProperties_[index].height - optionProperties_[nextIndex].fontheight) / MIDDLE_CHILD_INDEX +
                  optionProperties_[nextIndex].height;
            distance = (dir == PickerScrollDirection::DOWN) ? val : (0.0f - val);
            distance = std::floor(distance);
            break;
        case PickerOptionIndex::COLUMN_INDEX_2: // last
            if (dir == PickerScrollDirection::DOWN) {
                distance = optionProperties_[index].height;
            } else {
                val = optionProperties_[index].height +
                      (optionProperties_[nextIndex].height - optionProperties_[nextIndex].fontheight) /
                          MIDDLE_CHILD_INDEX;
                distance = 0.0f - val;
            }
            break;
        default:
            break;
    }
    return distance;
}

void PickerColumnPattern::PlayRestAnimation()
{
    PickerScrollDirection dir = scrollDelta_ > 0.0 ? PickerScrollDirection::DOWN : PickerScrollDirection::UP;
    int32_t middleIndex = static_cast<int32_t>(GetShowCount()) / 2;
    double shiftDistance = (dir == PickerScrollDirection::UP) ? optionProperties_[middleIndex].prevDistance
                                                              : optionProperties_[middleIndex].nextDistance;
    double shiftThreshold = shiftDistance / 2;
    if (GreatOrEqual(static_cast<double>(std::abs(scrollDelta_)), std::abs(shiftThreshold))) {
        InnerHandleScroll(LessNotEqual(scrollDelta_, 0.0), true);
        scrollDelta_ = scrollDelta_ - std::abs(shiftDistance) * (dir == PickerScrollDirection::UP ? -1 : 1);
    }

    CreateAnimation(scrollDelta_, 0.0);
}

DimensionRect PickerColumnPattern::CalculateHotZone(
    int32_t index, int32_t midSize, float middleChildHeight, float otherChildHeight)
{
    float hotZoneHeight = 0.0f;
    float hotZoneOffsetY = 0.0f;
    if (index == midSize) {
        hotZoneHeight = middleChildHeight;
    }
    if (size_.Height() <= middleChildHeight) {
        hotZoneHeight = index == midSize ? size_.Height() : 0;
    } else if (size_.Height() <= (middleChildHeight + HOT_ZONE_HEIGHT_CANDIDATE * otherChildHeight)) {
        if ((index == midSize + 1) || (index == midSize - 1)) {
            hotZoneHeight = (size_.Height() - middleChildHeight) / MIDDLE_CHILD_INDEX;
            hotZoneOffsetY = (index == midSize - 1) ? (otherChildHeight - hotZoneHeight) : 0;
        }
    } else if (size_.Height() <= (middleChildHeight + HOT_ZONE_HEIGHT_DISAPPEAR * otherChildHeight)) {
        if ((index == midSize + 1) || (index == midSize - 1)) {
            hotZoneHeight = otherChildHeight;
        } else if ((index == midSize + HOT_ZONE_HEIGHT_CANDIDATE) || (index == midSize - HOT_ZONE_HEIGHT_CANDIDATE)) {
            hotZoneHeight = (size_.Height() - middleChildHeight - HOT_ZONE_HEIGHT_CANDIDATE * otherChildHeight) /
                            MIDDLE_CHILD_INDEX;
            hotZoneOffsetY = (index == midSize - HOT_ZONE_HEIGHT_CANDIDATE) ? (otherChildHeight - hotZoneHeight) : 0;
        }
    } else {
        if ((index == midSize + 1) || (index == midSize - 1)) {
            hotZoneHeight = otherChildHeight;
        } else if ((index == midSize + HOT_ZONE_HEIGHT_CANDIDATE) || (index == midSize - HOT_ZONE_HEIGHT_CANDIDATE)) {
            hotZoneHeight = otherChildHeight;
        }
    }
    OffsetF hotZoneOffset;
    SizeF hotZoneSize;
    hotZoneOffset.SetX(0.0f);
    hotZoneOffset.SetY(hotZoneOffsetY);
    hotZoneSize.SetWidth(size_.Width());
    hotZoneSize.SetHeight(hotZoneHeight);
    DimensionRect hotZoneRegion;
    hotZoneRegion.SetSize(DimensionSize(Dimension(hotZoneSize.Width()), Dimension(hotZoneSize.Height())));
    hotZoneRegion.SetOffset(DimensionOffset(Dimension(hotZoneOffset.GetX()), Dimension(hotZoneOffset.GetY())));
    return hotZoneRegion;
}

void PickerColumnPattern::AddHotZoneRectToText()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto childSize = static_cast<int32_t>(host->GetChildren().size());
    auto midSize = childSize / MIDDLE_CHILD_INDEX;
    auto middleChildHeight = optionProperties_[midSize].height;
    auto otherChildHeight = optionProperties_[midSize - 1].height;
    for (int32_t i = 0; i < childSize; i++) {
        RefPtr<FrameNode> childNode = DynamicCast<FrameNode>(host->GetChildAtIndex(i));
        CHECK_NULL_VOID(childNode);
        DimensionRect hotZoneRegion = CalculateHotZone(i, midSize, middleChildHeight, otherChildHeight);
        childNode->AddHotZoneRect(hotZoneRegion);
    }
}

void PickerColumnPattern::SetSelectedMarkListener(const std::function<void(std::string& selectedColumnId)>& listener)
{
    focusedListerner_ = listener;
    if (!circleUtils_) {
        circleUtils_ = new PickerColumnPatternCircleUtils<PickerColumnPattern>();
    }
}

void PickerColumnPattern::SetSelectedMark(bool focus, bool notify, bool reRender)
{
    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();
    CHECK_NULL_VOID(pickerTheme);
    if (pickerTheme->IsCircleDial()) {
        CHECK_NULL_VOID(circleUtils_);
        circleUtils_->SetSelectedMark(this, pickerTheme, focus, notify, reRender);
    }
}

void PickerColumnPattern::SetSelectedMarkId(const std::string& strColumnId)
{
    selectedColumnId_ = strColumnId;
}

void PickerColumnPattern::SetSelectedMarkPaint(bool paint)
{
    selectedMarkPaint_ = paint;
}

void PickerColumnPattern::UpdateUserSetSelectColor()
{
    isUserSetSelectColor_ = true;
    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();
    CHECK_NULL_VOID(pickerTheme);
    UpdateSelectedTextColor(pickerTheme);
}

bool PickerColumnPattern::NotLoopOptions() const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto showOptionCount = GetShowCount();
    auto pattern = host->GetPattern<PickerColumnPattern>();
    CHECK_NULL_RETURN(pattern, false);
    uint32_t totalOptionCount = pattern->GetOptionCount();
    return totalOptionCount <= showOptionCount / HALF_NUMBER + 1; // the critical value of loop condition.
}

void PickerColumnPattern::AddAnimationTextProperties(
    uint32_t currentIndex, const RefPtr<TextLayoutProperty>& textLayoutProperty)
{
    TextProperties properties;
    if (textLayoutProperty->HasFontSize()) {
        MeasureContext measureContext;
        measureContext.textContent = MEASURE_SIZE_STRING;
        measureContext.fontSize = textLayoutProperty->GetFontSize().value();
        auto size = MeasureUtil::MeasureTextSize(measureContext);
        if (!optionProperties_.empty()) {
            optionProperties_[currentIndex].fontheight = size.Height();
            if (optionProperties_[currentIndex].fontheight > optionProperties_[currentIndex].height) {
                optionProperties_[currentIndex].fontheight = optionProperties_[currentIndex].height;
            }
        }
        SetOptionShiftDistance();
        properties.fontSize = Dimension(textLayoutProperty->GetFontSize().value().ConvertToPx());
    }
    if (textLayoutProperty->HasTextColor()) {
        properties.currentColor = textLayoutProperty->GetTextColor().value();
    }
    if (textLayoutProperty->HasFontWeight()) {
        properties.fontWeight = textLayoutProperty->GetFontWeight().value();
    }
    if (currentIndex > 0) {
        properties.upFontSize = animationProperties_[currentIndex - 1].fontSize;
        animationProperties_[currentIndex - 1].downFontSize = properties.fontSize;

        properties.upColor = animationProperties_[currentIndex - 1].currentColor;
        animationProperties_[currentIndex - 1].downColor = properties.currentColor;

        properties.upFontWeight = animationProperties_[currentIndex - 1].fontWeight;
        animationProperties_[currentIndex - 1].downFontWeight = properties.fontWeight;
    }
    animationProperties_.emplace_back(properties);
}

void PickerColumnPattern::FlushAnimationTextProperties(bool isDown)
{
    if (!animationProperties_.size()) {
        return;
    }
    if (isDown) {
        for (size_t i = 0; i < animationProperties_.size(); i++) {
            if (i > 0) {
                animationProperties_[i - 1].upFontSize = animationProperties_[i].upFontSize;
                animationProperties_[i - 1].fontSize = animationProperties_[i].fontSize;
                animationProperties_[i - 1].downFontSize = animationProperties_[i].downFontSize;

                animationProperties_[i - 1].upColor = animationProperties_[i].upColor;
                animationProperties_[i - 1].currentColor = animationProperties_[i].currentColor;
                animationProperties_[i - 1].downColor = animationProperties_[i].downColor;
            }
            if (i == (animationProperties_.size() - 1)) {
                animationProperties_[i].upFontSize = animationProperties_[i].fontSize;
                animationProperties_[i].fontSize = animationProperties_[i].fontSize * FONT_SIZE_PERCENT_50;
                animationProperties_[i].downFontSize = Dimension();

                animationProperties_[i].upColor = animationProperties_[i].currentColor;
                auto colorEvaluator = AceType::MakeRefPtr<LinearEvaluator<Color>>();
                animationProperties_[i].currentColor =
                    colorEvaluator->Evaluate(Color(), animationProperties_[i].currentColor, FONT_SIZE_PERCENT_50);
                animationProperties_[i].downColor = Color();
            }
        }
    } else {
        for (size_t i = animationProperties_.size() - 1;; i--) {
            if (i == 0) {
                animationProperties_[i].upFontSize = Dimension();
                animationProperties_[i].downFontSize = animationProperties_[i].fontSize;
                animationProperties_[i].fontSize = animationProperties_[i].fontSize * FONT_SIZE_PERCENT_50;

                animationProperties_[i].upColor = Color();
                animationProperties_[i].downColor = animationProperties_[i].currentColor;
                auto colorEvaluator = AceType::MakeRefPtr<LinearEvaluator<Color>>();
                animationProperties_[i].currentColor =
                    colorEvaluator->Evaluate(Color(), animationProperties_[i].currentColor, FONT_SIZE_PERCENT_50);
                break;
            } else {
                animationProperties_[i].upFontSize = animationProperties_[i - 1].upFontSize;
                animationProperties_[i].fontSize = animationProperties_[i - 1].fontSize;
                animationProperties_[i].downFontSize = animationProperties_[i - 1].downFontSize;

                animationProperties_[i].upColor = animationProperties_[i - 1].upColor;
                animationProperties_[i].currentColor = animationProperties_[i - 1].currentColor;
                animationProperties_[i].downColor = animationProperties_[i - 1].downColor;
            }
        }
    }
}

bool PickerColumnPattern::GetOptionItemCount(uint32_t& itemCounts)
{
    itemCounts = GetShowCount();
    return true;
}

bool PickerColumnPattern::IsLanscape(uint32_t itemCount)
{
    return (itemCount == OPTION_COUNT_PHONE_LANDSCAPE);
}

#ifdef SUPPORT_DIGITAL_CROWN
std::string& PickerColumnPattern::GetSelectedColumnId()
{
    return selectedColumnId_;
}

bool PickerColumnPattern::IsCrownEventEnded()
{
    return isCrownEventEnded_;
}

int32_t PickerColumnPattern::GetDigitalCrownSensitivity()
{
    if (crownSensitivity_ == INVALID_CROWNSENSITIVITY) {
        auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
        CHECK_NULL_RETURN(pipeline, DEFAULT_CROWNSENSITIVITY);
        auto pickerTheme = pipeline->GetTheme<PickerTheme>();
        CHECK_NULL_RETURN(pickerTheme, DEFAULT_CROWNSENSITIVITY);
        crownSensitivity_ = pickerTheme->GetDigitalCrownSensitivity();
    }

    return crownSensitivity_;
}

void PickerColumnPattern::SetDigitalCrownSensitivity(int32_t crownSensitivity)
{
    crownSensitivity_ = crownSensitivity;
}

bool PickerColumnPattern::OnCrownEvent(const CrownEvent& event)
{
    CHECK_NULL_RETURN(circleUtils_, false);
    return circleUtils_->OnCrownEvent(this, event);
}

void PickerColumnPattern::HandleCrownBeginEvent(const CrownEvent& event)
{
    auto toss = GetToss();
    CHECK_NULL_VOID(toss);
    double offsetY = 0.0;
    toss->SetStart(offsetY);
    yLast_ = offsetY;
    pressed_ = true;
    isCrownEventEnded_ = false;
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    frameNode->AddFRCSceneInfo(PICKER_DRAG_SCENE, event.angularVelocity, SceneStatus::START);
}

void PickerColumnPattern::HandleCrownMoveEvent(const CrownEvent& event)
{
    SetMainVelocity(event.angularVelocity);
    animationBreak_ = false;
    isCrownEventEnded_ = false;
    CHECK_NULL_VOID(pressed_);
    auto toss = GetToss();
    CHECK_NULL_VOID(toss);
    CHECK_NULL_VOID(circleUtils_);
    auto offsetY = circleUtils_->GetCrownRotatePx(event, GetDigitalCrownSensitivity());
    offsetY += yLast_;
    toss->SetEnd(offsetY);
    UpdateColumnChildPosition(offsetY);
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    frameNode->AddFRCSceneInfo(PICKER_DRAG_SCENE, event.angularVelocity, SceneStatus::RUNNING);
}

void PickerColumnPattern::HandleCrownEndEvent(const CrownEvent& event)
{
    SetMainVelocity(event.angularVelocity);
    pressed_ = false;
    isCrownEventEnded_ = true;
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);

    yOffset_ = 0.0;
    yLast_ = 0.0;
    if (!animationCreated_) {
        ScrollOption(0.0);
        if (hapticController_) {
            hapticController_->Stop();
        }
        return;
    }

    PickerScrollDirection dir =
        GreatNotEqual(scrollDelta_, 0.0f) ? PickerScrollDirection::DOWN : PickerScrollDirection::UP;
    auto middleIndex = static_cast<int32_t>(GetShowCount()) / MIDDLE_CHILD_INDEX;
    auto shiftDistance = (dir == PickerScrollDirection::UP) ? optionProperties_[middleIndex].prevDistance
                                                            : optionProperties_[middleIndex].nextDistance;
    auto shiftThreshold = shiftDistance / MIDDLE_CHILD_INDEX;
    if (GreatOrEqual(static_cast<double>(std::abs(scrollDelta_)), std::abs(shiftThreshold))) {
        InnerHandleScroll(LessNotEqual(scrollDelta_, 0.0), true);
        scrollDelta_ = scrollDelta_ - std::abs(shiftDistance) * (dir == PickerScrollDirection::UP ? -1 : 1);
    }
    CreateAnimation(scrollDelta_, 0.0);
    frameNode->AddFRCSceneInfo(PICKER_DRAG_SCENE, mainVelocity_, SceneStatus::END);
    if (hapticController_) {
        hapticController_->Stop();
    }
}
#endif
} // namespace OHOS::Ace::NG

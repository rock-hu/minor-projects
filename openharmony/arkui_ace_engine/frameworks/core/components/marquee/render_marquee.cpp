/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/marquee/render_marquee.h"

#include "base/log/event_report.h"
#include "core/components/marquee/marquee_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

// Defines how long delay will be between each jump.
constexpr double DEFAULT_MARQUEE_SCROLL_DELAY = 85.0;

bool IsPlayingAnimation(const RefPtr<Animator>& controller)
{
    return (controller->GetStatus() == Animator::Status::RUNNING);
}

} // namespace

void RenderMarquee::Start()
{
    if (!NeedMarquee()) {
        return;
    }
    if ((!childText_) || (!controller_)) {
        startAfterLayout_ = true;
        LOGW("Node has not built yet, animation will start after layout.");
        return;
    }
    if (isHidden_) {
        startAfterShowed_ = true;
        LOGW("Marquee is hidden, animation will start when showed.");
        return;
    }
    if (controller_->GetStatus() == Animator::Status::PAUSED) {
        controller_->Resume();
    } else if (controller_->GetStatus() != Animator::Status::RUNNING) {
        // Start first loop
        currentLoop_ = 1;
        UpdateAnimation();
        if (needAnimation_) {
            controller_->Play();
        }
    }
}

void RenderMarquee::Stop()
{
    startAfterLayout_ = false;
    startAfterShowed_ = false;
    if (!controller_) {
        LOGE("Node has not initialized.");
        return;
    }
    if (!IsPlayingAnimation(controller_)) {
        return;
    }
    controller_->Pause();
}

void RenderMarquee::OnHiddenChanged(bool hidden)
{
    isHidden_ = hidden;
    if (!controller_) {
        return;
    }
    if (hidden) {
        if (IsPlayingAnimation(controller_)) {
            startAfterShowed_ = true;
            controller_->Pause();
        }
    } else {
        if (startAfterShowed_) {
            startAfterShowed_ = false;
            Start();
        }
    }
}

void RenderMarquee::UpdateAnimation()
{
    double from = 0.0;
    double to = 0.0;
    if (direction_ == MarqueeDirection::LEFT) {
        from = GetLayoutSize().Width();
        to = -childText_->GetLayoutSize().Width();
    } else {
        from = -childText_->GetLayoutSize().Width();
        to = GetLayoutSize().Width();
    }
    // "scrollAmount_" won't be zero, since it's initialized at Update().
    int32_t duration = static_cast<int32_t>(std::abs(to - from) * DEFAULT_MARQUEE_SCROLL_DELAY / scrollAmount_);
    if (duration <= 0) {
        needAnimation_ = false;
        LOGI("Animation duration is negative, don't need animation.");
        return;
    }
    needAnimation_ = true;
    if (translate_) {
        controller_->RemoveInterpolator(translate_);
    }
    translate_ = AceType::MakeRefPtr<CurveAnimation<double>>(from, to, Curves::LINEAR);
    auto weak = AceType::WeakClaim(this);
    translate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto marquee = weak.Upgrade();
        if (marquee) {
            marquee->UpdateChildPosition(value);
        }
    }));
    controller_->SetDuration(duration);
    controller_->AddInterpolator(translate_);
}

void RenderMarquee::UpdateChildPosition(double position)
{
    childPosition_ = Offset(position, 0.0);
    childText_->SetPosition(childPosition_);
    MarkNeedRender();
}

void RenderMarquee::OnAnimationStart()
{
    if (startEvent_) {
        startEvent_();
    }
    if (onStartEvent_) {
        onStartEvent_();
    }
}

void RenderMarquee::OnAnimationStop()
{
    if (bounceEvent_) {
        bounceEvent_();
    }
    if (onBounceEvent_) {
        onBounceEvent_();
    }
    bool continueAnimation = false;
    if (loop_ <= 0) {
        // Infinite loop
        continueAnimation = true;
    } else {
        if (currentLoop_ < loop_) {
            currentLoop_++;
            continueAnimation = true;
        } else {
            // All loop finished
            if (finishEvent_) {
                finishEvent_();
            }
            if (onFinishEvent_) {
                onFinishEvent_();
                playerFinishControl_ = true;
            }
        }
    }
    if (continueAnimation) {
        UpdateAnimation();
        if (needAnimation_) {
            controller_->Play();
        }
    }
}

void RenderMarquee::Update(const RefPtr<Component>& component)
{
    const RefPtr<MarqueeComponent> marquee = AceType::DynamicCast<MarqueeComponent>(component);
    if (!marquee) {
        LOGE("marquee component is null");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    GetMarqueeCallback(component);
    value_ = marquee->GetValue();
    start_ = marquee->GetPlayerStatus();
    if (start_) {
        startAfterLayout_ = true;
    }
    textStyle_ = marquee->GetTextStyle();
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }
    if (context->UseLiteStyle()) {
        // lite loop time is 1000ms, while default marquee loop is 85ms.
        scrollAmount_ = marquee->GetScrollAmount() * DEFAULT_MARQUEE_SCROLL_DELAY / 1000;
    } else {
        scrollAmount_ = marquee->GetScrollAmount();
    }
    if (LessOrEqual(scrollAmount_, 0.0)) {
        scrollAmount_ = DEFAULT_MARQUEE_SCROLL_AMOUNT;
    }
    currentLoop_ = 1;
    loop_ = marquee->GetLoop();
    if (loop_ <= 0) {
        loop_ = ANIMATION_REPEAT_INFINITE;
    }
    auto textDir = GetTextDirection(marquee->GetValue());
    bool systemRTL = context != nullptr ? context->IsRightToLeft() : false;
    bool isRTL = textDir == TextDirection::INHERIT ? systemRTL : (textDir == TextDirection::RTL);
    direction_ =
        isRTL ? (marquee->GetDirection() == MarqueeDirection::LEFT ? MarqueeDirection::RIGHT : MarqueeDirection::LEFT)
              : marquee->GetDirection();

    bounceEvent_ = AceAsyncEvent<void()>::Create(marquee->GetBounceEventId(), context_);
    finishEvent_ = AceAsyncEvent<void()>::Create(marquee->GetFinishEventId(), context_);
    startEvent_ = AceAsyncEvent<void()>::Create(marquee->GetStartEventId(), context_);
    if (playerFinishControl_) {
        playerFinishControl_ = false;
    }
    bool playStatus = playerFinishControl_ ? false : marquee->GetPlayerStatus();
    const auto& methodController = marquee->GetController();
    if (methodController) {
        auto weak = AceType::WeakClaim(this);
        methodController->SetFunction(
            [weak]() {
                auto marquee = weak.Upgrade();
                if (marquee) {
                    marquee->Start();
                }
            },
            [weak]() {
                auto marquee = weak.Upgrade();
                if (marquee) {
                    marquee->Stop();
                }
            });
        GetPlayerCtr(component, context->GetIsDeclarative(), playStatus);
    }
    if (!controller_) {
        controller_ = CREATE_ANIMATOR(GetContext());
        auto weak = AceType::WeakClaim(this);
        controller_->AddStartListener(Animator::StatusCallback([weak]() {
            auto marquee = weak.Upgrade();
            if (marquee) {
                marquee->OnAnimationStart();
            }
        }));
        controller_->AddStopListener(Animator::StatusCallback([weak]() {
            auto marquee = weak.Upgrade();
            if (marquee) {
                marquee->OnAnimationStop();
            }
        }));
    }
    MarkNeedLayout();
}

void RenderMarquee::PerformLayout()
{
    const auto& children = GetChildren();
    if (children.empty()) {
        LOGW("Marquee has no text child!");
        return;
    }
    // Layout child text, child's width has no limit, marquee's width is constrained by parent.
    LayoutParam innerLayout;
    innerLayout.SetMaxSize(Size(Size::INFINITE_SIZE, GetLayoutParam().GetMaxSize().Height()));
    innerLayout.SetMinSize(GetLayoutParam().GetMinSize());
    childText_ = children.front();
    ACE_DCHECK(childText_);
    childText_->Layout(innerLayout);
    lastLayoutSize_ = GetLayoutSize();
    Size layoutSize = GetLayoutParam().Constrain(Size(Size::INFINITE_SIZE, childText_->GetLayoutSize().Height()));
    if (childPosition_.IsErrorOffset() || lastLayoutSize_ != layoutSize) {
        // Initialize child position.
        if (direction_ == MarqueeDirection::LEFT) {
            childPosition_ = Offset(layoutSize.Width(), 0.0);
        } else {
            childPosition_ = Offset(-childText_->GetLayoutSize().Width(), 0.0);
        }
    }
    childText_->SetPosition(childPosition_);
    SetLayoutSize(layoutSize);
    if (!NeedMarquee()) {
        Stop();
        childText_->SetPosition(GetTextPosition());
        return;
    }
    if (lastLayoutSize_ != childText_->GetLayoutSize() && IsPlayingAnimation(controller_)) {
        UpdateAnimation();
    }
    if (startAfterLayout_) {
        startAfterLayout_ = false;
        Start();
    }
}

void RenderMarquee::GetPlayerCtr(const RefPtr<Component>& component, bool emdit, bool control)
{
    const RefPtr<MarqueeComponent> marquee = AceType::DynamicCast<MarqueeComponent>(component);
    if (!marquee) {
        LOGE("marquee component is null");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    const auto& methodController = marquee->GetController();
    if (emdit) {
        if (control) {
            methodController->Start();
        } else {
            methodController->Stop();
            playerFinishControl_ = false;
        }
    }
}

void RenderMarquee::GetMarqueeCallback(const RefPtr<Component>& component)
{
    const RefPtr<MarqueeComponent> marquee = AceType::DynamicCast<MarqueeComponent>(component);
    if (!marquee) {
        LOGE("marquee component is null");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    if (marquee->GetOnStart()) {
        onStartEvent_ = *marquee->GetOnStart();
    }
    if (marquee->GetOnBounce()) {
        onBounceEvent_ = *marquee->GetOnBounce();
    }
    if (marquee->GetOnFinish()) {
        onFinishEvent_ = *marquee->GetOnFinish();
    }
}

bool RenderMarquee::NeedMarquee() const
{
    if (!childText_) {
        return true;
    }
    auto context = GetContext().Upgrade();
    if (!context) {
        return true;
    }
    // Is width of text longer than container.
    auto needMarquee = (childText_->GetLayoutSize().Width() > GetLayoutSize().Width());
    if (context->GetIsDeclarative()) {
        return needMarquee;
    }
    const static int32_t PLATFORM_VERSION_SIX = 6;
    if (context->GetMinPlatformVersion() >= PLATFORM_VERSION_SIX) {
        return needMarquee;
    }
    return true;
}

Offset RenderMarquee::GetTextPosition() const
{
    if (!childText_) {
        return Offset(0.0, 0.0);
    }

    auto textWidth = childText_->GetLayoutSize().Width();
    auto marqueeWidth = GetLayoutSize().Width();
    if (GreatOrEqual(textWidth, marqueeWidth)) {
        return Offset(0.0, 0.0);
    }

    auto textAlign = textStyle_.GetTextAlign();
    // adjust START and END
    if (textAlign == TextAlign::START) {
        textAlign = RenderNode::GetTextDirection() == TextDirection::LTR ? TextAlign::LEFT : TextAlign::RIGHT;
    }
    if (textAlign == TextAlign::END) {
        textAlign = RenderNode::GetTextDirection() == TextDirection::LTR ? TextAlign::RIGHT : TextAlign::LEFT;
    }

    const static double HALF_DIVIDE = 2.0;
    if (textAlign == TextAlign::CENTER) {
        return Offset((marqueeWidth - textWidth) / HALF_DIVIDE, 0.0);
    } else if (textAlign == TextAlign::RIGHT) {
        return Offset(marqueeWidth - textWidth, 0.0);
    } else {
        return Offset(0.0, 0.0);
    }
}

} // namespace OHOS::Ace

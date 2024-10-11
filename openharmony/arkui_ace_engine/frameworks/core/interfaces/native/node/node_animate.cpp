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

#include "core/interfaces/native/node/node_animate.h"

#include "base/error/error_code.h"
#include "core/animation/spring_curve.h"
#include "core/common/ace_engine.h"

namespace OHOS::Ace::NG::ViewAnimate {
namespace {
const std::vector<OHOS::Ace::RefPtr<OHOS::Ace::Curve>> CURVES_LIST = {
    OHOS::Ace::Curves::LINEAR,
    OHOS::Ace::Curves::EASE,
    OHOS::Ace::Curves::EASE_IN,
    OHOS::Ace::Curves::EASE_OUT,
    OHOS::Ace::Curves::EASE_IN_OUT,
    OHOS::Ace::Curves::FAST_OUT_SLOW_IN,
    OHOS::Ace::Curves::LINEAR_OUT_SLOW_IN,
    OHOS::Ace::Curves::FAST_OUT_LINEAR_IN,
    OHOS::Ace::Curves::EXTREME_DECELERATION,
    OHOS::Ace::Curves::SHARP,
    OHOS::Ace::Curves::RHYTHM,
    OHOS::Ace::Curves::SMOOTH,
    OHOS::Ace::Curves::FRICTION,
};

const std::vector<AnimationDirection> DIRECTION_LIST = {
    AnimationDirection::NORMAL,
    AnimationDirection::REVERSE,
    AnimationDirection::ALTERNATE,
    AnimationDirection::ALTERNATE_REVERSE,
};

enum class ArkUICurveType {
    CURVE_TYPE_CURVE = 0,
    CURVE_TYPE_STEPS,
    CURVE_TYPE_CUBIC_BEZIER,
    CURVE_TYPE_SPRING,
    CURVE_TYPE_SPRING_MOTION,
    CURVE_TYPE_RESPONSIVE_SPRING_MOTION,
    CURVE_TYPE_INTERPOLATING_SPRING,
    CURVE_TYPE_CUSTOM,
};
} // namespace

void AnimateToInner(ArkUIContext* context, AnimationOption& option, const std::function<void()>& animateToFunc,
    const std::function<void()>& onFinishFunc, bool immediately)
{
    CHECK_NULL_VOID(context);
    ContainerScope scope(context->id);
    auto containerSafely = Container::GetContainer(context->id);
    CHECK_NULL_VOID(containerSafely);
    auto pipelineContext = containerSafely->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);

    ACE_SCOPED_TRACE("duration:%d, curve:%s, iteration:%d", option.GetDuration(), option.GetCurve()->ToString().c_str(),
        option.GetIteration());
    auto triggerId = context->id;
    AceEngine::Get().NotifyContainers([triggerId, option](const RefPtr<Container>& container) {
        auto context = container->GetPipelineContext();
        if (!context) {
            // pa container do not have pipeline context.
            return;
        }
        if (!container->GetSettings().usingSharedRuntime) {
            return;
        }
        if (!container->IsFRSCardContainer() && !container->WindowIsShow()) {
            return;
        }
        ContainerScope scope(container->GetInstanceId());
        context->FlushBuild();
        if (context->GetInstanceId() == triggerId) {
            return;
        }
        context->PrepareOpenImplicitAnimation();
    });
    pipelineContext->OpenImplicitAnimation(option, option.GetCurve(), onFinishFunc);
    auto previousOption = pipelineContext->GetSyncAnimationOption();
    pipelineContext->SetSyncAnimationOption(option);
    // Execute the function.
    animateToFunc();
    pipelineContext->FlushOnceVsyncTask();
    AceEngine::Get().NotifyContainers([triggerId](const RefPtr<Container>& container) {
        auto context = container->GetPipelineContext();
        if (!context) {
            // pa container do not have pipeline context.
            return;
        }
        if (!container->GetSettings().usingSharedRuntime) {
            return;
        }
        if (!container->IsFRSCardContainer() && !container->WindowIsShow()) {
            return;
        }
        ContainerScope scope(container->GetInstanceId());
        context->FlushBuild();
        if (context->GetInstanceId() == triggerId) {
            return;
        }
        context->PrepareCloseImplicitAnimation();
    });
    pipelineContext->CloseImplicitAnimation();
    pipelineContext->SetSyncAnimationOption(previousOption);
    if (immediately) {
        pipelineContext->FlushMessages();
    } else {
        pipelineContext->RequestFrame();
    }
}

void AnimateTo(ArkUIContext* context, ArkUIAnimateOption option, void (*event)(void* userData), void* user)
{
    AnimationOption animationOption;
    animationOption.SetDuration(NearZero(static_cast<double>(option.tempo)) ? 0 : option.duration);
    animationOption.SetTempo(option.tempo);
    if (option.iCurve) {
        auto curve = reinterpret_cast<Curve*>(option.iCurve);
        animationOption.SetCurve(AceType::Claim(curve));
    } else {
        animationOption.SetCurve(
            CURVES_LIST[option.curve > static_cast<ArkUI_Int32>(CURVES_LIST.size()) ? 0 : option.curve]);
    }
    animationOption.SetDelay(option.delay);
    animationOption.SetIteration(option.iterations);
    animationOption.SetAnimationDirection(
        DIRECTION_LIST[option.playMode > static_cast<ArkUI_Int32>(DIRECTION_LIST.size()) ? 0 : option.playMode]);
    animationOption.SetFinishCallbackType(static_cast<FinishCallbackType>(option.finishCallbackType));

    if (option.expectedFrameRateRange) {
        RefPtr<FrameRateRange> frameRateRange = AceType::MakeRefPtr<FrameRateRange>(option.expectedFrameRateRange->min,
            option.expectedFrameRateRange->max, option.expectedFrameRateRange->expected);
        animationOption.SetFrameRateRange(frameRateRange);
    }

    auto onEvent = [event, user]() {
        if (event) {
            event(user);
        }
    };

    auto onFinishEvent = [option]() {
        if (option.onFinishCallback) {
            auto* onFinishFunc = reinterpret_cast<void (*)(void*)>(option.onFinishCallback);
            onFinishFunc(option.user);
        }
    };

    AnimateToInner(context, animationOption, onEvent, onFinishEvent, false);
}

void KeyframeAnimateTo(ArkUIContext* context, ArkUIKeyframeAnimateOption* animateOption)
{
    CHECK_NULL_VOID(context);
    ContainerScope scope(context->id);
    auto containerSafely = Container::GetContainer(context->id);
    CHECK_NULL_VOID(containerSafely);
    auto pipelineContext = containerSafely->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);

    AnimationOption option;
    if (animateOption->onFinish) {
        auto onFinishEvent = [onFinish = animateOption->onFinish, userData = animateOption->userData,
                                 id = context->id]() {
            ContainerScope scope(id);
            onFinish(userData);
        };
        option.SetOnFinishEvent(onFinishEvent);
    }

    option.SetDelay(animateOption->delay);
    option.SetIteration(animateOption->iterations);

    int duration = 0;
    for (int32_t i = 0; i < animateOption->keyframeSize; i++) {
        duration += animateOption->keyframes[i].duration;
    }
    option.SetDuration(duration);
    // actual curve is in keyframe, this curve will not be effective
    option.SetCurve(Curves::EASE_IN_OUT);
    pipelineContext->FlushBuild();
    pipelineContext->OpenImplicitAnimation(option, option.GetCurve(), option.GetOnFinishEvent());

    for (int32_t i = 0; i < animateOption->keyframeSize; i++) {
        auto keyframe = animateOption->keyframes[i];
        if (!keyframe.event) {
            continue;
        }
        auto event = [&keyframe, &pipelineContext]() {
            keyframe.event(keyframe.userData);
            pipelineContext->FlushBuild();
            if (!pipelineContext->IsLayouting()) {
                pipelineContext->FlushUITasks(true);
            }
        };
        if (keyframe.curve) {
            auto curve = reinterpret_cast<Curve*>(keyframe.curve);
            AnimationUtils::AddDurationKeyFrame(keyframe.duration, AceType::Claim(curve), event);
        } else {
            AnimationUtils::AddDurationKeyFrame(keyframe.duration, Curves::EASE_IN_OUT, event);
        }
    }
    pipelineContext->CloseImplicitAnimation();
    pipelineContext->FlushAfterLayoutCallbackInImplicitAnimationTask();
}

RefPtr<Animation<double>> ParseAnimatorAnimation(const ArkUIAnimatorOption* option)
{
    if (option->keyframeSize > 0) {
        auto keyframeAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
        for (int32_t i = 0; i < option->keyframeSize; i++) {
            auto keyframe = option->keyframes[i];
            auto animatorKeyframe = AceType::MakeRefPtr<Keyframe<double>>(keyframe.keyTime, keyframe.keyValue);
            if (keyframe.curve) {
                auto curve = reinterpret_cast<Curve*>(keyframe.curve);
                animatorKeyframe->SetCurve(AceType::Claim(curve));
            } else {
                animatorKeyframe->SetCurve(Curves::EASE_IN_OUT);
            }
            keyframeAnimation->AddKeyframe(animatorKeyframe);
        }
        return keyframeAnimation;
    } else {
        RefPtr<Animation<double>> animation;
        auto curve = reinterpret_cast<Curve*>(option->easing);
        if (curve) {
            animation = AceType::MakeRefPtr<CurveAnimation<double>>(option->begin, option->end, AceType::Claim(curve));
        } else {
            animation = AceType::MakeRefPtr<CurveAnimation<double>>(option->begin, option->end, Curves::EASE_IN_OUT);
        }
        return animation;
    }
}

void ParseAnimatorOption(const RefPtr<Animator>& animator, ArkUIAnimatorOption* option)
{
    animator->SetDuration(option->duration);
    animator->SetIteration(option->iterations);
    animator->SetAnimationDirection(
        DIRECTION_LIST[option->direction > static_cast<ArkUI_Int32>(DIRECTION_LIST.size()) ? 0 : option->direction]);
    animator->SetStartDelay(option->delay);
    animator->SetFillMode(static_cast<FillMode>(option->fill));

    if (option->isHasExpectedFrameRateRange) {
        FrameRateRange frameRateRange { option->expectedFrameRateRange.min, option->expectedFrameRateRange.max,
            option->expectedFrameRateRange.expected };
        animator->SetExpectedFrameRateRange(frameRateRange);
    }
}

void RegisterAnimatorOnFrameEvent(const RefPtr<Animator>& animator, ArkUIAnimatorOption* option)
{
    animator->ClearInterpolators();
    //onframe
    auto onFrameCallback = [callback = option->onFrame, userData = option->frameUserData](double value) {
        if (callback) {
            ArkUI_AnimatorOnFrameEvent event;
            event.progress = static_cast<ArkUI_Float32>(value);
            event.userData = userData;
            callback(&event);
        }
    };
    RefPtr<Animation<double>> animation = ParseAnimatorAnimation(option);
    if (animation) {
        animation->AddListener(onFrameCallback);
        animator->AddInterpolator(animation);
    }
}

void RegisterAnimatorEvent(const RefPtr<Animator>& animator, ArkUIAnimatorOption* option)
{
    animator->ClearStopListeners();
    animator->ClearIdleListeners();
    animator->ClearRepeatListeners();

    RegisterAnimatorOnFrameEvent(animator, option);
    
    //onfinish
    animator->AddStopListener([callback = option->onFinish, userData = option->finishUserData] {
        if (callback) {
            ArkUI_AnimatorEvent event;
            event.userData = userData;
            callback(&event);
        }
    });

    //oncancel
    animator->AddIdleListener([callback = option->onCancel, userData = option->cancelUserData] {
        if (callback) {
            ArkUI_AnimatorEvent event;
            event.userData = userData;
            callback(&event);
        }
    });

    //onRepeat
    animator->AddRepeatListener([callback = option->onRepeat, userData = option->repeatUserData] {
        if (callback) {
            ArkUI_AnimatorEvent event;
            event.userData = userData;
            callback(&event);
        }
    });
}

ArkUIAnimatorHandle CreateAnimator(ArkUIContext* context, ArkUIAnimatorOption* option)
{
    CHECK_NULL_RETURN(context, nullptr);
    ContainerScope scope(context->id);
    auto containerSafely = Container::GetContainer(context->id);
    CHECK_NULL_RETURN(containerSafely, nullptr);
    auto pipelineContext = containerSafely->GetPipelineContext();
    CHECK_NULL_RETURN(pipelineContext, nullptr);

    ArkUIAnimator* result = new ArkUIAnimator;
    auto animator = CREATE_ANIMATOR("ohos.animator");
    animator->AttachScheduler(pipelineContext);
    ParseAnimatorOption(animator, option);
    RegisterAnimatorEvent(animator, option);
    animator->IncRefCount();
    result->animator = AceType::RawPtr(animator);
    result->context = context;
    result->animatorOption = option;
    return result;
}

void DisposeAnimator(ArkUIAnimatorHandle animatorHandle)
{
    if (animatorHandle->animator) {
        auto* animator = reinterpret_cast<Animator*>(animatorHandle->animator);
        if (animator) {
            animator->DecRefCount();
        }
    }
    delete animatorHandle;
}

int32_t AnimatorReset(ArkUIAnimatorHandle animatorHandle, ArkUIAnimatorOption* option)
{
    CHECK_NULL_RETURN(animatorHandle, ERROR_CODE_PARAM_INVALID);
    CHECK_NULL_RETURN(animatorHandle->animator, ERROR_CODE_PARAM_INVALID);
    auto animator = reinterpret_cast<Animator*>(animatorHandle->animator);
    animator->Cancel();
    ParseAnimatorOption(AceType::Claim(animator), option);
    RegisterAnimatorEvent(AceType::Claim(animator), option);
    animatorHandle->animatorOption = option;
    animator->ResetIsReverse();
    return ERROR_CODE_NO_ERROR;
}

int32_t AnimatorPlay(ArkUIAnimatorHandle animatorHandle)
{
    CHECK_NULL_RETURN(animatorHandle, ERROR_CODE_PARAM_INVALID);
    CHECK_NULL_RETURN(animatorHandle->animator, ERROR_CODE_PARAM_INVALID);
    auto animator = reinterpret_cast<Animator*>(animatorHandle->animator);
    animator->Play();
    return ERROR_CODE_NO_ERROR;
}

int32_t AnimatorFinish(ArkUIAnimatorHandle animatorHandle)
{
    CHECK_NULL_RETURN(animatorHandle, ERROR_CODE_PARAM_INVALID);
    CHECK_NULL_RETURN(animatorHandle->animator, ERROR_CODE_PARAM_INVALID);
    auto animator = reinterpret_cast<Animator*>(animatorHandle->animator);
    animator->Finish();
    return ERROR_CODE_NO_ERROR;
}

int32_t AnimatorPause(ArkUIAnimatorHandle animatorHandle)
{
    CHECK_NULL_RETURN(animatorHandle, ERROR_CODE_PARAM_INVALID);
    CHECK_NULL_RETURN(animatorHandle->animator, ERROR_CODE_PARAM_INVALID);
    auto animator = reinterpret_cast<Animator*>(animatorHandle->animator);
    animator->Pause();
    return ERROR_CODE_NO_ERROR;
}

int32_t AnimatorCancel(ArkUIAnimatorHandle animatorHandle)
{
    CHECK_NULL_RETURN(animatorHandle, ERROR_CODE_PARAM_INVALID);
    CHECK_NULL_RETURN(animatorHandle->animator, ERROR_CODE_PARAM_INVALID);
    auto animator = reinterpret_cast<Animator*>(animatorHandle->animator);
    animator->Cancel();
    return ERROR_CODE_NO_ERROR;
}

int32_t AnimatorReverse(ArkUIAnimatorHandle animatorHandle)
{
    CHECK_NULL_RETURN(animatorHandle, ERROR_CODE_PARAM_INVALID);
    CHECK_NULL_RETURN(animatorHandle->animator, ERROR_CODE_PARAM_INVALID);
    auto animator = reinterpret_cast<Animator*>(animatorHandle->animator);
    animator->Reverse();
    return ERROR_CODE_NO_ERROR;
}

ArkUICurveHandle CreateCurve(ArkUI_Int32 curve)
{
    auto iCurve = AceType::RawPtr(CURVES_LIST[curve > static_cast<ArkUI_Int32>(CURVES_LIST.size()) ? 0 : curve]);
    iCurve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(iCurve);
}

ArkUICurveHandle CreateStepsCurve(ArkUI_Int32 count, ArkUI_Bool end)
{
    auto curve = AceType::MakeRefPtr<StepsCurve>(count, static_cast<StepsCurvePosition>(end));
    curve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(AceType::RawPtr(curve));
}

ArkUICurveHandle CreateCubicBezierCurve(ArkUI_Float32 x1, ArkUI_Float32 y1, ArkUI_Float32 x2, ArkUI_Float32 y2)
{
    auto curve = AceType::MakeRefPtr<CubicCurve>(x1, y1, x2, y2);
    curve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(AceType::RawPtr(curve));
}

ArkUICurveHandle CreateSpringCurve(
    ArkUI_Float32 velocity, ArkUI_Float32 mass, ArkUI_Float32 stiffness, ArkUI_Float32 damping)
{
    auto curve = AceType::MakeRefPtr<SpringCurve>(velocity, mass, stiffness, damping);
    curve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(AceType::RawPtr(curve));
}

ArkUICurveHandle CreateSpringMotion(
    ArkUI_Float32 response, ArkUI_Float32 dampingFraction, ArkUI_Float32 overlapDuration)
{
    auto curve = AceType::MakeRefPtr<ResponsiveSpringMotion>(response, dampingFraction, overlapDuration);
    curve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(AceType::RawPtr(curve));
}

ArkUICurveHandle CreateResponsiveSpringMotion(
    ArkUI_Float32 response, ArkUI_Float32 dampingFraction, ArkUI_Float32 overlapDuration)
{
    auto curve = AceType::MakeRefPtr<ResponsiveSpringMotion>(response, dampingFraction, overlapDuration);
    curve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(AceType::RawPtr(curve));
}

ArkUICurveHandle CreateInterpolatingSpring(
    ArkUI_Float32 velocity, ArkUI_Float32 mass, ArkUI_Float32 stiffness, ArkUI_Float32 damping)
{
    auto curve = AceType::MakeRefPtr<InterpolatingSpring>(velocity, mass, stiffness, damping);
    curve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(AceType::RawPtr(curve));
}

ArkUICurveHandle CreateCustomCurve(ArkUI_Float32 (*interpolate)(ArkUI_Float32 fraction, void* userData), void* userData)
{
    auto func = [interpolate, userData](float value) -> float {
        if (interpolate) {
            return interpolate(value, userData);
        }
        return 0.0f;
    };
    auto curve = AceType::MakeRefPtr<CustomCurve>(func);
    curve->IncRefCount();
    return reinterpret_cast<ArkUICurveHandle>(AceType::RawPtr(curve));
}

void DisposeCurve(ArkUICurveHandle curve)
{
    CHECK_NULL_VOID(curve);
    auto* curvePtr = reinterpret_cast<Curve*>(curve);
    if (curvePtr) {
        curvePtr->DecRefCount();
    }
}
} // namespace OHOS::Ace::NG::ViewAnimate
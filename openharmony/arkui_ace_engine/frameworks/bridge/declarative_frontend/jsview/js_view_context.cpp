/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/jsview/js_view_context.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>

#include "base/log/ace_trace.h"
#include "base/log/jank_frame_report.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "bridge/common/utils/engine_helper.h"
#include "bridge/common/utils/utils.h"
#include "bridge/declarative_frontend/engine/functions/js_animation_on_finish_function.h"
#include "bridge/declarative_frontend/engine/js_converter.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "bridge/declarative_frontend/jsview/js_tabs_feature.h"
#include "bridge/declarative_frontend/jsview/models/view_context_model_impl.h"
#include "core/animation/animation_pub.h"
#include "core/common/ace_engine.h"
#include "core/common/recorder/event_recorder.h"
#include "core/components/common/properties/animation_option.h"
#include "core/components_ng/base/view_stack_model.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/view_context/view_context_model_ng.h"
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_pattern.h"
#include "bridge/declarative_frontend/jsview/js_search.h"
#include "bridge/declarative_frontend/jsview/js_textfield.h"

#ifdef USE_ARK_ENGINE
#include "bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#endif

namespace OHOS::Ace {

std::unique_ptr<ViewContextModel> ViewContextModel::instance_ = nullptr;
std::mutex ViewContextModel::mutex_;

ViewContextModel* ViewContextModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::ViewContextModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::ViewContextModelNG());
            } else {
                instance_.reset(new Framework::ViewContextModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t DEFAULT_DURATION = 1000; // ms
constexpr int64_t MICROSEC_TO_MILLISEC = 1000;
constexpr int32_t INVALID_ID = -1;
constexpr int32_t INDEX_ONE = 1;
constexpr int32_t INDEX_TWO = 2;
constexpr int32_t LENGTH_ONE = 1;
constexpr int32_t LENGTH_TWO = 2;
constexpr int32_t LENGTH_THREE = 3;
constexpr int32_t MAX_FLUSH_COUNT = 2;
constexpr uint32_t DEBUG_DURATION = 150;

std::unordered_map<int32_t, std::string> UICONTEXT_ERROR_MAP = {
    { ERROR_CODE_BIND_SHEET_CONTENT_ERROR, "The bindSheetContent is incorrect." },
    { ERROR_CODE_BIND_SHEET_CONTENT_ALREADY_EXIST, "The bindSheetContent already exists." },
    { ERROR_CODE_BIND_SHEET_CONTENT_NOT_FOUND, "The bindSheetContent cannot be found." },
    { ERROR_CODE_TARGET_ID_NOT_EXIST, "The targetId does not exist." },
    { ERROR_CODE_TARGET_NOT_ON_MAIN_TREE, "The node of targetId is not in the component tree." },
    { ERROR_CODE_TARGET_NOT_PAGE_CHILD,
        "The node of targetId is not a child of the page node or NavDestination node." },
    { ERROR_CODE_INTERNAL_ERROR, "Internal error." },
    { ERROR_CODE_PARAM_INVALID, "Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;"
        "2. Incorrect parameter types; 3. Parameter verification failed." },
    { ERROR_CODE_DIALOG_CONTENT_ERROR, "The ComponentContent is incorrect. " },
    { ERROR_CODE_DIALOG_CONTENT_ALREADY_EXIST, "The ComponentContent already exists. " },
    { ERROR_CODE_DIALOG_CONTENT_NOT_FOUND, "The ComponentContent cannot be found. " },
    { ERROR_CODE_TARGET_INFO_NOT_EXIST, "The targetId does not exist. " },
    { ERROR_CODE_TARGET_NOT_ON_COMPONENT_TREE, "The node of targetId is not in the component tree. " }
};

void PrintAnimationInfo(const AnimationOption& option, AnimationInterface interface, const std::optional<int32_t>& cnt)
{
    auto animationInterfaceName = GetAnimationInterfaceName(interface);
    CHECK_NULL_VOID(animationInterfaceName);
    if (option.GetIteration() == ANIMATION_REPEAT_INFINITE) {
        if (interface == AnimationInterface::KEYFRAME_ANIMATE_TO) {
            TAG_LOGI(AceLogTag::ACE_ANIMATION,
                "keyframeAnimateTo iteration is infinite, remember to stop it. total duration:%{public}d",
                option.GetDuration());
        } else {
            TAG_LOGI(AceLogTag::ACE_ANIMATION,
                "%{public}s iteration is infinite. duration:%{public}d, curve:%{public}s",
                animationInterfaceName, option.GetDuration(), option.GetCurve()->ToString().c_str());
        }
        return;
    }
    if (cnt) {
        TAG_LOGI(AceLogTag::ACE_ANIMATION, "%{public}s starts, [%{public}s], finish cnt:%{public}d",
            animationInterfaceName, option.ToString().c_str(), cnt.value());
    }
}

// check whether this container needs to perform animateTo
bool CheckContainer(const RefPtr<Container>& container)
{
    auto context = container->GetPipelineContext();
    if (!context) {
        return false;
    }
    if (!container->GetSettings().usingSharedRuntime) {
        return false;
    }
    if (!container->IsFRSCardContainer() && !container->WindowIsShow()) {
        return false;
    }
    auto executor = container->GetTaskExecutor();
    CHECK_NULL_RETURN(executor, false);
    return executor->WillRunOnCurrentThread(TaskExecutor::TaskType::UI);
}

bool GetAnyContextIsLayouting(const RefPtr<PipelineBase>& currentPipeline)
{
    if (currentPipeline->IsLayouting()) {
        return true;
    }
    bool isLayouting = false;
    AceEngine::Get().NotifyContainers([&isLayouting](const RefPtr<Container>& container) {
        if (isLayouting) {
            // One container is already in layouting
            return;
        }
        if (!CheckContainer(container)) {
            return;
        }
        auto context = container->GetPipelineContext();
        isLayouting |= context->IsLayouting();
    });
    return isLayouting;
}

void AnimateToForStageMode(const RefPtr<PipelineBase>& pipelineContext, const AnimationOption& option,
    JSRef<JSFunc> jsAnimateToFunc, int32_t triggerId, const std::optional<int32_t>& count)
{
    pipelineContext->StartImplicitAnimation(option, option.GetCurve(), option.GetOnFinishEvent(), count);
    auto previousOption = pipelineContext->GetSyncAnimationOption();
    pipelineContext->SetSyncAnimationOption(option);
    // Execute the function.
    jsAnimateToFunc->Call(jsAnimateToFunc);
    pipelineContext->FlushOnceVsyncTask();
    AceEngine::Get().NotifyContainersOrderly([triggerId,
        multiInstanceEnabled = SystemProperties::GetMultiInstanceEnabled()](const RefPtr<Container>& container) {
        if (!CheckContainer(container)) {
            return;
        }
        auto context = container->GetPipelineContext();
        ContainerScope scope(container->GetInstanceId());
        context->FlushBuild();
        if (context->GetInstanceId() == triggerId) {
            return;
        }
        context->PrepareCloseImplicitAnimation();
        if (multiInstanceEnabled) {
            AnimationUtils::CloseImplicitAnimation(context);
        }
    });
    pipelineContext->CloseImplicitAnimation();
    pipelineContext->SetSyncAnimationOption(previousOption);
}

void FlushDirtyNodesWhenExist(const RefPtr<PipelineBase>& pipelineContext,
    const AnimationOption& option, const std::optional<int32_t>& count, AnimationInterface interface)
{
    auto animationInterfaceName = GetAnimationInterfaceName(interface);
    CHECK_NULL_VOID(animationInterfaceName);
    int32_t flushCount = 0;
    bool isDirtyNodesEmpty = pipelineContext->IsDirtyNodesEmpty();
    bool isDirtyLayoutNodesEmpty = pipelineContext->IsDirtyLayoutNodesEmpty();
    while (!isDirtyNodesEmpty || (!isDirtyLayoutNodesEmpty && !pipelineContext->IsLayouting())) {
        if (flushCount >= MAX_FLUSH_COUNT || option.GetIteration() != ANIMATION_REPEAT_INFINITE) {
            TAG_LOGD(AceLogTag::ACE_ANIMATION, "%{public}s, option:%{public}s, finish cnt:%{public}d,"
                "dirtyNodes is empty:%{public}d, dirtyLayoutNodes is empty:%{public}d",
                animationInterfaceName, option.ToString().c_str(), count.value_or(-1),
                isDirtyNodesEmpty, isDirtyLayoutNodesEmpty);
            break;
        }
        if (!isDirtyNodesEmpty) {
            pipelineContext->FlushBuild();
            isDirtyLayoutNodesEmpty = pipelineContext->IsDirtyLayoutNodesEmpty();
        }
        if (!isDirtyLayoutNodesEmpty && !pipelineContext->IsLayouting()) {
            pipelineContext->FlushUITasks(true);
        }
        isDirtyNodesEmpty = pipelineContext->IsDirtyNodesEmpty();
        isDirtyLayoutNodesEmpty = pipelineContext->IsDirtyLayoutNodesEmpty();
        flushCount++;
    }
}

void StartAnimationForStageMode(const RefPtr<PipelineBase>& pipelineContext, const AnimationOption& option,
    JSRef<JSFunc> jsAnimateToFunc, const std::optional<int32_t>& count, bool immediately)
{
    auto triggerId = pipelineContext->GetInstanceId();
    ACE_SCOPED_TRACE("%s, instanceId:%d, finish cnt:%d", option.ToString().c_str(), triggerId, count.value_or(-1));
    PrintAnimationInfo(
        option, immediately ? AnimationInterface::ANIMATE_TO_IMMEDIATELY : AnimationInterface::ANIMATE_TO, count);
    if (!ViewStackModel::GetInstance()->IsEmptyStack()) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION,
            "when call animateTo, node stack is not empty, not suitable for animateTo."
            "param is [option:%{public}s]", option.ToString().c_str());
    }
    NG::ScopedViewStackProcessor scopedProcessor;
    AceEngine::Get().NotifyContainersOrderly([triggerId, &option,
        multiInstanceEnabled = SystemProperties::GetMultiInstanceEnabled()](const RefPtr<Container>& container) {
        if (!CheckContainer(container)) {
            return;
        }
        auto context = container->GetPipelineContext();
        ContainerScope scope(container->GetInstanceId());
        context->FlushBuild();
        if (context->GetInstanceId() == triggerId) {
            return;
        }
        context->PrepareOpenImplicitAnimation();
        if (multiInstanceEnabled) {
            AnimationUtils::OpenImplicitAnimation(option, option.GetCurve(), nullptr, context);
        }
    });
    pipelineContext->PrepareOpenImplicitAnimation();
    FlushDirtyNodesWhenExist(pipelineContext, option, count,
        immediately ? AnimationInterface::ANIMATE_TO_IMMEDIATELY : AnimationInterface::ANIMATE_TO);
    if (!pipelineContext->CatchInteractiveAnimations([pipelineContext, option, jsAnimateToFunc, triggerId, count]() {
        AnimateToForStageMode(pipelineContext, option, jsAnimateToFunc, triggerId, count);
    })) {
        AnimateToForStageMode(pipelineContext, option, jsAnimateToFunc, triggerId, count);
    }
    pipelineContext->FlushAfterLayoutCallbackInImplicitAnimationTask();
    if (immediately) {
        pipelineContext->FlushModifier();
        pipelineContext->FlushMessages();
        JankFrameReport::GetInstance().RecordAnimateEnd();
    } else {
        pipelineContext->RequestFrame();
    }
}

void StartAnimateToForFaMode(const RefPtr<PipelineBase>& pipelineContext, AnimationOption& option,
    JSRef<JSFunc> jsAnimateToFunc, const std::optional<int32_t>& count, bool immediately)
{
    ACE_SCOPED_TRACE("%s, instanceId:%d, finish cnt:%d", option.ToString().c_str(), pipelineContext->GetInstanceId(),
        count.value_or(-1));
    PrintAnimationInfo(
        option, immediately ? AnimationInterface::ANIMATE_TO_IMMEDIATELY : AnimationInterface::ANIMATE_TO, count);
    if (!ViewStackModel::GetInstance()->IsEmptyStack()) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION,
            "when call animateTo, node stack is not empty, not suitable for animateTo. param is [duration:%{public}d, "
            "curve:%{public}s, iteration:%{public}d]",
            option.GetDuration(), option.GetCurve()->ToString().c_str(), option.GetIteration());
    }
    NG::ScopedViewStackProcessor scopedProcessor;
    pipelineContext->FlushBuild();
    pipelineContext->OpenImplicitAnimation(option, option.GetCurve(), option.GetOnFinishEvent());
    auto previousOption = pipelineContext->GetSyncAnimationOption();
    pipelineContext->SetSyncAnimationOption(option);
    jsAnimateToFunc->Call(jsAnimateToFunc);
    pipelineContext->FlushBuild();
    pipelineContext->CloseImplicitAnimation();
    pipelineContext->SetSyncAnimationOption(previousOption);
    if (immediately) {
        pipelineContext->FlushModifier();
        pipelineContext->FlushMessages();
        JankFrameReport::GetInstance().RecordAnimateEnd();
    } else {
        pipelineContext->RequestFrame();
    }
}

int64_t GetFormAnimationTimeInterval(const RefPtr<PipelineBase>& pipelineContext)
{
    CHECK_NULL_RETURN(pipelineContext, 0);
    return (GetMicroTickCount() - pipelineContext->GetFormAnimationStartTime()) / MICROSEC_TO_MILLISEC;
}

bool CheckIfSetFormAnimationDuration(const RefPtr<PipelineBase>& pipelineContext, const AnimationOption& option)
{
    CHECK_NULL_RETURN(pipelineContext, false);
    return pipelineContext->IsFormAnimationFinishCallback() && pipelineContext->IsFormRenderExceptDynamicComponent() &&
        option.GetDuration() > (DEFAULT_DURATION - GetFormAnimationTimeInterval(pipelineContext));
}

std::function<float(float)> ParseCallBackFunction(const JSRef<JSObject>& curveObj)
{
    std::function<float(float)> customCallBack = nullptr;
    JSRef<JSVal> onCallBack = curveObj->GetProperty("__curveCustomFunc");
    if (onCallBack->IsFunction()) {
        auto frameNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
        RefPtr<JsFunction> jsFuncCallBack =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onCallBack));
        customCallBack = [func = std::move(jsFuncCallBack), id = Container::CurrentIdSafely(), node = frameNode](
                             float time) -> float {
            ContainerScope scope(id);
            auto pipelineContext = PipelineContext::GetCurrentContextSafely();
            CHECK_NULL_RETURN(pipelineContext, 1.0f);
            pipelineContext->UpdateCurrentActiveNode(node);
            JSRef<JSVal> params[1];
            params[0] = JSRef<JSVal>::Make(ToJSValue(time));
            auto result = func->ExecuteJS(1, params);
            return result->IsNumber() ? result->ToNumber<float>() : 1.0f;
        };
    }
    return customCallBack;
}

struct KeyframeParam {
    int32_t duration = 0;
    RefPtr<Curve> curve;
    std::function<void()> animationClosure;
};

AnimationOption ParseKeyframeOverallParam(const JSExecutionContext& executionContext,
    const JSRef<JSObject>& obj, std::optional<int32_t>& count)
{
    JSRef<JSVal> onFinish = obj->GetProperty("onFinish");
    AnimationOption option;
    if (onFinish->IsFunction()) {
        count = GetAnimationFinishCount();
        auto jsFunc = AceType::MakeRefPtr<JsAnimationOnFinishFunction>(JSRef<JSFunc>::Cast(onFinish));
        std::function<void()> onFinishEvent = [execCtx = executionContext, func = std::move(jsFunc),
                            id = Container::CurrentIdSafely()]() mutable {
            CHECK_NULL_VOID(func);
            ContainerScope scope(id);
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute(false);
            func = nullptr;
        };
        option.SetOnFinishEvent(onFinishEvent);
    }
    auto delay = obj->GetPropertyValue<int32_t>("delay", 0);
    auto iterations = obj->GetPropertyValue<int32_t>("iterations", 1);
    JSRef<JSVal> rateRangeObjectArgs = obj->GetProperty("expectedFrameRateRange");
    if (rateRangeObjectArgs->IsObject()) {
        JSRef<JSObject> rateRangeObj = JSRef<JSObject>::Cast(rateRangeObjectArgs);
        int32_t fRRmin = rateRangeObj->GetPropertyValue<int32_t>("min", -1);
        int32_t fRRmax = rateRangeObj->GetPropertyValue<int32_t>("max", -1);
        int32_t fRRExpected = rateRangeObj->GetPropertyValue<int32_t>("expected", -1);
        TAG_LOGD(AceLogTag::ACE_ANIMATION, "[keyframe] SetExpectedFrameRateRange"
            "{%{public}d, %{public}d, %{public}d}", fRRmin, fRRmax, fRRExpected);
        RefPtr<FrameRateRange> frameRateRange = AceType::MakeRefPtr<FrameRateRange>(fRRmin, fRRmax, fRRExpected);
        option.SetFrameRateRange(frameRateRange);
    }
    option.SetDelay(delay);
    option.SetIteration(iterations);
    return option;
}

std::vector<KeyframeParam> ParseKeyframes(const JSExecutionContext& executionContext, const JSRef<JSArray>& arr)
{
    std::vector<KeyframeParam> params;
    for (size_t index = 0; index != arr->Length(); ++index) {
        if (!arr->GetValueAt(index)->IsObject()) {
            continue;
        }
        auto info = JSRef<JSObject>::Cast(arr->GetValueAt(index));
        KeyframeParam param;

        auto jsEventValue = info->GetProperty("event");
        if (!jsEventValue->IsFunction()) {
            continue;
        }
        param.duration = info->GetPropertyValue<int32_t>("duration", DEFAULT_DURATION);
        if (param.duration < 0) {
            param.duration = 0;
        }
        RefPtr<JsFunction> jsFunc =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(jsEventValue));
        param.animationClosure = [execCtx = executionContext, func = std::move(jsFunc)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute();
        };
        auto curveArgs = info->GetProperty("curve");
        param.curve = JSViewContext::ParseCurve(curveArgs, true);
        params.emplace_back(param);
    }
    return params;
}

napi_value CreateErrorValue(napi_env env, int32_t errCode, const std::string& errMsg = "")
{
    napi_value code = nullptr;
    std::string codeStr = std::to_string(errCode);
    napi_create_string_utf8(env, codeStr.c_str(), codeStr.length(), &code);
    napi_value msg = nullptr;
    napi_create_string_utf8(env, errMsg.c_str(), errMsg.length(), &msg);
    napi_value error = nullptr;
    napi_create_error(env, code, msg, &error);
    return error;
}

RefPtr<NG::FrameNode> ParseContentNode(const JSCallbackInfo& info)
{
    EcmaVM* vm = info.GetVm();
    CHECK_NULL_RETURN(vm, nullptr);
    auto jsTargetNode = info[0];
    auto* targetNodePtr = jsTargetNode->GetLocalHandle()->ToNativePointer(vm)->Value();
    CHECK_NULL_RETURN(targetNodePtr, nullptr);
    NG::FrameNode* sheetContentNode = reinterpret_cast<NG::FrameNode*>(targetNodePtr);
    CHECK_NULL_RETURN(sheetContentNode, nullptr);
    return AceType::Claim(sheetContentNode);
}

void ReturnPromise(const JSCallbackInfo& info, int32_t errCode)
{
    auto engine = EngineHelper::GetCurrentEngine();
    CHECK_NULL_VOID(engine);
    NativeEngine* nativeEngine = engine->GetNativeEngine();
    auto env = reinterpret_cast<napi_env>(nativeEngine);
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);

    if (errCode != ERROR_CODE_NO_ERROR) {
        napi_value result = CreateErrorValue(env, errCode, UICONTEXT_ERROR_MAP[errCode]);
        napi_reject_deferred(env, deferred, result);
    } else {
        napi_value result = nullptr;
        napi_get_undefined(env, &result);
        napi_resolve_deferred(env, deferred, result);
    }
    CHECK_NULL_VOID(promise);
    auto jsPromise = JsConverter::ConvertNapiValueToJsVal(promise);
    if (!jsPromise->IsObject()) {
        TAG_LOGE(AceLogTag::ACE_SHEET, "Return promise failed.");
        return;
    }
    info.SetReturnValue(JSRef<JSObject>::Cast(jsPromise));
}

void StartKeyframeAnimation(const RefPtr<PipelineBase>& pipelineContext, AnimationOption& overallAnimationOption,
    std::vector<KeyframeParam>& keyframes, const std::optional<int32_t>& count)
{
    // flush build and flush ui tasks before open animation closure.
    pipelineContext->FlushBuild();
    if (!pipelineContext->IsLayouting()) {
        pipelineContext->FlushUITasks(true);
    }

    // flush build when exist dirty nodes, flush ui tasks when exist dirty layout nodes.
    FlushDirtyNodesWhenExist(pipelineContext,
        overallAnimationOption, count, AnimationInterface::KEYFRAME_ANIMATE_TO);

    // start KeyframeAnimation.
    pipelineContext->StartImplicitAnimation(
        overallAnimationOption, overallAnimationOption.GetCurve(), overallAnimationOption.GetOnFinishEvent(), count);
    for (auto& keyframe : keyframes) {
        if (!keyframe.animationClosure) {
            continue;
        }
        AceTraceBeginWithArgs("keyframe duration%d", keyframe.duration);
        AnimationUtils::AddDurationKeyFrame(keyframe.duration, keyframe.curve, [&keyframe, &pipelineContext]() {
            keyframe.animationClosure();
            pipelineContext->FlushBuild();
            if (!pipelineContext->IsLayouting()) {
                pipelineContext->FlushUITasks(true);
            } else {
                TAG_LOGI(AceLogTag::ACE_ANIMATION, "isLayouting, maybe some layout keyframe animation not generated");
            }
        });
        AceTraceEnd();
    }

    // close KeyframeAnimation.
    AnimationUtils::CloseImplicitAnimation();
}
} // namespace

RefPtr<Curve> JSViewContext::ParseCurve(const JSRef<JSVal>& curveArgs, bool exceptSpring)
{
    RefPtr<Curve> curve;
    if (curveArgs->IsString()) {
        auto curveString = curveArgs->ToString();
        if (exceptSpring) {
            curve = CreateCurveExceptSpring(curveString);
        } else {
            curve = CreateCurve(curveString);
        }
    } else if (curveArgs->IsObject()) {
        JSRef<JSObject> curveObject = JSRef<JSObject>::Cast(curveArgs);
        JSRef<JSVal> curveString = curveObject->GetProperty("__curveString");
        if (!curveString->IsString()) {
            return Curves::EASE_IN_OUT;
        }
        auto aniTimFunc = curveString->ToString();
        std::string customFuncName(DOM_ANIMATION_TIMING_FUNCTION_CUSTOM);
        if (aniTimFunc == customFuncName) {
            auto customCurveFunc = ParseCallBackFunction(curveObject);
            curve = CreateCurve(customCurveFunc);
        } else if (exceptSpring) {
            curve = CreateCurveExceptSpring(aniTimFunc);
        } else {
            curve = CreateCurve(aniTimFunc);
        }
    } else {
        curve = Curves::EASE_IN_OUT;
    }
    return curve;
}

const AnimationOption JSViewContext::CreateAnimation(const JSRef<JSObject>& animationArgs, bool isForm)
{
    AnimationOption option;
    // If the attribute does not exist, the default value is used.
    auto duration = animationArgs->GetPropertyValue<int32_t>("duration", DEFAULT_DURATION);
    auto delay = animationArgs->GetPropertyValue<int32_t>("delay", 0);
    auto iterations = animationArgs->GetPropertyValue<int32_t>("iterations", 1);
    auto tempo = animationArgs->GetPropertyValue<double>("tempo", 1.0);
    if (SystemProperties::GetRosenBackendEnabled() && NearZero(tempo)) {
        // set duration to 0 to disable animation.
        duration = 0;
    }
    auto direction = StringToAnimationDirection(animationArgs->GetPropertyValue<std::string>("playMode", "normal"));
    auto finishCallbackType = static_cast<FinishCallbackType>(
        animationArgs->GetPropertyValue<int32_t>("finishCallbackType", 0));
    auto curve = ParseCurve(animationArgs->GetProperty("curve"));

    // limit animation for ArkTS Form
    if (isForm) {
        if (duration > static_cast<int32_t>(DEFAULT_DURATION)) {
            duration = static_cast<int32_t>(DEFAULT_DURATION);
        }
        if (delay != 0) {
            delay = 0;
        }
        if (SystemProperties::IsFormAnimationLimited() && iterations != 1) {
            iterations = 1;
        }
        if (!NearEqual(tempo, 1.0)) {
            tempo = 1.0;
        }
    }

    int32_t fRRmin = 0;
    int32_t fRRmax = 0;
    int32_t fRRExpected = 0;
    JSRef<JSVal> rateRangeObjectArgs = animationArgs->GetProperty("expectedFrameRateRange");
    if (rateRangeObjectArgs->IsObject()) {
        JSRef<JSObject> rateRangeObj = JSRef<JSObject>::Cast(rateRangeObjectArgs);
        fRRmin = rateRangeObj->GetPropertyValue<int32_t>("min", -1);
        fRRmax = rateRangeObj->GetPropertyValue<int32_t>("max", -1);
        fRRExpected = rateRangeObj->GetPropertyValue<int32_t>("expected", -1);
        TAG_LOGD(AceLogTag::ACE_ANIMATION, "[animation/animateTo] SetExpectedFrameRateRange"
            "{%{public}d, %{public}d, %{public}d}", fRRmin, fRRmax, fRRExpected);
    }
    RefPtr<FrameRateRange> frameRateRange = AceType::MakeRefPtr<FrameRateRange>(fRRmin, fRRmax, fRRExpected);

    option.SetDuration(duration);
    option.SetDelay(delay);
    option.SetIteration(iterations);
    option.SetTempo(tempo);
    option.SetAnimationDirection(direction);
    option.SetCurve(curve);
    option.SetFinishCallbackType(finishCallbackType);
    option.SetFrameRateRange(frameRateRange);
    return option;
}

void JSViewContext::JSAnimation(const JSCallbackInfo& info)
{
    ACE_FUNCTION_TRACE();
    auto scopedDelegate = EngineHelper::GetCurrentDelegateSafely();
    if (!scopedDelegate) {
        // this case usually means there is no foreground container, need to figure out the reason.
        return;
    }
    if (ViewStackModel::GetInstance()->CheckTopNodeFirstBuilding()) {
        // the node sets attribute value for the first time. No animation is generated.
        return;
    }
    AnimationOption option = AnimationOption();
    auto container = Container::CurrentSafely();
    CHECK_NULL_VOID(container);
    auto pipelineContextBase = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContextBase);
    if (pipelineContextBase->IsFormAnimationFinishCallback() &&
        pipelineContextBase->IsFormRenderExceptDynamicComponent() &&
        GetFormAnimationTimeInterval(pipelineContextBase) > DEFAULT_DURATION) {
        TAG_LOGW(
            AceLogTag::ACE_FORM, "[Form animation] Form finish callback triggered animation cannot exceed 1000ms.");
        return;
    }
    if (info[0]->IsNull() || !info[0]->IsObject()) {
        ViewContextModel::GetInstance()->closeAnimation(option, true);
        return;
    }
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> onFinish = obj->GetProperty("onFinish");
    std::function<void()> onFinishEvent;
    if (onFinish->IsFunction()) {
        auto frameNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onFinish));
        onFinishEvent = [execCtx = info.GetExecutionContext(), func = std::move(jsFunc),
                            id = Container::CurrentIdSafely(), node = frameNode]() mutable {
            CHECK_NULL_VOID(func);
            ContainerScope scope(id);
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            auto pipelineContext = PipelineContext::GetCurrentContextSafely();
            CHECK_NULL_VOID(pipelineContext);
            pipelineContext->UpdateCurrentActiveNode(node);
            func->Execute();
            func = nullptr;
        };
    }

    option = CreateAnimation(obj, pipelineContextBase->IsFormRenderExceptDynamicComponent());
    if (pipelineContextBase->IsFormAnimationFinishCallback() &&
        pipelineContextBase->IsFormRenderExceptDynamicComponent() &&
        option.GetDuration() > (DEFAULT_DURATION - GetFormAnimationTimeInterval(pipelineContextBase))) {
        option.SetDuration(DEFAULT_DURATION - GetFormAnimationTimeInterval(pipelineContextBase));
        TAG_LOGW(AceLogTag::ACE_FORM, "[Form animation]  Form animation SetDuration: %{public}lld ms",
            static_cast<long long>(DEFAULT_DURATION - GetFormAnimationTimeInterval(pipelineContextBase)));
    }

    option.SetOnFinishEvent(onFinishEvent);
    if (SystemProperties::GetRosenBackendEnabled()) {
        option.SetAllowRunningAsynchronously(true);
    }
    PrintAnimationInfo(option, AnimationInterface::ANIMATION, std::nullopt);
    AceScopedTrace paramTrace("duration:%d, curve:%s, iteration:%d", option.GetDuration(),
        option.GetCurve()->ToString().c_str(), option.GetIteration());
    option.SetAnimationInterface(AnimationInterface::ANIMATION);
    ViewContextModel::GetInstance()->openAnimation(option);
    JankFrameReport::GetInstance().ReportJSAnimation();
}

void JSViewContext::JSAnimateTo(const JSCallbackInfo& info)
{
    ACE_FUNCTION_TRACE();
    AnimateToInner(info, false);
}

void JSViewContext::JSAnimateToImmediately(const JSCallbackInfo& info)
{
    ACE_FUNCTION_TRACE();
    AnimateToInner(info, true);
}

void RecordAnimationFinished(int32_t count)
{
    if (Recorder::EventRecorder::Get().IsRecordEnable(Recorder::EventCategory::CATEGORY_ANIMATION)) {
        Recorder::EventParamsBuilder builder;
        builder.SetEventCategory(Recorder::EventCategory::CATEGORY_ANIMATION)
            .SetEventType(Recorder::EventType::ANIMATION_FINISHED)
            .SetExtra(Recorder::KEY_COUNT, std::to_string(count));
        Recorder::EventRecorder::Get().OnEvent(std::move(builder));
    }
}

void JSViewContext::AnimateToInner(const JSCallbackInfo& info, bool immediately)
{
    auto currentId = Container::CurrentIdSafelyWithCheck();
    bool needCheck = false;
    if (!Container::CheckRunOnThreadByThreadId(currentId, false)) {
        // fix DynamicComponent get wrong container when calling the animateTo function.
        auto localContainerId = ContainerScope::CurrentLocalId();
        TAG_LOGI(AceLogTag::ACE_ANIMATION,
            "AnimateToInner not run on running thread, currentId: %{public}d, localId: %{public}d",
            currentId, localContainerId);
        if (localContainerId > 0 && Container::CheckRunOnThreadByThreadId(localContainerId, false)) {
            currentId = localContainerId;
        } else {
            needCheck = true;
        }
    }
    ContainerScope scope(currentId);
    auto scopedDelegate = EngineHelper::GetCurrentDelegateSafely();
    if (!scopedDelegate) {
        // this case usually means there is no foreground container, need to figure out the reason.
        const char* funcName = immediately ? "animateToImmediately" : "animateTo";
        TAG_LOGW(AceLogTag::ACE_ANIMATION,
            "can not find current context ,%{public}s faild, please use uiContext.%{public}s to specify the context",
            funcName, funcName);
        return;
    }
    if (info.Length() < 2) {
        return;
    }
    if (!info[0]->IsObject()) {
        return;
    }
    // 2nd argument should be a closure passed to the animateTo function.
    if (!info[1]->IsFunction()) {
        return;
    }

    auto container = Container::CurrentSafely();
    CHECK_NULL_VOID(container);
    if (needCheck && !Container::CheckRunOnThreadByThreadId(container->GetInstanceId(), false)) {
        const char* funcName = immediately ? "animateToImmediately" : "animateTo";
        TAG_LOGW(AceLogTag::ACE_ANIMATION,
            "the context found cannot run on current thread, %{public}s failed, "
            "please use uiContext.%{public}s to specify the context",
            funcName, funcName);
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    if (pipelineContext->IsFormAnimationFinishCallback() && pipelineContext->IsFormRenderExceptDynamicComponent() &&
        GetFormAnimationTimeInterval(pipelineContext) > DEFAULT_DURATION) {
        TAG_LOGW(
            AceLogTag::ACE_FORM, "[Form animation] Form finish callback triggered animation cannot exceed 1000ms.");
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    AnimationOption option = CreateAnimation(obj, pipelineContext->IsFormRenderExceptDynamicComponent());
    auto iterations = option.GetIteration();
    JSRef<JSVal> onFinish = obj->GetProperty("onFinish");
    std::function<void()> onFinishEvent;
    std::optional<int32_t> count;
    auto traceStreamPtr = std::make_shared<std::stringstream>();
    RefPtr<Curve> debugCurve = Curves::FAST_OUT_LINEAR_IN;
    auto isDebugAnim = option.GetDuration() == DEBUG_DURATION && debugCurve->IsEqual(option.GetCurve());
    if (onFinish->IsFunction()) {
        count = GetAnimationFinishCount();
        auto frameNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
        auto jsFunc = AceType::MakeRefPtr<JsAnimationOnFinishFunction>(JSRef<JSFunc>::Cast(onFinish));
        onFinishEvent = [execCtx = info.GetExecutionContext(), func = std::move(jsFunc),
                            id = Container::CurrentIdSafely(), traceStreamPtr, node = frameNode, count,
                            iterations, isDebugAnim]() mutable {
            RecordAnimationFinished(iterations);
            CHECK_NULL_VOID(func);
            ContainerScope scope(id);
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCOPED_TRACE("onFinish[cnt:%d]", count.value());
            auto pipelineContext = PipelineContext::GetCurrentContextSafely();
            CHECK_NULL_VOID(pipelineContext);
            pipelineContext->UpdateCurrentActiveNode(node);
            TAG_LOGI(AceLogTag::ACE_ANIMATION, "animateTo finish, cnt:%{public}d", count.value());
            func->Execute(isDebugAnim);
            if (isDebugAnim) {
                TAG_LOGI(AceLogTag::ACE_ANIMATION, "animateTo finish after ExecuteJS, cnt:%{public}d", count.value());
            }
            func = nullptr;
            AceAsyncTraceEnd(0, traceStreamPtr->str().c_str(), true);
        };
    } else {
        onFinishEvent = [traceStreamPtr, iterations]() {
            RecordAnimationFinished(iterations);
            AceAsyncTraceEnd(0, traceStreamPtr->str().c_str(), true);
        };
    }

    option.SetOnFinishEvent(onFinishEvent);
    option.SetAnimationInterface(
        immediately ? AnimationInterface::ANIMATE_TO_IMMEDIATELY : AnimationInterface::ANIMATE_TO);
    *traceStreamPtr << "AnimateTo, Options"
                    << " duration:" << option.GetDuration()
                    << ",iteration:" << option.GetIteration()
                    << ",delay:" << option.GetDelay()
                    << ",tempo:" << option.GetTempo()
                    << ",direction:" << (uint32_t) option.GetAnimationDirection()
                    << ",curve:" << (option.GetCurve() ? option.GetCurve()->ToString().c_str() : "");
    AceAsyncTraceBegin(0, traceStreamPtr->str().c_str(), true);
    if (CheckIfSetFormAnimationDuration(pipelineContext, option)) {
        option.SetDuration(DEFAULT_DURATION - GetFormAnimationTimeInterval(pipelineContext));
        TAG_LOGW(AceLogTag::ACE_FORM, "[Form animation]  Form animation SetDuration: %{public}lld ms",
            static_cast<long long>(DEFAULT_DURATION - GetFormAnimationTimeInterval(pipelineContext)));
    }
    if (SystemProperties::GetRosenBackendEnabled()) {
        bool usingSharedRuntime = container->GetSettings().usingSharedRuntime;
        if (usingSharedRuntime) {
            if (GetAnyContextIsLayouting(pipelineContext)) {
                TAG_LOGW(AceLogTag::ACE_ANIMATION,
                    "Pipeline layouting, post animateTo, dur:%{public}d, curve:%{public}s",
                    option.GetDuration(), option.GetCurve() ? option.GetCurve()->ToString().c_str() : "");
                pipelineContext->GetTaskExecutor()->PostTask(
                    [id = Container::CurrentIdSafely(), option, func = JSRef<JSFunc>::Cast(info[1]), count,
                        immediately]() mutable {
                        ContainerScope scope(id);
                        auto container = Container::CurrentSafely();
                        CHECK_NULL_VOID(container);
                        auto pipelineContext = container->GetPipelineContext();
                        CHECK_NULL_VOID(pipelineContext);
                        StartAnimationForStageMode(pipelineContext, option, func, count, immediately);
                    },
                    TaskExecutor::TaskType::UI, "ArkUIAnimateToForStageMode", PriorityType::IMMEDIATE);
                return;
            }
            StartAnimationForStageMode(pipelineContext, option, JSRef<JSFunc>::Cast(info[1]), count, immediately);
        } else {
            StartAnimateToForFaMode(pipelineContext, option, JSRef<JSFunc>::Cast(info[1]), count, immediately);
        }
    } else {
        pipelineContext->FlushBuild();
        pipelineContext->SaveExplicitAnimationOption(option);
        // Execute the function.
        JSRef<JSFunc> jsAnimateToFunc = JSRef<JSFunc>::Cast(info[1]);
        jsAnimateToFunc->Call(info[1]);
        pipelineContext->FlushBuild();
        pipelineContext->CreateExplicitAnimator(onFinishEvent);
        pipelineContext->ClearExplicitAnimationOption();
    }
}

void JSViewContext::JSKeyframeAnimateTo(const JSCallbackInfo& info)
{
    ACE_FUNCTION_TRACE();
    auto scopedDelegate = EngineHelper::GetCurrentDelegateSafely();
    if (!scopedDelegate) {
        // this case usually means there is no foreground container, need to figure out the reason.
        return;
    }
    if (info.Length() < 2) {
        return;
    }
    if (!info[0]->IsObject()) {
        return;
    }
    if (!info[1]->IsArray()) {
        return;
    }
    JSRef<JSArray> keyframeArr = JSRef<JSArray>::Cast(info[1]);
    if (keyframeArr->Length() == 0) {
        return;
    }

    auto container = Container::CurrentSafely();
    CHECK_NULL_VOID(container);
    if (!Container::CheckRunOnThreadByThreadId(container->GetInstanceId(), false)) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION, "the context found cannot run current thread, KeyframeAnimateTo "
                                           "failed, please use correct ui context.");
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    std::optional<int32_t> count;
    auto overallAnimationOption = ParseKeyframeOverallParam(info.GetExecutionContext(), obj, count);
    auto keyframes = ParseKeyframes(info.GetExecutionContext(), keyframeArr);
    int duration = 0;
    for (auto& keyframe : keyframes) {
        duration += keyframe.duration;
    }
    overallAnimationOption.SetDuration(duration);
    // actual curve is in keyframe, this curve will not be effective
    overallAnimationOption.SetCurve(Curves::EASE_IN_OUT);
    AceScopedTrace trace("KeyframeAnimateTo iteration:%d, delay:%d",
                         overallAnimationOption.GetIteration(), overallAnimationOption.GetDelay());
    PrintAnimationInfo(overallAnimationOption, AnimationInterface::KEYFRAME_ANIMATE_TO, count);
    overallAnimationOption.SetAnimationInterface(AnimationInterface::KEYFRAME_ANIMATE_TO);
    if (!ViewStackModel::GetInstance()->IsEmptyStack()) {
        TAG_LOGW(AceLogTag::ACE_ANIMATION,
            "when call keyframeAnimateTo, node stack is not empty, not suitable for keyframeAnimateTo."
            "param is [duration:%{public}d, delay:%{public}d, iteration:%{public}d]",
            overallAnimationOption.GetDuration(), overallAnimationOption.GetDelay(),
            overallAnimationOption.GetIteration());
    }
    NG::ScopedViewStackProcessor scopedProcessor;
    StartKeyframeAnimation(pipelineContext, overallAnimationOption, keyframes, count);
    pipelineContext->FlushAfterLayoutCallbackInImplicitAnimationTask();
}

void JSViewContext::SetDynamicDimming(const JSCallbackInfo& info)
{
    EcmaVM* vm = info.GetVm();
    CHECK_NULL_VOID(vm);
    auto jsTargetNode = info[0];
    auto jsDimming = info[1];
    auto* targetNodePtr = jsTargetNode->GetLocalHandle()->ToNativePointer(vm)->Value();
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(targetNodePtr);
    CHECK_NULL_VOID(frameNode);
    if (!info[1]->IsNumber()) {
        return;
    }
    float dimming = info[1]->ToNumber<float>();
    RefPtr<Ace::NG::RenderContext> renderContext = frameNode->GetRenderContext();
    renderContext->UpdateDynamicDimDegree(std::clamp(dimming, 0.0f, 1.0f));
}

void JSViewContext::JSOpenBindSheet(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_ONE) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }

    auto sheetContentNode = ParseContentNode(info);
    if (sheetContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_BIND_SHEET_CONTENT_ERROR);
        return;
    }

    // parse SheetStyle and callbacks
    NG::SheetStyle sheetStyle;
    sheetStyle.sheetHeight.sheetMode = NG::SheetMode::LARGE;
    sheetStyle.showDragBar = true;
    sheetStyle.showInPage = false;
    std::function<void()> onAppearCallback;
    std::function<void()> onDisappearCallback;
    std::function<void()> onWillAppearCallback;
    std::function<void()> onWillDisappearCallback;
    std::function<void()> shouldDismissFunc;
    std::function<void(const int32_t)> onWillDismissCallback;
    std::function<void(const float)> onHeightDidChangeCallback;
    std::function<void(const float)> onDetentsDidChangeCallback;
    std::function<void(const float)> onWidthDidChangeCallback;
    std::function<void(const float)> onTypeDidChangeCallback;
    std::function<void()> titleBuilderFunction;
    std::function<void()> sheetSpringBackFunc;
    if (paramCnt >= LENGTH_TWO && info[INDEX_ONE]->IsObject()) {
        JSViewAbstract::ParseSheetCallback(info[INDEX_ONE], onAppearCallback, onDisappearCallback, shouldDismissFunc,
            onWillDismissCallback, onWillAppearCallback, onWillDisappearCallback, onHeightDidChangeCallback,
            onDetentsDidChangeCallback, onWidthDidChangeCallback, onTypeDidChangeCallback, sheetSpringBackFunc);
        JSViewAbstract::ParseSheetStyle(info[INDEX_ONE], sheetStyle);
        JSViewAbstract::ParseSheetTitle(info[INDEX_ONE], sheetStyle, titleBuilderFunction);
    }

    int32_t targetId = INVALID_ID;
    if (paramCnt >= LENGTH_THREE) {
        if (!info[INDEX_TWO]->IsNumber()) {
            ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
            return;
        }
        targetId = info[INDEX_TWO]->ToNumber<int32_t>();
        if (targetId < 0) {
            ReturnPromise(info, ERROR_CODE_TARGET_ID_NOT_EXIST);
            return;
        }
    }
    TAG_LOGI(AceLogTag::ACE_SHEET, "paramCnt: %{public}d, contentId: %{public}d, targetId: %{public}d",
        paramCnt, sheetContentNode->GetId(), targetId);
    auto ret = ViewContextModel::GetInstance()->OpenBindSheet(sheetContentNode,
        std::move(titleBuilderFunction), sheetStyle, std::move(onAppearCallback), std::move(onDisappearCallback),
        std::move(shouldDismissFunc), std::move(onWillDismissCallback),  std::move(onWillAppearCallback),
        std::move(onWillDisappearCallback), std::move(onHeightDidChangeCallback),
        std::move(onDetentsDidChangeCallback), std::move(onWidthDidChangeCallback),
        std::move(onTypeDidChangeCallback), std::move(sheetSpringBackFunc), Container::CurrentId(), targetId);

    ReturnPromise(info, ret);
    return;
}

void JSViewContext::JSUpdateBindSheet(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_TWO) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto sheetContentNode = ParseContentNode(info);
    if (sheetContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_BIND_SHEET_CONTENT_ERROR);
        return;
    }

    bool isPartialUpdate = false;
    if (paramCnt == LENGTH_THREE) {
        if (!info[INDEX_TWO]->IsBoolean()) {
            ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
            return;
        }
        isPartialUpdate = info[INDEX_TWO]->ToBoolean();
    }

    NG::SheetStyle sheetStyle;
    std::function<void()> titleBuilderFunction;
    if (paramCnt >= LENGTH_TWO && info[INDEX_ONE]->IsObject()) {
        JSViewAbstract::ParseSheetStyle(info[INDEX_ONE], sheetStyle, isPartialUpdate);
        JSViewAbstract::ParseSheetTitle(info[INDEX_ONE], sheetStyle, titleBuilderFunction);
    } else {
        sheetStyle.sheetHeight.sheetMode = NG::SheetMode::LARGE;
        sheetStyle.showDragBar = true;
        sheetStyle.showInPage = false;
        isPartialUpdate = false;
    }
    TAG_LOGI(AceLogTag::ACE_SHEET, "paramCnt: %{public}d, contentId: %{public}d, isPartialUpdate: %{public}d",
        paramCnt, sheetContentNode->GetId(), isPartialUpdate);
    auto ret = ViewContextModel::GetInstance()->UpdateBindSheet(
        sheetContentNode, sheetStyle, isPartialUpdate, Container::CurrentId());
    ReturnPromise(info, ret);
    return;
}

void JSViewContext::JSCloseBindSheet(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_ONE) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }

    auto sheetContentNode = ParseContentNode(info);
    if (sheetContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_BIND_SHEET_CONTENT_ERROR);
        return;
    }

    TAG_LOGI(AceLogTag::ACE_SHEET, "paramCnt: %{public}d, contentId: %{public}d", paramCnt, sheetContentNode->GetId());
    auto ret =
        ViewContextModel::GetInstance()->CloseBindSheet(sheetContentNode, Container::CurrentId());
    ReturnPromise(info, ret);
    return;
}

int32_t ParseTargetInfo(const JSRef<JSObject>& obj, int32_t& targetId)
{
    CHECK_EQUAL_RETURN(obj->IsEmpty(), true, ERROR_CODE_PARAM_INVALID);
    auto targetInfoID = obj->GetProperty("id");
    if (targetInfoID->IsNumber()) {
        targetId = targetInfoID->ToNumber<int32_t>();
    } else if (targetInfoID->IsString()) {
        std::string targetIdString = targetInfoID->ToString();
        auto targetInfoComponentId = obj->GetProperty("componentId");
        if (targetInfoComponentId->IsNumber()) {
            auto componentId = targetInfoComponentId->ToNumber<int32_t>();
            auto targetComponentIdNode =
                ElementRegister::GetInstance()->GetSpecificItemById<NG::FrameNode>(componentId);
            CHECK_NULL_RETURN(targetComponentIdNode, ERROR_CODE_TARGET_INFO_NOT_EXIST);
            if (targetComponentIdNode->GetInspectorId().value_or("") == targetIdString) {
                targetId = targetComponentIdNode->GetId();
                return ERROR_CODE_NO_ERROR;
            }
            auto targetNode = NG::FrameNode::FindChildByName(targetComponentIdNode, targetIdString);
            CHECK_NULL_RETURN(targetNode, ERROR_CODE_TARGET_INFO_NOT_EXIST);
            targetId = targetNode->GetId();
        } else {
            auto targetNode = ElementRegister::GetInstance()->GetAttachedFrameNodeById(targetIdString);
            CHECK_NULL_RETURN(targetNode, ERROR_CODE_TARGET_INFO_NOT_EXIST);
            targetId = targetNode->GetId();
        }
    }
    if (targetId < 0) {
        return ERROR_CODE_PARAM_INVALID;
    }
    return ERROR_CODE_NO_ERROR;
}

void JSViewContext::JSOpenPopup(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_TWO) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto popupContentNode = ParseContentNode(info);
    if (popupContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_DIALOG_CONTENT_ERROR);
        return;
    }
    auto popupParam = AceType::MakeRefPtr<PopupParam>();
    CHECK_NULL_VOID(popupParam);
    popupParam->SetIsShow(true);
    popupParam->SetUseCustomComponent(true);
    if (info[INDEX_ONE]->IsObject()) {
        auto popupObj = JSRef<JSObject>::Cast(info[INDEX_ONE]);
        int32_t targetId = INVALID_ID;
        auto result = ParseTargetInfo(popupObj, targetId);
        if (result == ERROR_CODE_NO_ERROR) {
            popupParam->SetTargetId(std::to_string(targetId));
        } else {
            ReturnPromise(info, result);
            return;
        }
    } else {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    if (paramCnt == LENGTH_THREE && info[INDEX_TWO]->IsObject()) {
        auto popupObj = JSRef<JSObject>::Cast(info[INDEX_TWO]);
        JSViewAbstract::ParseContentPopupCommonParam(info, popupObj, popupParam);
    } else {
        JSRef<JSObject> popupObj = JSRef<JSObject>::New();
        JSViewAbstract::ParseContentPopupCommonParam(info, popupObj, popupParam);
    }
    auto ret = JSViewAbstract::OpenPopup(popupParam, popupContentNode);
    if (ret == ERROR_CODE_INTERNAL_ERROR) {
        ret = ERROR_CODE_NO_ERROR;
    }
    ReturnPromise(info, ret);
    return;
}

bool UpdateParsePopupParam(const JSCallbackInfo& info, RefPtr<PopupParam>& popupParam,
    const RefPtr<NG::UINode>& customNode, bool isPartialUpdate)
{
    if ((!popupParam) || (!customNode)) {
        return false;
    }
    auto paramCnt = info.Length();
    if (!(paramCnt >= LENGTH_TWO && info[INDEX_ONE]->IsObject())) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return false;
    }
    auto param = AceType::MakeRefPtr<PopupParam>();
    CHECK_NULL_RETURN(param, false);
    auto result = JSViewAbstract::GetPopupParam(param, customNode);
    if (result == ERROR_CODE_NO_ERROR) {
        if (isPartialUpdate) {
            popupParam = param;
        } else {
            popupParam->SetTargetId(param->GetTargetId());
        }
    } else {
        if (result == ERROR_CODE_INTERNAL_ERROR) {
            result = ERROR_CODE_NO_ERROR;
        }
        ReturnPromise(info, result);
        return false;
    }
    auto isShowInSubWindow = param->IsShowInSubWindow();
    auto focusable = param->GetFocusable();
    popupParam->SetIsShow(true);
    popupParam->SetUseCustomComponent(true);
    popupParam->SetIsPartialUpdate(isPartialUpdate);
    auto popupObj = JSRef<JSObject>::Cast(info[INDEX_ONE]);
    JSViewAbstract::ParseContentPopupCommonParam(info, popupObj, popupParam);
    popupParam->SetShowInSubWindow(isShowInSubWindow);
    popupParam->SetFocusable(focusable);
    return true;
}

void JSViewContext::JSUpdatePopup(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_TWO) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto popupContentNode = ParseContentNode(info);
    if (popupContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_DIALOG_CONTENT_ERROR);
        return;
    }
    auto popupParam = AceType::MakeRefPtr<PopupParam>();
    CHECK_NULL_VOID(popupParam);
    bool isPartialUpdate = false;
    if (paramCnt == LENGTH_THREE) {
        if (!info[INDEX_TWO]->IsBoolean()) {
            ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
            return;
        }
        isPartialUpdate = info[INDEX_TWO]->ToBoolean();
    }
    auto result = UpdateParsePopupParam(info, popupParam, popupContentNode, isPartialUpdate);
    if (!result) {
        return;
    }
    auto ret = JSViewAbstract::UpdatePopup(popupParam, popupContentNode);
    if (ret == ERROR_CODE_INTERNAL_ERROR) {
        ret = ERROR_CODE_NO_ERROR;
    }
    ReturnPromise(info, ret);
    return;
}

void JSViewContext::JSClosePopup(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_ONE) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto popupContentNode = ParseContentNode(info);
    if (popupContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_DIALOG_CONTENT_ERROR);
        return;
    }
    auto ret = JSViewAbstract::ClosePopup(popupContentNode);
    if (ret == ERROR_CODE_INTERNAL_ERROR) {
        ret = ERROR_CODE_NO_ERROR;
    }
    ReturnPromise(info, ret);
    return;
}

int32_t GetMenuParam(NG::MenuParam& menuParam, const RefPtr<NG::UINode>& node)
{
    if (!node) {
        TAG_LOGE(AceLogTag::ACE_DIALOG, "Content of menu is null.");
        return ERROR_CODE_DIALOG_CONTENT_ERROR;
    }
    auto context = node->GetContextWithCheck();
    CHECK_NULL_RETURN(context, ERROR_CODE_INTERNAL_ERROR);
    auto overlayManager = context->GetOverlayManager();
    if (!overlayManager) {
        return ERROR_CODE_INTERNAL_ERROR;
    }
    auto menuNode = overlayManager->GetMenuNodeWithExistContent(node);
    if (!menuNode) {
        TAG_LOGE(AceLogTag::ACE_DIALOG, "GetMenuParam failed because cannot find menuNode.");
        return ERROR_CODE_DIALOG_CONTENT_NOT_FOUND;
    }
    auto wrapperPattern = AceType::DynamicCast<NG::MenuWrapperPattern>(menuNode->GetPattern());
    CHECK_NULL_RETURN(wrapperPattern, ERROR_CODE_INTERNAL_ERROR);
    auto menuProperties = wrapperPattern->GetMenuParam();
    menuParam = menuProperties;
    return ERROR_CODE_NO_ERROR;
}

void JSViewContext::JSOpenMenu(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_TWO) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto menuContentNode = ParseContentNode(info);
    if (menuContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_DIALOG_CONTENT_ERROR);
        return;
    }
    int32_t targetId = INVALID_ID;
    if (info[INDEX_ONE]->IsObject()) {
        auto menuObj = JSRef<JSObject>::Cast(info[INDEX_ONE]);
        auto result = ParseTargetInfo(menuObj, targetId);
        if (result != ERROR_CODE_NO_ERROR) {
            ReturnPromise(info, result);
            return;
        }
    } else {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    NG::MenuParam menuParam;
    JSRef<JSObject> menuObj;
    if (paramCnt == LENGTH_THREE && info[INDEX_TWO]->IsObject()) {
        menuObj = JSRef<JSObject>::Cast(info[INDEX_TWO]);
    }
    JSViewAbstract::ParseContentMenuCommonParam(info, menuObj, menuParam);
    auto ret = JSViewAbstract::OpenMenu(menuParam, menuContentNode, targetId);
    if (ret == ERROR_CODE_INTERNAL_ERROR) {
        ret = ERROR_CODE_NO_ERROR;
    }
    ReturnPromise(info, ret);
    return;
}

void JSViewContext::JSUpdateMenu(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_TWO) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto menuContentNode = ParseContentNode(info);
    if (menuContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_DIALOG_CONTENT_ERROR);
        return;
    }
    bool isPartialUpdate = false;
    if (paramCnt == LENGTH_THREE) {
        if (!info[INDEX_TWO]->IsBoolean()) {
            ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
            return;
        }
        isPartialUpdate = info[INDEX_TWO]->ToBoolean();
    }
    NG::MenuParam menuParam;
    if (paramCnt >= LENGTH_TWO && info[INDEX_ONE]->IsObject()) {
        NG::MenuParam menuParamOpen;
        //Get current param config
        auto result = GetMenuParam(menuParamOpen, menuContentNode);
        if (result != ERROR_CODE_NO_ERROR && result != ERROR_CODE_INTERNAL_ERROR) {
            ReturnPromise(info, result);
            return;
        }
        if (isPartialUpdate) {
            menuParam = menuParamOpen;
        }
        auto menuObj = JSRef<JSObject>::Cast(info[INDEX_ONE]);
        JSViewAbstract::ParseContentMenuCommonParam(info, menuObj, menuParam);
        //Updating these parameters is not supported
        menuParam.isShowInSubWindow = menuParamOpen.isShowInSubWindow;
        menuParam.previewMode = menuParamOpen.previewMode;
    } else {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto ret = JSViewAbstract::UpdateMenu(menuParam, menuContentNode);
    if (ret == ERROR_CODE_INTERNAL_ERROR) {
        ret = ERROR_CODE_NO_ERROR;
    }
    ReturnPromise(info, ret);
    return;
}

void JSViewContext::JSCloseMenu(const JSCallbackInfo& info)
{
    auto paramCnt = info.Length();
    if (paramCnt < LENGTH_ONE) {
        ReturnPromise(info, ERROR_CODE_PARAM_INVALID);
        return;
    }
    auto menuContentNode = ParseContentNode(info);
    if (menuContentNode == nullptr) {
        ReturnPromise(info, ERROR_CODE_DIALOG_CONTENT_ERROR);
        return;
    }
    auto ret = JSViewAbstract::CloseMenu(menuContentNode);
    if (ret == ERROR_CODE_INTERNAL_ERROR) {
        ret = ERROR_CODE_NO_ERROR;
    }
    ReturnPromise(info, ret);
    return;
}

void JSViewContext::IsFollowingSystemFontScale(const JSCallbackInfo& info)
{
    auto container = Container::CurrentSafely();
    CHECK_NULL_VOID(container);
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    auto follow = pipelineContext->IsFollowSystem();
    auto followRef = JSRef<JSVal>::Make(JSVal(ToJSValue(follow)));
    info.SetReturnValue(followRef);
    return;
}

void JSViewContext::GetMaxFontScale(const JSCallbackInfo& info)
{
    auto container = Container::CurrentSafely();
    CHECK_NULL_VOID(container);
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    auto maxFontScale = pipelineContext->GetMaxAppFontScale();
    auto maxFontScaleRef = JSRef<JSVal>::Make(JSVal(ToJSValue(maxFontScale)));
    info.SetReturnValue(maxFontScaleRef);
    return;
}

void JSViewContext::SetEnableSwipeBack(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    if (!info[0]->IsBoolean()) {
        return;
    }
    auto container = Container::CurrentSafely();
    CHECK_NULL_VOID(container);
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    pipelineContext->SetEnableSwipeBack(info[0]->ToBoolean());
}

void JSViewContext::JSSetKeyboardAppearanceConfig(const JSCallbackInfo& info)
{
    EcmaVM* vm = info.GetVm();
    CHECK_NULL_VOID(vm);
    auto jsTargetNode = info[0];
    auto* targetNodePtr = jsTargetNode->GetLocalHandle()->ToNativePointer(vm)->Value();
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(targetNodePtr);
    CHECK_NULL_VOID(frameNode);
    if (!info[1]->IsObject()) {
        return;
    }
    auto nodeTag = frameNode->GetTag();
    if (nodeTag == V2::TEXTINPUT_ETS_TAG) {
        JSTextField::SetKeyboardAppearanceConfig(info);
    } else if (nodeTag == V2::SEARCH_ETS_TAG) {
        JSSearch::SetKeyboardAppearanceConfig(info);
    }
}

void JSViewContext::JSBind(BindingTarget globalObj)
{
    JSClass<JSViewContext>::Declare("Context");
    JSClass<JSViewContext>::StaticMethod("animation", JSAnimation);
    JSClass<JSViewContext>::StaticMethod("animateTo", JSAnimateTo);
    JSClass<JSViewContext>::StaticMethod("animateToImmediately", JSAnimateToImmediately);
    JSClass<JSViewContext>::StaticMethod("keyframeAnimateTo", JSKeyframeAnimateTo);
    JSClass<JSViewContext>::StaticMethod("setDynamicDimming", SetDynamicDimming);
    JSClass<JSViewContext>::StaticMethod("openBindSheet", JSOpenBindSheet);
    JSClass<JSViewContext>::StaticMethod("updateBindSheet", JSUpdateBindSheet);
    JSClass<JSViewContext>::StaticMethod("closeBindSheet", JSCloseBindSheet);
    JSClass<JSViewContext>::StaticMethod("openPopup", JSOpenPopup);
    JSClass<JSViewContext>::StaticMethod("updatePopup", JSUpdatePopup);
    JSClass<JSViewContext>::StaticMethod("closePopup", JSClosePopup);
    JSClass<JSViewContext>::StaticMethod("openMenu", JSOpenMenu);
    JSClass<JSViewContext>::StaticMethod("updateMenu", JSUpdateMenu);
    JSClass<JSViewContext>::StaticMethod("closeMenu", JSCloseMenu);
    JSClass<JSViewContext>::StaticMethod("isFollowingSystemFontScale", IsFollowingSystemFontScale);
    JSClass<JSViewContext>::StaticMethod("getMaxFontScale", GetMaxFontScale);
    JSClass<JSViewContext>::StaticMethod("bindTabsToScrollable", JSTabsFeature::BindTabsToScrollable);
    JSClass<JSViewContext>::StaticMethod("unbindTabsFromScrollable", JSTabsFeature::UnbindTabsFromScrollable);
    JSClass<JSViewContext>::StaticMethod("bindTabsToNestedScrollable", JSTabsFeature::BindTabsToNestedScrollable);
    JSClass<JSViewContext>::StaticMethod(
        "unbindTabsFromNestedScrollable", JSTabsFeature::UnbindTabsFromNestedScrollable);
    JSClass<JSViewContext>::StaticMethod("enableSwipeBack", JSViewContext::SetEnableSwipeBack);
    JSClass<JSViewContext>::StaticMethod("setKeyboardAppearanceConfig", JSViewContext::JSSetKeyboardAppearanceConfig);
    JSClass<JSViewContext>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework

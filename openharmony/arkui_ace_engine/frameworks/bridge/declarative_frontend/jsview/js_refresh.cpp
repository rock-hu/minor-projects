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

#include "frameworks/bridge/declarative_frontend/jsview/js_refresh.h"

#include <cstdint>

#include "interfaces/inner_api/ui_session/ui_session_manager.h"

#include "base/log/ace_scoring_log.h"
#include "bridge/declarative_frontend/jsview/js_refresh.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/jsview/models/refresh_model_impl.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/refresh/refresh_model.h"
#include "core/components_ng/pattern/refresh/refresh_model_ng.h"

namespace OHOS::Ace {
namespace {
constexpr int32_t DEFAULT_FRICTION = 62;
constexpr int32_t MAX_FRICTION = 100;
} // namespace
std::unique_ptr<RefreshModel> RefreshModel::instance_ = nullptr;
std::mutex RefreshModel::mutex_;

RefreshModel* RefreshModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::RefreshModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::RefreshModelNG());
            } else {
                instance_.reset(new Framework::RefreshModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {

void ParseRefreshingObject(const JSCallbackInfo& info, const JSRef<JSVal>& changeEventVal)
{
    CHECK_NULL_VOID(changeEventVal->IsFunction());

    auto vm = info.GetVm();
    auto jsFunc = JSRef<JSFunc>::Cast(changeEventVal);
    auto func = jsFunc->GetLocalHandle();
    WeakPtr<NG::FrameNode> targetNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    auto changeEvent = [vm, func = panda::CopyableGlobal(vm, func), node = targetNode](const std::string& param) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        if (param != "true" && param != "false") {
            return;
        }
        bool newValue = StringToBool(param);
        ACE_SCORING_EVENT("Refresh.ChangeEvent");
        PipelineContext::SetCallBackNode(node);
        panda::Local<panda::JSValueRef> params[1] = { panda::BooleanRef::New(vm, newValue) };
        func->Call(vm, func.ToLocal(), params, 1);
    };
    RefreshModel::GetInstance()->SetChangeEvent(std::move(changeEvent));
}

void JSRefresh::SetPullToRefresh(const JSCallbackInfo& info)
{
    bool pullToRefresh = true;
    if (info[0]->IsBoolean()) {
        pullToRefresh = info[0]->ToBoolean();
    }
    RefreshModel::GetInstance()->SetPullToRefresh(pullToRefresh);
}

void JSRefresh::JSBind(BindingTarget globalObj)
{
    JSClass<JSRefresh>::Declare("Refresh");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSRefresh>::StaticMethod("create", &JSRefresh::Create, opt);
    JSClass<JSRefresh>::StaticMethod("refreshOffset", &JSRefresh::JsRefreshOffset);
    JSClass<JSRefresh>::StaticMethod("pullToRefresh", &JSRefresh::SetPullToRefresh, opt);
    JSClass<JSRefresh>::StaticMethod("pullDownRatio", &JSRefresh::SetPullDownRatio);
    JSClass<JSRefresh>::StaticMethod("maxPullDownDistance", &JSRefresh::SetMaxPullDownDistance);
    JSClass<JSRefresh>::StaticMethod("onAttach", &JSInteractableView::JsOnAttach);
    JSClass<JSRefresh>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSRefresh>::StaticMethod("onDetach", &JSInteractableView::JsOnDetach);
    JSClass<JSRefresh>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSRefresh>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSRefresh>::InheritAndBind<JSContainerBase>(globalObj);
}

void JSRefresh::SetPullDownRatio(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }

    auto args = info[0];
    std::optional<float> pulldownRatio = std::nullopt;
    if (!args->IsNumber() || std::isnan(args->ToNumber<float>())) {
        RefreshModel::GetInstance()->SetPullDownRatio(pulldownRatio);
        return;
    }
    pulldownRatio = std::clamp(args->ToNumber<float>(), 0.f, 1.f);
    RefreshModel::GetInstance()->SetPullDownRatio(pulldownRatio);
}

void JSRefresh::SetMaxPullDownDistance(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }

    auto args = info[0];
    if (!args->IsNumber()) {
        RefreshModel::GetInstance()->SetMaxPullDownDistance(std::nullopt);
        return;
    }
    float maxPullDownDistance = args->ToNumber<float>();
    if (std::isnan(maxPullDownDistance)) {
        RefreshModel::GetInstance()->SetMaxPullDownDistance(std::nullopt);
        return;
    }
    maxPullDownDistance = std::max(maxPullDownDistance, 0.0f);
    RefreshModel::GetInstance()->SetMaxPullDownDistance(maxPullDownDistance);
}

void JSRefresh::JsRefreshOffset(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    JsRefreshOffset(info[0]);
}

void JSRefresh::JsRefreshOffset(const JSRef<JSVal>& jsVal)
{
    CalcDimension value(0.0f);
    if (!ParseJsDimensionVpNG(jsVal, value)) {
        value.SetValue(0.0f);
    }
    RefreshModel::GetInstance()->SetRefreshOffset(value);
}

void JSRefresh::Create(const JSCallbackInfo& info)
{
    if (!info[0]->IsObject()) {
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto refreshing = paramObject->GetProperty("refreshing");
    auto jsOffset = paramObject->GetProperty("offset");
    auto friction = paramObject->GetProperty("friction");
    auto promptText = paramObject->GetProperty("promptText");
    JSRef<JSVal> changeEventVal;
    RefreshModel::GetInstance()->Create();

    if (refreshing->IsBoolean()) {
        RefreshModel::GetInstance()->SetRefreshing(refreshing->ToBoolean());
        changeEventVal = paramObject->GetProperty("$refreshing");
        ParseRefreshingObject(info, changeEventVal);
    } else if (refreshing->IsObject()) {
        JSRef<JSObject> refreshingObj = JSRef<JSObject>::Cast(refreshing);
        changeEventVal = refreshingObj->GetProperty("changeEvent");
        ParseRefreshingObject(info, changeEventVal);
        RefreshModel::GetInstance()->SetRefreshing(refreshingObj->GetProperty("value")->ToBoolean());
    } else {
        RefreshModel::GetInstance()->SetRefreshing(false);
    }
    CalcDimension offset;
    if (ParseJsDimensionVp(jsOffset, offset)) {
        if (GreatOrEqual(offset.Value(), 0.0) && offset.Unit() != DimensionUnit::PERCENT) {
            RefreshModel::GetInstance()->SetIndicatorOffset(offset);
        }
    }
    ParsFrictionData(friction);
    if (!ParseRefreshingContent(paramObject)) {
        bool isCustomBuilderExist = ParseCustomBuilder(info);
        RefreshModel::GetInstance()->SetIsCustomBuilderExist(isCustomBuilderExist);
    }

    std::string loadingStr = "";
    if (SystemProperties::ConfigChangePerform()) {
        RefPtr<ResourceObject> resObj;
        if (ParseJsString(promptText, loadingStr, resObj)) {
            RefreshModel::GetInstance()->CreateWithResourceObj(resObj);
            RefreshModel::GetInstance()->SetLoadingText(loadingStr);
        } else {
            RefreshModel::GetInstance()->ResetLoadingText();
        }
    } else if (ParseJsString(promptText, loadingStr)) {
        RefreshModel::GetInstance()->SetLoadingText(loadingStr);
    } else {
        RefreshModel::GetInstance()->ResetLoadingText();
    }
}

bool JSRefresh::ParseRefreshingContent(const JSRef<JSObject>& paramObject)
{
    JSRef<JSVal> contentParam = paramObject->GetProperty("refreshingContent");
    if (!contentParam->IsObject()) {
        return false;
    }
    JSRef<JSObject> contentObject = JSRef<JSObject>::Cast(contentParam);
    JSRef<JSVal> builderNodeParam = contentObject->GetProperty("builderNode_");
    if (!builderNodeParam->IsObject()) {
        return false;
    }
    JSRef<JSObject> builderNodeObject = JSRef<JSObject>::Cast(builderNodeParam);
    JSRef<JSVal> nodeptr = builderNodeObject->GetProperty("nodePtr_");
    if (nodeptr.IsEmpty()) {
        return false;
    }
    const auto* vm = nodeptr->GetEcmaVM();
    CHECK_NULL_RETURN(nodeptr->GetLocalHandle()->IsNativePointer(vm), false);
    auto* node = nodeptr->GetLocalHandle()->ToNativePointer(vm)->Value();
    auto* frameNode = reinterpret_cast<NG::FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, false);
    RefPtr<NG::FrameNode> refPtrFrameNode = AceType::Claim(frameNode);
    RefreshModel::GetInstance()->SetCustomBuilder(refPtrFrameNode);
    RefreshModel::GetInstance()->SetIsCustomBuilderExist(false);
    return true;
}

bool JSRefresh::ParseCustomBuilder(const JSCallbackInfo& info)
{
    if (!info[0]->IsObject()) {
        return false;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto builder = paramObject->GetProperty("builder");
    RefPtr<NG::UINode> customNode;
    if (builder->IsFunction()) {
        {
            NG::ScopedViewStackProcessor builderViewStackProcessor;
            auto jsFunc = JSRef<JSFunc>::Cast(builder);
            jsFunc->Call(info.This());
            customNode = NG::ViewStackProcessor::GetInstance()->Finish();
        }
        RefreshModel::GetInstance()->SetCustomBuilder(customNode);
        return true;
    } else {
        RefreshModel::GetInstance()->SetCustomBuilder(customNode);
        return false;
    }
}

void JSRefresh::ParsFrictionData(const JsiRef<JsiValue>& friction)
{
    int32_t frictionNumber = DEFAULT_FRICTION;
    if (friction->IsString()) {
        frictionNumber = StringUtils::StringToInt(friction->ToString());
        if ((frictionNumber == 0 && friction->ToString() != "0") || frictionNumber < 0 ||
            frictionNumber > MAX_FRICTION) {
            frictionNumber = DEFAULT_FRICTION;
        }
    } else if (friction->IsNumber()) {
        frictionNumber = friction->ToNumber<int32_t>();
        if (frictionNumber < 0 || frictionNumber > MAX_FRICTION) {
            frictionNumber = DEFAULT_FRICTION;
        }
    }
    RefreshModel::GetInstance()->SetFriction(frictionNumber);
}
} // namespace OHOS::Ace::Framework

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

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_refresh_ffi.h"

#include <optional>

#include "cj_lambda.h"

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_view_abstract_ffi.h"
#include "core/components/refresh/refresh_theme.h"
#include "core/components_ng/pattern/refresh/refresh_model_ng.h"

using namespace OHOS::Ace;
using namespace OHOS::Ace::Framework;

extern "C" {
void FfiOHOSAceFrameworkRefreshCreate(bool refreshing, double offsetValue, int32_t offsetUnit, int32_t friction)
{
    RefPtr<RefreshTheme> theme = GetTheme<RefreshTheme>();
    if (!theme) {
        LOGE("Refresh Theme is null");
        return;
    }
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("Refresh Instance is null");
    }
    RefreshModel::GetInstance()->Create();
    RefreshModel::GetInstance()->SetLoadingDistance(theme->GetLoadingDistance());
    RefreshModel::GetInstance()->SetRefreshDistance(theme->GetRefreshDistance());
    RefreshModel::GetInstance()->SetProgressDistance(theme->GetProgressDistance());
    RefreshModel::GetInstance()->SetProgressDiameter(theme->GetProgressDiameter());
    RefreshModel::GetInstance()->SetMaxDistance(theme->GetMaxDistance());
    RefreshModel::GetInstance()->SetShowTimeDistance(theme->GetShowTimeDistance());
    RefreshModel::GetInstance()->SetTextStyle(theme->GetTextStyle());
    RefreshModel::GetInstance()->SetProgressColor(theme->GetProgressColor());
    RefreshModel::GetInstance()->SetProgressBackgroundColor(theme->GetBackgroundColor());

    RefreshModel::GetInstance()->SetRefreshing(refreshing);

    Dimension offset(offsetValue, static_cast<DimensionUnit>(offsetUnit));
    if (offset.Value() <= 0.0) {
        RefreshModel::GetInstance()->SetRefreshDistance(theme->GetRefreshDistance());
    } else {
        RefreshModel::GetInstance()->SetUseOffset(true);
        RefreshModel::GetInstance()->SetIndicatorOffset(offset);
    }

    RefreshModel::GetInstance()->SetFriction(friction);
    if (friction <= 0) {
        RefreshModel::GetInstance()->IsRefresh(true);
    }
}

void FfiOHOSAceFrameworkRefreshCreateWithChangeEvent(
    bool refreshing, double offsetValue, int32_t offsetUnit, int32_t friction, void (*callback)(bool isRefreshing))
{
    RefPtr<RefreshTheme> theme = GetTheme<RefreshTheme>();
    if (!theme) {
        LOGE("FfiOHOSAceFrameworkRefreshCreateWithChangeEvent, Refresh Theme is null");
        return;
    }
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshCreateWithChangeEvent, Refresh Instance is null");
    }
    RefreshModel::GetInstance()->Create();
    RefreshModel::GetInstance()->SetLoadingDistance(theme->GetLoadingDistance());
    RefreshModel::GetInstance()->SetRefreshDistance(theme->GetRefreshDistance());
    RefreshModel::GetInstance()->SetProgressDistance(theme->GetProgressDistance());
    RefreshModel::GetInstance()->SetProgressDiameter(theme->GetProgressDiameter());
    RefreshModel::GetInstance()->SetTextStyle(theme->GetTextStyle());
    RefreshModel::GetInstance()->SetProgressColor(theme->GetProgressColor());
    RefreshModel::GetInstance()->SetMaxDistance(theme->GetMaxDistance());
    RefreshModel::GetInstance()->SetShowTimeDistance(theme->GetShowTimeDistance());
    RefreshModel::GetInstance()->SetProgressBackgroundColor(theme->GetBackgroundColor());

    WeakPtr<NG::FrameNode> targetNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    auto changeEvent = [lambda = CJLambda::Create(callback), node = targetNode](const std::string& value) -> void {
        LOGI("FfiOHOSAceFrameworkRefreshCreateWithChangeEvent refreshing is %{public}s", value.c_str());
        PipelineContext::SetCallBackNode(node);
        bool newValue = value == "true";
        lambda(newValue);
    };
    RefreshModel::GetInstance()->SetChangeEvent(std::move(changeEvent));
    RefreshModel::GetInstance()->SetRefreshing(refreshing);

    Dimension offset(offsetValue, static_cast<DimensionUnit>(offsetUnit));
    if (offset.Value() <= 0.0) {
        RefreshModel::GetInstance()->SetRefreshDistance(theme->GetRefreshDistance());
    } else {
        LOGI("FfiOHOSAceFrameworkRefreshCreateWithChangeEvent, offset value > 0");
        RefreshModel::GetInstance()->SetUseOffset(true);
        RefreshModel::GetInstance()->SetIndicatorOffset(offset);
    }

    RefreshModel::GetInstance()->SetFriction(friction);
    if (friction <= 0) {
        RefreshModel::GetInstance()->IsRefresh(true);
    }
}

void FfiOHOSAceFrameworkRefreshPop()
{
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshPop, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->Pop();
}

void FfiOHOSAceFrameworkRefreshOnStateChange(void (*callback)(const int32_t value))
{
    WeakPtr<NG::FrameNode> targetNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    auto onStateChange = [func = CJLambda::Create(callback), node = targetNode](const int32_t& key) {
        LOGI("Refresh.OnStateChange");
        PipelineContext::SetCallBackNode(node);
        func(key);
    };
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshOnStateChange, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->SetOnStateChange(std::move(onStateChange));
}

void FfiOHOSAceFrameworkRefreshOnRefreshing(void (*callback)())
{
    WeakPtr<NG::FrameNode> targetNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    auto onRefreshing = [func = CJLambda::Create(callback), node = targetNode]() {
        LOGI("Refresh.OnRefreshing");
        PipelineContext::SetCallBackNode(node);
        func();
    };
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshOnRefreshing, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->SetOnRefreshing(std::move(onRefreshing));
}

void FfiOHOSAceFrameworkRefreshOnOffsetChange(void (*callback)(const float value))
{
    WeakPtr<NG::FrameNode> targetNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    auto onOffsetChange = [func = CJLambda::Create(callback), node = targetNode](const float value) {
        LOGI("Refresh.OnRefreshing");
        PipelineContext::SetCallBackNode(node);
        func(value);
    };
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshOnOffsetChange, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->SetOnOffsetChange(std::move(onOffsetChange));
}

void FfiOHOSAceFrameworkRefreshOffset(double offsetValue, int32_t offsetUnit)
{
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshOffset, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->SetRefreshOffset(
        Dimension(offsetValue, static_cast<OHOS::Ace::DimensionUnit>(offsetUnit)));
}

void FfiOHOSAceFrameworkRefreshPullToRefresh(bool value)
{
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshPullToRefresh, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->SetPullToRefresh(value);
}

void FfiOHOSAceFrameworkRefreshPullDownRatio(double ratio)
{
    float value = 0.0;
    if (LessNotEqual(ratio, 0.0)) {
        value = 0.0;
    } else if (GreatNotEqual(ratio, 1.0)) {
        value = 1.0;
    }
    value = ratio;
    std::optional<float> ratioValue = value;
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshPullDownRatio, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->SetPullDownRatio(ratioValue);
}

void FfiOHOSAceFrameworkRefreshResetPullDownRatio()
{
    std::optional<float> ratioValue = std::nullopt;
    if (RefreshModel::GetInstance() == nullptr) {
        LOGE("FfiOHOSAceFrameworkRefreshResetPullDownRatio, Refresh Instance is null");
        return;
    }
    RefreshModel::GetInstance()->SetPullDownRatio(ratioValue);
}
}

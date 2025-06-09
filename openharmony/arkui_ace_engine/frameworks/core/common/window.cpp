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

#include "core/common/window.h"

#include "core/common/container.h"

namespace OHOS::Ace {
Window::Window(std::unique_ptr<PlatformWindow> platformWindow) : platformWindow_(std::move(platformWindow))
{
    CHECK_NULL_VOID(platformWindow_);
    auto&& callback = [this](uint64_t nanoTimestamp, uint32_t frameCount) { OnVsync(nanoTimestamp, frameCount); };
    platformWindow_->RegisterVsyncCallback(callback);
    LOGI("Window Created success.");
}

void Window::RequestFrame()
{
    if (!onShow_) {
        return;
    }
    if (!isRequestVsync_ && platformWindow_ != nullptr) {
        platformWindow_->RequestFrame();
        isRequestVsync_ = true;
    }
}

void Window::SetRootRenderNode(const RefPtr<RenderNode>& root)
{
    CHECK_NULL_VOID(platformWindow_);
    platformWindow_->SetRootRenderNode(root);
}

void Window::OnVsync(uint64_t nanoTimestamp, uint32_t frameCount)
{
    isRequestVsync_ = false;

    for (auto& callback : callbacks_) {
        if (callback.callback_ == nullptr) {
            continue;
        }
        callback.callback_(nanoTimestamp, frameCount);
    }
}

void Window::SetVsyncCallback(AceVsyncCallback&& callback)
{
    callbacks_.push_back({
        .callback_ = std::move(callback),
        .containerId_ = Container::CurrentId(),
    });
}

void Window::SetForceVsyncRequests(bool forceVsyncRequests)
{
    forceVsync_ = forceVsyncRequests;
}

void Window::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    if (!onShow_) {
        return;
    }
    if (platformWindow_ != nullptr) {
        platformWindow_->SetUiDvsyncSwitch(dvsyncSwitch);
    }
}

int64_t Window::GetDeadlineByFrameCount(int64_t deadline, int64_t ts, int64_t frameBufferCount)
{
    if (!dvsyncOn_) {
        return deadline;
    }

    constexpr int64_t MAX_INBIHIT_PREDICT_DUR = 100 * 1000000;
    int64_t lastInbihitPredictTs = lastDVsyncInbihitPredictTs_;
    if (frameBufferCount < 1) {
        if (lastInbihitPredictTs == 0 || (ts - lastInbihitPredictTs < MAX_INBIHIT_PREDICT_DUR)) {
            // 100ms, inbihit
            deadline = 0;
        }
        if (lastInbihitPredictTs == 0) {
            lastDVsyncInbihitPredictTs_ = ts;
        }
    } else {
        if (lastInbihitPredictTs != 0) {
            lastDVsyncInbihitPredictTs_ = 0;
        }
    }
    return deadline;
}

WidthBreakpoint Window::GetWidthBreakpoint(const WidthLayoutBreakPoint& layoutBreakpoints) const
{
    double density = PipelineBase::GetCurrentDensity();
    double width = 0.0;
    if (NearZero(density)) {
        width = GetCurrentWindowRect().Width();
    } else {
        width = GetCurrentWindowRect().Width() / density;
    }

    WidthBreakpoint breakpoint;
    if (width < layoutBreakpoints.widthVPXS_) {
        breakpoint = WidthBreakpoint::WIDTH_XS;
    } else if (width < layoutBreakpoints.widthVPSM_) {
        breakpoint = WidthBreakpoint::WIDTH_SM;
    } else if (width < layoutBreakpoints.widthVPMD_) {
        breakpoint = WidthBreakpoint::WIDTH_MD;
    } else if (width < layoutBreakpoints.widthVPLG_) {
        breakpoint = WidthBreakpoint::WIDTH_LG;
    } else {
        breakpoint = WidthBreakpoint::WIDTH_XL;
    }
    return breakpoint;
}

HeightBreakpoint Window::GetHeightBreakpoint(const HeightLayoutBreakPoint& layoutBreakpoints) const
{
    auto width = GetCurrentWindowRect().Width();
    auto height = GetCurrentWindowRect().Height();
    auto aspectRatio = 0.0;
    if (NearZero(width)) {
        aspectRatio = 0.0;
    } else {
        aspectRatio = height / width;
    }

    HeightBreakpoint breakpoint;
    if (aspectRatio < layoutBreakpoints.heightVPRATIOSM_) {
        breakpoint = HeightBreakpoint::HEIGHT_SM;
    } else if (aspectRatio < layoutBreakpoints.heightVPRATIOMD_) {
        breakpoint = HeightBreakpoint::HEIGHT_MD;
    } else {
        breakpoint = HeightBreakpoint::HEIGHT_LG;
    }
    return breakpoint;
}
} // namespace OHOS::Ace

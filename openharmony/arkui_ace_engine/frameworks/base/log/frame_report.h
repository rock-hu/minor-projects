/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_FRAME_REPORT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_FRAME_REPORT_H

#include <string>
#include <unordered_map>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {
enum class FrameSchedEvent {
    SCHED_EVENT_BASE = 0,
    INIT,
    UI_FLUSH_BEGIN = 20001,
    UI_FLUSH_END,
    UI_SCB_WORKER_BEGIN,
    UI_SCB_WORKER_END,
    SCHED_EVENT_MAX,
};
using FrameInitFunc = void (*)();
using FrameGetEnableFunc = int (*)();
using BeginFlushAnimationFunc = void (*)();
using EndFlushAnimationFunc = void (*)();
using BeginFlushBuildFunc = void (*)();
using EndFlushBuildFunc = void (*)();
using BeginFlushLayoutFunc = void (*)();
using EndFlushLayoutFunc = void (*)();
using BeginFlushRenderFunc = void (*)();
using EndFlushRenderFunc = void (*)();
using BeginFlushRenderFinishFunc = void (*)();
using EndFlushRenderFinishFunc = void (*)();
using BeginProcessPostFlushFunc = void (*)();
using BeginListFlingFunc = void (*)();
using EndListFlingFunc = void (*)();
using FlushBeginFunc = void (*)();
using FlushEndFunc = void (*)();
using SetFrameParamFunc = void (*)(int, int, int, int);
using EnableSelfRenderFunc = void (*)();
using DisableSelfRenderFunc = void (*)();
using ReportSchedEventFunc =
    void (*)(FrameSchedEvent, const std::unordered_map<std::string, std::string>&);

class ACE_EXPORT FrameReport final {
public:
    static FrameReport& GetInstance();
    void Init();
    int GetEnable();
    void BeginFlushAnimation();
    void EndFlushAnimation();

    void BeginFlushBuild();
    void EndFlushBuild();
    void BeginFlushLayout();
    void EndFlushLayout();
    void BeginFlushRender();
    void EndFlushRender();
    void BeginFlushRenderFinish();
    void EndFlushRenderFinish();
    void BeginProcessPostFlush();
    void BeginListFling();
    void EndListFling();
    void FlushBegin();
    void FlushEnd();
    void SetFrameParam(int requestId, int load, int schedFrameNum, int value);
    void EnableSelfRender();
    void DisableSelfRender();
    void ReportSchedEvent(
        FrameSchedEvent event, const std::unordered_map<std::string, std::string>& payLoad);

private:
    FrameReport();
    ~FrameReport();
    bool LoadLibrary();
    void CloseLibrary();
    void* LoadSymbol(const char* symName);
    int GetFrameReportEnable();
    void* frameSchedHandle_ = nullptr;
    bool frameSchedSoLoaded_ = false;
    bool enable_ = false;

    ACE_EXPORT FrameInitFunc frameInitFunc_ = nullptr;
    ACE_EXPORT FrameGetEnableFunc frameGetEnableFunc_ = nullptr;
    ACE_EXPORT BeginFlushAnimationFunc beginFlushAnimationFunc_ = nullptr;
    ACE_EXPORT EndFlushAnimationFunc endFlushAnimationFunc_ = nullptr;
    ACE_EXPORT BeginFlushBuildFunc beginFlushBuildFunc_ = nullptr;
    ACE_EXPORT EndFlushBuildFunc endFlushBuildFunc_ = nullptr;
    ACE_EXPORT BeginFlushLayoutFunc beginFlushLayoutFunc_ = nullptr;
    ACE_EXPORT EndFlushLayoutFunc endFlushLayoutFunc_ = nullptr;
    ACE_EXPORT BeginFlushRenderFunc beginFlushRenderFunc_ = nullptr;
    ACE_EXPORT EndFlushRenderFunc endFlushRenderFunc_ = nullptr;
    ACE_EXPORT BeginFlushRenderFinishFunc beginFlushRenderFinishFunc_ = nullptr;
    ACE_EXPORT EndFlushRenderFinishFunc endFlushRenderFinishFunc_ = nullptr;
    ACE_EXPORT BeginProcessPostFlushFunc beginProcessPostFunc_ = nullptr;
    ACE_EXPORT BeginListFlingFunc beginListFlingFunc_ = nullptr;
    ACE_EXPORT EndListFlingFunc endListFlingFunc_ = nullptr;
    ACE_EXPORT FlushBeginFunc flushBeginFunc_ = nullptr;
    ACE_EXPORT FlushEndFunc flushEndFunc_ = nullptr;
    ACE_EXPORT SetFrameParamFunc setFrameParamFunc_ = nullptr;
    ACE_EXPORT EnableSelfRenderFunc enableSelfRenderFunc_ = nullptr;
    ACE_EXPORT DisableSelfRenderFunc disableSelfRenderFunc_ = nullptr;
    ACE_EXPORT ReportSchedEventFunc reportSchedEventFunc_ = nullptr;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_FRAME_REPORT_H

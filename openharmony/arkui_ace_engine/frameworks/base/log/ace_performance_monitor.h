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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_PERFORMANCE_MONITOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_PERFORMANCE_MONITOR_H

#include <chrono>
#include <cstdint>
#include <map>

namespace OHOS::Ace {

enum class MonitorTag {
    COMPONENT_CREATION = 0,
    COMPONENT_LIFECYCLE,
    COMPONENT_UPDATE,
    JS_CALLBACK,
    STATIC_API,
    OTHER,
};

enum class MonitorStatus {
    IDLE = 0,
    RUNNING,
};

#define COMPONENT_CREATION_DURATION() ScopedMonitor scopedMonitor(MonitorTag::COMPONENT_CREATION)
#define COMPONENT_LIFECYCLE_DURATION() ScopedMonitor scopedMonitor(MonitorTag::COMPONENT_LIFECYCLE)
#define COMPONENT_UPDATE_DURATION() ScopedMonitor scopedMonitor(MonitorTag::COMPONENT_UPDATE)
#define JS_CALLBACK_DURATION() ScopedMonitor scopedMonitor(MonitorTag::JS_CALLBACK)
#define STATIC_API_DURATION() ScopedMonitor scopedMonitor(MonitorTag::STATIC_API)
#define OTHER_DURATION() ScopedMonitor scopedMonitor(MonitorTag::OTHER)

typedef std::chrono::steady_clock::time_point TimePoint;

class ScopedMonitor {
public:
    explicit ScopedMonitor(MonitorTag tag);
    ~ScopedMonitor();

private:
    MonitorTag tag_;
    TimePoint begin_;
    TimePoint end_;
};

class ArkUIPerfMonitor {
public:
    static ArkUIPerfMonitor& GetInstance();
    ArkUIPerfMonitor();
    void StartPerf();
    void FinishPerf();
    void RecordTimeSlice(MonitorTag tag, int64_t duration);
    void RecordStateMgmtNode(int64_t num);
    void RecordLayoutNode(int64_t num = 1);
    void RecordRenderNode(int64_t num = 1);
    void RecordDisplaySyncRate(int32_t displaySyncRate);
    void SetRecordingStatus(MonitorTag tag, MonitorStatus status);

private:
    void InitPerfMonitor();
    void ClearPerfMonitor();
    void FlushPerfMonitor();
    std::map<MonitorTag, int64_t> timeSlice_;
    int64_t propertyNum_;
    int64_t stateMgmtNodeNum_;
    int64_t layoutNodeNum_;
    int64_t renderNodeNum_;
    TimePoint begin_;
    TimePoint end_;
    int64_t monitorStatus_;
    int32_t displaySyncRate_ = 0;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_PERFORMANCE_MONITOR_H

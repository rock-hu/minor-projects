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

#include "core/components_ng/manager/drag_drop/drag_drop_behavior_reporter/drag_drop_behavior_reporter.h"

#include "base/log/event_report.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
const std::string PNAME_ID {"ARKUI"};
const std::string PVERSION_ID {"1.0.0"};
} // namespace

DragDropBehaviorReporter& DragDropBehaviorReporter::GetInstance()
{
    static DragDropBehaviorReporter behaviorReporter;
    return behaviorReporter;
}

void DragDropBehaviorReporter::UpdateDragStartResult(DragStartResult result)
{
    if (startResult_ != DragStartResult::UNKNOW && result == DragStartResult::DRAG_START_SUCCESS) {
        return;
    }
    startResult_ = result;
}

void DragDropBehaviorReporter::UpdateDragStopResult(DragStopResult result)
{
    if (stopResult_ != DragStopResult::UNKNOW && result == DragStopResult::DRAG_SOTP_SUCCESS) {
        return;
    }
    stopResult_ = result;
}

void DragDropBehaviorReporter::UpdateRecordSize(int32_t recordSize)
{
    recordSize_ = recordSize;
}

void DragDropBehaviorReporter::UpdateSummaryType(const std::string& summaryType)
{
    summaryType_ = summaryType;
}

void DragDropBehaviorReporter::UpdateAllowDropType(const std::set<std::string>& allowDropType)
{
    allowDropType_ = allowDropType;
}

void DragDropBehaviorReporter::UpdateIsCrossing(CrossingEnd isCrossing)
{
    isCrossing_ = isCrossing;
}

void DragDropBehaviorReporter::UpdateContainerId(int32_t containerId)
{
    containerId_ = containerId;
}

void DragDropBehaviorReporter::Reset()
{
    isCrossing_ = CrossingEnd::NOT_CROSSING;
    startResult_ = DragStartResult::UNKNOW;
    stopResult_ = DragStopResult::UNKNOW;
    recordSize_ = 0;
    summaryType_ = "";
    allowDropType_ = {};
}

void DragDropBehaviorReporter::Submit(DragReporterPharse pharse, int32_t containerId)
{
    bool isStart = pharse == DragReporterPharse::DRAG_START;
    std::string dragBehavior = isStart ? "DRAG_START" : "DRAG_STOP";
    int32_t result = isStart ? static_cast<int32_t>(startResult_) : static_cast<int32_t>(stopResult_);
    std::string allowDropTypes;
    for (const auto& type: allowDropType_) {
        std::string str = type + ";";
        allowDropTypes += str;
    }
    containerId_ = containerId_ == INSTANCE_ID_UNDEFINED ? containerId : containerId_;
    auto container = Container::GetContainer(containerId_);
    CHECK_NULL_VOID(container);
    std::string hostName = container->GetBundleName();

    DragInfo dragInfo { static_cast<int32_t>(isCrossing_), result, recordSize_, dragBehavior, PNAME_ID,
        PVERSION_ID, hostName, summaryType_, allowDropTypes };

    auto pipeline = container->GetPipelineContext();
    CHECK_NULL_VOID(pipeline);
    auto taskScheduler = pipeline->GetTaskExecutor();
    CHECK_NULL_VOID(taskScheduler);
    taskScheduler->PostTask(
        [dragInfo]() {
            EventReport::ReportDragInfo(dragInfo);
        },
        TaskExecutor::TaskType::BACKGROUND, "ArkUIDragDropBehaviorReporter");
    Reset();
}
} // namespace OHOS::Ace::NG

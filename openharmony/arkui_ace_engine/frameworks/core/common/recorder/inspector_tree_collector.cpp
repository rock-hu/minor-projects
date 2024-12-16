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
#include "core/common/recorder/inspector_tree_collector.h"

#include <memory>
#include <string>

namespace OHOS::Ace::Recorder {
InspectorTreeCollector& InspectorTreeCollector::Get()
{
    static InspectorTreeCollector instance;
    return instance;
}

void InspectorTreeCollector::GetTree(GetInspectorTree&& getTreeFunc, OnInspectorTreeResult&& callback)
{
    if (taskNum_ != 0) {
        onResultFuncList_.emplace_back(std::move(callback));
        return;
    }
    root_ = JsonUtil::Create(true);
    taskNum_ = 0;
    onResultFuncList_.emplace_back(std::move(callback));
    IncreaseTaskNum();
    getTreeFunc();
    DecreaseTaskNum();
}

void InspectorTreeCollector::IncreaseTaskNum()
{
    UpdateTaskNum(1);
}

void InspectorTreeCollector::DecreaseTaskNum()
{
    UpdateTaskNum(-1);
}

void InspectorTreeCollector::UpdateTaskNum(int32_t num)
{
    taskNum_ += num;
    if (taskNum_ == 0) {
        if (!onResultFuncList_.empty()) {
            for (const auto& func : onResultFuncList_) {
                func(std::make_shared<std::string>(root_->ToString()));
            }
            onResultFuncList_.clear();
        }
        root_ = JsonUtil::Create(true);
    }
}

std::unique_ptr<JsonValue>& InspectorTreeCollector::GetJson()
{
    return root_;
}
} // namespace OHOS::Ace::Recorder

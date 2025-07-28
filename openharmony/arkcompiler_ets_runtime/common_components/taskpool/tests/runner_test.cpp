/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "common_components/tests/test_helper.h"
#include "common_components/taskpool/runner.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace common {
class RunnerTest : public common::test::BaseTestWithScope {
protected:
    void SetUp() override {}
    void TearDown() override {}

    static std::function<void(native_handle_type)> CreateMockPrologueHook()
    {
        return [](native_handle_type handle) {};
    }

    static std::function<void(native_handle_type)> CreateMockEpilogueHook()
    {
        return [](native_handle_type handle) {};
    }
};

class MockTask : public Task {
public:
    explicit MockTask(int32_t id)
        : Task(id), executed_(false) {}

    bool Run(uint32_t threadId) override
    {
        executed_ = true;
        return true;
    }

    bool IsExecuted() const { return executed_; }

private:
    std::atomic<bool> executed_;
};

HWTEST_F_L0(RunnerTest, InitializeRunnerWithThreads) {
    constexpr uint32_t threadNum = 4;
    Runner runner(threadNum, RunnerTest::CreateMockPrologueHook(), RunnerTest::CreateMockEpilogueHook());
    EXPECT_EQ(runner.GetTotalThreadNum(), threadNum);

    runner.TerminateThread();
}
}
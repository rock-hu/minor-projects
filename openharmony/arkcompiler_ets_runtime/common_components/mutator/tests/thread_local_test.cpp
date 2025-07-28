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

#include "common_components/mutator/thread_local.h"
#include "common_components/tests/test_helper.h"

namespace common {

class ThreadLocalTest : public common::test::BaseTestWithScope {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F_L0(ThreadLocalTest, GetThreadLocalData_ReturnsNonNull) {
    ThreadLocalData* data = ThreadLocal::GetThreadLocalData();
    EXPECT_NE(data, nullptr);
}
}
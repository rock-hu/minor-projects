/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hybrid/xgc/xgc.h"

#include <gtest/gtest.h>

namespace arkplatform::mem::test {

TEST(XGCTest, CreateDestroyTest) {
    auto xgcCreated = XGC::Create();
    ASSERT_EQ(xgcCreated, true);
    xgcCreated = XGC::Create();
    ASSERT_EQ(xgcCreated, false) << "Create singleton XGC class twice";
    auto *xgc = XGC::GetInstance();
    ASSERT_NE(xgc, nullptr);
    auto xgcDestroyed = XGC::Destroy();
    ASSERT_EQ(xgcDestroyed, true);
}

} // namespace arkplatform::mem::test

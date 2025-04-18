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

#ifndef TEST_UNITTEST_CORE_EVENT_MOUSE_STYLE_MANAGER_TEST_NG_H
#define TEST_UNITTEST_CORE_EVENT_MOUSE_STYLE_MANAGER_TEST_NG_H
#include "gtest/gtest.h"
#include <string>

#define private public
#define protected public

#include "test/mock/base/mock_pixel_map.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/render/mock_render_context.h"

#include "base/mousestyle/mouse_style.h"

namespace OHOS::Ace::NG {

struct MockMouseFormatCase {
    bool userSetCursor;
    std::optional<int32_t> mouseStyleNodeId;
    int32_t nodeId;
    MouseFormat mouseFormat;
    bool isByPass;
    MouseStyleChangeReason reason;
    bool expectSetResult;
    MouseFormat expectMouseFormat;
};

class MouseStyleManagerTestNG : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    void SetUp() override;
    void TearDown() override;
};
} // namespace OHOS::Ace::NG

#endif //TEST_UNITTEST_CORE_EVENT_MOUSE_STYLE_MANAGER_TEST_NG_H
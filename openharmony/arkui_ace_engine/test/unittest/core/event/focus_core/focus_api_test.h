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

#ifndef TEST_UNITTEST_CORE_EVENT_FOCUS_API_TEST_H
#define TEST_UNITTEST_CORE_EVENT_FOCUS_API_TEST_H
#include <cstddef>
#include <cstdint>

#include "test/unittest/core/event/focus_core/focus_test_base.h"
#include "gtest/gtest.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/size_t.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components/theme/app_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/event/focus_hub.h"
#include "core/components_ng/event/touch_event.h"
#include "core/components_ng/manager/focus/focus_view.h"
#include "core/components_ng/pattern/pattern.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/event/key_event.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "test/unittest/core/event/frame_node_on_tree.h"

namespace OHOS::Ace::NG {

class FocusApiTestNG : public FocusTestBaseNg, public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    testing::AssertionResult IsRequestImmediatelyByIdStateEqual(int32_t caseNum,
        const std::vector<bool>& actualState, const std::vector<bool>& expectState);
};
} // namespace OHOS::Ace::NG

#endif //TEST_UNITTEST_CORE_EVENT_FOCUS_HUB_TEST_NG_H
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

#ifndef FOUNDATION_ACE_TEST_UNITTEST_CORE_PATTERN_RELATIVE_CONTAINER_BASE_TEST_NG_H
#define FOUNDATION_ACE_TEST_UNITTEST_CORE_PATTERN_RELATIVE_CONTAINER_BASE_TEST_NG_H

#include "old_cases/relative_container_common_ng.h"
#include "test/unittest/core/pattern/test_ng.h"

#define protected public
#define private public

#include "core/components_ng/pattern/relative_container/relative_container_layout_algorithm.h"
#include "core/components_ng/pattern/relative_container/relative_container_layout_property.h"
#include "core/components_ng/pattern/relative_container/relative_container_model_ng.h"
#include "core/components_ng/pattern/relative_container/relative_container_pattern.h"

namespace OHOS::Ace::NG {
using namespace testing;
using namespace testing::ext;

class RelativeContainerBaseTestNG : public TestNG {
public:
    static void AddAlignRule(std::map<AlignDirection, AlignRule>& alignRules, const AlignDirection& direction,
        const std::string& id, const HorizontalAlign& horizontalRule);
    static void AddAlignRule(std::map<AlignDirection, AlignRule>& alignRules, const AlignDirection& direction,
        const std::string& id, const VerticalAlign& verticalRule);
    RefPtr<FrameNode> CreateRelativeContainer(const std::function<void(RelativeContainerModelNG)>& callback);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_TEST_UNITTEST_CORE_PATTERN_RELATIVE_CONTAINER_BASE_TEST_NG_H
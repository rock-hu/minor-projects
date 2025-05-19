/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_TEST_MOCK_MOCK_MOUSE_STYLE_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_TEST_MOCK_MOCK_MOUSE_STYLE_H

#include "gmock/gmock.h"

#include "base/mousestyle/mouse_style.h"

namespace OHOS::Ace {
class MockMouseStyle : public MouseStyle {
    DECLARE_ACE_TYPE(MockMouseStyle, MouseStyle);

public:
    MockMouseStyle() = default;
    ~MockMouseStyle() override = default;

    MOCK_CONST_METHOD2(SetPointerStyle, bool(int32_t, MouseFormat));
    MOCK_CONST_METHOD2(GetPointerStyle, int32_t(int32_t, int32_t&));
    MOCK_CONST_METHOD2(ChangePointerStyle, bool(int32_t, MouseFormat));
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_TEST_MOCK_MOCK_MOUSE_STYLE_H

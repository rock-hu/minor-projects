/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_ACELITE_TEST_TEXT_H
#define OHOS_ACELITE_TEST_TEXT_H

#include "base_test.h"

namespace OHOS {
namespace ACELite {
class TextTddTest : public BaseTest {
public:
    TextTddTest();
    ~TextTddTest() override {};
    void ComponentTextAttributeSetValueTest001();
    void ComponentTextStyleSetColorTest002();
    void ComponentTextStyleSetOverflowTest003();
    void ComponentTextStyleSetLetterSpacingTest004();
    void ComponentTextStyleSetAlignTest005();
    void ComponentTextStyleSetSizeFamilyTest006();
    void ComponentTextStyleSetLineHeightTest007();
    void ComponentTextStyleSetLineHeightTest008();
    void RunTests();
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TEST_TEXT_H

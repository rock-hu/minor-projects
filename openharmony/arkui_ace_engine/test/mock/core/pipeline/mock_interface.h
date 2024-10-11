/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PIPELINE_NG_TEST_MOCK_MOCK_INTERFACE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PIPELINE_NG_TEST_MOCK_MOCK_INTERFACE_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {
class FontManager : public AceType {
    DECLARE_ACE_TYPE(FontManager, AceType);
};

class ManagerInterface : public AceType {
    DECLARE_ACE_TYPE(ManagerInterface, AceType);
};

class OffscreenCanvas : public AceType {
    DECLARE_ACE_TYPE(OffscreenCanvas, AceType);
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PIPELINE_NG_TEST_MOCK_MOCK_INTERFACE_H

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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_OSAL_BACKGROUND_TASK_HELPER_OHOS_H
#define FOUNDATION_ACE_ADAPTER_OHOS_OSAL_BACKGROUND_TASK_HELPER_OHOS_H

#include "base/background_task_helper/background_task_helper.h"

namespace OHOS::Ace {

class BackgroundTaskHelperOhos : public BackgroundTaskHelper {
    DECLARE_ACE_TYPE(BackgroundTaskHelperOhos, BackgroundTaskHelper);

public:
    bool HasBackgroundTask() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_OSAL_BACKGROUND_TASK_HELPER_OHOS_H
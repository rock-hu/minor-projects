/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACE_COMPONENT_TEST_PROXY_H
#define OHOS_ACE_COMPONENT_TEST_PROXY_H

#include "component_test/component_test_manager_impl.h"

#include "base/memory/ace_type.h"
#include "core/common/container.h"

namespace OHOS::Ace::ComponentTest {
class ACE_FORCE_EXPORT ComponentTestManagerProxy {
public:
    static void PostUITask(std::function<void(void*)>&& task, std::function<void(void*)>&& onFinish,
        void* data = nullptr, uint32_t delay = 0);
    static void PostJSTask(std::function<void(void*)>&& task, std::function<void(void*)>&& onFinish,
        void* data = nullptr, uint32_t delay = 0);
    static void SetTestCaseAttribute(TestCaseAttribute attribute);
    static void Record(std::string info, std::string position, Result result);
    static void Finish();
    static void ClaimLongOperation();
    static void LongOperationComplete();
    static void RequestContinuousIdleStatusNotification(IdleNotifycallback&& notifycallback);
    static std::string GetTestConfigPage();
    static bool IsComponentTestEnabled();

private:
    static RefPtr<ComponentTestManager> GetCurrentComponentTestManager();
};
} // namespace OHOS::Ace::ComponentTest

#endif // OHOS_ACE_COMPONENT_TEST_PROXY_H

/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_NAPI_TEST_UNITTEST_TEST_H
#define FOUNDATION_ACE_NAPI_TEST_UNITTEST_TEST_H
#include "event_handler.h"
#include "native_engine.h"
#include "test_common.h"

class NativeEngineTest : public testing::Test {
public:
    NativeEngineTest();
    virtual ~NativeEngineTest();
    static void *Run(void *args);
protected:
    NativeEngine* engine_;
    NativeEngine* moduleNameEngine_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> eventHandler_ = nullptr;
};

#endif /* FOUNDATION_ACE_NAPI_TEST_UNITTEST_TEST_H */

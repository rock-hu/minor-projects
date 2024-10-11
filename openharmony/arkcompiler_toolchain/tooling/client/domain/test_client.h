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

#ifndef ECMASCRIPT_TOOLING_CLIENT_DOMAIN_TEST_CLIENT_H
#define ECMASCRIPT_TOOLING_CLIENT_DOMAIN_TEST_CLIENT_H

#include <iostream>
#include <map>

#include "tooling/base/pt_types.h"

using PtJson = panda::ecmascript::tooling::PtJson;
using Result = panda::ecmascript::tooling::Result;
namespace OHOS::ArkCompiler::Toolchain {
class TestClient final {
public:
    TestClient(uint32_t sessionId) : sessionId_(sessionId) {}
    ~TestClient() = default;

    bool DispatcherCmd(const std::string &cmd);
    int SuccessCommand();
    int FailCommand();

private:
    uint32_t sessionId_;
};
} // OHOS::ArkCompiler::Toolchain
#endif  // ECMASCRIPT_TOOLING_CLIENT_DOMAIN_TEST_CLIENT_H
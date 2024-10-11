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

#ifndef ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_SOURCE_TEST_H
#define ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_SOURCE_TEST_H

#include "tooling/test/client_utils/test_util.h"

namespace panda::ecmascript::tooling::test {
class JsSourceTest : public TestActions {
public:
    JsSourceTest()
    {
        testAction = {
            {SocketAction::SEND, "enable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "runtime-enable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "run"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            // load sample.js
            {SocketAction::RECV, "Debugger.scriptParsed", ActionRule::CUSTOM_RULE, [] (auto, auto, auto) -> bool {
                return true;
            }, TestCase::SOURCE},
            // break on start
            {SocketAction::RECV, "Debugger.paused", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, [] (auto recv, auto, auto) -> bool {
                std::unique_ptr<PtJson> json = PtJson::Parse(recv);
                Result ret;
                int id;
                ret = json->GetInt("id", &id);
                if (ret != Result::SUCCESS) {
                    return false;
                }

                std::unique_ptr<PtJson> result = nullptr;
                ret = json->GetObject("result", &result);
                if (ret != Result::SUCCESS) {
                    return false;
                }

                std::string scriptSource;
                ret = result->GetString("scriptSource", &scriptSource);
                if (ret != Result::SUCCESS) {
                    return false;
                }
                return true;
            }},
            // reply success and run
            {SocketAction::SEND, "success"},
            {SocketAction::SEND, "resume"},
            {SocketAction::RECV, "Debugger.resumed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
        };
    }

    std::pair<std::string, std::string> GetEntryPoint() override
    {
        return {pandaFile_, entryPoint_};
    }
    ~JsSourceTest() = default;

private:
    std::string pandaFile_ = DEBUGGER_ABC_DIR "sample.abc";
    std::string sourceFile_ = DEBUGGER_JS_DIR "sample.js";
    std::string entryPoint_ = "_GLOBAL::func_main_0";
};

std::unique_ptr<TestActions> GetJsSourceTest()
{
    return std::make_unique<JsSourceTest>();
}
}  // namespace panda::ecmascript::tooling::test

#endif  // ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_SOURCE_TEST_H

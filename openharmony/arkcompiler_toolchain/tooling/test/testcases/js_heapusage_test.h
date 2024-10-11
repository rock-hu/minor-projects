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

#ifndef ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_HEAPUSAGE_TEST_H
#define ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_HEAPUSAGE_TEST_H

#include "tooling/test/client_utils/test_util.h"

namespace panda::ecmascript::tooling::test {
class JsHeapusageTest : public TestActions {
public:
    JsHeapusageTest()
    {
        testAction = {
            {SocketAction::SEND, "enable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "runtime-enable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "run"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            // load sample.js
            {SocketAction::RECV, "Debugger.scriptParsed", ActionRule::STRING_CONTAIN},
            // break on start
            {SocketAction::RECV, "Debugger.paused", ActionRule::STRING_CONTAIN},
            // set breakpoint
            {SocketAction::SEND, "b " DEBUGGER_JS_DIR "sample.js 22"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            // hit breakpoint after resume first time
            {SocketAction::SEND, "resume"},
            {SocketAction::RECV, "Debugger.resumed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::RECV, "Debugger.paused", ActionRule::STRING_CONTAIN},
            // hit breakpoint after resume second time
            {SocketAction::SEND, "resume"},
            {SocketAction::RECV, "Debugger.resumed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::RECV, "Debugger.paused", ActionRule::CUSTOM_RULE,
                [this](auto recv, auto, auto) -> bool {
                    std::unique_ptr<PtJson> json = PtJson::Parse(recv);
                    Result ret;
                    std::string method = "";
                    ret = json->GetString("method", &method);
                    if (ret != Result::SUCCESS || method != "Debugger.paused") {
                        return false;
                    }

                    std::unique_ptr<PtJson> params = nullptr;
                    ret = json->GetObject("params", &params);
                    if (ret != Result::SUCCESS) {
                        return false;
                    }

                    std::unique_ptr<PtJson> hitBreakpoints = nullptr;
                    ret = params->GetArray("hitBreakpoints", &hitBreakpoints);
                    if (ret != Result::SUCCESS) {
                        return false;
                    }

                    std::string breakpoint = "";
                    breakpoint = hitBreakpoints->Get(0)->GetString();
                    if (ret != Result::SUCCESS || breakpoint.find(sourceFile_) == std::string::npos ||
                        breakpoint.find("21") == std::string::npos) {
                        return false;
                    }
                    return true;
                }},
            {SocketAction::SEND, "heapusage"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE,
                [](auto recv, auto, auto) -> bool {
                    std::unique_ptr<PtJson> json = PtJson::Parse(recv);
                    Result ret;
                    int id = 0;
                    ret = json->GetInt("id", &id);
                    if (ret != Result::SUCCESS) {
                        return false;
                    }

                    std::unique_ptr<PtJson> result = nullptr;
                    ret = json->GetObject("result", &result);
                    if (ret != Result::SUCCESS) {
                        return false;
                    }

                    int usedSize = 0;
                    ret = result->GetInt("usedSize", &usedSize);
                    if (ret != Result::SUCCESS) {
                        return false;
                    }

                    int totalSize = 0;
                    ret = result->GetInt("totalSize", &totalSize);
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
    ~JsHeapusageTest() = default;

private:
    std::string pandaFile_ = DEBUGGER_ABC_DIR "sample.abc";
    std::string sourceFile_ = DEBUGGER_JS_DIR "sample.js";
    std::string entryPoint_ = "_GLOBAL::func_main_0";
};

std::unique_ptr<TestActions> GetJsHeapusageTest()
{
    return std::make_unique<JsHeapusageTest>();
}
}  // namespace panda::ecmascript::tooling::test

#endif // ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_BREAKPOINT_TEST_H

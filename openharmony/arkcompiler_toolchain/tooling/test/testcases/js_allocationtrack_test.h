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

#ifndef ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_ALLOCATIONTRACK_TEST_H
#define ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_ALLOCATIONTRACK_TEST_H

#include "tooling/test/client_utils/test_util.h"

namespace panda::ecmascript::tooling::test {
class JsAllocationtrackTest : public TestActions {
public:
    JsAllocationtrackTest()
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
            {SocketAction::SEND, "heapprofiler-enable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "allocationtrack"},
            {SocketAction::RECV, "", ActionRule::STRING_CONTAIN},
            
            {SocketAction::SEND, "b " DEBUGGER_JS_DIR "sample.js 22"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            // hit breakpoint after resume first time
            {SocketAction::SEND, "resume"},
            {SocketAction::RECV, "Debugger.resumed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::RECV, "Debugger.paused", ActionRule::STRING_CONTAIN},

            {SocketAction::SEND, "resume"},
            {SocketAction::RECV, "Debugger.resumed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::RECV, "Debugger.paused", ActionRule::STRING_CONTAIN},
            {SocketAction::SEND, "allocationtrack-stop"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, [this] (auto recv, auto, auto) -> bool {
                return RecvReportProgress(recv);
            }},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, [this] (auto recv, auto, bool& needMoreMsg) -> bool {
                return RecvReportProgressFinished(recv, needMoreMsg);
            }},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, [this] (auto recv, auto, bool& needMoreMsg) -> bool {
                return RecvReportProgressChunk(recv, needMoreMsg);
            }},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "heapprofiler-disable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            // reply success and run
            {SocketAction::SEND, "success"},
            {SocketAction::SEND, "resume"},
            {SocketAction::RECV, "Debugger.resumed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
        };
    }

    bool RecvReportProgress(std::string recv)
    {
        std::unique_ptr<PtJson> json = PtJson::Parse(recv);
        Result ret;
        std::string method;
        ret = json->GetString("method", &method);
        if (ret != Result::SUCCESS) {
            return false;
        }

        if (method != "HeapProfiler.reportHeapSnapshotProgress") {
            return false;
        }
        std::unique_ptr<PtJson> params = nullptr;
        ret = json->GetObject("params", &params);
        if (ret != Result::SUCCESS) {
            return false;
        }
        int done;
        ret = params->GetInt("done", &done);
        if (ret != Result::SUCCESS) {
            return false;
        }
        return true;
    }

    bool RecvReportProgressFinished(std::string recv, bool &needMoreMsg)
    {
        needMoreMsg = false;
        std::unique_ptr<PtJson> json = PtJson::Parse(recv);
        Result ret;
        std::string method;
        ret = json->GetString("method", &method);
        if (ret != Result::SUCCESS) {
            return false;
        }

        if (method != "HeapProfiler.reportHeapSnapshotProgress") {
            return false;
        }

        std::unique_ptr<PtJson> params = nullptr;
        ret = json->GetObject("params", &params);
        if (ret != Result::SUCCESS) {
            return false;
        }

        int done;
        ret = params->GetInt("done", &done);
        if (ret != Result::SUCCESS) {
            return false;
        }

        bool finished;
        ret = params->GetBool("finished", &finished);
        if (ret != Result::SUCCESS) {
            needMoreMsg = true;
        }
        return true;
    }

    bool RecvReportProgressChunk(std::string recv, bool& needMoreMsg)
    {
        static std::string content;
        needMoreMsg = false;
        std::unique_ptr<PtJson> json = PtJson::Parse(recv);
        Result ret;
        std::string method;
        ret = json->GetString("method", &method);
        if (ret != Result::SUCCESS) {
            return false;
        }

        if (method != "HeapProfiler.addHeapSnapshotChunk") {
            return false;
        }

        std::unique_ptr<PtJson> params = nullptr;
        ret = json->GetObject("params", &params);
        if (ret != Result::SUCCESS) {
            return false;
        }

        std::string chunk;
        ret = params->GetString("chunk", &chunk);
        if (ret != Result::SUCCESS) {
            return false;
        }

        content += chunk;
        std::unique_ptr<PtJson> contentJson = PtJson::Parse(content);
        if (contentJson == nullptr || contentJson->Stringify().empty()) {
            needMoreMsg = true;
        }
        return true;
    }

    std::pair<std::string, std::string> GetEntryPoint() override
    {
        return {pandaFile_, entryPoint_};
    }

    ~JsAllocationtrackTest() = default;

private:
    std::string pandaFile_ = DEBUGGER_ABC_DIR "sample.abc";
    std::string sourceFile_ = DEBUGGER_JS_DIR "sample.js";
    std::string entryPoint_ = "sample";
};

std::unique_ptr<TestActions> GetJsAllocationtrackTest()
{
    return std::make_unique<JsAllocationtrackTest>();
}
}  // namespace panda::ecmascript::tooling::test

#endif  // ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_ALLOCATIONTRACK_TEST_H

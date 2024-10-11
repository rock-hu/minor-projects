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

#ifndef ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_HEAPPROFILER_DUMP_TEST_H
#define ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_HEAPPROFILER_DUMP_TEST_H

#include "tooling/test/client_utils/test_util.h"

namespace panda::ecmascript::tooling::test {
class JsHeapProfilerDumpTest : public TestActions {
public:
    JsHeapProfilerDumpTest()
    {
        testAction = {
            {SocketAction::SEND, "enable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "runtime-enable"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            {SocketAction::SEND, "run"},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},
            // load export_variable_second.js
            {SocketAction::RECV, "Debugger.scriptParsed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "Debugger.paused", ActionRule::STRING_CONTAIN},
            {SocketAction::SEND, "resume"},
            {SocketAction::RECV, "Debugger.resumed", ActionRule::STRING_CONTAIN},
            {SocketAction::RECV, "", ActionRule::CUSTOM_RULE, MatchRule::replySuccess},

            {SocketAction::SEND, "hd"},
            {SocketAction::RECV, "HeapProfiler.reportHeapSnapshotProgress", ActionRule::CUSTOM_RULE,
                [] (auto recv, auto, auto) -> bool {
                std::unique_ptr<PtJson> json = PtJson::Parse(recv);
                Result ret;
                std::unique_ptr<PtJson> params;
                ret = json->GetObject("params", &params);
                if (ret != Result::SUCCESS) {
                    return false;
                }

                int32_t done;
                ret = params->GetInt("done", &done);
                if (ret != Result::SUCCESS || done != 0) {
                    return false;
                }

                int32_t total;
                ret = params->GetInt("total", &total);
                if (ret != Result::SUCCESS) {
                    return false;
                }
                return true;
            }},
            {SocketAction::RECV, "HeapProfiler.reportHeapSnapshotProgress", ActionRule::CUSTOM_RULE,
                [] (auto recv, auto, auto) -> bool {
                std::unique_ptr<PtJson> json = PtJson::Parse(recv);
                Result ret;
                std::unique_ptr<PtJson> params;
                ret = json->GetObject("params", &params);
                if (ret != Result::SUCCESS) {
                    return false;
                }

                int32_t done;
                ret = params->GetInt("done", &done);
                if (ret != Result::SUCCESS) {
                    return false;
                }

                int32_t total;
                ret = params->GetInt("total", &total);
                if (ret != Result::SUCCESS) {
                    return false;
                }
                return true;
            }},
            {SocketAction::RECV, "HeapProfiler.addHeapSnapshotChunk", ActionRule::CUSTOM_RULE,
                [] (auto recv, auto, auto) -> bool {
                std::unique_ptr<PtJson> json = PtJson::Parse(recv);
                Result ret;
                std::unique_ptr<PtJson> params;
                ret = json->GetObject("params", &params);
                if (ret != Result::SUCCESS) {
                    return false;
                }

                std::string chunk;
                ret = params->GetString("chunk", &chunk);
                if (ret != Result::SUCCESS) {
                    return false;
                }
                return true;
            }},
            // reply success and run
            {SocketAction::SEND, "success"},
        };
    }

    std::pair<std::string, std::string> GetEntryPoint() override
    {
        return {pandaFile_, entryPoint_};
    }
    ~JsHeapProfilerDumpTest() = default;

private:
    std::string pandaFile_ = DEBUGGER_ABC_DIR "sample.abc";
    std::string sourceFile_ = DEBUGGER_JS_DIR "sample.js";
    std::string entryPoint_ = "_GLOBAL::func_main_0";
};

std::unique_ptr<TestActions> GetJsHeapProfilerDumpTest()
{
    return std::make_unique<JsHeapProfilerDumpTest>();
}
}  // namespace panda::ecmascript::tooling::test

#endif  // ECMASCRIPT_TOOLING_TEST_TESTCASES_JS_HEAPPROFILER_DUMP_TEST_H

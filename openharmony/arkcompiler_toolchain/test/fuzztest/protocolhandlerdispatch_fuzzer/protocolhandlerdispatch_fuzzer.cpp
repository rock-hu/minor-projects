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

#include "protocolhandlerdispatch_fuzzer.h"
#include "ecmascript/napi/include/jsnapi.h"
#include "tooling/protocol_handler.h"

using namespace panda;
using namespace panda::ecmascript;
using namespace panda::ecmascript::tooling;

namespace OHOS {
    void ProtocolHandlerDispatchFuzzTest(const uint8_t* data, size_t size)
    {
        RuntimeOption option;
        option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
        auto vm = JSNApi::CreateJSVM(option);
        {
            if (size <= 0) {
                return;
            }
            using ProtoHandler = const std::function<void(const void *, const std::string &)>;
            ProtoHandler ph = [data, size]([[maybe_unused]] const void *d,
                [[maybe_unused]] const std::string &s) -> void { d = data + size; };
            ProtocolHandler handler(ph, vm);
            std::string msg((const char*)data, size);
            handler.DispatchCommand(msg + std::string("hello"));
        }
        JSNApi::DestroyJSVM(vm);
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::ProtocolHandlerDispatchFuzzTest(data, size);
    return 0;
}

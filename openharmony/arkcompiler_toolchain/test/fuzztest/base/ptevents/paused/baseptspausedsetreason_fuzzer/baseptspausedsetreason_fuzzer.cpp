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

#include "baseptspausedsetreason_fuzzer.h"
#include "ecmascript/napi/include/jsnapi.h"
#include "tooling/base/pt_events.h"
#include "tooling/dispatcher.h"
#include "test/fuzztest/common_fuzzer/common_fuzzer.h"

using namespace panda;
using namespace panda::ecmascript;
using namespace panda::ecmascript::tooling;

#define MAXBYTELEN sizeof(int32_t)

namespace OHOS {
    void BasePtsPausedSetReasonFuzzTest(const uint8_t* data, size_t size)
    {
        auto cn = std::make_unique<common_fuzzer>();
        auto vm = cn->GetEcvm();
        if (size <= 0) {
            return;
        }
        auto str = cn->GetString(data, size);
        Paused paused;
        std::vector<std::unique_ptr<CallFrame>> v;
        paused.SetCallFrames(std::move(v)).SetReason(PauseReason::EXCEPTION);
        cn->DestroyEcvm(vm);
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::BasePtsPausedSetReasonFuzzTest(data, size);
    return 0;
}
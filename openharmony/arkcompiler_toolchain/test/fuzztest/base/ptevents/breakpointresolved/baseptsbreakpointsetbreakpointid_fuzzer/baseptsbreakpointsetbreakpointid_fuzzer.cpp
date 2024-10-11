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

#include "baseptsbreakpointsetbreakpointid_fuzzer.h"
#include "ecmascript/napi/include/jsnapi.h"
#include "tooling/base/pt_events.h"
#include "tooling/dispatcher.h"
#include "test/fuzztest/common_fuzzer/common_fuzzer.h"

using namespace panda;
using namespace panda::ecmascript;
using namespace panda::ecmascript::tooling;

#define MAXBYTELEN sizeof(int32_t)

namespace OHOS {
    void BasePtsBreakpointSetBreakpointIdFuzzTest(const uint8_t* data, size_t size)
    {
        auto cn = std::make_unique<common_fuzzer>();
        auto vm = cn->GetEcvm();
        int index = 0;
        int32_t scid = 2;
        std::string str(data, data + size);
        if (size <= 0) {
            return;
        }
        if (size > MAXBYTELEN) {
            size = MAXBYTELEN;
        }
        if (memcpy_s(&index, MAXBYTELEN, data, size) != 0) {
            std::cout << "memcpy_s failed!";
            UNREACHABLE();
        }
        BreakpointResolved breakpointResolved;
        auto location = std::make_unique<Location>();
        location->SetScriptId(scid).SetLine(index);
        breakpointResolved.SetBreakpointId("00").SetLocation(std::move(location));
        cn->DestroyEcvm(vm);
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::BasePtsBreakpointSetBreakpointIdFuzzTest(data, size);
    return 0;
}
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

#include "baseptbase64decode_fuzzer.h"
#include "ecmascript/napi/include/jsnapi.h"
#include "tooling/base/pt_base64.h"
#include "tooling/dispatcher.h"
#include "test/fuzztest/common_fuzzer/common_fuzzer.h"

using namespace panda;
using namespace panda::ecmascript;
using namespace panda::ecmascript::tooling;

#define MAXBYTELEN sizeof(int32_t)

namespace OHOS {
    void BasePtBase64DecodeFuzzTest(const uint8_t* data, size_t size)
    {
        auto cn = std::make_unique<common_fuzzer>();
        auto vm = cn->GetEcvm();
        if (size <= 0) {
            return;
        }
        auto src = cn->GetString(data, size);
        std::string dest;
        dest.resize(PtBase64::EncodedSize(src.size()));
        PtBase64::Encode(dest.data(), src.data(), src.size());
        src = dest;
        dest.resize(PtBase64::DecodedSize(src.size()));
        PtBase64::Decode(dest.data(), src.data(), src.size());
        cn->DestroyEcvm(vm);
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::BasePtBase64DecodeFuzzTest(data, size);
    return 0;
}
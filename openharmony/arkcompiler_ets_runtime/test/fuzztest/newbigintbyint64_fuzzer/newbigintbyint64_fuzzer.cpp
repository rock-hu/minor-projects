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

#include "ecmascript/napi/include/jsnapi.h"
#include "ecmascript/log_wrapper.h"
#include "ecmascript/base/string_helper.h"
#include "newbigintbyint64_fuzzer.h"

using namespace panda;
using namespace panda::ecmascript;

namespace OHOS {
    void NewBigIntByInt64FuzzTest(const uint8_t* data, size_t size)
    {
        RuntimeOption option;
        option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
        EcmaVM *vm = JSNApi::CreateJSVM(option);
        if (size <= 0) {
            return;
        }
        int64_t input = 0;
        size_t maxByteLen = 8;
        if (size > maxByteLen) {
            size = maxByteLen;
        }
        if (memcpy_s(&input, maxByteLen, data, size) != EOK) {
            std::cout << "memcpy_s failed!";
            UNREACHABLE();
        }
        BigIntRef::New(vm, input);
        JSNApi::DestroyJSVM(vm);
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::NewBigIntByInt64FuzzTest(data, size);
    return 0;
}
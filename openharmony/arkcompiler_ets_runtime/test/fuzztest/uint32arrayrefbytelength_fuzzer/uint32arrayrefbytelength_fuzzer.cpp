/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uint32arrayrefbytelength_fuzzer.h"
#include "ecmascript/ecma_string-inl.h"
#include "ecmascript/napi/include/jsnapi.h"

using namespace panda;
using namespace panda::ecmascript;

#define MAXBYTELEN sizeof(int32_t)

namespace OHOS {
void Uint32ArrayRefByteLengthFuzzTest(const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
    EcmaVM *vm = JSNApi::CreateJSVM(option);
    if (data == nullptr || size <= 0) {
        LOG_ECMA(ERROR) << "illegal input!";
        return;
    }
    int32_t input;
    if (size > MAXBYTELEN) {
        size = MAXBYTELEN;
    }
    if (memcpy_s(&input, MAXBYTELEN, data, size) != 0) {
        LOG_ECMA(ERROR) << "memcpy_s failed !";
        UNREACHABLE();
    }
    const int32_t MaxMenory = 1024;
    if (input > MaxMenory) {
        input = MaxMenory;
    }
    Local<ArrayBufferRef> ref = ArrayBufferRef::New(vm, input);
    Local<Uint32ArrayRef> typedArray = Uint32ArrayRef::New(vm, ref, (int32_t)size, (int32_t)size);
    typedArray->ByteLength(vm);
    JSNApi::DestroyJSVM(vm);
}
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Run your code on data.
    OHOS::Uint32ArrayRefByteLengthFuzzTest(data, size);
    return 0;
}
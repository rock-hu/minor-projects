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

#include "jsvaluerefisbig64array_fuzzer.h"
#include "ecmascript/ecma_string-inl.h"
#include "ecmascript/napi/include/jsnapi.h"

using namespace panda;
using namespace panda::ecmascript;

#define MAXBYTELEN sizeof(int32_t)

namespace OHOS {
void JSValueRefIsBigInt64ArrayFuzzTest(const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(common::LOG_LEVEL::ERROR);
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
    Local<BigInt64ArrayRef> typedArray = BigInt64ArrayRef::New(vm, ref, (int32_t)size, (int32_t)size);
    typedArray->IsBigInt64Array(vm);
    JSNApi::DestroyJSVM(vm);
}

void JSValueRefIsBigUint64ArrayRefNewFuzzTest(const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(common::LOG_LEVEL::ERROR);
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
    Local<BigUint64ArrayRef> typedArray = BigUint64ArrayRef::New(vm, ref, (int32_t)size, (int32_t)size);
    typedArray->IsBigUint64Array(vm);
    JSNApi::DestroyJSVM(vm);
}
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Run your code on data.
    OHOS::JSValueRefIsBigInt64ArrayFuzzTest(data, size);
    OHOS::JSValueRefIsBigUint64ArrayRefNewFuzzTest(data, size);
    return 0;
}
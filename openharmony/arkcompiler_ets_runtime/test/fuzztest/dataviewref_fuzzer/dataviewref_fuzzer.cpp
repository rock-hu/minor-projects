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

#include "dataviewref_fuzzer.h"
#include "ecmascript/base/string_helper.h"
#include "ecmascript/base/utf_helper.h"
#include "ecmascript/napi/include/jsnapi.h"

using namespace panda;
using namespace panda::ecmascript;

namespace OHOS {
void DataViewRefByteOffSetFuzzTest([[maybe_unused]]const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
    EcmaVM *vm = JSNApi::CreateJSVM(option);
    const int32_t length = 15;
    Local<ArrayBufferRef> arrayBuffer = ArrayBufferRef::New(vm, length);
    const uint32_t byteOffset = 5;
    const uint32_t byteLength = 7;
    Local<DataViewRef> dataView = DataViewRef::New(vm, arrayBuffer, byteOffset, byteLength);
    if (size <= 0) {
        LOG_ECMA(ERROR) << "illegal input!";
        return;
    }
    (void)dataView->ByteOffset();
    JSNApi::DestroyJSVM(vm);
}

void DataViewRefByteLengthFuzzTest([[maybe_unused]]const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
    EcmaVM *vm = JSNApi::CreateJSVM(option);
    const int32_t length = 15;
    Local<ArrayBufferRef> arrayBuffer = ArrayBufferRef::New(vm, length);
    const uint32_t byteOffset = 5;
    const uint32_t byteLength = 7;
    Local<DataViewRef> dataView = DataViewRef::New(vm, arrayBuffer, byteOffset, byteLength);
    if (size <= 0) {
        LOG_ECMA(ERROR) << "illegal input!";
        return;
    }
    (void)dataView->ByteLength();
    JSNApi::DestroyJSVM(vm);
}

void DataViewRefGetArrayBufferFuzzTest([[maybe_unused]]const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
    EcmaVM *vm = JSNApi::CreateJSVM(option);
    const int32_t length = 15;
    Local<ArrayBufferRef> arrayBuffer = ArrayBufferRef::New(vm, length);
    const uint32_t byteOffset = 5;
    const uint32_t byteLength = 7;
    Local<DataViewRef> dataView = DataViewRef::New(vm, arrayBuffer, byteOffset, byteLength);
    if (size <= 0) {
        LOG_ECMA(ERROR) << "illegal input!";
        return;
    }
    (void)dataView->GetArrayBuffer(vm);
    JSNApi::DestroyJSVM(vm);
}
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Run your code on data.
    OHOS::DataViewRefByteOffSetFuzzTest(data, size);
    OHOS::DataViewRefByteLengthFuzzTest(data, size);
    OHOS::DataViewRefGetArrayBufferFuzzTest(data, size);
    return 0;
}
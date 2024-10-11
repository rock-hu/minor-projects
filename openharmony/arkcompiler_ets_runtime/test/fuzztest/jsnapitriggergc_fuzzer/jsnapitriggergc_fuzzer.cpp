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

#include "jsnapitriggergc_fuzzer.h"
#include "ecmascript/ecma_string-inl.h"
#include "ecmascript/napi/include/jsnapi.h"

using namespace panda;
using namespace panda::ecmascript;

namespace OHOS {
constexpr size_t DEFAULT_THREAD_COUNT = 3;

void JSNApiTriggerGCFuzztest([[maybe_unused]]const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
    EcmaVM *vm = JSNApi::CreateJSVM(option);
    if (size <= 0) {
        LOG_ECMA(ERROR) << "illegal input!";
        return;
    }
    if (size % DEFAULT_THREAD_COUNT == 0) {
        JSNApi::TRIGGER_GC_TYPE gcType = JSNApi::TRIGGER_GC_TYPE::FULL_GC;
        JSNApi::TriggerGC(vm, gcType);
    } else if (size % DEFAULT_THREAD_COUNT == 1) {
        JSNApi::TRIGGER_GC_TYPE gcType = JSNApi::TRIGGER_GC_TYPE::OLD_GC;
        JSNApi::TriggerGC(vm, gcType);
    } else {
        JSNApi::TRIGGER_GC_TYPE gcType = JSNApi::TRIGGER_GC_TYPE::SEMI_GC;
        JSNApi::TriggerGC(vm, gcType);
    }
    JSNApi::DestroyJSVM(vm);
}
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Run your code on data.
    OHOS::JSNApiTriggerGCFuzztest(data, size);
    return 0;
}
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

#include <fuzzer/FuzzedDataProvider.h>
#include "setrefgetvalue_fuzzer.h"
#include "ecmascript/base/string_helper.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_set.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/linked_hash_table.h"
#include "ecmascript/napi/include/jsnapi.h"
#include "ecmascript/napi/jsnapi_helper.h"
#include "ecmascript/object_factory.h"

using namespace panda;
using namespace panda::ecmascript;

namespace OHOS {
void SetRefGetValueFuzztest(const uint8_t *data, size_t size)
{
    RuntimeOption option;
    option.SetLogLevel(common::LOG_LEVEL::ERROR);
    EcmaVM *vm = JSNApi::CreateJSVM(option);
    {
        JsiFastNativeScope scope(vm);
        if (size <= 0) {
            LOG_ECMA(ERROR) << "illegal input!";
            return;
        }
        ObjectFactory *factory = vm->GetFactory();
        JSHandle<GlobalEnv> env = vm->GetGlobalEnv();
        JSHandle<JSTaggedValue> constructor = env->GetBuiltinsSetFunction();
        JSHandle<JSSet> jsSet =
            JSHandle<JSSet>::Cast(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), constructor));
        JSHandle<LinkedHashSet> hashSet = LinkedHashSet::Create(vm->GetJSThread());
        jsSet->SetLinkedSet(vm->GetJSThread(), hashSet);
        JSHandle<JSTaggedValue> setTag = JSHandle<JSTaggedValue>::Cast(jsSet);
        Local<SetRef> set = JSNApiHelper::ToLocal<SetRef>(setTag);
        FuzzedDataProvider fdp(data, size);
        std::string str = fdp.ConsumeRandomLengthString(1024);
        JSHandle<JSTaggedValue> value(factory->NewFromStdString(str));
        std::string str1 = fdp.ConsumeRandomLengthString(1024);
        JSHandle<JSTaggedValue> firstValue(factory->NewFromStdString(str1));
        JSSet::Add(vm->GetJSThread(), jsSet, firstValue);
        std::string str2 = fdp.ConsumeRandomLengthString(1024);
        JSHandle<JSTaggedValue> secondValue(factory->NewFromStdString(str2));
        JSSet::Add(vm->GetJSThread(), jsSet, secondValue);
        int index = fdp.ConsumeIntegralInRange<int>(0, 1);
        set->GetValue(vm, index);
    }
    JSNApi::DestroyJSVM(vm);
}
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Run your code on data.
    OHOS::SetRefGetValueFuzztest(data, size);
    return 0;
}
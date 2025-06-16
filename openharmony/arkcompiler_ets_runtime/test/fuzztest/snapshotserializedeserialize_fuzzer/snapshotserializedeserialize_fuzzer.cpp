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

#include "snapshotserializedeserialize_fuzzer.h"

#include "ecmascript/log_wrapper.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/snapshot/mem/snapshot.h"

using namespace panda;
using namespace panda::ecmascript;

namespace OHOS {
    void SnapshotSerializeDeserializeFuzzTest(const uint8_t* data, size_t size)
    {
        RuntimeOption option;
        option.SetLogLevel(common::LOG_LEVEL::ERROR);
        EcmaVM *vm = JSNApi::CreateJSVM(option);
        {
            JsiFastNativeScope scope(vm);
            if (size <= 0) {
                return;
            }
            auto factory = vm->GetFactory();
            JSHandle<TaggedArray> array = factory->NewTaggedArray(*data);
            if (*data > 0) {
                JSHandle<EcmaString> str = factory->NewFromStdString("str1");
                array->Set(vm->GetAssociatedJSThread(), 0, str.GetTaggedValue());
            }
            const CString fileName = "snapshot";
            Snapshot snapshotSerialize(vm);
            // serialize
            snapshotSerialize.Serialize(*array, nullptr, fileName);
            // deserialize
            Snapshot snapshotDeserialize(vm);
            snapshotDeserialize.Deserialize(SnapshotType::VM_ROOT, fileName);
            // remove snapshot file if exist
            std::remove(fileName.c_str());
        }
        JSNApi::DestroyJSVM(vm);
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::SnapshotSerializeDeserializeFuzzTest(data, size);
    return 0;
}
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

#include "ecmascript/js_api/js_api_plain_array_iterator.h"

#include "ecmascript/base/typed_array_helper-inl.h"
#include "ecmascript/containers/containers_errors.h"
#include "ecmascript/js_api/js_api_plain_array.h"
#include "ecmascript/js_array.h"

namespace panda::ecmascript {
using BuiltinsBase = base::BuiltinsBase;
using ContainerError = containers::ContainerError;
using ErrorFlag = containers::ErrorFlag;
JSTaggedValue JSAPIPlainArrayIterator::Next(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> input(BuiltinsBase::GetThis(argv));
    if (!input->IsJSAPIPlainArrayIterator()) {
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::BIND_ERROR,
                                                            "The Symbol.iterator method cannot be bound");
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    JSHandle<JSAPIPlainArrayIterator> iter(input);
    JSHandle<JSTaggedValue> plainArray(thread, iter->GetIteratedPlainArray());
    JSHandle<JSTaggedValue> undefinedHandle(thread, JSTaggedValue::Undefined());
    if (plainArray->IsUndefined()) {
        return JSIterator::CreateIterResultObject(thread, undefinedHandle, true).GetTaggedValue();
    }

    JSHandle<JSAPIPlainArray> apiPlainArray(plainArray);
    ASSERT(plainArray->IsJSAPIPlainArray());

    uint32_t length = apiPlainArray->GetLength();
    int32_t index = iter->GetNextIndex();
    if (static_cast<uint32_t>(index) >= length) {
        iter->SetIteratedPlainArray(thread, undefinedHandle);
        return JSIterator::CreateIterResultObject(thread, undefinedHandle, true).GetTaggedValue();
    }
    iter->SetNextIndex(index + 1);
    JSHandle<TaggedArray> valueArray(thread, TaggedArray::Cast(apiPlainArray->GetValues().GetTaggedObject()));
    JSHandle<JSTaggedValue> value(thread, valueArray->Get(index));
    JSHandle<TaggedArray> keyArray(thread, TaggedArray::
                                   Cast(JSHandle<JSAPIPlainArray>(plainArray)->GetKeys().GetTaggedObject()));
    JSHandle<JSTaggedValue> keyHandle(thread, keyArray->Get(index));
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<TaggedArray> array(factory->NewTaggedArray(2)); // 2 means the length of array
    array->Set(thread, 0, keyHandle);
    array->Set(thread, 1, value);
    JSHandle<JSTaggedValue> keyAndValue(JSArray::CreateArrayFromList(thread, array));
    return JSIterator::CreateIterResultObject(thread, keyAndValue, false).GetTaggedValue();
}
} // namespace panda::ecmascript

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

#include "ecmascript/js_api/js_api_tree_set.h"

#include "ecmascript/containers/containers_errors.h"
#include "ecmascript/tagged_tree.h"

namespace panda::ecmascript {
using ContainerError = containers::ContainerError;
using ErrorFlag = containers::ErrorFlag;
void JSAPITreeSet::Add(JSThread *thread, const JSHandle<JSAPITreeSet> &set, const JSHandle<JSTaggedValue> &value)
{
    if (!TaggedTreeSet::IsKey(value.GetTaggedValue())) {
        JSHandle<EcmaString> result = JSTaggedValue::ToString(thread, value.GetTaggedValue());
        RETURN_IF_ABRUPT_COMPLETION(thread);
        CString errorMsg =
            "The type of \"value\" must be Key of JS. Received value is: " + ConvertToString(thread, *result);
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::TYPE_ERROR, errorMsg.c_str());
        THROW_NEW_ERROR_AND_RETURN(thread, error);
    }
    JSHandle<TaggedTreeSet> setHandle(thread, TaggedTreeSet::Cast(set->GetTreeSet(thread).GetTaggedObject()));

    JSTaggedValue newSet = TaggedTreeSet::Add(thread, setHandle, value);
    RETURN_IF_ABRUPT_COMPLETION(thread);
    set->SetTreeSet(thread, newSet);
}

int JSAPITreeSet::GetSize(const JSThread *thread) const
{
    return TaggedTreeSet::Cast(GetTreeSet(thread).GetTaggedObject())->NumberOfElements();
}

JSTaggedValue JSAPITreeSet::GetKey(JSThread *thread, int entry) const
{
    ASSERT_PRINT(entry < GetSize(thread), "entry must less than capacity");
    JSTaggedValue key = TaggedTreeSet::Cast(GetTreeSet(thread).GetTaggedObject())->GetKey(thread, entry);
    return key.IsHole() ? JSTaggedValue::Undefined() : key;
}

bool JSAPITreeSet::Delete(JSThread *thread, const JSHandle<JSAPITreeSet> &set, const JSHandle<JSTaggedValue> &key)
{
    JSHandle<TaggedTreeSet> setHandle(thread, TaggedTreeSet::Cast(set->GetTreeSet(thread).GetTaggedObject()));

    int entry = TaggedTreeSet::FindEntry(thread, setHandle, key);
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
    if (entry < 0) {
        return false;
    }
    JSTaggedValue newSet = TaggedTreeSet::Delete(thread, setHandle, entry);
    set->SetTreeSet(thread, newSet);
    return true;
}

bool JSAPITreeSet::Has(JSThread *thread, const JSHandle<JSAPITreeSet> &set, const JSHandle<JSTaggedValue> &key)
{
    JSHandle<TaggedTreeSet> setHandle(thread, TaggedTreeSet::Cast(set->GetTreeSet(thread).GetTaggedObject()));
    return TaggedTreeSet::FindEntry(thread, setHandle, key) >= 0;
}

void JSAPITreeSet::Clear(const JSThread *thread, const JSHandle<JSAPITreeSet> &set)
{
    int cap = set->GetSize(thread);
    if (cap == 0) {
        return;
    }
    JSTaggedValue fn = TaggedTreeSet::Cast(set->GetTreeSet(thread).GetTaggedObject())->GetCompare(thread);
    JSHandle<JSTaggedValue> compareFn = JSHandle<JSTaggedValue>(thread, fn);
    JSTaggedValue internal = TaggedTreeSet::Create(thread, cap);
    if (!compareFn->IsUndefined() && !compareFn->IsNull()) {
        TaggedTreeSet::Cast(internal.GetTaggedObject())->SetCompare(thread, compareFn.GetTaggedValue());
    }
    set->SetTreeSet(thread, internal);
}

JSTaggedValue JSAPITreeSet::PopFirst(JSThread *thread, const JSHandle<JSAPITreeSet> &set)
{
    JSHandle<TaggedTreeSet> setHandle(thread, TaggedTreeSet::Cast(set->GetTreeSet(thread).GetTaggedObject()));
    int entry = setHandle->GetMinimum(setHandle->GetRootEntries());
    if (entry < 0) {
        return JSTaggedValue::Undefined();
    }
    JSHandle<JSTaggedValue> value(thread, setHandle->GetKey(thread, entry));
    JSTaggedValue newSet = TaggedTreeSet::Delete(thread, setHandle, entry);
    set->SetTreeSet(thread, newSet);
    return value.GetTaggedValue();
}

JSTaggedValue JSAPITreeSet::PopLast(JSThread *thread, const JSHandle<JSAPITreeSet> &set)
{
    JSHandle<TaggedTreeSet> setHandle(thread, TaggedTreeSet::Cast(set->GetTreeSet(thread).GetTaggedObject()));
    int entry = setHandle->GetMaximum(setHandle->GetRootEntries());
    if (entry < 0) {
        return JSTaggedValue::Undefined();
    }
    JSHandle<JSTaggedValue> value(thread, setHandle->GetKey(thread, entry));
    JSTaggedValue newSet = TaggedTreeSet::Delete(thread, setHandle, entry);
    set->SetTreeSet(thread, newSet);
    return value.GetTaggedValue();
}
}  // namespace panda::ecmascript

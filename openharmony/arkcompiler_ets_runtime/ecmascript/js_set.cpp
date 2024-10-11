/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ecmascript/js_set.h"
#include "ecmascript/linked_hash_table.h"

namespace panda::ecmascript {
void JSSet::Add(JSThread *thread, const JSHandle<JSSet> &set, const JSHandle<JSTaggedValue> &value)
{
    if (!LinkedHashSet::IsKey(value.GetTaggedValue())) {
        //  throw error
        THROW_TYPE_ERROR(thread, "the value must be Key of JSSet");
    }
    JSHandle<LinkedHashSet> setHandle(thread, LinkedHashSet::Cast(set->GetLinkedSet().GetTaggedObject()));

    JSHandle<LinkedHashSet> newSet = LinkedHashSet::Add(thread, setHandle, value);
    set->SetLinkedSet(thread, newSet);
}

bool JSSet::Delete(const JSThread *thread, const JSHandle<JSSet> &set, const JSHandle<JSTaggedValue> &value)
{
    JSHandle<LinkedHashSet> setHandle(thread, LinkedHashSet::Cast(set->GetLinkedSet().GetTaggedObject()));
    int entry = setHandle->FindElement(thread, value.GetTaggedValue());
    if (entry == -1) {
        return false;
    }
    setHandle->RemoveEntry(thread, entry);
    return true;
}

void JSSet::Clear(const JSThread *thread, const JSHandle<JSSet> &set)
{
    LinkedHashSet *linkedSet = LinkedHashSet::Cast(set->GetLinkedSet().GetTaggedObject());
    JSHandle<LinkedHashSet> setHandle(thread, LinkedHashSet::Cast(set->GetLinkedSet().GetTaggedObject()));
    JSHandle<LinkedHashSet> newSet = linkedSet->Clear(thread, setHandle);
    set->SetLinkedSet(thread, newSet);
}

bool JSSet::Has(const JSThread *thread, JSTaggedValue value) const
{
    return LinkedHashSet::Cast(GetLinkedSet().GetTaggedObject())->Has(thread, value);
}

uint32_t JSSet::GetSize() const
{
    return LinkedHashSet::Cast(GetLinkedSet().GetTaggedObject())->NumberOfElements();
}

JSTaggedValue JSSet::GetValue(int entry) const
{
    ASSERT_PRINT(entry >= 0 && static_cast<uint32_t>(entry) < GetSize(),
        "entry must be non-negative integer less than capacity");
    return LinkedHashSet::Cast(GetLinkedSet().GetTaggedObject())->GetValue(entry);
}
}  // namespace panda::ecmascript

/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_LAYOUT_INFO_INL_H
#define ECMASCRIPT_LAYOUT_INFO_INL_H

#include "ecmascript/layout_info.h"

#include "ecmascript/ic/properties_cache.h"
#include "ecmascript/js_thread.h"

namespace panda::ecmascript {
inline int LayoutInfo::GetPropertiesCapacity() const
{
    return static_cast<int>((GetLength()) >> ELEMENTS_INDEX_LOG2);
}

inline int LayoutInfo::NumberOfElements() const
{
    return GetExtraLength();
}

inline void LayoutInfo::SetNumberOfElements([[maybe_unused]] const JSThread *thread, int properties)
{
    SetExtraLength(properties);
}

inline uint32_t LayoutInfo::GetKeyIndex(int index) const
{
    return static_cast<uint32_t>(index) << ELEMENTS_INDEX_LOG2;
}

inline uint32_t LayoutInfo::GetAttrIndex(int index) const
{
    return (static_cast<uint32_t>(index) << ELEMENTS_INDEX_LOG2) + ATTR_INDEX_OFFSET;
}

inline void LayoutInfo::SetWithoutBarrier(uint32_t idx, const JSTaggedValue &value)
{
    ASSERT(idx < GetLength());
    ASSERT(!value.IsHeapObject());
    size_t offset = JSTaggedValue::TaggedTypeSize() * idx;

    Barriers::SetPrimitive<JSTaggedType>(GetData(), offset, value.GetRawData());
}

inline void LayoutInfo::SetPropertyInit(const JSThread *thread, int index, const JSTaggedValue &key,
                                        const PropertyAttributes &attr)
{
    uint32_t fixedIdx = GetKeyIndex(index);
    TaggedArray::Set(thread, fixedIdx, key);
    TaggedArray::Set(thread, fixedIdx + ATTR_INDEX_OFFSET, attr.GetNormalTagged());
}

inline void LayoutInfo::SetNormalAttr(const JSThread *thread, int index, const PropertyAttributes &attr)
{
    uint32_t fixedIdx = GetAttrIndex(index);
    PropertyAttributes oldAttr(TaggedArray::Get(thread, fixedIdx));
    oldAttr.SetNormalAttr(attr.GetNormalAttr());
    TaggedArray::Set(thread, fixedIdx, oldAttr.GetTaggedValue());
}

inline JSTaggedValue LayoutInfo::GetKey(const JSThread *thread, int index) const
{
    uint32_t fixedIdx = GetKeyIndex(index);
    return TaggedArray::Get(thread, fixedIdx);
}

inline PropertyAttributes LayoutInfo::GetAttr(const JSThread *thread, int index) const
{
    uint32_t fixedIdx = GetAttrIndex(index);
    return PropertyAttributes(TaggedArray::Get(thread, fixedIdx));
}

template<RBMode mode>
inline PropertyAttributes LayoutInfo::GetAttr(const JSThread *thread, int index) const
{
    uint32_t fixedIdx = GetAttrIndex(index);
    return PropertyAttributes(TaggedArray::Get<mode>(thread, fixedIdx));
}

inline JSTaggedValue LayoutInfo::GetSortedKey(const JSThread *thread, int index) const
{
    uint32_t fixedIdx = GetSortedIndex(thread, index);
    return GetKey(thread, fixedIdx);
}

inline uint32_t LayoutInfo::GetSortedIndex(const JSThread *thread, int index) const
{
    return GetAttr(thread, index).GetSortedIndex();
}

inline void LayoutInfo::SetSortedIndex(const JSThread *thread, int index, int sortedIndex)
{
    uint32_t fixedIdx = GetAttrIndex(index);
    PropertyAttributes attr(TaggedArray::Get(thread, fixedIdx));
    attr.SetSortedIndex(sortedIndex);
    TaggedArray::Set(thread, fixedIdx, attr.GetTaggedValue());
}

inline int LayoutInfo::FindElementWithCache(const JSThread *thread, JSHClass *cls, JSTaggedValue key,
                                            int propertiesNumber)
{
    ASSERT(NumberOfElements() >= propertiesNumber);
    const int MAX_ELEMENTS_LINER_SEARCH = 9; // 9: Builtins Object properties number is nine;
    if (propertiesNumber <= MAX_ELEMENTS_LINER_SEARCH) {
        void *properties = reinterpret_cast<void *>(GetProperties());
        size_t keyOffset = 0;
        if (thread->NeedReadBarrier()) {
            for (int i = 0; i < propertiesNumber; i++) {
                JSTaggedValue propKey(Barriers::GetTaggedValue<RBMode::FAST_CMC_RB>(thread,
                    ToUintPtr(properties) + i * sizeof(Properties) + keyOffset));
                if (propKey == key) {
                    return i;
                }
            }
        } else {
            for (int i = 0; i < propertiesNumber; i++) {
                JSTaggedValue propKey(Barriers::GetTaggedValue<RBMode::FAST_NO_RB>(thread,
                    ToUintPtr(properties) + i * sizeof(Properties) + keyOffset));
                if (propKey == key) {
                    return i;
                }
            }
        }
        return -1;
    }

    // jit compile thread not use cache
    if (thread->IsJitThread()) {
        return BinarySearch(thread, key, propertiesNumber);
    }

    PropertiesCache *cache = thread->GetPropertiesCache();
    int index = cache->Get(thread, cls, key);
    if (index == PropertiesCache::NOT_FOUND) {
        index = BinarySearch(thread, key, propertiesNumber);
        if (index != -1) {
            cache->Set(thread, cls, key, index);
        }
    }
    return index;
}

inline int LayoutInfo::BinarySearch(const JSThread *thread, JSTaggedValue key, int propertiesNumber)
{
    ASSERT(NumberOfElements() >= propertiesNumber);
    int low = 0;
    int elements = NumberOfElements();
    int high = elements - 1;
    uint32_t keyHash = key.GetKeyHashCode(thread);

    ASSERT(low <= high);

    while (low <= high) {
        int mid = low + (high - low) / 2;  // 2: half
        JSTaggedValue midKey = GetSortedKey(thread, mid);
        uint32_t midHash = midKey.GetKeyHashCode(thread);
        if (midHash > keyHash) {
            high = mid - 1;
        } else if (midHash < keyHash) {
            low = mid + 1;
        } else {
            int sortIndex = static_cast<int>(GetSortedIndex(thread, mid));
            JSTaggedValue currentKey = GetKey(thread, sortIndex);
            if (currentKey == key) {
                return sortIndex < propertiesNumber ? sortIndex : -1;
            }
            int midLeft = mid;
            int midRight = mid;
            while (midLeft - 1 >= 0) {
                sortIndex = static_cast<int>(GetSortedIndex(thread, --midLeft));
                currentKey = GetKey(thread, sortIndex);
                if (currentKey.GetKeyHashCode(thread) == keyHash) {
                    if (currentKey == key) {
                        return sortIndex < propertiesNumber ? sortIndex : -1;
                    }
                } else {
                    break;
                }
            }
            while (midRight + 1 < elements) {
                sortIndex = static_cast<int>(GetSortedIndex(thread, ++midRight));
                currentKey = GetKey(thread, sortIndex);
                if (currentKey.GetKeyHashCode(thread) == keyHash) {
                    if (currentKey == key) {
                        return sortIndex < propertiesNumber ? sortIndex : -1;
                    }
                } else {
                    break;
                }
            }
            return -1;
        }
    }
    return -1;
}

inline void LayoutInfo::SetIsNotHole(const JSThread *thread, int index)
{
    uint32_t fixedIdx = GetAttrIndex(index);
    PropertyAttributes attr(TaggedArray::Get(thread, fixedIdx));
    attr.SetIsNotHole(true);
    TaggedArray::Set(thread, fixedIdx, attr.GetTaggedValue());
}

inline void LayoutInfo::UpdateTrackTypeAttr(const JSThread *thread, int index, const PropertyAttributes &attr)
{
    uint32_t fixedIdx = GetAttrIndex(index);
    PropertyAttributes oldAttr(TaggedArray::Get(thread, fixedIdx));
    oldAttr.SetNormalAttr(attr.GetNormalAttr());
    oldAttr.SetIsPGODumped(false);
    SetWithoutBarrier(fixedIdx, oldAttr.GetTaggedValue());
}

inline void LayoutInfo::SetIsPGODumped(const JSThread *thread, int index)
{
    uint32_t fixedIdx = GetAttrIndex(index);
    PropertyAttributes attr(TaggedArray::Get(thread, fixedIdx));
    attr.SetIsPGODumped(true);
    SetWithoutBarrier(fixedIdx, attr.GetTaggedValue());
}

inline bool LayoutInfo::CheckIsDuplicateKey(const JSThread *thread, int curKeyIdx, uint32_t curKeyHashCode,
                                            const JSTaggedValue &key)
{
    while (curKeyIdx > 0) {
        JSTaggedValue prevKey = GetSortedKey(thread, --curKeyIdx);
        if (prevKey.GetKeyHashCode(thread) < curKeyHashCode) {
            return false;
        }
        if (prevKey == key) {
            return true;
        }
    }
    return false;
}

template<bool checkDuplicateKeys /* = false*/>
void LayoutInfo::AddKey(const JSThread *thread, [[maybe_unused]] int index, const JSTaggedValue &key,
                        const PropertyAttributes &attr)
{
    int number = NumberOfElements();
    ASSERT(attr.GetOffset() == static_cast<uint32_t>(number));
    ASSERT(number + 1 <= GetPropertiesCapacity());
    ASSERT(number == index);
    SetNumberOfElements(thread, number + 1);
    SetPropertyInit(thread, number, key, attr);

    uint32_t keyHash = key.GetKeyHashCode(thread);
    int insertIndex = number;
    for (; insertIndex > 0; --insertIndex) {
        auto sortedIndex = GetSortedIndex(thread, insertIndex - 1);
        JSTaggedValue prevKey = GetKey(thread, sortedIndex);
        if (prevKey.GetKeyHashCode(thread) <= keyHash) {
            break;
        }
        SetSortedIndex(thread, insertIndex, sortedIndex);
    }
    SetSortedIndex(thread, insertIndex, number);
    if constexpr (checkDuplicateKeys) {
        if (CheckIsDuplicateKey(thread, insertIndex, keyHash, key)) {
            THROW_TYPE_ERROR(const_cast<JSThread *>(thread), "property keys can not duplicate");
        }
    }
}
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_LAYOUT_INFO_INL_H

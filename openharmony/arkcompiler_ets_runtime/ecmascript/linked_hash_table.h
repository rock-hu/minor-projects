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

#ifndef ECMASCRIPT_LINKED_HASH_TABLE_H
#define ECMASCRIPT_LINKED_HASH_TABLE_H

#include "ecmascript/js_tagged_value.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_symbol.h"
#include "ecmascript/js_tagged_number.h"
#include "ecmascript/tagged_array-inl.h"

namespace panda::ecmascript {
class LinkedHash {
public:
    static int Hash(const JSThread *thread, JSTaggedValue key);
};

/**
 * memory in LinkedHashTable is divided into 3 parts
 * 1.array[0-2] is used to store common information of hashtale such as numberOfElements and capacity
 * 2.array[3,3+capacity] is buckets which store the position of entry
 * 3.array[3+capacity+1,3+capacity + capacity*(entry_size+1)] is the entry stored in order, the last element of an entry
 * is a number which point to next entry.
 * */
template<typename Derived, typename HashObject>
class LinkedHashTable : public TaggedArray {
public:
    static const int MIN_CAPACITY = 4;
    static const int NUMBER_OF_ELEMENTS_INDEX = 0;
    static const int NUMBER_OF_DELETED_ELEMENTS_INDEX = 1;
    static const int CAPACITY_INDEX = 2;
    static const int NEXT_TABLE_INDEX = 3;
    static const int ELEMENTS_START_INDEX = 4;
    // Don't shrink a HashTable below this capacity.
    static const int MIN_SHRINK_CAPACITY = 16;

    static JSHandle<Derived> Create(const JSThread *thread, int numberOfElements, MemSpaceKind spaceKind);

    static JSHandle<Derived> Insert(const JSThread *thread, const JSHandle<Derived> &table,
                                    const JSHandle<JSTaggedValue> &key, const JSHandle<JSTaggedValue> &value);

    static JSHandle<Derived> InsertWeakRef(const JSThread *thread, const JSHandle<Derived> &table,
                                           const JSHandle<JSTaggedValue> &key, const JSHandle<JSTaggedValue> &value);

    static JSHandle<Derived> GrowCapacity(const JSThread *thread, const JSHandle<Derived> &table,
                                          int numberOfAddedElements = 1);

    static JSHandle<Derived> Remove(const JSThread *thread, const JSHandle<Derived> &table,
                                    const JSHandle<JSTaggedValue> &key);

    static JSHandle<Derived> Shrink(const JSThread *thread, const JSHandle<Derived> &table, int additionalCapacity = 0);

    static int GetLengthOfTable(int numberOfElements)
    {
        return ELEMENTS_START_INDEX + numberOfElements + numberOfElements * (HashObject::ENTRY_SIZE + 1);
    }

    inline bool HasSufficientCapacity(int numOfAddElements) const
    {
        int numberOfElements = NumberOfElements();
        int numOfDelElements = NumberOfDeletedElements();
        int capacity = Capacity();
        int nof = numberOfElements + numOfAddElements;
        // Return true if:
        //   50% is still free after adding numOfAddElements elements and
        //   at most 50% of the free elements are deleted elements.
        if ((nof < capacity) && ((numOfDelElements <= (capacity - nof) / 2))) {  // 2: half
            int neededFree = nof / 2;                                            // 2: half
            if (nof + neededFree <= capacity) {
                return true;
            }
        }
        return false;
    }

    inline int FindElement(const JSThread *thread, JSTaggedValue key) const
    {
        if (!IsKey(key)) {
            return -1;
        }
        int hash = LinkedHash::Hash(thread, key);
        uint32_t bucket = HashToBucket(hash);
        for (JSTaggedValue entry = GetElement(thread, BucketToIndex(bucket)); !entry.IsHole();
            entry = GetNextEntry(thread, entry.GetInt())) {
            JSTaggedValue element = GetKey(thread, entry.GetInt());
            if (element.IsHole()) {
                continue;
            }
            if (element.IsWeak()) {
                element.RemoveWeakTag();
            }
            if (HashObject::IsMatch(thread, key, element)) {
                return entry.GetInt();
            }
        }
        return -1;
    }

    inline void RemoveEntry(const JSThread *thread, int entry)
    {
        ASSERT_PRINT(entry >= 0 && entry < Capacity(), "entry must be a non-negative integer less than capacity");
        int index = static_cast<int>(EntryToIndex(entry));
        for (int i = 0; i < HashObject::ENTRY_SIZE; i++) {
            SetElement(thread, index + i, JSTaggedValue::Hole());
        }
        SetNumberOfElements(thread, NumberOfElements() - 1);
        SetNumberOfDeletedElements(thread, NumberOfDeletedElements() + 1);
    }

    inline void RemoveEntryFromGCThread(int entry)
    {
        // Skip Barrier
        ASSERT_PRINT(entry >= 0 && entry < Capacity(), "entry must be a non-negative integer less than capacity");
        int index = static_cast<int>(EntryToIndex(entry));
        for (int i = 0; i < HashObject::ENTRY_SIZE; i++) {
            Barriers::SetPrimitive<JSTaggedType>(GetData(), JSTaggedValue::TaggedTypeSize() * (index + i),
                                                 JSTaggedValue::Hole().GetRawData());
        }
        JSTaggedValue newNumOfElements = JSTaggedValue(NumberOfElements() - 1);
        JSTaggedValue newNumOfDeletedElements = JSTaggedValue(NumberOfDeletedElements() + 1);
        Barriers::SetPrimitive<JSTaggedType>(GetData(), JSTaggedValue::TaggedTypeSize() * NUMBER_OF_ELEMENTS_INDEX,
                                             newNumOfElements.GetRawData());
        Barriers::SetPrimitive<JSTaggedType>(GetData(),
            JSTaggedValue::TaggedTypeSize() * NUMBER_OF_DELETED_ELEMENTS_INDEX, newNumOfDeletedElements.GetRawData());
    }

    inline static int ComputeCapacity(uint32_t atLeastSpaceFor)
    {
        // Add 50% slack to make slot collisions sufficiently unlikely.
        // See matching computation in HashTable::HasSufficientCapacity().
        uint32_t rawCap = atLeastSpaceFor + (atLeastSpaceFor >> 1UL);
        int capacity = static_cast<int>(helpers::math::GetPowerOfTwoValue32(rawCap));
        return (capacity > MIN_CAPACITY) ? capacity : MIN_CAPACITY;
    }

    inline static int ComputeCapacityWithShrink(int currentCapacity, int atLeastSpaceFor)
    {
        // Shrink to fit the number of elements if only a quarter of the
        // capacity is filled with elements.
        if (atLeastSpaceFor > (currentCapacity / 4)) {  // 4: quarter
            return currentCapacity;
        }
        // Recalculate the smaller capacity actually needed.
        int newCapacity = ComputeCapacity(atLeastSpaceFor);
        ASSERT_PRINT(newCapacity > atLeastSpaceFor, "new capacity must greater than atLeastSpaceFor");
        // Don't go lower than room for MIN_SHRINK_CAPACITY elements.
        if (newCapacity < Derived::MIN_SHRINK_CAPACITY) {
            return currentCapacity;
        }
        return newCapacity;
    }

    inline int NumberOfElements() const
    {
        return GetPrimitive(NUMBER_OF_ELEMENTS_INDEX).GetInt();
    }

    inline int NumberOfDeletedElements() const
    {
        return GetPrimitive(NUMBER_OF_DELETED_ELEMENTS_INDEX).GetInt();
    }

    inline int Capacity() const
    {
        return JSTaggedValue(GetPrimitive(CAPACITY_INDEX)).GetInt();
    }

    inline JSTaggedValue GetKey(const JSThread *thread, int entry) const
    {
        int index = static_cast<int>(EntryToIndex(entry));
        return GetElement(thread, index);
    }

    inline JSTaggedValue GetValue(const JSThread *thread, int entry) const
    {
        int index = static_cast<int>(EntryToIndex(entry)) + HashObject::ENTRY_VALUE_INDEX;
        return GetElement(thread, index);
    }

    inline static bool IsKey(JSTaggedValue key)
    {
        return !key.IsHole();
    }

    inline void SetNumberOfElements(const JSThread *thread, int nof)
    {
        Set(thread, NUMBER_OF_ELEMENTS_INDEX, JSTaggedValue(nof));
    }

    inline void SetNumberOfDeletedElements(const JSThread *thread, int nod)
    {
        Set(thread, NUMBER_OF_DELETED_ELEMENTS_INDEX, JSTaggedValue(nod));
    }

    inline void SetCapacity(const JSThread *thread, int capacity)
    {
        Set(thread, CAPACITY_INDEX, JSTaggedValue(capacity));
    }

    inline JSTaggedValue GetNextTable(const JSThread *thread) const
    {
        return JSTaggedValue(Get(thread, NEXT_TABLE_INDEX));
    }

    inline void SetNextTable(const JSThread *thread, JSTaggedValue nextTable)
    {
        Set(thread, NEXT_TABLE_INDEX, nextTable);
    }

    inline int GetDeletedElementsAt(const JSThread *thread, int entry) const
    {
        ASSERT_PRINT(!GetNextTable(thread).IsUndefined(), "function only execute after rehash");
        int currentEntry = entry - 1;
        if (NumberOfDeletedElements() == -1) {
            return entry;
        }
        while (currentEntry >= 0) {
            if (GetKey(thread, currentEntry).IsHole()) {
                return GetDeletedNum(thread, currentEntry);
            }
            currentEntry--;
        }
        return 0;
    }

    inline void Rehash(const JSThread *thread, Derived *newTable)
    {
        ASSERT_PRINT(newTable != nullptr && newTable->Capacity() > NumberOfElements(), "can not rehash to new table");
        // Rehash elements to new table
        int numberOfAllElements = NumberOfElements() + NumberOfDeletedElements();
        int desEntry = 0;
        int currentDeletedElements = 0;
        SetNextTable(thread, JSTaggedValue(newTable));
        for (int i = 0; i < numberOfAllElements; i++) {
            int fromIndex = static_cast<int>(EntryToIndex(i));
            JSTaggedValue key = GetElement(thread, fromIndex);
            if (key.IsHole()) {
                // store num_of_deleted_element before entry i; it will be used when iterator update.
                currentDeletedElements++;
                SetDeletedNum(thread, i, JSTaggedValue(currentDeletedElements));
                continue;
            }

            if (key.IsWeak()) {
                // If the key is a weak reference, we use the weak referent to calculate the new index in the new table.
                key.RemoveWeakTag();
            }

            int bucket = static_cast<int>(newTable->HashToBucket(LinkedHash::Hash(thread, key)));
            newTable->InsertNewEntry(thread, bucket, desEntry);
            int desIndex = static_cast<int>(newTable->EntryToIndex(desEntry));
            for (int j = 0; j < HashObject::ENTRY_SIZE; j++) {
                newTable->SetElement(thread, desIndex + j, GetElement(thread, fromIndex + j));
            }
            desEntry++;
        }
        newTable->SetNumberOfElements(thread, NumberOfElements());
        newTable->SetNumberOfDeletedElements(thread, 0);
    }

protected:
    inline JSTaggedValue GetElement(const JSThread *thread, int index) const
    {
        ASSERT(index >= 0 && index < static_cast<int>(GetLength()));
        return Get(thread, index);
    }

    inline void SetElement(const JSThread *thread, int index, JSTaggedValue element)
    {
        ASSERT(index >= 0 && index < static_cast<int>(GetLength()));
        Set(thread, index, element);
    }

    inline void SetKey(const JSThread *thread, int entry, JSTaggedValue key)
    {
        int index = static_cast<int>(EntryToIndex(entry));
        SetElement(thread, index, key);
    }

    inline void SetValue(const JSThread *thread, int entry, JSTaggedValue value)
    {
        int index = static_cast<int>(EntryToIndex(entry)) + HashObject::ENTRY_VALUE_INDEX;
        SetElement(thread, index, value);
    }

    inline JSTaggedValue GetNextEntry(const JSThread *thread, int entry) const
    {
        int index = static_cast<int>(EntryToIndex(entry)) + HashObject::ENTRY_SIZE;
        return GetElement(thread, index);
    }

    inline void SetNextEntry(const JSThread *thread, int entry, JSTaggedValue nextEntry)
    {
        int index = static_cast<int>(EntryToIndex(entry)) + HashObject::ENTRY_SIZE;
        SetElement(thread, index, nextEntry);
    }

    inline uint32_t HashToBucket(uint32_t hash) const
    {
        return hash & static_cast<uint32_t>(Capacity() - 1);
    }

    inline static uint32_t BucketToIndex(uint32_t bucket)
    {
        return bucket + ELEMENTS_START_INDEX;
    }

    // min entry = 0
    inline uint32_t EntryToIndex(uint32_t entry) const
    {
        return ELEMENTS_START_INDEX + Capacity() + static_cast<int>(entry) * (HashObject::ENTRY_SIZE + 1);
    }

    inline void InsertNewEntry(const JSThread *thread, int bucket, int entry)
    {
        int bucketIndex = static_cast<int>(BucketToIndex(bucket));
        JSTaggedValue previousEntry = GetElement(thread, bucketIndex);
        SetNextEntry(thread, entry, previousEntry);
        SetElement(thread, bucketIndex, JSTaggedValue(entry));
    }

    inline int GetDeletedNum(const JSThread *thread, int entry) const
    {
        ASSERT_PRINT(!GetNextTable(thread).IsUndefined(), "function only execute after rehash");
        return GetNextEntry(thread, entry).GetInt();
    }

    inline void SetDeletedNum(const JSThread *thread, int entry, JSTaggedValue num)
    {
        ASSERT_PRINT(!GetNextTable(thread).IsUndefined(), "function only execute after rehash");
        SetNextEntry(thread, entry, num);
    }
};

class LinkedHashMapObject {
public:
    // key must be string now for other object has no 'equals' method
    static inline bool IsMatch(const JSThread *thread, JSTaggedValue key, JSTaggedValue other)
    {
        return JSTaggedValue::SameValueZero(thread, key, other);
    }

    static const int ENTRY_SIZE = 2;
    static const int ENTRY_VALUE_INDEX = 1;
};

class LinkedHashMap : public LinkedHashTable<LinkedHashMap, LinkedHashMapObject> {
public:
    static LinkedHashMap *Cast(TaggedObject *obj)
    {
        return static_cast<LinkedHashMap *>(obj);
    }
    static JSHandle<LinkedHashMap> PUBLIC_API Create(const JSThread *thread, int numberOfElements = MIN_CAPACITY,
        MemSpaceKind spaceKind = MemSpaceKind::LOCAL);

    static JSHandle<LinkedHashMap> Delete(const JSThread *thread, const JSHandle<LinkedHashMap> &obj,
        const JSHandle<JSTaggedValue> &key);

    static JSHandle<LinkedHashMap> Set(const JSThread *thread, const JSHandle<LinkedHashMap> &obj,
        const JSHandle<JSTaggedValue> &key, const JSHandle<JSTaggedValue> &value);

    static JSHandle<LinkedHashMap> SetWeakRef(const JSThread *thread, const JSHandle<LinkedHashMap> &obj,
        const JSHandle<JSTaggedValue> &key, const JSHandle<JSTaggedValue> &value);

    JSTaggedValue Get(const JSThread *thread, JSTaggedValue key) const;

    static JSHandle<LinkedHashMap> Shrink(const JSThread *thread, const JSHandle<LinkedHashMap> &table,
        int additionalCapacity = 0);

    bool Has(const JSThread *thread, JSTaggedValue key) const;

    static JSHandle<LinkedHashMap> Clear(const JSThread *thread, const JSHandle<LinkedHashMap> &table);

    void ClearAllDeadEntries(const JSThread *thread, std::function<bool(JSTaggedValue)> &visitor);
    DECL_DUMP()
};

class LinkedHashSetObject {
public:
    // key must be string now for other object has no 'equals' method
    static inline bool IsMatch(const JSThread *thread, JSTaggedValue key, JSTaggedValue other)
    {
        return JSTaggedValue::SameValueZero(thread, key, other);
    }

    static const int ENTRY_SIZE = 1;
    static const int ENTRY_VALUE_INDEX = 0;
};

class LinkedHashSet : public LinkedHashTable<LinkedHashSet, LinkedHashSetObject> {
public:
    static LinkedHashSet *Cast(TaggedObject *obj)
    {
        return static_cast<LinkedHashSet *>(obj);
    }
    static JSHandle<LinkedHashSet> Create(const JSThread *thread, int numberOfElements = MIN_CAPACITY,
        MemSpaceKind spaceKind = MemSpaceKind::LOCAL);

    static JSHandle<LinkedHashSet> Delete(const JSThread *thread, const JSHandle<LinkedHashSet> &obj,
        const JSHandle<JSTaggedValue> &key);

    static JSHandle<LinkedHashSet> Add(const JSThread *thread, const JSHandle<LinkedHashSet> &obj,
        const JSHandle<JSTaggedValue> &key);

    static JSHandle<LinkedHashSet> AddWeakRef(const JSThread *thread, const JSHandle<LinkedHashSet> &obj,
        const JSHandle<JSTaggedValue> &key);

    static JSHandle<LinkedHashSet> Shrink(const JSThread *thread, const JSHandle<LinkedHashSet> &table,
        int additionalCapacity = 0);

    bool Has(const JSThread *thread, JSTaggedValue key) const;

    static JSHandle<LinkedHashSet> Clear(const JSThread *thread, const JSHandle<LinkedHashSet> &table);
    DECL_DUMP()
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_LINKED_HASH_TABLE_H

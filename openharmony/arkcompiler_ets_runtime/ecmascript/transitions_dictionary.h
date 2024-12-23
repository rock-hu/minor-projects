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

#ifndef ECMASCRIPT_TRANSITIONS_DICTIONARY_H
#define ECMASCRIPT_TRANSITIONS_DICTIONARY_H

#include "ecmascript/accessor_data.h"
#include "ecmascript/js_symbol.h"
#include "ecmascript/js_tagged_number.h"
#include "ecmascript/tagged_hash_table.h"

namespace panda::ecmascript {
class TransitionsDictionary : public TaggedHashTable<TransitionsDictionary> {
public:
    using HashTableT = TaggedHashTable<TransitionsDictionary>;
    static inline bool IsMatch([[maybe_unused]] const JSTaggedValue &key,
                               [[maybe_unused]] const JSTaggedValue &otherKey)
    {
        LOG_ECMA(FATAL) << "this branch is unreachable";
        UNREACHABLE();
    }
    static inline int Hash([[maybe_unused]] const JSTaggedValue &key)
    {
        LOG_ECMA(FATAL) << "this branch is unreachable";
        UNREACHABLE();
    }

    static inline bool IsMatch(const JSTaggedValue &key, const JSTaggedValue &metaData, const JSTaggedValue &otherKey,
                               const JSTaggedValue &otherDetails)
    {
        return key == otherKey && metaData == otherDetails;
    }

    static inline int Hash(const JSTaggedValue &key, const JSTaggedValue &metaData)
    {
        ASSERT(key.IsStringOrSymbol());

        uint32_t hash = 0;
        if (key.IsString()) {
            hash = EcmaStringAccessor(key).GetHashcode();
        } else if (key.IsSymbol()) {
            hash = JSSymbol::Cast(key.GetTaggedObject())->GetHashField();
        }
        int metaDataHash = metaData.IsInt() ? metaData.GetInt() : static_cast<int>(metaData.GetRawData());
        return static_cast<int>(hash) + metaDataHash;
    }

    inline static int GetKeyIndex(int entry)
    {
        return HashTableT::TABLE_HEADER_SIZE + entry * GetEntrySize() + ENTRY_KEY_INDEX;
    }
    inline static int GetValueIndex(int entry)
    {
        return HashTableT::TABLE_HEADER_SIZE + entry * GetEntrySize() + ENTRY_VALUE_INDEX;
    }
    inline static int GetEntryIndex(int entry)
    {
        return HashTableT::TABLE_HEADER_SIZE + entry * GetEntrySize();
    }
    inline static int GetEntrySize()
    {
        return ENTRY_SIZE;
    }

    static TransitionsDictionary *Cast(TaggedObject *object)
    {
        return reinterpret_cast<TransitionsDictionary *>(object);
    }

    static constexpr int DEFAULT_ELEMENTS_NUMBER = 16;
    static JSHandle<TransitionsDictionary> Create(const JSThread *thread,
                                                  int numberOfElements = DEFAULT_ELEMENTS_NUMBER)
    {
        return HashTableT::Create(thread, numberOfElements);
    }

    // Attempt to shrink the dictionary after deletion of key.
    inline static JSHandle<TransitionsDictionary> Shrink(const JSThread *thread,
                                                         const JSHandle<TransitionsDictionary> &dictionary)
    {
        return HashTableT::Shrink(thread, dictionary, 0);
    }

    inline JSTaggedValue GetAttributes(int entry) const
    {
        int index = GetEntryIndex(entry) + ENTRY_DETAILS_INDEX;
        return HashTableT::Get(index);
    }

    inline void SetAttributes(const JSThread *thread, int entry, JSTaggedValue metaData)
    {
        int index = GetEntryIndex(entry) + ENTRY_DETAILS_INDEX;
        HashTableT::Set(thread, index, metaData);
    }

    inline void SetEntry(const JSThread *thread, int entry, const JSTaggedValue &key, const JSTaggedValue &value,
                         const JSTaggedValue &metaData)
    {
        SetKey(thread, entry, key);
        JSTaggedValue weakValue = JSTaggedValue(value.CreateAndGetWeakRef());
        SetValue(thread, entry, weakValue);
        if (!metaData.IsHeapObject()) {
            SetAttributes(thread, entry, metaData);
            return;
        }
        JSTaggedValue weakMetaData = JSTaggedValue(metaData.CreateAndGetWeakRef());
        SetAttributes(thread, entry, weakMetaData);
    }

    inline void RemoveElement(const JSThread *thread, int entry)
    {
        SetKey(thread, entry, JSTaggedValue::Hole());
        SetValue(thread, entry, JSTaggedValue::Hole());
        SetAttributes(thread, entry, JSTaggedValue::Hole());
        IncreaseHoleEntriesCount(thread);
    }

    int FindEntry(const JSTaggedValue &key, const JSTaggedValue &metaData);
    template <typename Callback>
    void IterateEntryValue(Callback callback)
    {
        auto number = EntriesCount();
        int size = Size();
        int hasIteratedNum = 0;
        for (int entry = 0; entry < size; entry++) {
            JSTaggedValue ret = GetValue(entry);
            if (ret.IsWeak()) {
                auto next = ret.GetTaggedWeakRef();
                callback(JSHClass::Cast(next));
                hasIteratedNum++;
                if (hasIteratedNum >= number) {
                    return;
                }
            }
        }
    }
    static JSHandle<TransitionsDictionary> PutIfAbsent(const JSThread *thread,
                                                       const JSHandle<TransitionsDictionary> &dictionary,
                                                       const JSHandle<JSTaggedValue> &key,
                                                       const JSHandle<JSTaggedValue> &value,
                                                       const JSHandle<JSTaggedValue> &metaData);
    // For test
    static JSHandle<TransitionsDictionary> Remove(const JSThread *thread, const JSHandle<TransitionsDictionary> &table,
                                                  const JSHandle<JSTaggedValue> &key, const JSTaggedValue &metaData);
    void Rehash(const JSThread *thread, TransitionsDictionary *newTable);

    static bool CheckWeakExist(const JSTaggedValue &value)
    {
        if (value == JSTaggedValue::Undefined()) {
            return false;
        }
        return true;
    }

    static int ComputeCompactSize(const JSHandle<TransitionsDictionary> &table, int computeHashTableSize,
        int tableSize, int addedElements)
    {
        int realEntryCount = 0;
        for (int i = 0; i < tableSize; i++) {
            // value is weak reference, if not use will be set undefined.
            if (TransitionsDictionary::CheckWeakExist(table->GetValue(i))) {
                realEntryCount++;
            }
        }
        return std::min(computeHashTableSize,
            static_cast<int>(helpers::math::GetPowerOfTwoValue32(realEntryCount + addedElements) * HASH_TABLE_BUFFER));
    }

    static constexpr int ENTRY_SIZE = 3;
    static constexpr int ENTRY_KEY_INDEX = 0;
    static constexpr int ENTRY_VALUE_INDEX = 1;
    static constexpr int ENTRY_DETAILS_INDEX = 2;
    static constexpr int HASH_TABLE_BUFFER = 2;
    DECL_DUMP()
};
}  // namespace panda::ecmascript
#endif

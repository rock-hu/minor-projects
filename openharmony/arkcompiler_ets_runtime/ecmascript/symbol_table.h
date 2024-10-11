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

#ifndef ECMASCRIPT_SYMBOL_TABLE_H
#define ECMASCRIPT_SYMBOL_TABLE_H

#include "ecmascript/ecma_string.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/js_symbol.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tagged_hash_table.h"

namespace panda::ecmascript {
class SymbolTable : public TaggedHashTable<SymbolTable> {
public:
    using HashTable = TaggedHashTable<SymbolTable>;
    static SymbolTable *Cast(TaggedObject *object)
    {
        return reinterpret_cast<SymbolTable *>(object);
    }
    inline static int GetKeyIndex(int entry)
    {
        return HashTable::TABLE_HEADER_SIZE + entry * GetEntrySize() + ENTRY_KEY_INDEX;
    }
    inline static int GetValueIndex(int entry)
    {
        return HashTable::TABLE_HEADER_SIZE + entry * GetEntrySize() + ENTRY_VALUE_INDEX;
    }
    inline static int GetEntryIndex(int entry)
    {
        return HashTable::TABLE_HEADER_SIZE + entry * GetEntrySize();
    }
    inline static int GetEntrySize()
    {
        return ENTRY_SIZE;
    }
    static inline bool IsMatch(const JSTaggedValue &name, const JSTaggedValue &other)
    {
        if (name.IsHole() || name.IsUndefined()) {
            return false;
        }

        auto *nameString = static_cast<EcmaString *>(name.GetTaggedObject());
        auto *otherString = static_cast<EcmaString *>(other.GetTaggedObject());
        return EcmaStringAccessor::StringsAreEqual(nameString, otherString);
    }
    static inline uint32_t Hash(const JSTaggedValue &obj)
    {
        if (obj.IsHeapObject()) {
            if (obj.IsString()) {
                auto *nameString = static_cast<EcmaString *>(obj.GetTaggedObject());
                return EcmaStringAccessor(nameString).GetHashcode();
            }
            return JSSymbol::ComputeHash();
        }
        LOG_ECMA(FATAL) << "this branch is unreachable";
        UNREACHABLE();
    }

    static const int DEFAULT_ELEMENTS_NUMBER = 64;
    static JSHandle<SymbolTable> Create(JSThread *thread, int numberOfElements = DEFAULT_ELEMENTS_NUMBER)
    {
        return HashTable::Create(thread, numberOfElements);
    }

    inline bool ContainsKey(const JSTaggedValue &key)
    {
        int entry = FindEntry(key);
        return entry != -1;
    }

    inline JSTaggedValue GetSymbol(const JSTaggedValue &key)
    {
        int entry = FindEntry(key);
        ASSERT(entry != -1);
        return GetValue(entry);
    }

    inline JSTaggedValue FindSymbol(const JSTaggedValue &value)
    {
        JSSymbol *symbol = JSSymbol::Cast(value.GetTaggedObject());
        JSTaggedValue des = symbol->GetDescription();
        if (!des.IsUndefined()) {
            if (ContainsKey(des)) {
                return des;
            }
        }
        return JSTaggedValue::Undefined();
    }
    static int ComputeCompactSize([[maybe_unused]] const JSHandle<SymbolTable> &table, int computeHashTableSize,
        [[maybe_unused]] int tableSize, [[maybe_unused]] int addedElements)
    {
        return computeHashTableSize;
    }
    static constexpr int ENTRY_KEY_INDEX = 0;
    static constexpr int ENTRY_VALUE_INDEX = 1;
    static constexpr int ENTRY_SIZE = 2;
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_SYMBOL_TABLE_H
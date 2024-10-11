/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_DFX_HPROF_STRING_HASHMAP_H
#define ECMASCRIPT_DFX_HPROF_STRING_HASHMAP_H

#include "ecmascript/ecma_vm.h"
#include "ecmascript/mem/c_containers.h"
#include "ecmascript/mem/c_string.h"

namespace panda::ecmascript {
using StringKey = uint64_t;
using StringId = uint64_t;

// An Implementation for Native StringTable without Auto Mem-Management
// To make sure when using String, it still stays where it was.
class StringHashMap {
public:
    explicit StringHashMap(const EcmaVM *vm) : vm_(vm)
    {
        ASSERT(vm_ != nullptr);
    }
    ~StringHashMap()
    {
        Clear();
    }
    NO_MOVE_SEMANTIC(StringHashMap);
    NO_COPY_SEMANTIC(StringHashMap);
    /*
     * The ID is the seat number in JSON file Range from 0~string_table_.size()
     */
    StringId GetStringId(const CString *cstr) const;
    /*
     * Get all keys sorted by insert order
     */
    const CVector<StringKey> &GetOrderedKeyStorage() const
    {
        return orderedKey_;
    }
    /*
     * Get string by its hash key
     */
    CString *GetStringByKey(StringKey key) const;
    std::pair<uint64_t, CString *> GetStringAndIdPair(StringKey key) const;
    StringId InsertStrAndGetStringId(const CString &cstrArg);
    size_t GetCapcity() const
    {
        ASSERT(orderedKey_.size() == hashmap_.size());
        return orderedKey_.size();
    }
    /*
     * For external call to use this StringTable
     */
    CString *GetString(const CString &cstr);

private:
    StringKey GenerateStringKey(const CString *cstr) const;
    CString *FindOrInsertString(const CString *cstr);
    /*
     * Free all memory
     */
    void Clear();
    const EcmaVM *vm_;
    CVector<StringKey> orderedKey_;  // Used for Serialize Order
    size_t index_ {2};  // 2: Offset the String-Table Header
    CUnorderedMap<StringKey, StringId> indexMap_;
    CUnorderedMap<StringKey, CString *> hashmap_;
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_DFX_HPROF_STRING_HASHMAP_H

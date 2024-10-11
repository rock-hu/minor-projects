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

#ifndef ECMASCRIPT_MEM_FREE_OBJECT_SET_H
#define ECMASCRIPT_MEM_FREE_OBJECT_SET_H

#include <cstdint>

#include "libpandabase/macros.h"
#include "ecmascript/mem/jit_fort_memdesc.h"

namespace panda::ecmascript {
using SetType = int32_t;

class FreeObject;

template <typename T>
class FreeObjectList;

template <typename T>
class FreeObjectSet {
public:
    explicit FreeObjectSet(SetType type) : setType_(type)
    {
        Rebuild();
    }
    explicit FreeObjectSet(SetType type, MemDescPool *pool) : setType_(type), memDescPool_(pool)
    {
        Rebuild();
    }
    ~FreeObjectSet() = default;

    inline bool Empty() const
    {
        return available_ == 0;
    }

    inline size_t Available() const
    {
        return available_;
    }

    void Free(uintptr_t begin, size_t size);

    void Rebuild();

    T *LookupSmallFreeObject(size_t size);
    T *LookupLargeFreeObject(size_t size);
    T *ObtainSmallFreeObject(size_t size);
    T *ObtainLargeFreeObject(size_t size);

    NO_COPY_SEMANTIC(FreeObjectSet);
    NO_MOVE_SEMANTIC(FreeObjectSet);

    static constexpr SetType INVALID_SET_TYPE = -1;

private:
    FreeObjectSet *next_ = nullptr;
    FreeObjectSet *prev_ = nullptr;
    SetType setType_ = INVALID_SET_TYPE;
    size_t available_ = 0;
    uint64_t isAdded_ = 0; // 0: not added, 1: is added
    T *freeObject_ = nullptr;
    MemDescPool *memDescPool_ {nullptr};
    friend class FreeObjectList<T>;
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_MEM_FREE_OBJECT_SET_H

/**
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PANDA_PLUGINS_ETS_RUNTIME_MEM_ETS_REFERENCE_H
#define PANDA_PLUGINS_ETS_RUNTIME_MEM_ETS_REFERENCE_H

#include "libpandabase/macros.h"
#include "runtime/mem/refstorage/reference.h"
#include "runtime/mem/refstorage/reference_storage.h"
#include "plugins/ets/runtime/napi/ets_napi.h"
#include "plugins/ets/runtime/types/ets_class.h"
#include "plugins/ets/runtime/types/ets_object.h"

namespace ark::ets {
class EtsReference final {
public:
    DEFAULT_COPY_SEMANTIC(EtsReference);
    DEFAULT_MOVE_SEMANTIC(EtsReference);
    EtsReference() = default;
    ~EtsReference() = default;

    using EtsObjectType = mem::Reference::ObjectType;

    bool IsStack() const
    {
        return GetReference()->IsStack();
    }

    bool IsLocal() const
    {
        return GetReference()->IsLocal();
    }

    bool IsGlobal() const
    {
        return GetReference()->IsGlobal();
    }

    bool IsWeak() const
    {
        return GetReference()->IsWeak();
    }

    static mem::Reference *CastToReference(EtsReference *etsRef)
    {
        return reinterpret_cast<mem::Reference *>(etsRef);
    }

    static EtsReference *CastFromReference(mem::Reference *ref)
    {
        return reinterpret_cast<EtsReference *>(ref);
    }

private:
    const mem::Reference *GetReference() const
    {
        return reinterpret_cast<const mem::Reference *>(this);
    }
};

[[maybe_unused]] static inline EtsReference *EtsObjectToEtsRef(ets_object obj)
{
    return reinterpret_cast<EtsReference *>(obj);
}

[[maybe_unused]] static inline ets_object EtsRefToEtsObject(EtsReference *ref)
{
    return reinterpret_cast<ets_object>(ref);
}

[[maybe_unused]] static inline EtsReference *EtsNapiWeakToEtsRef(ets_object obj)
{
    auto ref = reinterpret_cast<EtsReference *>(obj);
    ASSERT(ref->IsWeak());
    return ref;
}

[[maybe_unused]] static inline ets_object EtsRefToEtsNapiWeak(EtsReference *ref)
{
    ASSERT(ref->IsWeak());
    return reinterpret_cast<ets_object>(ref);
}

class EtsReferenceStorage final : private mem::ReferenceStorage {
public:
    EtsReferenceStorage(mem::GlobalObjectStorage *globalStorage, mem::InternalAllocatorPtr allocator,
                        bool refCheckValidate)
        : mem::ReferenceStorage(globalStorage, allocator, refCheckValidate)
    {
    }
    ~EtsReferenceStorage() = default;

    NO_COPY_SEMANTIC(EtsReferenceStorage);
    NO_MOVE_SEMANTIC(EtsReferenceStorage);

    static EtsReference *NewEtsStackRef(EtsObject **obj)
    {
        mem::Reference *ref = mem::ReferenceStorage::NewStackRef(reinterpret_cast<ObjectHeader **>(obj));
        return EtsReference::CastFromReference(ref);
    }

    EtsReference *NewEtsRef(EtsObject *obj, EtsReference::EtsObjectType objType)
    {
        mem::Reference *ref = NewRef(obj->GetCoreType(), objType);
        return EtsReference::CastFromReference(ref);
    }

    void RemoveEtsRef(EtsReference *etsRef)
    {
        RemoveRef(EtsReference::CastToReference(etsRef));
    }

    [[nodiscard]] EtsObject *GetEtsObject(EtsReference *etsRef)
    {
        return EtsObject::FromCoreType(GetObject(EtsReference::CastToReference(etsRef)));
    }

    [[nodiscard]] bool IsValidEtsRef(EtsReference *etsRef)
    {
        return IsValidRef(EtsReference::CastToReference(etsRef));
    }

    bool PushLocalEtsFrame(uint32_t capacity)
    {
        return PushLocalFrame(capacity);
    }

    EtsReference *PopLocalEtsFrame(EtsReference *result)
    {
        return EtsReference::CastFromReference(PopLocalFrame(EtsReference::CastToReference(result)));
    }

    mem::ReferenceStorage *GetAsReferenceStorage()
    {
        return this;
    }
};
}  // namespace ark::ets

#endif  // PANDA_PLUGINS_ETS_RUNTIME_MEM_ETS_REFERENCE_H

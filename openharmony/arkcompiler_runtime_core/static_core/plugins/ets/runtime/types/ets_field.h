/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_FFI_CLASSES_ETS_FIELD_H_
#define PANDA_PLUGINS_ETS_RUNTIME_FFI_CLASSES_ETS_FIELD_H_

#include "libpandafile/file.h"
#include "runtime/include/field.h"

namespace ark::ets {

class EtsClass;
class EtsString;

class EtsField {
public:
    EtsClass *GetDeclaringClass() const;

    EtsClass *GetType() const;

    uint32_t GetAccessFlags() const
    {
        return GetCoreType()->GetAccessFlags();
    }

    uint32_t GetFieldId() const
    {
        return GetCoreType()->GetFileId().GetOffset();
    }

    size_t GetOffset() const
    {
        return GetCoreType()->GetOffset();
    }

    Field *GetRuntimeField()
    {
        return GetCoreType();
    }

    const char *GetName() const
    {
        return reinterpret_cast<const char *>(GetCoreType()->GetName().data);
    }

    PANDA_PUBLIC_API EtsString *GetNameString() const;

    bool IsPublic() const
    {
        return GetCoreType()->IsPublic();
    }

    bool IsPrivate() const
    {
        return GetCoreType()->IsPrivate();
    }

    bool IsProtected() const
    {
        return GetCoreType()->IsProtected();
    }

    bool IsReadonly() const
    {
        // NOTE(shumilov-petr): Need to dump extra info in frontend
        return false;
    }

    bool IsStatic() const
    {
        return GetCoreType()->IsStatic();
    }

    bool IsDeclaredIn(const EtsClass *klass) const;

    static EtsField *FromRuntimeField(Field *field)
    {
        return reinterpret_cast<EtsField *>(field);
    }

    static const EtsField *FromRuntimeField(const Field *field)
    {
        return reinterpret_cast<const EtsField *>(field);
    }

    Field *GetCoreType()
    {
        return reinterpret_cast<Field *>(this);
    }

    const Field *GetCoreType() const
    {
        return reinterpret_cast<const Field *>(this);
    }

    PANDA_PUBLIC_API const char *GetTypeDescriptor() const;

    EtsField() = delete;
    ~EtsField() = delete;
    NO_COPY_SEMANTIC(EtsField);
    NO_MOVE_SEMANTIC(EtsField);
};

}  // namespace ark::ets

#endif  // PANDA_PLUGINS_ETS_RUNTIME_FFI_CLASSES_ETS_FIELD_H_

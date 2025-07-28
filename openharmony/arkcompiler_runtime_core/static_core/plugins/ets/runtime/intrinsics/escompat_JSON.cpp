/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "intrinsics.h"
#include "plugins/ets/runtime/ets_utils.h"
#include "helpers/json_helper.h"

namespace ark::ets::intrinsics {

static EtsField *GetInstanceFieldByName(EtsClass *cls, EtsString *name)
{
    auto fieldName = name->GetMutf8();
    auto instanceField = ManglingUtils::GetFieldIDByDisplayName(cls, fieldName);
    return instanceField;
}

EtsBoolean EscompatJSONGetJSONStringifyIgnoreByIdx(EtsClass *cls, EtsLong idx)
{
    EtsField *field = cls->GetFieldByIndex(idx);
    bool ret = false;
    if (field == nullptr) {
        return static_cast<EtsBoolean>(ret);
    }
    auto *runtimeClass = field->GetDeclaringClass()->GetRuntimeClass();
    const panda_file::File &pf = *runtimeClass->GetPandaFile();
    panda_file::FieldDataAccessor fda(pf, field->GetRuntimeField()->GetFileId());
    fda.EnumerateAnnotations([&pf, &ret](panda_file::File::EntityId annId) {
        panda_file::AnnotationDataAccessor ada(pf, annId);
        const char *className = utf::Mutf8AsCString(pf.GetStringData(ada.GetClassId()).data);
        if (className == panda_file_items::class_descriptors::JSON_STRINGIFY_IGNORE) {
            ret = true;
        }
    });
    return static_cast<EtsBoolean>(ret);
}

EtsBoolean EscompatJSONGetJSONStringifyIgnoreByName(EtsClass *cls, EtsString *name)
{
    EtsField *field = GetInstanceFieldByName(cls, name);
    bool ret = false;
    if (field == nullptr) {
        return static_cast<EtsBoolean>(ret);
    }
    auto *runtimeClass = field->GetDeclaringClass()->GetRuntimeClass();
    const panda_file::File &pf = *runtimeClass->GetPandaFile();
    panda_file::FieldDataAccessor fda(pf, field->GetRuntimeField()->GetFileId());
    fda.EnumerateAnnotations([&pf, &ret](panda_file::File::EntityId annId) {
        panda_file::AnnotationDataAccessor ada(pf, annId);
        const char *className = utf::Mutf8AsCString(pf.GetStringData(ada.GetClassId()).data);
        if (className == panda_file_items::class_descriptors::JSON_STRINGIFY_IGNORE) {
            ret = true;
        }
    });
    return static_cast<EtsBoolean>(ret);
}

EtsBoolean EscompatJSONGetJSONParseIgnoreFromAnnotation(EtsClass *cls, EtsLong idx)
{
    EtsField *field = cls->GetFieldByIndex(idx);
    bool ret = false;
    if (field == nullptr) {
        return static_cast<EtsBoolean>(ret);
    }
    auto *runtimeClass = field->GetDeclaringClass()->GetRuntimeClass();
    const panda_file::File &pf = *runtimeClass->GetPandaFile();
    panda_file::FieldDataAccessor fda(pf, field->GetRuntimeField()->GetFileId());
    fda.EnumerateAnnotations([&pf, &ret](panda_file::File::EntityId annId) {
        panda_file::AnnotationDataAccessor ada(pf, annId);
        const char *className = utf::Mutf8AsCString(pf.GetStringData(ada.GetClassId()).data);
        if (className == panda_file_items::class_descriptors::JSON_PARSE_IGNORE) {
            ret = true;
        }
    });
    return static_cast<EtsBoolean>(ret);
}

EtsString *EscompatJSONGetJSONRenameByIdx(EtsClass *cls, EtsLong idx)
{
    auto *thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);

    EtsField *field = cls->GetFieldByIndex(idx);
    VMHandle<EtsString> retStrHandle;
    if (field == nullptr) {
        return VMHandle<EtsString>(thread, EtsString::CreateNewEmptyString()->GetCoreType()).GetPtr();
    }
    auto *runtimeClass = field->GetDeclaringClass()->GetRuntimeClass();
    const panda_file::File &pf = *runtimeClass->GetPandaFile();
    panda_file::FieldDataAccessor fda(pf, field->GetRuntimeField()->GetFileId());
    fda.EnumerateAnnotations([&pf, &retStrHandle, &thread](panda_file::File::EntityId annId) {
        panda_file::AnnotationDataAccessor ada(pf, annId);
        const char *className = utf::Mutf8AsCString(pf.GetStringData(ada.GetClassId()).data);
        if (className == panda_file_items::class_descriptors::JSON_RENAME) {
            const auto value = ada.GetElement(0).GetScalarValue();
            const auto id = value.Get<panda_file::File::EntityId>();
            auto stringData = pf.GetStringData(id);
            retStrHandle = VMHandle<EtsString>(
                thread, EtsString::CreateFromMUtf8(reinterpret_cast<const char *>(stringData.data))->GetCoreType());
        }
    });
    return retStrHandle.GetPtr();
}

EtsString *EscompatJSONGetJSONRenameByName(EtsClass *cls, EtsString *name)
{
    auto *thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);

    EtsField *field = GetInstanceFieldByName(cls, name);
    VMHandle<EtsString> retStrHandle;
    if (field == nullptr) {
        return VMHandle<EtsString>(thread, EtsString::CreateNewEmptyString()->GetCoreType()).GetPtr();
    }
    auto *runtimeClass = field->GetDeclaringClass()->GetRuntimeClass();
    const panda_file::File &pf = *runtimeClass->GetPandaFile();
    panda_file::FieldDataAccessor fda(pf, field->GetRuntimeField()->GetFileId());
    fda.EnumerateAnnotations([&pf, &retStrHandle, &thread](panda_file::File::EntityId annId) {
        panda_file::AnnotationDataAccessor ada(pf, annId);
        const char *className = utf::Mutf8AsCString(pf.GetStringData(ada.GetClassId()).data);
        if (className == panda_file_items::class_descriptors::JSON_RENAME) {
            const auto value = ada.GetElement(0).GetScalarValue();
            const auto id = value.Get<panda_file::File::EntityId>();
            auto stringData = pf.GetStringData(id);
            retStrHandle = VMHandle<EtsString>(
                thread, EtsString::CreateFromMUtf8(reinterpret_cast<const char *>(stringData.data))->GetCoreType());
        }
    });
    return retStrHandle.GetPtr();
}

extern "C" EtsString *EscompatJSONStringifyFast(EtsObject *value)
{
    auto coro = EtsCoroutine::GetCurrent();
    ASSERT(coro->HasPendingException() == false);

    EtsHandleScope scope(coro);
    EtsHandle<EtsObject> valueHandle(coro, value);

    helpers::JSONStringifier stringifier;
    return stringifier.Stringify(valueHandle);
}

}  // namespace ark::ets::intrinsics

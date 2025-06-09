/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_TYPES_ETS_TYPEAPI_TYPE_H
#define PANDA_PLUGINS_ETS_RUNTIME_TYPES_ETS_TYPEAPI_TYPE_H

#include "mem/object_pointer.h"
#include "plugins/ets/runtime/types/ets_object.h"
#include "plugins/ets/runtime/types/ets_runtime_linker.h"
#include "plugins/ets/runtime/types/ets_string.h"

namespace ark::ets {

namespace test {
class EtsTypeAPITest;
}  // namespace test

class EtsCoroutine;

class EtsTypeAPIType : public EtsObject {
public:
    EtsTypeAPIType() = delete;
    ~EtsTypeAPIType() = delete;

    NO_COPY_SEMANTIC(EtsTypeAPIType);
    NO_MOVE_SEMANTIC(EtsTypeAPIType);

    EtsObject *AsObject()
    {
        return this;
    }

    const EtsObject *AsObject() const
    {
        return this;
    }

    static EtsTypeAPIType *FromEtsObject(EtsObject *type)
    {
        return reinterpret_cast<EtsTypeAPIType *>(type);
    }

    static EtsTypeAPIType *FromCoreType(ObjectHeader *objectHeader)
    {
        return reinterpret_cast<EtsTypeAPIType *>(objectHeader);
    }

    EtsRuntimeLinker *GetContextLinker()
    {
        return contextLinker_;
    }

    EtsString *GetRuntimeTypeDescriptor()
    {
        return td_;
    }

private:
    ObjectPointer<EtsString> td_;
    ObjectPointer<EtsRuntimeLinker> contextLinker_;

    friend class test::EtsTypeAPITest;
};

}  // namespace ark::ets

#endif  // PANDA_PLUGINS_ETS_RUNTIME_TYPES_ETS_TYPEAPI_TYPE_H

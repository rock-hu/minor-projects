/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_ETS_CLASS_LINKER_H_
#define PANDA_PLUGINS_ETS_RUNTIME_ETS_CLASS_LINKER_H_

#include "plugins/ets/runtime/ets_class_root.h"
#include "runtime/include/mem/panda_smart_pointers.h"
#include "runtime/include/mem/panda_string.h"
#include "runtime/include/mem/panda_containers.h"

namespace ark {
class Method;
class ClassLinker;
class ClassLinkerContext;
class ClassLinkerErrorHandler;
}  // namespace ark

namespace ark::ets {

class EtsClass;
class EtsClassLinkerExtension;
class EtsCoroutine;

class PANDA_PUBLIC_API EtsClassLinker {
public:
    static Expected<PandaUniquePtr<EtsClassLinker>, PandaString> Create(ClassLinker *classLinker);
    ~EtsClassLinker() = default;

    bool Initialize();

    bool InitializeClass(EtsCoroutine *coroutine, EtsClass *klass);
    EtsClass *GetClassRoot(EtsClassRoot root) const;
    EtsClass *GetClass(const char *name, bool needCopyDescriptor = false,
                       ClassLinkerContext *classLinkerContext = nullptr,
                       ClassLinkerErrorHandler *errorHandler = nullptr);
    EtsClass *GetClass(const panda_file::File &pf, panda_file::File::EntityId id,
                       ClassLinkerContext *classLinkerContext = nullptr,
                       ClassLinkerErrorHandler *errorHandler = nullptr);
    Method *GetMethod(const panda_file::File &pf, panda_file::File::EntityId id);
    Method *GetAsyncImplMethod(Method *method, EtsCoroutine *coroutine);
    EtsClass *GetPromiseClass();
    EtsClass *GetPromiseRefClass();
    EtsClass *GetArrayClass();
    EtsClass *GetWaitersListClass();
    EtsClass *GetMutexClass();
    EtsClass *GetEventClass();
    EtsClass *GetCondVarClass();
    EtsClass *GetArrayBufferClass();
    EtsClass *GetStringBuilderClass();
    EtsClass *GetSharedMemoryClass();
    EtsClass *GetObjectClass();
    EtsClass *GetVoidClass();
    EtsClass *GetTypeAPIFieldClass();
    EtsClass *GetTypeAPIMethodClass();
    EtsClass *GetTypeAPIParameterClass();
    EtsClass *GetIFunctionClass();
    EtsClass *GetFinalizableWeakRefClass();
    // NOTE(molotkovmikhail): Need to implement a method cache that should be accessible by some class and method ids.
    Method *GetSubscribeOnAnotherPromiseMethod();

    EtsClassLinkerExtension *GetEtsClassLinkerExtension()
    {
        return ext_;
    }

    NO_COPY_SEMANTIC(EtsClassLinker);
    NO_MOVE_SEMANTIC(EtsClassLinker);

private:
    explicit EtsClassLinker(ClassLinker *classLinker);

    ClassLinker *classLinker_ {};
    EtsClassLinkerExtension *ext_ {};

    friend class mem::Allocator;
};

}  // namespace ark::ets

#endif  // !PANDA_PLUGINS_ETS_RUNTIME_ETS_CLASS_LINKER_H_

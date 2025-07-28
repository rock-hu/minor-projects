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

#include "file.h"
#include "include/object_header.h"
#include "include/thread_scopes.h"
#include "intrinsics.h"
#include "libpandabase/utils/logger.h"
#include "libpandabase/utils/utf.h"
#include "libziparchive/extractortool/extractor.h"
#include "runtime/handle_scope-inl.h"
#include "plugins/ets/runtime/ets_class_linker_extension.h"
#include "plugins/ets/runtime/ets_coroutine.h"
#include "plugins/ets/runtime/ets_exceptions.h"
#include "plugins/ets/runtime/ets_stubs-inl.h"
#include "plugins/ets/runtime/types/ets_abc_file.h"
#include "plugins/ets/runtime/types/ets_primitives.h"
#include "plugins/ets/runtime/types/ets_runtime_linker.h"
#include "plugins/ets/runtime/types/ets_string.h"
#include "runtime/mem/local_object_handle.h"

namespace ark::ets::intrinsics {

static EtsAbcFile *CreateAbcFile(EtsCoroutine *coro, ClassLinkerContext *ctx,
                                 std::unique_ptr<const panda_file::File> &&pf)
{
    auto *abcFile = EtsAbcFile::FromEtsObject(EtsObject::Create(coro, PlatformTypes(coro)->coreAbcFile));
    abcFile->SetPandaFile(pf.get());

    Runtime::GetCurrent()->GetClassLinker()->AddPandaFile(std::move(pf), ctx);
    return abcFile;
}

EtsAbcFile *EtsAbcFileLoadAbcFile(EtsRuntimeLinker *runtimeLinker, EtsString *filePath)
{
    ASSERT(filePath != nullptr);
    ASSERT(runtimeLinker != nullptr);
    auto *ctx = runtimeLinker->GetClassLinkerContext();
    auto *coro = EtsCoroutine::GetCurrent();

    const auto path = filePath->GetMutf8();
    std::unique_ptr<const panda_file::File> pf {nullptr};
    {
        // Loading panda-file might be time-consuming, which would affect GC
        // unless being executed in native scope
        ScopedNativeCodeThread etsNativeScope(coro);
        pf = panda_file::OpenPandaFileOrZip(path);
    }

    if (pf == nullptr) {
        // get hap path
        auto pathStr = std::string(path.begin(), path.end());
        size_t pos = pathStr.rfind("/ets/");
        if (pos == std::string::npos) {
            ets::ThrowEtsException(coro, panda_file_items::class_descriptors::ABC_FILE_NOT_FOUND_ERROR,
                                   PandaString("Open failed, file: ") + path);
            return nullptr;
        }
        std::string hapPath = pathStr.substr(0, pos);
        hapPath += ".hap";

        std::shared_ptr<ark::extractor::Extractor> extractor = std::make_shared<ark::extractor::Extractor>(hapPath);
        if (!extractor || !extractor->Init()) {
            ets::ThrowEtsException(coro, panda_file_items::class_descriptors::ABC_FILE_NOT_FOUND_ERROR,
                                   PandaString("Open failed, file: ") + path);
            return nullptr;
        }
        auto safeData = extractor->GetSafeData(pathStr);
        ASSERT(safeData != nullptr);
        pf = panda_file::OpenPandaFileFromSecureMemory(safeData->GetDataPtr(), safeData->GetDataLen(), pathStr);
    }

    if (pf == nullptr) {
        ets::ThrowEtsException(coro, panda_file_items::class_descriptors::ABC_FILE_NOT_FOUND_ERROR,
                               PandaString("Abc file not found: ") + path);
        return nullptr;
    }
    return CreateAbcFile(coro, ctx, std::move(pf));
}

EtsAbcFile *EtsAbcFileLoadFromMemory(EtsRuntimeLinker *runtimeLinker, ObjectHeader *rawFileArray)
{
    ASSERT(rawFileArray != nullptr);
    ASSERT(runtimeLinker != nullptr);

    auto *ctx = runtimeLinker->GetClassLinkerContext();
    auto *coro = EtsCoroutine::GetCurrent();
    [[maybe_unused]] EtsHandleScope hs(coro);
    EtsHandle arrayHandle(coro, reinterpret_cast<EtsByteArray *>(rawFileArray));

    std::unique_ptr<const panda_file::File> pf {nullptr};
    {
        // Loading panda-file might be time-consuming, which would affect GC
        // unless being executed in native scope
        ScopedNativeCodeThread etsNativeScope(coro);
        pf = panda_file::OpenPandaFileFromMemory(arrayHandle->GetData<void>(), arrayHandle->GetLength());
    }

    if (pf == nullptr) {
        ets::ThrowEtsException(coro, panda_file_items::class_descriptors::ERROR,
                               PandaString("Failed to load abc file from memory"));
        return nullptr;
    }
    return CreateAbcFile(coro, ctx, std::move(pf));
}

EtsClass *EtsAbcFileLoadClass(EtsAbcFile *abcFile, EtsRuntimeLinker *runtimeLinker, EtsString *clsName, EtsBoolean init)
{
    const auto name = clsName->GetMutf8();
    PandaString descriptor;
    const auto *classDescriptor = ClassHelper::GetDescriptor(utf::CStringAsMutf8(name.c_str()), &descriptor);

    const auto *pf = abcFile->GetPandaFile();
    const auto classId = pf->GetClassId(classDescriptor);
    if (!classId.IsValid() || pf->IsExternal(classId)) {
        return nullptr;
    }

    auto *coro = EtsCoroutine::GetCurrent();
    auto *classLinker = Runtime::GetCurrent()->GetClassLinker();
    auto *ctx = runtimeLinker->GetClassLinkerContext();
    auto *linkerErrorHandler = PandaEtsVM::GetCurrent()->GetEtsClassLinkerExtension()->GetErrorHandler();
    auto *klass = classLinker->LoadClass(*pf, classId, ctx, linkerErrorHandler, true);
    if (UNLIKELY(klass == nullptr)) {
        ASSERT(coro->HasPendingException());
        return nullptr;
    }

    if (UNLIKELY(init != 0 && !klass->IsInitialized())) {
        if (UNLIKELY(!classLinker->InitializeClass(coro, klass))) {
            ASSERT(coro->HasPendingException());
            return nullptr;
        }
    }
    return EtsClass::FromRuntimeClass(klass);
}

EtsString *EtsAbcFileGetFilename(EtsAbcFile *abcFile)
{
    auto filename = abcFile->GetPandaFile()->GetFilename();
    return EtsString::CreateFromMUtf8(filename.c_str());
}

}  // namespace ark::ets::intrinsics

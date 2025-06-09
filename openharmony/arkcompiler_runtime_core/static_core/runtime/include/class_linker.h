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
#ifndef PANDA_RUNTIME_CLASS_LINKER_H_
#define PANDA_RUNTIME_CLASS_LINKER_H_

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "compiler/aot/aot_manager.h"
#include "libpandabase/mem/arena_allocator.h"
#include "libpandabase/os/mutex.h"
#include "libpandabase/utils/utf.h"
#include "libpandafile/class_data_accessor-inl.h"
#include "libpandafile/file.h"
#include "libpandafile/file_items.h"
#include "runtime/class_linker_context.h"
#include "runtime/include/class.h"
#include "runtime/include/field.h"
#include "runtime/include/itable_builder.h"
#include "runtime/include/imtable_builder.h"
#include "runtime/include/language_context.h"
#include "runtime/include/mem/panda_containers.h"
#include "runtime/include/mem/panda_smart_pointers.h"
#include "runtime/include/mem/panda_string.h"
#include "runtime/include/method.h"
#include "runtime/include/vtable_builder_interface.h"

namespace ark {

using compiler::AotManager;

class ClassLinkerErrorHandler;

class ClassLinker {
public:
    enum class Error {
        CLASS_NOT_FOUND,
        FIELD_NOT_FOUND,
        METHOD_NOT_FOUND,
        NO_CLASS_DEF,
        CLASS_CIRCULARITY,
        OVERRIDES_FINAL,
        MULTIPLE_OVERRIDE,
        MULTIPLE_IMPLEMENT,
    };

    ClassLinker(mem::InternalAllocatorPtr allocator, std::vector<std::unique_ptr<ClassLinkerExtension>> &&extensions);

    ~ClassLinker();

    bool Initialize(bool compressedStringEnabled = true);

    bool InitializeRoots(ManagedThread *thread);

    PANDA_PUBLIC_API Class *GetClass(const uint8_t *descriptor, bool needCopyDescriptor, ClassLinkerContext *context,
                                     ClassLinkerErrorHandler *errorHandler = nullptr);

    PANDA_PUBLIC_API Class *GetClass(const panda_file::File &pf, panda_file::File::EntityId id,
                                     ClassLinkerContext *context, ClassLinkerErrorHandler *errorHandler = nullptr);

    PANDA_PUBLIC_API Class *GetClass(const Method &caller, panda_file::File::EntityId id,
                                     ClassLinkerErrorHandler *errorHandler = nullptr);

    inline Class *GetLoadedClass(const panda_file::File &pf, panda_file::File::EntityId id,
                                 ClassLinkerContext *context);

    Class *LoadClass(const panda_file::File &pf, panda_file::File::EntityId classId, ClassLinkerContext *context,
                     ClassLinkerErrorHandler *errorHandler = nullptr, bool addToRuntime = true)
    {
        return LoadClass(&pf, classId, pf.GetStringData(classId).data, context, errorHandler, addToRuntime);
    }

    Method *GetMethod(const panda_file::File &pf, panda_file::File::EntityId id, ClassLinkerContext *context = nullptr,
                      ClassLinkerErrorHandler *errorHandler = nullptr);

    Method *GetMethod(const Method &caller, panda_file::File::EntityId id,
                      ClassLinkerErrorHandler *errorHandler = nullptr);

    Field *GetField(const panda_file::File &pf, panda_file::File::EntityId id, bool isStatic,
                    ClassLinkerContext *context = nullptr, ClassLinkerErrorHandler *errorHandler = nullptr);

    Field *GetField(const Method &caller, panda_file::File::EntityId id, bool isStatic,
                    ClassLinkerErrorHandler *errorHandler = nullptr);

    PANDA_PUBLIC_API void AddPandaFile(std::unique_ptr<const panda_file::File> &&pf,
                                       ClassLinkerContext *context = nullptr);

    template <typename Callback>
    void EnumeratePandaFiles(Callback cb, bool skipIntrinsics = true) const
    {
        os::memory::LockHolder lock(pandaFilesLock_);
        for (const auto &fileData : pandaFiles_) {
            if (skipIntrinsics && fileData.pf->GetFilename().empty()) {
                continue;
            }

            if (!cb(*fileData.pf)) {
                break;
            }
        }
    }

    template <typename Callback>
    void EnumerateBootPandaFiles(Callback cb) const
    {
        os::memory::LockHolder lock {bootPandaFilesLock_};
        for (const auto &file : bootPandaFiles_) {
            if (!cb(*file)) {
                break;
            }
        }
    }

    const PandaVector<const panda_file::File *> &GetBootPandaFiles() const
    {
        return bootPandaFiles_;
    }

    AotManager *GetAotManager()
    {
        return aotManager_.get();
    }

    PandaString GetClassContextForAot(bool useAbsPath = true)
    {
        PandaString aotCtx;
        EnumeratePandaFiles(compiler::AotClassContextCollector(&aotCtx, useAbsPath));
        return aotCtx;
    }

    template <class Callback>
    void EnumerateClasses(const Callback &cb, mem::VisitGCRootFlags flags = mem::VisitGCRootFlags::ACCESS_ROOT_ALL)
    {
        for (auto &ext : extensions_) {
            if (ext == nullptr) {
                continue;
            }

            if (!ext->EnumerateClasses(cb, flags)) {
                return;
            }
        }
    }

    template <class Callback>
    void EnumerateContexts(const Callback &cb)
    {
        for (auto &ext : extensions_) {
            if (ext == nullptr) {
                continue;
            }
            ext->EnumerateContexts(cb);
        }
    }

    template <class Callback>
    void EnumerateContextsForDump(const Callback &cb, std::ostream &os)
    {
        auto findParent = [](ClassLinkerContext *parent, ClassLinkerContext *ctxPtr, size_t &parentIndex,
                             bool &founded) {
            if (parent == ctxPtr) {
                founded = true;
                return false;
            }
            parentIndex++;
            return true;
        };

        size_t registerIndex = 0;
        ClassLinkerContext *parent = nullptr;
        ClassLinkerExtension *ext = nullptr;

        auto enumCallback = [&registerIndex, &parent, &cb, &os, &ext, &findParent](ClassLinkerContext *ctx) {
            os << "#" << registerIndex << " ";
            if (!cb(ctx, os, parent)) {
                return true;
            }
            if (parent != nullptr) {
                size_t parentIndex = 0;
                bool founded = false;
                ext->EnumerateContexts([parent, &parentIndex, &founded, &findParent](ClassLinkerContext *ctxPtr) {
                    return findParent(parent, ctxPtr, parentIndex, founded);
                });
                if (founded) {
                    os << "|Parent class loader: #" << parentIndex << "\n";
                } else {
                    os << "|Parent class loader: unknown\n";
                }
            } else {
                os << "|Parent class loader: empty\n";
            }
            registerIndex++;
            return true;
        };
        for (auto &extPtr : extensions_) {
            if (extPtr == nullptr) {
                continue;
            }
            ext = extPtr.get();
            ext->EnumerateContexts(enumCallback);
        }
    }

    PANDA_PUBLIC_API bool InitializeClass(ManagedThread *thread, Class *klass);

    bool HasExtension(const LanguageContext &ctx)
    {
        return extensions_[ark::panda_file::GetLangArrIndex(ctx.GetLanguage())].get() != nullptr;
    }

    bool HasExtension(panda_file::SourceLang lang)
    {
        return extensions_[ark::panda_file::GetLangArrIndex(lang)].get() != nullptr;
    }

    void ResetExtension(panda_file::SourceLang lang);

    ClassLinkerExtension *GetExtension(const LanguageContext &ctx)
    {
        ClassLinkerExtension *extension = extensions_[ark::panda_file::GetLangArrIndex(ctx.GetLanguage())].get();
        ASSERT(extension != nullptr);
        return extension;
    };

    ClassLinkerExtension *GetExtension(panda_file::SourceLang lang)
    {
        ClassLinkerExtension *extension = extensions_[ark::panda_file::GetLangArrIndex(lang)].get();
        ASSERT(extension != nullptr);
        return extension;
    };

    Class *ObjectToClass(const ObjectHeader *object)
    {
        ASSERT(object->ClassAddr<Class>()->IsClassClass());
        return extensions_[ark::panda_file::GetLangArrIndex(object->ClassAddr<BaseClass>()->GetSourceLang())]
            ->FromClassObject(const_cast<ObjectHeader *>(object));
    }

    size_t GetClassObjectSize(Class *cls, panda_file::SourceLang lang)
    {
        // We can't get SourceLang from cls, because it may not be initialized yet (see #12894)
        return extensions_[ark::panda_file::GetLangArrIndex(lang)]->GetClassObjectSizeFromClassSize(
            cls->GetClassSize());
    }

    void AddClassRoot(ClassRoot root, Class *klass);

    Class *CreateArrayClass(ClassLinkerExtension *ext, const uint8_t *descriptor, bool needCopyDescriptor,
                            Class *componentClass);

    void FreeClassData(Class *classPtr);

    void FreeClass(Class *classPtr);

    mem::InternalAllocatorPtr GetAllocator() const
    {
        return allocator_;
    }

    bool IsInitialized() const
    {
        return isInitialized_;
    }

    Class *FindLoadedClass(const uint8_t *descriptor, ClassLinkerContext *context = nullptr);

    size_t NumLoadedClasses();

    void VisitLoadedClasses(size_t flag);

    PANDA_PUBLIC_API Class *BuildClass(const uint8_t *descriptor, bool needCopyDescriptor, uint32_t accessFlags,
                                       Span<Method> methods, Span<Field> fields, Class *baseClass,
                                       Span<Class *> interfaces, ClassLinkerContext *context, bool isInterface);

    bool IsPandaFileRegistered(const panda_file::File *file)
    {
        os::memory::LockHolder lock(pandaFilesLock_);
        for (const auto &data : pandaFiles_) {
            if (data.pf.get() == file) {
                return true;
            }
        }

        return false;
    }

    ClassLinkerContext *GetAppContext(std::string_view pandaFile)
    {
        ClassLinkerContext *appContext = nullptr;
        EnumerateContexts([pandaFile, &appContext](ClassLinkerContext *context) -> bool {
            auto filePaths = context->GetPandaFilePaths();
            for (auto &file : filePaths) {
                if (file == pandaFile) {
                    appContext = context;
                    return false;
                }
            }
            return true;
        });
        return appContext;
    }

    void RemoveCreatedClassInExtension(Class *klass);

    Class *LoadClass(const panda_file::File *pf, const uint8_t *descriptor, panda_file::SourceLang lang);

private:
    struct ClassInfo {
        PandaUniquePtr<VTableBuilder> vtableBuilder;
        PandaUniquePtr<ITableBuilder> itableBuilder;
        PandaUniquePtr<IMTableBuilder> imtableBuilder;
        size_t size;
        size_t numSfields;
    };

    bool LinkEntitiesAndInitClass(Class *klass, ClassInfo *classInfo, ClassLinkerExtension *ext,
                                  const uint8_t *descriptor);

    Field *GetFieldById(Class *klass, const panda_file::FieldDataAccessor &fieldDataAccessor,
                        ClassLinkerErrorHandler *errorHandler, bool isStatic);

    Field *GetFieldBySignature(Class *klass, const panda_file::FieldDataAccessor &fieldDataAccessor,
                               ClassLinkerErrorHandler *errorHandler, bool isStatic);

    Method *GetMethod(const Class *klass, const panda_file::MethodDataAccessor &methodDataAccessor,
                      ClassLinkerErrorHandler *errorHandler);

    bool LinkBootClass(Class *klass);

    Class *LoadArrayClass(const uint8_t *descriptor, bool needCopyDescriptor, ClassLinkerContext *context,
                          ClassLinkerErrorHandler *errorHandler);

    Class *LoadClass(const panda_file::File *pf, panda_file::File::EntityId classId, const uint8_t *descriptor,
                     ClassLinkerContext *context, ClassLinkerErrorHandler *errorHandler, bool addToRuntime = true);

    Class *LoadClass(panda_file::ClassDataAccessor *classDataAccessor, const uint8_t *descriptor, Class *baseClass,
                     Span<Class *> interfaces, ClassLinkerContext *context, ClassLinkerExtension *ext,
                     ClassLinkerErrorHandler *errorHandler);

    Class *LoadBaseClass(panda_file::ClassDataAccessor *cda, const LanguageContext &ctx, ClassLinkerContext *context,
                         ClassLinkerErrorHandler *errorHandler);

    std::optional<Span<Class *>> LoadInterfaces(panda_file::ClassDataAccessor *cda, ClassLinkerContext *context,
                                                ClassLinkerErrorHandler *errorHandler);

    void FreeITableAndInterfaces(ITable itable, Span<Class *> &interfaces);

    [[nodiscard]] bool LinkFields(Class *klass, ClassLinkerErrorHandler *errorHandler);

    [[nodiscard]] bool LoadFields(Class *klass, panda_file::ClassDataAccessor *dataAccessor,
                                  ClassLinkerErrorHandler *errorHandler);

    [[nodiscard]] bool LinkMethods(Class *klass, ClassInfo *classInfo, ClassLinkerErrorHandler *errorHandler);

    [[nodiscard]] bool LoadMethods(Class *klass, ClassInfo *classInfo, panda_file::ClassDataAccessor *dataAccessor,
                                   ClassLinkerErrorHandler *errorHandler);

    [[nodiscard]] bool SetupClassInfo(ClassInfo &info, panda_file::ClassDataAccessor *dataAccessor, Class *base,
                                      Span<Class *> interfaces, ClassLinkerContext *context,
                                      ClassLinkerErrorHandler *errorHandler);

    [[nodiscard]] bool SetupClassInfo(ClassInfo &info, Span<Method> methods, Span<Field> fields, Class *base,
                                      Span<Class *> interfaces, bool isInterface,
                                      ClassLinkerErrorHandler *errorHandler);

    static bool LayoutFields(Class *klass, Span<Field> fields, bool isStatic, ClassLinkerErrorHandler *errorHandler);

    mem::InternalAllocatorPtr allocator_;

    PandaVector<const panda_file::File *> bootPandaFiles_ GUARDED_BY(bootPandaFilesLock_);

    struct PandaFileLoadData {
        ClassLinkerContext *context;
        std::unique_ptr<const panda_file::File> pf;
    };

    mutable os::memory::Mutex pandaFilesLock_;
    mutable os::memory::Mutex bootPandaFilesLock_;
    PandaVector<PandaFileLoadData> pandaFiles_ GUARDED_BY(pandaFilesLock_);

    PandaUniquePtr<AotManager> aotManager_;
    // Just to free them at destroy
    os::memory::Mutex copiedNamesLock_;
    PandaList<const uint8_t *> copiedNames_ GUARDED_BY(copiedNamesLock_);

    std::array<std::unique_ptr<ClassLinkerExtension>, ark::panda_file::LANG_COUNT> extensions_;

    bool isInitialized_ {false};

    NO_COPY_SEMANTIC(ClassLinker);
    NO_MOVE_SEMANTIC(ClassLinker);
};

class ClassLinkerErrorHandler {
public:
    virtual void OnError(ClassLinker::Error error, const PandaString &message) = 0;

public:
    ClassLinkerErrorHandler() = default;
    virtual ~ClassLinkerErrorHandler() = default;

    NO_MOVE_SEMANTIC(ClassLinkerErrorHandler);
    NO_COPY_SEMANTIC(ClassLinkerErrorHandler);
};

}  // namespace ark

#endif  // PANDA_RUNTIME_CLASS_LINKER_H_

/*
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

#ifndef PANDA_ASSEMBLER_ASSEMBLY_EMITTER_H
#define PANDA_ASSEMBLER_ASSEMBLY_EMITTER_H

#include <memory>
#include <type_traits>
#include <vector>
#include <unordered_map>

#include "assembly-ins.h"
#include "assembly-literals.h"
#include "assembly-program.h"
#include "assembly-type.h"
#include "assembly-function.h"
#include "bytecode_emitter.h"
#include "file_item_container.h"
#include "pgo.h"

namespace ark::pandasm {

class AsmEmitter {
public:
    struct PandaFileToPandaAsmMaps {
        std::unordered_map<uint32_t, std::string> methods;
        std::unordered_map<uint32_t, std::string> fields;
        std::unordered_map<uint32_t, std::string> classes;
        std::unordered_map<uint32_t, std::string> strings;
        std::unordered_map<uint32_t, std::string> literalarrays;
    };

    struct AsmEntityCollections {
        std::unordered_map<std::string, panda_file::BaseMethodItem *> methodItems;
        std::unordered_map<std::string, panda_file::BaseFieldItem *> fieldItems;
        std::unordered_map<std::string, panda_file::BaseClassItem *> classItems;
        std::unordered_map<std::string_view, panda_file::StringItem *> stringItems;
        std::unordered_map<std::string, panda_file::LiteralArrayItem *> literalarrayItems;
    };

    PANDA_PUBLIC_API static bool Emit(panda_file::ItemContainer *items, const Program &program,
                                      PandaFileToPandaAsmMaps *maps = nullptr, bool emitDebugInfo = true,
                                      ark::panda_file::pgo::ProfileOptimizer *profileOpt = nullptr);

    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    PANDA_PUBLIC_API static bool Emit(panda_file::Writer *writer, const Program &program,
                                      std::map<std::string, size_t> *stat = nullptr,
                                      PandaFileToPandaAsmMaps *maps = nullptr, bool debugInfo = true,
                                      ark::panda_file::pgo::ProfileOptimizer *profileOpt = nullptr);

    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    PANDA_PUBLIC_API static bool Emit(const std::string &filename, const Program &program,
                                      std::map<std::string, size_t> *stat = nullptr,
                                      PandaFileToPandaAsmMaps *maps = nullptr, bool debugInfo = true,
                                      ark::panda_file::pgo::ProfileOptimizer *profileOpt = nullptr);

    PANDA_PUBLIC_API static std::unique_ptr<const panda_file::File> Emit(const Program &program,
                                                                         PandaFileToPandaAsmMaps *maps = nullptr);

    PANDA_PUBLIC_API static bool AssignProfileInfo(Program *program);

    PANDA_PUBLIC_API static std::string GetLastError()
    {
        return lastError_;
    }

private:
    static void MakeStringItems(panda_file::ItemContainer *items, const Program &program,
                                AsmEntityCollections &entities);
    static void MakeLiteralItems(panda_file::ItemContainer *items, const Program &program,
                                 AsmEmitter::AsmEntityCollections &entities);
    static void MakeArrayTypeItems(panda_file::ItemContainer *items, const Program &program,
                                   AsmEntityCollections &entities);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool HandleRecordAsForeign(
        panda_file::ItemContainer *items, const Program &program, AsmEntityCollections &entities,
        const std::unordered_map<panda_file::Type::TypeId, panda_file::PrimitiveTypeItem *> &primitiveTypes,
        const std::string &name, const Record &rec);
    static bool HandleBaseRecord(panda_file::ItemContainer *items, const Program &program, const std::string &name,
                                 const Record &rec, panda_file::ClassItem *record);
    static bool HandleInterfaces(panda_file::ItemContainer *items, const Program &program, const std::string &name,
                                 const Record &rec, panda_file::ClassItem *record);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool HandleFields(
        panda_file::ItemContainer *items, const Program &program, AsmEmitter::AsmEntityCollections &entities,
        const std::unordered_map<panda_file::Type::TypeId, panda_file::PrimitiveTypeItem *> &primitiveTypes,
        const std::string &name, const Record &rec, panda_file::ClassItem *record);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool HandleRecord(
        panda_file::ItemContainer *items, const Program &program, AsmEntityCollections &entities,
        const std::unordered_map<panda_file::Type::TypeId, panda_file::PrimitiveTypeItem *> &primitiveTypes,
        const std::string &name, const Record &rec);
    static bool MakeRecordItems(
        panda_file::ItemContainer *items, const Program &program, AsmEntityCollections &entities,
        const std::unordered_map<panda_file::Type::TypeId, panda_file::PrimitiveTypeItem *> &primitiveTypes);
    static panda_file::StringItem *GetMethodName(panda_file::ItemContainer *items, const Function &func,
                                                 const std::string &name);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool HandleAreaForInner(panda_file::ItemContainer *items, const Program &program,
                                   panda_file::ClassItem **area, panda_file::ForeignClassItem **foreignArea,
                                   const std::string &name, const std::string &recordOwnerName);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool HandleRecordOnwer(panda_file::ItemContainer *items, const Program &program,
                                  panda_file::ClassItem **area, panda_file::ForeignClassItem **foreignArea,
                                  const std::string &name, const std::string &recordOwnerName);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool HandleFunctionParams(
        panda_file::ItemContainer *items, const Program &program, size_t idx, const std::string &name,
        const Function &func,
        const std::unordered_map<panda_file::Type::TypeId, panda_file::PrimitiveTypeItem *> &primitiveTypes,
        std::vector<panda_file::MethodParamItem> &params);
    static bool HandleFunctionLocalVariables(panda_file::ItemContainer *items, const Function &func,
                                             const std::string &name);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool CreateMethodItem(panda_file::ItemContainer *items, AsmEmitter::AsmEntityCollections &entities,
                                 const Function &func, panda_file::TypeItem *typeItem, panda_file::ClassItem *area,
                                 panda_file::ForeignClassItem *foreignArea, uint32_t accessFlags,
                                 panda_file::StringItem *methodName, const std::string &mangledName,
                                 const std::string &name, std::vector<panda_file::MethodParamItem> &params);
    static bool MakeFunctionItems(
        panda_file::ItemContainer *items, const Program &program, AsmEntityCollections &entities,
        const std::unordered_map<panda_file::Type::TypeId, panda_file::PrimitiveTypeItem *> &primitiveTypes,
        bool emitDebugInfo);
    static bool MakeRecordAnnotations(panda_file::ItemContainer *items, const Program &program,
                                      const AsmEntityCollections &entities);
    static void SetCodeAndDebugInfo(panda_file::ItemContainer *items, panda_file::MethodItem *method,
                                    const Function &func, bool emitDebugInfo);
    static void SetMethodSourceLang(const Program &program, panda_file::MethodItem *method, const Function &func,
                                    const std::string &name);
    static bool AddMethodAndParamsAnnotations(panda_file::ItemContainer *items, const Program &program,
                                              const AsmEmitter::AsmEntityCollections &entities,
                                              panda_file::MethodItem *method, const Function &func);
    static bool MakeFunctionDebugInfoAndAnnotations(panda_file::ItemContainer *items, const Program &program,
                                                    const AsmEntityCollections &entities, bool emitDebugInfo);
    static void FillMap(PandaFileToPandaAsmMaps *maps, AsmEntityCollections &entities);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static void EmitDebugInfo(panda_file::ItemContainer *items, const Program &program,
                              const std::vector<uint8_t> *bytes, const panda_file::MethodItem *method,
                              const Function &func, const std::string &name, bool emitDebugInfo);
    static bool EmitFunctions(panda_file::ItemContainer *items, const Program &program,
                              const AsmEntityCollections &entities, bool emitDebugInfo);

    static panda_file::TypeItem *GetTypeItem(
        panda_file::ItemContainer *items,
        const std::unordered_map<panda_file::Type::TypeId, panda_file::PrimitiveTypeItem *> &primitiveTypes,
        const Type &type, const Program &program);

    PANDA_PUBLIC_API static void SetLastError(const std::string &message)
    {
        lastError_ = message;
    }

    static bool CheckValueType(Value::Type valueType, const Type &type, const Program &program);

    static bool CheckValueEnumCase(const Value *value, const Type &type, const Program &program);
    static bool CheckValueArrayCase(const Value *value, const Type &type, const Program &program);
    static bool CheckValueMethodCase(const Value *value, const Program &program);
    static bool CheckValueRecordCase(const Value *value, const Program &program);
    static bool CheckValue(const Value *value, const Type &type, const Program &program);
    static std::string GetMethodSignatureFromProgram(const std::string &name, const Program &program);

    static panda_file::LiteralItem *CreateLiteralItem(
        panda_file::ItemContainer *container, const Value *value, std::vector<panda_file::LiteralItem> *out,
        const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);

    template <class PrimType>
    static panda_file::ScalarValueItem *CreateScalarPrimValueItem(panda_file::ItemContainer *container,
                                                                  const Value *value,
                                                                  std::vector<panda_file::ScalarValueItem> *out)
    {
        static_assert(std::is_arithmetic<PrimType>::value);
        auto v = value->GetAsScalar()->GetValue<PrimType>();
        if (out != nullptr) {
            out->emplace_back(v);
            return &out->back();
        }

        if constexpr (std::is_same<PrimType, uint32_t>::value) {
            return container->GetOrCreateIntegerValueItem(v);
        } else if constexpr (std::is_same<PrimType, uint64_t>::value) {
            return container->GetOrCreateLongValueItem(v);
        } else if constexpr (std::is_same<PrimType, float>::value) {
            return container->GetOrCreateFloatValueItem(v);
        } else if constexpr (std::is_same<PrimType, double>::value) {
            return container->GetOrCreateDoubleValueItem(v);
        } else {
            UNREACHABLE();
            return nullptr;
        }
    }

    static panda_file::ScalarValueItem *CreateScalarStringValueItem(panda_file::ItemContainer *container,
                                                                    const Value *value,
                                                                    std::vector<panda_file::ScalarValueItem> *out);
    static panda_file::ScalarValueItem *CreateScalarRecordValueItem(
        panda_file::ItemContainer *container, const Value *value, std::vector<panda_file::ScalarValueItem> *out,
        const std::unordered_map<std::string, panda_file::BaseClassItem *> &classes);
    static panda_file::ScalarValueItem *CreateScalarMethodValueItem(
        panda_file::ItemContainer *container, const Value *value, std::vector<panda_file::ScalarValueItem> *out,
        const Program &program, const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);
    static panda_file::ScalarValueItem *CreateScalarEnumValueItem(
        panda_file::ItemContainer *container, const Value *value, std::vector<panda_file::ScalarValueItem> *out,
        const std::unordered_map<std::string, panda_file::BaseFieldItem *> &fields);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static panda_file::ScalarValueItem *CreateScalarAnnotationValueItem(
        panda_file::ItemContainer *container, const Value *value, std::vector<panda_file::ScalarValueItem> *out,
        const Program &program, const std::unordered_map<std::string, panda_file::BaseClassItem *> &classes,
        const std::unordered_map<std::string, panda_file::BaseFieldItem *> &fields,
        const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);

    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static panda_file::ScalarValueItem *CreateScalarValueItem(
        panda_file::ItemContainer *container, const Value *value, std::vector<panda_file::ScalarValueItem> *out,
        const Program &program, const std::unordered_map<std::string, panda_file::BaseClassItem *> &classes,
        const std::unordered_map<std::string, panda_file::BaseFieldItem *> &fields,
        const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);

    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static panda_file::ValueItem *CreateValueItem(
        panda_file::ItemContainer *container, const Value *value, const Program &program,
        const std::unordered_map<std::string, panda_file::BaseClassItem *> &classes,
        const std::unordered_map<std::string, panda_file::BaseFieldItem *> &fields,
        const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);

    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static panda_file::AnnotationItem *CreateAnnotationItem(
        panda_file::ItemContainer *container, const AnnotationData &annotation, const Program &program,
        const std::unordered_map<std::string, panda_file::BaseClassItem *> &classes,
        const std::unordered_map<std::string, panda_file::BaseFieldItem *> &fields,
        const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);

    static panda_file::MethodHandleItem *CreateMethodHandleItem(
        panda_file::ItemContainer *container, const MethodHandle &mh,
        const std::unordered_map<std::string, panda_file::BaseFieldItem *> &fields,
        const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);

    template <class T>
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    static bool AddAnnotations(T *item, panda_file::ItemContainer *container, const AnnotationMetadata &metadata,
                               const Program &program,
                               const std::unordered_map<std::string, panda_file::BaseClassItem *> &classes,
                               const std::unordered_map<std::string, panda_file::BaseFieldItem *> &fields,
                               const std::unordered_map<std::string, panda_file::BaseMethodItem *> &methods);

    // NOTE(mgonopolsky): Refactor to introduce a single error-processing mechanism for parser and emitter
    // NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
    PANDA_PUBLIC_API static std::string lastError_;
};

std::string GetOwnerName(std::string name);
std::string GetItemName(std::string name);

}  // namespace ark::pandasm

#endif  // PANDA_ASSEMBLER_ASSEMBLY_EMITTER_H

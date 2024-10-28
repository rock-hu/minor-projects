/*
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

#include "evaluate/helpers.h"
#include "checker/types/globalTypesHolder.h"
#include "ir/ets/etsPrimitiveType.h"
#include "ir/ets/etsTypeReference.h"
#include "ir/ets/etsTypeReferencePart.h"
#include "ir/expressions/identifier.h"
#include "ir/statements/blockStatement.h"
#include "ir/ts/tsArrayType.h"
#include "ir/typeNode.h"

#include "assembler/assembly-type.h"
#include "libpandafile/field_data_accessor-inl.h"
#include "libpandafile/file-inl.h"

#include <algorithm>
#include <unordered_map>

namespace ark::es2panda::evaluate::helpers {

namespace {

ir::TypeNode *PrimitiveToTypeNode(panda_file::Type::TypeId typeId, checker::ETSChecker *checker)
{
    ir::PrimitiveType irType;
    switch (typeId) {
        case panda_file::Type::TypeId::VOID:
            irType = ir::PrimitiveType::VOID;
            break;
        case panda_file::Type::TypeId::U1:
            irType = ir::PrimitiveType::BOOLEAN;
            break;
        case panda_file::Type::TypeId::I8:
            irType = ir::PrimitiveType::BYTE;
            break;
        case panda_file::Type::TypeId::U16:
            irType = ir::PrimitiveType::CHAR;
            break;
        case panda_file::Type::TypeId::I16:
            irType = ir::PrimitiveType::SHORT;
            break;
        case panda_file::Type::TypeId::I32:
            irType = ir::PrimitiveType::INT;
            break;
        case panda_file::Type::TypeId::I64:
            irType = ir::PrimitiveType::LONG;
            break;
        case panda_file::Type::TypeId::F32:
            irType = ir::PrimitiveType::FLOAT;
            break;
        case panda_file::Type::TypeId::F64:
            irType = ir::PrimitiveType::DOUBLE;
            break;
        default:
            UNREACHABLE();
    }

    return checker->AllocNode<ir::ETSPrimitiveType>(irType);
}

ir::TypeNode *ClassReferenceToTypeNode(std::string_view name, checker::ETSChecker *checker)
{
    util::UString typeName(name, checker->Allocator());
    return CreateETSTypeReference(checker, typeName.View());
}

ir::TypeNode *ReferenceToTypeNode(std::string_view typeSignature, checker::ETSChecker *checker)
{
    ASSERT(checker);
    ASSERT(!typeSignature.empty());
    switch (typeSignature[0]) {
        case 'L': {
            // Variable is a reference.
            ASSERT(typeSignature.back() == ';');
            // Required to remove "std/core/" prefix, otherwise type name won't be parsed.
            auto startPos = typeSignature.find_last_of('/');
            if (startPos == std::string_view::npos) {
                startPos = 1;
            } else {
                startPos += 1;
            }
            return ClassReferenceToTypeNode(typeSignature.substr(startPos, typeSignature.size() - 1 - startPos),
                                            checker);
        }
        case '[': {
            // Variable is an array.
            size_t rank = std::count(typeSignature.begin(), typeSignature.end(), '[');
            auto *elementType = ToTypeNode(typeSignature.substr(rank), checker);
            if (elementType != nullptr) {
                for (size_t i = 0; i < rank; ++i) {
                    elementType = checker->AllocNode<ir::TSArrayType>(elementType);
                }
                return elementType;
            }
            return nullptr;
        }
        default:
            return nullptr;
    }
    return nullptr;
}

}  // namespace

ir::TypeNode *ToTypeNode(std::string_view typeSignature, checker::ETSChecker *checker)
{
    ASSERT(checker);
    ASSERT(!typeSignature.empty());

    if (typeSignature[0] == 'L' || typeSignature[0] == '[') {
        return ReferenceToTypeNode(typeSignature, checker);
    }

    pandasm::Type type = pandasm::Type::FromDescriptor(typeSignature);

    return PrimitiveToTypeNode(type.GetId(), checker);
}

ir::TypeNode *PandaTypeToTypeNode(const panda_file::File &pf, panda_file::FieldDataAccessor &fda,
                                  checker::ETSChecker *checker)
{
    auto pandaType = panda_file::Type::GetTypeFromFieldEncoding(fda.GetType());
    if (pandaType.IsReference()) {
        auto typeId = panda_file::FieldDataAccessor::GetTypeId(pf, fda.GetFieldId());
        std::string_view refSignature = utf::Mutf8AsCString(pf.GetStringData(typeId).data);
        return ReferenceToTypeNode(refSignature, checker);
    }
    return PrimitiveToTypeNode(pandaType.GetId(), checker);
}

ir::TypeNode *PandaTypeToTypeNode(const panda_file::File &pf, panda_file::Type pandaType,
                                  panda_file::File::EntityId classId, checker::ETSChecker *checker)
{
    if (pandaType.IsReference()) {
        ASSERT(classId.IsValid());
        std::string_view refSignature = utf::Mutf8AsCString(pf.GetStringData(classId).data);
        return ReferenceToTypeNode(refSignature, checker);
    }
    return PrimitiveToTypeNode(pandaType.GetId(), checker);
}

static checker::Type *PrimitiveToCheckerType(panda_file::Type::TypeId typeId, checker::GlobalTypesHolder *globalTypes)
{
    ASSERT(globalTypes);
    switch (typeId) {
        case panda_file::Type::TypeId::VOID:
            return globalTypes->GlobalETSVoidType();
        case panda_file::Type::TypeId::U1:
            return globalTypes->GlobalBooleanType();
        case panda_file::Type::TypeId::I8:
            return globalTypes->GlobalCharType();
        case panda_file::Type::TypeId::U8:
            return globalTypes->GlobalByteType();
        case panda_file::Type::TypeId::I16:
            [[fallthrough]];
        case panda_file::Type::TypeId::U16:
            return globalTypes->GlobalShortType();
        case panda_file::Type::TypeId::I32:
            [[fallthrough]];
        case panda_file::Type::TypeId::U32:
            return globalTypes->GlobalIntType();
        case panda_file::Type::TypeId::F32:
            return globalTypes->GlobalFloatType();
        case panda_file::Type::TypeId::F64:
            return globalTypes->GlobalDoubleType();
        case panda_file::Type::TypeId::I64:
            [[fallthrough]];
        case panda_file::Type::TypeId::U64:
            return globalTypes->GlobalLongType();
        default:
            return nullptr;
    }
    return nullptr;
}

static std::optional<std::string> ReferenceToName(std::string_view typeSignature,
                                                  checker::GlobalTypesHolder *globalTypes)
{
    static constexpr const size_t ARRAY_RANK_SYMBOLS = 2;

    ASSERT(globalTypes);
    ASSERT(!typeSignature.empty());

    switch (typeSignature[0]) {
        case 'L': {
            // Variable is a reference.
            ASSERT(typeSignature.back() == ';');
            // Required to remove "std/core/" prefix, otherwise type name won't be parsed.
            auto startPos = typeSignature.find_last_of('/');
            if (startPos == std::string_view::npos) {
                startPos = 1;
            } else {
                startPos += 1;
            }
            return std::string(typeSignature.substr(startPos, typeSignature.size() - 1 - startPos));
        }
        case '[': {
            // Variable is an array.
            auto rank = std::count(typeSignature.begin(), typeSignature.end(), '[');
            auto elementType = ToTypeName(typeSignature.substr(rank), globalTypes);
            if (!elementType) {
                return elementType;
            }

            auto &arrayType = *elementType;
            auto subtypeSize = arrayType.size();
            arrayType.resize(subtypeSize + rank * ARRAY_RANK_SYMBOLS);
            for (size_t i = subtypeSize, end = arrayType.size(); i < end; i += ARRAY_RANK_SYMBOLS) {
                arrayType[i] = '[';
                arrayType[i + 1] = ']';
            }
            return arrayType;
        }
        default:
            UNREACHABLE();
    }
    return {};
}

std::optional<std::string> ToTypeName(std::string_view typeSignature, checker::GlobalTypesHolder *globalTypes)
{
    ASSERT(globalTypes);
    ASSERT(!typeSignature.empty());

    if (typeSignature[0] == 'L' || typeSignature[0] == '[') {
        return ReferenceToName(typeSignature, globalTypes);
    }

    pandasm::Type type = pandasm::Type::FromDescriptor(typeSignature);

    auto *checkerType = PrimitiveToCheckerType(type.GetId(), globalTypes);
    ASSERT(checkerType);
    return checkerType->ToString();
}

panda_file::Type::TypeId GetTypeId(std::string_view typeSignature)
{
    if (typeSignature.empty()) {
        return panda_file::Type::TypeId::INVALID;
    }
    if (typeSignature[0] == 'L' || typeSignature[0] == '[') {
        return panda_file::Type::TypeId::REFERENCE;
    }
    pandasm::Type type = pandasm::Type::FromDescriptor(typeSignature);
    return type.GetId();
}

ir::BlockStatement *GetEnclosingBlock(ir::Identifier *ident)
{
    ASSERT(ident);

    ir::AstNode *iter = ident;

    while (iter->Parent() != nullptr && !iter->IsBlockStatement()) {
        iter = iter->Parent();
    }

    ASSERT(iter);
    return iter->AsBlockStatement();
}

SafeStateScope::SafeStateScope(checker::ETSChecker *checker, varbinder::ETSBinder *varBinder)
    : checker_(checker),
      varBinder_(varBinder),
      checkerScope_(checker->Scope()),
      binderTopScope_(varBinder->TopScope()),
      binderVarScope_(varBinder->VarScope()),
      binderScope_(varBinder->GetScope()),
      binderProgram_(varBinder->Program()),
      recordTable_(varBinder->GetRecordTable())
{
}

SafeStateScope::~SafeStateScope()
{
    (void)checker_;
    (void)varBinder_;
    (void)checkerScope_;
    (void)binderTopScope_;
    (void)binderVarScope_;
    (void)binderScope_;
    (void)binderProgram_;
    (void)recordTable_;
    ASSERT(checkerScope_ == checker_->Scope());
    ASSERT(binderTopScope_ == varBinder_->TopScope());
    ASSERT(binderVarScope_ == varBinder_->VarScope());
    ASSERT(binderScope_ == varBinder_->GetScope());
    ASSERT(binderProgram_ == varBinder_->Program());
    ASSERT(recordTable_ == varBinder_->GetRecordTable());
}

void AddExternalProgram(parser::Program *program, parser::Program *extProgram, std::string_view moduleName)
{
    ASSERT(program);
    ASSERT(extProgram);

    auto &extSources = program->ExternalSources();
    if (extSources.count(moduleName) == 0) {
        extSources.emplace(moduleName, program->Allocator()->Adapter());
    }
    extSources.at(moduleName).emplace_back(extProgram);
}

ir::ETSTypeReference *CreateETSTypeReference(checker::ETSChecker *checker, util::StringView name)
{
    auto *identRef = checker->AllocNode<ir::Identifier>(name, checker->Allocator());
    auto *typeRefPart = checker->AllocNode<ir::ETSTypeReferencePart>(identRef);
    return checker->AllocNode<ir::ETSTypeReference>(typeRefPart);
}

std::pair<std::string_view, std::string_view> SplitRecordName(std::string_view recordName)
{
    std::string_view moduleName;
    std::string_view newRecordName;

    if (auto pos = recordName.find_last_of('.'); pos != std::string_view::npos) {
        moduleName = recordName.substr(0, pos);
        newRecordName = recordName.substr(pos + 1, recordName.size());
    } else {
        newRecordName = recordName;
    }

    return std::make_pair(moduleName, newRecordName);
}

ir::ClassProperty *CreateClassProperty(checker::ETSChecker *checker, std::string_view name, ir::TypeNode *type,
                                       ir::ModifierFlags modifiers)
{
    ASSERT(type);

    auto *fieldIdent = checker->AllocNode<ir::Identifier>(name, checker->Allocator());
    auto *field =
        checker->AllocNode<ir::ClassProperty>(fieldIdent, nullptr, type, modifiers, checker->Allocator(), false);

    return field;
}

ir::ModifierFlags GetModifierFlags(panda_file::ClassDataAccessor &da)
{
    auto modifierFlags = ir::ModifierFlags::NONE;
    auto accFlags = da.GetAccessFlags();
    if ((accFlags & ACC_ABSTRACT) != 0) {
        modifierFlags |= ir::ModifierFlags::ABSTRACT;
    }
    if ((accFlags & ACC_FINAL) != 0) {
        modifierFlags |= ir::ModifierFlags::FINAL;
    }
    return modifierFlags;
}

}  // namespace ark::es2panda::evaluate::helpers

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

#ifndef ES2PANDA_COMPILER_CHECKER_ETS_FUNCTION_HELPERS_H
#define ES2PANDA_COMPILER_CHECKER_ETS_FUNCTION_HELPERS_H

#include "varbinder/varbinder.h"
#include "varbinder/declaration.h"
#include "varbinder/ETSBinder.h"
#include "varbinder/scope.h"
#include "varbinder/variable.h"
#include "varbinder/variableFlags.h"
#include "checker/ETSchecker.h"
#include "checker/ets/typeRelationContext.h"
#include "checker/types/ets/etsAsyncFuncReturnType.h"
#include "checker/types/ets/etsObjectType.h"
#include "checker/types/type.h"
#include "checker/types/typeFlag.h"
#include "ir/astNode.h"
#include "ir/typeNode.h"
#include "ir/base/catchClause.h"
#include "ir/base/classDefinition.h"
#include "ir/base/classProperty.h"
#include "ir/base/methodDefinition.h"
#include "ir/base/scriptFunction.h"
#include "ir/base/spreadElement.h"
#include "ir/ets/etsFunctionType.h"
#include "ir/ets/etsParameterExpression.h"
#include "ir/ets/etsTypeReference.h"
#include "ir/ets/etsTypeReferencePart.h"
#include "ir/expressions/arrowFunctionExpression.h"
#include "ir/expressions/assignmentExpression.h"
#include "ir/expressions/callExpression.h"
#include "ir/expressions/functionExpression.h"
#include "ir/expressions/identifier.h"
#include "ir/expressions/literals/numberLiteral.h"
#include "ir/expressions/memberExpression.h"
#include "ir/expressions/objectExpression.h"
#include "ir/expressions/thisExpression.h"
#include "ir/statements/blockStatement.h"
#include "ir/statements/doWhileStatement.h"
#include "ir/statements/expressionStatement.h"
#include "ir/statements/forInStatement.h"
#include "ir/statements/forOfStatement.h"
#include "ir/statements/forUpdateStatement.h"
#include "ir/statements/returnStatement.h"
#include "ir/statements/switchStatement.h"
#include "ir/statements/whileStatement.h"
#include "ir/ts/tsArrayType.h"
#include "ir/ts/tsInterfaceBody.h"
#include "ir/ts/tsTypeAliasDeclaration.h"
#include "ir/ts/tsTypeParameter.h"
#include "ir/ts/tsTypeParameterInstantiation.h"
#include "parser/program/program.h"
#include "util/helpers.h"
#include "util/language.h"

namespace ark::es2panda::checker {

static Type *MaybeBoxedType(ETSChecker *checker, Type *type, ir::Expression *expr)
{
    if (!type->HasTypeFlag(TypeFlag::ETS_PRIMITIVE)) {
        return type;
    }
    auto *relation = checker->Relation();
    auto *oldNode = relation->GetNode();
    relation->SetNode(expr);
    auto *res = checker->PrimitiveTypeAsETSBuiltinType(type);
    relation->SetNode(oldNode);
    return res;
}

static const Substitution *BuildImplicitSubstitutionForArguments(ETSChecker *checker, Signature *signature,
                                                                 const ArenaVector<ir::Expression *> &arguments)
{
    Substitution *substitution = checker->NewSubstitution();
    auto *sigInfo = signature->GetSignatureInfo();
    auto &sigParams = signature->GetSignatureInfo()->typeParams;

    for (size_t ix = 0; ix < arguments.size(); ix++) {
        auto *arg = arguments[ix];
        if (arg->IsObjectExpression()) {
            continue;
        }

        auto *const argType = arg->IsSpreadElement()
                                  ? MaybeBoxedType(checker, arg->AsSpreadElement()->Argument()->Check(checker),
                                                   arg->AsSpreadElement()->Argument())
                                  : MaybeBoxedType(checker, arg->Check(checker), arg);
        auto *const paramType = (ix < signature->MinArgCount()) ? sigInfo->params[ix]->TsType()
                                : sigInfo->restVar != nullptr   ? sigInfo->restVar->TsType()
                                                                : nullptr;

        if (paramType == nullptr) {
            continue;
        }

        if (!checker->EnhanceSubstitutionForType(sigInfo->typeParams, paramType, argType, substitution)) {
            return nullptr;
        }
    }

    if (substitution->size() != sigParams.size()) {
        for (const auto typeParam : sigParams) {
            auto newTypeParam = typeParam->AsETSTypeParameter();
            if (auto it = substitution->find(newTypeParam); it != substitution->cend()) {
                continue;
            }
            if (newTypeParam->GetDefaultType() == nullptr) {
                return nullptr;
            }
            auto dflt = newTypeParam->GetDefaultType()->Substitute(checker->Relation(), substitution);
            if (!checker->EnhanceSubstitutionForType(sigInfo->typeParams, newTypeParam, dflt, substitution)) {
                return nullptr;
            }
        }

        if (substitution->size() != sigParams.size() &&
            (signature->Function()->ReturnTypeAnnotation() == nullptr ||
             !checker->EnhanceSubstitutionForType(sigInfo->typeParams,
                                                  signature->Function()->ReturnTypeAnnotation()->TsType(),
                                                  signature->ReturnType(), substitution))) {
            return nullptr;
        }
    }

    return substitution;
}

static const Substitution *BuildExplicitSubstitutionForArguments(ETSChecker *checker, Signature *signature,
                                                                 const ArenaVector<ir::TypeNode *> &params,
                                                                 const lexer::SourcePosition &pos,
                                                                 TypeRelationFlag flags)
{
    auto &sigParams = signature->GetSignatureInfo()->typeParams;
    auto *substitution = checker->NewSubstitution();
    auto *constraintsSubstitution = checker->NewSubstitution();
    ArenaVector<Type *> instArgs {checker->Allocator()->Adapter()};

    for (size_t ix = 0; ix < params.size(); ++ix) {
        instArgs.push_back(MaybeBoxedType(checker, params[ix]->GetType(checker), params[ix]));
        if (ix < sigParams.size()) {
            ETSChecker::EmplaceSubstituted(constraintsSubstitution, sigParams[ix]->AsETSTypeParameter(), instArgs[ix]);
        }
    }
    for (size_t ix = instArgs.size(); ix < sigParams.size(); ++ix) {
        auto *dflt = sigParams[ix]->AsETSTypeParameter()->GetDefaultType();
        if (dflt == nullptr) {
            break;
        }

        dflt = dflt->Substitute(checker->Relation(), constraintsSubstitution);
        instArgs.push_back(dflt);
        ETSChecker::EmplaceSubstituted(constraintsSubstitution, sigParams[ix]->AsETSTypeParameter(), instArgs[ix]);
    }
    if (sigParams.size() != instArgs.size()) {
        if ((flags & TypeRelationFlag::NO_THROW) != 0) {
            return nullptr;
        }
        checker->LogTypeError({"Expected ", sigParams.size(), " type arguments, got ", instArgs.size(), " ."}, pos);
        return nullptr;
    }

    for (size_t ix = 0; ix < sigParams.size(); ix++) {
        if (!checker->IsCompatibleTypeArgument(sigParams[ix]->AsETSTypeParameter(), instArgs[ix],
                                               constraintsSubstitution)) {
            return nullptr;
        }
        ETSChecker::EmplaceSubstituted(substitution, sigParams[ix]->AsETSTypeParameter(), instArgs[ix]);
    }
    return substitution;
}

static Signature *MaybeSubstituteTypeParameters(ETSChecker *checker, Signature *signature,
                                                const ir::TSTypeParameterInstantiation *typeArguments,
                                                const ArenaVector<ir::Expression *> &arguments,
                                                const lexer::SourcePosition &pos, TypeRelationFlag flags)
{
    if (typeArguments == nullptr && signature->GetSignatureInfo()->typeParams.empty()) {
        return signature;
    }

    const Substitution *substitution =
        (typeArguments != nullptr)
            ? BuildExplicitSubstitutionForArguments(checker, signature, typeArguments->Params(), pos, flags)
            : BuildImplicitSubstitutionForArguments(checker, signature, arguments);

    return (substitution == nullptr) ? nullptr : signature->Substitute(checker->Relation(), substitution);
}

static bool CmpAssemblerTypesWithRank(Signature *sig1, Signature *sig2)
{
    for (size_t ix = 0; ix < sig1->MinArgCount(); ix++) {
        std::stringstream s1;
        std::stringstream s2;
        sig1->Params()[ix]->TsType()->ToAssemblerTypeWithRank(s1);
        sig2->Params()[ix]->TsType()->ToAssemblerTypeWithRank(s2);
        if (s1.str() != s2.str()) {
            return false;
            break;
        }
    }
    return true;
}

static bool HasSameAssemblySignature(ETSFunctionType *func1, ETSFunctionType *func2)
{
    for (auto *sig1 : func1->CallSignatures()) {
        for (auto *sig2 : func2->CallSignatures()) {
            if (sig1->MinArgCount() != sig2->MinArgCount()) {
                continue;
            }
            bool allSame = CmpAssemblerTypesWithRank(sig1, sig2);
            if (!allSame) {
                continue;
            }
            auto *rv1 = sig1->RestVar();
            auto *rv2 = sig2->RestVar();
            if (rv1 == nullptr && rv2 == nullptr) {
                return true;
            }
            if (rv1 == nullptr || rv2 == nullptr) {  // exactly one of them is null
                return false;
            }
            std::stringstream s1;
            std::stringstream s2;
            rv1->TsType()->ToAssemblerTypeWithRank(s1);
            rv2->TsType()->ToAssemblerTypeWithRank(s2);
            if (s1.str() == s2.str()) {
                return true;
            }
        }
    }
    return false;
}

static bool CheckInterfaceOverride(ETSChecker *const checker, ETSObjectType *const interface,
                                   Signature *const signature)
{
    bool isOverriding = checker->CheckOverride(signature, interface);

    for (auto *const superInterface : interface->Interfaces()) {
        isOverriding |= CheckInterfaceOverride(checker, superInterface, signature);
    }

    return isOverriding;
}

static varbinder::Scope *NodeScope(ir::AstNode *ast)
{
    if (ast->IsBlockStatement()) {
        return ast->AsBlockStatement()->Scope();
    }
    if (ast->IsDoWhileStatement()) {
        return ast->AsDoWhileStatement()->Scope();
    }
    if (ast->IsForInStatement()) {
        return ast->AsForInStatement()->Scope();
    }
    if (ast->IsForOfStatement()) {
        return ast->AsForOfStatement()->Scope();
    }
    if (ast->IsForUpdateStatement()) {
        return ast->AsForUpdateStatement()->Scope();
    }
    if (ast->IsSwitchStatement()) {
        return ast->AsSwitchStatement()->Scope();
    }
    if (ast->IsWhileStatement()) {
        return ast->AsWhileStatement()->Scope();
    }
    if (ast->IsCatchClause()) {
        return ast->AsCatchClause()->Scope();
    }
    if (ast->IsClassDefinition()) {
        return ast->AsClassDefinition()->Scope();
    }
    if (ast->IsScriptFunction()) {
        return ast->AsScriptFunction()->Scope()->ParamScope();
    }
    return nullptr;
}

}  // namespace ark::es2panda::checker

#endif

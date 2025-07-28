/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "genericBridgesLowering.h"

#include "compiler/lowering/scopesInit/scopesInitPhase.h"
#include "compiler/lowering/util.h"

namespace ark::es2panda::compiler {

std::string GenericBridgesPhase::CreateMethodDefinitionString(ir::ClassDefinition const *classDefinition,
                                                              checker::Signature const *baseSignature,
                                                              ir::ScriptFunction const *derivedFunction,
                                                              std::vector<ir::AstNode *> &typeNodes) const noexcept
{
    constexpr std::size_t SOURCE_CODE_LENGTH = 128U;

    std::string str1 {};
    str1.reserve(2U * SOURCE_CODE_LENGTH);

    std::string str2 {};
    str2.reserve(SOURCE_CODE_LENGTH);

    auto const &functionName = derivedFunction->Id()->Name().Mutf8();
    str1 = functionName + '(';

    str2 += ")." + functionName + '(';

    auto const &baseParameters = baseSignature->Params();
    auto const &derivedParameters = derivedFunction->Signature()->Params();
    auto const parameterNumber = baseParameters.size();

    for (std::size_t i = 0U; i < parameterNumber; ++i) {
        if (i != 0U) {
            str1 += ", ";
            str2 += ", ";
        }

        auto const *const derivedParameter = derivedParameters[i];
        auto const &parameterName = derivedParameter->Name().Utf8();
        str1 += parameterName;
        typeNodes.emplace_back(
            context_->AllocNode<ir::OpaqueTypeNode>(baseParameters[i]->TsType(), context_->Allocator()));
        str1 += ": @@T" + std::to_string(typeNodes.size());

        str2 += parameterName;
        typeNodes.emplace_back(
            context_->AllocNode<ir::OpaqueTypeNode>(derivedParameter->TsType(), context_->Allocator()));
        str2 += " as @@T" + std::to_string(typeNodes.size());
    }

    typeNodes.emplace_back(context_->AllocNode<ir::OpaqueTypeNode>(
        const_cast<checker::Type *>(derivedFunction->Signature()->ReturnType()), context_->Allocator()));
    str1 += "): @@T" + std::to_string(typeNodes.size()) + ' ';

    typeNodes.emplace_back(context_->AllocNode<ir::OpaqueTypeNode>(
        const_cast<checker::Type *>(classDefinition->TsType()), context_->Allocator()));
    str2 = "{ return (this as @@T" + std::to_string(typeNodes.size()) + str2 + "); }";

    str1 += str2;
    return str1;
}

void GenericBridgesPhase::AddGenericBridge(ir::ClassDefinition const *const classDefinition,
                                           ir::MethodDefinition *const methodDefinition,
                                           checker::Signature const *baseSignature,
                                           ir::ScriptFunction const *const derivedFunction) const
{
    auto *parser = context_->parser->AsETSParser();
    std::vector<ir::AstNode *> typeNodes {};
    typeNodes.reserve(2U * baseSignature->Params().size() + 2U);

    auto const sourceCode = CreateMethodDefinitionString(classDefinition, baseSignature, derivedFunction, typeNodes);
    auto *const bridgeMethodDefinition = parser->CreateFormattedClassMethodDefinition(sourceCode, typeNodes);
    ES2PANDA_ASSERT(bridgeMethodDefinition != nullptr);
    auto *const bridgeMethod = bridgeMethodDefinition->AsMethodDefinition();
    ES2PANDA_ASSERT(bridgeMethod != nullptr && methodDefinition->Id() != nullptr);
    bridgeMethod->AddModifier(methodDefinition->Modifiers());
    bridgeMethod->ClearModifier(ir::ModifierFlags::NATIVE | ir::ModifierFlags::ABSTRACT);
    bridgeMethod->AddAstNodeFlags(methodDefinition->GetAstNodeFlags());
    bridgeMethod->SetParent(const_cast<ir::ClassDefinition *>(classDefinition));

    auto *varBinder = context_->checker->VarBinder()->AsETSBinder();
    auto *scope = NearestScope(methodDefinition);
    auto scopeGuard = varbinder::LexicalScope<varbinder::Scope>::Enter(varBinder, scope);
    InitScopesPhaseETS::RunExternalNode(bridgeMethod, varBinder);

    varbinder::BoundContext boundCtx {varBinder->GetRecordTable(), const_cast<ir::ClassDefinition *>(classDefinition),
                                      true};
    varBinder->AsETSBinder()->ResolveReferencesForScopeWithContext(bridgeMethod, scope);

    auto *checker = context_->checker->AsETSChecker();
    auto const checkerCtx =
        checker::SavedCheckerContext(checker,
                                     checker::CheckerStatus::IN_CLASS | checker::CheckerStatus::IGNORE_VISIBILITY |
                                         checker::CheckerStatus::IN_BRIDGE_TEST,
                                     classDefinition->TsType()->AsETSObjectType());
    auto scopeCtx = checker::ScopeContext(checker, scope);

    //  Note: we need to create and set function/method type here because the general method `BuildMethodSignature(...)`
    //  is not suitable for this case. Moreover, we have to save and restore proper type for `methodDefinition` because
    //  call to `BuildFunctionSignature(...)` breaks it!
    auto *methodType = methodDefinition->Id()->Variable()->TsType()->AsETSFunctionType();

    checker->BuildFunctionSignature(bridgeMethod->Function());
    auto *const bridgeMethodType = checker->BuildMethodType(bridgeMethod->Function());
    checker->CheckIdenticalOverloads(methodType, bridgeMethodType, bridgeMethod);
    bridgeMethod->SetTsType(bridgeMethodType);
    methodType->AddCallSignature(bridgeMethod->Function()->Signature());
    methodDefinition->Id()->Variable()->SetTsType(methodType);

    bridgeMethod->Check(checker);
}

void GenericBridgesPhase::ProcessScriptFunction(ir::ClassDefinition const *const classDefinition,
                                                ir::ScriptFunction *const baseFunction,
                                                ir::MethodDefinition *const derivedMethod,
                                                Substitutions const &substitutions) const
{
    auto *const checker = context_->checker->AsETSChecker();
    auto *const relation = checker->Relation();

    auto const overrides = [checker, relation, classDefinition](checker::Signature const *source,
                                                                checker::Signature const *target) -> bool {
        checker::SavedCheckerContext const checkerCtx(
            checker, checker->Context().Status() | checker::CheckerStatus::IN_BRIDGE_TEST,
            classDefinition->TsType()->AsETSObjectType());
        checker::SavedTypeRelationFlagsContext const savedFlags(relation, checker::TypeRelationFlag::BRIDGE_CHECK);
        return relation->SignatureIsSupertypeOf(const_cast<checker::Signature *>(source),
                                                const_cast<checker::Signature *>(target));
    };

    //  We are not interested in functions that either don't have type parameters at all
    //  or have type parameters that are not modified in the derived class
    ES2PANDA_ASSERT(baseFunction);
    auto const *baseSignature1 = baseFunction->Signature()->Substitute(relation, substitutions.baseConstraints);
    if (baseSignature1 == baseFunction->Signature()) {
        return;
    }

    auto *baseSignature2 = baseFunction->Signature()->Substitute(relation, substitutions.derivedSubstitutions);
    if (baseSignature2 == baseFunction->Signature()) {
        return;
    }
    baseSignature2 = baseSignature2->Substitute(relation, substitutions.derivedConstraints);

    ir::ScriptFunction const *derivedFunction = nullptr;
    checker::ETSFunctionType const *methodType = derivedMethod->Id()->Variable()->TsType()->AsETSFunctionType();
    for (auto *signature : methodType->CallSignatures()) {
        signature = signature->Substitute(relation, substitutions.derivedConstraints);
        if (overrides(baseSignature1, signature) || checker->HasSameAssemblySignature(baseSignature1, signature)) {
            //  NOTE: we already have custom-implemented method with the required bridge signature.
            //  Probably sometimes we will issue warning notification here...
            return;
        }

        if (overrides(signature, baseSignature1) && overrides(baseSignature1, baseSignature2)) {
            // This derived overload already handles the base union signature.
            return;
        }

        if (derivedFunction == nullptr && overrides(signature, baseSignature2)) {
            //  NOTE: we don't care the possible case of mapping several derived function to the same bridge
            //  signature. Probably sometimes we will process it correctly or issue warning notification here...
            derivedFunction = signature->Function();
        }
    }

    if (derivedFunction != nullptr && derivedFunction != baseFunction) {
        AddGenericBridge(classDefinition, derivedMethod, baseSignature1, derivedFunction);
    }
}

void GenericBridgesPhase::MaybeAddGenericBridges(ir::ClassDefinition const *const classDefinition,
                                                 ir::MethodDefinition *const baseMethod,
                                                 ir::MethodDefinition *const derivedMethod,
                                                 Substitutions const &substitutions) const
{
    ProcessScriptFunction(classDefinition, baseMethod->Function(), derivedMethod, substitutions);
    for (auto *const overload : baseMethod->Overloads()) {
        ProcessScriptFunction(classDefinition, overload->Function(), derivedMethod, substitutions);
    }
}

void GenericBridgesPhase::CreateGenericBridges(ir::ClassDefinition const *const classDefinition,
                                               Substitutions &substitutions,
                                               ArenaVector<ir::AstNode *> const &items) const
{
    auto const &classBody = classDefinition->Body();

    //  Collect type parameters defaults/constraints in the derived class
    auto *checker = context_->checker->AsETSChecker();
    substitutions.derivedConstraints = checker->NewSubstitution();

    auto const *const classType = classDefinition->TsType()->AsETSObjectType();
    auto const &typeParameters = classType->GetConstOriginalBaseType()->AsETSObjectType()->TypeArguments();
    for (auto *const parameter : typeParameters) {
        auto *const typeParameter = parameter->AsETSTypeParameter();
        checker->EmplaceSubstituted(substitutions.derivedConstraints, typeParameter,
                                    typeParameter->GetConstraintType());
    }

    for (auto *item : items) {
        if (item->IsMethodDefinition()) {
            // Skip `static`, `final` and special methods...
            auto *const method = item->AsMethodDefinition();
            ES2PANDA_ASSERT(method->Id());
            if (method->Kind() != ir::MethodDefinitionKind::METHOD || method->IsStatic() || method->IsFinal() ||
                method->Id()->Name().Utf8().find("lambda$invoke$") != std::string_view::npos) {
                continue;
            }

            // Check if the derived class has any possible overrides of this method
            auto isOverridePred = [&name = method->Id()->Name()](ir::AstNode const *node) -> bool {
                return node->IsMethodDefinition() && !node->IsStatic() &&
                       node->AsMethodDefinition()->Id()->Name() == name;
            };
            auto it = std::find_if(classBody.cbegin(), classBody.end(), isOverridePred);
            if (it != classBody.cend()) {
                MaybeAddGenericBridges(classDefinition, method, (*it)->AsMethodDefinition(), substitutions);
            }
        }
    }
}

GenericBridgesPhase::Substitutions GenericBridgesPhase::GetSubstitutions(
    checker::ETSObjectType const *const objectType, ArenaVector<checker::Type *> const &typeParameters) const noexcept
{
    auto const &typeArguments = objectType->TypeArguments();
    auto const parameterNumber = typeParameters.size();
    ES2PANDA_ASSERT(parameterNumber == typeArguments.size());

    auto *checker = context_->checker->AsETSChecker();
    Substitutions substitutions {};
    substitutions.derivedSubstitutions = checker->NewSubstitution();
    substitutions.baseConstraints = checker->NewSubstitution();

    //  We need to check if the class derived from base generic class (or implementing generic interface)
    //  has either explicit class type substitutions or type parameters with narrowing constraints.
    for (std::size_t i = 0U; i < parameterNumber; ++i) {
        auto *const typeParameter = typeParameters[i]->AsETSTypeParameter();
        checker::Type *const typeArgument = typeArguments[i];

        //  Collect type parameters defaults/constraints in the base class
        //  and type argument substitutions in the derived class
        checker->EmplaceSubstituted(substitutions.derivedSubstitutions, typeParameter, typeArgument);
        if (auto *const defaultType = typeParameter->GetDefaultType(); defaultType != nullptr) {
            checker->EmplaceSubstituted(substitutions.baseConstraints, typeParameter, defaultType);
        } else {
            checker->EmplaceSubstituted(substitutions.baseConstraints, typeParameter,
                                        typeParameter->GetConstraintType());
        }
    }

    return substitutions;
}

void GenericBridgesPhase::ProcessInterfaces(ir::ClassDefinition *const classDefinition,
                                            ArenaVector<checker::ETSObjectType *> const &interfaces) const
{
    for (auto const *interfaceType : interfaces) {
        if (auto const &typeParameters = interfaceType->GetConstOriginalBaseType()->AsETSObjectType()->TypeArguments();
            !typeParameters.empty()) {
            if (Substitutions substitutions = GetSubstitutions(interfaceType, typeParameters);
                // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
                (substitutions.derivedSubstitutions != nullptr) && !substitutions.derivedSubstitutions->empty()) {
                ES2PANDA_ASSERT(interfaceType->GetDeclNode()->IsTSInterfaceDeclaration());
                auto const &interfaceBody = interfaceType->GetDeclNode()->AsTSInterfaceDeclaration()->Body()->Body();
                CreateGenericBridges(classDefinition, substitutions, interfaceBody);
            }
        }

        ProcessInterfaces(classDefinition, interfaceType->Interfaces());
    }
}

ir::ClassDefinition *GenericBridgesPhase::ProcessClassDefinition(ir::ClassDefinition *const classDefinition) const
{
    //  Check class interfaces.
    ProcessInterfaces(classDefinition, classDefinition->TsType()->AsETSObjectType()->Interfaces());

    //  Check if the base class is a generic class.
    if (classDefinition->Super() == nullptr || classDefinition->Super()->TsType() == nullptr ||
        !classDefinition->Super()->TsType()->IsETSObjectType()) {
        return classDefinition;
    }

    auto const *const superType = classDefinition->Super()->TsType()->AsETSObjectType();
    auto const &typeParameters = superType->GetConstOriginalBaseType()->AsETSObjectType()->TypeArguments();
    if (typeParameters.empty()) {
        return classDefinition;
    }

    //  Check if the class derived from base generic class has either explicit class type substitutions
    //  or type parameters with narrowing constraints.
    if (Substitutions substitutions = GetSubstitutions(superType, typeParameters);
        (substitutions.derivedSubstitutions != nullptr) &&
        // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
        !substitutions.derivedSubstitutions->empty()) {
        // If it has, then probably the generic bridges should be created.
        auto const &superClassBody =
            classDefinition->Super()->TsType()->AsETSObjectType()->GetDeclNode()->AsClassDefinition()->Body();
        CreateGenericBridges(classDefinition, substitutions, superClassBody);
    }

    return classDefinition;
}

bool GenericBridgesPhase::PerformForModule(public_lib::Context *ctx, parser::Program *program)
{
    context_ = ctx;

    program->Ast()->TransformChildrenRecursively(
        // CC-OFFNXT(G.FMT.14-CPP) project code style
        [this](ir::AstNode *ast) -> ir::AstNode * {
            if (ast->IsClassDefinition()) {
                return ProcessClassDefinition(ast->AsClassDefinition());
            }
            return ast;
        },
        Name());

    return true;
}

}  // namespace ark::es2panda::compiler

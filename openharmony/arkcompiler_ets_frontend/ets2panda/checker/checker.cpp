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

#include "checker.h"

#include "public/public.h"
#include "checker/types/globalTypesHolder.h"
#include "checker/types/ts/unionType.h"

namespace ark::es2panda::checker {
Checker::Checker(util::DiagnosticEngine &diagnosticEngine)
    : allocator_(SpaceType::SPACE_TYPE_COMPILER, nullptr, true),
      context_(this, CheckerStatus::NO_OPTS),
      globalTypes_(allocator_.New<GlobalTypesHolder>(&allocator_)),
      relation_(allocator_.New<TypeRelation>(this)),
      diagnosticEngine_(diagnosticEngine)
{
}

void Checker::Initialize(varbinder::VarBinder *varbinder)
{
    varbinder_ = varbinder;
    scope_ = varbinder_->TopScope();
    program_ = varbinder_->Program();
}

void Checker::LogError(const diagnostic::DiagnosticKind &diagnostic,
                       const util::DiagnosticMessageParams &diagnosticParams, const lexer::SourcePosition &pos)
{
    diagnosticEngine_.LogDiagnostic(diagnostic, diagnosticParams, pos);
}

void Checker::LogError(const diagnostic::DiagnosticKind &diagnostic, const lexer::SourcePosition &pos)
{
    LogError(diagnostic, {}, pos);
}

void Checker::LogTypeError(std::string_view message, const lexer::SourcePosition &pos)
{
    diagnosticEngine_.LogSemanticError(message, pos);
}

void Checker::Warning(const std::string_view message, const lexer::SourcePosition &pos) const
{
    diagnosticEngine_.LogWarning(message, pos);
}

void Checker::ReportWarning(const util::DiagnosticMessageParams &list, const lexer::SourcePosition &pos)
{
    diagnosticEngine_.LogWarning(list, pos);
}

bool Checker::IsAllTypesAssignableTo(Type *source, Type *target)
{
    if (source->TypeFlags() == TypeFlag::UNION) {
        auto &types = source->AsUnionType()->ConstituentTypes();

        return std::all_of(types.begin(), types.end(),
                           [this, target](auto *it) { return IsAllTypesAssignableTo(it, target); });
    }

    return relation_->IsAssignableTo(source, target);
}

bool Checker::IsTypeIdenticalTo(Type *source, Type *target)
{
    return relation_->IsIdenticalTo(source, target);
}

bool Checker::IsTypeIdenticalTo(Type *source, Type *target, const diagnostic::DiagnosticKind &diagKind,
                                const util::DiagnosticMessageParams &diagParams, const lexer::SourcePosition &errPos)
{
    if (!IsTypeIdenticalTo(source, target)) {
        relation_->GetChecker()->LogError(diagKind, diagParams, errPos);
        return false;
    }

    return true;
}

bool Checker::IsTypeIdenticalTo(Type *source, Type *target, const diagnostic::DiagnosticKind &diagKind,
                                const lexer::SourcePosition &errPos)
{
    return IsTypeIdenticalTo(source, target, diagKind, {}, errPos);
}

bool Checker::IsTypeAssignableTo(Type *source, Type *target)
{
    return relation_->IsAssignableTo(source, target);
}

bool Checker::IsTypeAssignableTo(Type *source, Type *target, const diagnostic::DiagnosticKind &diagKind,
                                 const util::DiagnosticMessageParams &list, const lexer::SourcePosition &errPos)
{
    if (!IsTypeAssignableTo(source, target)) {
        relation_->RaiseError(diagKind, list, errPos);
    }

    return true;
}

bool Checker::IsTypeComparableTo(Type *source, Type *target)
{
    return relation_->IsComparableTo(source, target);
}

bool Checker::IsTypeComparableTo(Type *source, Type *target, const diagnostic::DiagnosticKind &diagKind,
                                 const util::DiagnosticMessageParams &list, const lexer::SourcePosition &errPos)
{
    if (!IsTypeComparableTo(source, target)) {
        relation_->RaiseError(diagKind, list, errPos);
    }

    return true;
}

bool Checker::AreTypesComparable(Type *source, Type *target)
{
    return IsTypeComparableTo(source, target) || IsTypeComparableTo(target, source);
}

bool Checker::IsTypeEqualityComparableTo(Type *source, Type *target)
{
    return IsTypeComparableTo(source, target);
}

parser::Program *Checker::Program() const
{
    return program_;
}

void Checker::SetProgram(parser::Program *program)
{
    program_ = program;
}

varbinder::VarBinder *Checker::VarBinder() const
{
    return varbinder_;
}

void Checker::SetAnalyzer(SemanticAnalyzer *analyzer)
{
    analyzer_ = analyzer;
}

checker::SemanticAnalyzer *Checker::GetAnalyzer() const
{
    return analyzer_;
}

bool Checker::IsAnyError()
{
    return DiagnosticEngine().IsAnyError();
}

ScopeContext::ScopeContext(Checker *checker, varbinder::Scope *newScope)
    : checker_(checker), prevScope_(checker_->scope_), prevProgram_(checker_->Program())
{
    checker_->scope_ = newScope;
    if (newScope != nullptr && newScope->Node() != nullptr) {
        auto *topStatement = newScope->Node()->GetTopStatement();
        if (topStatement->IsETSModule()) {
            checker_->SetProgram(topStatement->AsETSModule()->Program());
        }
    }
}

void Checker::CleanUp()
{
    context_ = CheckerContext(this, CheckerStatus::NO_OPTS);
    globalTypes_ = allocator_.New<GlobalTypesHolder>(&allocator_);
    relation_ = allocator_.New<TypeRelation>(this);
    identicalResults_.cached.clear();
    assignableResults_.cached.clear();
    comparableResults_.cached.clear();
    uncheckedCastableResults_.cached.clear();
    supertypeResults_.cached.clear();
    typeStack_.clear();
    namedTypeStack_.clear();
}

}  // namespace ark::es2panda::checker

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

#include "methodDefinition.h"

#include "checker/TSchecker.h"
#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "classDefinition.h"
#include "ir/ts/tsInterfaceBody.h"
#include "compiler/lowering/util.h"

namespace ark::es2panda::ir {

ScriptFunction *MethodDefinition::Function()
{
    return value_->IsFunctionExpression() ? value_->AsFunctionExpression()->Function() : nullptr;
}

const ScriptFunction *MethodDefinition::Function() const
{
    return value_->IsFunctionExpression() ? value_->AsFunctionExpression()->Function() : nullptr;
}

PrivateFieldKind MethodDefinition::ToPrivateFieldKind(bool const isStatic) const
{
    switch (kind_) {
        case MethodDefinitionKind::METHOD: {
            return isStatic ? PrivateFieldKind::STATIC_METHOD : PrivateFieldKind::METHOD;
        }
        case MethodDefinitionKind::GET: {
            return isStatic ? PrivateFieldKind::STATIC_GET : PrivateFieldKind::GET;
        }
        case MethodDefinitionKind::SET: {
            return isStatic ? PrivateFieldKind::STATIC_SET : PrivateFieldKind::SET;
        }
        default: {
            ES2PANDA_UNREACHABLE();
        }
    }
}

void MethodDefinition::ResolveReferences(const NodeTraverser &cb) const
{
    cb(key_);
    cb(value_);

    for (auto *it : VectorIterationGuard(overloads_)) {
        cb(it);
    }

    for (auto *it : VectorIterationGuard(decorators_)) {
        cb(it);
    }
}

void MethodDefinition::Iterate(const NodeTraverser &cb) const
{
    cb(key_);
    cb(value_);

    for (auto *it : overloads_) {
        if (it->Parent() == this) {
            cb(it);
        }
    }

    for (auto *it : VectorIterationGuard(decorators_)) {
        cb(it);
    }
}

void MethodDefinition::TransformChildren(const NodeTransformer &cb, std::string_view const transformationName)
{
    if (auto *transformedNode = cb(key_); key_ != transformedNode) {
        key_->SetTransformedNode(transformationName, transformedNode);
        key_ = transformedNode->AsExpression();
    }

    if (auto *transformedNode = cb(value_); value_ != transformedNode) {
        value_->SetTransformedNode(transformationName, transformedNode);
        value_ = transformedNode->AsExpression();
    }

    for (auto *&it : VectorIterationGuard(overloads_)) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsMethodDefinition();
        }
    }

    for (auto *&it : VectorIterationGuard(decorators_)) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsDecorator();
        }
    }
}

void MethodDefinition::Dump(ir::AstDumper *dumper) const
{
    const char *kind = nullptr;

    switch (kind_) {
        case MethodDefinitionKind::CONSTRUCTOR: {
            kind = "constructor";
            break;
        }
        case MethodDefinitionKind::METHOD: {
            kind = "method";
            break;
        }
        case MethodDefinitionKind::EXTENSION_METHOD: {
            kind = "extensionmethod";
            break;
        }
        case MethodDefinitionKind::GET: {
            kind = "get";
            break;
        }
        case MethodDefinitionKind::SET: {
            kind = "set";
            break;
        }
        case MethodDefinitionKind::EXTENSION_GET: {
            kind = "extensionget";
            break;
        }
        case MethodDefinitionKind::EXTENSION_SET: {
            kind = "extensionset";
            break;
        }
        default: {
            ES2PANDA_UNREACHABLE();
        }
    }

    dumper->Add({{"type", "MethodDefinition"},
                 {"key", key_},
                 {"kind", kind},
                 {"accessibility", AstDumper::Optional(AstDumper::ModifierToString(flags_))},
                 {"static", IsStatic()},
                 {"optional", IsOptionalDeclaration()},
                 {"computed", isComputed_},
                 {"value", value_},
                 {"overloads", overloads_},
                 {"decorators", decorators_}});
}

void MethodDefinition::DumpPrefix(ir::SrcDumper *dumper) const
{
    if (compiler::HasGlobalClassParent(this)) {
        dumper->Add("function ");
        return;
    }

    if (Parent() != nullptr && Parent()->IsClassDefinition() && !Parent()->AsClassDefinition()->IsLocal()) {
        if (IsPrivate()) {
            dumper->Add("private ");
        } else if (IsProtected()) {
            dumper->Add("protected ");
        } else if (IsInternal()) {
            dumper->Add("internal ");
        } else {
            dumper->Add("public ");
        }
    }

    if (IsStatic()) {
        dumper->Add("static ");
    }

    if (IsAbstract() && !(Parent()->IsTSInterfaceBody() ||
                          (BaseOverloadMethod() != nullptr && BaseOverloadMethod()->Parent()->IsTSInterfaceBody()))) {
        dumper->Add("abstract ");
    }

    if (IsFinal()) {
        dumper->Add("final ");
    }

    if (IsNative()) {
        dumper->Add("native ");
    }

    if (IsAsync()) {
        dumper->Add("async ");
    }

    if (IsOverride()) {
        dumper->Add("override ");
    }

    if (IsGetter()) {
        dumper->Add("get ");
    } else if (IsSetter()) {
        dumper->Add("set ");
    }
}

void MethodDefinition::Dump(ir::SrcDumper *dumper) const
{
    if (compiler::HasGlobalClassParent(this) && Id()->Name().Is(compiler::Signatures::INIT_METHOD)) {
        Function()->Body()->Dump(dumper);
        return;
    }

    for (auto method : overloads_) {
        method->Dump(dumper);
        dumper->Endl();
    }

    for (auto *anno : value_->AsFunctionExpression()->Function()->Annotations()) {
        anno->Dump(dumper);
    }
    DumpPrefix(dumper);

    if (key_ != nullptr) {
        key_->Dump(dumper);
    }

    if (value_ != nullptr) {
        value_->Dump(dumper);
    }
}

void MethodDefinition::Compile(compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void MethodDefinition::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *MethodDefinition::Check(checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::VerifiedType MethodDefinition::Check(checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

MethodDefinition *MethodDefinition::Clone(ArenaAllocator *const allocator, AstNode *const parent)
{
    auto *const key = key_->Clone(allocator, nullptr)->AsExpression();
    auto *const value = value_->Clone(allocator, nullptr)->AsExpression();
    auto *const clone = allocator->New<MethodDefinition>(kind_, key, value, flags_, allocator, isComputed_);

    if (parent != nullptr) {
        clone->SetParent(parent);
    }

    key->SetParent(clone);
    value->SetParent(clone);

    for (auto *const decorator : decorators_) {
        clone->AddDecorator(decorator->Clone(allocator, clone));
    }

    clone->baseOverloadMethod_ = baseOverloadMethod_;

    for (auto *const overloads : overloads_) {
        clone->AddOverload(overloads->Clone(allocator, clone));
    }

    return clone;
}

void MethodDefinition::InitializeOverloadInfo()
{
    ES2PANDA_ASSERT(this->Function() != nullptr);

    overloadInfo_ = {this->Function()->Signature()->MinArgCount(),
                     this->Function()->Signature()->ArgCount(),
                     false,
                     this->IsDeclare(),
                     (this->Function()->Signature()->RestVar() != nullptr),
                     this->Function()->Signature()->ReturnType()->IsETSVoidType()};
}

void MethodDefinition::ResetOverloads()
{
    auto baseOverloadMethod = baseOverloadMethod_;
    baseOverloadMethod_ = nullptr;
    for (auto *overload : overloads_) {
        overload->CleanUp();
    }
    ClearOverloads();

    if ((Function() == nullptr) || !Function()->IsOverload()) {
        return;
    }

    Function()->ClearFlag(ir::ScriptFunctionFlags::OVERLOAD);
    /*
     * if this method and it's baseOverloadMethod are in two different files,
     * no need to move it to the body of baseOverloadMethod's contianing class in cleanup.
     */
    if (GetTopStatement() != baseOverloadMethod->GetTopStatement()) {
        return;
    }

    auto parent = baseOverloadMethod->Parent();
    ES2PANDA_ASSERT(parent->IsClassDefinition() || parent->IsTSInterfaceBody());
    auto &body =
        parent->IsClassDefinition() ? parent->AsClassDefinition()->Body() : parent->AsTSInterfaceBody()->Body();

    for (auto *elem : body) {
        if (elem == this) {
            return;
        }
    }

    body.emplace_back(this);
}

void MethodDefinition::CleanUp()
{
    AstNode::CleanUp();
    ResetOverloads();
}

}  // namespace ark::es2panda::ir

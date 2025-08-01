/*
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

#include "astNode.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"
#include "ir/typed.h"

namespace ark::es2panda::ir {

AstNode::AstNode(AstNode const &other)
{
    range_ = other.range_;
    type_ = other.type_;
    if (other.variable_ != nullptr) {
        variable_ = other.variable_;
    }
    flags_ = other.flags_;
    astNodeFlags_ = other.astNodeFlags_;
    // boxing_unboxing_flags_ {};  leave default value!
}

[[nodiscard]] bool AstNode::IsExported() const noexcept
{
    if (UNLIKELY(IsClassDefinition())) {
        return parent_->IsExported();
    }

    return (flags_ & ModifierFlags::EXPORT) != 0;
}

[[nodiscard]] bool AstNode::IsDefaultExported() const noexcept
{
    if (UNLIKELY(IsClassDefinition())) {
        return parent_->IsDefaultExported();
    }

    return (flags_ & ModifierFlags::DEFAULT_EXPORT) != 0;
}

[[nodiscard]] bool AstNode::IsExportedType() const noexcept
{
    if (UNLIKELY(IsClassDefinition())) {
        return this->parent_->IsExportedType();
    }

    return (flags_ & ModifierFlags::EXPORT_TYPE) != 0;
}

[[nodiscard]] bool AstNode::HasExportAlias() const noexcept
{
    if (UNLIKELY(IsClassDefinition())) {
        return parent_->HasExportAlias();
    }

    return (astNodeFlags_ & AstNodeFlags::HAS_EXPORT_ALIAS) != 0;
}

bool AstNode::IsScopeBearer() const noexcept
{
    return false;
}

varbinder::Scope *AstNode::Scope() const noexcept
{
    ES2PANDA_UNREACHABLE();
}

void AstNode::ClearScope() noexcept
{
    ES2PANDA_UNREACHABLE();
}

ir::ClassElement *AstNode::AsClassElement()
{
    ES2PANDA_ASSERT(IsMethodDefinition() || IsClassProperty() || IsClassStaticBlock());
    return reinterpret_cast<ir::ClassElement *>(this);
}

const ir::ClassElement *AstNode::AsClassElement() const
{
    ES2PANDA_ASSERT(IsMethodDefinition() || IsClassProperty() || IsClassStaticBlock());
    return reinterpret_cast<const ir::ClassElement *>(this);
}

template <typename R, typename T>
static R GetTopStatementImpl(T *self)
{
    auto iter = self;

    while (iter->Parent()) {
        iter = iter->Parent();
    }

    return reinterpret_cast<R>(iter);
}

ir::BlockStatement *AstNode::GetTopStatement()
{
    return GetTopStatementImpl<ir::BlockStatement *>(this);
}

const ir::BlockStatement *AstNode::GetTopStatement() const
{
    return GetTopStatementImpl<const ir::BlockStatement *>(this);
}

AstNode *AstNode::Clone([[maybe_unused]] ArenaAllocator *const allocator, [[maybe_unused]] AstNode *const parent)
{
    ES2PANDA_UNREACHABLE();
}

void AstNode::TransformChildrenRecursively(const NodeTransformer &cb, std::string_view transformationName)
{  // post-order, but use when you don't care about the order
    TransformChildrenRecursivelyPostorder(cb, transformationName);
}

void AstNode::TransformChildrenRecursivelyPreorder(const NodeTransformer &cb, std::string_view transformationName)
{
    TransformChildren(
        [=](AstNode *child) {
            auto *res = cb(child);
            res->TransformChildrenRecursivelyPreorder(cb, transformationName);
            return res;
        },
        transformationName);
}

void AstNode::TransformChildrenRecursivelyPostorder(const NodeTransformer &cb, std::string_view transformationName)
{
    TransformChildren(
        [=](AstNode *child) {
            child->TransformChildrenRecursivelyPostorder(cb, transformationName);
            return cb(child);
        },
        transformationName);
}

void AstNode::IterateRecursively(const NodeTraverser &cb) const
{  // pre-order, use when you don't care
    IterateRecursivelyPreorder(cb);
}

void AstNode::IterateRecursivelyPreorder(const NodeTraverser &cb) const
{
    Iterate([=](AstNode *child) {
        cb(child);
        child->IterateRecursivelyPreorder(cb);
    });
}

void AstNode::IterateRecursivelyPostorder(const NodeTraverser &cb) const
{
    Iterate([=](AstNode *child) {
        child->IterateRecursivelyPostorder(cb);
        cb(child);
    });
}

void AnyChildHelper(bool *found, const NodePredicate &cb, AstNode *ast)
{
    if (*found) {
        return;
    }

    if (cb(ast)) {
        *found = true;
        return;
    }

    ast->Iterate([=](AstNode *child) { AnyChildHelper(found, cb, child); });
}

bool AstNode::IsAnyChild(const NodePredicate &cb) const
{
    bool found = false;
    Iterate([&found, cb](AstNode *child) { AnyChildHelper(&found, cb, child); });
    return found;
}

void FindChildHelper(AstNode *&found, const NodePredicate &cb, AstNode *ast)
{
    if (found != nullptr) {
        return;
    }

    if (cb(ast)) {
        found = ast;
        return;
    }

    ast->Iterate([&found, cb](AstNode *child) { FindChildHelper(found, cb, child); });
}

AstNode *AstNode::FindChild(const NodePredicate &cb) const
{
    AstNode *found = nullptr;
    Iterate([&found, cb](AstNode *child) { FindChildHelper(found, cb, child); });
    return found;
}

varbinder::Scope *AstNode::EnclosingScope(const ir::AstNode *expr) noexcept
{
    while (expr != nullptr && !expr->IsScopeBearer()) {
        expr = expr->Parent();
    }
    return expr != nullptr ? expr->Scope() : nullptr;
}

std::string AstNode::DumpJSON() const
{
    ir::AstDumper dumper {this};
    return dumper.Str();
}

std::string AstNode::DumpEtsSrc() const
{
    ir::SrcDumper dumper {this};
    return dumper.Str();
}

std::string AstNode::DumpDecl() const
{
    ir::SrcDumper dumper {this, true};
    dumper.Run();
    return dumper.Str();
}

std::string AstNode::IsolatedDumpDecl() const
{
    ir::SrcDumper dumper {this, true, true};
    dumper.Run();
    return dumper.Str();
}

void AstNode::SetOriginalNode(AstNode *originalNode) noexcept
{
    originalNode_ = originalNode;
}

AstNode *AstNode::OriginalNode() const noexcept
{
    return originalNode_;
}

void AstNode::SetTransformedNode(std::string_view const transformationName, AstNode *transformedNode)
{
    transformedNode->SetOriginalNode(this);
    transformedNode_ = std::make_optional(std::make_pair(transformationName, transformedNode));
}

void AstNode::CleanUp()
{
    SetVariable(nullptr);
    if (IsScopeBearer()) {
        ClearScope();
    }
    if (IsTyped()) {
        this->AsTyped()->SetTsType(nullptr);
    }
}

bool AstNode::IsReadonly() const noexcept
{
    return (flags_ & ModifierFlags::READONLY) != 0;
}

// NOTE: For readonly parameter type
bool AstNode::IsReadonlyType() const noexcept
{
    return (flags_ & ModifierFlags::READONLY_PARAMETER) != 0;
}

bool AstNode::IsOptionalDeclaration() const noexcept
{
    return (flags_ & ModifierFlags::OPTIONAL) != 0;
}

bool AstNode::IsDefinite() const noexcept
{
    return (flags_ & ModifierFlags::DEFINITE) != 0;
}

bool AstNode::IsConstructor() const noexcept
{
    return (flags_ & ModifierFlags::CONSTRUCTOR) != 0;
}

bool AstNode::IsOverride() const noexcept
{
    return (flags_ & ModifierFlags::OVERRIDE) != 0;
}

AstNode *AstNode::ShallowClone(ArenaAllocator *allocator)
{
    auto clone = Construct(allocator);
    CopyTo(clone);
    return clone;
}

void AstNode::CopyTo(AstNode *other) const
{
    ES2PANDA_ASSERT(other->type_ == type_);

    other->parent_ = parent_;
    other->range_ = range_;
    other->flags_ = flags_;
    other->astNodeFlags_ = astNodeFlags_;
    other->boxingUnboxingFlags_ = boxingUnboxingFlags_;
    other->variable_ = variable_;
    other->originalNode_ = originalNode_;
    other->transformedNode_ = transformedNode_;
}

AstNode *AstNode::Construct([[maybe_unused]] ArenaAllocator *allocator)
{
    ES2PANDA_UNREACHABLE();
}
}  // namespace ark::es2panda::ir

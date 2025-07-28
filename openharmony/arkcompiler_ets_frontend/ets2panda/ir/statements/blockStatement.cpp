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

#include "blockStatement.h"

#include "compiler/core/pandagen.h"
#include "compiler/core/regScope.h"
#include "compiler/core/ETSGen.h"
#include "checker/TSchecker.h"
#include "checker/ETSchecker.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"
#include "utils/arena_containers.h"

namespace ark::es2panda::ir {
void BlockStatement::TransformChildren(const NodeTransformer &cb, std::string_view const transformationName)
{
    // This will survive pushing element to the back of statements_ in the process
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (size_t ix = 0; ix < statements_.size(); ix++) {
        if (auto *transformedNode = cb(statements_[ix]); statements_[ix] != transformedNode) {
            statements_[ix]->SetTransformedNode(transformationName, transformedNode);
            statements_[ix] = transformedNode->AsStatement();
        }
    }
}

AstNode *BlockStatement::Clone(ArenaAllocator *const allocator, AstNode *const parent)
{
    ArenaVector<Statement *> statements(allocator->Adapter());

    for (auto *statement : this->statements_) {
        statements.push_back(statement->Clone(allocator, parent)->AsStatement());
    }

    auto retVal = util::NodeAllocator::ForceSetParent<ir::BlockStatement>(allocator, allocator, std::move(statements));
    ES2PANDA_ASSERT(retVal != nullptr);
    retVal->SetParent(parent);

    return retVal;
}

void BlockStatement::Iterate(const NodeTraverser &cb) const
{
    // This will survive pushing element to the back of statements_ in the process
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (size_t ix = 0; ix < statements_.size(); ix++) {
        cb(statements_[ix]);
    }
}

void BlockStatement::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", IsProgram() ? "Program" : "BlockStatement"}, {"statements", statements_}});
}

void BlockStatement::Dump(ir::SrcDumper *dumper) const
{
    // NOTE(nsizov): trailing blocks
    if (Parent() != nullptr && (Parent()->IsBlockStatement() || Parent()->IsCallExpression())) {
        dumper->Add("{");
        if (!statements_.empty()) {
            dumper->IncrIndent();
            dumper->Endl();
        }
    }
    for (auto statement : statements_) {
        statement->Dump(dumper);
        if (statement != statements_.back()) {
            dumper->Endl();
        }
    }
    if (Parent() != nullptr && (Parent()->IsBlockStatement() || Parent()->IsCallExpression())) {
        if (!statements_.empty()) {
            dumper->DecrIndent();
            dumper->Endl();
        }
        dumper->Add("}");
    }
}

void BlockStatement::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void BlockStatement::Compile([[maybe_unused]] compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *BlockStatement::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::VerifiedType BlockStatement::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

BlockStatement *BlockStatement::Construct(ArenaAllocator *allocator)
{
    ArenaVector<Statement *> statementList(allocator->Adapter());
    return allocator->New<BlockStatement>(allocator, std::move(statementList));
}

void BlockStatement::CopyTo(AstNode *other) const
{
    auto otherImpl = other->AsBlockStatement();

    otherImpl->scope_ = scope_;
    otherImpl->statements_ = statements_;
    otherImpl->trailingBlocks_ = trailingBlocks_;

    Statement::CopyTo(other);
}

}  // namespace ark::es2panda::ir

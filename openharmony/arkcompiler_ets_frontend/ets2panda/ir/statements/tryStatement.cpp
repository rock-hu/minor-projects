/**
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

#include "tryStatement.h"

#include "checker/TSchecker.h"
#include "compiler/core/pandagen.h"
#include "compiler/core/ETSGen.h"
#include "compiler/core/dynamicContext.h"
#include "compiler/base/catchTable.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"
#include "ir/base/catchClause.h"
#include "ir/statements/blockStatement.h"

namespace ark::es2panda::ir {
void TryStatement::TransformChildren(const NodeTransformer &cb, std::string_view transformationName)
{
    if (auto *transformedNode = cb(block_); block_ != transformedNode) {
        block_->SetTransformedNode(transformationName, transformedNode);
        block_ = transformedNode->AsBlockStatement();
    }

    for (auto *&it : VectorIterationGuard(catchClauses_)) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsCatchClause();
        }
    }

    if (finalizer_ != nullptr) {
        if (auto *transformedNode = cb(finalizer_); finalizer_ != transformedNode) {
            finalizer_->SetTransformedNode(transformationName, transformedNode);
            finalizer_ = transformedNode->AsBlockStatement();
        }
    }
}

void TryStatement::Iterate(const NodeTraverser &cb) const
{
    cb(block_);

    for (auto *it : VectorIterationGuard(catchClauses_)) {
        cb(it);
    }

    if (finalizer_ != nullptr) {
        cb(finalizer_);
    }
}

void TryStatement::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TryStatement"},
                 {"block", block_},
                 {"handler", catchClauses_},
                 {"finalizer", AstDumper::Nullish(finalizer_)}});
}

void TryStatement::Dump(ir::SrcDumper *dumper) const
{
    ES2PANDA_ASSERT(block_ != nullptr);
    dumper->Add("try {");
    dumper->IncrIndent();
    dumper->Endl();
    block_->Dump(dumper);
    dumper->DecrIndent();
    dumper->Endl();
    dumper->Add("}");
    for (auto clause : catchClauses_) {
        dumper->Add(" catch ");
        clause->Dump(dumper);
    }
    if (finalizer_ != nullptr) {
        dumper->Add(" finally {");
        dumper->IncrIndent();
        dumper->Endl();
        finalizer_->Dump(dumper);
        dumper->DecrIndent();
        dumper->Endl();
        dumper->Add("}");
    }
}

bool TryStatement::HasDefaultCatchClause() const
{
    return (!catchClauses_.empty() && catchClauses_.back()->IsDefaultCatchClause());
}

void TryStatement::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void TryStatement::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *TryStatement::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::VerifiedType TryStatement::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

TryStatement::TryStatement(TryStatement const &other, ArenaAllocator *allocator)
    : Statement(other),
      catchClauses_(allocator->Adapter()),
      finalizerInsertions_(allocator->Adapter()),
      finallyCanCompleteNormally_(other.finallyCanCompleteNormally_)
{
    block_ = other.block_ == nullptr ? nullptr : other.block_->Clone(allocator, this)->AsBlockStatement();
    for (auto &cc : other.catchClauses_) {
        if (cc == nullptr) {
            catchClauses_.push_back(nullptr);
            continue;
        }
        auto *ccClone = cc->Clone(allocator, this);
        ES2PANDA_ASSERT(ccClone != nullptr);
        catchClauses_.push_back(ccClone->AsCatchClause());
    }
    if (other.finalizer_ == nullptr) {
        finalizer_ = nullptr;
    } else {
        auto *otherFinalizerClone = other.finalizer_->Clone(allocator, this);
        ES2PANDA_ASSERT(otherFinalizerClone != nullptr);
        finalizer_ = otherFinalizerClone->AsBlockStatement();
    }
    for (auto &[label, st] : other.finalizerInsertions_) {
        finalizerInsertions_.emplace_back(label, st);
    }
}

TryStatement *TryStatement::Clone(ArenaAllocator *const allocator, AstNode *const parent)
{
    auto *const clone = allocator->New<TryStatement>(*this, allocator);
    ES2PANDA_ASSERT(clone != nullptr);
    if (parent != nullptr) {
        clone->SetParent(parent);
    }

    clone->SetRange(Range());
    return clone;
}

}  // namespace ark::es2panda::ir

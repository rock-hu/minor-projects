/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ifStatement.h"

#include "compiler/base/condition.h"
#include "compiler/core/pandagen.h"
#include "ir/astDump.h"
#include "ir/statements/blockStatement.h"
#include "ir/expression.h"
#include "typescript/checker.h"

namespace panda::es2panda::ir {

void IfStatement::Iterate(const NodeTraverser &cb) const
{
    cb(test_);
    cb(consequent_);

    if (alternate_) {
        cb(alternate_);
    }
}

void IfStatement::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "IfStatement"},
                 {"test", test_},
                 {"consequent", consequent_},
                 {"alternate", AstDumper::Nullable(alternate_)}});
}

void IfStatement::Compile(compiler::PandaGen *pg) const
{
    auto *consequentEnd = pg->AllocLabel();
    compiler::Label *statementEnd = consequentEnd;

    compiler::Condition::Compile(pg, test_, consequentEnd);
    consequent_->Compile(pg);

    if (alternate_) {
        statementEnd = pg->AllocLabel();
        pg->Branch(pg->Insns().back()->Node(), statementEnd);

        pg->SetLabel(this, consequentEnd);
        alternate_->Compile(pg);
    }

    pg->SetLabel(this, statementEnd);
}

checker::Type *IfStatement::Check(checker::Checker *checker) const
{
    checker::Type *testType = test_->Check(checker);
    checker->CheckTruthinessOfType(testType, Start());
    checker->CheckTestingKnownTruthyCallableOrAwaitableType(test_, testType, consequent_);

    consequent_->Check(checker);

    if (alternate_) {
        alternate_->Check(checker);
    }

    return nullptr;
}

void IfStatement::UpdateSelf(const NodeUpdater &cb, binder::Binder *binder)
{
    test_ = std::get<ir::AstNode *>(cb(test_))->AsExpression();
    consequent_ = UpdateIfStatementChildStatement(cb, binder, consequent_);

    if (alternate_) {
        alternate_ = UpdateIfStatementChildStatement(cb, binder, alternate_);
    }
}

Statement *IfStatement::UpdateIfStatementChildStatement(const NodeUpdater &cb,
                                                        const binder::Binder *binder,
                                                        Statement *statement) const
{
    auto newStatement = cb(statement);
    if (std::holds_alternative<ir::AstNode *>(newStatement)) {
        return std::get<ir::AstNode *>(newStatement)->AsStatement();
    }

    ASSERT(std::holds_alternative<std::vector<ir::AstNode *>>(newStatement));
    ArenaVector<ir::Statement *> statements(binder->Allocator()->Adapter());
    auto newStatements = std::get<std::vector<ir::AstNode *>>(newStatement);
    for (auto *it : newStatements) {
        statements.push_back(it->AsStatement());
    }
    return binder->Allocator()->New<ir::BlockStatement>(nullptr, std::move(statements));
}

}  // namespace panda::es2panda::ir

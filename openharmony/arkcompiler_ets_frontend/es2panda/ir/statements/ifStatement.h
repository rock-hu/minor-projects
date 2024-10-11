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

#ifndef ES2PANDA_IR_STATEMENT_IF_STATEMENT_H
#define ES2PANDA_IR_STATEMENT_IF_STATEMENT_H

#include "ir/statement.h"

namespace panda::es2panda::compiler {
class PandaGen;
}  // namespace panda::es2panda::compiler

namespace panda::es2panda::checker {
class Checker;
class Type;
}  // namespace panda::es2panda::checker

namespace panda::es2panda::ir {

class Expression;

class IfStatement : public Statement {
public:
    explicit IfStatement(Expression *test, Statement *consequent, Statement *alternate)
        : Statement(AstNodeType::IF_STATEMENT),
          test_(test),
          consequent_(consequent),
          alternate_(alternate)
    {
    }

    const Expression *Test() const
    {
        return test_;
    }

    const Statement *Consequent() const
    {
        return consequent_;
    }

    const Statement *Alternate() const
    {
        return alternate_;
    }
    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Compile(compiler::PandaGen *pg) const override;
    checker::Type *Check(checker::Checker *checker) const override;
    void UpdateSelf(const NodeUpdater &cb, binder::Binder *binder) override;

protected:
    Statement *UpdateIfStatementChildStatement(const NodeUpdater &cb,
                                               const binder::Binder *binder,
                                               Statement *statement) const;

    Expression *test_;
    Statement *consequent_;
    Statement *alternate_;
};

}  // namespace panda::es2panda::ir

#endif

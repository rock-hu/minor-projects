/**
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

#ifndef ES2PANDA_IR_STATEMENT_WHILE_STATEMENT_H
#define ES2PANDA_IR_STATEMENT_WHILE_STATEMENT_H

#include "ir/statements/loopStatement.h"

namespace ark::es2panda::varbinder {
class LoopScope;
}  // namespace ark::es2panda::varbinder

namespace ark::es2panda::ir {
class Expression;

class WhileStatement : public LoopStatement {
public:
    explicit WhileStatement(Expression *test, Statement *body)
        : LoopStatement(AstNodeType::WHILE_STATEMENT), test_(test), body_(body)
    {
    }

    const Expression *Test() const
    {
        return test_;
    }

    Expression *Test()
    {
        return test_;
    }

    const Statement *Body() const
    {
        return body_;
    }

    Statement *Body()
    {
        return body_;
    }

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;

    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    void Compile([[maybe_unused]] compiler::ETSGen *etsg) const override;
    checker::Type *Check([[maybe_unused]] checker::TSChecker *checker) override;
    checker::Type *Check([[maybe_unused]] checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

private:
    Expression *test_;
    Statement *body_;
};
}  // namespace ark::es2panda::ir

#endif /* WHILE_STATEMENT_H */

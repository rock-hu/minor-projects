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

#ifndef ES2PANDA_IR_STATEMENT_VARIABLE_DECLARATOR_H
#define ES2PANDA_IR_STATEMENT_VARIABLE_DECLARATOR_H

#include <ir/expression.h>
#include <ir/statement.h>

namespace panda::es2panda::compiler {
class PandaGen;
}  // namespace panda::es2panda::compiler

namespace panda::es2panda::checker {
class Checker;
class Type;
}  // namespace panda::es2panda::checker

namespace panda::es2panda::ir {

class Expression;

class VariableDeclarator : public Statement {
public:
    explicit VariableDeclarator(Expression *ident)
        : Statement(AstNodeType::VARIABLE_DECLARATOR), id_(ident), init_(nullptr)
    {
    }

    explicit VariableDeclarator(Expression *ident, Expression *init)
        : Statement(AstNodeType::VARIABLE_DECLARATOR), id_(ident), init_(init)
    {
    }

    Expression *Init()
    {
        return init_;
    }

    const Expression *Init() const
    {
        return init_;
    }

    Expression *Id()
    {
        return id_;
    }

    const Expression *Id() const
    {
        return id_;
    }

    bool Definite() const
    {
        return definite_;
    }

    void SetDefinite(bool definite)
    {
        definite_ = definite;
    }

    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Compile(compiler::PandaGen *pg) const override;
    checker::Type *Check(checker::Checker *checker) const override;
    void UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder) override;

private:
    Expression *id_;
    Expression *init_;
    // TODO(xucheng): don't use now, it will be used in type checker later
    bool definite_ {false};
};

}  // namespace panda::es2panda::ir

#endif

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

#ifndef ES2PANDA_IR_STATEMENT_VARIABLE_DECLARATION_H
#define ES2PANDA_IR_STATEMENT_VARIABLE_DECLARATION_H

#include <ir/statement.h>

namespace panda::es2panda::compiler {
class PandaGen;
}  // namespace panda::es2panda::compiler

namespace panda::es2panda::checker {
class Checker;
class Type;
}  // namespace panda::es2panda::checker

namespace panda::es2panda::ir {

class VariableDeclarator;

class VariableDeclaration : public Statement {
public:
    enum class VariableDeclarationKind { CONST, LET, VAR };

    explicit VariableDeclaration(VariableDeclarationKind kind, ArenaVector<VariableDeclarator *> &&declarators,
                                 bool declare)
        : Statement(AstNodeType::VARIABLE_DECLARATION),
          kind_(kind),
          declarators_(std::move(declarators)),
          declare_(declare)
    {
    }

    const ArenaVector<VariableDeclarator *> &Declarators() const
    {
        return declarators_;
    }

    VariableDeclarationKind Kind() const
    {
        return kind_;
    }

    bool Declare() const
    {
        return declare_;
    }

    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Compile(compiler::PandaGen *pg) const override;
    checker::Type *Check(checker::Checker *checker) const override;
    void UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder) override;

private:
    VariableDeclarationKind kind_;
    ArenaVector<VariableDeclarator *> declarators_;
    bool declare_;
};

}  // namespace panda::es2panda::ir

#endif

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

#ifndef ES2PANDA_IR_MODULE_IMPORT_DECLARATION_H
#define ES2PANDA_IR_MODULE_IMPORT_DECLARATION_H

#include <ir/module/assertClause.h>
#include <ir/statement.h>
#include <util/ustring.h>

namespace panda::es2panda::compiler {
class PandaGen;
}  // namespace panda::es2panda::compiler

namespace panda::es2panda::checker {
class Checker;
class Type;
}  // namespace panda::es2panda::checker

namespace panda::es2panda::ir {

class StringLiteral;

class ImportDeclaration : public Statement {
public:
    explicit ImportDeclaration(StringLiteral *source, ArenaVector<AstNode *> &&specifiers,
                               AssertClause *assertClause, bool isType, bool isLazy)
        : Statement(AstNodeType::IMPORT_DECLARATION),
          source_(source),
          specifiers_(std::move(specifiers)),
          assertClause_(assertClause),
          isType_(isType),
          isLazy_(isLazy)
    {
    }

    const StringLiteral *Source() const
    {
        return source_;
    }

    const ArenaVector<AstNode *> &Specifiers() const
    {
        return specifiers_;
    }

    bool IsType() const
    {
        return isType_;
    }

    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    checker::Type *Check([[maybe_unused]] checker::Checker *checker) const override;
    void UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder) override;

private:
    StringLiteral *source_;
    ArenaVector<AstNode *> specifiers_;
    AssertClause *assertClause_;
    bool isType_;
    bool isLazy_ { false };
};

}  // namespace panda::es2panda::ir

#endif

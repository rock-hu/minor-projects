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

#ifndef ES2PANDA_IR_STATEMENT_LABELLED_STATEMENT_H
#define ES2PANDA_IR_STATEMENT_LABELLED_STATEMENT_H

#include "ir/statement.h"
#include "util/ustring.h"

namespace ark::es2panda::checker {
class ETSAnalyzer;
}  // namespace ark::es2panda::checker

namespace ark::es2panda::ir {
class Identifier;

class LabelledStatement : public Statement {
public:
    explicit LabelledStatement(Identifier *ident, Statement *body)
        : Statement(AstNodeType::LABELLED_STATEMENT), ident_(ident), body_(body)
    {
    }

    // NOTE (csabahurton): these friend relationships can be removed once there are getters for private fields
    friend class checker::ETSAnalyzer;

    const Statement *Body() const
    {
        return body_;
    }

    const Identifier *Ident() const
    {
        return ident_;
    }

    Identifier *Ident()
    {
        return ident_;
    }

    const ir::AstNode *GetReferencedStatement() const;

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;

    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile(compiler::PandaGen *pg) const override;
    void Compile(compiler::ETSGen *etsg) const override;
    checker::Type *Check(checker::TSChecker *checker) override;
    checker::Type *Check(checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

private:
    Identifier *ident_;
    Statement *body_;
};
}  // namespace ark::es2panda::ir

#endif

/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_IR_TS_CLASS_IMPLEMENTS_H
#define ES2PANDA_IR_TS_CLASS_IMPLEMENTS_H

#include "ir/expression.h"

namespace ark::es2panda::ir {
class TSTypeParameterInstantiation;

class TSClassImplements : public Expression {
public:
    explicit TSClassImplements(Expression *expression, TSTypeParameterInstantiation *typeParameters)
        : Expression(AstNodeType::TS_CLASS_IMPLEMENTS), expression_(expression), typeParameters_(typeParameters)
    {
    }

    explicit TSClassImplements(Expression *expression)
        : Expression(AstNodeType::TS_CLASS_IMPLEMENTS), expression_(expression)
    {
    }

    Expression *Expr()
    {
        return expression_;
    }

    const Expression *Expr() const
    {
        return expression_;
    }

    const TSTypeParameterInstantiation *TypeParameters() const
    {
        return typeParameters_;
    }

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;
    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    void Compile(compiler::ETSGen *etsg) const override;
    checker::Type *Check([[maybe_unused]] checker::TSChecker *checker) override;
    checker::Type *Check([[maybe_unused]] checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

private:
    Expression *expression_;
    TSTypeParameterInstantiation *typeParameters_ {};
};
}  // namespace ark::es2panda::ir

#endif

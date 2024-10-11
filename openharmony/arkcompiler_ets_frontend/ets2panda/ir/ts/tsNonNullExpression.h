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

#ifndef ES2PANDA_IR_TS_NON_NULL_EXPRESSION_H
#define ES2PANDA_IR_TS_NON_NULL_EXPRESSION_H

#include "ir/expression.h"

namespace ark::es2panda::checker {
class ETSAnalyzer;
}  // namespace ark::es2panda::checker
namespace ark::es2panda::ir {
class TSNonNullExpression : public Expression {
public:
    explicit TSNonNullExpression(Expression *expr) : Expression(AstNodeType::TS_NON_NULL_EXPRESSION), expr_(expr) {}

    // NOTE (vivienvoros): these friend relationships can be removed once there are getters for private fields
    friend class checker::ETSAnalyzer;

    const Expression *Expr() const noexcept
    {
        return expr_;
    }

    Expression *Expr() noexcept
    {
        return expr_;
    }

    void SetExpr(Expression *expr) noexcept
    {
        expr_ = expr;
    }

    checker::Type *OriginalType() const noexcept
    {
        return originalType_;
    }

    void SetOriginalType(checker::Type *type) noexcept
    {
        originalType_ = type;
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

    [[nodiscard]] TSNonNullExpression *Clone(ArenaAllocator *allocator, AstNode *parent) override;

private:
    Expression *expr_;
    checker::Type *originalType_ = nullptr;
};
}  // namespace ark::es2panda::ir

#endif

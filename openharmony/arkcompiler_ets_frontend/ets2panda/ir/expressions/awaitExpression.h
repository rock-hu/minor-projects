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

#ifndef ES2PANDA_IR_EXPRESSION_AWAIT_EXPRESSION_H
#define ES2PANDA_IR_EXPRESSION_AWAIT_EXPRESSION_H

#include "ir/expression.h"

namespace ark::es2panda::checker {
class ETSAnalyzer;
}  // namespace ark::es2panda::checker

namespace ark::es2panda::ir {
class AwaitExpression : public Expression {
public:
    AwaitExpression() = delete;
    ~AwaitExpression() override = default;

    NO_COPY_SEMANTIC(AwaitExpression);
    NO_MOVE_SEMANTIC(AwaitExpression);

    explicit AwaitExpression(Expression *argument) : Expression(AstNodeType::AWAIT_EXPRESSION), argument_(argument) {}

    // NOTE (csabahurton): friend relationship can be removed once there are getters for private fields
    friend class checker::ETSAnalyzer;

    [[nodiscard]] const Expression *Argument() const noexcept
    {
        return argument_;
    }

    [[nodiscard]] AwaitExpression *Clone(ArenaAllocator *allocator, AstNode *parent) override;

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
    Expression *argument_;
};
}  // namespace ark::es2panda::ir

#endif

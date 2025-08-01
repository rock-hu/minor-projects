/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_IR_EXPRESSION_LITERAL_NULL_LITERAL_H
#define ES2PANDA_IR_EXPRESSION_LITERAL_NULL_LITERAL_H

#include "ir/expressions/literal.h"

namespace ark::es2panda::ir {
class NullLiteral : public Literal {
public:
    ~NullLiteral() override = default;

    NO_COPY_SEMANTIC(NullLiteral);
    NO_MOVE_SEMANTIC(NullLiteral);

    explicit NullLiteral() : Literal(AstNodeType::NULL_LITERAL) {}

    [[nodiscard]] NullLiteral *Clone(ArenaAllocator *allocator, AstNode *parent) override;

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;
    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile(compiler::PandaGen *pg) const override;
    void Compile(compiler::ETSGen *etsg) const override;
    checker::Type *Check(checker::TSChecker *checker) override;
    checker::VerifiedType Check(checker::ETSChecker *checker) override;

    std::string ToString() const override
    {
        return std::string {"null"};
    }

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }
};
}  // namespace ark::es2panda::ir

#endif

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

#ifndef ES2PANDA_IR_TS_TYPE_REFERENCE_H
#define ES2PANDA_IR_TS_TYPE_REFERENCE_H

#include "ir/typeNode.h"

namespace ark::es2panda::varbinder {
class Variable;
}  // namespace ark::es2panda::varbinder

namespace ark::es2panda::ir {
class TSTypeParameterInstantiation;

class TSTypeReference : public TypeNode {
public:
    explicit TSTypeReference(Expression *typeName, TSTypeParameterInstantiation *typeParams)
        : TypeNode(AstNodeType::TS_TYPE_REFERENCE), typeName_(typeName), typeParams_(typeParams)
    {
    }

    const TSTypeParameterInstantiation *TypeParams() const
    {
        return typeParams_;
    }

    const Expression *TypeName() const
    {
        return typeName_;
    }

    ir::Identifier *BaseName() const;

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;
    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    void Compile(compiler::ETSGen *etsg) const override;
    checker::Type *Check([[maybe_unused]] checker::TSChecker *checker) override;
    checker::Type *GetType([[maybe_unused]] checker::TSChecker *checker) override;
    checker::Type *Check([[maybe_unused]] checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

private:
    Expression *typeName_;
    TSTypeParameterInstantiation *typeParams_;
};
}  // namespace ark::es2panda::ir

#endif

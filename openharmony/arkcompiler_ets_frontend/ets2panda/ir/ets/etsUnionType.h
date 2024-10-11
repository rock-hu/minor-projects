/*
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

#ifndef ES2PANDA_IR_ETS_UNION_TYPE_H
#define ES2PANDA_IR_ETS_UNION_TYPE_H

#include "ir/typeNode.h"

namespace ark::es2panda::ir {
class ETSUnionType : public TypeNode {
public:
    explicit ETSUnionType(ArenaVector<TypeNode *> &&types)
        : TypeNode(AstNodeType::ETS_UNION_TYPE), types_(std::move(types))
    {
    }

    const ArenaVector<TypeNode *> &Types() const
    {
        return types_;
    }

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;
    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    checker::Type *Check([[maybe_unused]] checker::TSChecker *checker) override;
    checker::Type *Check([[maybe_unused]] checker::ETSChecker *checker) override;
    checker::Type *GetType([[maybe_unused]] checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

    [[nodiscard]] ETSUnionType *Clone(ArenaAllocator *allocator, AstNode *parent) override;

private:
    ArenaVector<TypeNode *> types_;
};
}  // namespace ark::es2panda::ir

#endif

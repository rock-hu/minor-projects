/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_IR_ETS_KEYOF_TYPE_H
#define ES2PANDA_IR_ETS_KEYOF_TYPE_H

#include "ir/typeNode.h"

namespace ark::es2panda::ir {
class ETSKeyofType : public TypeNode {
public:
    explicit ETSKeyofType(TypeNode *type, ArenaAllocator *const allocator)
        : TypeNode(AstNodeType::ETS_KEYOF_TYPE, allocator), type_(type)
    {
    }

    ir::TypeNode *GetTypeRef() const
    {
        return type_;
    }

    void TransformChildren(const NodeTransformer &cb, std::string_view transformationName) override;
    void Iterate(const NodeTraverser &cb) const override;
    void Dump(ir::AstDumper *dumper) const override;
    void Dump(ir::SrcDumper *dumper) const override;
    void Compile([[maybe_unused]] compiler::PandaGen *pg) const override;
    checker::Type *Check([[maybe_unused]] checker::TSChecker *checker) override;
    checker::VerifiedType Check([[maybe_unused]] checker::ETSChecker *checker) override;
    checker::Type *GetType([[maybe_unused]] checker::ETSChecker *checker) override;

    void Accept(ASTVisitorT *v) override
    {
        v->Accept(this);
    }

    [[nodiscard]] ETSKeyofType *Clone(ArenaAllocator *allocator, AstNode *parent) override;

private:
    TypeNode *type_;
};
}  // namespace ark::es2panda::ir

#endif
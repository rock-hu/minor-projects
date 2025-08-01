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

#ifndef ES2PANDA_IR_TYPE_NODE_H
#define ES2PANDA_IR_TYPE_NODE_H

#include "ir/expression.h"
#include "ir/annotationAllowed.h"

namespace ark::es2panda::checker {
class TSChecker;
class Type;
}  // namespace ark::es2panda::checker

namespace ark::es2panda::ir {
class TypeNode : public AnnotationAllowed<Expression> {
public:
    TypeNode() = delete;
    ~TypeNode() override = default;

    NO_COPY_SEMANTIC(TypeNode);
    NO_MOVE_SEMANTIC(TypeNode);

    [[nodiscard]] bool IsTypeNode() const noexcept override
    {
        return true;
    }

    [[nodiscard]] virtual checker::Type *GetType([[maybe_unused]] checker::TSChecker *checker)
    {
        return nullptr;
    }

    [[nodiscard]] virtual checker::Type *GetType([[maybe_unused]] checker::ETSChecker *checker)
    {
        return nullptr;
    }

    [[nodiscard]] TypeNode *Clone(ArenaAllocator *allocator, AstNode *parent) override;

    void CopyTo(AstNode *other) const override;

protected:
    explicit TypeNode(AstNodeType const type, ArenaAllocator *const allocator)
        : AnnotationAllowed<Expression>(type, allocator)
    {
    }

    explicit TypeNode(AstNodeType const type, ModifierFlags const flags, ArenaAllocator *const allocator)
        : AnnotationAllowed<Expression>(type, flags, allocator)
    {
    }
};
}  // namespace ark::es2panda::ir

#endif /* ES2PANDA_IR_TYPE_NODE_H */

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

#ifndef ES2PANDA_UTIL_INCLUDE_ETS_NEW_MULTI_DIV_ARRAY_INSTANCE_EXPRESSION_BUILDER
#define ES2PANDA_UTIL_INCLUDE_ETS_NEW_MULTI_DIV_ARRAY_INSTANCE_EXPRESSION_BUILDER

#include "ir/ets/etsNewMultiDimArrayInstanceExpression.h"
#include "mem/arena_allocator.h"
#include "astBuilder.h"

namespace ark::es2panda::ir {

class ETSNewMultiDivArrayInstanceExpressionBuilder : public AstBuilder<ETSNewMultiDimArrayInstanceExpression> {
public:
    explicit ETSNewMultiDivArrayInstanceExpressionBuilder(ark::ArenaAllocator *allocator)
        : AstBuilder(allocator), dimension_(allocator->Adapter())
    {
    }

    ETSNewMultiDivArrayInstanceExpressionBuilder &SetTypeReference(TypeNode *typeReference)
    {
        typeReference_ = typeReference;
        return *this;
    }

    ETSNewMultiDivArrayInstanceExpressionBuilder &AddDimension(Expression *dimension)
    {
        dimension_.emplace_back(dimension);
        return *this;
    }

    ETSNewMultiDimArrayInstanceExpression *Build()
    {
        auto node = AllocNode(typeReference_, std::move(dimension_));
        return node;
    }

private:
    ir::TypeNode *typeReference_ {};
    ArenaVector<ir::Expression *> dimension_;
};

}  // namespace ark::es2panda::ir
#endif  // ES2PANDA_UTIL_INCLUDE_ETS_NEW_MULTI_DIV_ARRAY_INSTANCE_EXPRESSION_BUILDER
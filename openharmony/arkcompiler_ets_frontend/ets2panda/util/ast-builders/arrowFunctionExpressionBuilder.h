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

#ifndef ES2PANDA_UTIL_INCLUDE_ARROW_FUNCTION_EXPRESSION_BUILDER
#define ES2PANDA_UTIL_INCLUDE_ARROW_FUNCTION_EXPRESSION_BUILDER

#include "mem/arena_allocator.h"
#include "astBuilder.h"
#include "ir/expressions/arrowFunctionExpression.h"

namespace ark::es2panda::ir {

class ArrowFunctionExpressionBuilder : public AstBuilder<ir::ArrowFunctionExpression> {
public:
    explicit ArrowFunctionExpressionBuilder(ark::ArenaAllocator *allocator) : AstBuilder(allocator) {}

    ArrowFunctionExpressionBuilder &SetFunction(ScriptFunction *func)
    {
        func_ = func;
        return *this;
    }

    ArrowFunctionExpression *Build()
    {
        auto *node = AllocNode(func_);
        return node;
    }

private:
    ScriptFunction *func_ {};
};

}  // namespace ark::es2panda::ir
#endif  // ES2PANDA_UTIL_INCLUDE_ARROW_FUNCTION_EXPRESSION_BUILDER
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

#ifndef ES2PANDA_UTIL_INCLUDE_ASSERT_STATEMENT_BUILDER
#define ES2PANDA_UTIL_INCLUDE_ASSERT_STATEMENT_BUILDER

#include "mem/arena_allocator.h"
#include "astBuilder.h"
#include "ir/statements/assertStatement.h"

namespace ark::es2panda::ir {

class AssertStatementBuilder : public AstBuilder<ir::AssertStatement> {
public:
    explicit AssertStatementBuilder(ark::ArenaAllocator *allocator) : AstBuilder(allocator) {}

    AssertStatementBuilder &SetTest(Expression *test)
    {
        test_ = test;
        return *this;
    }

    AssertStatementBuilder &SetSecond(Expression *second)
    {
        second_ = second;
        return *this;
    }

    AssertStatement *Build()
    {
        auto *node = AllocNode(test_, second_);
        return node;
    }

private:
    Expression *test_ {};
    Expression *second_ {};
};

}  // namespace ark::es2panda::ir
#endif  // ES2PANDA_UTIL_INCLUDE_ASSERT_STATEMENT_BUILDER
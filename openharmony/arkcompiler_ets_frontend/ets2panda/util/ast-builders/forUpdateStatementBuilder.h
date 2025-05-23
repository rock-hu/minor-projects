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

#ifndef ES2PANDA_UTIL_INCLUDE_FOR_UPDATE_STATEMENT_BUILDER
#define ES2PANDA_UTIL_INCLUDE_FOR_UPDATE_STATEMENT_BUILDER

#include "mem/arena_allocator.h"
#include "astBuilder.h"
#include "ir/statements/forUpdateStatement.h"

namespace ark::es2panda::ir {

class ForUpdateStatementBuilder : public AstBuilder<ir::ForUpdateStatement> {
public:
    explicit ForUpdateStatementBuilder(ark::ArenaAllocator *allocator) : AstBuilder(allocator) {}

    ForUpdateStatementBuilder &SetInit(AstNode *init)
    {
        init_ = init;
        return *this;
    }

    ForUpdateStatementBuilder &SetTest(Expression *test)
    {
        test_ = test;
        return *this;
    }

    ForUpdateStatementBuilder &SetBody(Statement *body)
    {
        body_ = body;
        return *this;
    }

    ForUpdateStatementBuilder &SetUpdate(Expression *update)
    {
        update_ = update;
        return *this;
    }

    ForUpdateStatement *Build()
    {
        auto *node = AllocNode(init_, test_, update_, body_);
        return node;
    }

private:
    AstNode *init_ {};
    Expression *test_ {};
    Expression *update_ {};
    Statement *body_ {};
};

}  // namespace ark::es2panda::ir
#endif  // ES2PANDA_UTIL_INCLUDE_FOR_UPDATE_STATEMENT_BUILDER
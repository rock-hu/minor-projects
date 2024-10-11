/**
 * Copyright (c) 2021 - 2023 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_IR_EXPRESSION_LITERAL_H
#define ES2PANDA_IR_EXPRESSION_LITERAL_H

#include "ir/expression.h"

namespace ark::es2panda::ir {
class Literal : public Expression {
public:
    Literal() = delete;
    ~Literal() override = default;

    NO_COPY_SEMANTIC(Literal);
    NO_MOVE_SEMANTIC(Literal);

    [[nodiscard]] bool IsLiteral() const noexcept override
    {
        return true;
    }

protected:
    explicit Literal(AstNodeType const type) : Expression(type) {}
};
}  // namespace ark::es2panda::ir

#endif

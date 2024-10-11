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

#include "expression.h"
#include "typeNode.h"

namespace ark::es2panda::ir {

AnnotatedExpression::AnnotatedExpression(AnnotatedExpression const &other, ArenaAllocator *const allocator)
    : Annotated<Expression>(static_cast<Annotated<Expression> const &>(other))
{
    if (auto *const annotation = other.TypeAnnotation(); annotation != nullptr) {
        SetTsTypeAnnotation(annotation->Clone(allocator, this)->AsTypeNode());
    }
}

}  // namespace ark::es2panda::ir

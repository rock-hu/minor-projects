/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "numberType.h"

#include <binder/variable.h>
#include <typescript/types/enumType.h>

namespace panda::es2panda::checker {

void NumberType::ToString(std::stringstream &ss) const
{
    ss << "number";
}

void NumberType::Identical(TypeRelation *relation, Type *other)
{
    if (other->IsNumberType()) {
        relation->Result(true);
    }
}

void NumberType::AssignmentTarget(TypeRelation *relation, Type *source)
{
    if (source->IsNumberLiteralType()) {
        relation->Result(true);
    } else if (source->IsEnumType()) {
        const EnumType *sourceEnumType = source->AsEnumType();
        const binder::EnumVariable *enumVar = sourceEnumType->EnumVar();

        if (std::holds_alternative<double>(enumVar->Value())) {
            relation->Result(true);
        }
    }
}

TypeFacts NumberType::GetTypeFacts() const
{
    return TypeFacts::NUMBER_FACTS;
}

Type *NumberType::Instantiate([[maybe_unused]] ArenaAllocator *allocator, [[maybe_unused]] TypeRelation *relation,
                              [[maybe_unused]] GlobalTypesHolder *globalTypes)
{
    return this;
}

}  // namespace panda::es2panda::checker

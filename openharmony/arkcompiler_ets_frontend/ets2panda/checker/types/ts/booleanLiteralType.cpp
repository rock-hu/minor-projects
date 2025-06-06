/**
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

#include "booleanLiteralType.h"

namespace ark::es2panda::checker {
void BooleanLiteralType::ToString(std::stringstream &ss, [[maybe_unused]] bool precise) const
{
    if (value_) {
        ss << "true";
        return;
    }

    ss << "false";
}

void BooleanLiteralType::ToStringAsSrc(std::stringstream &ss) const
{
    ss << "boolean";
}

void BooleanLiteralType::Identical(TypeRelation *relation, Type *other)
{
    if (other->IsBooleanLiteralType()) {
        relation->Result(other->AsBooleanLiteralType()->Value());
    }
}

void BooleanLiteralType::AssignmentTarget([[maybe_unused]] TypeRelation *relation, [[maybe_unused]] Type *source) {}

TypeFacts BooleanLiteralType::GetTypeFacts() const
{
    return value_ ? TypeFacts::TRUE_FACTS : TypeFacts::FALSE_FACTS;
}

Type *BooleanLiteralType::Instantiate([[maybe_unused]] ArenaAllocator *allocator,
                                      [[maybe_unused]] TypeRelation *relation,
                                      [[maybe_unused]] GlobalTypesHolder *globalTypes)
{
    return this;
}
}  // namespace ark::es2panda::checker

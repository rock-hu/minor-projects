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

#include "unknownType.h"

namespace panda::es2panda::checker {

void UnknownType::ToString(std::stringstream &ss) const
{
    ss << "unknown";
}

TypeFacts UnknownType::GetTypeFacts() const
{
    return TypeFacts::ALL;
}

void UnknownType::Identical(TypeRelation *relation, Type *other)
{
    if (other->IsUnknownType()) {
        relation->Result(true);
    }
}

void UnknownType::AssignmentTarget([[maybe_unused]] TypeRelation *relation, [[maybe_unused]] Type *source)
{
    relation->Result(true);
}

Type *UnknownType::Instantiate([[maybe_unused]] ArenaAllocator *allocator, [[maybe_unused]] TypeRelation *relation,
                               [[maybe_unused]] GlobalTypesHolder *globalTypes)
{
    return this;
}

}  // namespace panda::es2panda::checker

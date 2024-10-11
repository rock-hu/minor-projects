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

#ifndef ES2PANDA_COMPILER_TYPESCRIPT_TYPES_FUNCTION_TYPE_H
#define ES2PANDA_COMPILER_TYPESCRIPT_TYPES_FUNCTION_TYPE_H

#include <unordered_set>
#include "objectType.h"

namespace panda::es2panda::checker {

class FunctionType : public ObjectType {
public:
    explicit FunctionType(ObjectDescriptor *desc) : ObjectType(ObjectType::ObjectTypeKind::FUNCTION, desc) {}

    void ToString(std::stringstream &ss) const override;
    TypeFacts GetTypeFacts() const override;
    Type *Instantiate(ArenaAllocator *allocator, TypeRelation *relation, GlobalTypesHolder *globalTypes) override;
};

}  // namespace panda::es2panda::checker

#endif /* TYPESCRIPT_TYPES_FUNCTION_TYPE_H */

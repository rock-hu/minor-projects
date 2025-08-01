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

#ifndef ES2PANDA_UTIL_INCLUDE_ETS_TYPE_REFERENCE_BUILDER
#define ES2PANDA_UTIL_INCLUDE_ETS_TYPE_REFERENCE_BUILDER

#include "ir/ets/etsTypeReference.h"
#include "mem/arena_allocator.h"
#include "astBuilder.h"

namespace ark::es2panda::ir {

class ETSTypeReferenceBuilder : public AstBuilder<ir::ETSTypeReference> {
public:
    explicit ETSTypeReferenceBuilder(ark::ArenaAllocator *allocator)
        : AstBuilder(allocator), body_(Allocator()->Adapter())
    {
    }

    ETSTypeReferenceBuilder &SetETSTypeReferencePart(ETSTypeReferencePart *typeReferencePart)
    {
        etsTypeReferencePart_ = typeReferencePart;
        return *this;
    }

    ETSTypeReference *Build()
    {
        auto node = AllocNode(etsTypeReferencePart_, Allocator());
        return node;
    }

private:
    ArenaVector<AstNode *> body_;
    ETSTypeReferencePart *etsTypeReferencePart_ = nullptr;
};

}  // namespace ark::es2panda::ir
#endif  // ES2PANDA_UTIL_INCLUDE_ETS_TYPE_REFERENCE_BUILDER
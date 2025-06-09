/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "castingContext.h"
#include "checker/types/type.h"
#include "checker/ETSchecker.h"

namespace ark::es2panda::checker {
CastingContext::CastingContext(TypeRelation *relation, const diagnostic::DiagnosticKind &diagKind,
                               const util::DiagnosticMessageParams &list, ConstructorData &&data)
{
    flags_ |= data.extraFlags;

    const SavedTypeRelationFlagsContext savedTypeRelationFlags(relation, flags_);
    relation->SetNode(data.node);

    const bool isLegalBoxedPrimitiveConversion = relation->IsLegalBoxedPrimitiveConversion(data.target, data.source);
    relation->Result(false);
    if (!relation->IsSupertypeOf(data.target, data.source) && !isLegalBoxedPrimitiveConversion) {
        relation->IsCastableTo(data.source, data.target);
        // #22954 string comparison
        if (!relation->IsTrue() && data.source->ToString() == data.target->ToString()) {
            relation->Result(true);
        }
        if (!relation->IsTrue() && (flags_ & TypeRelationFlag::NO_THROW) == 0) {
            relation->RaiseError(diagKind, list, data.pos);
        }
    }

    if (isLegalBoxedPrimitiveConversion && !relation->IsTrue()) {
        auto *const checker = relation->GetChecker()->AsETSChecker();
        Type *sourceUnboxedType = checker->MaybeUnboxType(data.source);
        relation->GetNode()->AddBoxingUnboxingFlags(checker->GetUnboxingFlag(sourceUnboxedType));
        relation->GetNode()->AddBoxingUnboxingFlags(checker->GetBoxingFlag(data.target));
    }

    uncheckedCast_ = relation->UncheckedCast();
    relation->SetNode(nullptr);
}

bool CastingContext::UncheckedCast() const noexcept
{
    return uncheckedCast_;
}
}  // namespace ark::es2panda::checker

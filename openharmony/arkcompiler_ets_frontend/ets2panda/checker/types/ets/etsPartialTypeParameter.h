/*
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

#ifndef ES2PANDA_COMPILER_CHECKER_TYPES_ETS_PARTIAL_TYPE_PARAMETER_TYPE_H
#define ES2PANDA_COMPILER_CHECKER_TYPES_ETS_PARTIAL_TYPE_PARAMETER_TYPE_H

#include "checker/types/ets/etsTypeParameter.h"

namespace ark::es2panda::checker {
class ETSChecker;

class ETSPartialTypeParameter : public Type {
public:
    ETSPartialTypeParameter() = delete;
    ~ETSPartialTypeParameter() override = default;

    NO_COPY_SEMANTIC(ETSPartialTypeParameter);
    NO_MOVE_SEMANTIC(ETSPartialTypeParameter);

    explicit ETSPartialTypeParameter(ETSTypeParameter *typeParam, ETSChecker *checker);

    ETSTypeParameter *GetUnderlying() const
    {
        return typeParameter_;
    }

    void Identical(TypeRelation *relation, Type *other) override;
    void AssignmentTarget(TypeRelation *relation, Type *source) override;
    bool AssignmentSource(TypeRelation *relation, Type *target) override;
    void Cast(TypeRelation *relation, Type *target) override;
    void CastTarget(TypeRelation *relation, Type *source) override;
    void IsSupertypeOf(TypeRelation *relation, Type *source) override;
    void IsSubtypeOf(TypeRelation *relation, Type *target) override;
    Type *Substitute(TypeRelation *relation, const Substitution *substitution) override;

    void ToString(std::stringstream &ss, bool precise) const override;
    void ToAssemblerType(std::stringstream &ss) const override;
    void ToDebugInfoType(std::stringstream &ss) const override;

    ETSPartialTypeParameter *Instantiate(ArenaAllocator *allocator, TypeRelation *relation,
                                         GlobalTypesHolder *globalTypes) override;

private:
    ETSChecker *const checker_;
    ETSTypeParameter *const typeParameter_;
};
}  // namespace ark::es2panda::checker

#endif

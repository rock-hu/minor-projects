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

#ifndef ES2PANDA_GENERIC_BRIDGES_LOWERING_H
#define ES2PANDA_GENERIC_BRIDGES_LOWERING_H

#include "compiler/lowering/phase.h"

namespace ark::es2panda::compiler {

class GenericBridgesPhase : public Phase {
public:
    std::string_view Name() const override
    {
        return "CreateGenericBridges";
    }

    bool Perform(public_lib::Context *ctx, parser::Program *program) override;

private:
    struct Substitutions {
        checker::Substitution *derivedSubstitutions = nullptr;
        checker::Substitution *baseConstraints = nullptr;
        checker::Substitution *derivedConstraints = nullptr;
    };

    ir::ClassDefinition *ProcessClassDefinition(ir::ClassDefinition *classDefinition) const;

    void CreateGenericBridges(ir::ClassDefinition const *classDefinition, Substitutions &substitutions) const;

    void MaybeAddGenericBridges(ir::ClassDefinition const *classDefinition, ir::MethodDefinition *baseMethod,
                                ir::MethodDefinition *derivedMethod, Substitutions const &substitutions) const;

    void ProcessScriptFunction(ir::ClassDefinition const *classDefinition, ir::ScriptFunction *baseFunction,
                               ir::MethodDefinition *derivedMethod, Substitutions const &substitutions) const;

    void AddGenericBridge(ir::ClassDefinition const *classDefinition, ir::MethodDefinition *methodDefinition,
                          checker::Signature const *baseSignature, ir::ScriptFunction const *derivedFunction) const;

    std::string CreateMethodDefinitionString(ir::ClassDefinition const *classDefinition,
                                             checker::Signature const *baseSignature,
                                             ir::ScriptFunction const *derivedFunction,
                                             std::vector<ir::AstNode *> &typeNodes) const noexcept;

    public_lib::Context *context_ = nullptr;
};
}  // namespace ark::es2panda::compiler

#endif

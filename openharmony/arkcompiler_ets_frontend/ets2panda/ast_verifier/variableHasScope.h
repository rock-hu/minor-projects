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

#ifndef ES2PANDA_COMPILER_CORE_AST_VERIFIER_VARIABLEHASSCOPE_H
#define ES2PANDA_COMPILER_CORE_AST_VERIFIER_VARIABLEHASSCOPE_H

#include "checkContext.h"

#include "varbinder/varbinder.h"

namespace ark::es2panda::compiler::ast_verifier {

class VariableHasScope {
public:
    explicit VariableHasScope(ArenaAllocator &allocator) : allocator_ {allocator} {}

    [[nodiscard]] CheckResult operator()(CheckContext &ctx, const ir::AstNode *ast);
    static std::optional<varbinder::LocalVariable *> GetLocalScopeVariable(ArenaAllocator &allocator, CheckContext &ctx,
                                                                           const ir::AstNode *ast);
    bool ScopeEncloseVariable(CheckContext &ctx, const varbinder::LocalVariable *var);

private:
    ArenaAllocator &allocator_;
    bool CheckAstExceptions(const ir::AstNode *ast);
};

}  // namespace ark::es2panda::compiler::ast_verifier

#endif  // ES2PANDA_COMPILER_CORE_AST_VERIFIER_VARIABLEHASSCOPE_H

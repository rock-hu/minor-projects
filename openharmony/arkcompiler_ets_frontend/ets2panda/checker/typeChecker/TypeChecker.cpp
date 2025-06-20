/*
 * Copyright (c) 2021 - 2025 Huawei Device Co., Ltd.
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

#include "checker/typeChecker/TypeChecker.h"
#include "generated/diagnostic.h"

namespace ark::es2panda::checker {

void ETSTypeChecker::VisitArrowFunctionExpression(ir::ArrowFunctionExpression *node)
{
    Iterate(node);
    if (!node->TsType()->IsETSObjectType()) {
        LogError(diagnostic::INFER_FAIL_ON_LAMBDA, {node->TsType()}, node->Start());
    }
}

bool RunTypeChecker(Checker *checker, ScriptExtension ext, ir::AstNode *node)
{
    switch (ext) {
        case ScriptExtension::ETS:
            return ETSTypeChecker(checker).Check(node);
        case ScriptExtension::JS:
            return JSTypeChecker(checker).Check(node);
        case ScriptExtension::TS:
            return TSTypeChecker(checker).Check(node);
        case ScriptExtension::AS:
            return ASTypeChecker(checker).Check(node);
        default:
            ES2PANDA_UNREACHABLE();
    }
}

}  // namespace ark::es2panda::checker

/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstring>
#include <algorithm>
#include "util.h"

// NOLINTBEGIN

static es2panda_Impl *g_impl = nullptr;
static es2panda_Context *g_ctx = nullptr;
static bool g_isFound = false;

void FindClass(es2panda_AstNode *ast)
{
    if (g_impl->IsClassDeclaration(ast) &&
        Es2pandaAstNodeType::AST_NODE_TYPE_CLASS_DECLARATION == g_impl->AstNodeTypeConst(g_ctx, ast)) {
        g_isFound = true;
    }
}

int main(int argc, char **argv)
{
    if (argc < MIN_ARGC) {
        return 1;
    }

    if (GetImpl() == nullptr) {
        return NULLPTR_IMPL_ERROR_CODE;
    }
    g_impl = GetImpl();
    const char **args = const_cast<const char **>(&(argv[1]));
    auto config = g_impl->CreateConfig(argc - 1, args);
    g_ctx = g_impl->CreateContextFromFile(config, argv[argc - 1]);
    if (g_ctx == nullptr) {
        return NULLPTR_CONTEXT_ERROR_CODE;
    }

    g_impl->ProceedToState(g_ctx, ES2PANDA_STATE_PARSED);

    if (g_impl->ContextState(g_ctx) == ES2PANDA_STATE_ERROR) {
        return PROCEED_ERROR_CODE;
    }

    auto root = g_impl->ProgramAst(g_ctx, g_impl->ContextProgram(g_ctx));

    g_impl->AstNodeIterateConst(g_ctx, root, FindClass);

    if (!g_isFound) {
        return TEST_ERROR_CODE;
    }

    g_impl->ProceedToState(g_ctx, ES2PANDA_STATE_CHECKED);
    if (g_impl->ContextState(g_ctx) == ES2PANDA_STATE_ERROR) {
        return PROCEED_ERROR_CODE;
    }

    g_impl->AstNodeRecheck(g_ctx, root);

    g_impl->ProceedToState(g_ctx, ES2PANDA_STATE_BIN_GENERATED);
    if (g_impl->ContextState(g_ctx) == ES2PANDA_STATE_ERROR) {
        return PROCEED_ERROR_CODE;
    }

    g_impl->DestroyContext(g_ctx);
    g_impl->DestroyConfig(config);

    return 0;
}

// NOLINTEND

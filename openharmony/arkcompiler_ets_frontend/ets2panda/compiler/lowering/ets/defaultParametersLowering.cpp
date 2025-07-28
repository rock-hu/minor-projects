/*
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

#include "defaultParametersLowering.h"
#include "compiler/lowering/util.h"

#include <checker/ETSchecker.h>

namespace ark::es2panda::compiler {

static ir::Statement *TransformInitializer(ArenaAllocator *allocator, parser::ETSParser *parser,
                                           ir::ETSParameterExpression *param)
{
    auto const ident = param->Ident();
    auto const init = param->Initializer();
    auto const typeAnnotation = param->TypeAnnotation();

    param->SetIdent(Gensym(allocator));

    param->Ident()->SetTsTypeAnnotation(typeAnnotation);
    typeAnnotation->SetParent(param->Ident());

    param->SetInitializer(nullptr);
    ES2PANDA_ASSERT(param->IsOptional());

    return parser->CreateFormattedStatement("let @@I1: @@T2 = (@@I3 !== undefined) ? @@I4 : (@@E5 as @@T6)", ident,
                                            typeAnnotation->Clone(allocator, nullptr), param->Ident()->Name(),
                                            param->Ident()->Name(), init, typeAnnotation->Clone(allocator, nullptr));
}

static void ValidateDefaultParamInDeclare(public_lib::Context *ctx, ir::ScriptFunction *function,
                                          std::vector<ir::ETSParameterExpression *> &params)
{
    for (auto param : params) {
        if (param->Initializer() == nullptr) {
            continue;
        }
        param->SetInitializer(nullptr);
        if ((function->Flags() & ir::ScriptFunctionFlags::EXTERNAL) != 0U) {
            ctx->checker->AsETSChecker()->LogError(diagnostic::DEFAULT_PARAM_IN_DECLARE, param->Start());
        }
    }
}

static void TransformFunction(public_lib::Context *ctx, ir::ScriptFunction *function)
{
    auto const &params = function->Params();
    std::vector<ir::ETSParameterExpression *> defaultParams;

    for (auto *param : params) {
        if (!param->IsETSParameterExpression()) {  // #23134
            ES2PANDA_ASSERT(ctx->diagnosticEngine->IsAnyError());
            continue;
        }
        if (param->AsETSParameterExpression()->Initializer() == nullptr) {
            continue;
        }
        if (param->AsETSParameterExpression()->TypeAnnotation() == nullptr) {  // #23134
            ES2PANDA_ASSERT(ctx->diagnosticEngine->IsAnyError());
            continue;
        }
        defaultParams.push_back(param->AsETSParameterExpression());
    }

    if (defaultParams.empty()) {
        return;
    }
    if (!function->HasBody()) {  // #23134
        ValidateDefaultParamInDeclare(ctx, function, defaultParams);
        return;
    }
    auto const body = function->Body()->AsBlockStatement();
    auto const allocator = ctx->allocator;
    auto const parser = ctx->parser->AsETSParser();

    body->Statements().insert(body->Statements().begin(), defaultParams.size(), nullptr);

    for (size_t dfltIdx = 0; dfltIdx < defaultParams.size(); ++dfltIdx) {
        auto const param = defaultParams.at(dfltIdx);
        auto stmt = TransformInitializer(allocator, parser, param);
        body->Statements()[dfltIdx] = stmt;
        // From a developer's perspective, this locational information is more intuitive.
        stmt->SetParent(param);
        RefineSourceRanges(stmt);
        stmt->SetParent(body);
    }
}

bool DefaultParametersLowering::PerformForModule(public_lib::Context *ctx, parser::Program *program)
{
    program->Ast()->TransformChildrenRecursivelyPreorder(
        // CC-OFFNXT(G.FMT.14-CPP) project code style
        [ctx](ir::AstNode *const node) -> ir::AstNode * {
            if (node->IsScriptFunction()) {
                TransformFunction(ctx, node->AsScriptFunction());
            }
            return node;
        },
        Name());

    return true;
}

bool DefaultParametersLowering::PostconditionForModule([[maybe_unused]] public_lib::Context *ctx,
                                                       parser::Program const *program)
{
    return !program->Ast()->IsAnyChild([ctx](ir::AstNode const *node) {
        if (!node->IsScriptFunction()) {
            return false;
        }
        for (auto p : node->AsScriptFunction()->Params()) {
            if (!p->IsETSParameterExpression()) {  // #23134
                ES2PANDA_ASSERT(ctx->diagnosticEngine->IsAnyError());
                (void)ctx;
                continue;
            }
            if (p->AsETSParameterExpression()->Initializer() != nullptr) {
                return true;
            }
        }
        return false;
    });
}

}  // namespace ark::es2panda::compiler

/**
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "partialExportClassGen.h"

#include "checker/ETSchecker.h"
#include "varbinder/ETSBinder.h"
#include "varbinder/recordTable.h"

namespace ark::es2panda::compiler {

static void GeneratePartialDeclForExported(const public_lib::Context *const ctx, ir::AstNode *const node)
{
    // NOTE (mmartin): handle interfaces

    if (node->IsClassDeclaration()) {
        ctx->checker->AsETSChecker()->CreatePartialType(node->AsClassDeclaration()->Definition()->TsType());
    }
}

bool PartialExportClassGen::Perform(public_lib::Context *const ctx, parser::Program *const program)
{
    if (ctx->checker->VarBinder()->IsGenStdLib()) {
        for (const auto &[_, extPrograms] : program->ExternalSources()) {
            (void)_;
            for (auto *const extProg : extPrograms) {
                Perform(ctx, extProg);
            }
        }
    }

    program->Ast()->TransformChildrenRecursively(
        [ctx, &program](ir::AstNode *const ast) {
            if ((ast->IsClassDeclaration() || ast->IsTSInterfaceDeclaration()) && ast->IsExported()) {
                auto *const savedProg = ctx->checker->VarBinder()->AsETSBinder()->Program();
                ctx->checker->VarBinder()->AsETSBinder()->SetProgram(program);
                GeneratePartialDeclForExported(ctx, ast);
                ctx->checker->VarBinder()->AsETSBinder()->SetProgram(savedProg);
            }

            return ast;
        },
        Name());

    return true;
}

}  // namespace ark::es2panda::compiler

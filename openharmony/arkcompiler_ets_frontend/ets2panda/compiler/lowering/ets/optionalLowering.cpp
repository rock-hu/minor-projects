/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "optionalLowering.h"
#include "checker/ETSchecker.h"
#include "compiler/lowering/util.h"
#include "compiler/lowering/scopesInit/scopesInitPhase.h"
#include "ir/statements/blockStatement.h"
#include "ir/expressions/memberExpression.h"
#include "parser/ETSparser.h"
#include "varbinder/ETSBinder.h"

namespace ark::es2panda::compiler {

std::string_view OptionalLowering::Name() const
{
    return "OptionalLowering";
}

static ir::AstNode *RefineSourceRanges(ir::AstNode *node)
{
    auto const isDummyLoc = [](lexer::SourceRange const &range) {
        return range.start.index == 0 && range.start.line == 0;
    };

    auto const refine = [isDummyLoc](ir::AstNode *n) {
        if (isDummyLoc(n->Range())) {
            n->SetRange(n->Parent()->Range());
        };
    };

    refine(node);
    node->IterateRecursively(refine);
    return node;
}

template <typename Expr, typename GetSource, typename SetSource>
static ir::AstNode *LowerOptionalExpr(GetSource const &getSource, SetSource const &setSource, public_lib::Context *ctx,
                                      Expr *const expr, ir::ChainExpression *const chain)
{
    auto *const allocator = ctx->allocator;
    auto *const parser = ctx->parser->AsETSParser();
    auto *const varbinder = ctx->parserProgram->VarBinder();

    auto expressionCtx = varbinder::LexicalScope<varbinder::Scope>::Enter(varbinder, NearestScope(expr));
    auto *tmpIdent = Gensym(allocator);
    auto *tmpIdentClone = tmpIdent->Clone(allocator, nullptr);

    // '0's act as placeholders
    auto *sequenceExpr = parser->CreateFormattedExpression(
        "let @@I1 = 0;"
        "(@@I2 == null ? undefined : 0);",
        tmpIdent, tmpIdentClone);
    sequenceExpr->SetParent(chain->Parent());
    InitScopesPhaseETS::RunExternalNode(sequenceExpr, ctx->parserProgram->VarBinder());

    auto const &stmts = sequenceExpr->AsBlockExpression()->Statements();
    stmts[0]->AsVariableDeclaration()->Declarators()[0]->SetInit(getSource(expr));
    stmts[1]->AsExpressionStatement()->GetExpression()->AsConditionalExpression()->SetAlternate(chain->GetExpression());

    setSource(expr, parser->CreateFormattedExpression("@@I1", tmpIdentClone->Clone(allocator, nullptr)));
    return sequenceExpr;
}

static ir::AstNode *LowerExpression(public_lib::Context *ctx, ir::MemberExpression *const expr,
                                    ir::ChainExpression *chain)
{
    ASSERT(expr->IsOptional());
    expr->ClearOptional();
    return LowerOptionalExpr<ir::MemberExpression>([](auto *e) { return e->Object(); },
                                                   [](auto *e, auto *obj) { e->SetObject(obj); }, ctx, expr, chain);
}

static ir::AstNode *LowerExpression(public_lib::Context *ctx, ir::CallExpression *const expr,
                                    ir::ChainExpression *chain)
{
    ASSERT(expr->IsOptional());
    expr->ClearOptional();
    return LowerOptionalExpr<ir::CallExpression>([](auto *e) { return e->Callee(); },
                                                 [](auto *e, auto *callee) { e->SetCallee(callee); }, ctx, expr, chain);
}

static ir::Expression *FindOptionalInChain(ir::Expression *expr)
{
    if (expr->IsMemberExpression()) {
        auto typed = expr->AsMemberExpression();
        return typed->IsOptional() ? typed : FindOptionalInChain(typed->Object());
    }
    if (expr->IsCallExpression()) {
        auto typed = expr->AsCallExpression();
        return typed->IsOptional() ? typed : FindOptionalInChain(typed->Callee());
    }
    if (expr->IsTSNonNullExpression()) {
        return FindOptionalInChain(expr->AsTSNonNullExpression()->Expr());
    }
    UNREACHABLE();
}

static ir::AstNode *LowerChain(public_lib::Context *ctx, ir::ChainExpression *const chain)
{
    auto optional = FindOptionalInChain(chain->GetExpression());
    if (optional->IsMemberExpression()) {
        return LowerExpression(ctx, optional->AsMemberExpression(), chain);
    }
    if (optional->IsCallExpression()) {
        return LowerExpression(ctx, optional->AsCallExpression(), chain);
    }
    UNREACHABLE();
}

bool OptionalLowering::Perform(public_lib::Context *ctx, parser::Program *program)
{
    for (auto &[_, ext_programs] : program->ExternalSources()) {
        (void)_;
        for (auto *extProg : ext_programs) {
            Perform(ctx, extProg);
        }
    }

    program->Ast()->TransformChildrenRecursively(
        [ctx](ir::AstNode *const node) -> ir::AstNode * {
            if (node->IsChainExpression()) {
                return RefineSourceRanges(LowerChain(ctx, node->AsChainExpression()));
            }
            return node;
        },
        Name());

    return true;
}

bool OptionalLowering::Postcondition(public_lib::Context *ctx, const parser::Program *program)
{
    for (auto &[_, ext_programs] : program->ExternalSources()) {
        (void)_;
        for (auto *extProg : ext_programs) {
            if (!Postcondition(ctx, extProg)) {
                return false;
            }
        }
    }

    return !program->Ast()->IsAnyChild([](const ir::AstNode *node) {
        return node->IsChainExpression() || (node->IsMemberExpression() && node->AsMemberExpression()->IsOptional()) ||
               (node->IsCallExpression() && node->AsCallExpression()->IsOptional());
    });
}

}  // namespace ark::es2panda::compiler

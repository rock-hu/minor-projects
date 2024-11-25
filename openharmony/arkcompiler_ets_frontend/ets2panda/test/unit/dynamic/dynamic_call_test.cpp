/**
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

#include "checker/ets/dynamic/dynamicCall.h"
#include "checker/types/ets/etsDynamicType.h"
#include "test/unit/es2panda_unit_gtest.h"
#include "ir/expressions/callExpression.h"
#include "ir/expressions/memberExpression.h"
#include "ir/expressions/identifier.h"
#include "ir/ets/etsNewClassInstanceExpression.h"
#include "ir/ets/etsTypeReferencePart.h"
#include "ir/ets/etsTypeReference.h"
#include "ir/ts/tsQualifiedName.h"
#include "util/helpers.h"
#include "compiler/lowering/scopesInit/scopesInitPhase.h"
#include "util/language.h"
#include "parser/ETSparser.h"

namespace ark::es2panda::testing {

class DynamicCall : public Es2pandaUnitGtest {
public:
    std::pair<parser::Program *, ir::Expression *> ParseExpr(const std::string &strExpr)
    {
        auto program =
            Allocator()->New<parser::Program>(Allocator(), Allocator()->New<varbinder::ETSBinder>(Allocator()));
        program->VarBinder()->SetProgram(program);
        program->VarBinder()->InitTopScope();
        auto etsParser = parser::ETSParser(program, CompilerOptions {});
        auto expr = etsParser.CreateExpression(strExpr);
        return {program, expr};
    }

    ir::Expression *MarkChainDynamic(ir::Expression *obj)
    {
        if (obj == nullptr) {
            return nullptr;
        }
        auto dynamicType = Allocator()->New<checker::ETSDynamicType>(
            Allocator(), std::make_tuple("test", "test", Language::FromString("sts").value()),
            std::make_tuple(obj, checker::ETSObjectFlags::NO_OPTS, nullptr), false);
        if (obj->IsETSTypeReference()) {
            obj = obj->AsETSTypeReference()->Part()->Name();
        }
        while (obj != nullptr && (obj->IsMemberExpression() || obj->IsTSQualifiedName())) {
            obj->SetTsType(dynamicType);
            if (obj->IsMemberExpression()) {
                obj = obj->AsMemberExpression()->Object();
            } else if (obj->IsTSQualifiedName()) {
                obj = obj->AsTSQualifiedName()->Left();
            }
        }
        obj->SetTsType(dynamicType);
        return obj;
    }

    std::tuple<parser::Program *, ir::Expression *, ir::Expression *> ParseDynExpr(const std::string &strExpr)
    {
        auto [prog, expr] = ParseExpr(strExpr);
        ir::Expression *obj = nullptr;
        if (expr->IsCallExpression()) {
            obj = expr->AsCallExpression()->Callee();
        } else {
            obj = expr->AsETSNewClassInstanceExpression()->GetTypeRef()->AsETSTypeReference();
        }
        auto first = MarkChainDynamic(obj);
        return {prog, obj, first};
    }

    void AddDynImport(const char *specifierName, varbinder::ETSBinder *varbinder, ir::Identifier *node)
    {
        auto aIdent = Allocator()->New<ir::Identifier>(specifierName, Allocator());
        ArenaVector<ir::AstNode *> specifiers {Allocator()->Adapter()};
        auto specifier = Allocator()->New<ir::ImportSpecifier>(aIdent, aIdent);
        specifiers.emplace_back(specifier);
        auto importSrc = Allocator()->New<ir::ImportSource>(Allocator()->New<ir::StringLiteral>("/tmp"),
                                                            Allocator()->New<ir::StringLiteral>(),
                                                            Language::FromString("js").value(), false);
        auto importDecl =
            util::NodeAllocator::Alloc<ir::ETSImportDeclaration>(Allocator(), importSrc, std::move(specifiers));
        compiler::InitScopesPhaseETS::RunExternalNode(importDecl, varbinder);
        varbinder->BuildImportDeclaration(importDecl);
        auto var = varbinder->TopScope()->Find(specifierName);
        node->SetVariable(var.variable);
    }

    void AssertNameEq(const ArenaVector<util::StringView> &name, std::initializer_list<const char *> expected)
    {
        ASSERT_EQ(name.size(), expected.size());
        auto it1 = expected.begin();
        auto it2 = name.begin();
        while (it2 != name.end()) {
            ASSERT_EQ(util::StringView(*it1), *it2);
            it1++, it2++;
        }
    }
};

TEST_F(DynamicCall, JoinDynMemberChain)
{
    auto strExpr = "A.b.c.d()";
    auto [prog, obj, first] = ParseDynExpr(strExpr);
    auto [squeezedObj, name] = checker::DynamicCall::SqueezeExpr(Allocator(), obj->AsMemberExpression());
    AssertNameEq(name, {"b", "c", "d"});
    ASSERT(squeezedObj->IsIdentifier());
    auto varbinder = prog->VarBinder()->AsETSBinder();
    {
        // With empty varbinder A is local variable
        auto [finalObj, callName] = checker::DynamicCall::ResolveCall(varbinder, obj);
        AssertNameEq(callName, {"b", "c", "d"});
    }
    // Now A is import => we can optimize
    AddDynImport("A", varbinder, first->AsIdentifier());
    auto [finalObj, callName] = checker::DynamicCall::ResolveCall(varbinder, obj);
    AssertNameEq(callName, {"A", "b", "c", "d"});
}

TEST_F(DynamicCall, JoinCompitedMemberChain)
{
    auto strExpr = "A.b.c[0].d.e.f()";
    auto [prog, obj, first] = ParseDynExpr(strExpr);
    auto [squeezedObj, name] = checker::DynamicCall::SqueezeExpr(Allocator(), obj->AsMemberExpression());
    // Can't optimize []
    AssertNameEq(name, {"d", "e", "f"});
    ASSERT_EQ(squeezedObj,
              obj->AsMemberExpression()->Object()->AsMemberExpression()->Object()->AsMemberExpression()->Object());
    auto varbinder = prog->VarBinder()->AsETSBinder();
    {
        // Can't optimize []
        auto [finalObj, callName] = checker::DynamicCall::ResolveCall(varbinder, obj);
        AssertNameEq(callName, {"d", "e", "f"});
    }
    // Can't optimize []
    AddDynImport("A", varbinder, first->AsIdentifier());
    auto [finalObj, callName] = checker::DynamicCall::ResolveCall(varbinder, obj);
    AssertNameEq(callName, {"d", "e", "f"});
}

TEST_F(DynamicCall, JoinDynCallMember)
{
    auto strExpr = "A.b().c.d()";
    auto [program, obj, first] = ParseDynExpr(strExpr);
    auto [squeezedObj, name] = checker::DynamicCall::SqueezeExpr(Allocator(), obj->AsMemberExpression());
    AssertNameEq(name, {"c", "d"});
    ASSERT_EQ(squeezedObj, obj->AsMemberExpression()->Object()->AsMemberExpression()->Object());

    auto varbinder = program->VarBinder()->AsETSBinder();
    auto [finalObj, callName] = checker::DynamicCall::ResolveCall(varbinder, obj);
    AssertNameEq(callName, {"c", "d"});
}

TEST_F(DynamicCall, JoinDynStaticCallMember)
{
    auto strExpr = "A.b.c.d.e()";
    auto [program, obj, first] = ParseDynExpr(strExpr);

    auto bObj = obj->AsMemberExpression()->Object()->AsMemberExpression()->Object();
    ASSERT_EQ(bObj->AsMemberExpression()->Property()->AsIdentifier()->Name(), "c");
    auto staticType = Allocator()->New<checker::ETSObjectType>(Allocator(), checker::ETSObjectFlags::NO_OPTS);
    bObj->AsMemberExpression()->Object()->SetTsType(staticType);

    auto [squeezedObj, name] = checker::DynamicCall::SqueezeExpr(Allocator(), obj->AsMemberExpression());
    AssertNameEq(name, {"d", "e"});
    ASSERT_EQ(squeezedObj, bObj);

    auto varbinder = program->VarBinder()->AsETSBinder();
    AddDynImport("A", varbinder, first->AsIdentifier());
    auto [finalObj, callName] = checker::DynamicCall::ResolveCall(varbinder, obj);
    AssertNameEq(callName, {"d", "e"});
}

TEST_F(DynamicCall, TsQualifiedName)
{
    auto strExpr = "new A.b.c.d()";
    auto [program, obj, first] = ParseDynExpr(strExpr);
    auto varbinder = program->VarBinder()->AsETSBinder();
    AddDynImport("A", varbinder, first->AsIdentifier());
    auto [finalObj, callName] = checker::DynamicCall::ResolveCall(varbinder, obj);
    AssertNameEq(callName, {"A", "b", "c", "d"});
}

}  // namespace ark::es2panda::testing

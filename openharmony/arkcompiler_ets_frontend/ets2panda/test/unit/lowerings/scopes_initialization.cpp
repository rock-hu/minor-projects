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

#include <gtest/gtest.h>
#include <algorithm>
#include "macros.h"

#include "test/unit/node_creator.h"
#include "compiler/lowering/scopesInit/scopesInitPhase.h"
#include "varbinder/tsBinding.h"
#include "varbinder/ETSBinder.h"

namespace ark::es2panda {

class ScopesInitPhaseTest : public testing::Test {
public:
    ~ScopesInitPhaseTest() override = default;

    ScopesInitPhaseTest()
        : allocator_(std::make_unique<ArenaAllocator>(SpaceType::SPACE_TYPE_COMPILER)), nodeGen_(allocator_.get())
    {
    }

    static void SetUpTestCase()
    {
        constexpr auto COMPILER_SIZE = operator""_MB(256ULL);
        mem::MemConfig::Initialize(0, 0, COMPILER_SIZE, 0, 0, 0);
        PoolManager::Initialize();
    }

    ArenaAllocator *Allocator()
    {
        return allocator_.get();
    }

    gtests::NodeGenerator &NodeGen()
    {
        return nodeGen_;
    }

    /*
     * Shortcut to convert single elemnt block expression body to it's name
     * Example: { let x; } => x
     */
    static ir::Identifier *BodyToFirstName(ir::Statement *body)
    {
        return body->AsBlockStatement()
            ->Statements()
            .front()
            ->AsVariableDeclaration()
            ->Declarators()
            .front()
            ->Id()
            ->AsIdentifier();
    }

    NO_COPY_SEMANTIC(ScopesInitPhaseTest);
    NO_MOVE_SEMANTIC(ScopesInitPhaseTest);

private:
    std::unique_ptr<ArenaAllocator> allocator_;
    gtests::NodeGenerator nodeGen_;
};

TEST_F(ScopesInitPhaseTest, TestForUpdateLoop)
{
    /*
     * for (int x = 0; x < 10; x++ ) { let x; }
     */
    auto varbinder = varbinder::VarBinder(Allocator());
    auto forNode = NodeGen().CreateForUpdate();
    compiler::InitScopesPhaseETS::RunExternalNode(forNode, &varbinder);

    auto blockScope = forNode->Body()->AsBlockStatement()->Scope();
    auto loopScope = forNode->Scope();
    auto parScope = loopScope->Parent();
    ASSERT_EQ(blockScope->Parent(), loopScope);

    const auto &scopeBindings = blockScope->Bindings();
    const auto &parBindings = parScope->Bindings();

    ASSERT_EQ(scopeBindings.size(), 1);
    ASSERT_EQ(parBindings.size(), 1);

    auto parName = forNode->Init()->AsVariableDeclaration()->Declarators()[0]->Id()->AsIdentifier();
    auto name = BodyToFirstName(forNode->Body());
    ASSERT_EQ(scopeBindings.begin()->first, name->Name());
    ASSERT_EQ(parBindings.begin()->first, parName->Name());
    ASSERT_EQ(scopeBindings.begin()->second, name->Variable());
    ASSERT_EQ(parBindings.begin()->second, parName->Variable());
    ASSERT_NE(parName->Variable(), name->Variable());
}

TEST_F(ScopesInitPhaseTest, CreateWhile)
{
    /*
     * while (x < 10) { let x; }
     */
    auto varbinder = varbinder::VarBinder(Allocator());
    auto whileNode = NodeGen().CreateWhile();

    compiler::InitScopesPhaseETS::RunExternalNode(whileNode, &varbinder);

    auto whileScope = whileNode->Scope();
    auto bodyScope = whileNode->Body()->AsBlockStatement()->Scope();
    ASSERT_EQ(bodyScope->Parent(), whileScope);

    const auto &bodyBindings = bodyScope->Bindings();
    auto name = BodyToFirstName(whileNode->Body());
    ASSERT_EQ(bodyBindings.size(), 1);
    ASSERT_EQ(bodyBindings.begin()->first, name->Name());
    ASSERT_EQ(bodyBindings.begin()->second, name->Variable());
}

}  // namespace ark::es2panda

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

#include "unit_test.h"
#include "optimizer/optimizations/if_conversion.h"

namespace ark::compiler {
class IfConversionTest : public GraphTest {
public:
    IfConversionTest()
    {
        SetGraphArch(ark::RUNTIME_ARCH);
#ifndef NDEBUG
        // GraphChecker hack: LowLevel instructions may appear only after Lowering pass:
        GetGraph()->SetLowLevelInstructionsEnabled();
#endif
    }
    Graph *CreateExpectedJointTriangleImm();
    Graph *CreateExpJointTriangleWithTrickFloatPhi();
    Graph *CreateExpectedJointDiamondImm();
    Graph *CreateExpectedJointDiamond();
    Graph *CreateExpJointDiamondWithDroppedSelect();
    void CreateNonLoopInvariantNotPreventConversion();
};

// NOLINTBEGIN(readability-magic-numbers)
/*
 * Test Graph:
 *              [entry}
 *                 |
 *            /---[2]---\
 *            |         |
 *           [3]        |
 *            |         |
 *            \---[4]---/
 *                 |
 *               [exit]
 */
TEST_F(IfConversionTest, TriangleTrueImm)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(3U, Opcode::Compare).b().CC(CC_B).Inputs(0U, 1U);
            INST(4U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(3U, 4U)
        {
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
        }
        BASIC_BLOCK(4U, -1L)
        {
            INST(6U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 5U}});
            INST(7U, Opcode::Return).u64().Inputs(6U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, -1L)
        {
            INST(3U, Opcode::Compare).b().CC(CC_B).Inputs(0U, 1U);
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(4U, Opcode::SelectImm).u64().SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(5U, 0U, 3U);
            INST(7U, Opcode::Return).u64().Inputs(4U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

TEST_F(IfConversionTest, TriangleTrue)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(3U, Opcode::If).SrcType(DataType::UINT64).CC(CC_NE).Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 4U)
        {
            INST(4U, Opcode::Mul).u64().Inputs(0U, 2U);
        }
        BASIC_BLOCK(4U, -1L)
        {
            INST(5U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 4U}});
            INST(6U, Opcode::Return).u64().Inputs(5U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, -1L)
        {
            INST(4U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(3U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_NE).Inputs(4U, 0U, 0U, 1U);
            INST(6U, Opcode::Return).u64().Inputs(3U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

/*
 * Test Graph:
 *              [entry}
 *                 |
 *            /---[2]---\
 *            |         |
 *            |        [3]
 *            |         |
 *            \---[4]---/
 *                 |
 *               [exit]
 */
TEST_F(IfConversionTest, TriangleFalseImm)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, 4U, 3U)
        {
            INST(3U, Opcode::Compare).b().CC(CC_AE).Inputs(0U, 1U);
            INST(4U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(3U, 4U)
        {
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
        }
        BASIC_BLOCK(4U, -1L)
        {
            INST(6U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 5U}});
            INST(7U, Opcode::Return).u64().Inputs(6U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, -1L)
        {
            INST(3U, Opcode::Compare).b().CC(CC_AE).Inputs(0U, 1U);
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(4U, Opcode::SelectImm).u64().SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(0U, 5U, 3U);
            INST(7U, Opcode::Return).u64().Inputs(4U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

TEST_F(IfConversionTest, TriangleFalse)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, 4U, 3U)
        {
            INST(3U, Opcode::If).SrcType(DataType::UINT64).CC(CC_LT).Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 4U)
        {
            INST(4U, Opcode::Mul).u64().Inputs(0U, 2U);
        }
        BASIC_BLOCK(4U, -1L)
        {
            INST(5U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 4U}});
            INST(6U, Opcode::Return).u64().Inputs(5U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, -1L)
        {
            INST(4U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(3U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_LT).Inputs(0U, 4U, 0U, 1U);
            INST(6U, Opcode::Return).u64().Inputs(3U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

Graph *IfConversionTest::CreateExpectedJointTriangleImm()
{
    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, 3U, 5U)
        {
            INST(3U, Opcode::Compare).b().Inputs(0U, 1U);
            INST(4U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(3U, 5U)
        {
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(6U, Opcode::Compare).b().Inputs(5U, 1U);
            INST(8U, Opcode::Mul).u64().Inputs(5U, 2U);
            INST(7U, Opcode::SelectImm).u64().SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(8U, 5U, 6U);
        }
        BASIC_BLOCK(5U, -1L)
        {
            INST(9U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 7U}});
            INST(10U, Opcode::Return).u64().Inputs(9U);
        }
    }
    return graph;
}

/*
 * Test Graph:
 *              [entry}
 *                 |
 *            /---[2]---\
 *            |         |
 *        /--[3]--\     |
 *        |       |     |
 *       [4]      |     |
 *        |       |     |
 *        \------[5]----/
 *                |
 *              [exit]
 */
TEST_F(IfConversionTest, JointTriangleImm)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, 3U, 5U)
        {
            INST(3U, Opcode::Compare).b().Inputs(0U, 1U);
            INST(4U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(3U, 4U, 5U)
        {
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(6U, Opcode::Compare).b().Inputs(5U, 1U);
            INST(7U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(6U);
        }
        BASIC_BLOCK(4U, 5U)
        {
            INST(8U, Opcode::Mul).u64().Inputs(5U, 2U);
        }
        BASIC_BLOCK(5U, -1L)
        {
            INST(9U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 5U}, {4U, 8U}});
            INST(10U, Opcode::Return).u64().Inputs(9U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateExpectedJointTriangleImm();
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

TEST_F(IfConversionTest, TriangleTwice)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        PARAMETER(2U, 2U).u64();
        BASIC_BLOCK(2U, 3U, 5U)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 1U);
            INST(4U, Opcode::If).SrcType(DataType::UINT64).CC(CC_NE).Inputs(3U, 1U);
        }
        BASIC_BLOCK(3U, 4U, 5U)
        {
            INST(5U, Opcode::If).SrcType(DataType::UINT64).CC(CC_NE).Inputs(3U, 2U);
        }
        BASIC_BLOCK(4U, 5U)
        {
            INST(6U, Opcode::Mul).u64().Inputs(3U, 2U);
        }
        BASIC_BLOCK(5U, -1L)
        {
            INST(7U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 3U}, {4U, 6U}});
            INST(8U, Opcode::Return).u64().Inputs(7U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        PARAMETER(2U, 2U).u64();
        BASIC_BLOCK(2U, -1L)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 1U);
            INST(6U, Opcode::Mul).u64().Inputs(3U, 2U);
            INST(5U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_NE).Inputs(6U, 3U, 3U, 2U);
            INST(4U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_NE).Inputs(5U, 0U, 3U, 1U);
            INST(8U, Opcode::Return).u64().Inputs(4U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

Graph *IfConversionTest::CreateExpJointTriangleWithTrickFloatPhi()
{
    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).f64();
        CONSTANT(2U, 10U);
        BASIC_BLOCK(2U, 3U, 5U)
        {
            INST(3U, Opcode::If).SrcType(DataType::UINT64).CC(CC_NE).Inputs(0U, 2U);
        }
        BASIC_BLOCK(3U, 5U)
        {
            INST(4U, Opcode::Mul).f64().Inputs(1U, 1U);
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(7U, Opcode::Mul).u64().Inputs(5U, 2U);
            INST(6U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_NE).Inputs(7U, 5U, 5U, 2U);
        }
        BASIC_BLOCK(5U, -1L)
        {
            INST(8U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 6U}});
            INST(9U, Opcode::Phi).f64().Inputs({{2U, 1U}, {3U, 4U}});
            INST(10U, Opcode::Mul).f64().Inputs(9U, 9U);
            INST(20U, Opcode::SaveState).NoVregs();
            INST(11U, Opcode::CallStatic).u64().InputsAutoType(8U, 10U, 20U);
            INST(12U, Opcode::Return).u64().Inputs(11U);
        }
    }
    return graph;
}

TEST_F(IfConversionTest, JointTriangleWithTrickFloatPhi)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).f64();
        CONSTANT(2U, 10U);
        BASIC_BLOCK(2U, 3U, 5U)
        {
            INST(3U, Opcode::If).SrcType(DataType::UINT64).CC(CC_NE).Inputs(0U, 2U);
        }
        BASIC_BLOCK(3U, 4U, 5U)
        {
            INST(4U, Opcode::Mul).f64().Inputs(1U, 1U);
            INST(5U, Opcode::Mul).u64().Inputs(0U, 2U);
            INST(6U, Opcode::If).SrcType(DataType::UINT64).CC(CC_NE).Inputs(5U, 2U);
        }
        BASIC_BLOCK(4U, 5U)
        {
            INST(7U, Opcode::Mul).u64().Inputs(5U, 2U);
        }
        BASIC_BLOCK(5U, -1L)
        {
            INST(8U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 5U}, {4U, 7U}});
            INST(9U, Opcode::Phi).f64().Inputs({{2U, 1U}, {3U, 4U}, {4U, 4U}});
            INST(10U, Opcode::Mul).f64().Inputs(9U, 9U);
            INST(20U, Opcode::SaveState).NoVregs();
            INST(11U, Opcode::CallStatic).u64().InputsAutoType(8U, 10U, 20U);
            INST(12U, Opcode::Return).u64().Inputs(11U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateExpJointTriangleWithTrickFloatPhi();
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

/*
 * Test Graph:
 *              [entry}
 *                 |
 *            /---[2]---\
 *            |         |
 *           [3]       [4]
 *            |         |
 *            \---[5]---/
 *                 |
 *               [exit]
 */
TEST_F(IfConversionTest, DiamondImm)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u32();
        PARAMETER(1U, 1U).u32();
        CONSTANT(2U, 0U);
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(3U, Opcode::Compare).b().Inputs(1U, 2U);
            INST(4U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(4U, 5U)
        {
            INST(5U, Opcode::Add).u32().Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 5U)
        {
            INST(7U, Opcode::Sub).u32().Inputs(0U, 1U);
        }
        BASIC_BLOCK(5U, -1L)
        {
            INST(8U, Opcode::Phi).u32().Inputs({{4U, 5U}, {3U, 7U}});
            INST(9U, Opcode::Return).u32().Inputs(8U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u32();
        PARAMETER(1U, 1U).u32();
        CONSTANT(2U, 0U);
        BASIC_BLOCK(2U, -1L)
        {
            INST(3U, Opcode::Compare).b().Inputs(1U, 2U);
            INST(7U, Opcode::Sub).u32().Inputs(0U, 1U);
            INST(5U, Opcode::Add).u32().Inputs(0U, 1U);
            INST(4U, Opcode::SelectImm).u32().SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(7U, 5U, 3U);
            INST(9U, Opcode::Return).u32().Inputs(4U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

Graph *IfConversionTest::CreateExpectedJointDiamondImm()
{
    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, 3U, 6U)
        {
            INST(2U, Opcode::Compare).b().Inputs(0U, 1U);
            INST(3U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(2U);
        }
        BASIC_BLOCK(3U, 6U)
        {
            INST(4U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(5U, Opcode::Compare).b().Inputs(4U, 1U);
            INST(7U, Opcode::Mul).u64().Inputs(4U, 1U);
            INST(8U, Opcode::Mul).u64().Inputs(4U, 0U);
            INST(6U, Opcode::SelectImm).u64().SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(7U, 8U, 5U);
        }
        BASIC_BLOCK(6U, -1L)
        {
            INST(9U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 6U}});
            INST(10U, Opcode::Return).u64().Inputs(9U);
        }
    }
    return graph;
}

/*
 * Test Graph:
 *              [entry}
 *                 |
 *            /---[2]---\
 *            |         |
 *        /--[3]--\     |
 *        |       |     |
 *       [4]     [5]    |
 *        |       |     |
 *        \------[6]----/
 *                |
 *              [exit]
 */
TEST_F(IfConversionTest, JointDiamondImm)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, 3U, 6U)
        {
            INST(2U, Opcode::Compare).b().Inputs(0U, 1U);
            INST(3U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(2U);
        }
        BASIC_BLOCK(3U, 4U, 5U)
        {
            INST(4U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(5U, Opcode::Compare).b().Inputs(4U, 1U);
            INST(6U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(5U);
        }
        BASIC_BLOCK(4U, 6U)
        {
            INST(7U, Opcode::Mul).u64().Inputs(4U, 1U);
        }
        BASIC_BLOCK(5U, 6U)
        {
            INST(8U, Opcode::Mul).u64().Inputs(4U, 0U);
        }
        BASIC_BLOCK(6U, -1L)
        {
            INST(9U, Opcode::Phi).u64().Inputs({{2U, 0U}, {4U, 7U}, {5U, 8U}});
            INST(10U, Opcode::Return).u64().Inputs(9U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateExpectedJointDiamondImm();
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

Graph *IfConversionTest::CreateExpectedJointDiamond()
{
    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, 3U, 6U)
        {
            INST(2U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GE).Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 6U)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(5U, Opcode::Mul).u64().Inputs(3U, 1U);
            INST(6U, Opcode::Mul).u64().Inputs(3U, 0U);
            INST(4U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_GT).Inputs(5U, 6U, 3U, 1U);
        }
        BASIC_BLOCK(6U, -1L)
        {
            INST(7U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 4U}});
            INST(8U, Opcode::Return).u64().Inputs(7U);
        }
    }
    return graph;
}

TEST_F(IfConversionTest, JointDiamond)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, 3U, 6U)
        {
            INST(2U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GE).Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 4U, 5U)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(4U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GT).Inputs(3U, 1U);
        }
        BASIC_BLOCK(4U, 6U)
        {
            INST(5U, Opcode::Mul).u64().Inputs(3U, 1U);
        }
        BASIC_BLOCK(5U, 6U)
        {
            INST(6U, Opcode::Mul).u64().Inputs(3U, 0U);
        }
        BASIC_BLOCK(6U, -1L)
        {
            INST(7U, Opcode::Phi).u64().Inputs({{2U, 0U}, {4U, 5U}, {5U, 6U}});
            INST(8U, Opcode::Return).u64().Inputs(7U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateExpectedJointDiamond();
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

Graph *IfConversionTest::CreateExpJointDiamondWithDroppedSelect()
{
    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, 3U, 6U)
        {
            INST(2U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GE).Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 6U)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(5U, Opcode::Mul).u64().Inputs(3U, 1U);
            INST(6U, Opcode::Mul).u64().Inputs(3U, 0U);
            INST(4U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_GT).Inputs(5U, 6U, 3U, 1U);
            // Second Select not needed
        }
        BASIC_BLOCK(6U, -1L)
        {
            INST(7U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 4U}});
            INST(8U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 3U}});
            INST(9U, Opcode::Add).u64().Inputs(7U, 8U);
            INST(10U, Opcode::Return).u64().Inputs(9U);
        }
    }
    return graph;
}

TEST_F(IfConversionTest, JointDiamondWithDroppedSelect)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, 3U, 6U)
        {
            INST(2U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GE).Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 4U, 5U)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(4U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GT).Inputs(3U, 1U);
        }
        BASIC_BLOCK(4U, 6U)
        {
            INST(5U, Opcode::Mul).u64().Inputs(3U, 1U);
        }
        BASIC_BLOCK(5U, 6U)
        {
            INST(6U, Opcode::Mul).u64().Inputs(3U, 0U);
        }
        BASIC_BLOCK(6U, -1L)
        {
            INST(7U, Opcode::Phi).u64().Inputs({{2U, 0U}, {4U, 5U}, {5U, 6U}});
            INST(8U, Opcode::Phi).u64().Inputs({{2U, 0U}, {4U, 3U}, {5U, 3U}});
            INST(9U, Opcode::Add).u64().Inputs(7U, 8U);
            INST(10U, Opcode::Return).u64().Inputs(9U);
        }
    }

    GetGraph()->RunPass<IfConversion>();

    auto graph = CreateExpJointDiamondWithDroppedSelect();
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

TEST_F(IfConversionTest, JointDiamondRunTwice)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, 3U, 6U)
        {
            INST(2U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GE).Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 4U, 5U)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(4U, Opcode::Mul).u64().Inputs(1U, 1U);
            INST(5U, Opcode::If).SrcType(DataType::UINT64).CC(CC_GT).Inputs(4U, 1U);
        }
        BASIC_BLOCK(4U, 6U) {}
        BASIC_BLOCK(5U, 6U) {}
        BASIC_BLOCK(6U, -1L)
        {
            INST(6U, Opcode::Phi).u64().Inputs({{2U, 0U}, {4U, 3U}, {5U, 3U}});
            INST(7U, Opcode::Return).u64().Inputs(6U);
        }
    }

    ASSERT_TRUE(GetGraph()->RunPass<IfConversion>());
    ASSERT_TRUE(GetGraph()->RunPass<Cleanup>());

    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(0U, 0U).u64();
        PARAMETER(1U, 1U).u64();
        BASIC_BLOCK(2U, -1L)
        {
            INST(3U, Opcode::Mul).u64().Inputs(0U, 0U);
            INST(2U, Opcode::Select).u64().SrcType(DataType::UINT64).CC(CC_GE).Inputs(3U, 0U, 0U, 1U);
            INST(7U, Opcode::Return).u64().Inputs(2U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}

/*
 *  No conversion allowed cases below.
 */
TEST_F(IfConversionTest, TriangleWithCall)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u64();
        CONSTANT(1U, 10U);
        CONSTANT(2U, 2U);
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(3U, Opcode::Compare).b().Inputs(0U, 1U);
            INST(4U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(3U, 4U)
        {
            INST(20U, Opcode::SaveState).NoVregs();
            INST(5U, Opcode::CallStatic).u64().InputsAutoType(0U, 2U, 20U);
        }
        BASIC_BLOCK(4U, -1L)
        {
            INST(6U, Opcode::Phi).u64().Inputs({{2U, 0U}, {3U, 5U}});
            INST(7U, Opcode::Return).u64().Inputs(6U);
        }
    }

    ASSERT_FALSE(GetGraph()->RunPass<IfConversion>());
}

TEST_F(IfConversionTest, DiamondThreeOperations)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u32();
        PARAMETER(1U, 1U).u32();
        CONSTANT(2U, 42U);
        CONSTANT(3U, 0U);
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(4U, Opcode::Compare).b().Inputs(1U, 3U);
            INST(5U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(4U);
        }
        BASIC_BLOCK(4U, 5U)
        {
            INST(6U, Opcode::Add).u32().Inputs(0U, 1U);
        }
        BASIC_BLOCK(3U, 5U)
        {
            INST(7U, Opcode::Add).u32().Inputs(0U, 2U);
            INST(8U, Opcode::Sub).u32().Inputs(7U, 1U);
        }
        BASIC_BLOCK(5U, -1L)
        {
            INST(9U, Opcode::Phi).u32().Inputs({{4U, 6U}, {3U, 8U}});
            INST(10U, Opcode::Return).u32().Inputs(9U);
        }
    }

    ASSERT_FALSE(GetGraph()->RunPass<IfConversion>());
}

TEST_F(IfConversionTest, DiamondThreePhis)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).u32();
        PARAMETER(1U, 1U).u32();
        CONSTANT(2U, 42U);
        CONSTANT(3U, 0U);
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(6U, Opcode::Add).u32().Inputs(0U, 1U);
            INST(7U, Opcode::Add).u32().Inputs(0U, 2U);
            INST(8U, Opcode::Sub).u32().Inputs(7U, 1U);
            INST(4U, Opcode::Compare).b().Inputs(1U, 3U);
            INST(5U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(4U);
        }
        BASIC_BLOCK(4U, 5U) {}
        BASIC_BLOCK(3U, 5U) {}
        BASIC_BLOCK(5U, -1L)
        {
            INST(9U, Opcode::Phi).u32().Inputs({{4U, 6U}, {3U, 8U}});
            INST(10U, Opcode::Phi).u32().Inputs({{4U, 6U}, {3U, 7U}});
            INST(11U, Opcode::Phi).u32().Inputs({{4U, 7U}, {3U, 8U}});
            INST(12U, Opcode::Add).u32().Inputs(9U, 10U);
            INST(13U, Opcode::Add).u32().Inputs(11U, 12U);
            INST(14U, Opcode::Return).u32().Inputs(13U);
        }
    }

    ASSERT_FALSE(GetGraph()->RunPass<IfConversion>());
}

TEST_F(IfConversionTest, TriangleFloat)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).f64();
        PARAMETER(1U, 1U).f64();
        PARAMETER(2U, 2U).f64();
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(3U, Opcode::Cmp).s32().SrcType(DataType::FLOAT64).Inputs(0U, 1U);
            INST(4U, Opcode::IfImm).SrcType(DataType::INT32).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(3U, 4U)
        {
            INST(5U, Opcode::Mul).f64().Inputs(0U, 2U);
        }
        BASIC_BLOCK(4U, -1L)
        {
            INST(6U, Opcode::Phi).f64().Inputs({{2U, 0U}, {3U, 5U}});
            INST(7U, Opcode::Return).f64().Inputs(6U);
        }
    }

    ASSERT_EQ(GetGraph()->RunPass<IfConversion>(), GetGraph()->GetEncoder()->CanEncodeFloatSelect());
}

TEST_F(IfConversionTest, TrianglePhiFloat)
{
    GRAPH(GetGraph())
    {
        PARAMETER(0U, 0U).f64();
        PARAMETER(1U, 1U).f64();
        PARAMETER(2U, 2U).f64();
        BASIC_BLOCK(2U, 3U, 4U)
        {
            INST(5U, Opcode::Mul).f64().Inputs(0U, 2U);
            INST(3U, Opcode::Cmp).s32().SrcType(DataType::FLOAT64).Inputs(0U, 1U);
            INST(4U, Opcode::IfImm).SrcType(DataType::INT32).CC(CC_NE).Imm(0U).Inputs(3U);
        }
        BASIC_BLOCK(3U, 4U) {}  // Instructions 5 moved up manually
        BASIC_BLOCK(4U, -1L)
        {
            INST(6U, Opcode::Phi).f64().Inputs({{2U, 0U}, {3U, 5U}});
            INST(7U, Opcode::Return).f64().Inputs(6U);
        }
    }

    ASSERT_EQ(GetGraph()->RunPass<IfConversion>(), GetGraph()->GetEncoder()->CanEncodeFloatSelect());
}

// CC-OFFNXT(huge_method, G.FUN.01) graph creation
TEST_F(IfConversionTest, LoopInvariantPreventTriangle)
{
    GRAPH(GetGraph())
    {
        PARAMETER(1U, 1U).ptr();
        CONSTANT(3U, 0x64U).i64();
        CONSTANT(4U, 0U).i64();
        CONSTANT(5U, 1U).i64();
        CONSTANT(30U, 2U).i64();

        BASIC_BLOCK(30U, 2U, 7U)
        {
            INST(8U, Opcode::Load).b().Inputs(1U, 4U);
            INST(11U, Opcode::Load).b().Inputs(1U, 5U);
            INST(32U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(8U);
        }

        BASIC_BLOCK(7U, 15U, 2U)
        {
            INST(34U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(11U);
        }

        BASIC_BLOCK(15U, 2U) {}

        BASIC_BLOCK(2U, 20U, 4U)
        {
            INST(41U, Opcode::Phi).b().Inputs(5U, 5U, 4U);
            INST(23U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(8U);
        }

        BASIC_BLOCK(4U, 20U, 18U)
        {
            INST(25U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(11U);
        }

        BASIC_BLOCK(18U, 20U) {}

        BASIC_BLOCK(20U, 16U)
        {
            INST(43U, Opcode::Phi).b().Inputs(5U, 5U, 4U);
        }

        BASIC_BLOCK(16U, 3U, 5U)
        {
            INST(13U, Opcode::Phi).i32().Inputs(4U, 53U);
            INST(14U, Opcode::Phi).i32().Inputs(5U, 37U);
            INST(44U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(43U);
        }

        BASIC_BLOCK(5U, 3U)
        {
            INST(26U, Opcode::Add).i32().Inputs(13U, 14U);
        }

        BASIC_BLOCK(3U, 6U, 8U)
        {
            INST(27U, Opcode::Phi).i32().Inputs(14U, 26U);
            INST(52U, Opcode::AddI).i32().Imm(2U).Inputs(27U);
            INST(29U, Opcode::Add).i32().Inputs(27U, 30U);
            INST(42U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(41U);
        }

        BASIC_BLOCK(6U, 8U)
        {
            INST(36U, Opcode::Sub).i32().Inputs(52U, 13U);
        }

        BASIC_BLOCK(8U, 31U, 16U)
        {
            INST(37U, Opcode::Phi).i32().Inputs(52U, 36U);
            INST(53U, Opcode::AddI).i32().Imm(1U).Inputs(13U);
            INST(39U, Opcode::Add).i32().Inputs(13U, 5U);
            INST(21U, Opcode::IfImm).SrcType(DataType::INT32).CC(CC_GE).Imm(0x64U).Inputs(53U);
        }

        BASIC_BLOCK(31U, -1L)
        {
            INST(40U, Opcode::Return).i32().Inputs(37U);
        }
    }
    ASSERT_FALSE(GetGraph()->RunPass<IfConversion>());
}

// CC-OFFNXT(huge_method, G.FUN.01) graph creation
TEST_F(IfConversionTest, LoopInvariantPreventDiamond)
{
    GRAPH(GetGraph())
    {
        PARAMETER(1U, 1U).ptr();
        CONSTANT(3U, 0x64U).i64();
        CONSTANT(4U, 0U).i64();
        CONSTANT(5U, 1U).i64();
        CONSTANT(30U, 2U).i64();

        BASIC_BLOCK(30U, 2U, 7U)
        {
            INST(8U, Opcode::Load).b().Inputs(1U, 4U);
            INST(11U, Opcode::Load).b().Inputs(1U, 5U);
            INST(32U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(8U);
        }

        BASIC_BLOCK(7U, 15U, 2U)
        {
            INST(34U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(11U);
        }

        BASIC_BLOCK(15U, 2U) {}

        BASIC_BLOCK(2U, 20U, 4U)
        {
            INST(41U, Opcode::Phi).b().Inputs(5U, 5U, 4U);
            INST(23U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(8U);
        }

        BASIC_BLOCK(4U, 20U, 18U)
        {
            INST(25U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(11U);
        }

        BASIC_BLOCK(18U, 20U) {}

        BASIC_BLOCK(20U, 16U)
        {
            INST(43U, Opcode::Phi).b().Inputs(5U, 5U, 4U);
        }

        BASIC_BLOCK(16U, 32U, 5U)
        {
            INST(13U, Opcode::Phi).i32().Inputs(4U, 53U);
            INST(14U, Opcode::Phi).i32().Inputs(5U, 37U);
            INST(44U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(43U);
        }

        BASIC_BLOCK(5U, 3U)
        {
            INST(26U, Opcode::Add).i32().Inputs(13U, 14U);
        }

        BASIC_BLOCK(32U, 3U)
        {
            INST(126U, Opcode::Sub).i32().Inputs(13U, 14U);
        }

        BASIC_BLOCK(3U, 6U, 33U)
        {
            INST(27U, Opcode::Phi).i32().Inputs(26U, 126U);
            INST(52U, Opcode::AddI).i32().Imm(2U).Inputs(27U);
            INST(29U, Opcode::Add).i32().Inputs(27U, 30U);
            INST(42U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_NE).Imm(0U).Inputs(41U);
        }

        BASIC_BLOCK(6U, 8U)
        {
            INST(36U, Opcode::Sub).i32().Inputs(52U, 13U);
        }

        BASIC_BLOCK(33U, 8U)
        {
            INST(136U, Opcode::Sub).i32().Inputs(13U, 52U);
        }

        BASIC_BLOCK(8U, 31U, 16U)
        {
            INST(37U, Opcode::Phi).i32().Inputs(36U, 136U);
            INST(53U, Opcode::AddI).i32().Imm(1U).Inputs(13U);
            INST(39U, Opcode::Add).i32().Inputs(13U, 5U);
            INST(21U, Opcode::IfImm).SrcType(DataType::INT32).CC(CC_GE).Imm(0x64U).Inputs(53U);
        }

        BASIC_BLOCK(31U, -1L)
        {
            INST(40U, Opcode::Return).i32().Inputs(37U);
        }
    }
    ASSERT_FALSE(GetGraph()->RunPass<IfConversion>());
}

// CC-OFFNXT(huge_method, G.FUN.01) graph creation, solid logic
void IfConversionTest::CreateNonLoopInvariantNotPreventConversion()
{
    GRAPH(GetGraph())
    {
        PARAMETER(1U, 1U).ptr();
        PARAMETER(2U, 2U).i64();
        PARAMETER(3U, 3U).i64();
        CONSTANT(12U, 1U).i64();
        CONSTANT(13U, 0U).i64();
        CONSTANT(80U, 10U).i64();

        BASIC_BLOCK(6U, 31U, 5U)
        {
            INST(16U, Opcode::Phi).i32().Inputs(12U, 62U);
            INST(21U, Opcode::Load).i32().Inputs(1U, 12U);
            INST(22U, Opcode::Compare).b().SrcType(DataType::INT32).CC(CC_LE).Inputs(21U, 16U);
            INST(23U, Opcode::IfImm).SrcType(DataType::BOOL).Imm(0U).Inputs(22U);
        }

        BASIC_BLOCK(5U, 3U, 4U)
        {
            INST(28U, Opcode::Phi).i64().Inputs(13U, 49U);
            INST(37U, Opcode::Load).b().Inputs(1U, 13U);
            INST(39U, Opcode::IfImm).SrcType(DataType::BOOL).Imm(0U).Inputs(37U);
        }

        BASIC_BLOCK(4U, 21U, 20U)
        {
            INST(93U, Opcode::Load).i32().Inputs(1U, 2U);
            INST(94U, Opcode::Compare).b().SrcType(DataType::INT32).Inputs(93U, 80U);
            INST(95U, Opcode::IfImm).SrcType(DataType::BOOL).Imm(0U).Inputs(94U);
        }

        BASIC_BLOCK(20U, 21U) {}

        BASIC_BLOCK(21U, 22U, 23U)
        {
            INST(96U, Opcode::Phi).i32().Inputs(12U, 13U);
            INST(99U, Opcode::Load).i32().Inputs(1U, 3U);
            INST(101U, Opcode::If).SrcType(DataType::INT32).CC(CC_NE).Inputs(80U, 99U);
        }

        BASIC_BLOCK(23U, 22U) {}

        BASIC_BLOCK(22U, 18U, 25U)
        {
            INST(102U, Opcode::Phi).i32().Inputs(12U, 13U);
            INST(103U, Opcode::And).i32().Inputs(102U, 96U);
            INST(104U, Opcode::Compare).b().SrcType(DataType::INT32).Inputs(103U, 13U);
            INST(105U, Opcode::If).SrcType(DataType::INT32).CC(CC_NE).Inputs(13U, 103U);
        }

        BASIC_BLOCK(25U, -1L)
        {
            INST(106U, Opcode::SaveState).Inputs(102U, 96U).SrcVregs({0U, 1U});
            INST(107U, Opcode::LoadAndInitClass).ref().Inputs(106U);
            INST(108U, Opcode::NewObject).ref().Inputs(107U, 106U);
            INST(109U, Opcode::SaveState).Inputs(108U).SrcVregs({0U});
            INST(113U, Opcode::Throw).Inputs(108U, 109U);
        }

        BASIC_BLOCK(18U, 5U)
        {
            INST(49U, Opcode::Add).i64().Inputs(28U, 12U);
        }

        BASIC_BLOCK(3U, 6U)
        {
            INST(62U, Opcode::Add).i32().Inputs(16U, 12U);
        }

        BASIC_BLOCK(31U, -1L)
        {
            INST(58U, Opcode::Return).i32().Inputs(21U);
        }
    }
}

// CC-OFFNXT(huge_method, G.FUN.01) graph creation
TEST_F(IfConversionTest, NonLoopInvariantNotPreventConversion)
{
    CreateNonLoopInvariantNotPreventConversion();
    ASSERT_TRUE(GetGraph()->RunPass<IfConversion>());
    auto graph = CreateEmptyGraph();
    GRAPH(graph)
    {
        PARAMETER(1U, 1U).ptr();
        PARAMETER(2U, 2U).i64();
        PARAMETER(3U, 3U).i64();
        CONSTANT(12U, 1U).i64();
        CONSTANT(13U, 0U).i64();
        CONSTANT(80U, 10U).i64();

        BASIC_BLOCK(42U, 6U) {}

        BASIC_BLOCK(6U, 31U, 5U)
        {
            INST(16U, Opcode::Phi).i32().Inputs(12U, 62U);
            INST(21U, Opcode::Load).i32().Inputs(1U, 12U);
            INST(22U, Opcode::Compare).b().SrcType(DataType::INT32).CC(CC_LE).Inputs(21U, 16U);
            INST(23U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(22U);
        }

        BASIC_BLOCK(5U, 3U, 4U)
        {
            INST(28U, Opcode::Phi).i64().Inputs(13U, 49U);
            INST(37U, Opcode::Load).b().Inputs(1U, 13U);
            INST(39U, Opcode::IfImm).SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(37U);
        }

        BASIC_BLOCK(4U, 18U, 25U)
        {
            INST(93U, Opcode::Load).i32().Inputs(1U, 2U);
            INST(94U, Opcode::Compare).b().SrcType(DataType::INT32).CC(CC_EQ).Inputs(93U, 80U);
            INST(115U, Opcode::SelectImm).i32().SrcType(DataType::BOOL).CC(CC_EQ).Imm(0U).Inputs(12U, 13U, 94U);
            INST(99U, Opcode::Load).i32().Inputs(1U, 3U);
            INST(114U, Opcode::Select).i32().SrcType(DataType::INT32).CC(CC_NE).Inputs(12U, 13U, 80U, 99U);
            INST(103U, Opcode::And).i32().Inputs(114U, 115U);
            INST(104U, Opcode::Compare).b().SrcType(DataType::INT32).CC(CC_EQ).Inputs(103U, 13U);
            INST(105U, Opcode::If).SrcType(DataType::INT32).CC(CC_NE).Inputs(13U, 103U);
        }

        BASIC_BLOCK(25U, -1L)
        {
            INST(106U, Opcode::SaveState).Inputs(114U, 115U).SrcVregs({0U, 1U});
            INST(107U, Opcode::LoadAndInitClass).ref().Inputs(106U);
            INST(108U, Opcode::NewObject).ref().Inputs(107U, 106U);
            INST(109U, Opcode::SaveState).Inputs(108U).SrcVregs({0U});
            INST(113U, Opcode::Throw).Inputs(108U, 109U);
        }

        BASIC_BLOCK(18U, 5U)
        {
            INST(49U, Opcode::Add).i64().Inputs(28U, 12U);
        }

        BASIC_BLOCK(3U, 6U)
        {
            INST(62U, Opcode::Add).i32().Inputs(16U, 12U);
        }

        BASIC_BLOCK(31U, -1L)
        {
            INST(58U, Opcode::Return).i32().Inputs(21U);
        }
    }
    ASSERT_TRUE(GraphComparator().Compare(GetGraph(), graph));
}
// NOLINTEND(readability-magic-numbers)

}  // namespace ark::compiler

/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "irregular_matrices.h"
#include "test/unittest/core/pattern/grid/grid_test_ng.h"

#include "core/components_ng/pattern/grid/irregular/grid_layout_range_solver.h"

namespace OHOS::Ace::NG {
class GridLayoutRangeTest : public GridTestNg {};

/**
 * @tc.name: LayoutRangeSolver::SolveForward001
 * @tc.desc: Test LayoutRangeSolver::SolveForward
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, SolveForward001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo2());
    CreateDone(frameNode_);

    GridLayoutInfo info;
    info.crossCount_ = 3;
    info.gridMatrix_ = MATRIX_DEMO_2;
    info.lineHeightMap_ = { { 0, 20.0f }, { 1, 40.0f }, { 2, 40.0f }, { 3, 10.0f }, { 4, 50.0f }, { 5, 70.0f } };

    GridLayoutRangeSolver solver(&info, AceType::RawPtr(frameNode_));

    info.currentOffset_ = 0.0f;
    info.startMainLineIndex_ = 3;
    info.startIndex_ = 4;
    auto res = solver.FindStartingRow(1.0f);
    EXPECT_EQ(res.row, 3);
    EXPECT_EQ(res.idx, 4);
    EXPECT_EQ(res.pos, 0.0f);

    info.currentOffset_ = -20.0f;
    info.startMainLineIndex_ = 0;
    res = solver.FindStartingRow(1.0f);
    EXPECT_EQ(res.row, 0);
    EXPECT_EQ(res.pos, -20.0f);

    info.currentOffset_ = -70.0f;
    info.startMainLineIndex_ = 0;
    res = solver.FindStartingRow(1.0f);
    EXPECT_EQ(res.row, 0);
    EXPECT_EQ(res.pos, -70.0f);

    // startMainLineIndex_ == 1 || startMainLineIndex_ == 2 is impossible.
    // LayoutRangeSolver always finds the first row of irregular items.

    info.currentOffset_ = -9.0f;
    info.startMainLineIndex_ = 3;
    res = solver.FindStartingRow(1.0f);
    EXPECT_EQ(res.row, 3);
    EXPECT_EQ(res.pos, -9.0f);

    for (int i = 0; i < 3; ++i) {
        info.currentOffset_ = -10.0f - i * 1.0f;
        info.startMainLineIndex_ = 3;
        res = solver.FindStartingRow(1.0f);
        EXPECT_EQ(res.row, 4);
        EXPECT_EQ(res.idx, 5);
        EXPECT_EQ(res.pos, 1.0f - i * 1.0f);
    }

    info.currentOffset_ = -110.0f;
    info.startMainLineIndex_ = 3;
    res = solver.FindStartingRow(1.0f);
    EXPECT_EQ(res.row, 4);
    EXPECT_EQ(res.pos, -99.0f);
}

/**
 * @tc.name: LayoutRangeSolver::CheckMultiRow001
 * @tc.desc: Test LayoutRangeSolver::CheckMultiRow
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, CheckMultiRow001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.irregularIndexes = {
        0, // [2 x 1]
        3, // [3 x 2]
    };
    auto onGetIrregularSizeByIndex = [](int32_t index) -> GridItemSize {
        if (index == 0) {
            return { .rows = 1, .columns = 2 };
        }
        return { .rows = 2, .columns = 3 };
    };

    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateDone(frameNode_);

    GridLayoutInfo info;
    info.crossCount_ = 3;
    info.gridMatrix_ = {
        { 0, { { 0, 0 }, { 0, -1 }, { 2, 2 } } },   // 0 | 0 | 2
        { 1, { { 0, 3 }, { 1, -3 }, { 2, -3 } } },  // 3 | 3 | 3
        { 2, { { 0, -3 }, { 1, -3 }, { 2, -3 } } }, // 3 | 3 | 3
    };

    GridLayoutRangeSolver solver(&info, AceType::RawPtr(frameNode_));
    EXPECT_EQ(solver.CheckMultiRow(2), std::make_pair(1, 3));

    EXPECT_EQ(solver.CheckMultiRow(0), std::make_pair(0, 0));
    EXPECT_EQ(solver.CheckMultiRow(1), std::make_pair(1, 3));
}

/**
 * @tc.name: LayoutRangeSolver::SolveBackward001
 * @tc.desc: Test LayoutRangeSolver::SolveBackward
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, SolveBackward001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.irregularIndexes = {
        0, // [2 x 1]
        3, // [2 x 2]
        4, // [1 x 2]
        6, // [2 x 1]
    };
    auto onGetIrregularSizeByIndex = [](int32_t index) -> GridItemSize {
        if (index == 4) {
            return { .rows = 2, .columns = 1 };
        }
        if (index == 3) {
            return { 2, 2 };
        }
        return { .rows = 1, .columns = 2 };
    };

    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateDone(frameNode_);

    GridLayoutInfo info;
    info.crossCount_ = 3;
    info.gridMatrix_ = {
        { 0, { { 0, 0 }, { 0, 0 }, { 2, 1 } } },   // 0 | 0 | 1
        { 1, { { 0, 2 }, { 1, 3 }, { 2, -3 } } },  // 2 | 3 | 3
        { 2, { { 0, 4 }, { 1, -3 }, { 2, -3 } } }, // 4 | 3 | 3
        { 3, { { 0, -4 }, { 1, 5 } } },            // 4 | 5 | x
        { 4, { { 0, 6 }, { 1, -6 }, { 2, 7 } } },  // 6 | 6 | 7
    };
    info.lineHeightMap_ = { { 0, 50.0f }, { 1, 30.0f }, { 2, 40.0f }, { 3, 30.0f }, { 4, 50.0f } };

    info.currentOffset_ = 20.0f;
    info.startMainLineIndex_ = 4;

    GridLayoutRangeSolver solver(&info, AceType::RawPtr(frameNode_));
    auto res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, -60.0f);
    EXPECT_EQ(res.row, 2);

    info.currentOffset_ = 80.0f;
    info.startMainLineIndex_ = 4;

    res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, -35.0f);
    EXPECT_EQ(res.row, 1);

    info.currentOffset_ = 200.0f;
    info.startMainLineIndex_ = 4;

    res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, 30.0f);
    EXPECT_EQ(res.row, 0);
}

/**
 * @tc.name: LayoutRangeSolver::SolveBackward002
 * @tc.desc: Test LayoutRangeSolver::SolveBackward
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, SolveBackward002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo10());
    CreateDone(frameNode_);

    GridLayoutInfo info;
    info.crossCount_ = 3;
    info.gridMatrix_ = MATRIX_DEMO_10;
    info.lineHeightMap_ = { { 0, 50.0f }, { 1, 30.0f }, { 2, 40.0f }, { 3, 30.0f }, { 4, 50.0f } };

    info.currentOffset_ = 20.0f;
    info.startMainLineIndex_ = 3;

    GridLayoutRangeSolver solver(&info, AceType::RawPtr(frameNode_));
    auto res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, -60.0f);
    EXPECT_EQ(res.row, 1);

    info.currentOffset_ = 6.0f;
    info.startMainLineIndex_ = 3;
    res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, -74.0f);
    EXPECT_EQ(res.row, 1);

    info.currentOffset_ = 5.0f;
    info.startMainLineIndex_ = 3;
    res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, 5.0f);
    EXPECT_EQ(res.row, 3);

    info.currentOffset_ = 4.0f;
    info.startMainLineIndex_ = 3;
    res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, 4.0f);
    EXPECT_EQ(res.row, 3);
}

/**
 * @tc.name: LayoutRangeSolver::SolveOverScroll001
 * @tc.desc: Test LayoutRangeSolver with overScroll
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, SolveOverScroll001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo12());
    CreateDone(frameNode_);

    GridLayoutInfo info;
    info.crossCount_ = 3;
    info.gridMatrix_ = MATRIX_DEMO_12;
    info.lineHeightMap_ = { { 0, 10.0f }, { 1, 10.0f }, { 2, 10.0f }, { 3, 10.0f }, { 4, 10.0f }, { 5, 10.0f },
        { 6, 10.0f } };

    float offset = -50.0f;
    info.currentOffset_ = offset;
    info.startMainLineIndex_ = 1;
    info.startIndex_ = 2;

    GridLayoutRangeSolver solver(&info, AceType::RawPtr(frameNode_));

    for (int i = 0; i < 10; ++i) {
        auto res = solver.FindStartingRow(0.0f);
        // can't scroll further if all items are already above viewport
        EXPECT_EQ(res.pos, offset);
        EXPECT_EQ(res.row, 1);
        offset -= 10.0f;
        info.currentOffset_ = offset;
    }
}

/**
 * @tc.name: LayoutRangeSolver::SolveOverScroll002
 * @tc.desc: Test LayoutRangeSolver with overScroll upwards
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, SolveOverScroll002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo12());
    CreateDone(frameNode_);

    GridLayoutInfo info;
    info.crossCount_ = 3;
    info.gridMatrix_ = MATRIX_DEMO_12;
    info.lineHeightMap_ = { { 0, 10.0f }, { 1, 10.0f }, { 2, 10.0f }, { 3, 10.0f }, { 4, 10.0f }, { 5, 10.0f },
        { 6, 10.0f } };

    float offset = 0.0f;
    info.currentOffset_ = offset;
    info.startMainLineIndex_ = 0;
    info.startIndex_ = 0;

    GridLayoutRangeSolver solver(&info, AceType::RawPtr(frameNode_));

    for (int i = 0; i < 10; ++i) {
        auto res = solver.FindStartingRow(0.0f);
        EXPECT_EQ(res.pos, offset);
        EXPECT_EQ(res.row, 0);
        offset += 10.0f;
        info.currentOffset_ = offset;
    }
}

/**
 * @tc.name: ScrollItem001
 * @tc.desc: Test ScrollToIndex
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, ScrollItem001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetColumnsGap(Dimension { 10.0f });
    model.SetRowsGap(Dimension { 10.0f });
    constexpr float itemHeight = 500.0f;
    CreateFixedHeightItems(1, itemHeight * 2 + 10.0f);
    CreateFixedHeightItems(1, itemHeight);
    CreateFixedHeightItems(1, itemHeight * 2 + 10.0f);
    CreateFixedHeightItems(19, itemHeight);
    CreateFixedHeightItems(1, itemHeight * 6 + 50.0f);
    CreateFixedHeightItems(77, itemHeight);
    CreateDone(frameNode_);
    const auto& info = pattern_->info_;

    pattern_->ScrollToIndex(88, false);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.startIndex_, 85);
    EXPECT_EQ(info.endIndex_, 90);
    pattern_->ScrollToIndex(2, false, ScrollAlign::CENTER);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.startIndex_, 2);
    EXPECT_EQ(info.endIndex_, 2);
}

/**
 * @tc.name: ScrollItem002
 * @tc.desc: Test scroll to center of long item
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, ScrollItem002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetColumnsGap(Dimension { 10.0f });
    model.SetRowsGap(Dimension { 10.0f });
    constexpr float itemHeight = 300.0f;
    CreateFixedHeightItems(1, itemHeight * 2 + 10.0f);
    CreateFixedHeightItems(1, itemHeight);
    CreateFixedHeightItems(1, itemHeight * 2 + 10.0f);
    CreateFixedHeightItems(19, itemHeight);
    CreateFixedHeightItems(1, itemHeight * 6 + 50.0f);
    CreateFixedHeightItems(77, itemHeight);
    CreateDone(frameNode_);
    const auto& info = pattern_->info_;

    pattern_->ScrollToIndex(22, false, ScrollAlign::CENTER, 0.0f);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.startIndex_, 22);
    EXPECT_EQ(info.endIndex_, 26);
    for (int i = info.startIndex_; i <= info.endIndex_; ++i) {
        EXPECT_TRUE(GetChildFrameNode(frameNode_, i)->IsActive());
    }
    EXPECT_EQ(GetChildY(frameNode_, 22), -525.0f);
}

/**
 * @tc.name: LayoutRangeSolver::Solve001
 * @tc.desc: Test LayoutRangeSolver::FindStartingRow when matrix is empty.
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, Solve001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions({});
    CreateDone(frameNode_);

    GridLayoutInfo info;
    info.crossCount_ = 3;

    info.currentOffset_ = 0.0f;
    info.startMainLineIndex_ = 0;

    GridLayoutRangeSolver solver(&info, AceType::RawPtr(frameNode_));
    auto res = solver.FindStartingRow(5.0f);
    EXPECT_EQ(res.pos, 0.0f);
    EXPECT_EQ(res.row, 0);
}

/**
 * @tc.name: HorizontalOverScroll001
 * @tc.desc: Test horizontal and overScroll
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, Horizontal001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetRowsGap(Dimension { 1.0f });
    model.SetColumnsGap(Dimension { 5.0f });
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedWidthItems(1, 910.0f);
    CreateFixedWidthItems(1, 300.0f);
    CreateFixedWidthItems(20, 605.0f);
    CreateFixedWidthItems(8, 300.0f);
    CreateDone(frameNode_);

    pattern_->scrollableEvent_->scrollable_->isTouching_ = true;
    UpdateCurrentOffset(FLT_MAX);
    const auto& info = pattern_->info_;
    EXPECT_EQ(info.startIndex_, 0);
    EXPECT_EQ(info.endIndex_, -1);
    for (int i = 0; i < 5; ++i) {
        EXPECT_FALSE(GetChildFrameNode(frameNode_, i)->IsActive());
    }
}

/**
 * @tc.name: ChangeTemplate001
 * @tc.desc: Test changing template
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, ChangeTemplate001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetColumnsGap(Dimension { 10.0f });
    model.SetRowsGap(Dimension { 20.0f });
    constexpr float itemHeight = 790.0f;
    CreateFixedHeightItems(22, itemHeight);
    CreateFixedHeightItems(1, (itemHeight + 20.0f) * 6);
    CreateFixedHeightItems(7, itemHeight);
    CreateFixedHeightItems(1, 200.0f);
    CreateDone(frameNode_);

    pattern_->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    FlushLayoutTask(frameNode_);

    const auto& info = pattern_->info_;
    EXPECT_EQ(info.startIndex_, 29);
    EXPECT_EQ(info.endIndex_, 30);
    EXPECT_EQ(info.currentOffset_, 10.0f);
    EXPECT_FLOAT_EQ(GetChildY(frameNode_, 29), 10.0f);

    layoutProperty_->UpdateColumnsTemplate("1fr 1fr 1fr");
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.startIndex_, 28);
    EXPECT_EQ(info.endIndex_, 30);
    EXPECT_EQ(info.currentOffset_, 10.0f);
    EXPECT_FLOAT_EQ(GetChildY(frameNode_, 28), 10.0f);
    EXPECT_FLOAT_EQ(GetChildY(frameNode_, 29), 10.0f);

    layoutProperty_->UpdateColumnsTemplate("1fr 1fr 1fr 1fr");
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    // startIdx changed, but currentOffset_ is maintained. So Item 28 no longer in range
    EXPECT_EQ(info.startIndex_, 21);
    EXPECT_EQ(info.endIndex_, 23);
    EXPECT_EQ(info.currentOffset_, 10.0f);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 29)->IsActive());
}

/**
 * @tc.name: Jump001
 * @tc.desc: Test jump to irregular item with extra offset
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, Jump001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetColumnsGap(Dimension { 10.0f });
    model.SetRowsGap(Dimension { 20.0f });
    constexpr float itemHeight = 300.0f;
    CreateFixedHeightItems(22, itemHeight);
    CreateFixedHeightItems(1, (itemHeight + 20.0f) * 6);
    CreateFixedHeightItems(77, itemHeight);
    CreateDone(frameNode_);
    pattern_->ScrollToIndex(22, false, ScrollAlign::AUTO, itemHeight);
    FlushLayoutTask(frameNode_);
    const auto& info = pattern_->info_;
    EXPECT_EQ(GetChildRect(frameNode_, 22).Bottom(), GRID_HEIGHT - itemHeight);
    EXPECT_EQ(info.startIndex_, 22);

    pattern_->ScrollToIndex(88, false, ScrollAlign::AUTO, itemHeight);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.startIndex_, 85);
    EXPECT_EQ(info.endIndex_, 93);
    for (int i = 85; i <= 93; ++i) {
        EXPECT_TRUE(GetChildFrameNode(frameNode_, i)->IsActive());
    }
    EXPECT_EQ(GetChildY(frameNode_, 91), 520.0f);
}

/**
 * @tc.name: MeasureToTarget001
 * @tc.desc: Test measure to target
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, MeasureToTarget001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetColumnsGap(Dimension { 10.0f });
    model.SetRowsGap(Dimension { 20.0f });
    constexpr float itemHeight = 790.0f;
    CreateFixedHeightItems(22, itemHeight);
    CreateFixedHeightItems(1, (itemHeight + 20.0f) * 6);
    CreateFixedHeightItems(77, itemHeight);
    CreateDone(frameNode_);
    pattern_->ScrollToIndex(23, true);
    EXPECT_EQ(pattern_->targetIndex_, 23);
    FlushLayoutTask(frameNode_);
    const auto& info = pattern_->info_;
    for (int i = 0; i < 24; ++i) {
        auto it = info.FindInMatrix(i);
        EXPECT_NE(it, info.gridMatrix_.end());
        bool found = false;
        for (auto item : it->second) {
            if (item.second == i) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found);
    }
    EXPECT_FLOAT_EQ(info.GetAnimatePosIrregular(23, GRID_HEIGHT, ScrollAlign::AUTO, 20.0f), 11350.0f);
}

/**
 * @tc.name: MeasureToTarget002
 * @tc.desc: Test measure to target
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, MeasureToTarget002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetColumnsGap(Dimension { 10.0f });
    model.SetRowsGap(Dimension { 20.0f });
    constexpr float itemHeight = 300.0f;
    CreateFixedHeightItems(22, itemHeight);
    CreateFixedHeightItems(1, (itemHeight + 20.0f) * 6);
    CreateFixedHeightItems(77, itemHeight);
    CreateDone(frameNode_);
    pattern_->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    FlushLayoutTask(frameNode_);
    const auto& info = pattern_->info_;
    EXPECT_EQ(info.startIndex_, 91);

    layoutProperty_->UpdateColumnsTemplate("1fr 1fr 1fr 1fr");
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);

    pattern_->ScrollToIndex(22, true, ScrollAlign::AUTO);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(pattern_->AnimateToTargetImpl(ScrollAlign::AUTO, nullptr));
    const float offset = info.GetAnimatePosIrregular(22, GRID_HEIGHT, ScrollAlign::AUTO, 20.0f);
    UpdateCurrentOffset(pattern_->GetTotalOffset() - offset);

    EXPECT_TRUE(NearEqual(GetChildY(frameNode_, 22), 0.0f));
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 22)->IsActive());

    pattern_->ScrollToIndex(22, true, ScrollAlign::AUTO);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.GetAnimatePosIrregular(22, GRID_HEIGHT, ScrollAlign::AUTO, 20.0f), -1.0f);
    pattern_->extraOffset_ = 0.0f;
    EXPECT_FALSE(pattern_->AnimateToTargetImpl(ScrollAlign::AUTO, nullptr));
}

/**
 * @tc.name: Cache001
 * @tc.desc: Test Grid preload items
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, Cache001, TestSize.Level1)
{
    GridModelNG model = CreateRepeatGrid(50, [](uint32_t idx) { return 200.0f; });
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    model.SetLayoutOptions({});
    model.SetCachedCount(2); // 2 lines
    CreateDone(frameNode_);
    EXPECT_EQ(frameNode_->GetTotalChildCount(), 50);
    EXPECT_EQ(frameNode_->GetChildren().size(), 1);
    const auto& info = pattern_->info_;
    EXPECT_EQ(info.startIndex_, 0);
    EXPECT_EQ(info.endIndex_, 11);
    const std::list<int32_t> preloadList = { 12, 13, 14, 15, 16, 17 };
    for (const int32_t i : preloadList) {
        EXPECT_FALSE(frameNode_->GetChildByIndex(i));
    }
    CheckPreloadListEqual(preloadList);
    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    EXPECT_TRUE(pattern_->preloadItemList_.empty());
    constexpr float itemWidth = (GRID_WIDTH - 20.0f) / 3.0f;
    for (const int32_t i : preloadList) {
        EXPECT_TRUE(frameNode_->GetChildByIndex(i));
        EXPECT_EQ(GetChildWidth(frameNode_, i), itemWidth);
        EXPECT_EQ(GetChildHeight(frameNode_, i), 200.0f);
    }

    // re-layout to trigger cache item layout
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(GetChildX(frameNode_, 12), 0.0f);
    EXPECT_EQ(GetChildX(frameNode_, 13), itemWidth + 10.0f);
    EXPECT_EQ(GetChildX(frameNode_, 14), (itemWidth + 10.0f) * 2);
    EXPECT_EQ(GetChildX(frameNode_, 15), 0.0f);
    EXPECT_EQ(GetChildX(frameNode_, 16), itemWidth + 10.0f);
    EXPECT_EQ(GetChildX(frameNode_, 17), (itemWidth + 10.0f) * 2);
    EXPECT_EQ(GetChildY(frameNode_, 12), 840.0f);
    EXPECT_EQ(GetChildY(frameNode_, 13), 840.0f);
    EXPECT_EQ(GetChildY(frameNode_, 14), 840.0f);
    EXPECT_EQ(GetChildY(frameNode_, 15), 1050.0f);
    EXPECT_EQ(GetChildY(frameNode_, 16), 1050.0f);
    EXPECT_EQ(GetChildY(frameNode_, 17), 1050.0f);
    pattern_->ScrollToIndex(49);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.startIndex_, 39);
    const std::list<int32_t> preloadList2 = { 38, 37, 36, 35, 34, 33 };
    CheckPreloadListEqual(preloadList2);
    const int64_t time = GetSysTimestamp();
    PipelineContext::GetCurrentContext()->OnIdle(time - 1);
    // no time to execute
    CheckPreloadListEqual(preloadList2);
}

/**
 * @tc.name: Cache002
 * @tc.desc: Test Grid preload items
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, Cache002, TestSize.Level1)
{
    GridModelNG model = CreateRepeatGrid(50, [](uint32_t idx) {
        if (idx == 0 || idx == 2) {
            return 410.0f;
        }
        if (idx == 22) {
            return 1250.0f;
        }
        return 200.0f;
    });
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetCachedCount(3); // 2 lines
    CreateDone(frameNode_);
    pattern_->ScrollToIndex(20, false, ScrollAlign::END);
    FlushLayoutTask(frameNode_);
    const auto& info = pattern_->info_;
    EXPECT_EQ(info.currentOffset_, -30.0f);
    EXPECT_EQ(info.startIndex_, 9);
    EXPECT_EQ(info.endIndex_, 20);
    const std::list<int32_t> preloadList = { 8, 21, 7, 22, 6, 23, 5, 24, 4, 25, 3, 26, 27, 28, 29 };
    for (const int32_t i : preloadList) {
        EXPECT_FALSE(frameNode_->GetChildByIndex(i));
    }
    CheckPreloadListEqual(preloadList);
    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    EXPECT_EQ(GetChildHeight(frameNode_, 22), 1250.0f);

    // re-layout to trigger cache item layout
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(GetChildY(frameNode_, 8), -240.0f);
    EXPECT_EQ(GetChildY(frameNode_, 7), -240.0f);
    EXPECT_EQ(GetChildY(frameNode_, 6), -240.0f);
    EXPECT_EQ(GetChildY(frameNode_, 22), 810.0f);
    EXPECT_EQ(GetChildY(frameNode_, 23), 1020.0f);
    EXPECT_EQ(GetChildY(frameNode_, 24), 1230.0f);
}

/**
 * @tc.name: Drag001
 * @tc.desc: Test grid dragged item
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutRangeTest, Drag001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(GetOptionDemo14());
    model.SetColumnsGap(Dimension { 10.0f });
    model.SetRowsGap(Dimension { 20.0f });
    model.SetEditable(true);
    constexpr float itemHeight = 300.0f;
    CreateFixedHeightItems(22, itemHeight);
    CreateFixedHeightItems(1, itemHeight * 5 + 100.0f);
    CreateFixedHeightItems(77, itemHeight);
    CreateDone(frameNode_);

    pattern_->ScrollToIndex(21, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 2)->IsActive());
    EXPECT_EQ(GetChildY(frameNode_, 2), 640.0f);
    EXPECT_EQ(GetChildY(frameNode_, 24), 640.0f);
    GestureEvent event;
    event.SetGlobalPoint(Point(5.0f, 650.0f));
    eventHub_->HandleOnItemDragStart(event);
    EXPECT_EQ(eventHub_->draggedIndex_, 24);
}
} // namespace OHOS::Ace::NG

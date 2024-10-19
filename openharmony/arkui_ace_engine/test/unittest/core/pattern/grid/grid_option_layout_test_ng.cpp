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

#include "grid_test_ng.h"
#include "test/mock/core/animation/mock_animation_manager.h"

#include "core/components_ng/pattern/grid/grid_layout/grid_layout_algorithm.h"
#include "core/components_ng/pattern/grid/grid_scroll/grid_scroll_with_options_layout_algorithm.h"
#include "core/components_ng/pattern/grid/irregular/grid_irregular_layout_algorithm.h"
#include "core/components_ng/pattern/grid/irregular/grid_layout_utils.h"
#include "core/components_ng/pattern/refresh/refresh_model_ng.h"

namespace OHOS::Ace::NG {

namespace {} // namespace

class GridOptionLayoutTestNg : public GridTestNg {
public:
};

/**
 * @tc.name: GridScrollWithOptions001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridScrollWithOptions001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5, 0 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize;
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollWithOptionsLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    if (AceType::InstanceOf<GridIrregularLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm())) {
        return;
    }
    layoutAlgorithm->GetTargetIndexInfoWithBenchMark(AccessibilityManager::RawPtr(frameNode_), false, 5);
    EXPECT_EQ(layoutAlgorithm->gridLayoutInfo_.startMainLineIndex_, 1);
}

/**
 * @tc.name: GridScrollWithOptions002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridScrollWithOptions002, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5, 0 };
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollWithOptionsLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    if (AceType::InstanceOf<GridIrregularLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm())) {
        return;
    }
    layoutAlgorithm->GetTargetIndexInfoWithBenchMark(AccessibilityManager::RawPtr(frameNode_), false, 5);
    EXPECT_EQ(layoutAlgorithm->gridLayoutInfo_.startMainLineIndex_, 5);
}

/**
 * @tc.name: GridScrollWithOptions003
 * @tc.desc: change grid columns after scroll
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridScrollWithOptions003, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize { 1, 2 };
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);
    pattern_->UpdateStartIndex(3);
    FlushLayoutTask(frameNode_);
    layoutProperty_->UpdateColumnsTemplate("1fr 1fr 1fr 1fr 1fr");
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollWithOptionsLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    if (AceType::InstanceOf<GridIrregularLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm())) {
        return;
    }
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(3, option, 1), std::make_pair(0, 2));
}

/**
 * @tc.name: GridScrollWithOptions004
 * @tc.desc: change grid columns after scroll, first line has empty position
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridScrollWithOptions004, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize { 1, 2 };
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);
    pattern_->UpdateStartIndex(3);
    FlushLayoutTask(frameNode_);
    layoutProperty_->UpdateColumnsTemplate("1fr 1fr 1fr 1fr 1fr 1fr");
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollWithOptionsLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    if (AceType::InstanceOf<GridIrregularLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm())) {
        return;
    }
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(3, option, 1), std::make_pair(0, 2));
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(2, option, 1), std::make_pair(3, 2));
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(1, option, 1), std::make_pair(1, 2));
}

/**
 * @tc.name: GridScrollWithOptions005
 * @tc.desc: second line full
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridScrollWithOptions005, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize { 1, 2 };
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);
    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollWithOptionsLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    if (AceType::InstanceOf<GridIrregularLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm())) {
        return;
    }
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(3, option, 1), std::make_pair(2, 2));
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(2, option, 1), std::make_pair(0, 2));
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(1, option, 1), std::make_pair(1, 2));
}

/**
 * @tc.name: GridScrollWithOptions006
 * @tc.desc: first irregular item in new line
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridScrollWithOptions006, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 3, 4, 5 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize { 1, 2 };
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);
    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollWithOptionsLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    if (AceType::InstanceOf<GridIrregularLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm())) {
        return;
    }
    EXPECT_EQ(layoutAlgorithm->GetCrossStartAndSpanWithUserFunction(4, option, 1), std::make_pair(2, 2));
}

/**
 * @tc.name: SearchIrregularFocusableChildInScroll001
 * @tc.desc: Test the function when the gridItem cannot be focused
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, SearchIrregularFocusableChildInScroll001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create gridItems with irregular shape in scroll grid.
     */
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5, 0 };
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step2. Find target child with specified index parameters.
     * @tc.expected: Can not find the target focus child.
     */
    int32_t tarMainIndex = 1;
    int32_t tarCrossIndex = 1;
    auto IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    RefPtr<FocusHub> result = IrregularFocusableChild.Upgrade();
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: SearchIrregularFocusableChildInScroll002
 * @tc.desc: Test the function when the gridItem can be focused
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, SearchIrregularFocusableChildInScroll002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create gridItems with irregular shape in scroll grid.
     */
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5, 0 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize { 1, 2 };
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateFocusableGridItems(10, ITEM_WIDTH, ITEM_HEIGHT);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step2. Find target child with specified index parameters.
     * @tc.expected: Can not find the target focus child.
     */
    int32_t tarMainIndex = 1;
    int32_t tarCrossIndex = 1;
    auto IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    RefPtr<FocusHub> result = IrregularFocusableChild.Upgrade();
    EXPECT_EQ(result, nullptr);

    /**
     * @tc.steps: step3. Call the function when isLeftStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    pattern_->isLeftStep_ = true;
    IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    pattern_->isLeftStep_ = false;

    /**
     * @tc.steps: step4. Call the function when isRightStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    tarCrossIndex = 0;
    pattern_->isRightStep_ = true;
    IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    pattern_->isRightStep_ = false;

    /**
     * @tc.steps: step5. Call the function when isUpStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    pattern_->isUpStep_ = true;
    IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    pattern_->isUpStep_ = false;

    /**
     * @tc.steps: step6. Call the function when isDownStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    pattern_->isDownStep_ = true;
    IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    pattern_->isDownStep_ = false;

    /**
     * @tc.steps: step7. Call the function when isLeftEndStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    pattern_->isLeftEndStep_ = true;
    IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    pattern_->isLeftEndStep_ = false;

    /**
     * @tc.steps: step8. Call the function when isRightEndStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    pattern_->isRightEndStep_ = true;
    IrregularFocusableChild = pattern_->SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    pattern_->isRightEndStep_ = false;
}

/**
 * @tc.name: GridPattern_GetItemRect001
 * @tc.desc: Test the GetItemRect function of Grid.
 * @tc.type: FUNCgetitemre
 */
HWTEST_F(GridOptionLayoutTestNg, GridPattern_GetItemRect001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init Grid then slide Grid by Scroller.
     */
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 1, 3 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize { 1, 2 };
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetLayoutOptions(option);
    CreateGridItems(10, -2, ITEM_HEIGHT);
    CreateDone(frameNode_);
    pattern_->UpdateStartIndex(3, ScrollAlign::START);
    FlushLayoutTask(frameNode_);

    /**
     * @tc.steps: step2. Get invalid GridItem Rect.
     * @tc.expected: Return 0 when input invalid index.
     */
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(-1), Rect()));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(2), Rect()));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(10), Rect()));

    /**
     * @tc.steps: step3. Get valid GridItem Rect.
     * @tc.expected: Return actual Rect when input valid index.
     */
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(3), Rect(0, 0, GRID_WIDTH, ITEM_HEIGHT)));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(4), Rect(0, ITEM_HEIGHT, GRID_WIDTH / 2, ITEM_HEIGHT)));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(7), Rect(GRID_WIDTH / 2, ITEM_HEIGHT * 2, GRID_WIDTH / 2, ITEM_HEIGHT)));

    /**
     * @tc.steps: step4. Slide Grid by Scroller.
     */
    UpdateCurrentOffset(ITEM_HEIGHT + ITEM_HEIGHT / 2);

    /**
     * @tc.steps: step5. Get invalid GridItem Rect.
     * @tc.expected: Return 0 when input invalid index.
     */
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(-1), Rect()));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(0), Rect()));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(10), Rect()));

    /**
     * @tc.steps: step6. Get valid GridItem Rect.
     * @tc.expected: Return actual Rect when input valid index.
     */
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(1), Rect(0, -ITEM_HEIGHT / 2, GRID_WIDTH, ITEM_HEIGHT)));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(2), Rect(0, ITEM_HEIGHT / 2, GRID_WIDTH / 2, ITEM_HEIGHT)));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(3), Rect(0, ITEM_HEIGHT + ITEM_HEIGHT / 2, GRID_WIDTH, ITEM_HEIGHT)));
    EXPECT_TRUE(IsEqual(pattern_->GetItemRect(5),
        Rect(GRID_WIDTH / 2, ITEM_HEIGHT * 2 + ITEM_HEIGHT / 2, GRID_WIDTH / 2, ITEM_HEIGHT)));
}

/**
 * @tc.name: GridPattern_GetItemIndex001
 * @tc.desc: Test the GetItemIndex function of Grid.
 * @tc.type: FUNCgetitemre
 */
HWTEST_F(GridOptionLayoutTestNg, GridPattern_GetItemIndex001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init Grid then slide Grid by Scroller.
     */
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 1, 3 };
    auto onGetIrregularSizeByIndex = [](int32_t index) {
        GridItemSize gridItemSize { 1, 2 };
        return gridItemSize;
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetLayoutOptions(option);
    CreateGridItems(10, -2, ITEM_HEIGHT);
    CreateDone(frameNode_);
    pattern_->UpdateStartIndex(3, ScrollAlign::START);
    FlushLayoutTask(frameNode_);

    /**
     * @tc.steps: step2. Get invalid GridItem index.
     * @tc.expected: Return -1 when input invalid index.
     */
    EXPECT_TRUE(IsEqual(pattern_->GetItemIndex(100000, -100000), -1));

    /**
     * @tc.steps: step3. Get valid GridItem index.
     * @tc.expected: Return actual Rect when input valid index.
     */
    EXPECT_TRUE(IsEqual(pattern_->GetItemIndex(GRID_WIDTH / 2, ITEM_HEIGHT / 2), 3));
}

/**
 * @tc.name: LayoutUtils::GetItemSize001
 * @tc.desc: Test LayoutUtils::GetItemSize
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GetItemSize001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.irregularIndexes = {
        0, // [2 x 1]
        1, // [1 x 2]
        2  // [2 x 1]
    };
    auto onGetIrregularSizeByIndex = [](int32_t index) -> GridItemSize {
        if (index == 1) {
            return { .rows = 2, .columns = 1 };
        }
        return { .rows = 1, .columns = 2 };
    };

    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetLayoutOptions(option);
    CreateDone(frameNode_);

    GridLayoutInfo info;
    auto* wrapper = AceType::RawPtr(frameNode_);

    info.crossCount_ = 2;
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 0).rows, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 0).columns, 2);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 1).rows, 2);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 1).columns, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 2).rows, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 2).columns, 2);

    info.axis_ = Axis::HORIZONTAL;
    // rows and columns should be flipped when horizontal
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 0).rows, 2);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 0).columns, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 1).rows, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 1).columns, 2);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 2).rows, 2);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 2).columns, 1);
}

/**
 * @tc.name: LayoutUtils::GetItemSize002
 * @tc.desc: Test LayoutUtils::GetItemSize with null callback
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GetItemSize002, TestSize.Level1)
{
    GridLayoutOptions option;
    option.irregularIndexes = {
        0, // [2 x 1]
        1, // [1 x 2]
        2  // [2 x 1]
    };
    option.getSizeByIndex = nullptr;

    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetLayoutOptions(option);
    CreateGridItems(3, ITEM_WIDTH, NULL_VALUE, GridItemStyle::NONE);
    CreateDone(frameNode_);

    GridLayoutInfo info;
    auto* wrapper = AceType::RawPtr(frameNode_);
    info.crossCount_ = 3;

    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 0).rows, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 0).columns, 3);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 1).rows, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 1).columns, 3);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 2).rows, 1);
    EXPECT_EQ(GridLayoutUtils::GetItemSize(&info, wrapper, 2).columns, 3);
}

/**
 * @tc.name: GridLayout005
 * @tc.desc: Test GridLayoutAlgorithm for coverage
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridLayout005, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5 };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step2.call GetItemRect
     * @tc.expected: The GetItemRect is crrect
     */
    GridItemRect retItemRect;
    auto pattern = frameNode_->GetPattern<GridPattern>();
    auto algorithm = AceType::MakeRefPtr<GridLayoutAlgorithm>(GridLayoutInfo {}, 2, 5);
    auto childLayoutProperty = GetChildLayoutProperty<GridItemLayoutProperty>(frameNode_, 0);
    ASSERT_NE(layoutProperty_, nullptr);
    ASSERT_NE(childLayoutProperty, nullptr);
    algorithm->GetItemRect(layoutProperty_, childLayoutProperty, 0);
    EXPECT_EQ(retItemRect.rowStart, -1);
    EXPECT_EQ(retItemRect.rowSpan, 1);
    EXPECT_EQ(retItemRect.columnStart, -1);
    EXPECT_EQ(retItemRect.columnSpan, 1);
}

/**
 * @tc.name: GetEndOffset004
 * @tc.desc: test EndOffset when content < viewport
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GetEndOffset004, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions({ .irregularIndexes = { 1, 5 } });
    model.SetColumnsGap(Dimension { 5.0f });
    model.SetRowsGap(Dimension { 5.0f });
    CreateFixedHeightItems(6, 100.0f);
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    // make content smaller than viewport
    ViewAbstract::SetHeight(CalcLength(700.0f));
    CreateDone(frameNode_);
    auto& info = pattern_->gridLayoutInfo_;
    pattern_->scrollableEvent_->scrollable_->isTouching_ = true;
    // line height + gap = 105
    for (int i = 0; i < 160; ++i) {
        UpdateCurrentOffset(-50.0f);
        EXPECT_EQ(pattern_->GetEndOffset(), info.startMainLineIndex_ * 105.0f);
    }
    EXPECT_LE(info.currentOffset_, -1000.0f);
    EXPECT_GE(info.startMainLineIndex_, 3);
}

/**
 * @tc.name: TestChildrenUpdate001
 * @tc.desc: Test updating existing children and adding children
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, TestChildrenUpdate001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedHeightItems(2, 100.0f);
    model.SetLayoutOptions({});
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateDone(frameNode_);
    auto& info = pattern_->gridLayoutInfo_;
    pattern_->scrollableEvent_->scrollable_->isTouching_ = true;
    EXPECT_FALSE(pattern_->irregular_);
    for (int i = 0; i < 2; ++i) {
        frameNode_->ChildrenUpdatedFrom(i);
        frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        FlushLayoutTask(frameNode_);
        EXPECT_EQ(GetChildOffset(frameNode_, 0), OffsetF(0, 0));
        EXPECT_EQ(GetChildOffset(frameNode_, 1), OffsetF(GRID_WIDTH / 4.0f, 0));
        const decltype(info.gridMatrix_) cmp = { { 0, { { 0, 0 }, { 1, 1 } } } };
        EXPECT_EQ(info.gridMatrix_, cmp);
        EXPECT_EQ(info.lineHeightMap_.size(), 1);
    }

    AddFixedHeightItems(3, 100.0f);
    frameNode_->ChildrenUpdatedFrom(2);
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    const decltype(info.gridMatrix_) cmp2 = { { 0, { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 } } }, { 1, { { 0, 4 } } } };
    EXPECT_EQ(info.gridMatrix_, cmp2);
    EXPECT_EQ(info.lineHeightMap_.size(), 2);
    EXPECT_EQ(GetChildOffset(frameNode_, 4), OffsetF(0.0f, 100.0f));
}

/**
 * @tc.name: GridLayoutTest001
 * @tc.desc: Test Grid Measure with GrdiLayoutOptions
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridLayoutTest001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize = { 1, 1 };
    int32_t gridItems[4][4] = { { 0, 0, 1, 1 }, { 0, 1, 1, 1 }, { 0, 2, 1, 1 }, { 1, 0, 1, 1 } };

    auto onGetRectByIndex = [gridItems](int32_t index) {
        GridItemRect itemRect;
        itemRect.rowStart = gridItems[index][0];
        itemRect.columnStart = gridItems[index][1];
        itemRect.rowSpan = gridItems[index][2];
        itemRect.columnSpan = gridItems[index][3];
        return itemRect;
    };
    option.getRectByIndex = std::move(onGetRectByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetRowsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    model.SetColumnsGap(Dimension(COL_GAP));
    model.SetRowsGap(Dimension(ROW_GAP));
    CreateFixedItems(4);
    CreateDone(frameNode_);

    EXPECT_EQ(pattern_->GetGridLayoutInfo().startMainLineIndex_, 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().endMainLineIndex_, 1);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startIndex_, 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().endIndex_, 3);
}

/**
 * @tc.name: GridLayout006
 * @tc.desc: Test GridLayoutAlgorithm for coverage
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, GridLayout006, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    option.irregularIndexes = { 6, 1, 2, 3, 4, 5 };
    option.getRectByIndex = [](int32_t index) {
        GridItemRect tmpItemRect;
        tmpItemRect.rowStart = 1;
        tmpItemRect.rowSpan = 20;
        tmpItemRect.columnStart = 1;
        tmpItemRect.columnSpan = 20;
        return tmpItemRect;
    };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr");
    model.SetLayoutOptions(option);
    CreateFixedItems(10);
    CreateDone(frameNode_);
    pattern_->UpdateStartIndex(3);
    FlushLayoutTask(frameNode_);
    layoutProperty_->UpdateColumnsTemplate("1fr 1fr 1fr 1fr 1fr");

    /**
     * @tc.steps: step2.call GetItemRect
     * @tc.expected: The GetItemRect is crrect
     */
    GridItemRect retItemRect;
    auto pattern = frameNode_->GetPattern<GridPattern>();
    auto algorithm = AceType::MakeRefPtr<GridLayoutAlgorithm>(GridLayoutInfo {}, 2, 5);
    auto childLayoutProperty = GetChildLayoutProperty<GridItemLayoutProperty>(frameNode_, 0);
    ASSERT_NE(layoutProperty_, nullptr);
    ASSERT_NE(childLayoutProperty, nullptr);
    retItemRect = algorithm->GetItemRect(layoutProperty_, childLayoutProperty, 0);
    EXPECT_EQ(retItemRect.rowStart, 1);
    EXPECT_EQ(retItemRect.rowSpan, 20);
    EXPECT_EQ(retItemRect.columnStart, 1);
    EXPECT_EQ(retItemRect.columnSpan, 20);
}

/**
 * @tc.name: LayoutOptions001
 * @tc.desc: Test LayoutOptions
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, LayoutOptions001, TestSize.Level1)
{
    /**
     * @tc.cases: Set GridLayoutOptions:irregularIndexes
     * @tc.expected: Each gridItem rect is correct
     */
    GridLayoutOptions option;
    option.irregularIndexes = { 6, 1, 4 };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateGridItems(10, -2, ITEM_HEIGHT);
    CreateDone(frameNode_);
    EXPECT_EQ(GetChildRect(frameNode_, 0), RectF(0.f, ITEM_HEIGHT * 0, ITEM_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 1), RectF(0.f, ITEM_HEIGHT * 1, GRID_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 2), RectF(0.f, ITEM_HEIGHT * 2, ITEM_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 3), RectF(ITEM_WIDTH, ITEM_HEIGHT * 2, ITEM_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 4), RectF(0.f, ITEM_HEIGHT * 3, GRID_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 5), RectF()); // out of view
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 0)->GetCollectionItemInfo().row, 0);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 0)->GetCollectionItemInfo().column, 0);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 1)->GetCollectionItemInfo().row, 1);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 1)->GetCollectionItemInfo().column, 0);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 2)->GetCollectionItemInfo().row, 2);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 2)->GetCollectionItemInfo().column, 0);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 3)->GetCollectionItemInfo().row, 2);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 3)->GetCollectionItemInfo().column, 1);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 4)->GetCollectionItemInfo().row, 3);
    EXPECT_EQ(
        GetChildAccessibilityProperty<GridItemAccessibilityProperty>(frameNode_, 4)->GetCollectionItemInfo().column, 0);
}

/**
 * @tc.name: SyncLayoutBeforeSpring001
 * @tc.desc: Test SyncLayoutBeforeSpring's invariant
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, SyncLayoutBeforeSpring001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Test OnModifyDone
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions({});
    CreateFixedItems(10);
    CreateDone(frameNode_);
    EXPECT_EQ(GetChildY(frameNode_, 9), 400.0f);

    pattern_->gridLayoutInfo_.currentOffset_ = -100.0f;
    pattern_->gridLayoutInfo_.synced_ = false;
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    // in a realistic scenario, this function only gets called during spring animation.
    // here we only test the invariant that overScroll is enabled during the sync layout before spring animation.
    pattern_->SyncLayoutBeforeSpring();
    EXPECT_EQ(GetChildY(frameNode_, 9), 300.0f);
    EXPECT_TRUE(pattern_->gridLayoutInfo_.synced_);
    EXPECT_FALSE(pattern_->forceOverScroll_);
}

/**
 * @tc.name: LayoutOptions002
 * @tc.desc: Test LayoutOptions
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, LayoutOptions002, TestSize.Level1)
{
    /**
     * @tc.cases: Set GridLayoutOptions:irregularIndexes getSizeByIndex
     * @tc.expected: Each gridItem rect is correct
     */
    GridLayoutOptions option;
    option.irregularIndexes = { 6, 1, 3, 4, 5, 0 };
    GetSizeByIndex onGetIrregularSizeByIndex = [](int32_t index) {
        if (index == 3) {
            return GridItemSize { 1, 2 };
        }
        return GridItemSize { 1, 4 };
    };
    option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    CreateGridItems(10, -2, ITEM_HEIGHT);
    CreateDone(frameNode_);
    EXPECT_EQ(GetChildRect(frameNode_, 0), RectF(0.f, ITEM_HEIGHT * 0, GRID_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 1), RectF(0.f, ITEM_HEIGHT * 1, GRID_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 2), RectF(0.f, ITEM_HEIGHT * 2, ITEM_WIDTH * 1, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 3), RectF(ITEM_WIDTH, ITEM_HEIGHT * 2, ITEM_WIDTH * 2, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 4), RectF(0.f, ITEM_HEIGHT * 3, GRID_WIDTH, ITEM_HEIGHT));
    EXPECT_EQ(GetChildRect(frameNode_, 5), RectF()); // out of view
}

/**
 * @tc.name: OutOfBounds001
 * @tc.desc: Test LayoutOptions
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, OutOfBounds001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetLayoutOptions({});
    CreateFixedHeightItems(30, 200.0f);
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateDone(frameNode_);
    pattern_->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(GetChildRect(frameNode_, 29).Bottom(), GRID_HEIGHT);
    EXPECT_FALSE(pattern_->IsOutOfBoundary(true));

    pattern_->scrollableEvent_->scrollable_->isTouching_ = true;
    UpdateCurrentOffset(-100.0f);
    EXPECT_TRUE(pattern_->IsOutOfBoundary(true));

    UpdateCurrentOffset(150.0f);
    EXPECT_GT(GetChildRect(frameNode_, 29).Bottom(), GRID_HEIGHT);
    EXPECT_FALSE(pattern_->IsOutOfBoundary(true));
}

/**
 * @tc.name: ScrollTo001
 * @tc.desc: Test ScrollTo Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, ScrollTo001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.irregularIndexes = { 45 };
    GridModelNG model = CreateGrid();
    model.SetLayoutOptions(option);
    model.SetColumnsTemplate("1fr");
    CreateFixedItems(50);
    CreateDone(frameNode_);

    pattern_->ScrollTo(ITEM_HEIGHT * 40);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startIndex_, 40);

    pattern_->ScrollTo(ITEM_HEIGHT * 20);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startIndex_, 20);
}

/**
 * @tc.name: ShowCache001
 * @tc.desc: Test Grid showCache items
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, ShowCache001, TestSize.Level1)
{
    GridModelNG model = CreateRepeatGrid(50, [](uint32_t idx) { return 200.0f; });
    model.SetColumnsTemplate("1fr 1fr");
    model.SetLayoutOptions({});
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    model.SetCachedCount(1, true);
    CreateDone(frameNode_);
    const auto& info = pattern_->gridLayoutInfo_;
    EXPECT_EQ(info.startIndex_, 0);
    EXPECT_EQ(info.endIndex_, 7);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 8)->IsActive());
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 9)->IsActive());
    EXPECT_EQ(GetChildWidth(frameNode_, 8), 235.0f);
    EXPECT_EQ(GetChildY(frameNode_, 8), 840.0f);
    EXPECT_EQ(GetChildY(frameNode_, 9), 840.0f);

    UpdateCurrentOffset(-400.0f);
    EXPECT_EQ(info.startIndex_, 2);
    EXPECT_EQ(info.endIndex_, 11);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 12)->IsActive());
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 13)->IsActive());
    EXPECT_EQ(GetChildY(frameNode_, 10), 650.0f);
    EXPECT_EQ(GetChildY(frameNode_, 11), 650.0f);
    EXPECT_EQ(GetChildY(frameNode_, 12), 860.0f);
    EXPECT_EQ(GetChildY(frameNode_, 13), 860.0f);
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 0)->IsActive());
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 1)->IsActive());
    EXPECT_EQ(GetChildY(frameNode_, 0), -400.0f);
    EXPECT_EQ(GetChildY(frameNode_, 1), -400.0f);
}

/**
 * @tc.name: ShowCache002
 * @tc.desc: Test Grid showCache items and change width
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, ShowCache002, TestSize.Level1)
{
    GridModelNG model = CreateRepeatGrid(50, [](uint32_t idx) { return 200.0f; });
    model.SetColumnsTemplate("1fr 1fr");
    model.SetLayoutOptions({});
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    model.SetCachedCount(2, true);
    CreateDone(frameNode_);
    const auto& info = pattern_->gridLayoutInfo_;
    EXPECT_EQ(info.endIndex_, 7);
    EXPECT_EQ(GetChildY(frameNode_, 9), 840.0f);

    UpdateCurrentOffset(-550.0f);
    EXPECT_EQ(info.startIndex_, 4);
    EXPECT_EQ(info.endIndex_, 13);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(GetChildWidth(frameNode_, 15), 235.0f);
    EXPECT_EQ(GetChildX(frameNode_, 15), 245.0f);

    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(CalcLength(200.0f), CalcLength(800.0f)));
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(GetChildWidth(frameNode_, 14), 95.0f);
    EXPECT_EQ(GetChildWidth(frameNode_, 15), 95.0f);
    EXPECT_EQ(GetChildX(frameNode_, 15), 105.0f);
    EXPECT_EQ(GetChildWidth(frameNode_, 3), 95.0f);
    EXPECT_EQ(GetChildX(frameNode_, 3), 105.0f);
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 3)->IsActive());
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 14)->IsActive());
    EXPECT_EQ(GetChildY(frameNode_, 2), -340.0f);
    EXPECT_EQ(GetChildY(frameNode_, 14), 920.0f);
    EXPECT_EQ(info.gridMatrix_.size(), 9);

    UpdateCurrentOffset(-300.0f);
    EXPECT_EQ(info.startIndex_, 8);
    EXPECT_EQ(info.endIndex_, 15);
    EXPECT_EQ(GetChildY(frameNode_, 4), -430.0f);
    EXPECT_EQ(GetChildX(frameNode_, 5), 105.0f);
    EXPECT_EQ(GetChildY(frameNode_, 16), 830.0f);
    EXPECT_EQ(GetChildY(frameNode_, 19), 1040.0f);
    EXPECT_EQ(GetChildWidth(frameNode_, 19), 95.0f);
}

/**
 * @tc.name: Refresh001
 * @tc.desc: Test Grid nested in refresh.
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, Refresh001, TestSize.Level1)
{
    PipelineContext::GetCurrentContext()->SetMinPlatformVersion(12);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(12);
    RefreshModelNG refreshModel;
    refreshModel.Create();
    auto refreshNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());

    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    model.SetLayoutOptions({});
    CreateGridItems(3); // 0-height items
    CreateDone();
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 0.0f);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startIndex_, 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startMainLineIndex_, 0);

    GestureEvent info;
    info.SetMainVelocity(1200.f);
    info.SetMainDelta(100.f);
    auto scrollable = pattern_->GetScrollableEvent()->scrollable_;
    ASSERT_TRUE(scrollable);
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 0.0f);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startIndex_, 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startMainLineIndex_, 0);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(refreshNode->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);
    EXPECT_EQ(frameNode_->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);
    EXPECT_EQ(frameNode_->GetRenderContext()->GetTransformTranslate()->y.Value(), 100);
    EXPECT_EQ(GetChildY(frameNode_, 0), 0.0f);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 0.0f);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startIndex_, 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().startMainLineIndex_, 0);
    scrollable->HandleDragUpdate(info);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 0.0f);
    EXPECT_FALSE(pattern_->IsOutOfBoundary(true));
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(refreshNode->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);
    EXPECT_EQ(frameNode_->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);
    EXPECT_EQ(frameNode_->GetRenderContext()->GetTransformTranslate()->y.ToString(), "179.37px"); // friction
    EXPECT_EQ(GetChildY(frameNode_, 0), 0.0f);
    EXPECT_FALSE(pattern_->OutBoundaryCallback());

    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(frameNode_->GetRenderContext()->GetTransformTranslate()->y.ToString(), "245.45px");
    EXPECT_EQ(GetChildY(frameNode_, 0), 0.0f);
    EXPECT_EQ(scrollable->state_, Scrollable::AnimationState::IDLE);

    MockAnimationManager::GetInstance().TickByVelocity(200.0f);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(refreshNode->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);
    EXPECT_EQ(GetChildY(frameNode_, 0), 0.0f);
    EXPECT_EQ(frameNode_->GetRenderContext()->GetTransformTranslate()->y.ToString(), "445.45px");
    EXPECT_EQ(refreshNode->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(refreshNode->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);
    EXPECT_EQ(GetChildY(frameNode_, 0), 0.0f);
    EXPECT_EQ(frameNode_->GetRenderContext()->GetTransformTranslate()->y.Value(), 64);
    EXPECT_EQ(refreshNode->GetGeometryNode()->GetFrameOffset().GetY(), 0.f);
}

/**
 * @tc.name: OnScrollStart001
 * @tc.desc: Test Grid onScrollStart
 * @tc.type: FUNC
 */
HWTEST_F(GridOptionLayoutTestNg, OnScrollStart001, TestSize.Level1)
{
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    model.SetLayoutOptions({});

    int32_t count = 0;
    model.SetOnScrollStart([&count]() { ++count; });
    int32_t stopCount = 0;
    model.SetOnScrollStop([&stopCount]() { ++stopCount; });
    CreateGridItems(20, ITEM_WIDTH, ITEM_HEIGHT);
    CreateDone();

    pattern_->ScrollToIndex(10, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(count, 0);

    GestureEvent info;
    info.SetMainVelocity(1200.f);
    info.SetMainDelta(100.f);
    auto scrollable = pattern_->GetScrollableEvent()->scrollable_;
    (*scrollable->panRecognizerNG_->onActionStart_)(info);
    (*scrollable->panRecognizerNG_->onActionUpdate_)(info);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(count, 1);
    (*scrollable->panRecognizerNG_->onActionEnd_)(info);
    FlushLayoutTask(frameNode_);

    MockAnimationManager::GetInstance().TickByVelocity(2000.f);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(stopCount, 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 0.0f);
    EXPECT_EQ(scrollable->state_, Scrollable::AnimationState::FRICTION);

    MockAnimationManager::GetInstance().TickByVelocity(100.f);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(stopCount, 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 100.0f);
    EXPECT_EQ(scrollable->state_, Scrollable::AnimationState::SPRING);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(stopCount, 1);
    EXPECT_EQ(scrollable->state_, Scrollable::AnimationState::IDLE);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 0.0f);
}
} // namespace OHOS::Ace::NG

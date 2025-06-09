/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/render/mock_render_context.h"
#include "test/mock/core/rosen/mock_canvas.h"

#include "core/components_ng/pattern/grid/grid_item_pattern.h"
#include "core/components_ng/pattern/grid/grid_layout/grid_layout_algorithm.h"
#include "core/components_ng/pattern/grid/grid_scroll/grid_scroll_layout_algorithm.h"
#include "core/components_ng/pattern/text_field/text_field_manager.h"
#include "test/mock/core/animation/mock_animation_manager.h"

namespace OHOS::Ace::NG {

namespace {} // namespace

class GridLayoutTestNg : public GridTestNg {};

/**
 * @tc.name: AdaptiveLayout001
 * @tc.desc: Test property AdaptiveLayout
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayout001, TestSize.Level1)
{
    float itemWidth = 50.f;
    GridModelNG model = CreateGrid();
    model.SetLayoutDirection(FlexDirection::ROW);
    model.SetEditable(true);
    model.SetCellLength(ITEM_MAIN_SIZE);
    model.SetMinCount(4);
    model.SetMaxCount(2);
    CreateGridItems(10, itemWidth, ITEM_MAIN_SIZE);
    CreateDone();

    /**
     * @tc.steps: step1. While the before set minCount > maxCount
     * @tc.expected: would let minCount = 1, maxCount = Infinity;
     */
    int32_t colsNumber = 4;
    for (int32_t index = 0; index < 10; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = index % colsNumber * itemWidth;
        float offsetY = floor(index / colsNumber) * ITEM_MAIN_SIZE;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_MAIN_SIZE);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: GridItemGetInnerFocusPaintRectTest001
 * @tc.desc: GridItem GetInnerFocusPaintRect test.
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, GridItemGetInnerFocusPaintRectTest001, TestSize.Level1)
{
    CreateGrid();
    CreateFixedItems(10);
    CreateDone();
    auto gridItemNode = GetChildFrameNode(frameNode_, 0);
    auto focusHub = GetChildFocusHub(frameNode_, 0);
    auto GetInnerFocusPaintRect = focusHub->getInnerFocusRectFunc_;

    /**
     * @tc.steps: step1. Set paintRect when grid item does not have border radius.
     * @tc.expected: Focus border radius is equal to 4.0_vp.
     */
    RoundRect paintRect;
    GetInnerFocusPaintRect(paintRect);
    EdgeF radius = paintRect.GetCornerRadius(RoundRect::CornerPos::TOP_LEFT_POS);
    float expectRadius = GRIDITEM_FOCUS_INTERVAL.ConvertToPx();
    EXPECT_EQ(radius.x, expectRadius);
    EXPECT_EQ(radius.y, expectRadius);

    /**
     * @tc.steps: step2. Set paintRect when grid item has border radius.
     * @tc.expected: Focus border radius is equal to 11.0_vp.
     */
    auto renderContext = gridItemNode->GetRenderContext();
    renderContext->UpdateBorderRadius({ BORDER_RADIUS, BORDER_RADIUS, BORDER_RADIUS, BORDER_RADIUS });
    GetInnerFocusPaintRect(paintRect);
    radius = paintRect.GetCornerRadius(RoundRect::CornerPos::TOP_LEFT_POS);
    expectRadius = (GRIDITEM_FOCUS_INTERVAL + BORDER_RADIUS).ConvertToPx();
    EXPECT_EQ(radius.x, expectRadius);
    EXPECT_EQ(radius.y, expectRadius);
}

/**
 * @tc.name: SearchIrregularFocusableChildInNormalGrid001
 * @tc.desc: Test ability of a fixed shape grid to obtain irregular shape focal item.
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, SearchIrregularFocusableChildInNormalGrid001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create gridItems with irregular shape in fixed shape grid.
     */
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateBigItem(1, 2, 1, 2);
    CreateFocusableGridItems(10, ITEM_MAIN_SIZE, ITEM_MAIN_SIZE);
    CreateDone();

    auto& focus = pattern_->focusHandler_;
    /**
     * @tc.steps: step2. Find target child with specified index parameters.
     * @tc.expected: Can find the target focus child.
     */
    int32_t tarMainIndex = 1;
    int32_t tarCrossIndex = 1;
    focus.isLeftStep_ = true;
    auto IrregularFocusableChild = focus.SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    RefPtr<FocusHub> result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    focus.isLeftStep_ = false;

    /**
     * @tc.steps: step3. Call the function when isRightStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    focus.isRightStep_ = true;
    IrregularFocusableChild = focus.SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    focus.isRightStep_ = false;

    /**
     * @tc.steps: step4. Call the function when isUpStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    focus.isUpStep_ = true;
    IrregularFocusableChild = focus.SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    focus.isUpStep_ = false;

    /**
     * @tc.steps: step5. Call the function when isDownStep_ is true.
     * @tc.expected: Can find the target focus child.
     */
    focus.isDownStep_ = true;
    IrregularFocusableChild = focus.SearchIrregularFocusableChild(tarMainIndex, tarCrossIndex);
    result = IrregularFocusableChild.Upgrade();
    EXPECT_NE(result, nullptr);
    focus.isDownStep_ = false;
}

/**
 * @tc.name: GridLayout001
 * @tc.desc: Test GridLayoutAlgorithm for coverage
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, GridLayout001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(16);
    CreateDone();

    /**
     * @tc.steps: step1. call InitGridCeils
     * @tc.expected: The GetLayoutProperty is !nullptr
     */
    auto pattern = frameNode_->GetPattern<GridPattern>();
    auto algorithm = AceType::MakeRefPtr<GridLayoutAlgorithm>(GridLayoutInfo {}, 4, 4);
    algorithm->InitGridCeils(AceType::RawPtr(frameNode_), { 0.0f, 0.0f });
    algorithm->crossCount_ = 5;
    algorithm->mainCount_ = 5;
    algorithm->InitGridCeils(AceType::RawPtr(frameNode_), { 0.0f, 0.0f });
    EXPECT_NE(pattern->GetLayoutProperty<GridLayoutProperty>(), nullptr);
}

/**
 * @tc.name: GridLayout002
 * @tc.desc: Test GridLayoutAlgorithm for coverage
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, GridLayout002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("");
    model.SetRowsTemplate("");
    CreateFixedItems(16);
    CreateDone();

    /**
     * @tc.steps: step1. Change to smaller mainSize
     * @tc.expected: The GetLayoutProperty is correct
     */
    auto pattern = frameNode_->GetPattern<GridPattern>();
    auto algorithm = AceType::MakeRefPtr<GridLayoutAlgorithm>(GridLayoutInfo {}, 4, 4);
    algorithm->InitGridCeils(AceType::RawPtr(frameNode_), { 0.0f, 0.0f });
    algorithm->crossCount_ = 5;
    algorithm->mainCount_ = 5;
    algorithm->InitGridCeils(AceType::RawPtr(frameNode_), { 0.0f, 0.0f });
    EXPECT_NE(pattern->GetLayoutProperty<GridLayoutProperty>(), nullptr);
}

/**
 * @tc.name: GridLayout003
 * @tc.desc: Test GridLayoutAlgorithm for coverage
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, GridLayout003, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    std::string emptyString;
    model.SetColumnsTemplate(emptyString);
    model.SetRowsTemplate(emptyString);
    CreateFixedItems(16);
    CreateDone();

    /**
     * @tc.steps: step1. Change to smaller mainSize
     * @tc.expected: The GetLayoutProperty is correct
     */
    auto pattern = frameNode_->GetPattern<GridPattern>();
    auto algorithm = AceType::MakeRefPtr<GridLayoutAlgorithm>(GridLayoutInfo {}, 4, 4);
    algorithm->info_.currentOffset_ = 0.0f;
    auto layoutProperty = pattern->CreateLayoutProperty();
    algorithm->InitGridCeils(AceType::RawPtr(frameNode_), { 0.0f, 0.0f });
    algorithm->crossCount_ = 5;
    algorithm->mainCount_ = 5;
    algorithm->InitGridCeils(AceType::RawPtr(frameNode_), { 0.0f, 0.0f });
    EXPECT_NE(layoutProperty, nullptr);
}

/**
 * @tc.name: LayoutRTL001
 * @tc.desc: Test property ayout with Direction RTL
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, LayoutRTL001, TestSize.Level1)
{
    float itemWidth = 60.f;
    GridModelNG model = CreateGrid();
    model.SetIsRTL(TextDirection::RTL);
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    CreateGridItems(12, itemWidth, ITEM_MAIN_SIZE);
    CreateDone();

    int32_t colsNumber = 4;
    for (int32_t index = 0; index < 10; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = WIDTH - index % colsNumber * itemWidth - itemWidth;
        float offsetY = floor(index / colsNumber) * ITEM_MAIN_SIZE;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_MAIN_SIZE);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: AdaptiveLayoutRTL001
 * @tc.desc: Test property AdaptiveLayout with GridDirection Row and Direction RTL
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutRTL001, TestSize.Level1)
{
    float itemWidth = 50.f;
    GridModelNG model = CreateGrid();
    model.SetLayoutDirection(FlexDirection::ROW);
    model.SetEditable(true);
    model.SetCellLength(ITEM_MAIN_SIZE);
    model.SetMinCount(4);
    model.SetMaxCount(2);
    model.SetIsRTL(TextDirection::RTL);
    CreateGridItems(10, itemWidth, ITEM_MAIN_SIZE);
    CreateDone();

    /**
     * @tc.steps: step1. While the before set minCount > maxCount
     * @tc.expected: would let minCount = 1, maxCount = Infinity;
     */
    int32_t colsNumber = 4;
    for (int32_t index = 0; index < 10; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = WIDTH - index % colsNumber * itemWidth - itemWidth;
        float offsetY = floor(index / colsNumber) * ITEM_MAIN_SIZE;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_MAIN_SIZE);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: AdaptiveLayoutRTL002
 * @tc.desc: Test property AdaptiveLayout with GridDirection Column and Direction RTL
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutRTL002, TestSize.Level1)
{
    float itemWidth = 50.f;
    GridModelNG model = CreateGrid();
    model.SetLayoutDirection(FlexDirection::COLUMN);
    model.SetEditable(true);
    model.SetCellLength(ITEM_MAIN_SIZE);
    model.SetMinCount(4);
    model.SetMaxCount(2);
    model.SetIsRTL(TextDirection::RTL);
    CreateGridItems(10, itemWidth, ITEM_MAIN_SIZE);
    CreateDone();

    /**
     * @tc.steps: step1. While the before set minCount > maxCount
     * @tc.expected: would let minCount = 1, maxCount = Infinity;
     */
    int32_t colsNumber = 4;
    int32_t rowsNumber = 4;
    for (int32_t index = 0; index < 8; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = WIDTH - floor(index / rowsNumber) * itemWidth * 2 - itemWidth;
        float offsetY = index % colsNumber * ITEM_MAIN_SIZE;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_MAIN_SIZE);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: AdaptiveLayoutRTL003
 * @tc.desc: Test property AdaptiveLayout with GridDirection RowReverse and Direction RTL
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutRTL003, TestSize.Level1)
{
    float itemWidth = 50.f;
    GridModelNG model = CreateGrid();
    model.SetLayoutDirection(FlexDirection::ROW_REVERSE);
    model.SetEditable(true);
    model.SetCellLength(ITEM_MAIN_SIZE);
    model.SetMinCount(4);
    model.SetMaxCount(2);
    model.SetIsRTL(TextDirection::RTL);
    CreateGridItems(10, itemWidth, ITEM_MAIN_SIZE);
    CreateDone();

    /**
     * @tc.steps: step1. While the before set minCount > maxCount
     * @tc.expected: would let minCount = 1, maxCount = Infinity;
     */
    int32_t colsNumber = 4;
    for (int32_t index = 0; index < 10; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = index % colsNumber * itemWidth + (WIDTH - colsNumber * itemWidth);
        float offsetY = floor(index / colsNumber) * ITEM_MAIN_SIZE;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_MAIN_SIZE);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: AdaptiveLayoutRTL004
 * @tc.desc: Test property AdaptiveLayout with GridDirection ColumnReverse and Direction RTL
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutRTL004, TestSize.Level1)
{
    float itemWidth = 50.0f;
    GridModelNG model = CreateGrid();
    model.SetLayoutDirection(FlexDirection::COLUMN_REVERSE);
    model.SetEditable(true);
    model.SetCellLength(ITEM_MAIN_SIZE);
    model.SetMinCount(4);
    model.SetMaxCount(2);
    model.SetIsRTL(TextDirection::RTL);
    CreateGridItems(10, itemWidth, ITEM_MAIN_SIZE);
    CreateDone();

    /**
     * @tc.steps: step1. While the before set minCount > maxCount
     * @tc.expected: would let minCount = 1, maxCount = Infinity;
     */
    int32_t colsNumber = 4;
    int32_t rowsNumber = 4;
    for (int32_t index = 0; index < 8; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = WIDTH - floor(index / rowsNumber) * itemWidth * 2 - itemWidth;
        float offsetY = HEIGHT - index % colsNumber * ITEM_MAIN_SIZE - ITEM_MAIN_SIZE;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_MAIN_SIZE);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: LayoutWithAutoStretch001
 * @tc.desc: Test Grid Layout with auto-stretch
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, LayoutWithAutoStretch001, TestSize.Level1)
{
    float itemWidth = 40.0f;
    float itemHeight = 75.0f;
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("repeat(auto-stretch, 40)");
    model.SetRowsTemplate("repeat(auto-stretch, 75)");
    model.SetRowsGap(Dimension(5));
    model.SetColumnsGap(Dimension(5));
    CreateGridItems(25, itemWidth, itemHeight);
    CreateDone();

    int32_t rowsNumber = 5;
    int32_t columnsNumber = 5;
    float realColumnsGap = 10.0f;
    float realRowsGap = 6.25f;
    for (int32_t index = 0; index < 25; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = index % columnsNumber * (itemWidth + realColumnsGap);
        float offsetY = index / rowsNumber * (itemHeight + realRowsGap);
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, itemHeight);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: GridGetChildrenExpandedSize001
 * @tc.desc: Test Grid GetChildrenExpandedSize.
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, GridGetChildrenExpandedSize001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create gridItem and initialize related properties.
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(10));
    CreateGridItems(10, ITEM_MAIN_SIZE, ITEM_MAIN_SIZE);
    CreateDone();
    EXPECT_EQ(pattern_->GetChildrenExpandedSize(), SizeF(WIDTH, ITEM_MAIN_SIZE * 5 + 10 * 4));

    auto padding = 10.f;
    ViewAbstract::SetPadding(AceType::RawPtr(frameNode_), CalcLength(5.f));
    EXPECT_EQ(pattern_->GetChildrenExpandedSize(), SizeF(WIDTH - padding, ITEM_MAIN_SIZE * 5 + 10 * 4));

    ClearOldNodes();
    model = CreateGrid();
    model.SetLayoutDirection(FlexDirection::COLUMN);
    model.SetRowsTemplate("1fr 1fr");
    model.SetColumnsGap(Dimension(10));
    CreateGridItems(10, ITEM_MAIN_SIZE, ITEM_MAIN_SIZE);
    CreateDone();
    EXPECT_EQ(pattern_->GetChildrenExpandedSize(), SizeF(ITEM_MAIN_SIZE * 5 + 10 * 4, HEIGHT));

    ViewAbstract::SetPadding(AceType::RawPtr(frameNode_), CalcLength(5.f));
    EXPECT_EQ(pattern_->GetChildrenExpandedSize(), SizeF(ITEM_MAIN_SIZE * 5 + 10 * 4, HEIGHT - padding));
}

/**
 * @tc.name: AdaptiveLayoutCrossCountTest001
 * @tc.desc: Test AdaptiveLayout crossCount
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutCrossCountTest001, TestSize.Level1)
{
    GridModelNG model;
    RefPtr<ScrollControllerBase> positionController = model.CreatePositionController();
    RefPtr<ScrollProxy> scrollBarProxy = model.CreateScrollBarProxy();
    model.Create(positionController, scrollBarProxy);
    ViewAbstract::SetHeight(CalcLength(Infinity<float>()));
    ViewAbstract::SetWidth(CalcLength(Infinity<int32_t>()));
    model.SetLayoutDirection(FlexDirection::COLUMN);
    GridTestNg::GetGrid();
    CreateFixedItems(1);
    CreateDone();

    EXPECT_EQ(pattern_->GetCrossCount(), 1);
}

/**
 * @tc.name: AdaptiveLayoutCrossCountTest002
 * @tc.desc: Test AdaptiveLayout crossCount
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutCrossCountTest002, TestSize.Level1)
{
    GridModelNG model;
    RefPtr<ScrollControllerBase> positionController = model.CreatePositionController();
    RefPtr<ScrollProxy> scrollBarProxy = model.CreateScrollBarProxy();
    model.Create(positionController, scrollBarProxy);
    ViewAbstract::SetHeight(CalcLength(Infinity<float>()));
    ViewAbstract::SetWidth(CalcLength(Infinity<int32_t>()));
    model.SetLayoutDirection(FlexDirection::ROW);
    GridTestNg::GetGrid();
    CreateFixedItems(1);
    CreateDone();

    EXPECT_EQ(pattern_->GetCrossCount(), 1);
}

/**
 * @tc.name: AdaptiveLayoutCrossCountTest003
 * @tc.desc: Test AdaptiveLayout crossCount
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutCrossCountTest003, TestSize.Level1)
{
    GridModelNG model;
    RefPtr<ScrollControllerBase> positionController = model.CreatePositionController();
    RefPtr<ScrollProxy> scrollBarProxy = model.CreateScrollBarProxy();
    model.Create(positionController, scrollBarProxy);
    ViewAbstract::SetHeight(CalcLength(Infinity<float>()));
    ViewAbstract::SetWidth(CalcLength(Infinity<int32_t>()));
    model.SetLayoutDirection(FlexDirection::COLUMN_REVERSE);
    GridTestNg::GetGrid();
    CreateFixedItems(1);
    CreateDone();

    EXPECT_EQ(pattern_->GetCrossCount(), 1);
}

/**
 * @tc.name: AdaptiveLayoutCrossCountTest004
 * @tc.desc: Test AdaptiveLayout crossCount
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, AdaptiveLayoutCrossCountTest004, TestSize.Level1)
{
    GridModelNG model;
    RefPtr<ScrollControllerBase> positionController = model.CreatePositionController();
    RefPtr<ScrollProxy> scrollBarProxy = model.CreateScrollBarProxy();
    model.Create(positionController, scrollBarProxy);
    ViewAbstract::SetHeight(CalcLength(Infinity<float>()));
    ViewAbstract::SetWidth(CalcLength(Infinity<int32_t>()));
    model.SetLayoutDirection(FlexDirection::ROW_REVERSE);
    GridTestNg::GetGrid();
    CreateFixedItems(1);
    CreateDone();

    EXPECT_EQ(pattern_->GetCrossCount(), 1);
}

namespace {
int32_t EstimateIndex(float pos)
{
    auto lines = std::floor((pos - 105.0f) / 105.0f);
    return lines * 3 + 1;
}
} // namespace

/**
 * @tc.name: LargeDelta001
 * @tc.desc: Test Grid Scrolling with large delta.
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, LargeDelta001, TestSize.Level1)
{
    // Move to OptionTestNg later when Irregular layout is fixed
    GridLayoutOptions option;
    option.irregularIndexes = { 0 };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    model.SetColumnsGap(Dimension(COL_GAP));
    model.SetRowsGap(Dimension(ROW_GAP));
    CreateFixedItems(4000);
    CreateDone();

    const auto& info = pattern_->info_;
    UpdateCurrentOffset(-30000.0f);
    EXPECT_EQ(info.startIndex_, 853);
    EXPECT_EQ(EstimateIndex(30000.0f), 853);
    UpdateCurrentOffset(-300.0f);
    EXPECT_EQ(info.startIndex_, EstimateIndex(30300.0f));
    UpdateCurrentOffset(20000.0f);
    EXPECT_EQ(info.startIndex_, EstimateIndex(10300.0f));
    UpdateCurrentOffset(250.0f);
    EXPECT_EQ(info.startIndex_, EstimateIndex(10050.0f));

    UpdateCurrentOffset(-35000.0f);
    EXPECT_EQ(info.startIndex_, EstimateIndex(45050.0f));
}

/**
 * @tc.name: SpringEffect001
 * @tc.desc: change dataSource when overScroll at bottom, test the edgeEffect.
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, SpringEffect001, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(1);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(40);
    CreateDone();

    ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    EXPECT_EQ(pattern_->info_.startIndex_, 32);
    EXPECT_EQ(pattern_->info_.endIndex_, 39);

    GestureEvent info;
    info.SetMainVelocity(-1200.f);
    info.SetMainDelta(-400.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushUITasks();

    EXPECT_EQ(pattern_->info_.startIndex_, 38);
    EXPECT_EQ(pattern_->info_.endIndex_, 39);
    EXPECT_EQ(GetChildY(frameNode_, 39), 0);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 19);

    // remove the last child.
    frameNode_->RemoveChildAtIndex(39);
    frameNode_->ChildrenUpdatedFrom(39);

    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushUITasks();
    EXPECT_FLOAT_EQ(GetChildY(frameNode_, 38), -9.4070988);
    EXPECT_EQ(pattern_->info_.startIndex_, 38);
    EXPECT_EQ(pattern_->info_.endIndex_, 38);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 19);

    MockAnimationManager::GetInstance().Tick();
    FlushUITasks();
    EXPECT_EQ(pattern_->info_.startIndex_, 32);
    EXPECT_EQ(pattern_->info_.endIndex_, 38);
    EXPECT_FLOAT_EQ(pattern_->info_.currentOffset_, 0);
    EXPECT_FLOAT_EQ(GetChildY(frameNode_, 38), 300);
    EXPECT_EQ(GetChildRect(frameNode_, 38).Bottom(), 400.0f);
    EXPECT_TRUE(GetItem(38, true)->IsActive());
    EXPECT_TRUE(GetItem(34, true)->IsActive());
}

/**
 * @tc.name: EstimateHeight001
 * @tc.desc: Test EstimateHeight when have bigItem.
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, EstimateHeight001, TestSize.Level1)
{
    /*
     * 0 0
     * 0 0
     * 1 2
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(5));
    CreateBigItem(0, 1, 0, 1, 240.0f, 150.0f);
    CreateFixedItems(10);
    CreateDone();

    EXPECT_EQ(pattern_->info_.lineHeightMap_[0], 72.5f);
    EXPECT_EQ(pattern_->info_.lineHeightMap_[1], 72.5f);
    EXPECT_EQ(pattern_->info_.lineHeightMap_[2], ITEM_MAIN_SIZE);

    UpdateCurrentOffset(-100.0f);

    EXPECT_EQ(pattern_->info_.startIndex_, 0);
    EXPECT_EQ(pattern_->info_.startMainLineIndex_, 1);
    EXPECT_EQ(pattern_->info_.currentOffset_, -22.5);

    pattern_->isSmoothScrolling_ = true;
    pattern_->isConfigScrollable_ = true;
    auto tempGridLayoutInfo = pattern_->info_;
    pattern_->infoCopy_ = std::make_unique<GridLayoutInfo>(pattern_->info_);
    pattern_->info_ = tempGridLayoutInfo;
    EXPECT_EQ(pattern_->EstimateHeight(), 100.0f);
}

/**
 * @tc.name: GridLayoutInfo001
 * @tc.desc: Test Grid Layout Info
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, GridLayoutInfo001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.irregularIndexes = { 0 };
    auto onGetRectByIndex = [](int32_t index) {
        GridItemRect gridItemRect { 2, 2, 1, 1 };
        return gridItemRect;
    };
    option.getRectByIndex = std::move(onGetRectByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetRowsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    model.SetColumnsGap(Dimension(COL_GAP));
    model.SetRowsGap(Dimension(ROW_GAP));
    CreateFixedItems(1);
    CreateDone();

    EXPECT_EQ(pattern_->info_.startIndex_, 0);
    EXPECT_EQ(pattern_->info_.endIndex_, 0);
    EXPECT_EQ(pattern_->info_.startMainLineIndex_, 2);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 2);
}

/**
 * @tc.name: GridLayoutInfo002
 * @tc.desc: Test Grid Layout Info
 * @tc.type: FUNC
 */
HWTEST_F(GridLayoutTestNg, GridLayoutInfo002, TestSize.Level1)
{
    GridLayoutOptions option;
    auto onGetRectByIndex = [](int32_t index) {
        if (index == 0) {
            GridItemRect gridItemRect { 2, 2, 1, 1 };
            return gridItemRect;
        } else {
            GridItemRect gridItemRect { 1, 1, 1, 1 };
            return gridItemRect;
        }
    };
    option.getRectByIndex = std::move(onGetRectByIndex);
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetRowsTemplate("1fr 1fr 1fr");
    model.SetLayoutOptions(option);
    model.SetColumnsGap(Dimension(COL_GAP));
    model.SetRowsGap(Dimension(ROW_GAP));
    CreateFixedItems(2);
    CreateDone();

    EXPECT_EQ(pattern_->info_.startIndex_, 0);
    EXPECT_EQ(pattern_->info_.endIndex_, 1);
    EXPECT_EQ(pattern_->info_.startMainLineIndex_, 1);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 2);
}
} // namespace OHOS::Ace::NG

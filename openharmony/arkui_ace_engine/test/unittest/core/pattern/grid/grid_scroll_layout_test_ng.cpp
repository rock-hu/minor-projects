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
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/render/mock_render_context.h"
#include "test/mock/core/rosen/mock_canvas.h"

#include "core/components_ng/pattern/grid/grid_item_pattern.h"
#include "core/components_ng/pattern/grid/grid_layout/grid_layout_algorithm.h"
#include "core/components_ng/pattern/text_field/text_field_manager.h"
#include "core/components_ng/pattern/grid/grid_scroll/grid_scroll_layout_algorithm.h"

namespace OHOS::Ace::NG {

namespace {} // namespace

class GridScrollLayoutTestNg : public GridTestNg {
public:
    void UpdateLayoutInfo();
    void UpdateLayoutWrapper(RefPtr<FrameNode>& frameNode, float width = GRID_WIDTH, float height = GRID_HEIGHT);
};

void GridScrollLayoutTestNg::UpdateLayoutInfo()
{
    GetGrid();
    ViewStackProcessor::GetInstance()->Finish();
    FlushLayoutTask(frameNode_);
    pattern_->gridLayoutInfo_.lineHeightMap_[0] = ITEM_HEIGHT;
    pattern_->gridLayoutInfo_.gridMatrix_[0][0] = 0;
    pattern_->gridLayoutInfo_.gridMatrix_[0][1] = 1;
    pattern_->gridLayoutInfo_.gridMatrix_[1][0] = 0;
    pattern_->gridLayoutInfo_.gridMatrix_[1][1] = 1;
}

void GridScrollLayoutTestNg::UpdateLayoutWrapper(RefPtr<FrameNode>& frameNode, float width, float height)
{
    LayoutConstraintF LayoutConstraint;
    LayoutConstraint.parentIdealSize = { GRID_WIDTH, GRID_HEIGHT };
    LayoutConstraint.percentReference = { GRID_WIDTH, GRID_HEIGHT };
    LayoutConstraint.selfIdealSize = { width, height };
    LayoutConstraint.maxSize = { width, GRID_HEIGHT };
    frameNode->Measure(LayoutConstraint);
    frameNode->Layout();
}

/**
 * @tc.name: GridPaintMethodTest001
 * @tc.desc: Test grid paint method GetForegroundDrawFunction function
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridPaintMethodTest001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);
    auto paintMethod = AceType::DynamicCast<GridPaintMethod>(pattern_->CreateNodePaintMethod());
    auto paintProperty = pattern_->CreatePaintProperty();
    PaintWrapper paintWrapper(frameNode_->GetRenderContext(), frameNode_->GetGeometryNode(), paintProperty);
    auto drawFunction = paintMethod->GetForegroundDrawFunction(&paintWrapper);
    Testing::MockCanvas rsCanvas;
    EXPECT_CALL(rsCanvas, DetachPen()).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, AttachBrush(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DrawRect(_)).WillRepeatedly(Return());
    drawFunction(rsCanvas);
    paintMethod = nullptr;
    drawFunction(rsCanvas);

    /**
     * @tc.steps: step1. When EdgeEffect::SPRING
     * @tc.expected: CreateNodePaintMethod would not trigger SetEdgeEffect
     */
    pattern_->SetEdgeEffect(EdgeEffect::SPRING);
    paintMethod = AceType::DynamicCast<GridPaintMethod>(pattern_->CreateNodePaintMethod());
    EXPECT_EQ(paintMethod->edgeEffect_.Upgrade(), nullptr);

    /**
     * @tc.steps: step2. When EdgeEffect::FADE
     * @tc.expected: CreateNodePaintMethod would trigger SetEdgeEffect
     */
    pattern_->SetEdgeEffect(EdgeEffect::FADE);
    paintMethod = AceType::DynamicCast<GridPaintMethod>(pattern_->CreateNodePaintMethod());
    EXPECT_NE(paintMethod->edgeEffect_.Upgrade(), nullptr);
}

/**
 * @tc.name: ScrollLayout001
 * @tc.desc: Test UpdateOffsetOnVirtualKeyboardHeightChange
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, ScrollLayout001, TestSize.Level1)
{
    auto textFieldManager = AceType::MakeRefPtr<TextFieldManagerNG>();
    MockPipelineContext::GetCurrent()->SetTextFieldManager(textFieldManager);
    auto textFieldPattern = AceType::MakeRefPtr<Pattern>();
    textFieldManager->SetOnFocusTextField(textFieldPattern);
    const Offset clickPosition = Offset(100.f, GRID_HEIGHT + ITEM_HEIGHT);
    textFieldManager->SetClickPosition(clickPosition);

    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetMaxCount(2);
    CreateFixedItems(18);
    CreateDone(frameNode_);
    const float smallerHeight = GRID_HEIGHT - ITEM_HEIGHT;
    RectF gridRect(0.f, 0.f, GRID_WIDTH, smallerHeight);
    auto mockRenderContext = AceType::DynamicCast<MockRenderContext>(frameNode_->renderContext_);
    mockRenderContext->rect_ = gridRect;

    /**
     * @tc.steps: step1. Change to smaller mainSize
     * @tc.expected: The mainSize is correct
     */
    auto gridFocus = frameNode_->GetOrCreateFocusHub();
    gridFocus->RequestFocusImmediately();
    // change grid height and trigger Measure
    UpdateLayoutWrapper(frameNode_, GRID_WIDTH, smallerHeight);
    float gridHeight = frameNode_->GetGeometryNode()->GetFrameSize().Height();
    EXPECT_FLOAT_EQ(gridHeight, smallerHeight);
}

/**
 * @tc.name: ScrollLayout002
 * @tc.desc: Test UpdateOffsetOnVirtualKeyboardHeightChange that currentOffset wuold not change
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, ScrollLayout002, TestSize.Level1)
{
    auto textFieldManager = AceType::MakeRefPtr<TextFieldManagerNG>();
    MockPipelineContext::GetCurrent()->SetTextFieldManager(textFieldManager);
    auto textFieldPattern = AceType::MakeRefPtr<Pattern>();
    textFieldManager->SetOnFocusTextField(textFieldPattern);
    const Offset clickPosition = Offset(100.f, 100.f);
    textFieldManager->SetClickPosition(clickPosition);

    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    model.SetMaxCount(2);
    CreateFixedItems(18);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step1. While axis_ == Axis::HORIZONTAL
     * @tc.expected: currentOffset_ would not change
     */
    pattern_->gridLayoutInfo_.axis_ = Axis::HORIZONTAL;
    const float smallerHeight = GRID_HEIGHT - ITEM_HEIGHT;
    // change grid height and trigger Measure
    UpdateLayoutWrapper(frameNode_, GRID_WIDTH, smallerHeight);
    float currentOffset = pattern_->gridLayoutInfo_.currentOffset_;
    EXPECT_FLOAT_EQ(currentOffset, 0.f);

    /**
     * @tc.steps: step2. While Grid !IsCurrentFocus()
     * @tc.expected: currentOffset_ would not change
     */
    UpdateLayoutWrapper(frameNode_, GRID_WIDTH, GRID_HEIGHT); // reset Grid height
    pattern_->gridLayoutInfo_.axis_ = Axis::VERTICAL;
    // change grid height and trigger Measure
    UpdateLayoutWrapper(frameNode_, GRID_WIDTH, smallerHeight);
    currentOffset = pattern_->gridLayoutInfo_.currentOffset_;
    EXPECT_FLOAT_EQ(currentOffset, 0.f);

    /**
     * @tc.steps: step3. While clickPosition is in Grid
     * @tc.expected: currentOffset_ would not change
     */
    UpdateLayoutWrapper(frameNode_, GRID_WIDTH, GRID_HEIGHT); // reset Grid height
    pattern_->gridLayoutInfo_.axis_ = Axis::VERTICAL;
    // change grid height and trigger Measure
    UpdateLayoutWrapper(frameNode_, GRID_WIDTH, smallerHeight);
    currentOffset = pattern_->gridLayoutInfo_.currentOffset_;
    EXPECT_FLOAT_EQ(currentOffset, 0.f);
}


/**
 * @tc.name: GridScrollTest001
 * @tc.desc: Test FireOnScrollBarUpdate Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridScrollTest001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    ScrollBarUpdateFunc scrollFunc = [](int32_t index, Dimension offset) {
        std::optional<float> horizontalOffset = offset.ConvertToPx();
        std::optional<float> verticalOffset = offset.ConvertToPx();
        return std::make_pair(horizontalOffset, verticalOffset);
    };
    model.SetRowsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(5));
    model.SetOnScrollBarUpdate(std::move(scrollFunc));
    CreateBigItem(1, 1, 1, 2);
    CreateFixedItems(2);
    CreateDone(frameNode_);
    Dimension offset(1.0);
    auto fireOnScroll = eventHub_->FireOnScrollBarUpdate(1.0, offset);
    EXPECT_FLOAT_EQ(fireOnScroll.first.value(), 1.0f);
    EXPECT_FLOAT_EQ(fireOnScroll.second.value(), 1.0f);
}

/**
 * @tc.name: GridScrollTest002
 * @tc.desc: Test CalculateLargeItemOffset Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridScrollTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create gridItem and initialize related properties.
     */
    GridModelNG model;
    model.Create(nullptr, nullptr);
    ViewAbstract::SetWidth(CalcLength(GRID_WIDTH));
    ViewAbstract::SetHeight(CalcLength(GRID_HEIGHT));
    model.SetRowsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(5));
    UpdateLayoutInfo();
    auto gridScrollLayoutAlgorithm = AceType::MakeRefPtr<GridScrollLayoutAlgorithm>(pattern_->gridLayoutInfo_, 2, 0);
    ASSERT_NE(gridScrollLayoutAlgorithm, nullptr);
    auto ret = gridScrollLayoutAlgorithm->CalculateLargeItemOffset(OffsetF(100, 100), 0, 1, 0);
    EXPECT_EQ(ret.GetY(), 100.f - ITEM_HEIGHT);
    EXPECT_EQ(ret.GetX(), 100.f);
}

/**
 * @tc.name: GridScrollTest003
 * @tc.desc: Test CalculateLargeItemOffset Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridScrollTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create gridItem and initialize related properties.
     */
    GridModelNG model;
    model.Create(nullptr, nullptr);
    ViewAbstract::SetWidth(CalcLength(GRID_WIDTH));
    ViewAbstract::SetHeight(CalcLength(GRID_HEIGHT));
    model.SetColumnsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(5));
    CreateFixedItems(10);
    UpdateLayoutInfo();
    auto gridScrollLayoutAlgorithm = AceType::MakeRefPtr<GridScrollLayoutAlgorithm>(pattern_->gridLayoutInfo_, 2, 0);
    ASSERT_NE(gridScrollLayoutAlgorithm, nullptr);
    auto ret = gridScrollLayoutAlgorithm->CalculateLargeItemOffset(OffsetF(0, 100), 1, 1, 0);
    EXPECT_EQ(ret.GetY(), 100.0f);
    EXPECT_EQ(ret.GetX(), 0.0f);
}

/**
 * @tc.name: GridScrollTest004
 * @tc.desc: Test CalculateLargeItemOffset Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridScrollTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create gridItem and initialize related properties.
     */
    GridModelNG model;
    model.Create(nullptr, nullptr);
    ViewAbstract::SetWidth(CalcLength(GRID_WIDTH));
    ViewAbstract::SetHeight(CalcLength(GRID_HEIGHT));
    model.SetRowsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(5));
    UpdateLayoutInfo();
    auto gridScrollLayoutAlgorithm = AceType::MakeRefPtr<GridScrollLayoutAlgorithm>(pattern_->gridLayoutInfo_, 2, 0);
    auto ret1 = gridScrollLayoutAlgorithm->CalculateLargeItemOffset(OffsetF(0, 100), 1, 1, 0);
    EXPECT_EQ(ret1.GetY(), 100.0f);
    EXPECT_EQ(ret1.GetX(), 0.0f);
}

/**
 * @tc.name: GridScrollTest005
 * @tc.desc: Test AdjustRowColSpan Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridScrollTest005, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    GridItemModelNG itemModel;
    itemModel.Create(GridItemStyle::NONE);
    itemModel.SetRowStart(NULL_VALUE);
    itemModel.SetRowEnd(NULL_VALUE);
    itemModel.SetColumnStart(NULL_VALUE);
    itemModel.SetColumnEnd(NULL_VALUE);
    ViewStackProcessor::GetInstance()->Pop();
    CreateDone(frameNode_);
    auto layoutProperty = GetChildLayoutProperty<GridItemLayoutProperty>(frameNode_, 0);
    EXPECT_EQ(layoutProperty->GetRowStart(), NULL_VALUE);
    EXPECT_EQ(layoutProperty->GetRowEnd(), NULL_VALUE);
    EXPECT_EQ(layoutProperty->GetColumnStart(), NULL_VALUE);
    EXPECT_EQ(layoutProperty->GetColumnEnd(), NULL_VALUE);
}

/**
 * @tc.name: GetTotalHeight001
 * @tc.desc: Test GetTotalHeight Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetTotalHeight001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create 10 gridItem
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetTotalHeight(), ITEM_HEIGHT * 3);

    /**
     * @tc.steps: step2. Create 20 gridItem
     */
    model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetTotalHeight(), ITEM_HEIGHT * 5);
}

/**
 * @tc.name: GetAverageHeight001
 * @tc.desc: Test GetAverageHeight Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetAverageHeight001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create 10 gridItem
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetAverageHeight(), 60);

    /**
     * @tc.steps: step2. Create 20 gridItem
     */
    ClearOldNodes();
    model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetAverageHeight(), 50);
}

/**
 * @tc.name: ChangeItemNumber001
 * @tc.desc: Test ChangeItemNumber
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, ChangeItemNumber001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(5);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step1. Add item
     * @tc.expected: The added item in the correct position
     */
    for (int32_t i = 0; i < 4; i++) {
        GridItemModelNG itemModel;
        itemModel.Create(GridItemStyle::NONE);
        ViewAbstract::SetHeight(CalcLength(Dimension(ITEM_HEIGHT)));
        RefPtr<UINode> currentNode = ViewStackProcessor::GetInstance()->Finish();
        auto currentFrameNode = AceType::DynamicCast<FrameNode>(currentNode);
        currentFrameNode->MountToParent(frameNode_);
    }
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(IsEqual(GetChildOffset(frameNode_, 6), OffsetF(ITEM_WIDTH * 2, ITEM_HEIGHT)));
    EXPECT_TRUE(IsEqual(GetChildOffset(frameNode_, 8), OffsetF(0, ITEM_HEIGHT * 2)));
}


/**
 * @tc.name: UpdateGridMatrix001
 * @tc.desc: Test UpdateGridMatrix
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, UpdateGridMatrix001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create 100 items
     * @tc.expected: The added item in the correct position
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateGridItems(100, ITEM_WIDTH, NULL_VALUE, GridItemStyle::NONE);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step2. Scroll To 99 item
     * @tc.expected: Scroll to the correct position,lineHeightMap_ size is 25
     */
    pattern_->ScrollToIndex(99, true, ScrollAlign::END);
    EXPECT_TRUE(IsEqual<int32_t>(pattern_->gridLayoutInfo_.lineHeightMap_.size(), 25));
}


/**
 * @tc.name: GridLayout004
 * @tc.desc: Test GridLayoutAlgorithm for coverage
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridLayout004, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step1. isVertical_ is true
     * @tc.expected: The curRow and curCol is correct
     */
    int32_t curRow = 0;
    int32_t curCol = 0;
    auto pattern = frameNode_->GetPattern<GridPattern>();
    auto algorithm = AceType::MakeRefPtr<GridLayoutAlgorithm>(GridLayoutInfo {}, 2, 5);
    EXPECT_EQ(algorithm->crossCount_, 2);
    EXPECT_EQ(algorithm->mainCount_, 5);
    algorithm->GetNextGrid(curRow, curCol);
    EXPECT_EQ(curRow, 0);
    EXPECT_EQ(curCol, 1);
    EXPECT_TRUE(algorithm->isVertical_);
    algorithm->GetNextGrid(curRow, curCol);
    EXPECT_EQ(curRow, 1);
    EXPECT_EQ(curCol, 0);
    curRow = 1;
    curCol = 1;

    /**
     * @tc.steps: step2. isVertical_ is false
     * @tc.expected: The curRow and curCol is correct
     */
    algorithm->isVertical_ = false;
    algorithm->GetNextGrid(curRow, curCol);
    EXPECT_EQ(curRow, 2);
    EXPECT_EQ(curCol, 1);
    algorithm->GetNextGrid(curRow, curCol);
    EXPECT_EQ(curRow, 3);
    EXPECT_EQ(curCol, 1);
    curRow = 5;
    algorithm->GetNextGrid(curRow, curCol);
    EXPECT_EQ(curRow, 0);
    EXPECT_EQ(curCol, 2);
}

/**
 * @tc.name: UpdateOverlayModifier001
 * @tc.desc: Test grid paint method UpdateOverlayModifier function
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, UpdateOverlayModifier001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step1. create paintMethod
     */
    auto paintMethod = AceType::DynamicCast<GridPaintMethod>(pattern_->CreateNodePaintMethod());
    auto paintProperty = pattern_->CreatePaintProperty();
    PaintWrapper paintWrapper(frameNode_->GetRenderContext(), frameNode_->GetGeometryNode(), paintProperty);
    paintMethod->UpdateOverlayModifier(nullptr);

    /**
     * @tc.steps: step2. call UpdateOverlayModifier
     * @tc.expected: scrollBar is nullptr
     */
    paintMethod->UpdateOverlayModifier(&paintWrapper);
    auto scrollBar = paintMethod->scrollBar_.Upgrade();
    EXPECT_EQ(scrollBar, nullptr);
}

/**
 * @tc.name: UpdateOverlayModifier002
 * @tc.desc: Test grid paint method UpdateOverlayModifier function
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, UpdateOverlayModifier002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step1. create scrollBar and paintMethod
     * @tc.expected: scrollBar is !nullptr
     */
    pattern_->scrollBar_ = AceType::MakeRefPtr<ScrollBar>(DisplayMode::AUTO);
    auto paintMethod = AceType::DynamicCast<GridPaintMethod>(pattern_->CreateNodePaintMethod());
    auto paintProperty = pattern_->CreatePaintProperty();
    PaintWrapper paintWrapper(frameNode_->GetRenderContext(), frameNode_->GetGeometryNode(), paintProperty);
    paintMethod->UpdateOverlayModifier(nullptr);
    auto scrollBar = paintMethod->scrollBar_.Upgrade();
    EXPECT_NE(scrollBar, nullptr);

    /**
     * @tc.steps: step2. call UpdateOverlayModifier
     * @tc.expected: AnimationTyp is correct
     */
    paintMethod->UpdateOverlayModifier(&paintWrapper);
    EXPECT_EQ(scrollBar->GetOpacityAnimationType(), OpacityAnimationType::NONE);
    EXPECT_EQ(scrollBar->GetHoverAnimationType(), HoverAnimationType::NONE);

    /**
     * @tc.steps: step3. call UpdateOverlayModifier when scrollBarOverlayModifier is seted
     * @tc.expected: AnimationTyp is correct
     */
    pattern_->CreateScrollBarOverlayModifier();
    EXPECT_EQ(pattern_->scrollBarOverlayModifier_, nullptr);
    paintMethod->SetScrollBarOverlayModifier(pattern_->GetScrollBarOverlayModifier());
    auto scrollBarOverlayModifier = paintMethod->scrollBarOverlayModifier_.Upgrade();
    EXPECT_EQ(scrollBarOverlayModifier, nullptr);
    paintMethod->UpdateOverlayModifier(&paintWrapper);
    EXPECT_EQ(scrollBar->GetOpacityAnimationType(), OpacityAnimationType::NONE);
    EXPECT_EQ(scrollBar->GetHoverAnimationType(), HoverAnimationType::NONE);
    scrollBar = nullptr;
}

/**
 * @tc.name: UpdateOverlayModifier003
 * @tc.desc: Test grid paint method UpdateOverlayModifier function
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, UpdateOverlayModifier003, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step1. create scrollBar and paintMethod and ScrollBarOverlayModifier
     * @tc.expected: scrollBar is !nullptr
     */
    pattern_->scrollBar_ = AceType::MakeRefPtr<ScrollBar>(DisplayMode::AUTO);
    pattern_->scrollBar_->SetScrollable(true);
    auto paintMethod = AceType::DynamicCast<GridPaintMethod>(pattern_->CreateNodePaintMethod());
    auto paintProperty = pattern_->CreatePaintProperty();
    PaintWrapper paintWrapper(frameNode_->GetRenderContext(), frameNode_->GetGeometryNode(), paintProperty);
    EXPECT_TRUE(pattern_->scrollBar_->NeedPaint());
    pattern_->CreateScrollBarOverlayModifier();
    EXPECT_NE(pattern_->scrollBarOverlayModifier_, nullptr);
    paintMethod->SetScrollBarOverlayModifier(pattern_->GetScrollBarOverlayModifier());
    auto scrollBarOverlayModifier = paintMethod->scrollBarOverlayModifier_.Upgrade();
    EXPECT_NE(scrollBarOverlayModifier, nullptr);
    auto scrollBar = paintMethod->scrollBar_.Upgrade();
    EXPECT_NE(scrollBar, nullptr);

    /**
     * @tc.steps: step2. call UpdateOverlayModifier
     * @tc.expected: AnimationTyp is correct
     */
    paintMethod->UpdateOverlayModifier(&paintWrapper);
    EXPECT_EQ(scrollBar->GetOpacityAnimationType(), OpacityAnimationType::NONE);
    EXPECT_EQ(scrollBar->GetHoverAnimationType(), HoverAnimationType::NONE);

    /**
     * @tc.steps: step3. change PositionMode and call UpdateOverlayModifier
     * @tc.expected: AnimationTyp is correct
     */
    scrollBar->SetPositionMode(PositionMode::BOTTOM);
    paintMethod->UpdateOverlayModifier(&paintWrapper);
    scrollBar = paintMethod->scrollBar_.Upgrade();
    EXPECT_EQ(scrollBar->GetOpacityAnimationType(), OpacityAnimationType::NONE);
    EXPECT_EQ(scrollBar->GetHoverAnimationType(), HoverAnimationType::NONE);
    scrollBar = nullptr;
}

/**
 * @tc.name: PaintEdgeEffect001
 * @tc.desc: Test grid paint method PaintEdgeEffect function
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, PaintEdgeEffect001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);

    /**
     * @tc.steps: step1. init scrollBar
     * @tc.expected: scrollBar is !nullptr
     */
    pattern_->scrollBar_ = AceType::MakeRefPtr<ScrollBar>(DisplayMode::AUTO);
    pattern_->scrollBar_->SetScrollable(true);
    auto paintMethod = AceType::DynamicCast<GridPaintMethod>(pattern_->CreateNodePaintMethod());
    auto paintProperty = pattern_->CreatePaintProperty();
    PaintWrapper paintWrapper(frameNode_->GetRenderContext(), frameNode_->GetGeometryNode(), paintProperty);
    EXPECT_TRUE(pattern_->scrollBar_->NeedPaint());
    pattern_->CreateScrollBarOverlayModifier();
    EXPECT_NE(pattern_->scrollBarOverlayModifier_, nullptr);
    paintMethod->SetScrollBarOverlayModifier(pattern_->GetScrollBarOverlayModifier());
    auto scrollBarOverlayModifier = paintMethod->scrollBarOverlayModifier_.Upgrade();
    EXPECT_NE(scrollBarOverlayModifier, nullptr);
    auto scrollBar = paintMethod->scrollBar_.Upgrade();
    EXPECT_NE(scrollBar, nullptr);

    /**
     * @tc.steps: step2. call PaintEdgeEffect
     * @tc.expected: edgeEffect_ is !nullptr
     */
    auto scrollEdgeEffect = AceType::MakeRefPtr<ScrollEdgeEffect>();
    paintMethod->SetEdgeEffect(scrollEdgeEffect);
    Testing::MockCanvas rsCanvas;
    EXPECT_CALL(rsCanvas, DetachPen()).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, AttachBrush(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DrawRect(_)).WillRepeatedly(Return());
    EXPECT_CALL(rsCanvas, DetachBrush()).WillRepeatedly(ReturnRef(rsCanvas));
    paintMethod->PaintEdgeEffect(nullptr, rsCanvas);
    paintMethod->PaintEdgeEffect(&paintWrapper, rsCanvas);
    EXPECT_NE(paintMethod->edgeEffect_.Upgrade(), nullptr);
}

/**
 * @tc.name: GridScrollTest006
 * @tc.desc: Test SetOnScrollBarUpdate Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridScrollTest006, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    ScrollBarUpdateFunc scrollFunc = [](int32_t index, Dimension offset) {
        std::optional<float> horizontalOffset = offset.ConvertToPx();
        std::optional<float> verticalOffset = offset.ConvertToPx();
        return std::make_pair(horizontalOffset, verticalOffset);
    };
    model.SetRowsTemplate("1fr 1fr");
    CreateFixedItems(2);
    model.SetGridHeight(Dimension(5));
    model.SetScrollBarMode(DisplayMode::AUTO);
    model.SetScrollBarColor("#FF0000");
    model.SetScrollBarWidth("10vp");
    model.SetIsRTL(TextDirection::LTR);

    NestedScrollOptions nestedOpt;
    model.SetNestedScroll(std::move(nestedOpt));
    ScrollToIndexFunc value;
    model.SetOnScrollToIndex(std::move(value));
    CreateDone(frameNode_);
    auto paintProperty = frameNode_->GetPaintProperty<ScrollablePaintProperty>();
    EXPECT_EQ(paintProperty->GetBarStateString(), "BarState.Auto");

    auto pattern = frameNode_->GetPattern<GridPattern>();
    EXPECT_TRUE(pattern->isConfigScrollable_);
    auto eventHub = frameNode_->GetEventHub<GridEventHub>();
    EXPECT_FALSE(eventHub->onScrollToIndex_);
}

/**
 * @tc.name: GridSCroll001
 * @tc.desc: Test SetSelected Function.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GridSCroll001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create GridItemModelNG object
     */
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    GridItemModelNG itemModel;
    itemModel.Create(GridItemStyle::NONE);
    itemModel.SetRowStart(NULL_VALUE);
    itemModel.SetRowEnd(NULL_VALUE);
    itemModel.SetColumnStart(NULL_VALUE);
    itemModel.SetColumnEnd(NULL_VALUE);
    ViewStackProcessor::GetInstance()->Pop();

    /**
     * @tc.steps: step2. Test Create function
     */
    std::function<void(int32_t)> deepRenderFunc = [](int32_t innerNodeId) {};
    bool isLazy = true;
    itemModel.Create(std::move(deepRenderFunc), isLazy, GridItemStyle::PLAIN);

    /**
     * @tc.steps: step3. invoke SetSelected function
     */
    itemModel.SetSelected(true);
    CreateDone(frameNode_);

    /**
     * @tc.expected: gridItemPattern->isSelected_ is true
     */
    auto gridItemPattern = GetChildPattern<GridItemPattern>(frameNode_, 1);
    EXPECT_TRUE(gridItemPattern->isSelected_);
}

/**
 * @tc.name: SupplyAllData2ZeroIndex001
 * @tc.desc: Test GridScrollLayoutAlgorithm::SupplyAllData2ZeroIndex
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SupplyAllData2ZeroIndex001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    CreateFixedItems(30);
    CreateDone(frameNode_);

    pattern_->ScrollToIndex(20, true, ScrollAlign::END);
    FlushLayoutTask(frameNode_);

    EXPECT_EQ(pattern_->GetGridLayoutInfo().gridMatrix_.size(), 4);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().lineHeightMap_.size(), 4);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().gridMatrix_.at(0).at(0), 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().gridMatrix_.at(3).at(2), 11);
}

/**
 * @tc.name: SupplyAllData2ZeroIndex002
 * @tc.desc: Test GridScrollLayoutAlgorithm::SupplyAllData2ZeroIndex
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SupplyAllData2ZeroIndex002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    CreateFixedItems(30);
    CreateDone(frameNode_);

    pattern_->ScrollToIndex(20, true, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    pattern_->ScrollToIndex(10, true, ScrollAlign::CENTER);
    FlushLayoutTask(frameNode_);

    EXPECT_EQ(pattern_->GetGridLayoutInfo().gridMatrix_.size(), 4);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().lineHeightMap_.size(), 4);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().gridMatrix_.at(0).at(0), 0);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().gridMatrix_.at(3).at(2), 11);
}


/**
 * @tc.name: OnModifyDone001
 * @tc.desc: Test OnModifyDone
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, OnModifyDone001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Test OnModifyDone
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetMultiSelectable(true);
    CreateFixedItems(10);
    CreateDone(frameNode_);
    auto paintProperty = pattern_->GetPaintProperty<ScrollablePaintProperty>();
    EXPECT_TRUE(pattern_->multiSelectable_);
    EXPECT_TRUE(pattern_->isMouseEventInit_);
    EXPECT_TRUE(pattern_->GetScrollableEvent());
    EXPECT_TRUE(paintProperty->GetScrollBarProperty());
    EXPECT_TRUE(frameNode_->GetFocusHub());
    EXPECT_TRUE(pattern_->GetScrollableEvent()->GetScrollable());
    EXPECT_TRUE(pattern_->IsNeedInitClickEventRecorder());

    /**
     * @tc.steps: step2. Call OnModifyDone
     */
    pattern_->OnModifyDone();
    EXPECT_TRUE(pattern_->multiSelectable_);
    EXPECT_TRUE(pattern_->isMouseEventInit_);
    EXPECT_TRUE(pattern_->GetScrollableEvent());
    EXPECT_TRUE(paintProperty->GetScrollBarProperty());
    EXPECT_TRUE(frameNode_->GetFocusHub());
    EXPECT_TRUE(pattern_->GetScrollableEvent()->GetScrollable());
    EXPECT_TRUE(pattern_->IsNeedInitClickEventRecorder());

    /**
     * @tc.steps: step3. Change MultiSelectable and Call OnModifyDone
     */
    pattern_->SetMultiSelectable(false);
    pattern_->OnModifyDone();
    EXPECT_FALSE(pattern_->multiSelectable_);
    EXPECT_FALSE(pattern_->isMouseEventInit_);
}

/**
 * @tc.name: GetEndOffset001
 * @tc.desc: Test GetEndOffset
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetEndOffset001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetEndOffset(), ITEM_HEIGHT);
}

/**
 * @tc.name: GetEndOffset002
 * @tc.desc: Test GetEndOffset
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetEndOffset002, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);
    pattern_->SetEdgeEffect(EdgeEffect::SPRING, true);
    EXPECT_EQ(pattern_->GetEndOffset(), 0.f);
}

/**
 * @tc.name: GetEndOffset003
 * @tc.desc: Test GetEndOffset
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetEndOffset003, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(20);
    CreateDone(frameNode_);
    pattern_->SetEdgeEffect(EdgeEffect::SPRING, true);
    EXPECT_EQ(pattern_->GetEndOffset(), 0.f);
}

/**
 * @tc.name: GetVisibleSelectedItems001
 * @tc.desc: Test GetVisibleSelectedItems
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetVisibleSelectedItems001, TestSize.Level1)
{
    /**
     * @tc.cases: Set item(index:1) isSelected and call GetVisibleSelectedItems
     * @tc.expected: Has 1 item selected
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(20);
    CreateDone(frameNode_);
    GetChildPattern<GridItemPattern>(frameNode_, 1)->SetSelected(true);
    EXPECT_EQ(pattern_->GetVisibleSelectedItems().size(), 1);
    EXPECT_FALSE(pattern_->irregular_);
}

/**
 * @tc.name: AdaptToChildMainSize001
 * @tc.desc: Test AdaptToChildMainSize
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, AdaptToChildMainSize001, TestSize.Level1)
{
    /**
     * @tc.cases: Set ColumnsTemplate, not set grid height
     */
    GridModelNG model;
    RefPtr<ScrollControllerBase> positionController = model.CreatePositionController();
    RefPtr<ScrollProxy> scrollBarProxy = model.CreateScrollBarProxy();
    model.Create(positionController, scrollBarProxy);
    ViewAbstract::SetWidth(CalcLength(GRID_WIDTH));
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetCellLength(ITEM_HEIGHT);
    model.SetMaxCount(4);
    CreateFixedItems(20);
    GetGrid();
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().lastMainSize_, 1000.f);
}

/**
 * @tc.name: AdaptToChildMainSize002
 * @tc.desc: Test AdaptToChildMainSize
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, AdaptToChildMainSize002, TestSize.Level1)
{
    /**
     * @tc.cases: Set RowsTemplate, not set grid width
     */
    GridModelNG model;
    RefPtr<ScrollControllerBase> positionController = model.CreatePositionController();
    RefPtr<ScrollProxy> scrollBarProxy = model.CreateScrollBarProxy();
    model.Create(positionController, scrollBarProxy);
    ViewAbstract::SetHeight(CalcLength(GRID_HEIGHT));
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    model.SetCellLength(ITEM_WIDTH);
    model.SetMaxCount(4);
    CreateFixedItems(20);
    GetGrid();
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().lastMainSize_, 600.f);
}

/**
 * @tc.name: LayoutCachedItem001
 * @tc.desc: Test LayoutCachedItem
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, LayoutCachedItem001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set CachedCount:1
     * @tc.expected: The item(index:16) below view is active, no item above view
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetCachedCount(1);
    CreateFixedItems(40);
    CreateDone(frameNode_);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 16)->IsActive()); // the fifth row
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 20)->IsActive()); // the sixth row

    /**
     * @tc.steps: step2. Scroll down
     * @tc.expected: The item(index:0) above view is active, the item(index:20) below view is active
     */
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_UPDATE);
    pattern_->ScrollToIndex(4, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 0)->IsActive());
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 20)->IsActive());

    /**
     * @tc.steps: step3. Scroll down
     * @tc.expected: The item(index:4) above view is active, the item(index:24) below view is active
     */
    pattern_->ScrollToIndex(8, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 0)->IsActive());
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 4)->IsActive());
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 24)->IsActive()); // th seventh row

    /**
     * @tc.steps: step4. Scroll up
     * @tc.expected: The item(index:0) above view is active, the item(index:20) below view is active
     */
    pattern_->ScrollToIndex(4, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 0)->IsActive());
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 20)->IsActive());
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 24)->IsActive());

    /**
     * @tc.steps: step5. Scroll up
     * @tc.expected: The item(index:16) below view is active, no item above view
     */
    pattern_->ScrollToIndex(0, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 16)->IsActive());
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 20)->IsActive());
}


/**
 * @tc.name: ScrollLayoutRTL001
 * @tc.desc: Test Vertical Grid with Direction RTL
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, ScrollLayoutRTL001, TestSize.Level1)
{
    float itemWidth = 120.0f;
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    model.SetIsRTL(TextDirection::RTL);
    CreateFixedItems(18);
    CreateDone(frameNode_);

    int32_t colsNumber = 4;
    for (int32_t index = 0; index < 8; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = GRID_WIDTH - index % colsNumber * itemWidth - itemWidth;
        float offsetY = floor(index / colsNumber) * ITEM_HEIGHT;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_HEIGHT);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: ScrollLayoutRTL002
 * @tc.desc: Test Horizontal Grid with Direction RTL
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, ScrollLayoutRTL002, TestSize.Level1)
{
    float itemWidth = 120.0f;
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    model.SetIsRTL(TextDirection::RTL);
    CreateFixedItems(18);
    CreateDone(frameNode_);

    int32_t rowsNumber = 4;
    for (int32_t index = 0; index < 8; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = GRID_WIDTH - floor(index / rowsNumber) * itemWidth - itemWidth;
        float offsetY = index % rowsNumber * ITEM_HEIGHT;
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, ITEM_HEIGHT);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: AdaptToChildMainSize003
 * @tc.desc: Test Horizontal Grid with Infinity mainSize
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, AdaptToChildMainSize003, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr 1fr 1fr");
    ViewAbstract::SetWidth(CalcLength(Infinity<int32_t>()));
    CreateFixedItems(8);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().lastMainSize_, ITEM_WIDTH * 2);
}

/**
 * @tc.name: AdaptToChildMainSize004
 * @tc.desc: Test Vertical Grid with Infinity mainSize
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, AdaptToChildMainSize004, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    ViewAbstract::SetHeight(CalcLength(Infinity<int32_t>()));
    CreateFixedItems(8);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().lastMainSize_, ITEM_HEIGHT * 2);
}

/**
 * @tc.name: AdaptToChildMainSize005
 * @tc.desc: Test Vertical Grid with maxcount and 0 itemHeight
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, AdaptToChildMainSize005, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    ViewAbstract::SetHeight(CalcLength(100));
    model.SetMaxCount(1);
    CreateGridItems(1, 0, 0);
    CreateDone(frameNode_);
    EXPECT_EQ(pattern_->GetGridLayoutInfo().lastMainSize_, 100);
}

/*
 * @tc.name: GetResetMode001
 * @tc.desc: Test Reset Function when have bigItem
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetResetMode001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr 1fr");
    CreateFixedItems(40);
    CreateDone(frameNode_);

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    auto* wrapper = AceType::RawPtr(frameNode_);
    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, -1), std::make_pair(false, false));
    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, 1), std::make_pair(true, false));

    pattern_->ScrollToIndex(30, false, ScrollAlign::START);
    layoutAlgorithm->gridLayoutInfo_.startIndex_ = 30;
    frameNode_->childrenUpdatedFrom_ = 20;
    FlushLayoutTask(frameNode_);

    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, 0), std::make_pair(true, false));
    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, 10), std::make_pair(true, false));
    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, 25), std::make_pair(true, false));

    layoutAlgorithm->gridLayoutInfo_.hasBigItem_ = true;

    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, 0), std::make_pair(true, false));
    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, 10), std::make_pair(true, false));
    EXPECT_EQ(layoutAlgorithm->GetResetMode(wrapper, 25), std::make_pair(false, true));
}


/**
 * @tc.name: LayoutWithAutoStretch002
 * @tc.desc: Test Vertical Grid Layout with auto-stretch
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, LayoutWithAutoStretch002, TestSize.Level1)
{
    float itemWidth = 80.0f;
    float itemHeight = 150.0f;
    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("repeat(auto-stretch, 150)");
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    CreateGridItems(25, itemWidth, itemHeight);
    CreateDone(frameNode_);

    int32_t rowsNumber = 5;
    int32_t columnsNumber = 5;
    float realColumnsGap = 10.f;
    float realRowsGap = 12.5f;
    for (int32_t index = 0; index < 25; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = index / columnsNumber * (itemWidth + realColumnsGap);
        float offsetY = index % rowsNumber * (itemHeight + realRowsGap);
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, itemHeight);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: LayoutWithAutoStretch003
 * @tc.desc: Test Horizental Grid Layout with auto-stretch
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, LayoutWithAutoStretch003, TestSize.Level1)
{
    float itemWidth = 80.0f;
    float itemHeight = 150.0f;
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("repeat(auto-stretch, 80)");
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    CreateGridItems(25, itemWidth, itemHeight);
    CreateDone(frameNode_);

    int32_t rowsNumber = 5;
    int32_t columnsNumber = 5;
    float realColumnsGap = 20.f;
    float realRowsGap = 10.f;
    for (int32_t index = 0; index < 25; index++) {
        RectF childRect = GetChildRect(frameNode_, index);
        float offsetX = index % columnsNumber * (itemWidth + realColumnsGap);
        float offsetY = index / rowsNumber * (itemHeight + realRowsGap);
        RectF expectRect = RectF(offsetX, offsetY, itemWidth, itemHeight);
        EXPECT_TRUE(IsEqual(childRect, expectRect)) << "index: " << index;
    }
}

/**
 * @tc.name: Cache001
 * @tc.desc: Test Grid preload items
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Cache001, TestSize.Level1)
{
    GridModelNG model = CreateRepeatGrid(50, [](uint32_t idx) { return 200.0f; });
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    model.SetCachedCount(2); // 2 lines
    CreateDone(frameNode_);
    EXPECT_EQ(frameNode_->GetTotalChildCount(), 50);
    const auto& info = pattern_->gridLayoutInfo_;
    EXPECT_EQ(info.startIndex_, 0);
    EXPECT_EQ(info.endIndex_, 11);
    const std::list<int32_t> preloadList = { 12, 13, 14 };
    for (const int32_t i : preloadList) {
        EXPECT_FALSE(frameNode_->GetChildByIndex(i));
    }
    CheckPreloadListEqual(preloadList);
    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    EXPECT_TRUE(pattern_->preloadItemList_.empty());
    for (const int32_t i : preloadList) {
        EXPECT_TRUE(frameNode_->GetChildByIndex(i));
        EXPECT_EQ(GetChildRect(frameNode_, i).Height(), 200.0f);
    }
    FlushLayoutTask(frameNode_);
    // preload next line
    const std::list<int32_t> preloadList2 = { 15, 16, 17 };
    CheckPreloadListEqual(preloadList2);
    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    EXPECT_TRUE(pattern_->preloadItemList_.empty());
    for (const int32_t i : preloadList2) {
        EXPECT_TRUE(frameNode_->GetChildByIndex(i));
        EXPECT_EQ(GetChildRect(frameNode_, i).Height(), 200.0f);
    }
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(pattern_->preloadItemList_.empty());

    pattern_->ScrollToIndex(49);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(info.startIndex_, 39);
    // GridScroll algo currently not capable of preloading backward
    EXPECT_TRUE(pattern_->preloadItemList_.empty());
}

/**
 * @tc.name: Stretch001
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch001, TestSize.Level1)
{
    /**
     * 0: [0], [1]
     *
     * 1 will stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetColumnsTemplate("1fr 1fr");

    CreateFixedHeightItems(1, 150);
    CreateAdaptChildSizeGridItems(1);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect0 = pattern_->GetItemRect(0);
    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect0.Height(), childRect1.Height());
}

/**
 * @tc.name: Stretch002
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch002, TestSize.Level1)
{
    /**
     * 0: [0], [1]
     * 1: [0]
     *
     * 1 will not stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetColumnsTemplate("1fr 1fr");

    CreateBigItem(0, 1, 0, 0, ITEM_WIDTH, 200);
    CreateAdaptChildSizeGridItems(1);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect1.Height(), 0);
}

/**
 * @tc.name: Stretch003
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch003, TestSize.Level1)
{
    /**
     * 0: [0], [1]
     * 1: [0], [2]
     * 2: [3], [4]
     *
     * 1 and 2 will not stretch
     * 3 will stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetColumnsTemplate("1fr 1fr");

    CreateBigItem(0, 1, 0, 0, ITEM_WIDTH, 200);
    CreateAdaptChildSizeGridItems(3);
    CreateFixedHeightItems(1, 150);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect1.Height(), 0);

    auto childRect3 = pattern_->GetItemRect(3);
    auto childRect4 = pattern_->GetItemRect(4);
    EXPECT_EQ(childRect4.Height(), childRect3.Height());
}

/**
 * @tc.name: Stretch004
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch004, TestSize.Level1)
{
    /**
     * 0: [0], [0], [1]
     *
     * 1 will not stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetColumnsTemplate("1fr 1fr 1fr");

    CreateBigItem(0, 1, 0, 1, ITEM_WIDTH, 200);
    CreateAdaptChildSizeGridItems(1);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect1.Height(), 0);
    auto childRect2 = pattern_->GetItemRect(2);
    EXPECT_EQ(childRect2.Height(), 0);
}

/**
 * @tc.name: Stretch005
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch005, TestSize.Level1)
{
    /**
     *  0
     * [0]
     * [1]
     *
     * 1 will stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetRowsTemplate("1fr 1fr");

    CreateFixedHeightItems(1, 150);
    CreateAdaptChildSizeGridItems(1);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect0 = pattern_->GetItemRect(0);
    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect0.Width(), childRect1.Width());
}

/**
 * @tc.name: Stretch006
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch006, TestSize.Level1)
{
    /**
     *  0
     * [0]
     * [0]
     * [1]
     *
     * 1 will not stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetRowsTemplate("1fr 1fr 1fr");

    CreateBigItem(0, 1, 0, 0, ITEM_WIDTH, ITEM_HEIGHT);
    CreateAdaptChildSizeGridItems(1);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect1.Width(), 0);
}

/**
 * @tc.name: Stretch007
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch007, TestSize.Level1)
{
    /**
     *  0    1    2
     * [0], [0], [3]
     * [1], [2], [4]
     *
     * 1 and 2 will not stretch
     * 3 will stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetRowsTemplate("1fr 1fr");

    CreateBigItem(0, 0, 0, 1, ITEM_WIDTH, ITEM_HEIGHT);
    CreateAdaptChildSizeGridItems(3);
    CreateFixedHeightItems(1, 150);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect1.Width(), 0);

    auto childRect3 = pattern_->GetItemRect(3);
    auto childRect4 = pattern_->GetItemRect(4);
    EXPECT_EQ(childRect4.Width(), childRect3.Width());
}

/**
 * @tc.name: Stretch008
 * @tc.desc: Test Grid AlignItems STRETCH
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, Stretch008, TestSize.Level1)
{
    /**
     *  0    1
     * [0], [0]
     * [0], [0]
     * [1], [2]
     *
     * 1 and 2 will not stretch
     */
    GridModelNG model = CreateGrid();
    model.SetAlignItems(GridItemAlignment::STRETCH);
    model.SetRowsTemplate("1fr 1fr 1fr");

    CreateBigItem(0, 1, 0, 1, ITEM_WIDTH, ITEM_HEIGHT);
    CreateAdaptChildSizeGridItems(2);

    CreateDone(frameNode_);
    FlushLayoutTask(frameNode_);

    auto childRect1 = pattern_->GetItemRect(1);
    EXPECT_EQ(childRect1.Width(), 0);
    auto childRect2 = pattern_->GetItemRect(2);
    EXPECT_EQ(childRect2.Width(), 0);
}

/**
 * @tc.name: MarginPadding001
 * @tc.desc: Test margin/padding
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, MarginPadding001, TestSize.Level1)
{
    ColumnModelNG colModel;
    colModel.Create(Dimension(0), nullptr, "");
    auto colNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    CreateFixedItems(4);
    CreateDone(colNode);

    MarginProperty margin = { CalcLength(1), CalcLength(3), CalcLength(5), CalcLength(7) };
    PaddingProperty padding = { CalcLength(2), CalcLength(4), CalcLength(6), CalcLength(8) };
    layoutProperty_->UpdateMargin(margin);
    layoutProperty_->UpdatePadding(padding);
    auto itemLayoutProperty = GetChildLayoutProperty<GridItemLayoutProperty>(frameNode_, 2);
    itemLayoutProperty->UpdateMargin(margin);
    itemLayoutProperty->UpdatePadding(padding);
    FlushLayoutTask(colNode, true);
    EXPECT_TRUE(IsEqual(frameNode_->GetGeometryNode()->GetFrameRect(), RectF(1, 5, 480, 800)));
    EXPECT_TRUE(IsEqual(GetChildRect(frameNode_, 2), RectF(3, 211, 233, 200)));
}

/**
 * @tc.name: SpringAnimationTest001
 * @tc.desc: Test Grid change height during spring animation. GridItem is not fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest001, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(3);
    CreateDone();

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(-1200.f);
    info.SetMainDelta(-200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -145.47027);

    /**
     * @tc.steps: step2. play spring animation frame by frame, and increase grid height during animation
     * @tc.expected: currentOffset will not change with the grid height
     */

    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT + 50))));
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -72.73513);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
}

/**
 * @tc.name: SpringAnimationTest002
 * @tc.desc: Test Grid change height during spring animation. GridItem is not fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest002, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(3);
    CreateDone();

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(-1200.f);
    info.SetMainDelta(-200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -145.47027);

    /**
     * @tc.steps: step2. play spring animation frame by frame, and decrease grid height during animation
     * @tc.expected: currentOffset will not change with the grid height
     */
    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT - 50))));
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -72.73513);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
}

/**
 * @tc.name: SpringAnimationTest003
 * @tc.desc: Test Grid change height during spring animation. GridItem is not fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest003, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(3);
    CreateDone();

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(1200.f);
    info.SetMainDelta(200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 174.74933);

    /**
     * @tc.steps: step2. play spring animation frame by frame, and increase grid height during animation
     * @tc.expected: currentOffset will not change with the grid height
     */
    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT + 50))));
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 87.374664);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
}

/**
 * @tc.name: SpringAnimationTest004
 * @tc.desc: Test Grid change height during spring animation. GridItem is not fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest004, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(3);
    CreateDone();

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(1200.f);
    info.SetMainDelta(200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 174.74933);

    /**
     * @tc.steps: step2. play spring animation frame by frame, and decrease grid height during animation
     * @tc.expected: currentOffset will not change with the grid height
     */
    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT - 50))));
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 87.37466);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
}

/**
 * @tc.name: SpringAnimationTest005
 * @tc.desc: Test Grid change height during spring animation. GridItem is fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest005, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, false);
    CreateFixedItems(21);
    CreateDone();

    pattern_->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    FlushLayoutTask(frameNode_);

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(-1200.f);
    info.SetMainDelta(-200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -126.00447);

    /**
     * @tc.steps: step2. play spring animation frame by frame, and decrease grid height during animation
     * @tc.expected: currentOffset will change with the grid height
     */
    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT + 50))));
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -163.00224 + 50);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -150);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -150);
}

/**
 * @tc.name: SpringAnimationTest006
 * @tc.desc: Test Grid change height during spring animation. GridItem is fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest006, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(20);
    CreateDone();
    pattern_->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(pattern_->IsAtBottom());

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(-200.f);
    info.SetMainDelta(-200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -126.00447);
    EXPECT_TRUE(pattern_->IsAtBottom());

    /**
     * @tc.steps: step2. play spring animation frame by frame, and decrease grid height during animation
     * @tc.expected: currentOffset will change with the grid height
     */
    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT - 50))));
    FlushLayoutTask(frameNode_);
    // the value of (currentOffset + gridMainSizeDelta) is greater than lineHeight(200), so move to next line
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -163.00224 - 50 + 200);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -50);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, -50);
}

/**
 * @tc.name: SpringAnimationTest007
 * @tc.desc: Test Grid change height during spring animation. GridItem is fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest007, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(40);
    CreateDone();

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(1400.f);
    info.SetMainDelta(200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 326.00445);

    /**
     * @tc.steps: step2. play spring animation frame by frame, and decrease grid height during animation
     * @tc.expected: currentOffset will not change with the grid height
     */
    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT + 50))));
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 163.00222);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
}

/**
 * @tc.name: SpringAnimationTest008
 * @tc.desc: Test Grid change height during spring animation. GridItem is not fully filled Grid.
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, SpringAnimationTest008, TestSize.Level1)
{
    MockAnimationManager::GetInstance().Reset();
    MockAnimationManager::GetInstance().SetTicks(2);
    auto model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(40);
    CreateDone();

    /**
     * @tc.steps: step1. Simulate a scrolling gesture.
     * @tc.expected: Grid trigger spring animation.
     */
    GestureEvent info;
    info.SetMainVelocity(1400.f);
    info.SetMainDelta(200.f);
    auto scrollable = pattern_->GetScrollableEvent()->GetScrollable();
    scrollable->HandleTouchDown();
    scrollable->HandleDragStart(info);
    scrollable->HandleDragUpdate(info);
    FlushLayoutTask(frameNode_);

    EXPECT_TRUE(pattern_->OutBoundaryCallback());
    scrollable->HandleTouchUp();
    scrollable->HandleDragEnd(info);
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 326.00445);

    /**
     * @tc.steps: step2. play spring animation frame by frame, and decrease grid height during animation
     * @tc.expected: currentOffset will not change with the grid height
     */
    MockAnimationManager::GetInstance().Tick();
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(Dimension(GRID_HEIGHT - 50))));
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 163.00222);

    MockAnimationManager::GetInstance().Tick();
    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
    EXPECT_TRUE(MockAnimationManager::GetInstance().AllFinished());

    FlushLayoutTask(frameNode_);
    EXPECT_FLOAT_EQ(pattern_->gridLayoutInfo_.currentOffset_, 0);
}
} // namespace OHOS::Ace::NG
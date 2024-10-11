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

namespace OHOS::Ace::NG {

namespace {} // namespace

class GridScrollerEventTestNg : public GridTestNg {
public:
    GridModelNG CreateWithNoProxy();
};

GridModelNG GridScrollerEventTestNg::CreateWithNoProxy()
{
    GridModelNG model;
    RefPtr<ScrollControllerBase> positionController = model.CreatePositionController();
    model.Create(positionController, nullptr);
    ViewAbstract::SetWidth(CalcLength(GRID_WIDTH));
    ViewAbstract::SetHeight(CalcLength(GRID_HEIGHT));
    GetGrid();
    return model;
}

/**
 * @tc.name: ScrollablePattern001
 * @tc.desc: Test scrollable_pattern
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, ScrollablePattern001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Test OnScrollPosition/OnScrollEnd.
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);
    double itemHeight = ITEM_HEIGHT;
    EXPECT_TRUE(pattern_->OnScrollPosition(itemHeight, SCROLL_FROM_UPDATE));
    EXPECT_TRUE(pattern_->OnScrollPosition(itemHeight, SCROLL_FROM_UPDATE));
    EXPECT_TRUE(pattern_->OnScrollPosition(itemHeight, SCROLL_FROM_START));
    pattern_->OnScrollEnd();
    auto coordinationEvent = AceType::MakeRefPtr<ScrollableCoordinationEvent>();
    auto event1 = [](float, float) { return true; };
    auto event2 = [](bool, float) {};
    auto event3 = [](float) {};
    coordinationEvent->SetOnScrollEvent(event1);
    coordinationEvent->SetOnScrollStartEvent(event2);
    coordinationEvent->SetOnScrollEndEvent(event3);
    EXPECT_TRUE(pattern_->OnScrollPosition(itemHeight, SCROLL_FROM_UPDATE));
    EXPECT_TRUE(pattern_->OnScrollPosition(itemHeight, SCROLL_FROM_UPDATE));
    EXPECT_TRUE(pattern_->OnScrollPosition(itemHeight, SCROLL_FROM_START));
    pattern_->OnScrollEnd();
    EXPECT_TRUE(pattern_->OnScrollPosition(itemHeight, SCROLL_FROM_UPDATE));
    pattern_->OnScrollEnd();

    /**
     * @tc.steps: step2. Test OnScrollCallback.
     */
    model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    CreateFixedItems(10);
    CreateDone(frameNode_);
    EXPECT_TRUE(pattern_->OnScrollCallback(-ITEM_HEIGHT, SCROLL_FROM_START));
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, 0);
    EXPECT_TRUE(pattern_->OnScrollCallback(-ITEM_HEIGHT, SCROLL_FROM_UPDATE));
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, -ITEM_HEIGHT);
    EXPECT_TRUE(pattern_->OnScrollCallback(-ITEM_HEIGHT, SCROLL_FROM_UPDATE));
    EXPECT_EQ(pattern_->GetGridLayoutInfo().currentOffset_, -ITEM_HEIGHT * 2);
}

/**
 * @tc.name: GridEventTestNg001
 * @tc.desc: Test scroll callback
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, GridEventTestNg001, TestSize.Level1)
{
    Dimension offsetY;
    ScrollState scrollState;
    auto event = [&offsetY, &scrollState](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
    };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(event);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_UPDATE);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), ITEM_HEIGHT);
    EXPECT_EQ(scrollState, ScrollState::SCROLL);

    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), -ITEM_HEIGHT);
    EXPECT_EQ(scrollState, ScrollState::FLING);

    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION_SPRING);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), ITEM_HEIGHT);
    EXPECT_EQ(scrollState, ScrollState::FLING);

    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_NONE);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), -ITEM_HEIGHT);
    EXPECT_EQ(scrollState, ScrollState::IDLE);

    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_AXIS);
    FlushLayoutTask(frameNode_);
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_BAR);
    FlushLayoutTask(frameNode_);
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION_CONTROLLER);
    FlushLayoutTask(frameNode_);
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_BAR_FLING);
    FlushLayoutTask(frameNode_);

    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_UPDATE);
    pattern_->scrollStop_ = true;
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), 0);
    EXPECT_EQ(scrollState, ScrollState::IDLE);

    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION);
    pattern_->scrollStop_ = true;
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), 0);
    EXPECT_EQ(scrollState, ScrollState::IDLE);

    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION_SPRING);
    pattern_->scrollStop_ = true;
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), 0);
    EXPECT_EQ(scrollState, ScrollState::IDLE);

    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_NONE);
    pattern_->scrollStop_ = true;
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(offsetY.ConvertToPx(), -ITEM_HEIGHT);
    EXPECT_EQ(scrollState, ScrollState::IDLE);

    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_AXIS);
    pattern_->scrollStop_ = true;
    FlushLayoutTask(frameNode_);
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_BAR);
    pattern_->scrollStop_ = true;
    FlushLayoutTask(frameNode_);
    pattern_->SetScrollAbort(true);
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_NONE);
    FlushLayoutTask(frameNode_);
}

/**
 * @tc.name: GridEventTestNg002
 * @tc.desc: Test scroll callback
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, GridEventTestNg002, TestSize.Level1)
{
    int32_t startIndex;
    int32_t endIndex;
    auto event = [&startIndex, &endIndex](int32_t start, int32_t end) {
        startIndex = start;
        endIndex = end;
    };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScrollIndex(event);
    CreateFixedItems(12);
    CreateDone(frameNode_);

    ScrollTo(ITEM_HEIGHT);
    EXPECT_EQ(startIndex, 2);
    EXPECT_EQ(endIndex, 9);

    ScrollTo(ITEM_HEIGHT * 2);
    EXPECT_EQ(startIndex, 4);
    EXPECT_EQ(endIndex, 11);
}

/**
 * @tc.name: GridEventTestNg003
 * @tc.desc: Test scroll callback
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, GridEventTestNg003, TestSize.Level1)
{
    bool isTrigger = false;
    auto event = [&isTrigger]() { isTrigger = true; };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnReachStart(event);
    CreateFixedItems(12);
    CreateDone(frameNode_);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    ScrollTo(ITEM_HEIGHT);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    ScrollTo(0.f);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION_SPRING);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION_SPRING);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_AXIS);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_AXIS);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_JUMP);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_JUMP);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION_CONTROLLER);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION_CONTROLLER);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_BAR);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_BAR);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_BAR_FLING);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_BAR_FLING);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
}

/**
 * @tc.name: GridEventTestNg004
 * @tc.desc: Test scroll callback
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, GridEventTestNg004, TestSize.Level1)
{
    bool isTrigger = false;
    auto event = [&isTrigger]() { isTrigger = true; };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnReachEnd(event);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    isTrigger = false;
    UpdateCurrentOffset(-ITEM_HEIGHT * 3);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    UpdateCurrentOffset(ITEM_HEIGHT);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_AXIS);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_AXIS);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION_SPRING);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION_SPRING);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_JUMP);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_JUMP);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_ANIMATION_CONTROLLER);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_ANIMATION_CONTROLLER);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_BAR);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_BAR);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_BAR_FLING);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_BAR_FLING);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);

    isTrigger = false;
    pattern_->UpdateCurrentOffset(-ITEM_HEIGHT, SCROLL_FROM_NONE);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isTrigger);
    isTrigger = false;
    pattern_->UpdateCurrentOffset(ITEM_HEIGHT, SCROLL_FROM_NONE);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isTrigger);
}

/**
 * @tc.name: GridEventTestNg005
 * @tc.desc: Verify onScrollStart, onScrollStop callback
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, GridEventTestNg005, TestSize.Level1)
{
    bool isScrollStartCalled = false;
    bool isScrollStopCalled = false;
    auto scrollStart = [&isScrollStartCalled]() { isScrollStartCalled = true; };
    auto scrollStop = [&isScrollStopCalled]() { isScrollStopCalled = true; };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScrollStart(scrollStart);
    model.SetOnScrollStop(scrollStop);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    pattern_->OnScrollCallback(100.f, SCROLL_FROM_START);
    EXPECT_TRUE(isScrollStartCalled);

    pattern_->SetScrollAbort(true);
    pattern_->OnScrollEndCallback();
    EXPECT_FALSE(isScrollStopCalled);
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isScrollStopCalled);
}

/**
 * @tc.name: GridEventTestNg006
 * @tc.desc: Test other condition
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, GridEventTestNg006, TestSize.Level1)
{
    auto event = [](Dimension, ScrollState) {
        ScrollFrameResult result;
        return result;
    };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScrollFrameBegin(event);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    auto scrollableEvent = pattern_->GetScrollableEvent();
    ASSERT_NE(scrollableEvent, nullptr);
    EXPECT_NE(scrollableEvent->GetScrollable()->callback_, nullptr);
    FlushLayoutTask(frameNode_);
}

/**
 * @tc.name: GridEventTestNg007
 * @tc.desc: Emulate consecutive swipes
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, GridEventTestNg007, TestSize.Level1)
{
    bool isScrollStartCalled = false;
    bool isScrollStopCalled = false;
    auto scrollStart = [&isScrollStartCalled]() { isScrollStartCalled = true; };
    auto scrollStop = [&isScrollStopCalled]() { isScrollStopCalled = true; };
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScrollStart(scrollStart);
    model.SetOnScrollStop(scrollStop);
    CreateFixedItems(10);
    CreateDone(frameNode_);

    pattern_->OnScrollCallback(100.f, SCROLL_FROM_START);
    EXPECT_TRUE(isScrollStartCalled);

    // reset
    isScrollStartCalled = false;

    pattern_->OnScrollEndCallback();
    EXPECT_TRUE(pattern_->scrollStop_);
    EXPECT_FALSE(isScrollStopCalled);

    // scrollStart again, before layout
    pattern_->OnScrollCallback(100.f, SCROLL_FROM_START);
    EXPECT_FALSE(isScrollStartCalled);
    EXPECT_FALSE(pattern_->scrollStop_);
    EXPECT_FALSE(isScrollStopCalled);

    pattern_->OnScrollEndCallback();
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isScrollStopCalled);
}

/**
 * @tc.name: VerticalGridWithoutScrollBar001
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll Without Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithoutScrollBarWithoutAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set seven kinds of scroll events: onScroll, onWillScroll, onDidScroll,
     *                   onScrollStart, onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnWillScrollCallBack = false;
    bool isOnDidScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    Dimension willScrollOffset;
    ScrollState willScrollState;
    auto onWillScroll = [&willScrollOffset, &willScrollState, &isOnWillScrollCallBack](
                            Dimension offset, ScrollState state, ScrollSource source) {
        willScrollOffset = offset;
        willScrollState = state;
        isOnWillScrollCallBack = true;
        ScrollFrameResult result;
        result.offset = offset;
        return result;
    };
    Dimension didScrollOffset;
    ScrollState didScrollState;
    auto onDidScroll = [&didScrollOffset, &didScrollState, &isOnDidScrollCallBack](
                           Dimension offset, ScrollState state) {
        didScrollOffset = offset;
        didScrollState = state;
        isOnDidScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    eventHub_->SetOnWillScroll(onWillScroll);
    eventHub_->SetOnDidScroll(onDidScroll);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's center position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll call back function should be triggered
     */
    pattern_->ScrollTo(ITEM_HEIGHT * 1);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnWillScrollCallBack);
    EXPECT_TRUE(isOnDidScrollCallBack);
    EXPECT_EQ(offsetY, willScrollOffset);
    EXPECT_EQ(offsetY, didScrollOffset);
    EXPECT_EQ(scrollState, willScrollState);
    EXPECT_EQ(scrollState, didScrollState);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: VerticalGridWithoutScrollBar002
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll Without Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithoutScrollBarWithoutAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's end position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll and onReachEnd should be triggered
     */
    pattern_->ScrollTo(ITEM_HEIGHT * 9);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: VerticalGridWithoutScrollBarWithAnimation001
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll Without Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithoutScrollBarWithAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;
    /**
     * @tc.steps: step2. Scroll to grid's center with animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: OnScroll OnScrollStart and onScrollStop call back functions should be triggered
     */
    pattern_->AnimateTo(-5 * ITEM_HEIGHT, 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-5 * ITEM_HEIGHT, SCROLL_FROM_ANIMATION);

    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: VerticalGridWithoutScrollBarWithAnimation002
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll Without Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithoutScrollBarWithAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's end without animation, and check the
     *                   five kinds of call back functions is triggered or not.
     * @tc.expected:All call back functions except onReachStart should be triggered
     */
    pattern_->AnimateTo(-20 * ITEM_HEIGHT, 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-20 * ITEM_HEIGHT, SCROLL_FROM_ANIMATION);

    EXPECT_EQ(pattern_->GetTotalOffset(), ITEM_HEIGHT * 20);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithoutScrollBarWithoutAnimation001
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll Without Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithoutScrollBarWithoutAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's center position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll call back function should be triggered
     */
    pattern_->ScrollTo(ITEM_WIDTH * 1);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithoutScrollBarWithoutAnimation002
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll Without Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithoutScrollBarWithoutAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's end position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll and onReachEnd should be triggered
     */
    pattern_->ScrollTo(ITEM_WIDTH * 10);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithoutScrollBarWithAnimation001
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll Without Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithoutScrollBarWithAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;
    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's center with animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: OnScroll OnScrollStart and onScrollStop call back functions should be triggered
     */
    pattern_->AnimateTo(-5 * ITEM_WIDTH, 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-5 * ITEM_WIDTH, SCROLL_FROM_ANIMATION);

    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithoutScrollBarWithAnimation002
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll Without Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithoutScrollBarWithAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;
    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateWithNoProxy();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's end without animation, and check the
     *                   five kinds of call back functions is triggered or not.
     * @tc.expected:All call back functions except onReachStart should be triggered
     */
    pattern_->AnimateTo(-10 * ITEM_WIDTH, 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-10 * ITEM_WIDTH, SCROLL_FROM_ANIMATION);

    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: VerticalGridWithScrollBarWithoutAnimation001
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll With Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithScrollBarWithoutAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's center position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll call back function should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->ScrollToIndex(5, false, ScrollAlign::CENTER, std::nullopt);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: VerticalGridWithScrollBarWithoutAnimation002
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll With Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithScrollBarWithoutAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's end position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll and onReachEnd should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->ScrollToIndex(19, false, ScrollAlign::END, std::nullopt);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: VerticalGridWithScrollBarWithAnimation001
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll With Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithScrollBarWithAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's center with animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: OnScroll OnScrollStart and onScrollStop call back functions should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->AnimateTo(Dimension(-5 * ITEM_HEIGHT, DimensionUnit::PX), 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-5 * ITEM_HEIGHT, SCROLL_FROM_ANIMATION_CONTROLLER);

    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: VerticalGridWithScrollBarWithAnimation002
 * @tc.desc: Test Grid(Axis::VERTICAL) Scroll With Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, VerticalGridWithScrollBarWithAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's end without animation, and check the
     *                   five kinds of call back functions is triggered or not.
     * @tc.expected:All call back functions except onReachStart should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->AnimateTo(Dimension(-10 * ITEM_HEIGHT, DimensionUnit::PX), 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-10 * ITEM_HEIGHT, SCROLL_FROM_ANIMATION_CONTROLLER);

    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithScrollBarWithoutAnimation001
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll With Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithScrollBarWithoutAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's center position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll call back function should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->ScrollToIndex(5, false, ScrollAlign::CENTER, std::nullopt);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithScrollBarWithoutAnimation002
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll With Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithScrollBarWithoutAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step3. Scroll to grid's end position without animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: Only onScroll and onReachEnd should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->ScrollToIndex(19, false, ScrollAlign::END, std::nullopt);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_FALSE(isOnScrollStartCallBack);
    EXPECT_FALSE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithScrollBarWithAnimation001
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll With Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithScrollBarWithAnimation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's center with animation, and check the
     *                   five kinds of call back functions are triggered or not.
     * @tc.expected: OnScroll OnScrollStart and onScrollStop call back functions should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->AnimateTo(Dimension(-5 * ITEM_WIDTH, DimensionUnit::PX), 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-5 * ITEM_WIDTH, SCROLL_FROM_ANIMATION_CONTROLLER);

    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_FALSE(isOnReachEndCallBack);
}

/**
 * @tc.name: HorizontalGridWithScrollBarWithAnimation002
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll With Scroll Bar With Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, HorizontalGridWithScrollBarWithAnimation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set five kinds of scroll events: onScroll, onScrollStart,
     *                   onScrollStop, onReachStart, onReachEnd
     */
    bool isOnScrollCallBack = false;
    bool isOnScrollStartCallBack = false;
    bool isOnScrollStopCallBack = false;
    bool isOnReachStartCallBack = false;
    bool isOnReachEndCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    auto onScrollStart = [&isOnScrollStartCallBack]() { isOnScrollStartCallBack = true; };
    auto onScrollStop = [&isOnScrollStopCallBack]() { isOnScrollStopCallBack = true; };
    auto onReachStart = [&isOnReachStartCallBack]() { isOnReachStartCallBack = true; };
    auto onReachEnd = [&isOnReachEndCallBack]() { isOnReachEndCallBack = true; };

    GridModelNG model = CreateGrid();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetOnScrollStart(onScrollStart);
    model.SetOnScrollStop(onScrollStop);
    model.SetOnReachStart(onReachStart);
    model.SetOnReachEnd(onReachEnd);
    model.SetSupportAnimation(true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    EXPECT_TRUE(isOnReachStartCallBack);
    isOnReachStartCallBack = false;

    /**
     * @tc.steps: step2. Scroll to grid's end without animation, and check the
     *                   five kinds of call back functions is triggered or not.
     * @tc.expected:All call back functions except onReachStart should be triggered
     */
    auto controller = pattern_->positionController_;
    controller->AnimateTo(Dimension(-10 * ITEM_WIDTH, DimensionUnit::PX), 1.f, Curves::LINEAR, false);
    pattern_->StopAnimate();
    pattern_->SetScrollAbort(false);
    pattern_->OnScrollEndCallback();
    pattern_->UpdateCurrentOffset(-10 * ITEM_WIDTH, SCROLL_FROM_ANIMATION_CONTROLLER);

    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnScrollStartCallBack);
    EXPECT_TRUE(isOnScrollStopCallBack);
    EXPECT_FALSE(isOnReachStartCallBack);
    EXPECT_TRUE(isOnReachEndCallBack);
}

/**
 * @tc.name: onWillScrollAndOnDidScroll001
 * @tc.desc: Test Grid(Axis::Vertical) Scroll, onWillScroll and onDidScroll
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, onWillScrollAndOnDidScroll001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set three kinds of scroll events: onScroll, onWillScroll, onDidScroll
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnWillScrollCallBack = false;
    bool isOnDidScrollCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    Dimension willScrollOffset;
    ScrollState willScrollState;
    auto onWillScroll = [&willScrollOffset, &willScrollState, &isOnWillScrollCallBack](
                            Dimension offset, ScrollState state, ScrollSource source) {
        willScrollOffset = offset * 2;
        willScrollState = state;
        isOnWillScrollCallBack = true;
        ScrollFrameResult result;
        result.offset = offset;
        return result;
    };
    Dimension didScrollOffset;
    ScrollState didScrollState;
    auto onDidScroll = [&didScrollOffset, &didScrollState, &isOnDidScrollCallBack](
                           Dimension offset, ScrollState state) {
        didScrollOffset = offset * 3;
        didScrollState = state;
        isOnDidScrollCallBack = true;
    };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    eventHub_->SetOnWillScroll(onWillScroll);
    eventHub_->SetOnDidScroll(onDidScroll);

    /**
     * @tc.steps: step2. Scroll to grid's end position without animation, and check the
     *                   three kinds of call back functions are triggered or not.
     * @tc.expected: All functions should be triggered
     */
    pattern_->ScrollTo(ITEM_HEIGHT * 5);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_EQ(offsetY.Value(), ITEM_HEIGHT * 5);
    EXPECT_EQ(willScrollOffset.Value(), ITEM_HEIGHT * 5 * 2);
    EXPECT_EQ(didScrollOffset.Value(), ITEM_HEIGHT * 5 * 3);
    EXPECT_EQ(scrollState, willScrollState);
    EXPECT_EQ(scrollState, didScrollState);
}

/**
 * @tc.name: onWillScrollAndOnDidScroll002
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll, onWillScroll and onDidScroll
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, onWillScrollAndOnDidScroll002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set three kinds of scroll events: onScroll, onWillScroll, onDidScroll
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnWillScrollCallBack = false;
    bool isOnDidScrollCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    Dimension willScrollOffset;
    ScrollState willScrollState;
    auto onWillScroll = [&willScrollOffset, &willScrollState, &isOnWillScrollCallBack](
                            Dimension offset, ScrollState state, ScrollSource source) {
        willScrollOffset = offset * 2;
        willScrollState = state;
        isOnWillScrollCallBack = true;
        ScrollFrameResult result;
        result.offset = offset;
        return result;
    };
    Dimension didScrollOffset;
    ScrollState didScrollState;
    auto onDidScroll = [&didScrollOffset, &didScrollState, &isOnDidScrollCallBack](
                           Dimension offset, ScrollState state) {
        didScrollOffset = offset * 3;
        didScrollState = state;
        isOnDidScrollCallBack = true;
    };

    GridModelNG model = CreateWithNoProxy();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    eventHub_->SetOnWillScroll(onWillScroll);
    eventHub_->SetOnDidScroll(onDidScroll);

    /**
     * @tc.steps: step2. Scroll to grid's end position without animation, and check the
     *                   three kinds of call back functions are triggered or not.
     * @tc.expected: All functions should be triggered
     */
    pattern_->ScrollTo(ITEM_WIDTH * 5);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnWillScrollCallBack);
    EXPECT_TRUE(isOnDidScrollCallBack);
    EXPECT_EQ(offsetY.Value(), ITEM_WIDTH * 5);
    EXPECT_EQ(willScrollOffset.Value(), ITEM_WIDTH * 5 * 2);
    EXPECT_EQ(didScrollOffset.Value(), ITEM_WIDTH * 5 * 3);
    EXPECT_EQ(scrollState, willScrollState);
    EXPECT_EQ(scrollState, didScrollState);
}

/**
 * @tc.name: onWillScrollAndOnDidScroll003
 * @tc.desc: Test Grid(Axis::HORIZONTAL) Scroll Without Scroll Bar Without Animation
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, onWillScrollAndOnDidScroll003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set three kinds of scroll events: onScroll, onWillScroll, onDidScroll
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnWillScrollCallBack = false;
    bool isOnDidScrollCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    Dimension willScrollOffset;
    ScrollState willScrollState;
    auto onWillScroll = [&willScrollOffset, &willScrollState, &isOnWillScrollCallBack](
                            Dimension offset, ScrollState state, ScrollSource source) {
        willScrollOffset = offset * 2;
        willScrollState = state;
        isOnWillScrollCallBack = true;
        ScrollFrameResult result;
        result.offset = offset;
        return result;
    };
    Dimension didScrollOffset;
    ScrollState didScrollState;
    auto onDidScroll = [&didScrollOffset, &didScrollState, &isOnDidScrollCallBack](
                           Dimension offset, ScrollState state) {
        didScrollOffset = offset * 3;
        didScrollState = state;
        isOnDidScrollCallBack = true;
    };

    GridModelNG model = CreateWithNoProxy();
    model.SetRowsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    eventHub_->SetOnWillScroll(onWillScroll);
    eventHub_->SetOnDidScroll(onDidScroll);

    /**
     * @tc.steps: step2. Scroll to a large offset beyond the total height of the grid without animation, and check
     *                   three kinds of call back functions are triggered and values are correct.
     * @tc.expected: All functions should be triggered
     */
    pattern_->ScrollBy(ITEM_WIDTH);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnWillScrollCallBack);
    EXPECT_TRUE(isOnDidScrollCallBack);
    EXPECT_EQ(offsetY.Value(), ITEM_WIDTH);
    EXPECT_EQ(willScrollOffset.Value(), ITEM_WIDTH * 2);
    EXPECT_EQ(didScrollOffset.Value(), ITEM_WIDTH * 3);
    EXPECT_EQ(scrollState, willScrollState);
    EXPECT_EQ(scrollState, didScrollState);
}

/**
 * @tc.name: onWillScrollAndOnDidScroll004
 * @tc.desc: Test Grid(Axis::Vertical) Scroll, onWillScroll and onDidScroll
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, onWillScrollAndOnDidScroll004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set three kinds of scroll events: onScroll, onWillScroll, onDidScroll
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnWillScrollCallBack = false;
    bool isOnDidScrollCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    Dimension willScrollOffset;
    ScrollState willScrollState;
    auto onWillScroll = [&willScrollOffset, &willScrollState, &isOnWillScrollCallBack](
                            Dimension offset, ScrollState state, ScrollSource source) {
        willScrollOffset = offset;
        willScrollState = state;
        isOnWillScrollCallBack = true;
        ScrollFrameResult result;
        result.offset = offset;
        return result;
    };
    Dimension didScrollOffset;
    ScrollState didScrollState;
    auto onDidScroll = [&didScrollOffset, &didScrollState, &isOnDidScrollCallBack](
                           Dimension offset, ScrollState state) {
        didScrollOffset = offset;
        didScrollState = state;
        isOnDidScrollCallBack = true;
    };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    eventHub_->SetOnWillScroll(onWillScroll);
    eventHub_->SetOnDidScroll(onDidScroll);

    /**
     * @tc.steps: step2. Scroll to a large offset beyond the total height of the grid without animation, and check
     *                   three kinds of call back functions are triggered and values are correct.
     * @tc.expected: All functions should be triggered
     */
    pattern_->ScrollTo(ITEM_HEIGHT * 10);
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(isOnScrollCallBack);
    EXPECT_TRUE(isOnWillScrollCallBack);
    EXPECT_TRUE(isOnDidScrollCallBack);
    EXPECT_EQ(offsetY.Value(), ITEM_HEIGHT * 6);
    EXPECT_EQ(willScrollOffset.Value(), ITEM_HEIGHT * 10);
    EXPECT_EQ(didScrollOffset.Value(), ITEM_HEIGHT * 6);
    EXPECT_EQ(scrollState, willScrollState);
    EXPECT_EQ(scrollState, didScrollState);
}

/**
 * @tc.name: onWillScrollAndOnDidScroll005
 * @tc.desc: Test Grid(Axis::Vertical) Scroll, onWillScroll and onDidScroll
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, onWillScrollAndOnDidScroll005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set three kinds of scroll events: onScroll, onWillScroll, onDidScroll
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnWillScrollCallBack = false;
    bool isOnDidScrollCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    Dimension willScrollOffset;
    ScrollState willScrollState;
    auto onWillScroll = [&willScrollOffset, &willScrollState, &isOnWillScrollCallBack](
                            Dimension offset, ScrollState state, ScrollSource source) {
        willScrollOffset = offset;
        willScrollState = state;
        isOnWillScrollCallBack = true;
        ScrollFrameResult result;
        result.offset = offset;
        return result;
    };
    Dimension didScrollOffset;
    ScrollState didScrollState;
    auto onDidScroll = [&didScrollOffset, &didScrollState, &isOnDidScrollCallBack](
                           Dimension offset, ScrollState state) {
        didScrollOffset = offset;
        didScrollState = state;
        isOnDidScrollCallBack = true;
    };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    eventHub_->SetOnWillScroll(onWillScroll);
    eventHub_->SetOnDidScroll(onDidScroll);

    /**
     * @tc.steps: step2. Scroll to a negative position without animation, and check
     *                   three kinds of call back functions are triggered and values are correct.
     * @tc.expected: All functions should be triggered
     */
    pattern_->ScrollTo(-ITEM_HEIGHT);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isOnScrollCallBack);
    EXPECT_FALSE(isOnWillScrollCallBack);
    EXPECT_FALSE(isOnDidScrollCallBack);
    EXPECT_EQ(offsetY.Value(), 0);
    EXPECT_EQ(willScrollOffset.Value(), 0);
    EXPECT_EQ(didScrollOffset.Value(), 0);
    EXPECT_EQ(scrollState, willScrollState);
    EXPECT_EQ(scrollState, didScrollState);
}

/**
 * @tc.name: onWillScrollAndOnDidScroll006
 * @tc.desc: Test Grid(Axis::Vertical) Scroll, onWillScroll and onDidScroll
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollerEventTestNg, onWillScrollAndOnDidScroll006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set three kinds of scroll events: onScroll, onWillScroll, onDidScroll
     * @tc.expected: creat grid
     */
    bool isOnScrollCallBack = false;
    bool isOnWillScrollCallBack = false;
    bool isOnDidScrollCallBack = false;

    Dimension offsetY;
    ScrollState scrollState;
    auto onScroll = [&offsetY, &scrollState, &isOnScrollCallBack](Dimension offset, ScrollState state) {
        offsetY = offset;
        scrollState = state;
        isOnScrollCallBack = true;
    };
    Dimension willScrollOffset;
    ScrollState willScrollState;
    auto onWillScroll = [&willScrollOffset, &willScrollState, &isOnWillScrollCallBack](
                            Dimension offset, ScrollState state, ScrollSource source) {
        willScrollOffset = offset;
        willScrollState = state;
        isOnWillScrollCallBack = true;
        ScrollFrameResult result;
        result.offset = offset;
        return result;
    };
    Dimension didScrollOffset;
    ScrollState didScrollState;
    auto onDidScroll = [&didScrollOffset, &didScrollState, &isOnDidScrollCallBack](
                           Dimension offset, ScrollState state) {
        didScrollOffset = offset;
        didScrollState = state;
        isOnDidScrollCallBack = true;
    };

    GridModelNG model = CreateWithNoProxy();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetOnScroll(onScroll);
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedItems(20);
    CreateDone(frameNode_);
    eventHub_->SetOnWillScroll(onWillScroll);
    eventHub_->SetOnDidScroll(onDidScroll);

    /**
     * @tc.steps: step2. Scroll to a negative position without animation, and check
     *                   three kinds of call back functions are triggered and values are correct.
     * @tc.expected: All functions should be triggered
     */
    pattern_->OnScrollCallback(ITEM_HEIGHT, SCROLL_FROM_UPDATE);
    FlushLayoutTask(frameNode_);
    EXPECT_FALSE(isOnScrollCallBack);
    EXPECT_TRUE(isOnWillScrollCallBack);
    EXPECT_FALSE(isOnDidScrollCallBack);
    EXPECT_EQ(offsetY.Value(), 0);
    EXPECT_EQ(willScrollOffset.Value(), -ITEM_HEIGHT);
    EXPECT_EQ(didScrollOffset.Value(), 0);
    EXPECT_EQ(willScrollState, ScrollState::SCROLL);
    EXPECT_EQ(scrollState, didScrollState);
}
} // namespace OHOS::Ace::NG

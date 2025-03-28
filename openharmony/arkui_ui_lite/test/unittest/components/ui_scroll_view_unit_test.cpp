/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_scroll_view.h"
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const uint16_t size = 0;
    const EasingFunc func(EasingEquation::CubicEaseOut);
    const uint8_t value = 0;
    const uint8_t threshold = 2;
    const uint8_t side = 0;
    const Point center = {0, 0};
}

class ScrollViewTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static UIScrollView* scrollView_;
};

UIScrollView* ScrollViewTest::scrollView_ = nullptr;

void ScrollViewTest::SetUpTestCase()
{
    if (scrollView_ == nullptr) {
        scrollView_ = new UIScrollView();
    }
}

void ScrollViewTest::TearDownTestCase()
{
    if (scrollView_ != nullptr) {
        delete scrollView_;
        scrollView_ = nullptr;
    }
}

/**
 * @tc.name: UIScrollViewSetScrollState_001
 * @tc.desc: Verify SetScrollState function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(ScrollViewTest, UIScrollViewSetScrollState_001, TestSize.Level0)
{
    UIScrollView::OnScrollListener* scrollListener = new UIScrollView::OnScrollListener();
    if (scrollListener == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    uint8_t state = UIScrollView::OnScrollListener::SCROLL_STATE_MOVE;
    scrollListener->SetScrollState(state);
    EXPECT_EQ(scrollListener->GetScrollState(), state);

    state = UIScrollView::OnScrollListener::SCROLL_STATE_STOP;
    scrollListener->SetScrollState(state);
    EXPECT_EQ(scrollListener->GetScrollState(), state);
    delete scrollListener;
}

/**
 * @tc.name: UIScrollViewGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(ScrollViewTest, UIScrollViewGetViewType_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(scrollView_->GetViewType(), UI_SCROLL_VIEW);
}

/**
 * @tc.name: UIScrollViewSetHorizontalScrollState_001
 * @tc.desc: Verify SetHorizontalScrollState function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(ScrollViewTest, UIScrollViewSetHorizontalScrollState_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetHorizontalScrollState(false);
    EXPECT_EQ(scrollView_->GetHorizontalScrollState(), false);

    scrollView_->SetHorizontalScrollState(true);
    EXPECT_EQ(scrollView_->GetHorizontalScrollState(), true);
}

/**
 * @tc.name: UIScrollViewSetVerticalScrollState_001
 * @tc.desc: Verify SetVerticalScrollState function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(ScrollViewTest, UIScrollViewSetVerticalScrollState_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetVerticalScrollState(false);
    EXPECT_EQ(scrollView_->GetVerticalScrollState(), false);

    scrollView_->SetVerticalScrollState(true);
    EXPECT_EQ(scrollView_->GetVerticalScrollState(), true);
}

/**
 * @tc.name: UIScrollViewSetMaxScrollDistance_001
 * @tc.desc: Verify SetMaxScrollDistance function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000E2RJK
 */
HWTEST_F(ScrollViewTest, UIScrollViewSetMaxScrollDistance_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const uint16_t distance = 10;
    scrollView_->SetMaxScrollDistance(distance);
    EXPECT_EQ(scrollView_->GetMaxScrollDistance(), distance);
}

/**
 * @tc.name: UIScrollViewSetDragACCLevel_001
 * @tc.desc: Verify SetDragACCLevel function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000E2RJJ
 */
HWTEST_F(ScrollViewTest, UIScrollViewSetDragACCLevel_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const uint16_t value = 10;
    scrollView_->SetDragACCLevel(value);
    EXPECT_EQ(scrollView_->GetDragACCLevel(), value);
}

/**
 * @tc.name: UIScrollViewSetSwipeACCLevel_001
 * @tc.desc: Verify SetSwipeACCLevel function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5P
 */
HWTEST_F(ScrollViewTest, UIScrollViewSetSwipeACCLevel_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const uint16_t value = 10;
    scrollView_->SetSwipeACCLevel(value);
    EXPECT_EQ(scrollView_->GetSwipeACCLevel(), value);
}

/**
 * @tc.name: UIScrollViewMoveChildByOffset_001
 * @tc.desc: Verify MoveChildByOffset function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEM
 */
HWTEST_F(ScrollViewTest, UIScrollViewMoveChildByOffset_001, TestSize.Level0)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView* view = new UIView();
    EXPECT_EQ(view->GetX(), 0);
    EXPECT_EQ(view->GetY(), 0);

    scrollView_->Add(view);

    const int16_t offsetX = 10;
    const int16_t offsetY = 20;
    scrollView_->SetThrowDrag(true);
    scrollView_->MoveChildByOffset(offsetX, offsetY);
    EXPECT_EQ(view->GetX(), offsetX);
    EXPECT_EQ(view->GetY(), offsetY);

    scrollView_->Remove(view);
    delete view;
}

/**
 * @tc.name: UIAbstractClockSetScrollBlankSize_001
 * @tc.desc: Verify SetScrollBlankSize function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetScrollBlankSize_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetScrollBlankSize(size);
    EXPECT_EQ(scrollView_->GetScrollBlankSize(), size);
}

/**
 * @tc.name: UIAbstractClockSetReboundSize_001
 * @tc.desc: Verify SetReboundSize function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetReboundSize_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetReboundSize(size);
    EXPECT_EQ(scrollView_->GetReboundSize(), size);
}

/**
 * @tc.name: UIAbstractClockSetDragFunc_001
 * @tc.desc: Verify SetDragFunc function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetDragFunc_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetDragFunc(func);
    EXPECT_EQ(scrollView_->GetDragFunc(), func);
}

#if defined(ENABLE_ROTATE_INPUT) && ENABLE_ROTATE_INPUT
/**
 * @tc.name: UIAbstractClockSetRotateACCLevel_001
 * @tc.desc: Verify SetRotateACCLevel function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetRotateACCLevel_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetRotateACCLevel(value);
    EXPECT_EQ(scrollView_->GetRotateACCLevel(), value);
}

/**
 * @tc.name: UIAbstractClockSetRotateThrowThreshold_001
 * @tc.desc: Verify SetRotateThrowThreshold function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetRotateThrowThreshold_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetRotateThrowThreshold(threshold);
    EXPECT_EQ(scrollView_->GetRotateThrowThreshold(), threshold);
}

/**
 * @tc.name: UIAbstractClockRotateEvent_001
 * @tc.desc: Verify OnRotateStartEvent OnRotateEvent OnRotateEndEvent function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollRotateEvent_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    RotateEvent* event = new RotateEvent(1);
    EXPECT_EQ(scrollView_->OnRotateStartEvent(*event), false);
    EXPECT_EQ(scrollView_->OnRotateEvent(*event), false);
    EXPECT_EQ(scrollView_->OnRotateEndEvent(*event), false);
    delete event;
    event = nullptr;
}
#endif

/**
 * @tc.name: UIAbstractClockSetScrollBarSide_001
 * @tc.desc: Verify SetScrollBarSide function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetScrollBarSide_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetScrollBarSide(side);
    EXPECT_EQ(scrollView_->GetScrollBarSide(), side);
}

/**
 * @tc.name: UIAbstractClockSetScrollBarCenter_001
 * @tc.desc: Verify SetScrollBarCenter function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetScrollBarCenter_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetScrollBarCenter(center);
    EXPECT_EQ(scrollView_->GetScrollBarCenter().x, center.x);
    EXPECT_EQ(scrollView_->GetScrollBarCenter().y, center.y);
}

/**
 * @tc.name: UIAbstractClockSetDirection_001
 * @tc.desc: Verify SetDirection function.
 */
HWTEST_F(ScrollViewTest, UIAbstractScrollSetDirection_001, TestSize.Level1)
{
    if (scrollView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    scrollView_->SetDirection(2);
    EXPECT_EQ(scrollView_->GetDirection(), 2);
}
} // namespace OHOS

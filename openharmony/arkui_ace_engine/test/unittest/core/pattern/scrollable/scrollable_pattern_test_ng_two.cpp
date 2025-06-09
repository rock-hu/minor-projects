/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>

#include "gtest/gtest.h"
#include "test/unittest/core/pattern/test_ng.h"

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/list/list_pattern.h"
#include "core/components_ng/pattern/scrollable/scrollable_pattern.h"

namespace OHOS::Ace::NG {
using namespace testing;
using namespace testing::ext;

class ScrollableTestNg : public TestNG {
public:
};

/**
 * @tc.name: IsSnapStopped001
 * @tc.desc: Test Scrollable IsSnapStopped
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, Scrollable_IsSnapStopped001, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    scrollable->state_ = Scrollable::AnimationState::TRANSITION;
    auto result = scrollable->IsSnapStopped();
    EXPECT_TRUE(result);

    scrollable->state_ = Scrollable::AnimationState::SNAP;
    result = scrollable->IsSnapStopped();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsSpringStopped001
 * @tc.desc: Test Scrollable IsSpringStopped
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, Scrollable_IsSpringStopped001, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    scrollable->state_ = Scrollable::AnimationState::SPRING;
    auto result = scrollable->IsSpringStopped();
    EXPECT_FALSE(result);

    scrollable->state_ = Scrollable::AnimationState::FRICTION;
    result = scrollable->IsSpringStopped();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: HandleDragUpdate001
 * @tc.desc: Test Scrollable HandleDragUpdate
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, HandleDragUpdate001, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    GestureEvent info;
    info.mainVelocity_ = 2.0;
    info.inputEventType_ = InputEventType::AXIS;
    scrollable->state_ = Scrollable::AnimationState::SNAP;
    scrollable->getSnapTypeCallback_ = []() { return SnapType::SCROLL_SNAP; };
    scrollable->HandleDragUpdate(info);
    EXPECT_TRUE(scrollable->isDragUpdateStop_);
    EXPECT_NE(scrollable->state_, Scrollable::AnimationState::IDLE);
}

/**
 * @tc.name: HandleDragUpdate002
 * @tc.desc: Test Scrollable HandleDragUpdate
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, HandleDragUpdate002, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    GestureEvent info;
    info.mainVelocity_ = 2.0;
    info.inputEventType_ = InputEventType::AXIS;
    scrollable->state_ = Scrollable::AnimationState::SNAP;
    scrollable->currentPos_ = 100.0;
    scrollable->getSnapTypeCallback_ = []() { return SnapType::NONE_SNAP; };
    scrollable->HandleDragUpdate(info);
    EXPECT_EQ(scrollable->state_, Scrollable::AnimationState::IDLE);
    EXPECT_EQ(scrollable->currentPos_, 0.0);
}

/**
 * @tc.name: UpdateScrollSnapStartOffset
 * @tc.desc: Test Scrollable UpdateScrollSnapStartOffset
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, UpdateScrollSnapStartOffset, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    scrollable->state_ = Scrollable::AnimationState::SNAP;
    scrollable->endPos_ = 6.0;
    scrollable->updateSnapAnimationCount_ = 2;
    scrollable->UpdateScrollSnapStartOffset(2.0);
    EXPECT_EQ(scrollable->endPos_, 4.0);
}

/**
 * @tc.name: ProcessListSnapMotion001
 * @tc.desc: Test Scrollable ProcessListSnapMotion
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, ProcessListSnapMotion001, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    scrollable->state_ = Scrollable::AnimationState::SNAP;
    scrollable->currentPos_ = 2.0;
    scrollable->snapVelocity_ = 4.0;
    scrollable->nextStep_ = 1.0f;
    scrollable->handleScrollCallback_ = [](float offset, int32_t source, NestedState state) {
        return ScrollResult { offset, true };
    };
    scrollable->ProcessListSnapMotion(6.0);
    EXPECT_EQ(scrollable->state_, Scrollable::AnimationState::IDLE);
    EXPECT_FALSE(scrollable->nextStep_.has_value());
}

/**
 * @tc.name: ProcessListSnapMotion002
 * @tc.desc: Test Scrollable ProcessListSnapMotion
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, ProcessListSnapMotion002, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    scrollable->state_ = Scrollable::AnimationState::FRICTION;
    scrollable->currentPos_ = 2.0;
    scrollable->snapVelocity_ = 4.0;
    scrollable->nextStep_ = 1.0f;
    scrollable->canOverScroll_ = false;
    scrollable->handleScrollCallback_ = [](float offset, int32_t source, NestedState state) {
        return ScrollResult { offset, true };
    };
    scrollable->ProcessListSnapMotion(6.0);
    EXPECT_NE(scrollable->state_, Scrollable::AnimationState::IDLE);
    EXPECT_FALSE(scrollable->scrollPause_);
}

/**
 * @tc.name: OnAnimateStop001
 * @tc.desc: Test Scrollable OnAnimateStop
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, OnAnimateStop001, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    scrollable->currentVelocity_ = 2.0;
    scrollable->moved_ = true;
    scrollable->OnAnimateStop();
    EXPECT_EQ(scrollable->currentVelocity_, 0.0);
    EXPECT_FALSE(scrollable->moved_);
}

/**
 * @tc.name: StopAxisAnimation001
 * @tc.desc: Test Scrollable StopAxisAnimation
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, StopAxisAnimation001, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    RefPtr<PipelineBase> context = AceType::MakeRefPtr<PipelineContext>();
    auto animator = Animator::CreateAnimator(nullptr, context, nullptr);
    animator->status_ = Animator::Status::RUNNING;
    animator->isBothBackwards = true;
    RefPtr<AxisAnimator> axisAnimator = AceType::MakeRefPtr<AxisAnimator>();
    axisAnimator->axisScrollAnimator_ = animator;
    scrollable->axisAnimator_ = axisAnimator;
    scrollable->moved_ = true;
    scrollable->StopAxisAnimation();
    EXPECT_EQ(scrollable->axisAnimator_->axisScrollAnimator_->status_, Animator::Status::STOPPED);
    EXPECT_FALSE(scrollable->axisAnimator_->axisScrollAnimator_->isBothBackwards);
}

/**
 * @tc.name: ProcessScrollOverCallback001
 * @tc.desc: Test Scrollable ProcessScrollOverCallback
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, ProcessScrollOverCallback001, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    double number = 4.0;
    scrollable->outBoundaryCallback_ = []() { return false; };
    scrollable->canOverScroll_ = false;
    scrollable->notifyScrollOverCallback_ = [&number](double velocity) { number += velocity; };
    scrollable->ProcessScrollOverCallback(2.0);
    EXPECT_NE(number, 6.0);
}

/**
 * @tc.name: ProcessScrollOverCallback002
 * @tc.desc: Test Scrollable ProcessScrollOverCallback
 * @tc.type: FUNC
 */
HWTEST_F(ScrollableTestNg, ProcessScrollOverCallback002, TestSize.Level1)
{
    RefPtr<Scrollable> scrollable =
        AceType::MakeRefPtr<Scrollable>([](double position, int32_t source) { return true; }, Axis::VERTICAL);
    double number = 4.0;
    scrollable->outBoundaryCallback_ = []() { return false; };
    scrollable->canOverScroll_ = true;
    scrollable->notifyScrollOverCallback_ = [&number](double velocity) { number += velocity; };
    scrollable->ProcessScrollOverCallback(2.0);
    EXPECT_EQ(number, 6.0);
}
} // namespace OHOS::Ace::NG
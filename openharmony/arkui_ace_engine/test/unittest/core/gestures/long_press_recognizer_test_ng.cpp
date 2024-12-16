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
#include "test/unittest/core/gestures/gestures_common_test_ng.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
class LongPressRecognizerTestNg : public GesturesCommonTestNg {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
};

void LongPressRecognizerTestNg::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
}

void LongPressRecognizerTestNg::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
}

/**
 * @tc.name: LongPressRecognizerTest001
 * @tc.desc: Test LongPressRecognizer function: OnAccepted OnRejected
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    OnLongPress onLongPress;
    TouchEvent touchEvent;

    /**
     * @tc.steps: step2. call OnAccepted function and compare result.
     * @tc.steps: case1: !onLongPress, !empty, repeat
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.touchPoints_[touchEvent.id] = touchEvent;
    longPressRecognizer.repeat_ = true;
    longPressRecognizer.OnAccepted();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);

    /**
     * @tc.steps: step2. call OnAccepted function and compare result.
     * @tc.steps: case2: !onLongPress, empty, !repeat
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.touchPoints_.clear();
    longPressRecognizer.repeat_ = false;
    longPressRecognizer.OnAccepted();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);

    /**
     * @tc.steps: step2. call OnAccepted function and compare result.
     * @tc.steps: case3: onLongPress, empty, !repeat
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.touchPoints_.clear();
    longPressRecognizer.repeat_ = false;
    longPressRecognizer.OnAccepted();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);

    /**
     * @tc.steps: step2. call OnAccepted function and compare result.
     * @tc.steps: case4: onLongPress, !empty, !repeat
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.touchPoints_.clear();
    longPressRecognizer.touchPoints_[touchEvent.id] = touchEvent;
    longPressRecognizer.repeat_ = false;
    longPressRecognizer.OnAccepted();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);

    /**
     * @tc.steps: step3. call OnRejected function and compare result.
     * @tc.expected: step3. result equals.
     */
    longPressRecognizer.OnRejected();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerTest002
 * @tc.desc: Test LongPressRecognizer function: HandleTouchMoveEvent
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case1: referee is not SUCCEED
     * @tc.expected: step2. result equals.
     */
    TouchEvent touchEvent;
    longPressRecognizer.touchPoints_[touchEvent.id] = touchEvent;
    longPressRecognizer.HandleTouchMoveEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.time_, touchEvent.time);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case2: referee is SUCCEED
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer.HandleTouchMoveEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.time_, touchEvent.time);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case2: referee is SUCCEED
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.currentFingers_ = longPressRecognizer.fingers_;
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer.HandleTouchMoveEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.time_, touchEvent.time);
}

/**
 * @tc.name: LongPressRecognizerTest003
 * @tc.desc: Test LongPressRecognizer function: HandleTouchDownEvent
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case1: pointsCount == fingers, useCatchMode_ is true
     * @tc.expected: step2. result equals.
     */
    TouchEvent touchEvent;
    longPressRecognizer.HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 1);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case2: pointsCount == fingers, useCatchMode_ is true
     * @tc.expected: step2. result equals.
     */
    touchEvent.sourceType = SourceType::MOUSE;
    longPressRecognizer.isForDrag_ = true;
    longPressRecognizer.touchPoints_[touchEvent.id] = touchEvent;
    longPressRecognizer.fingers_ = 1;
    longPressRecognizer.useCatchMode_ = true;
    longPressRecognizer.HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.globalPoint_.GetX(), touchEvent.x);
    EXPECT_EQ(longPressRecognizer.globalPoint_.GetY(), touchEvent.y);
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::READY);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case3: pointsCount == fingers, useCatchMode_ is false
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.useCatchMode_ = false;
    longPressRecognizer.HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::READY);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case4: referee is SUCCEED
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer.HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 1);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case5: change SourceType to KEYBOARD
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::PENDING;
    touchEvent.sourceType = SourceType::KEYBOARD;
    longPressRecognizer.HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 1);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case6: change isForDrag
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.isForDrag_ = !longPressRecognizer.isForDrag_;
    longPressRecognizer.HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 1);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: case7: change isDisableMouseLeft_
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.isDisableMouseLeft_ = !longPressRecognizer.isDisableMouseLeft_;
    longPressRecognizer.HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 1);
}

/**
 * @tc.name: LongPressRecognizerTest004
 * @tc.desc: Test LongPressRecognizer function: HandleTouchCancelEvent UpEvent
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: refereeState == RefereeState::SUCCEED
     * @tc.expected: step2. result equals.
     */
    TouchEvent touchEvent;
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer.HandleTouchUpEvent(touchEvent);
    longPressRecognizer.HandleTouchCancelEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);

    /**
     * @tc.steps: step2. call HandleTouchUpEvent function and compare result.
     * @tc.steps: refereeState == RefereeState::SUCCEED
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer.currentFingers_ = longPressRecognizer.fingers_;
    longPressRecognizer.HandleTouchUpEvent(touchEvent);
    longPressRecognizer.HandleTouchCancelEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);
}

/**
 * @tc.name: LongPressRecognizerDoRepeatTest001
 * @tc.desc: Test LongPressRecognizer function: DoRepeat
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerDoRepeatTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. call DoRepeat
     * @tc.steps: refereeState == RefereeState::SUCCEED
     * @tc.expected: step2. result equals.
     */
    TouchEvent touchEvent;
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer.fingers_ = 0;
    longPressRecognizer.DoRepeat();
    longPressRecognizer.HandleTouchCancelEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);

    /**
     * @tc.steps: step2. call DoRepeat
     * @tc.steps: refereeState == RefereeState::SUCCEED
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::DETECTING;
    longPressRecognizer.currentFingers_ = longPressRecognizer.fingers_;
    longPressRecognizer.fingers_ = 0;
    longPressRecognizer.DoRepeat();
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);
}

/**
 * @tc.name: LongPressRecognizerTest005
 * @tc.desc: Test LongPressRecognizer function: SendCallbackMsg
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    bool isRepeat = false;

    /**
     * @tc.steps: step2. call SendCallbackMsg function and compare result.
     * @tc.steps: case1: onAction is no, *onAction is no
     * @tc.expected: step2. result equals.
     */
    std::unique_ptr<GestureEventFunc> onAction;
    longPressRecognizer.SendCallbackMsg(onAction, isRepeat);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);

    /**
     * @tc.steps: step2. call SendCallbackMsg function and compare result.
     * @tc.steps: case2: onAction is yes, *onAction is no
     * @tc.expected: step2. result equals.
     */
    onAction = std::make_unique<GestureEventFunc>();
    longPressRecognizer.SendCallbackMsg(onAction, isRepeat);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);

    /**
     * @tc.steps: step2. call SendCallbackMsg function and compare result.
     * @tc.steps: case3: onAction is yes, *onAction is yes, touchEvent is empty
     * @tc.expected: step2. result equals.
     */
    onAction = std::make_unique<GestureEventFunc>([](GestureEvent) {});
    longPressRecognizer.SendCallbackMsg(onAction, isRepeat);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);

    /**
     * @tc.steps: step2. call SendCallbackMsg function and compare result.
     * @tc.steps: case4: touchEvent is not empty, have no X and Y
     * @tc.expected: step2. result equals.
     */
    TouchEvent touchEvent;
    longPressRecognizer.touchPoints_[touchEvent.id] = touchEvent;
    longPressRecognizer.SendCallbackMsg(onAction, isRepeat);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 1);

    /**
     * @tc.steps: step2. call SendCallbackMsg function and compare result.
     * @tc.steps: case4: touchEvent is not empty, have no X and Y
     * @tc.expected: step2. result equals.
     */
    touchEvent.tiltX = 0.0f;
    touchEvent.tiltY = 0.0f;
    longPressRecognizer.touchPoints_[touchEvent.id] = touchEvent;
    longPressRecognizer.SendCallbackMsg(onAction, isRepeat);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 1);
}

/**
 * @tc.name: LongPressRecognizerTest006
 * @tc.desc: Test LongPressRecognizer function: ReconcileFrom
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    RefPtr<LongPressRecognizer> longPressRecognizerPtr =
        AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION, FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. call ReconcileFrom function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    auto result = longPressRecognizer.ReconcileFrom(longPressRecognizerPtr);
    EXPECT_EQ(result, true);

    /**
     * @tc.steps: step2. call ReconcileFrom function and compare result.
     * @tc.steps: case2: recognizerPtr is nullptr
     * @tc.expected: step2. result equals.
     */
    result = longPressRecognizer.ReconcileFrom(nullptr);
    EXPECT_EQ(result, false);

    /**
     * @tc.steps: step2. call ReconcileFrom function and compare result.
     * @tc.steps: case3: recognizerPtr, duration not same
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.duration_ = 0;
    result = longPressRecognizer.ReconcileFrom(longPressRecognizerPtr);
    EXPECT_EQ(result, false);

    /**
     * @tc.steps: step2. call ReconcileFrom function and compare result.
     * @tc.steps: case4: recognizerPtr, duration same, fingers not same
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.duration_ = longPressRecognizerPtr->duration_;
    longPressRecognizer.fingers_ = longPressRecognizerPtr->fingers_ + 1;
    result = longPressRecognizer.ReconcileFrom(longPressRecognizerPtr);
    EXPECT_EQ(result, false);

    /**
     * @tc.steps: step2. call ReconcileFrom function and compare result.
     * @tc.steps: case5: recognizerPtr, fingers same, repeat not same
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.fingers_ = longPressRecognizerPtr->fingers_;
    longPressRecognizer.repeat_ = !longPressRecognizerPtr->repeat_;
    result = longPressRecognizer.ReconcileFrom(longPressRecognizerPtr);
    EXPECT_EQ(result, false);

    /**
     * @tc.steps: step2. call ReconcileFrom function and compare result.
     * @tc.steps: case5: recognizerPtr, repeat same, priorityMask not same
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.repeat_ = longPressRecognizerPtr->repeat_;
    longPressRecognizer.priorityMask_ = GestureMask::End;
    result = longPressRecognizer.ReconcileFrom(longPressRecognizerPtr);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc001
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    bool isCatchMode = false;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    auto frameNode = FrameNode::CreateFrameNode("myButton", 100, AceType::MakeRefPtr<Pattern>());
    longPressRecognizer.AttachFrameNode(frameNode);
    longPressRecognizer.HandleOverdueDeadline(isCatchMode);
    longPressRecognizer.DoRepeat();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);
    auto result = longPressRecognizer.GetLongPressActionFunc();
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc003
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    bool isCatchMode = false;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    auto frameNode = FrameNode::CreateFrameNode("myButton", 100, AceType::MakeRefPtr<Pattern>());
    longPressRecognizer.AttachFrameNode(frameNode);
    longPressRecognizer.HandleOverdueDeadline(isCatchMode);
    longPressRecognizer.DoRepeat();
    GestureEventFunc click;
    GestureEvent info;
    click = longPressRecognizer.GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc004
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    bool isCatchMode = false;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    auto frameNode = FrameNode::CreateFrameNode("myButton", 100, AceType::MakeRefPtr<Pattern>());
    longPressRecognizer.AttachFrameNode(frameNode);
    longPressRecognizer.HandleOverdueDeadline(isCatchMode);
    longPressRecognizer.DoRepeat();
    GestureEventFunc click;
    GestureEvent info;
    auto onActionStart = [](GestureEvent& info) { return true; };
    auto onActionUpdate = [](GestureEvent& info) { return true; };
    auto onActionEnd = [](GestureEvent& info) { return true; };
    longPressRecognizer.SetOnActionUpdate(onActionUpdate);
    longPressRecognizer.SetOnAction(onActionStart);
    longPressRecognizer.SetOnActionEnd(onActionEnd);
    click = longPressRecognizer.GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc002
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    bool isCatchMode = false;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer.fingers_ = SINGLE_FINGER_NUMBER;
    auto frameNode = FrameNode::CreateFrameNode("myButton", 100, AceType::MakeRefPtr<Pattern>());
    longPressRecognizer.AttachFrameNode(frameNode);
    longPressRecognizer.HandleOverdueDeadline(isCatchMode);
    longPressRecognizer.DoRepeat();
    auto result = longPressRecognizer.GetLongPressActionFunc();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::DETECTING;
    longPressRecognizer.fingers_ = SINGLE_FINGER_NUMBER;
    longPressRecognizer.HandleOverdueDeadline(isCatchMode);
    longPressRecognizer.DoRepeat();
    result = longPressRecognizer.GetLongPressActionFunc();
    EXPECT_NE(longPressRecognizer.refereeState_, RefereeState::DETECTING);
}

/**
 * @tc.name: LongPressRecognizerConvertPxToVpTest001
 * @tc.desc: Test LongPressRecognizer function: ConvertPxToVp
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerConvertPxToVpTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. call ConvertPxToVp function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    double result = longPressRecognizer.ConvertPxToVp(0.0);
    EXPECT_EQ(result, 0.0);
}

/**
 * @tc.name: LongPressRecognizerTest007
 * @tc.desc: Test LongPressRecognizer function: HandleOverdueDeadline  DoRepeat
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    bool isCatchMode = false;

    /**
     * @tc.steps: step2. call HandleOverdueDeadline function and compare result.
     * @tc.steps: case1: refereeState is SUCCESS, return
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::SUCCEED;
    auto frameNode = FrameNode::CreateFrameNode("myButton", 100, AceType::MakeRefPtr<Pattern>());
    longPressRecognizer.AttachFrameNode(frameNode);
    longPressRecognizer.HandleOverdueDeadline(isCatchMode);
    longPressRecognizer.DoRepeat();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);

    /**
     * @tc.steps: step2. call HandleOverdueDeadline function and compare result.
     * @tc.steps: case1: refereeState is DETECTING, isCatchMode is false
     * @tc.expected: step2. result equals.
     */
    longPressRecognizer.refereeState_ = RefereeState::DETECTING;
    longPressRecognizer.HandleOverdueDeadline(isCatchMode);
    longPressRecognizer.DoRepeat();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerSendCallbackMsgTest001
 * @tc.desc: Test LongPressRecognizer function: SendCallbackMsg
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerSendCallbackMsgTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create longPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);
    std::unique_ptr<GestureEventFunc> onAction;

    /**
     * @tc.steps: step2. call SendCallbackMsg function and compare result.
     * @tc.steps: case3: onAction is yes, *onAction is yes, touchEvent is empty, type is AXIS
     * @tc.expected: step2. result equals.
     */
    onAction = std::make_unique<GestureEventFunc>([](GestureEvent) {});
    longPressRecognizer.SendCallbackMsg(onAction, true);
    EXPECT_EQ(longPressRecognizer.touchPoints_.size(), 0);
}

/**
 * @tc.name: LongPressGestureTest001
 * @tc.desc: Test LongPressGesture CreateRecognizer function
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressGestureTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressGesture.
     */
    LongPressGestureModelNG longPressGestureModelNG;
    longPressGestureModelNG.Create(FINGER_NUMBER, false, LONG_PRESS_DURATION);

    RefPtr<GestureProcessor> gestureProcessor;
    gestureProcessor = NG::ViewStackProcessor::GetInstance()->GetOrCreateGestureProcessor();
    auto longPressGestureNG = AceType::DynamicCast<NG::LongPressGesture>(gestureProcessor->TopGestureNG());
    EXPECT_EQ(longPressGestureNG->duration_, LONG_PRESS_DURATION);

    LongPressGesture longPressGesture = LongPressGesture(FINGER_NUMBER, false, LONG_PRESS_DURATION, false, false);
    EXPECT_EQ(longPressGesture.repeat_, false);
    EXPECT_EQ(longPressGesture.duration_, LONG_PRESS_DURATION);
    EXPECT_EQ(longPressGesture.isForDrag_, false);
    EXPECT_EQ(longPressGesture.isDisableMouseLeft_, false);

    /**
     * @tc.steps: step2. call CreateRecognizer function and compare result
     * @tc.steps: case1: onActionId, onActionEndId, onActionCancelId not existed
     */
    auto longPressRecognizer = AceType::DynamicCast<LongPressRecognizer>(longPressGesture.CreateRecognizer());
    EXPECT_NE(longPressRecognizer, nullptr);
    EXPECT_EQ(longPressRecognizer->repeat_, false);
    EXPECT_EQ(longPressRecognizer->duration_, LONG_PRESS_DURATION);
    EXPECT_EQ(longPressRecognizer->isForDrag_, false);
    EXPECT_EQ(longPressRecognizer->isDisableMouseLeft_, false);

    /**
     * @tc.steps: step2. call CreateRecognizer function and compare result
     * @tc.steps: case2: onActionId, onActionEndId, onActionCancelId existed
     */
    std::unique_ptr<GestureEventFunc> onActionId;
    std::unique_ptr<GestureEventFunc> onActionEndId;
    std::unique_ptr<GestureEventNoParameter> onActionCancelId;
    longPressGesture.onActionId_ = std::move(onActionId);
    longPressGesture.onActionEndId_ = std::move(onActionEndId);
    longPressGesture.onActionCancelId_ = std::move(onActionCancelId);
    longPressRecognizer = AceType::DynamicCast<LongPressRecognizer>(longPressGesture.CreateRecognizer());
    EXPECT_NE(longPressRecognizer, nullptr);
    EXPECT_EQ(longPressRecognizer->repeat_, false);
    EXPECT_EQ(longPressRecognizer->duration_, LONG_PRESS_DURATION);
    EXPECT_EQ(longPressRecognizer->isForDrag_, false);
    EXPECT_EQ(longPressRecognizer->isDisableMouseLeft_, false);
}

/**
 * @tc.name: LongPressGestureCreateRecognizerTest001
 * @tc.desc: Test LongPressGesture CreateRecognizer function
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressGestureCreateRecognizerTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressGesture.
     */
    LongPressGestureModelNG longPressGestureModelNG;
    longPressGestureModelNG.Create(FINGER_NUMBER, false, LONG_PRESS_DURATION);

    RefPtr<GestureProcessor> gestureProcessor;
    gestureProcessor = NG::ViewStackProcessor::GetInstance()->GetOrCreateGestureProcessor();
    auto longPressGestureNG = AceType::DynamicCast<NG::LongPressGesture>(gestureProcessor->TopGestureNG());
    EXPECT_EQ(longPressGestureNG->duration_, LONG_PRESS_DURATION);

    LongPressGesture longPressGesture = LongPressGesture(FINGER_NUMBER, false, LONG_PRESS_DURATION, false, false);
    EXPECT_EQ(longPressGesture.repeat_, false);
    EXPECT_EQ(longPressGesture.duration_, LONG_PRESS_DURATION);
    EXPECT_EQ(longPressGesture.isForDrag_, false);
    EXPECT_EQ(longPressGesture.isDisableMouseLeft_, false);

    /**
     * @tc.steps: step2. call CreateRecognizer function and compare result
     * @tc.steps: case1: onActionId, onActionEndId, onActionCancelId not existed
     */
    longPressGesture.fingers_ = FINGER_NUMBER_OVER_MAX;
    longPressGesture.duration_ = 0;
    auto longPressRecognizer = AceType::DynamicCast<LongPressRecognizer>(longPressGesture.CreateRecognizer());
    EXPECT_NE(longPressRecognizer, nullptr);

    /**
     * @tc.steps: step2. call CreateRecognizer function and compare result
     * @tc.steps: case1: onActionId, onActionEndId, onActionCancelId not existed
     */
    longPressGesture.fingers_ = 0;
    longPressGesture.duration_ = 0;
    longPressRecognizer = AceType::DynamicCast<LongPressRecognizer>(longPressGesture.CreateRecognizer());
    EXPECT_NE(longPressRecognizer, nullptr);
}


/**
 * @tc.name: LongPressGestureCreateRecognizerTest002
 * @tc.desc: Test LongPressGesture CreateRecognizer function
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressGestureCreateRecognizerTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressGesture.
     */
    LongPressGestureModelNG longPressGestureModelNG;
    longPressGestureModelNG.Create(FINGER_NUMBER, false, LONG_PRESS_DURATION);

    RefPtr<GestureProcessor> gestureProcessor;
    gestureProcessor = NG::ViewStackProcessor::GetInstance()->GetOrCreateGestureProcessor();
    auto longPressGestureNG = AceType::DynamicCast<NG::LongPressGesture>(gestureProcessor->TopGestureNG());
    EXPECT_EQ(longPressGestureNG->duration_, LONG_PRESS_DURATION);

    LongPressGesture longPressGesture = LongPressGesture(FINGER_NUMBER, false, LONG_PRESS_DURATION, false, false);
    EXPECT_EQ(longPressGesture.repeat_, false);
    EXPECT_EQ(longPressGesture.duration_, LONG_PRESS_DURATION);
    EXPECT_EQ(longPressGesture.isForDrag_, false);
    EXPECT_EQ(longPressGesture.isDisableMouseLeft_, false);

    /**
     * @tc.steps: step2. call CreateRecognizer function and compare result
     * @tc.steps: case1: onActionId, onActionEndId, onActionCancelId not existed
     */
    longPressGesture.fingers_ = FINGER_NUMBER_OVER_MAX;
    longPressGesture.duration_ = 0;
    auto onActionStart = [](GestureEvent& info) { return true; };
    auto onActionEnd = [](GestureEvent& info) { return true; };
    auto onActionCancel = []() { return true; };
    longPressGesture.SetOnActionId(onActionStart);
    longPressGesture.SetOnActionEndId(onActionEnd);
    longPressGesture.SetOnActionCancelId(onActionCancel);
    auto longPressRecognizer = AceType::DynamicCast<LongPressRecognizer>(longPressGesture.CreateRecognizer());
    EXPECT_NE(longPressRecognizer, nullptr);

    /**
     * @tc.steps: step2. call CreateRecognizer function and compare result
     * @tc.steps: case1: onActionId, onActionEndId, onActionCancelId not existed
     */
    longPressGesture.fingers_ = 0;
    longPressGesture.duration_ = 0;
    longPressRecognizer = AceType::DynamicCast<LongPressRecognizer>(longPressGesture.CreateRecognizer());
    EXPECT_NE(longPressRecognizer, nullptr);
}

/**
 * @tc.name: LongPressRecognizerHandleTouchUpEventTest009
 * @tc.desc: Test LongPressRecognizer function: HandleTouchUpEvent
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleTouchUpEventTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create PinchRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    TouchEvent touchEvent;
    touchEvent.x = 100.0;
    touchEvent.y = 100.0;

    longPressRecognizer->refereeState_ = RefereeState::DETECTING;
    longPressRecognizer->currentFingers_ = longPressRecognizer->fingers_;
    longPressRecognizer->HandleTouchUpEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer->repeat_, false);
}

/**
 * @tc.name: LongPressRecognizerHandleTouchCancelEventTest001
 * @tc.desc: Test LongPressRecognizer function: HandleTouchCancelEvent
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleTouchCancelEventTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    TouchEvent touchEvent;
    touchEvent.x = 100.0;
    touchEvent.y = 100.0;

    longPressRecognizer->refereeState_ = RefereeState::DETECTING;
    longPressRecognizer->currentFingers_ = longPressRecognizer->fingers_;
    longPressRecognizer->HandleTouchCancelEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer->repeat_, false);
}

/**
 * @tc.name: LongPressRecognizerHandleTouchDownEventTest001
 * @tc.desc: Test LongPressRecognizer function: HandleTouchDownEvent
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleTouchDownEventTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    TouchEvent touchEvent;
    touchEvent.x = 100.0;
    touchEvent.y = 100.0;
    touchEvent.sourceType = SourceType::MOUSE;

    longPressRecognizer->refereeState_ = RefereeState::DETECTING;
    longPressRecognizer->isDisableMouseLeft_ = true;
    longPressRecognizer->currentFingers_ = longPressRecognizer->fingers_;
    longPressRecognizer->HandleTouchDownEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer->repeat_, false);
}

/**
 * @tc.name: LongPressRecognizerHandleTouchMoveEventTest001
 * @tc.desc: Test LongPressRecognizer function: HandleTouchMoveEvent
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleTouchMoveEventTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    TouchEvent touchEvent;
    touchEvent.x = 100.0;
    touchEvent.y = 100.0;
    touchEvent.sourceType = SourceType::MOUSE;

    longPressRecognizer->refereeState_ = RefereeState::DETECTING;
    longPressRecognizer->isDisableMouseLeft_ = true;
    longPressRecognizer->currentFingers_ = longPressRecognizer->fingers_;
    longPressRecognizer->isForDrag_ = true;
    longPressRecognizer->HandleTouchMoveEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer->repeat_, false);

    longPressRecognizer->refereeState_ = RefereeState::DETECTING;
    longPressRecognizer->isDisableMouseLeft_ = false;
    longPressRecognizer->currentFingers_ = longPressRecognizer->fingers_;
    longPressRecognizer->isForDrag_ = false;
    longPressRecognizer->HandleTouchMoveEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer->repeat_, false);
}

/**
 * @tc.name: LongPressRecognizerHandleOverdueDeadlineTest001
 * @tc.desc: Test LongPressRecognizer function: HandleOverdueDeadline
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleOverdueDeadlineTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    TouchEvent touchEvent;
    touchEvent.x = 100.0;
    touchEvent.y = 100.0;
    touchEvent.sourceType = SourceType::MOUSE;

    longPressRecognizer->refereeState_ = RefereeState::DETECTING;
    longPressRecognizer->HandleOverdueDeadline(true);
    EXPECT_EQ(longPressRecognizer->repeat_, false);
}

/**
 * @tc.name: LongPressRecognizerThumbnailTimerTest001
 * @tc.desc: Test ThumbnailTimer in LongPressRecognizer
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerThumbnailTimerTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. set callback function.
     */
    auto callback = [](Offset offset) {};
    longPressRecognizer->callback_ = callback;
    longPressRecognizer->ThumbnailTimer(0);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::READY);
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc005
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    bool isCatchMode = false;
    longPressRecognizer->refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer->HandleOverdueDeadline(isCatchMode);
    longPressRecognizer->DoRepeat();
    GestureEventFunc click;
    GestureEvent info;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::SUCCEED);

    auto onActionStart = [](GestureEvent& info) { return true; };
    auto onActionUpdate = [](GestureEvent& info) { return true; };
    auto onActionEnd = [](GestureEvent& info) { return true; };
    longPressRecognizer->SetOnActionUpdate(onActionUpdate);
    longPressRecognizer->SetOnAction(onActionStart);
    longPressRecognizer->SetOnActionEnd(onActionEnd);
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc006
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    GestureEventFunc click;
    GestureEvent info;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::READY);

    auto onActionStart = [](GestureEvent& info) { return true; };
    auto onActionUpdate = [](GestureEvent& info) { return true; };
    auto onActionEnd = [](GestureEvent& info) { return true; };
    longPressRecognizer->SetOnActionUpdate(onActionUpdate);
    longPressRecognizer->SetOnAction(onActionStart);
    longPressRecognizer->SetOnActionEnd(onActionEnd);
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::READY);
}

/**
 * @tc.name: LongPressRecognizerHandleTouchUpEventTest001
 * @tc.desc: Test HandleTouchUpEvent in LongPressRecognizer
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleTouchUpEventTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    TouchEvent touchEvent;

    /**
     * @tc.steps: step2. set callback function.
     */
    longPressRecognizer->refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer->currentFingers_ = longPressRecognizer->fingers_ + 1;
    auto callback = [](Offset offset) {};
    longPressRecognizer->callback_ = callback;
    longPressRecognizer->HandleTouchUpEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerThumbnailTimerTest002
 * @tc.desc: Test ThumbnailTimer in LongPressRecognizer
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerThumbnailTimerTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. set callback function.
     */
    auto callback = [](Offset offset) {};
    longPressRecognizer->callback_ = callback;
    longPressRecognizer->ThumbnailTimer(0);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::READY);
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc008
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    bool isCatchMode = false;
    longPressRecognizer->refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer->HandleOverdueDeadline(isCatchMode);
    longPressRecognizer->DoRepeat();
    GestureEventFunc click;
    GestureEvent info;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::SUCCEED);

    auto onActionStart = [](GestureEvent& info) { return true; };
    auto onActionUpdate = [](GestureEvent& info) { return true; };
    auto onActionEnd = [](GestureEvent& info) { return true; };
    longPressRecognizer->SetOnActionUpdate(onActionUpdate);
    longPressRecognizer->SetOnAction(onActionStart);
    longPressRecognizer->SetOnActionEnd(onActionEnd);
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerTestGetLongPressActionFunc009
 * @tc.desc: Test LongPressRecognizer function: GetLongPressActionFunc
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerTestGetLongPressActionFunc009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    GestureEventFunc click;
    GestureEvent info;

    /**
     * @tc.steps: step2. call GetLongPressActionFunc function and compare result.
     * @tc.steps: case1: normal case
     * @tc.expected: step2. result equals.
     */
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::READY);

    auto onActionStart = [](GestureEvent& info) { return true; };
    auto onActionUpdate = [](GestureEvent& info) { return true; };
    auto onActionEnd = [](GestureEvent& info) { return true; };
    longPressRecognizer->SetOnActionUpdate(onActionUpdate);
    longPressRecognizer->SetOnAction(onActionStart);
    longPressRecognizer->SetOnActionEnd(onActionEnd);
    click = longPressRecognizer->GetLongPressActionFunc();
    click(info);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::READY);
}

/**
 * @tc.name: LongPressRecognizerHandleTouchUpEventTest002
 * @tc.desc: Test HandleTouchUpEvent in LongPressRecognizer
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleTouchUpEventTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create LongPressRecognizer.
     */
    RefPtr<LongPressRecognizer> longPressRecognizer =AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    TouchEvent touchEvent;

    /**
     * @tc.steps: step2. set callback function.
     */
    longPressRecognizer->refereeState_ = RefereeState::SUCCEED;
    longPressRecognizer->currentFingers_ = longPressRecognizer->fingers_ + 1;
    auto callback = [](Offset offset) {};
    longPressRecognizer->callback_ = callback;
    longPressRecognizer->HandleTouchUpEvent(touchEvent);
    EXPECT_EQ(longPressRecognizer->refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: GestureAccessibilityEventTest002
 * @tc.desc: Test SetOnAccessibility in LongPressRecognizer
 */
HWTEST_F(LongPressRecognizerTestNg, GestureAccessibilityEventTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create longPressRecognizer.
     */
    LongPressRecognizer longPressRecognizer = LongPressRecognizer(LONG_PRESS_DURATION, FINGER_NUMBER, false);

    /**
     * @tc.steps: step2. set callback function.
     */
    auto onAccessibilityEvent = [](AccessibilityEventType eventType) {};
    longPressRecognizer.SetOnAccessibility(onAccessibilityEvent);
    ASSERT_NE(longPressRecognizer.onAccessibilityEventFunc_, nullptr);

    /**
     * @tc.steps: step3. call callback function.
     * @tc.expected: refereeState_ is SUCCEED.
     */
    longPressRecognizer.OnAccepted();
    EXPECT_EQ(longPressRecognizer.refereeState_, RefereeState::SUCCEED);
}

/**
 * @tc.name: LongPressRecognizerHandleOverdueDeadlineTest002
 * @tc.desc: Test LongPressRecognizer function: HandleOverdueDeadline
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerHandleOverdueDeadlineTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create Recognizer、TargetComponent.
     */
    RefPtr<LongPressRecognizer> longPressRecognizerPtr = AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    RefPtr<NG::TargetComponent> targetComponent = AceType::MakeRefPtr<TargetComponent>();
    auto gestureJudgeFunc = [](const RefPtr<GestureInfo>& gestureInfo, const std::shared_ptr<BaseGestureEvent>& info) {
        return GestureJudgeResult::REJECT;};
    auto frameNode = FrameNode::CreateFrameNode("myButton", 100, AceType::MakeRefPtr<Pattern>());
    auto guestureEventHub = frameNode->GetOrCreateGestureEventHub();
    PanDirection panDirection;
    targetComponent->SetOnGestureJudgeBegin(gestureJudgeFunc);
    longPressRecognizerPtr->targetComponent_ = targetComponent;
    longPressRecognizerPtr->targetComponent_->node_ = frameNode;
    TouchEvent touchEvent;
    touchEvent.tiltX.emplace(1.0f);
    touchEvent.tiltY.emplace(1.0f);
    longPressRecognizerPtr->touchPoints_[touchEvent.id] = touchEvent;
    /**
     * @tc.steps: step2. call HandleOverdueDeadline function and compare result.
     * @tc.steps: case1: gestureInfo_ is nullptr
     * @tc.expected: step2. result equals REJECT.
     */
    longPressRecognizerPtr->refereeState_ = RefereeState::DETECTING;
    longPressRecognizerPtr->gestureInfo_ = AceType::MakeRefPtr<GestureInfo>();
    longPressRecognizerPtr->HandleOverdueDeadline(true);
    EXPECT_EQ(longPressRecognizerPtr->disposal_, GestureDisposal::REJECT);

    /**
     * @tc.steps: step2. call HandleOverdueDeadline function and compare result.
     * @tc.steps: case2: gestureInfo_ is not nullptr, gestureInfo_->type_ = DRAG
     *                   isDragUserReject_ = true
     * @tc.expected: step2. result equals REJECT.
     */
    longPressRecognizerPtr->refereeState_ = RefereeState::DETECTING;
    longPressRecognizerPtr->gestureInfo_ = AceType::MakeRefPtr<GestureInfo>();
    longPressRecognizerPtr->gestureInfo_->type_ = GestureTypeName::DRAG;
    guestureEventHub->dragEventActuator_ = AceType::MakeRefPtr<DragEventActuator>(
        AceType::WeakClaim(AceType::RawPtr(guestureEventHub)), panDirection, 1, 50.0f);
    guestureEventHub->dragEventActuator_->isDragUserReject_ = true;
    longPressRecognizerPtr->HandleOverdueDeadline(true);
    EXPECT_EQ(longPressRecognizerPtr->disposal_, GestureDisposal::REJECT);

    /**
     * @tc.steps: step2. call HandleOverdueDeadline function and compare result.
     * @tc.steps: case3: gestureInfo_ is not nullptr, gestureInfo_->type_ = DRAG
     *                   isDragUserReject_ = false
     * @tc.expected: step2. isDragUserReject_ = true.
     */
    longPressRecognizerPtr->refereeState_ = RefereeState::DETECTING;
    guestureEventHub->dragEventActuator_->isDragUserReject_ = false;
    longPressRecognizerPtr->HandleOverdueDeadline(true);
    EXPECT_TRUE(guestureEventHub->dragEventActuator_->isDragUserReject_);
}

/**
 * @tc.name: LongPressRecognizerLongPressRecognizerTest
 * @tc.desc: Test LongPressRecognizer function: LongPressRecognizer
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, LongPressRecognizerLongPressRecognizerTest, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create Recognizer、TargetComponent.
     */
    LongPressRecognizer longPressRecognizer1 = LongPressRecognizer(100, 9, false, false, false);
    LongPressRecognizer longPressRecognizer2 = LongPressRecognizer(100, 11, false, false, false);
    LongPressRecognizer longPressRecognizer3 = LongPressRecognizer(0, 10, false, false, false);
    EXPECT_EQ(longPressRecognizer1.fingers_, 9);
    EXPECT_EQ(longPressRecognizer2.fingers_, 1);
    EXPECT_EQ(longPressRecognizer3.duration_, 500);
}

/**
 * @tc.name: DeadlineTimerTest
 * @tc.desc: Test LongPressRecognizer function: DeadlineTimer
 * @tc.type: FUNC
 */
HWTEST_F(LongPressRecognizerTestNg, DeadlineTimerTest, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create Recognizer、TargetComponent.
     */
    RefPtr<LongPressRecognizer> longPressRecognizerPtr = AceType::MakeRefPtr<LongPressRecognizer>(LONG_PRESS_DURATION,
        FINGER_NUMBER, false);
    longPressRecognizerPtr->DeadlineTimer(1, true);
    EXPECT_NE(longPressRecognizerPtr, nullptr);
}
} // namespace OHOS::Ace::NG
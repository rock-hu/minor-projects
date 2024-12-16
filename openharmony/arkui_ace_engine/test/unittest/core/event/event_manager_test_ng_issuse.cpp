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
#include "gtest/gtest.h"
#include "test/unittest/core/event/event_manager_test_ng.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/common/mock_window.h"
#include "test/mock/core/common/mock_frontend.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
/**
 * @tc.name: MouseEventTest001
 * @tc.desc: Test DispatchMouseEventNG
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].push_back(mouseEventTarget);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);

    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(false);
    };
    mouseEventTarget->SetCallback(onMouse);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent event;
    event.action = MouseAction::PRESS;
    bool retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::RELEASE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::MOVE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_ENTER;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_LEAVE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_LEAVE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest002
 * @tc.desc: Test DispatchMouseEventNG
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].push_back(mouseEventTarget);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);

    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(true);
    };
    mouseEventTarget->SetCallback(onMouse);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is TRUE.
     */
    MouseEvent event;
    event.action = MouseAction::PRESS;
    bool retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(retFlag);

    event.action = MouseAction::RELEASE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(retFlag);

    event.action = MouseAction::MOVE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(retFlag);

    event.action = MouseAction::WINDOW_ENTER;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(retFlag);

    event.action = MouseAction::WINDOW_LEAVE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(retFlag);

    event.action = MouseAction::WINDOW_LEAVE;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(retFlag);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest003
 * @tc.desc: Test DispatchMouseEventNG
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].push_back(mouseEventTarget);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);

    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(false);
    };
    mouseEventTarget->SetCallback(onMouse);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent event;
    event.action = MouseAction::PRESS;
    event.button = MouseButton::NONE_BUTTON;
    bool retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PRESS;
    event.button = MouseButton::NONE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::RELEASE;
    event.button = MouseButton::NONE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::MOVE;
    event.button = MouseButton::NONE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_ENTER;
    event.button = MouseButton::NONE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_LEAVE;
    event.button = MouseButton::NONE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_LEAVE;
    event.button = MouseButton::NONE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest004
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].push_back(mouseEventTarget);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);

    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(true);
    };
    mouseEventTarget->SetCallback(onMouse);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is false.
     */
    MouseEvent event;
    event.action = MouseAction::HOVER;
    event.button = MouseButton::LEFT_BUTTON;
    bool retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::HOVER_ENTER;
    event.button = MouseButton::RIGHT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::HOVER_MOVE;
    event.button = MouseButton::MIDDLE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::HOVER_EXIT;
    event.button = MouseButton::BACK_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PULL_DOWN;
    event.button = MouseButton::FORWARD_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PULL_MOVE;
    event.button = MouseButton::SIDE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PULL_UP;
    event.button = MouseButton::EXTRA_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::CANCEL;
    event.button = MouseButton::EXTRA_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest005
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest005, TestSize.Level1)
{
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(true);
    };
    mouseEventTarget->SetCallback(onMouse);
    eventManager->currMouseTestResults_.emplace_back(mouseEventTarget);
    auto mouseEventTarget2 = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID_2);
    const OnMouseEventFunc onMouse2 = [](MouseInfo& info) {
        info.SetStopPropagation(false);
    };
    mouseEventTarget2->SetCallback(onMouse2);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].emplace_back(mouseEventTarget2);

    MouseEvent event;
    event.action = MouseAction::PULL_DOWN;
    event.button = MouseButton::FORWARD_BUTTON;
    bool retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    event.action = MouseAction::PRESS;
    event.button = MouseButton::LEFT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(retFlag);
    event.action = MouseAction::HOVER;
    event.button = MouseButton::LEFT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    event.action = MouseAction::HOVER_ENTER;
    event.button = MouseButton::RIGHT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    event.action = MouseAction::HOVER_MOVE;
    event.button = MouseButton::MIDDLE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    event.action = MouseAction::HOVER_EXIT;
    event.button = MouseButton::BACK_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    event.action = MouseAction::PULL_UP;
    event.button = MouseButton::EXTRA_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest006
 * @tc.desc: Test DispatchMouseEventNG function.
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].push_back(mouseEventTarget);
    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(true);
    };
    mouseEventTarget->SetCallback(onMouse);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent event;
    event.action = MouseAction::MOVE;
    event.button = MouseButton::LEFT_BUTTON;
    bool result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);

    event.action = MouseAction::PRESS;
    event.button = MouseButton::LEFT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest007
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest007, TestSize.Level1)
{
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(true);
    };
    mouseEventTarget->SetCallback(onMouse);
    eventManager->currMouseTestResults_.emplace_back(mouseEventTarget);
    auto mouseEventTarget2 = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID_2);
    const OnMouseEventFunc onMouse2 = [](MouseInfo& info) {
        info.SetStopPropagation(false);
    };
    mouseEventTarget2->SetCallback(onMouse2);
    eventManager->pressMouseTestResultsMap_[MouseButton::RIGHT_BUTTON].emplace_back(mouseEventTarget2);

    MouseEvent event;
    event.action = MouseAction::RELEASE;
    event.button = MouseButton::MIDDLE_BUTTON;
    bool result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::RELEASE;
    event.button = MouseButton::BACK_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::MOVE;
    event.button = MouseButton::FORWARD_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::WINDOW_ENTER;
    event.button = MouseButton::SIDE_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::WINDOW_LEAVE;
    event.button = MouseButton::EXTRA_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::PRESS;
    event.button = MouseButton::LEFT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::RELEASE;
    event.button = MouseButton::LEFT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest008
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest008, TestSize.Level1)
{
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(true);
    };
    mouseEventTarget->SetCallback(onMouse);
    eventManager->currMouseTestResults_.emplace_back(mouseEventTarget);
    auto mouseEventTarget2 = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID_2);
    const OnMouseEventFunc onMouse2 = [](MouseInfo& info) {
        info.SetStopPropagation(false);
    };
    mouseEventTarget2->SetCallback(onMouse2);
    eventManager->pressMouseTestResultsMap_[MouseButton::MIDDLE_BUTTON].emplace_back(mouseEventTarget2);

    MouseEvent event;
    event.action = MouseAction::MOVE;
    event.button = MouseButton::BACK_BUTTON;
    bool result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::WINDOW_ENTER;
    event.button = MouseButton::SIDE_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::PRESS;
    event.button = MouseButton::LEFT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::RELEASE;
    event.button = MouseButton::LEFT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);
    event.action = MouseAction::RELEASE;
    event.button = MouseButton::RIGHT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::RELEASE;
    event.button = MouseButton::MIDDLE_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    event.action = MouseAction::RELEASE;
    event.button = MouseButton::BACK_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_TRUE(result);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest009
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(false);
    };
    mouseEventTarget->SetCallback(onMouse);
    eventManager->currMouseTestResults_.emplace_back(mouseEventTarget);

    auto mouseEventTarget2 = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID_2);
    const OnMouseEventFunc onMouse2 = [](MouseInfo& info) {
        info.SetStopPropagation(true);
    };
    mouseEventTarget2->SetCallback(onMouse2);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].emplace_back(mouseEventTarget2);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent event;
    event.action = MouseAction::PRESS;
    event.button = MouseButton::LEFT_BUTTON;

    bool result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);

    event.action = MouseAction::RELEASE;
    event.button = MouseButton::LEFT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);

    event.action = MouseAction::RELEASE;
    event.button = MouseButton::RIGHT_BUTTON;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest010
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->pressMouseTestResultsMap_[MouseButton::LEFT_BUTTON].push_back(mouseEventTarget);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);

    const OnMouseEventFunc onMousePress = [](MouseInfo& info) -> bool {
        info.SetStopPropagation(false);
        return false;
    };
    const OnMouseEventFunc onMouseRelease = [](MouseInfo& info) -> bool {
        return false;
    };
    mouseEventTarget->SetCallback(onMousePress);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent pressEvent;
    pressEvent.action = MouseAction::PRESS;
    pressEvent.button = MouseButton::LEFT_BUTTON;
    bool result = eventManager->DispatchMouseEventNG(pressEvent);
    EXPECT_FALSE(result);

    MouseEvent releaseEvent;
    releaseEvent.action = MouseAction::RELEASE;
    releaseEvent.button = MouseButton::LEFT_BUTTON;
    result = eventManager->DispatchMouseEventNG(releaseEvent);
    EXPECT_FALSE(result);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest011
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest011, TestSize.Level1) {
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
        info.SetStopPropagation(false);
    };
    mouseEventTarget->SetCallback(onMouse);
    eventManager->currMouseTestResults_.emplace_back(mouseEventTarget);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent event;
    event.action = MouseAction::PRESS;
    event.button = MouseButton::RIGHT_BUTTON;

    bool result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);

    event.action = MouseAction::RELEASE;
    result = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(result);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest012
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest012, TestSize.Level1) {
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    const OnMouseEventFunc onMouse = [](MouseInfo& info) {
    };
    mouseEventTarget->SetCallback(onMouse);
    eventManager->currMouseTestResults_.emplace_back(mouseEventTarget);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent event;
    event.action = MouseAction::MOVE;
    event.button = MouseButton::NONE_BUTTON;
    bool retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PRESS;
    event.button = MouseButton::NONE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PRESS;
    event.button = MouseButton::LEFT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_ENTER;
    event.button = MouseButton::RIGHT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::MOVE;
    event.button = MouseButton::MIDDLE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: MouseEventTest013
 * @tc.desc: Test DispatchMouseEventNG function
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseEventTest013, TestSize.Level1) {
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_THIRTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: retFlag is FALSE.
     */
    MouseEvent event;
    event.action = MouseAction::NONE;
    event.button = MouseButton::NONE_BUTTON;
    bool retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PRESS;
    event.button = MouseButton::LEFT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::RELEASE;
    event.button = MouseButton::RIGHT_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::MOVE;
    event.button = MouseButton::MIDDLE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_ENTER;
    event.button = MouseButton::BACK_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::WINDOW_LEAVE;
    event.button = MouseButton::FORWARD_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::PRESS;
    event.button = MouseButton::SIDE_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::RELEASE;
    event.button = MouseButton::EXTRA_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);

    event.action = MouseAction::MOVE;
    event.button = MouseButton::TASK_BUTTON;
    retFlag = eventManager->DispatchMouseEventNG(event);
    EXPECT_FALSE(retFlag);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}

/**
 * @tc.name: DispatchAxisEventIssueTest1
 * @tc.desc: Test DispatchAxisEventNG, axisTestResults_ is deleted each time it is dispatched.
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, DispatchAxisEventIssueTest1, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager.
     * @tc.expected: eventManager is not null.
     */
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    /**
     * @tc.steps: step4. Call DispatchAxisEventNG with axisTestResults_ not empty
     * @tc.expected: eventManager->axisTestResults_ is empty
     */
    auto axisEventTarget = AceType::MakeRefPtr<AxisEventTarget>();
    auto onAxisCallback = [](AxisInfo&) -> void {};
    axisEventTarget->SetOnAxisCallback(onAxisCallback);
    AxisEvent event;
    event.horizontalAxis = 0;
    event.verticalAxis = 0;
    event.pinchAxisScale = 0;
    eventManager->axisTestResults_.push_back(axisEventTarget);
    eventManager->DispatchAxisEventNG(event);
    EXPECT_TRUE(eventManager->axisTestResults_.empty());
}

/**
 * @tc.name: MouseLocationTest001
 * @tc.desc: Test HandleMouseEvent in less than API 14, the x、y location use coordinateOffset_.
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseLocationTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create EventManager and set mouse event target callback.
     * @tc.expected: eventManager is not null.
     */
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);
    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);
    mouseEventTarget->coordinateOffset_ = Offset(-100, 0);
    MouseInfo mouseInfo;
    const OnMouseEventFunc onMouse = [&mouseInfo](MouseInfo& info) {
        mouseInfo = info;
    };
    mouseEventTarget->SetCallback(onMouse);

    /**
     * @tc.steps: step2. Call DispatchMouseEventNG.
     * @tc.expected: the mouse local location value is right.
     */
    MouseEvent event;
    event.x = 200.0f;
    event.y = 200.0f;
    event.action = MouseAction::PRESS;
    eventManager->DispatchMouseEventNG(event);
    EXPECT_EQ(mouseInfo.GetLocalLocation().GetX(), 300.0f);
    EXPECT_EQ(mouseInfo.GetLocalLocation().GetY(), 200.0f);
}

/**
 * @tc.name: MouseLocationTest002
 * @tc.desc: Test HandleMouseEvent in great or equal API 14, the x、y location use Transform.
 * @tc.type: FUNC
 */
HWTEST_F(EventManagerTestNg, MouseLocationTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create FrameNode.
     */
    RefPtr<FrameNode> FRAME_NODE_0 = FrameNode::CreateFrameNode("0", 0, AceType::MakeRefPtr<Pattern>());
    RefPtr<FrameNode> FRAME_NODE_1 = FrameNode::CreateFrameNode("1", 1, AceType::MakeRefPtr<Pattern>());
    RefPtr<FrameNode> FRAME_NODE_2 = FrameNode::CreateFrameNode("2", 2, AceType::MakeRefPtr<Pattern>());
    FRAME_NODE_2->SetParent(WeakPtr<FrameNode>(FRAME_NODE_1));
    FRAME_NODE_1->SetParent(WeakPtr<FrameNode>(FRAME_NODE_0));

    /**
     * @tc.steps: step2. mock local matrix.
     */
    FRAME_NODE_0->localMat_ = Matrix4::CreateIdentity();
    FRAME_NODE_1->localMat_ = Matrix4::CreateIdentity();
    FRAME_NODE_2->localMat_ = Matrix4::Invert(Matrix4::CreateTranslate(400, 0, 0));

    /**
     * @tc.steps: step3. Create EventManager, and set mouse event target callback.
     * @tc.expected: eventManager is not null.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_FOURTEEN));
    auto eventManager = AceType::MakeRefPtr<EventManager>();
    ASSERT_NE(eventManager, nullptr);

    auto mouseEventTarget = AceType::MakeRefPtr<MouseEventTarget>(MOUSE, NODEID);
    eventManager->currMouseTestResults_.push_back(mouseEventTarget);
    mouseEventTarget->coordinateOffset_ = Offset(-100, 0);
    mouseEventTarget->AttachFrameNode(FRAME_NODE_2);
    MouseInfo mouseInfo;
    const OnMouseEventFunc onMouse = [&mouseInfo](MouseInfo& info) {
        mouseInfo = info;
    };
    mouseEventTarget->SetCallback(onMouse);

    /**
     * @tc.steps: step4. Call DispatchMouseEventNG.
     * @tc.expected: the mouse local location value is right.
     */
    MouseEvent event;
    event.x = 200.0f;
    event.y = 200.0f;
    event.action = MouseAction::PRESS;
    eventManager->DispatchMouseEventNG(event);
    EXPECT_EQ(mouseInfo.GetLocalLocation().GetX(), -200.0f);
    EXPECT_EQ(mouseInfo.GetLocalLocation().GetY(), 200.0f);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
}
}
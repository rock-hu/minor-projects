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
#include "base/memory/ace_type.h"
#define private public
#define protected public
#include "frameworks/core/components_ng/pattern/ui_extension/platform_pattern.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "frameworks/base/log/log_wrapper.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_layout_algorithm.h"
#include "core/components_ng/pattern/window_scene/scene/window_pattern.h"
#include "core/components_ng/render/adapter/rosen_render_context.h"
#include "core/components_ng/render/adapter/rosen_window.h"
#include "adapter/ohos/entrance/ace_extra_input_data.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
    const std::string PLATFORM_PATTERN_ETS_TAG = "PlatformPattern";
    const int32_t CODE = 10001;
    const std::string NAME = "Test name";
    const std::string MSG = "Test msg";
    const std::string TAG = "Test node tag";
    int32_t platformId = 1;
} // namespace

class PlatformPatternTestNg : public testing::Test {
public:
    static void SetUpTestSuite()
    {
        NG::MockPipelineContext::SetUp();
        MockContainer::SetUp();
        MockContainer::Current()->pipelineContext_ = PipelineBase::GetCurrentContext();
    }
    static void TearDownTestSuite()
    {
        MockContainer::Current()->pipelineContext_ = nullptr;
        NG::MockPipelineContext::TearDown();
    }
    void SetUp() override;
    void TearDown() override;
};

void PlatformPatternTestNg::SetUp() {}

void PlatformPatternTestNg::TearDown() {}

/**
 * @tc.name: PlatformPatternTest001
 * @tc.desc: Test PlatformPattern CreateLayoutAlgorithm/GetFocusPattern/OnModifyDone
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, PlatformPatternTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto platformNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto platformNode = FrameNode::GetOrCreateFrameNode(
        PLATFORM_PATTERN_ETS_TAG, platformNodeId, []() {
            return AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
        });
    ASSERT_NE(platformNode, nullptr);
    EXPECT_EQ(platformNode->GetTag(), PLATFORM_PATTERN_ETS_TAG);
    
    /**
     * @tc.steps: step2. get PlatformPattern
     */
    auto platformPattern = platformNode->GetPattern<PlatformPattern>();
    ASSERT_NE(platformPattern, nullptr);

    /**
     * @tc.steps: step3. call CreateLayoutAlgorithm.
     * @tc.expected: expect result is nullptr
     */
    auto layoutAlgorithm = platformPattern->CreateLayoutAlgorithm();
    ASSERT_NE(layoutAlgorithm, nullptr);

    /**
     * @tc.steps: step4. call GetFocusPattern.
     * @tc.expected: expect focusType is NODE.
     * @tc.expected: expect focusable is true.
     * @tc.expected: expect styleType is FORCE_NONE.
     */
    auto focusPattern = platformPattern->GetFocusPattern();
    ASSERT_EQ(focusPattern.focusType_, FocusType::NODE);
    ASSERT_TRUE(focusPattern.focusable_);
    ASSERT_EQ(focusPattern.styleType_, FocusStyleType::FORCE_NONE);

    /**
     * @tc.steps: step5. call OnModifyDone
     */
    platformPattern->OnModifyDone();
}

/**
 * @tc.name: PlatformPatternTest002
 * @tc.desc: Test PlatformPattern OnMountToParentDone
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, PlatformPatternTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto platformNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto platformNode = FrameNode::GetOrCreateFrameNode(
        PLATFORM_PATTERN_ETS_TAG, platformNodeId, []() {
            return AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
        });
    ASSERT_NE(platformNode, nullptr);
    EXPECT_EQ(platformNode->GetTag(), PLATFORM_PATTERN_ETS_TAG);
    
    /**
     * @tc.steps: step2. get PlatformPattern
     */
    auto platformPattern = platformNode->GetPattern<PlatformPattern>();
    ASSERT_NE(platformPattern, nullptr);

    /**
     * @tc.steps: step3. call OnMountToParentDone.
     */
    auto pattern = AceType::MakeRefPtr<Pattern>();
    auto attachFrameNode = FrameNode::CreateFrameNode(TAG, 1, pattern);
    ASSERT_NE(attachFrameNode, nullptr);
    platformPattern->AttachToFrameNode(attachFrameNode);
    ASSERT_NE(platformPattern->frameNode_.Upgrade(), nullptr);
    attachFrameNode->UpdateNodeStatus(NodeStatus::NORMAL_NODE);
    platformPattern->OnMountToParentDone();
}

/**
 * @tc.name: PlatformPatternTest003
 * @tc.desc: Test PlatformPattern HandleDragEvent
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, PlatformPatternTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto platformNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto platformNode = FrameNode::GetOrCreateFrameNode(
        PLATFORM_PATTERN_ETS_TAG, platformNodeId, []() {
            return AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
        });
    ASSERT_NE(platformNode, nullptr);
    EXPECT_EQ(platformNode->GetTag(), PLATFORM_PATTERN_ETS_TAG);
    
    /**
     * @tc.steps: step2. get PlatformPattern
     */
    auto platformPattern = platformNode->GetPattern<PlatformPattern>();
    ASSERT_NE(platformPattern, nullptr);

    /**
     * @tc.steps: step3. call InitializeDynamicComponent.
     * @tc.expected: expect dynamicComponentRenderer_ is not null.
     */
    OHOS::Ace::DragPointerEvent event;
    platformPattern->HandleDragEvent(event);
}

/**
 * @tc.name: PlatformPatternTest004
 * @tc.desc: Test PlatformPattern SetOnErrorCallback/FireOnErrorCallback
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, PlatformPatternTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto platformNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto platformNode = FrameNode::GetOrCreateFrameNode(
        PLATFORM_PATTERN_ETS_TAG, platformNodeId, []() {
            return AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
        });
    ASSERT_NE(platformNode, nullptr);
    EXPECT_EQ(platformNode->GetTag(), PLATFORM_PATTERN_ETS_TAG);
    
    /**
     * @tc.steps: step2. get PlatformPattern
     */
    auto platformPattern = platformNode->GetPattern<PlatformPattern>();
    ASSERT_NE(platformPattern, nullptr);

    /**
     * @tc.steps: step3. Test onError
     */
    auto onError =
        [](int32_t code, const std::string& name, const std::string& message) {
            EXPECT_EQ(code, CODE);
            EXPECT_EQ(name, NAME);
            EXPECT_EQ(message, MSG);
        };
    platformPattern->SetOnErrorCallback(std::move(onError));
    platformPattern->FireOnErrorCallback(CODE, NAME, MSG);
}

/**
 * @tc.name: PlatformPatternTest005
 * @tc.desc: Test PlatformPattern GetInstanceId/GetNodeId
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, PlatformPatternTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto platformNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto platformNode = FrameNode::GetOrCreateFrameNode(
        PLATFORM_PATTERN_ETS_TAG, platformNodeId, []() {
            return AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
        });
    ASSERT_NE(platformNode, nullptr);
    EXPECT_EQ(platformNode->GetTag(), PLATFORM_PATTERN_ETS_TAG);
    
    /**
     * @tc.steps: step2. get PlatformPattern
     */
    auto platformPattern = platformNode->GetPattern<PlatformPattern>();
    ASSERT_NE(platformPattern, nullptr);

    /**
     * @tc.steps: step3. call GetInstanceId/GetNodeId.
     */
    ASSERT_GE(platformPattern->GetNodeId(), 1);
    ASSERT_GE(platformPattern->GetInstanceId(), -1);
}

/**
 * @tc.name: PlatformPatternTest006
 * @tc.desc: Test PlatformPattern InitKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, PlatformPatternTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto platformNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto platformNode = FrameNode::GetOrCreateFrameNode(
        PLATFORM_PATTERN_ETS_TAG, platformNodeId, []() {
            return AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
        });
    ASSERT_NE(platformNode, nullptr);
    EXPECT_EQ(platformNode->GetTag(), PLATFORM_PATTERN_ETS_TAG);
    
    /**
     * @tc.steps: step2. get PlatformPattern
     */
    auto platformPattern = platformNode->GetPattern<PlatformPattern>();
    ASSERT_NE(platformPattern, nullptr);

    /**
     * @tc.steps: step3. get FocusHub
     */
    auto focusHub = platformNode->GetFocusHub();
    ASSERT_NE(focusHub, nullptr);
    EXPECT_FALSE(focusHub->isNodeNeedKey_);

    /**
     * @tc.steps: step4. call InitKeyEvent
     */
    platformPattern->InitKeyEvent(focusHub);
    EXPECT_TRUE(focusHub->isNodeNeedKey_);
}

/**
 * @tc.name: InitKeyEvent001
 * @tc.desc: Test PlatformPattern InitKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, InitKeyEvent001, TestSize.Level1)
{
    auto platformPattern = AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
    RefPtr<EventHub> eventHub = AceType::MakeRefPtr<EventHub>();
    auto focusHub = AceType::MakeRefPtr<FocusHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));

    focusHub->SetIsNodeNeedKey(true);
    platformPattern->InitKeyEvent(focusHub);
    focusHub->onFocusInternal_(focusHub->focusReason_);
    focusHub->onBlurInternal_();
    focusHub->onClearFocusStateCallback_();
    KeyEvent keyEvent;
    bool ret = focusHub->onPaintFocusStateCallback_();
    EXPECT_TRUE(ret);
    bool ret2 = focusHub->onKeyEventsInternal_[OnKeyEventType::DEFAULT].operator()(keyEvent);
    EXPECT_EQ(ret2, platformPattern->HandleKeyEvent(keyEvent));

    
    platformPattern = nullptr;
    focusHub->SetIsNodeNeedKey(true);
    focusHub->onFocusInternal_(focusHub->focusReason_);
    focusHub->onBlurInternal_();
    focusHub->onClearFocusStateCallback_();
    ret = focusHub->onPaintFocusStateCallback_();
    EXPECT_FALSE(ret);
    ret2 = focusHub->onKeyEventsInternal_[OnKeyEventType::DEFAULT].operator()(keyEvent);
    EXPECT_FALSE(ret2);
}

/**
 * @tc.name: InitTouchEvent001
 * @tc.desc: Test PlatformPattern InitTouchEvent
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, InitTouchEvent001, TestSize.Level1)
{
    auto platformPattern = AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto gestureHub = frameNode->GetOrCreateGestureEventHub();
    EXPECT_NE(gestureHub, nullptr);

    platformPattern->InitTouchEvent(gestureHub);
    TouchEventInfo touchEventInfo("touch");
    platformPattern->touchEvent_->callback_(touchEventInfo);
    EXPECT_NE(gestureHub, nullptr);

    auto touchTask = [](TouchEventInfo& info) {};
    platformPattern->touchEvent_ = AccessibilityManager::MakeRefPtr<TouchEventImpl>(std::move(touchTask));
    platformPattern->InitTouchEvent(gestureHub);
    EXPECT_NE(gestureHub, nullptr);
}

/**
 * @tc.name: InitMouseEvent001
 * @tc.desc: Test PlatformPattern InitMouseEvent
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, InitMouseEvent001, TestSize.Level1)
{
    auto platformPattern = AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
    WeakPtr<EventHub> eventHub = nullptr;
    RefPtr<InputEventHub> inputHub = AceType::MakeRefPtr<InputEventHub>(eventHub);
    EXPECT_NE(inputHub, nullptr);

    platformPattern->InitMouseEvent(inputHub);
    MouseInfo info;
    platformPattern->mouseEvent_->GetOnMouseEventFunc()(info);
    EXPECT_NE(inputHub, nullptr);

    auto mouseCallback = [](MouseInfo& info) {};
    platformPattern->mouseEvent_ = AceType::MakeRefPtr<InputEvent>(std::move(mouseCallback));
    platformPattern->InitMouseEvent(inputHub);
    EXPECT_NE(inputHub, nullptr);
}

/**
 * @tc.name: InitHoverEvent001
 * @tc.desc: Test PlatformPattern InitHoverEvent
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, InitHoverEvent001, TestSize.Level1)
{
    auto platformPattern = AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
    WeakPtr<EventHub> eventHub = nullptr;
    RefPtr<InputEventHub> inputHub = AceType::MakeRefPtr<InputEventHub>(eventHub);
    EXPECT_NE(inputHub, nullptr);

    platformPattern->InitHoverEvent(inputHub);
    bool isHover = true;
    platformPattern->hoverEvent_->GetOnHoverEventFunc()(isHover);
    EXPECT_NE(inputHub, nullptr);

    auto hoverCallback = [](const MouseInfo& info) {};
    platformPattern->hoverEvent_ = AceType::MakeRefPtr<InputEvent>(hoverCallback);
    platformPattern->InitHoverEvent(inputHub);
    EXPECT_NE(inputHub, nullptr);
}

/**
 * @tc.name: HandleTouchEvent001
 * @tc.desc: Test PlatformPattern HandleTouchEvent
 * @tc.type: FUNC
 */
HWTEST_F(PlatformPatternTestNg, HandleTouchEvent001, TestSize.Level1)
{
    auto platformPattern = AceType::MakeRefPtr<PlatformPattern>(AceLogTag::ACE_DEFAULT_DOMAIN, platformId);
    TouchEventInfo info("touch");
    info.SetSourceDevice(SourceType::NONE);
    platformPattern->HandleTouchEvent(info);

    info.SetSourceDevice(SourceType::TOUCH);
    info.pointerEvent_ = std::make_shared<MMI::PointerEvent>(1);
    
    auto pattern = AceType::MakeRefPtr<Pattern>();
    auto attachFrameNode = FrameNode::CreateFrameNode(TAG, 1, pattern);
    platformPattern->AttachToFrameNode(attachFrameNode);
    auto host = platformPattern->GetHost();
    EXPECT_NE(host, nullptr);
    auto pipeline = PipelineBase::GetCurrentContext();
    EXPECT_NE(pipeline, nullptr);

    TouchLocationInfo touchInfo1(1);
    TouchLocationInfo touchInfo2(2);
    TouchLocationInfo touchInfo3(3);
    touchInfo3.SetTouchType(TouchType::DOWN);
    info.changedTouches_.push_back(touchInfo1);
    info.changedTouches_.push_back(touchInfo2);
    info.changedTouches_.push_back(touchInfo3);
    auto changedTouches = info.GetChangedTouches();

    RefPtr<EventHub> eventHub = AceType::MakeRefPtr<EventHub>();
    host->focusHub_ =  AceType::MakeRefPtr<FocusHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    EXPECT_NE(host->focusHub_, nullptr);

    platformPattern->HandleTouchEvent(info);
    EXPECT_FALSE(changedTouches.empty());
    EXPECT_EQ(changedTouches.back().GetTouchType(), TouchType::DOWN);
}
} // namespace OHOS::Ace::NG

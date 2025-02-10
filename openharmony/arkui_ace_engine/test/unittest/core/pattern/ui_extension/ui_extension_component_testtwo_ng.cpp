/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "core/components_ng/pattern/ui_extension/isolated_component/isolated_pattern.h"
#include "core/components_ng/pattern/ui_extension/security_ui_extension_component/security_ui_extension_pattern.h"
#include "core/components_ng/pattern/ui_extension/session_wrapper.h"
#include "core/components_ng/pattern/ui_extension/session_wrapper_factory.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_component/modal_ui_extension_proxy_impl.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_component/session_wrapper_impl.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_component/ui_extension_pattern.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_component/ui_extension_proxy.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_config.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_model.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_model_ng.h"
#include "core/event/ace_events.h"
#include "core/event/mouse_event.h"
#include "core/event/touch_event.h"
#include "core/event/key_event.h"
#include "core/event/pointer_event.h"

#include "session/host/include/extension_session.h"
#include "session/host/include/session.h"
#include "ui/rs_surface_node.h"

#include "adapter/ohos/entrance/mmi_event_convertor.h"
#include "adapter/ohos/osal/want_wrap_ohos.h"
#include "base/error/error_code.h"
#include "base/geometry/offset.h"
#include "base/utils/utils.h"

#include "core/common/window.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/pattern/pattern.h"

#include "mock/mock_accessibility_child_tree_callback.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/common/mock_container.h"

#include "core/components_ng/render/adapter/rosen_window.h"
#include "test/mock/base/mock_task_executor.h"
#include "test/mock/core/render/mock_rosen_render_context.h"
#include "frameworks/core/components_ng/pattern/ui_extension/platform_event_proxy.h"
#include "test/unittest/core/pattern/ui_extension/mock/mock_window_scene_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
    const std::string UI_EXTENSION_COMPONENT_ETS_TAG = "UIExtensionComponent";
    const int32_t IGNORE_POSITION_TRANSITION_SWITCH = -990;
} // namespace

class UIExtensionComponentTestTwoNg : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    static void SetUpTestSuite()
    {
        MockPipelineContext::SetUp();
        MockContainer::SetUp();
        MockContainer::Current()->pipelineContext_ = MockPipelineContext::GetCurrentContext();
        MockContainer::Current()->pipelineContext_->taskExecutor_ = MockContainer::Current()->taskExecutor_;
    }
    static void TearDownTestSuite()
    {
        MockPipelineContext::TearDown();
        MockContainer::TearDown();
    }
};

void UIExtensionComponentTestTwoNg::SetUp() {
}

void UIExtensionComponentTestTwoNg::TearDown() {
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern OnDisconnect and GetInstanceId
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, OnAccessibilityEventTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test OnDisconnect
     */
    EXPECT_EQ(pattern->curPlaceholderType_, PlaceholderType::NONE);
    bool isAbnormal = true;
    pattern->OnDisconnect(isAbnormal);
    EXPECT_EQ(pattern->curPlaceholderType_, PlaceholderType::NONE);
    isAbnormal = false;
    pattern->OnDisconnect(isAbnormal);
    EXPECT_EQ(pattern->curPlaceholderType_, PlaceholderType::NONE);

    /**
     * @tc.steps: step3. test GetInstanceId
     */
    pattern->instanceId_ = 0;
    EXPECT_EQ(pattern->GetInstanceId(), pattern->instanceId_);
    pattern->instanceId_ = 1;
    EXPECT_EQ(pattern->GetInstanceId(), pattern->instanceId_);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern SetEventProxyFlag
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, SetEventProxyFlagTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test SetEventProxyFlag
     */
    // False - False
    int32_t flag = 0X00000000;
    EXPECT_EQ(flag, EventProxyFlag::EVENT_NONE);
    pattern->platformEventProxy_ = nullptr;
    ASSERT_EQ(pattern->platformEventProxy_, nullptr);
    pattern->SetEventProxyFlag(flag);

    // False - True
    pattern->platformEventProxy_ = AceType::MakeRefPtr<PlatformEventProxy>();
    ASSERT_NE(pattern->platformEventProxy_, nullptr);
    pattern->SetEventProxyFlag(flag);

    // True - True
    flag = 0X00000100;
    ASSERT_NE(flag, EventProxyFlag::EVENT_NONE);
    pattern->SetEventProxyFlag(flag);

    // True - False
    pattern->platformEventProxy_ = AceType::MakeRefPtr<PlatformEventProxy>();
    ASSERT_NE(pattern->platformEventProxy_, nullptr);
    pattern->SetEventProxyFlag(flag);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern HandleDragEvent
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, HandleDragEventTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test HandleDragEvent
     */
    DragPointerEvent info(100, 100, 100, 100);;
    ASSERT_EQ(pattern->platformEventProxy_, nullptr);
    pattern->HandleDragEvent(info);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(1);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    info.rawPointerEvent = pointerEvent;
    ASSERT_NE(info.rawPointerEvent, nullptr);
    pattern->HandleDragEvent(info);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern OnMountToParentDone and AfterMountToParent
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, OnMountToParentDoneTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test OnMountToParentDone
     */
    pattern->needReNotifyForeground_= false;
    pattern->OnMountToParentDone();

    pattern->needReNotifyForeground_= true;
    pattern->frameNode_ = nullptr;
    ASSERT_EQ(pattern->frameNode_.Upgrade(), nullptr);
    ASSERT_EQ(pattern->GetHost(), nullptr);
    ASSERT_EQ(WindowSceneHelper::FindWindowScene(pattern->GetHost()), nullptr);
    pattern->OnMountToParentDone();

    pattern->frameNode_ = uiExtensionNode;
    ASSERT_NE(pattern->frameNode_.Upgrade(), nullptr);
    ASSERT_NE(pattern->GetHost(), nullptr);
    ASSERT_NE(WindowSceneHelper::FindWindowScene(pattern->GetHost()), nullptr);
    pattern->OnMountToParentDone();

    /**
     * @tc.steps: step2. test AfterMountToParent
     */
    pattern->needReNotifyForeground_= false;
    pattern->AfterMountToParent();

    pattern->needReNotifyForeground_= true;
    pattern->frameNode_ = nullptr;
    ASSERT_EQ(pattern->frameNode_.Upgrade(), nullptr);
    ASSERT_EQ(pattern->GetHost(), nullptr);
    ASSERT_EQ(WindowSceneHelper::FindWindowScene(pattern->GetHost()), nullptr);
    pattern->AfterMountToParent();

    pattern->frameNode_ = uiExtensionNode;
    ASSERT_NE(pattern->frameNode_.Upgrade(), nullptr);
    ASSERT_NE(pattern->GetHost(), nullptr);
    ASSERT_NE(WindowSceneHelper::FindWindowScene(pattern->GetHost()), nullptr);
    pattern->AfterMountToParent();
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern RegisterVisibleAreaChange
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, RegisterVisibleAreaChangeTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test RegisterVisibleAreaChange
     */
    pattern->frameNode_ = nullptr;
    ASSERT_EQ(pattern->frameNode_.Upgrade(), nullptr);
    ASSERT_EQ(pattern->GetHost(), nullptr);
    pattern->RegisterVisibleAreaChange();

    pattern->frameNode_ = uiExtensionNode;
    ASSERT_NE(pattern->frameNode_.Upgrade(), nullptr);
    ASSERT_NE(pattern->GetHost(), nullptr);
    pattern->RegisterVisibleAreaChange();
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern RegisterEventProxyFlagCallback
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, RegisterEventProxyFlagCallbackTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test RegisterEventProxyFlagCallback
     */
    EXPECT_EQ(pattern->businessDataUECConsumeCallbacks_.size(), 0);
    pattern->RegisterEventProxyFlagCallback();
    EXPECT_EQ(pattern->businessDataUECConsumeCallbacks_.size(), 1);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern RegisterEventProxyFlagCallback
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, RegisterEventProxyFlagCallbackTest002, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test RegisterEventProxyFlagCallback
     */
    pattern->RegisterEventProxyFlagCallback();
    AAFwk::Want want;
    auto ret = pattern->businessDataUECConsumeCallbacks_.at(UIContentBusinessCode::EVENT_PROXY)(want);
    EXPECT_EQ(ret, -1);

    std::string type = "OccupyEvents";
    want.SetParam("type", type);
    ret = pattern->businessDataUECConsumeCallbacks_.at(UIContentBusinessCode::EVENT_PROXY)(want);
    EXPECT_EQ(ret, 0);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern RegisterReplyPageModeCallback
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, RegisterReplyPageModeCallbackTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test RegisterReplyPageModeCallback
     */
    EXPECT_EQ(pattern->businessDataUECConsumeReplyCallbacks_.size(), 0);
    pattern->RegisterReplyPageModeCallback();
    EXPECT_EQ(pattern->businessDataUECConsumeReplyCallbacks_.size(), 1);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern RegisterReplyPageModeCallback
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, RegisterReplyPageModeCallbackTest002, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test RegisterReplyPageModeCallback
     */
    pattern->RegisterReplyPageModeCallback();
    AAFwk::Want want;
    std::optional<AAFwk::Want> op = std::nullopt;
    auto ret = pattern->businessDataUECConsumeReplyCallbacks_.at(UIContentBusinessCode::SEND_PAGE_MODE)(
        want, op);
    EXPECT_EQ(ret, -1);

    std::string value = "testuec";
    AAFwk::Want want2;
    want2.SetParam("requestPageMode", value);
    op = want2;
    ret = pattern->businessDataUECConsumeReplyCallbacks_.at(UIContentBusinessCode::SEND_PAGE_MODE)(
        want, op);
    EXPECT_EQ(ret, -1);

    want.SetParam("requestPageMode", value);
    ret = pattern->businessDataUECConsumeReplyCallbacks_.at(UIContentBusinessCode::SEND_PAGE_MODE)(
        want, op);
    EXPECT_EQ(ret, 0);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern SendBusinessDataSyncReply
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, SendBusinessDataSyncReplyTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);
    ASSERT_NE(pattern->sessionWrapper_, nullptr);

    /**
     * @tc.steps: step2. test SendBusinessDataSyncReply
     */
    UIContentBusinessCode code = UIContentBusinessCode::SEND_PAGE_MODE;
    AAFwk::Want want;
    AAFwk::Want reply;
    auto ret = pattern->SendBusinessDataSyncReply(code, std::move(want), reply);
    ASSERT_EQ(ret, false);
    
    pattern->sessionWrapper_ = nullptr;
    ret = pattern->SendBusinessDataSyncReply(code, std::move(want), reply);
    ASSERT_EQ(ret, false);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern SendBusinessData
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, SendBusinessDataTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);
    ASSERT_NE(pattern->sessionWrapper_, nullptr);

    /**
     * @tc.steps: step2. test SendBusinessData
     */
    UIContentBusinessCode code = UIContentBusinessCode::SEND_PAGE_MODE;
    AAFwk::Want want;
    BusinessDataSendType type = BusinessDataSendType::ASYNC;
    auto ret = pattern->SendBusinessData(code, std::move(want), type);
    ASSERT_EQ(ret, false);
    
    pattern->sessionWrapper_ = nullptr;
    ret = pattern->SendBusinessData(code, std::move(want), type);
    ASSERT_EQ(ret, false);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern OnUIExtBusinessReceiveReply
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, OnUIExtBusinessReceiveReplyTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test OnUIExtBusinessReceiveReply
     */
    UIContentBusinessCode code = UIContentBusinessCode::SEND_PAGE_MODE;
    AAFwk::Want data;
    std::string value = "testuec";
    data.SetParam("requestPageMode", value);
    std::optional<AAFwk::Want> reply;
    AAFwk::Want want2;
    want2.SetParam("requestPageMode", value);
    reply = want2;
    pattern->OnUIExtBusinessReceiveReply(code, data, reply);
    EXPECT_EQ(reply->HasParameter("pageMode"), false);

    pattern->RegisterReplyPageModeCallback();
    pattern->OnUIExtBusinessReceiveReply(code, data, reply);
    EXPECT_EQ(reply->HasParameter("pageMode"), true);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern OnUIExtBusinessReceive
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, OnUIExtBusinessReceiveTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test OnUIExtBusinessReceive
     */
    UIContentBusinessCode code = UIContentBusinessCode::SEND_PAGE_MODE;
    AAFwk::Want data;
    pattern->OnUIExtBusinessReceive(code, data);

    pattern->RegisterReplyPageModeCallback();
    pattern->OnUIExtBusinessReceive(code, data);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern NotifyHostWindowMode and NotifyHostWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, NotifyHostWindowModeTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test NotifyHostWindowMode and NotifyHostWindowMode
     */
    Rosen::WindowMode mode = Rosen::WindowMode::WINDOW_MODE_UNDEFINED;
    pattern->isWindowModeFollowHost_ = false;
    pattern->NotifyHostWindowMode(mode);

    pattern->isWindowModeFollowHost_ = true;
    pattern->NotifyHostWindowMode(mode);

    mode = Rosen::WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    pattern->NotifyHostWindowMode(mode);

    pattern->sessionWrapper_ = nullptr;
    pattern->NotifyHostWindowMode(mode);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern OnFrameNodeChanged
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, OnFrameNodeChangedTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test OnFrameNodeChanged
     */
    bool isAncestorNodeGeometryChange = false;
    bool isAncestorNodeTransformChange = false;
    FrameNodeChangeInfoFlag flag = FRAME_NODE_CHANGE_GEOMETRY_CHANGE;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_TRUE(isAncestorNodeGeometryChange);
    ASSERT_FALSE(isAncestorNodeTransformChange);

    flag = FRAME_NODE_CHANGE_INFO_NONE;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_FALSE(isAncestorNodeGeometryChange);
    ASSERT_FALSE(isAncestorNodeTransformChange);

    flag = FRAME_NODE_CHANGE_START_SCROLL;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_FALSE(isAncestorNodeGeometryChange);
    ASSERT_FALSE(isAncestorNodeTransformChange);

    flag = FRAME_NODE_CHANGE_END_SCROLL;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_FALSE(isAncestorNodeGeometryChange);
    ASSERT_FALSE(isAncestorNodeTransformChange);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern OnFrameNodeChanged
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, OnFrameNodeChangedTest002, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test OnFrameNodeChanged
     */
    bool isAncestorNodeGeometryChange = false;
    bool isAncestorNodeTransformChange = false;
    FrameNodeChangeInfoFlag flag = FRAME_NODE_CHANGE_TRANSFORM_CHANGE;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_FALSE(isAncestorNodeGeometryChange);
    ASSERT_TRUE(isAncestorNodeTransformChange);

    flag = FRAME_NODE_CHANGE_TRANSITION_START;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_FALSE(isAncestorNodeGeometryChange);
    ASSERT_FALSE(isAncestorNodeTransformChange);

    flag = FRAME_NODE_CONTENT_CLIP_CHANGE;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_FALSE(isAncestorNodeGeometryChange);
    ASSERT_FALSE(isAncestorNodeTransformChange);

    flag = FRAME_NODE_CHANGE_START_ANIMATION;
    pattern->OnFrameNodeChanged(flag);
    isAncestorNodeGeometryChange = pattern->IsAncestorNodeGeometryChange(flag);
    isAncestorNodeTransformChange = pattern->IsAncestorNodeTransformChange(flag);
    ASSERT_FALSE(isAncestorNodeGeometryChange);
    ASSERT_FALSE(isAncestorNodeTransformChange);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern GetAccessibilityRectInfo
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, GetAccessibilityRectInfoTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test GetAccessibilityRectInfo
     */
    auto context = NG::PipelineContext::GetCurrentContext();
    context->instanceId_ = IGNORE_POSITION_TRANSITION_SWITCH;
    auto rectInfo = pattern->GetAccessibilityRectInfo();
    EXPECT_EQ(rectInfo.left, 0);
    EXPECT_EQ(rectInfo.top, 0);
    EXPECT_EQ(rectInfo.scaleX, 1.0f);
    EXPECT_EQ(rectInfo.scaleY, 1.0f);
#endif
}

/**
 * @tc.name: UIExtensionComponentTestTwoNg
 * @tc.desc: Test the method of pattern TransferAccessibilityRectInfo
 * @tc.type: FUNC
 */
HWTEST_F(UIExtensionComponentTestTwoNg, TransferAccessibilityRectInfoTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a UIExtensionComponent Node
     */
    auto uiExtensionNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto uiExtensionNode = FrameNode::GetOrCreateFrameNode(
        UI_EXTENSION_COMPONENT_ETS_TAG, uiExtensionNodeId, []() { return AceType::MakeRefPtr<UIExtensionPattern>(); });
    ASSERT_NE(uiExtensionNode, nullptr);
    EXPECT_EQ(uiExtensionNode->GetTag(), V2::UI_EXTENSION_COMPONENT_ETS_TAG);
    auto pattern = uiExtensionNode->GetPattern<UIExtensionPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. test TransferAccessibilityRectInfo
     */
    pattern->TransferAccessibilityRectInfo();
#endif
}
} // namespace OHOS::Ace::NG

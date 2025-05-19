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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define private public
#define protected public
#include "accessibility_system_ability_client.h"
#include "core/components_ng/base/observer_handler.h"
#include "test/mock/base/mock_task_executor.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

#include "base/log/dump_log.h"
#include "adapter/ohos/osal/js_accessibility_manager.h"
#include "frameworks/core/accessibility/accessibility_node.h"
#include "adapter/ohos/entrance/ace_container.h"

using namespace OHOS::Accessibility;
using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
} // namespace OHOS::Ace

namespace OHOS::Ace::NG {
namespace {
} // namespace

class MockAccessibilityChildTreeCallback : public AccessibilityChildTreeCallback {
public:
    explicit MockAccessibilityChildTreeCallback(int64_t accessibilityId)
        : AccessibilityChildTreeCallback(accessibilityId)
    {}

    ~MockAccessibilityChildTreeCallback() override = default;

    bool OnRegister(uint32_t windowId, int32_t treeId) override
    {
        return true;
    }

    bool OnDeregister() override
    {
        return true;
    }

    bool OnSetChildTree(int32_t childWindowId, int32_t childTreeId) override
    {
        return true;
    }

    bool OnDumpChildInfo(const std::vector<std::string>& params, std::vector<std::string>& info) override
    {
        return true;
    }

    void OnClearRegisterFlag() override
    {
        isReg_ = false;
    }

private:
    bool isReg_ = false;
};

class MockAccessibilitySAObserverCallback : public AccessibilitySAObserverCallback {
public:
    explicit MockAccessibilitySAObserverCallback(int64_t accessibilityId)
        : AccessibilitySAObserverCallback(accessibilityId)
    {}

    ~MockAccessibilitySAObserverCallback() override = default;

    bool OnState(bool state) override
    {
        return true;
    }
};

class MockStageManager : public StageManager {
public:
    explicit MockStageManager(const RefPtr<FrameNode>& stage)
        : StageManager(stage)
    {}

    ~MockStageManager() override = default;

    MOCK_METHOD(bool, IsSplitMode, (), (const));
    MOCK_METHOD(std::vector<RefPtr<FrameNode>>, GetTopPagesWithTransition, (), (const));
    MOCK_METHOD(std::vector<std::string>, GetTopPagePaths, (), (const));
    MOCK_METHOD(RefPtr<FrameNode>, GetLastPageWithTransition, (), (const));
};

class JsAccessibilityManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void JsAccessibilityManagerTest::SetUpTestCase()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
    MockContainer::Current()->taskExecutor_ = AceType::MakeRefPtr<MockTaskExecutor>();
    MockContainer::Current()->pipelineContext_ = MockPipelineContext::GetCurrentContext();
    MockContainer::Current()->pipelineContext_->taskExecutor_ = MockContainer::Current()->taskExecutor_;

    std::unique_ptr<std::ostream> ostream = std::make_unique<std::ostringstream>();
    ASSERT_NE(ostream, nullptr);
    DumpLog::GetInstance().SetDumpFile(std::move(ostream));
}

void JsAccessibilityManagerTest::TearDownTestCase()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
}

class MockJsAccessibilityManager : public Framework::JsAccessibilityManager {
public:
    MOCK_METHOD3(SendEventToAccessibilityWithNode,
        void(const AccessibilityEvent& accessibilityEvent,
            const RefPtr<AceType>& node, const RefPtr<PipelineBase>& context));
    MOCK_METHOD0(GenerateAccessibilityWorkMode, AccessibilityWorkMode());
    MOCK_METHOD1(HandleWillClickAccept, void(RefPtr<NG::FrameNode>& frameNode));
    MOCK_METHOD1(HandleDidClickAccept, void(RefPtr<NG::FrameNode>& frameNode));
};

/**
 * @tc.name: JsAccessibilityManager001
 * @tc.desc: UpdateAccessibilityElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    Accessibility::AccessibilityElementInfo nodeInfo;
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    accessibilityProperty->SetAccessibilityRole("test");
    accessibilityProperty->SetAccessibilityCustomRole("test");
    accessibilityProperty->SetUserRangeCurrentValue(5);
    accessibilityProperty->SetUserRangeMinValue(1);
    accessibilityProperty->SetUserRangeMaxValue(10);
    accessibilityProperty->SetAccessibilityLevel("yes");
    jsAccessibilityManager->UpdateAccessibilityElementInfo(frameNode, nodeInfo);

    /**
    * @tc.steps: step2. init accessibilityProperty and set level yes.
    * @tc.expected: UpdateAccessibilityElementInfo nodeInfo level is yes.
    */
    EXPECT_EQ(nodeInfo.GetAccessibilityLevel(), "yes");
}

/**
 * @tc.name: JsAccessibilityManager002
 * @tc.desc: UpdateElementInfoTreeId
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    Accessibility::AccessibilityElementInfo info;
    info.SetPageId(1);
    info.SetParent(0);
    info.SetBelongTreeId(10);
    info.SetAccessibilityId(11);
    jsAccessibilityManager->UpdateElementInfoTreeId(info);

    /**
    * @tc.steps: step2. init AccessibilityElementInfo and SetBelongTreeId 10, pageid 1, parent 0.
    * @tc.expected: UpdateElementInfoTreeId nodeInfo pageid is not equal 1.
    */
    EXPECT_NE(info.GetPageId(), 1);

    info.SetBelongTreeId(0);
    info.SetPageId(1);
    jsAccessibilityManager->UpdateElementInfoTreeId(info);

    /**
    * @tc.steps: step3. AccessibilityElementInfo and SetBelongTreeId 0
    * @tc.expected: UpdateElementInfoTreeId nodeInfo pageid is 1.
    */
    EXPECT_EQ(info.GetPageId(), 1);
}

/**
 * @tc.name: JsAccessibilityManager003
 * @tc.desc: UpdateAccessibilityVisible
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), false);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
     * @tc.steps: step2. test UpdateAccessibilityVisible
     */
    Accessibility::AccessibilityElementInfo nodeInfo;
    // without parent node return visiable false
    jsAccessibilityManager->UpdateAccessibilityVisible(frameNode, nodeInfo);
    EXPECT_EQ(nodeInfo.GetAccessibilityVisible(), false);

    nodeInfo.SetParent(0);
    jsAccessibilityManager->UpdateAccessibilityVisible(frameNode, nodeInfo);
    EXPECT_EQ(nodeInfo.GetAccessibilityVisible(), false);

    /**
     * @tc.steps: step3. test with page_node
     */
    Accessibility::AccessibilityElementInfo nodeInfoNew;
    auto pageNode = FrameNode::CreateFrameNode(V2::PAGE_ETS_TAG, 1,
        AceType::MakeRefPtr<Pattern>(), false);
    jsAccessibilityManager->UpdateAccessibilityVisible(pageNode, nodeInfoNew);
}

/**
 * @tc.name: JsAccessibilityManager004
 * @tc.desc: GenerateCommonProperty
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::WINDOW_SCENE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), true);
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    rootNode->AddChild(frameNode);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);
    jsAccessibilityManager->SetWindowId(1);

    /**
     * @tc.steps: step2. test GenerateCommonProperty
     */
    auto mainContext = context;
    Framework::CommonProperty commonProperty;
    jsAccessibilityManager->GenerateCommonProperty(context, commonProperty, mainContext, frameNode);
    EXPECT_EQ(commonProperty.innerWindowId, -1);

    jsAccessibilityManager->DumpTreeNodeCommonInfoNg(frameNode, commonProperty);
    jsAccessibilityManager->DumpTreeNodeSimplifyInfoNG(frameNode, 1, commonProperty, 1);
    jsAccessibilityManager->DumpTreeNG(frameNode, 1, frameNode->GetId(), true);
    jsAccessibilityManager->DumpTree(1, 1, true);

    jsAccessibilityManager->UpdateViewScale();
    EXPECT_EQ(jsAccessibilityManager->scaleX_, 1.0f);
}

/**
 * @tc.name: JsAccessibilityManager005
 * @tc.desc: dump func
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    Accessibility::AccessibilityElementInfo nodeInfo;
    nodeInfo.SetAccessibilityId(1);
    jsAccessibilityManager->DumpAccessibilityPropertyNG(nodeInfo);
    jsAccessibilityManager->DumpCommonPropertyNG(nodeInfo, 1);

    /**
     * @tc.steps: step2. test DumpTreeNG
     */
    bool useWindowId = false;
    uint32_t windowId = 1;
    int64_t rootId = -1;
    bool isDumpSimplify = false;
    jsAccessibilityManager->DumpTreeNG(useWindowId, windowId, rootId, isDumpSimplify);

    rootId = 1;
    jsAccessibilityManager->SetWindowId(0);
    jsAccessibilityManager->DumpTreeNG(useWindowId, windowId, rootId, isDumpSimplify);

    jsAccessibilityManager->SetWindowId(1);
    jsAccessibilityManager->DumpTreeNG(useWindowId, windowId, rootId, isDumpSimplify);

    /**
     * @tc.steps: step3. test DumpHoverTestNG/DumpTreeNodeSafeAreaInfoNg
     */
    bool verbose = false;
    jsAccessibilityManager->DumpHoverTestNG(windowId, rootId, 100, 100, verbose);
    jsAccessibilityManager->DumpTreeNodeSafeAreaInfoNg(frameNode);
    EXPECT_EQ(nodeInfo.GetAccessibilityId(), 1);
}

/**
 * @tc.name: JsAccessibilityManager006
 * @tc.desc: UpdateVirtualNodeFocus
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
     * @tc.steps: step2. test UpdateVirtualNodeFocus
     */
    Accessibility::AccessibilityElementInfo nodeInfo;
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    frameNode->SetGeometryNode(geometryNode);
    jsAccessibilityManager->SaveLast(0, frameNode);
    jsAccessibilityManager->UpdateVirtualNodeFocus();
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    ASSERT_NE(accessibilityProperty, nullptr);
    EXPECT_EQ(accessibilityProperty->GetAccessibilityFocusState(), false);
}

/**
 * @tc.name: JsAccessibilityManager007
 * @tc.desc: stateObserver
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
     * @tc.steps: step2. test SubscribeToastObserver
     */
    auto ret = jsAccessibilityManager->SubscribeToastObserver();
    EXPECT_EQ(ret, true);

    /**
     * @tc.steps: step3. test UnsubscribeToastObserver
     */
    ret = jsAccessibilityManager->UnsubscribeToastObserver();
    EXPECT_EQ(ret, true);

    /**
     * @tc.steps: step4. test SubscribeStateObserver
     */
    ret = jsAccessibilityManager->SubscribeStateObserver(16);
    EXPECT_EQ(ret, false);

    /**
     * @tc.steps: step5. test UnsubscribeStateObserver
     */
    ret = jsAccessibilityManager->UnsubscribeStateObserver(16);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: JsAccessibilityManager008
 * @tc.desc: SendAccessibilitySyncEvent, TransferAccessibilityAsyncEvent
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);
    auto client = Accessibility::AccessibilitySystemAbilityClient::GetInstance();

    /**
     * @tc.steps: step2. test SendAccessibilitySyncEvent with register true
     */
    AccessibilityEvent accessibilityEvent;
    Accessibility::AccessibilityEventInfo eventInfo;
    auto ret = jsAccessibilityManager->SendAccessibilitySyncEvent(accessibilityEvent, eventInfo);
    if (client) {
        bool isEnabled = false;
        client->IsEnabled(isEnabled);
        EXPECT_EQ(ret, isEnabled);
    } else {
        EXPECT_EQ(ret, false);
    }

    /**
     * @tc.steps: step3. test TransferAccessibilityAsyncEvent with register true
     */
    int64_t uiExtensionOffset = 1000;
    ret = jsAccessibilityManager->TransferAccessibilityAsyncEvent(eventInfo, uiExtensionOffset);
    if (client) {
        bool isEnabled = false;
        client->IsEnabled(isEnabled);
        EXPECT_EQ(ret, isEnabled);
    } else {
        EXPECT_EQ(ret, false);
    }

    /**
     * @tc.steps: step4. test SendAccessibilitySyncEvent with register false
     */
    jsAccessibilityManager->Register(false);
    ret = jsAccessibilityManager->SendAccessibilitySyncEvent(accessibilityEvent, eventInfo);
    EXPECT_EQ(ret, false);
    /**
     * @tc.steps: step5. test TransferAccessibilityAsyncEvent with register false
     */
    ret = jsAccessibilityManager->TransferAccessibilityAsyncEvent(eventInfo, uiExtensionOffset);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: JsAccessibilityManager009
 * @tc.desc: GetDelayTimeBeforeSendEvent
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    accessibilityProperty->SetAccessibilityLevel("yes");
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);

    /**
     * @tc.steps: step2. test GetPipelineByWindowId
     */
    AccessibilityEvent event;
    int64_t ret;
    ret = jsAccessibilityManager->GetDelayTimeBeforeSendEvent(event, frameNode);
    EXPECT_EQ(ret, 0);
    // test GetDelayTimeBeforeSendEvent with node selected, event type click, return 0
    event.type = AccessibilityEventType::CLICK;
    accessibilityProperty->SetUserSelected(true);
    ret = jsAccessibilityManager->GetDelayTimeBeforeSendEvent(event, frameNode);
    EXPECT_EQ(ret, 20);
    // test GetDelayTimeBeforeSendEvent with node not selected, return 0
    accessibilityProperty->SetUserSelected(false);
    ret = jsAccessibilityManager->GetDelayTimeBeforeSendEvent(event, frameNode);
    EXPECT_EQ(ret, 20);

    ret = jsAccessibilityManager->GetDelayTimeBeforeSendEvent(event, nullptr);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: JsAccessibilityManager010
 * @tc.desc: GetPipelineByWindowId
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);
    jsAccessibilityManager->SetWindowId(1);

    /**
     * @tc.steps: step2. test GetPipelineByWindowId
     */
    auto pattern = jsAccessibilityManager->GetPipelineByWindowId(1);
    EXPECT_NE(pattern, nullptr);

    pattern = jsAccessibilityManager->GetPipelineByWindowId(2);
    EXPECT_EQ(pattern, nullptr);
}

/**
 * @tc.name: JsAccessibilityManager011
 * @tc.desc: CheckDumpInfoParams,OnDumpInfoNG,CheckDumpHandleEventParams,DumpProperty
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    std::vector<std::string> params = {};
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
     * @tc.steps: step2. test CheckDumpInfoParams
     */
    auto ret = jsAccessibilityManager->CheckDumpInfoParams(params);
    EXPECT_EQ(ret, false);

    params = { "-inspector", "-accessibility", "-simplify"};
    ret = jsAccessibilityManager->CheckDumpInfoParams(params);
    EXPECT_EQ(ret, true);

    params = { "-inspector", "-accessibility"};
    ret = jsAccessibilityManager->CheckDumpInfoParams(params);
    EXPECT_EQ(ret, true);

    /**
     * @tc.steps: step3. test OnDumpInfoNG
     */
    params = { "-inspector", "-accessibility", "-simplify", "-w", "1"};
    jsAccessibilityManager->OnDumpInfoNG(params, 1, false);

    /**
     * @tc.steps: step4. test CheckDumpHandleEventParams
     */
    ret = jsAccessibilityManager->CheckDumpHandleEventParams(params);
    EXPECT_EQ(ret, true);

    /**
     * @tc.steps: step5. test DumpProperty
     */
    jsAccessibilityManager->DumpProperty(params);
}

/**
 * @tc.name: JsAccessibilityManager012
 * @tc.desc: Test UIExtensionManager ConvertActionTypeToBoolen
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto context = NG::PipelineContext::GetCurrentContext();
    int64_t elementId = 0;

    /**
     * @tc.steps: step2. test ConvertActionTypeToBoolen
     */
    auto ret = jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_FOCUS,
        frameNode, elementId, context);
    EXPECT_EQ(ret, false);

    ret = jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS,
        frameNode, elementId, context);
    EXPECT_EQ(ret, false);

    ret = jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLICK,
        frameNode, elementId, context);
    EXPECT_EQ(ret, false);

    ret = jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_LONG_CLICK,
        frameNode, elementId, context);
    EXPECT_EQ(ret, false);

    ret = jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS,
        frameNode, elementId, context);
    EXPECT_EQ(ret, true);

    ret = jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS,
        frameNode, elementId, context);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: JsAccessibilityManager013
 * @tc.desc: Test UIExtensionManager SendEventToAccessibilityWithNode/SendEventToAccessibilityWithNodeInner
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::WINDOW_SCENE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), true);
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    rootNode->AddChild(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    accessibilityProperty->SetAccessibilityLevel("yes");
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
     * @tc.steps: step2. test SendEventToAccessibilityWithNode
     */
    AccessibilityEvent accessibilityEvent;
    auto context = NG::PipelineContext::GetCurrentContext();
    EXPECT_EQ(jsAccessibilityManager->context_.Upgrade(), nullptr);
    jsAccessibilityManager->SetPipelineContext(context);
    EXPECT_NE(jsAccessibilityManager->context_.Upgrade(), nullptr);
    jsAccessibilityManager->SendEventToAccessibilityWithNode(accessibilityEvent, frameNode, context);

    /**
     * @tc.steps: step2. test SendEventToAccessibilityWithNodeInner
     */
    jsAccessibilityManager->SendEventToAccessibilityWithNodeInner(accessibilityEvent, frameNode, context);
}

/**
 * @tc.name: JsAccessibilityManager014
 * @tc.desc: Test UIExtensionManager RegisterSubWindowInteractionOperation/CanAccessibilityFocused
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::WINDOW_SCENE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), true);
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    rootNode->AddChild(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    accessibilityProperty->SetAccessibilityLevel("yes");
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
     * @tc.steps: step2. test RegisterSubWindowInteractionOperation
     */
    jsAccessibilityManager->RegisterSubWindowInteractionOperation(0);

    /**
     * @tc.steps: step3. test CanAccessibilityFocused
     */
    auto node = jsAccessibilityManager->GetAccessibilityNodeFromPage(0);
    EXPECT_FALSE(jsAccessibilityManager->CanAccessibilityFocused(node));
}

/**
 * @tc.name: JsAccessibilityManager015
 * @tc.desc: Test UIExtensionManager NeedRegisterChildTree
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
     * @tc.steps: step2. test NeedRegisterChildTree
     */
    uint32_t parentWindowId = 0;
    int32_t parentTreeId = 0;
    int64_t parentElementId = 0;
    jsAccessibilityManager->parentWindowId_ = parentWindowId;
    jsAccessibilityManager->parentTreeId_ = parentTreeId;
    jsAccessibilityManager->parentElementId_ = parentElementId;
    EXPECT_TRUE(jsAccessibilityManager->NeedRegisterChildTree(parentWindowId, parentTreeId, parentElementId));

    jsAccessibilityManager->Register(true);
    EXPECT_FALSE(jsAccessibilityManager->NeedRegisterChildTree(parentWindowId, parentTreeId, parentElementId));

    jsAccessibilityManager->parentWindowId_ = 1;
    EXPECT_TRUE(jsAccessibilityManager->NeedRegisterChildTree(parentWindowId, parentTreeId, parentElementId));
    jsAccessibilityManager->parentTreeId_ = 1;
    EXPECT_TRUE(jsAccessibilityManager->NeedRegisterChildTree(parentWindowId, parentTreeId, parentElementId));
    jsAccessibilityManager->parentElementId_ = 1;
    EXPECT_TRUE(jsAccessibilityManager->NeedRegisterChildTree(parentWindowId, parentTreeId, parentElementId));
}

/**
 * @tc.name: JsAccessibilityManager016
 * @tc.desc: Test UIExtensionManager OnDumpChildInfoForThird/CheckIsChildElement
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    std::vector<std::string> params = {};
    std::vector<std::string> info = {};

    /**
     * @tc.steps: step2. test CheckIsChildElement
     */
    int64_t elementId = 0;
    int64_t rootId = 0;
    EXPECT_FALSE(
        jsAccessibilityManager->CheckIsChildElement(elementId, params, info, Framework::DumpMode::TREE, rootId));
    EXPECT_FALSE(
        jsAccessibilityManager->CheckIsChildElement(elementId, params, info, Framework::DumpMode::NODE, rootId));
}

/**
 * @tc.name: JsAccessibilityManager017
 * @tc.desc: Test UIExtensionManager RegisterInteractionOperation
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::WINDOW_SCENE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), true);
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    rootNode->AddChild(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    accessibilityProperty->SetAccessibilityLevel("yes");
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    std::vector<std::string> params = {};
    std::vector<std::string> info = {};

    /**
     * @tc.steps: step2. test RegisterInteractionOperation
     */
    auto ret = jsAccessibilityManager->RegisterInteractionOperation(0);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: JsAccessibilityManager018
 * @tc.desc: Test UIExtensionManager RegisterAccessibilityChildTreeCallback/DeregisterAccessibilityChildTreeCallback
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    EXPECT_EQ(0, jsAccessibilityManager->childTreeCallbackMap_.size());

    /**
     * @tc.steps: step2. test RegisterAccessibilityChildTreeCallback
     */
    int64_t elementId0 = 0;
    auto callback0 = std::make_shared<MockAccessibilityChildTreeCallback>(0);
    jsAccessibilityManager->RegisterAccessibilityChildTreeCallback(elementId0, callback0);
    EXPECT_EQ(1, jsAccessibilityManager->childTreeCallbackMap_.size());
    EXPECT_EQ(callback0, jsAccessibilityManager->childTreeCallbackMap_[elementId0]);
    int64_t elementId1 = 1;
    auto callback1 = std::make_shared<MockAccessibilityChildTreeCallback>(1);
    jsAccessibilityManager->RegisterAccessibilityChildTreeCallback(elementId1, callback1);
    EXPECT_EQ(2, jsAccessibilityManager->childTreeCallbackMap_.size());
    EXPECT_EQ(callback1, jsAccessibilityManager->childTreeCallbackMap_[elementId1]);

    /**
     * @tc.steps: step3. test DeregisterAccessibilityChildTreeCallback
     */
    jsAccessibilityManager->DeregisterAccessibilityChildTreeCallback(elementId1);
    EXPECT_EQ(1, jsAccessibilityManager->childTreeCallbackMap_.size());
    EXPECT_EQ(callback0, jsAccessibilityManager->childTreeCallbackMap_[elementId0]);
    jsAccessibilityManager->DeregisterAccessibilityChildTreeCallback(elementId0);
    EXPECT_EQ(0, jsAccessibilityManager->childTreeCallbackMap_.size());
}

/**
 * @tc.name: JsAccessibilityManager019
 * @tc.desc: Test UIExtensionManager RegisterAccessibilitySAObserverCallback/DeregisterAccessibilitySAObserverCallback
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    EXPECT_EQ(0, jsAccessibilityManager->componentSACallbackMap_.size());

    /**
     * @tc.steps: step2. test RegisterAccessibilitySAObserverCallback
     */
    int64_t elementId0 = 0;
    auto callback0 = std::make_shared<MockAccessibilitySAObserverCallback>(0);
    jsAccessibilityManager->RegisterAccessibilitySAObserverCallback(elementId0, callback0);
    EXPECT_EQ(1, jsAccessibilityManager->componentSACallbackMap_.size());
    EXPECT_EQ(callback0, jsAccessibilityManager->componentSACallbackMap_[elementId0]);
    int64_t elementId1 = 1;
    auto callback1 = std::make_shared<MockAccessibilitySAObserverCallback>(1);
    jsAccessibilityManager->RegisterAccessibilitySAObserverCallback(elementId1, callback1);
    EXPECT_EQ(2, jsAccessibilityManager->componentSACallbackMap_.size());
    EXPECT_EQ(callback1, jsAccessibilityManager->componentSACallbackMap_[elementId1]);

    /**
     * @tc.steps: step3. test DeregisterAccessibilitySAObserverCallback
     */
    jsAccessibilityManager->DeregisterAccessibilitySAObserverCallback(elementId1);
    EXPECT_EQ(1, jsAccessibilityManager->componentSACallbackMap_.size());
    EXPECT_EQ(callback0, jsAccessibilityManager->componentSACallbackMap_[elementId0]);
    jsAccessibilityManager->DeregisterAccessibilitySAObserverCallback(elementId0);
    EXPECT_EQ(0, jsAccessibilityManager->componentSACallbackMap_.size());
}
/**
 * @tc.name: UpdateWindowInfo001
 * @tc.desc: UpdateWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, UpdateWindowInfo001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct jsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    auto pipelineContext = MockContainer::Current()->GetPipelineContext();
    ASSERT_NE(pipelineContext, nullptr);
    jsAccessibilityManager->context_ = pipelineContext;

    /**
     * @tc.steps: step2. construct windowInfo
     */
    AccessibilityWindowInfo windowInfo;
    windowInfo.left = 10;
    windowInfo.top = 10;
    windowInfo.scaleX = 1.0f;
    windowInfo.scaleY = 1.0f;

    /**
     * @tc.steps: step3. call and test UpdateWindowInfo method
     */
    jsAccessibilityManager->UpdateWindowInfo(windowInfo, pipelineContext);
    EXPECT_EQ(windowInfo.left, 10);
    EXPECT_EQ(windowInfo.top, 10);
    EXPECT_EQ(windowInfo.scaleX, 1.0f);
    EXPECT_EQ(windowInfo.scaleY, 1.0f);

    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    ASSERT_NE(container, nullptr);
    container->SetSingleHandTransform(Platform::SingleHandTransform(100.0f, 200.0f, 0.7f, 0.6f));

    jsAccessibilityManager->UpdateWindowInfo(windowInfo, pipelineContext);
    EXPECT_EQ(windowInfo.left, 107.0f);
    EXPECT_EQ(windowInfo.top, 206.0f);
    EXPECT_EQ(windowInfo.scaleX, 0.7f);
    EXPECT_EQ(windowInfo.scaleY, 0.6f);
}

/**
 * @tc.name: GenerateWindowInfo001
 * @tc.desc: GenerateWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, GenerateWindowInfo001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct jsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);

    /**
     * @tc.steps: step3. call and test GenerateWindowInfo method
     */
    auto windowInfo = jsAccessibilityManager->GenerateWindowInfo(nullptr, nullptr);
    EXPECT_EQ(windowInfo.left, 0);
    EXPECT_EQ(windowInfo.top, 0);
    EXPECT_EQ(windowInfo.scaleX, 1.0f);
    EXPECT_EQ(windowInfo.scaleY, 1.0f);

    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    windowInfo = jsAccessibilityManager->GenerateWindowInfo(frameNode, nullptr);

    auto pipelineContext = MockContainer::Current()->GetPipelineContext();
    ASSERT_NE(pipelineContext, nullptr);
    jsAccessibilityManager->context_ = pipelineContext;
    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    ASSERT_NE(container, nullptr);
    auto singleHandTransform = container->GetSingleHandTransform();
    container->SetSingleHandTransform(Platform::SingleHandTransform(0.0f, 0.0f, 1.0f, 1.0f));

    auto context = NG::PipelineContext::GetCurrentContext();
    ASSERT_NE(context, nullptr);
    jsAccessibilityManager->getParentRectHandler_ = nullptr;
    jsAccessibilityManager->getParentRectHandlerNew_ = nullptr;
    windowInfo = jsAccessibilityManager->GenerateWindowInfo(nullptr, context);

    EXPECT_EQ(windowInfo.left, 0);
    EXPECT_EQ(windowInfo.top, 0);
    EXPECT_EQ(windowInfo.scaleX, 1.0f);
    EXPECT_EQ(windowInfo.scaleY, 1.0f);

    /**
     * @tc.steps: step4. mock IsReentrantLimit, and then test GenerateWindowInfo method again
     */
    jsAccessibilityManager->RegisterGetParentRectHandler();
    EXPECT_NE(jsAccessibilityManager->getParentRectHandlerNew_, nullptr);
    jsAccessibilityManager->SetReentrantLimit(true);
    windowInfo = jsAccessibilityManager->GenerateWindowInfo(frameNode, context);
 
    EXPECT_EQ(windowInfo.left, 0);
    EXPECT_EQ(windowInfo.top, 0);
    EXPECT_EQ(windowInfo.scaleX, 1.0f);
    EXPECT_EQ(windowInfo.scaleY, 1.0f);

    /**
     * @tc.steps: step5. mock SingleHandTransform, and then test GenerateWindowInfo method again
     */
    container->SetSingleHandTransform(Platform::SingleHandTransform(100.0f, 200.0f, 0.7f, 0.7f));

    jsAccessibilityManager->UpdateWindowInfo(windowInfo, pipelineContext);
    EXPECT_EQ(windowInfo.left, 100.0f);
    EXPECT_EQ(windowInfo.top, 200.0f);
    EXPECT_EQ(windowInfo.scaleX, 0.7f);
    EXPECT_EQ(windowInfo.scaleY, 0.7f);
}

/**
 * @tc.name: SingleHandTransformTest001
 * @tc.desc: SetSingleHandTransform & GetSingleHandTransform
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, SingleHandTransformTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct AceContainer
     */
    auto pipelineContext = MockContainer::Current()->GetPipelineContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto container = Platform::AceContainer::GetContainer(pipelineContext->GetInstanceId());
    ASSERT_NE(container, nullptr);

    /**
     * @tc.steps: step2. test SetSingleHandTransform & GetSingleHandTransform
     */
    container->SetSingleHandTransform(Platform::SingleHandTransform(100.0f, 200.0f, 0.7f, 0.6f));
    auto singleHandTransform = container->GetSingleHandTransform();
    EXPECT_EQ(singleHandTransform.x_, 100.0f);
    EXPECT_EQ(singleHandTransform.y_, 200.0f);
    EXPECT_EQ(singleHandTransform.scaleX_, 0.7f);
    EXPECT_EQ(singleHandTransform.scaleY_, 0.6f);
}

/**
 * @tc.name: JsAccessibilityManager020
 * @tc.desc: dump event test  DumpProcessEventParameters
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager020, TestSize.Level1)
{
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);
    bool ret;
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = 1;
    accessibilityEvent.windowChangeTypes = WindowUpdateType::WINDOW_UPDATE_ACTIVE;
    accessibilityEvent.type = AccessibilityEventType::PAGE_CHANGE;
    // 1. parameter stackNodeId
    std::vector<std::string> params;
    params.push_back("-inspector");
    params.push_back("1");

    params.push_back("--stackNodeId");

    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, false);

    int64_t testStackId = 2;
    params.push_back(std::to_string(testStackId));
    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(accessibilityEvent.stackNodeId, testStackId);

    // 2. parameter beforeText
    params.push_back("--beforeText");
    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, false);

    std::string beforeText = "teststr1";
    params.push_back(beforeText);
    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(accessibilityEvent.beforeText, beforeText);
}

/**
 * @tc.name: JsAccessibilityManager020
 * @tc.desc: dump event test  DumpProcessEventParameters
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager0201, TestSize.Level1)
{
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);
    bool ret;
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = 1;
    accessibilityEvent.windowChangeTypes = WindowUpdateType::WINDOW_UPDATE_ACTIVE;
    accessibilityEvent.type = AccessibilityEventType::PAGE_CHANGE;
    std::vector<std::string> params;
    params.push_back("-inspector");
    params.push_back("1");

    // 3. parameter latestContent
    params.push_back("--latestContent");
    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, false);

    std::string latestContent = "teststr2";
    params.push_back(latestContent);
    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(accessibilityEvent.latestContent, latestContent);

    // 4. parameter latestContent
    params.push_back("--textAnnounced");
    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, false);

    std::string textAnnounced = "teststr3";
    params.push_back(textAnnounced);
    ret = jsAccessibilityManager->DumpProcessEventParameters(accessibilityEvent, params);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(accessibilityEvent.textAnnouncedForAccessibility, textAnnounced);
}

/**
 * @tc.name: JsAccessibilityManager021
 * @tc.desc: dump event test  DumpProcessEventParameters
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager021, TestSize.Level1)
{
    auto context = NG::PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    CHECK_NULL_VOID(frameNode);
    auto root = context->GetRootElement();
    CHECK_NULL_VOID(root);
    root->AddChild(frameNode);


    MockJsAccessibilityManager mockJsManger;
    mockJsManger.SetPipelineContext(context);
    mockJsManger.Register(true);

    int64_t nodeId = frameNode->GetAccessibilityId();
    int32_t eventId = 2;
    // 1. check parameter fail
    std::vector<std::string> params;
    params.push_back("-inspector");
    params.push_back("1");
    params.push_back("--stackNodeId");
    EXPECT_CALL(mockJsManger, SendEventToAccessibilityWithNode(_, _, _))
        .Times(0);

    mockJsManger.DumpSendEventTest(nodeId, eventId, params);

    // 2. check parameter success
    int64_t testStackId = 2;
    params.push_back(std::to_string(testStackId));

    EXPECT_CALL(mockJsManger, SendEventToAccessibilityWithNode(_, _, _))
        .Times(1);

    mockJsManger.DumpSendEventTest(nodeId, eventId, params);
}

/**
 * @tc.name: JsAccessibilityManager022
 * @tc.desc: dump event test  OnDumpInfoNG
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager022, TestSize.Level1)
{
    auto context = NG::PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    CHECK_NULL_VOID(frameNode);
    auto root = context->GetRootElement();
    CHECK_NULL_VOID(root);
    root->AddChild(frameNode);
    MockJsAccessibilityManager mockJsManger;
    mockJsManger.SetPipelineContext(context);
    mockJsManger.Register(true);

    std::vector<std::string> params;

    // 1.  dump event
    params.push_back("-inspector");
    params.push_back("--event-test");
    params.push_back(std::to_string(frameNode->GetAccessibilityId()).c_str());
    params.push_back("2");

    EXPECT_CALL(mockJsManger, SendEventToAccessibilityWithNode(_, _, _))
        .Times(1);
    mockJsManger.OnDumpInfoNG(params, 1, false);
}

/**
 * @tc.name: JsAccessibilityManager023
 * @tc.desc: UpdateAccessibilityElementInfo ScrollTriggerable
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct JsAccessibilityManager
     * @tc.desc: Create a FrameNode object and initialize the JsAccessibilityManager object.
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    CHECK_NULL_VOID(frameNode);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    Accessibility::AccessibilityElementInfo nodeInfo;

    // Update information in nodeInfo and verify that its default scrollable is true
    jsAccessibilityManager->UpdateAccessibilityElementInfo(frameNode, nodeInfo);
    CHECK_NULL_VOID(jsAccessibilityManager);
    EXPECT_EQ(nodeInfo.GetAccessibilityScrollable(), true);

    /**
    * @tc.steps: step2. Initialize accessibilityProperty and set ScrollTriggerable to false.
    * @tc.expected: The updated nodeInfo's ScrollTriggerable property should be false.
    */
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetUserScrollTriggerable(false);
    jsAccessibilityManager->UpdateAccessibilityElementInfo(frameNode, nodeInfo);
    EXPECT_EQ(nodeInfo.GetAccessibilityScrollable(), false);

    /**
    * @tc.steps: step3. Reset the ScrollTriggerable property of accessibilityProperty.
    * @tc.expected: The updated nodeInfo's ScrollTriggerable property should be true.
    */
    accessibilityProperty->ResetUserScrollTriggerable();
    jsAccessibilityManager->UpdateAccessibilityElementInfo(frameNode, nodeInfo);
    EXPECT_EQ(nodeInfo.GetAccessibilityScrollable(), true);
}

class MockRenderContextTest : public RenderContext {
public:
    RectF GetPaintRectWithoutTransform() override
    {
        return *retf;
    }
    std::shared_ptr<RectF> retf;
};

/**
 * @tc.name: FrameNodeAccessibilityVisible02
 * @tc.desc: Test the function accessibilityVisible
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, FrameNodeAccessibilityVisible02, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create parent frameNode and set up its rect.
     */
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    CHECK_NULL_VOID(frameNode);
    EXPECT_NE(frameNode->pattern_, nullptr);
    frameNode->isActive_ = true;
    auto pipeline = frameNode->GetContext();

    auto parentRender = AceType::MakeRefPtr<MockRenderContextTest>();
    CHECK_NULL_VOID(parentRender);
    auto parentRec = std::make_shared<RectF>(10.0, 10.0, 10.0, 5.0);
    parentRender->retf = parentRec;
    frameNode->renderContext_ = parentRender;

    /**
     * @tc.steps: step2. create child frameNode and set up its rect.
     */
    auto childNode = FrameNode::CreateFrameNode(
    "child", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    childNode->isActive_ = true;
    auto childRender = AceType::MakeRefPtr<MockRenderContextTest>();
    CHECK_NULL_VOID(childRender);
    auto childRec = std::make_shared<RectF>(10.0, 10.0, 10.0, 10.0);
    childRender->retf = childRec;
    childNode->renderContext_ = childRender;

    /**
     * @tc.steps: step3. add parent and child node to the pipeline context.
     */
    auto context = NG::PipelineContext::GetCurrentContext();
    context->GetRootElement()->AddChild(frameNode);
    frameNode->AddChild(childNode);

    /**
     * @tc.steps: step4. verify the child node's accessibilityVisible is true.
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    CHECK_NULL_VOID(jsAccessibilityManager);
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);
    RefPtr<NG::PipelineContext> ngPipeline;
    ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    std::list<AccessibilityElementInfo> extensionElementInfos;
    jsAccessibilityManager->SearchElementInfoByAccessibilityIdNG(
        childNode->GetAccessibilityId(), 1, extensionElementInfos, ngPipeline, 1);
    for (auto& extensionElementInfo : extensionElementInfos) {
        if (childNode->GetAccessibilityId() == extensionElementInfo.GetAccessibilityId()) {
            EXPECT_TRUE(extensionElementInfo.GetAccessibilityVisible());
        }
    }
}

/**
 * @tc.name: FrameNodeAccessibilityVisible03
 * @tc.desc: Test searching root element info when elementId is -1
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, FrameNodeAccessibilityVisible03, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create a pipeline context and set it as the current context.
     */
    auto context = NG::PipelineContext::GetCurrentContext();

    /**
     * @tc.steps: step2. create an instance of JsAccessibilityManager and set the pipeline context.
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    CHECK_NULL_VOID(jsAccessibilityManager);
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);

    /**
     * @tc.steps: step3. call the function SearchElementInfoByAccessibilityIdNG with an invalid elementId.
     */
    std::list<AccessibilityElementInfo> infos;
    jsAccessibilityManager->SearchElementInfoByAccessibilityIdNG(-1, 1, infos, context, 0);

    /**
     * @tc.steps: step4. verify that the infos list contains elements with specific properties.
     */
    EXPECT_FALSE(infos.empty());
    for (auto& info : infos) {
        EXPECT_FALSE(info.GetAccessibilityVisible());
    }
}

/**
 * @tc.name: JsAccessibilityManager024
 * @tc.desc: RegisterGetParentRectHandler
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager024, TestSize.Level1)
{
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    /**
    * @tc.steps: step2. test RegisterGetParentRectHandler
    */
    jsAccessibilityManager->uecRectInfo_.isChanged = false;
    jsAccessibilityManager->RegisterGetParentRectHandler();
    EXPECT_NE(jsAccessibilityManager->getParentRectHandlerNew_, nullptr);
}

/**
* @tc.name: JsAccessibilityManager025
* @tc.desc: GetTransformRectInfoRelativeToWindow
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager025, TestSize.Level1)
{
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    ASSERT_NE(jsAccessibilityManager, nullptr);
    auto frameNode = FrameNode::CreateFrameNode(
        "frameNode", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    /**
     * @tc.steps: step2. test GetTransformRectInfoRelativeToWindow
     */
    auto rectInfo = jsAccessibilityManager->GetTransformRectInfoRelativeToWindow(frameNode, context);
    EXPECT_EQ(rectInfo.left, 100);
    EXPECT_EQ(rectInfo.top, 200);
}

/**
* @tc.name: JsAccessibilityManager026
* @tc.desc: GetTransformRectInfoRelativeToWindow
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager026, TestSize.Level1) {
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    ASSERT_NE(jsAccessibilityManager, nullptr);

    auto result = jsAccessibilityManager->GetTransformRectInfoRelativeToWindow(nullptr, context);
    // Verify that the result is default constructed when node is null
    EXPECT_EQ(result.left, 0);
    EXPECT_EQ(result.top, 0);
    EXPECT_EQ(result.scaleX, 1.0f);  // Assuming default scaleX is 1.0
    EXPECT_EQ(result.scaleY, 1.0f);  // Assuming default scaleY is 1.0
}

/**
* @tc.name: JsAccessibilityManager027
* @tc.desc: GetTransformRectInfoRelativeToWindow
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager027, TestSize.Level1) {
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    ASSERT_NE(jsAccessibilityManager, nullptr);
    auto frameNode = FrameNode::CreateFrameNode(
        "frameNode", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);

    auto result = jsAccessibilityManager->GetTransformRectInfoRelativeToWindow(frameNode, nullptr);
    
    // Verify that the result is default constructed when context is null
    EXPECT_EQ(result.left, 0);
    EXPECT_EQ(result.top, 0);
    EXPECT_EQ(result.scaleX, 1.0f);
    EXPECT_EQ(result.scaleY, 1.0f);
}

/**
* @tc.name: JsAccessibilityManager028
* @tc.desc: GetPagePathInPageNodes
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager028, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create jsAccessibilityManager and create test data.
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto frameNode1 = FrameNode::CreateFrameNode(
        "frameNode1", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    frameNode1->SetHostPageId(1);
    auto frameNode2 = FrameNode::CreateFrameNode(
        "frameNode2", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    frameNode2->SetHostPageId(2);
    std::vector<RefPtr<NG::FrameNode>> pageNodes = {frameNode1, frameNode2};
    /**
     * @tc.steps: step2. test Empty pagePaths
     */
    std::vector<std::string> pagePaths = {};
    EXPECT_EQ(jsAccessibilityManager->GetPagePathInPageNodes(1, pageNodes, pagePaths),
    jsAccessibilityManager->GetPagePath());
    /**
     * @tc.steps: step3. test Mismatched sizes of pageNodes and pagePaths
     */
    pagePaths = {"path1"};
    EXPECT_EQ(jsAccessibilityManager->GetPagePathInPageNodes(1, pageNodes, pagePaths),
        jsAccessibilityManager->GetPagePath());
    /**
     * @tc.steps: step4. test PageId not found in pageNodes
     */
    pagePaths = {"path1", "path2"};
    EXPECT_EQ(jsAccessibilityManager->GetPagePathInPageNodes(3, pageNodes, pagePaths),
        jsAccessibilityManager->GetPagePath());
    /**
     * @tc.steps: step5. test PageId found in pageNodes
     */
    pagePaths = {"path1", "path2"};
    EXPECT_EQ(jsAccessibilityManager->GetPagePathInPageNodes(2, pageNodes, pagePaths), "path2");
}

/**
* @tc.name: JsAccessibilityManager029
* @tc.desc: GetCurrentWindowPages
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager029, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create jsAccessibilityManager and create test data.
     */
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    std::vector<RefPtr<FrameNode>> pageNodes;
    std::vector<std::string> pagePaths;
    auto context = MockPipelineContext::GetCurrentContext();
    RefPtr<MockStageManager> stageManager = AceType::MakeRefPtr<MockStageManager>(stageNode);
    context->stageManager_ = stageManager;
    ASSERT_NE(context->stageManager_, nullptr);

    auto frameNode1 = FrameNode::CreateFrameNode(
        "frameNode1", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    frameNode1->SetHostPageId(1);
    auto frameNode2 = FrameNode::CreateFrameNode(
        "frameNode2", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    frameNode2->SetHostPageId(2);
    auto frameNode3 = FrameNode::CreateFrameNode(
        "frameNode3", ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    frameNode3->SetHostPageId(3);
    /**
     * @tc.steps: step2. test GetCurrentWindowPages get data empty
     */
    EXPECT_CALL(*stageManager, IsSplitMode()).Times(2).WillRepeatedly(Return(true));
    EXPECT_CALL(*stageManager, GetTopPagesWithTransition()).Times(2)
        .WillRepeatedly(Return(std::vector<RefPtr<FrameNode>>{}));
    EXPECT_CALL(*stageManager, GetTopPagePaths()).Times(2)
        .WillRepeatedly(Return(std::vector<std::string>{"path1", "path2"}));
    
    jsAccessibilityManager->GetCurrentWindowPages(context, pageNodes, pagePaths);
    ASSERT_EQ(pageNodes.size(), 0);
    ASSERT_EQ(pagePaths.size(), 0);

    /**
     * @tc.steps: step3. test GetCurrentWindowPages get data size 2
     */
    EXPECT_CALL(*stageManager, GetTopPagesWithTransition()).Times(2)
        .WillRepeatedly(Return(std::vector<RefPtr<FrameNode>>{frameNode1, frameNode2}));
    jsAccessibilityManager->GetCurrentWindowPages(context, pageNodes, pagePaths);
    ASSERT_EQ(pageNodes.size(), 2);
    ASSERT_EQ(pagePaths.size(), 2);

    /**
     * @tc.steps: step4. test GetCurrentWindowPages with IsSplitMode false
     */
    EXPECT_CALL(*stageManager, IsSplitMode()).WillOnce(Return(false));
    EXPECT_CALL(*stageManager, GetLastPageWithTransition()).Times(1)
        .WillOnce(Return(frameNode3));
    jsAccessibilityManager->GetCurrentWindowPages(context, pageNodes, pagePaths);
    ASSERT_EQ(pageNodes.size(), 3);
}

/**
* @tc.name: JsAccessibilityManager030
* @tc.desc: IsEventIgnoredByWorkMode
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager030, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create jsAccessibilityManager and create test data.
     */
    auto context = NG::PipelineContext::GetCurrentContext();
    MockJsAccessibilityManager mockJsManger;
    mockJsManger.SetPipelineContext(context);
    mockJsManger.Register(true);

    AccessibilityEvent event;
    event.nodeId = 1;
    AccessibilityWorkMode accessibilityWorkMode;
    accessibilityWorkMode.isTouchExplorationEnabled = false;
    /**
     * @tc.steps: step2. test IsEventIgnoredByWorkMode return true
     */
    event.type = AccessibilityEventType::FOCUS;
    EXPECT_CALL(mockJsManger, GenerateAccessibilityWorkMode())
        .WillRepeatedly(Return(accessibilityWorkMode));
    EXPECT_TRUE(mockJsManger.IsEventIgnoredByWorkMode(event));
    
    event.type = AccessibilityEventType::ELEMENT_INFO_CHANGE;
    EXPECT_TRUE(mockJsManger.IsEventIgnoredByWorkMode(event));

    event.type = AccessibilityEventType::TEXT_CHANGE;
    EXPECT_TRUE(mockJsManger.IsEventIgnoredByWorkMode(event));

    /**
     * @tc.steps: step3. test IsEventIgnoredByWorkMode return false
     */
    event.type = AccessibilityEventType::CLICK;
    EXPECT_FALSE(mockJsManger.IsEventIgnoredByWorkMode(event));

    accessibilityWorkMode.isTouchExplorationEnabled = true;
    EXPECT_CALL(mockJsManger, GenerateAccessibilityWorkMode())
        .WillOnce(Return(accessibilityWorkMode));
    EXPECT_FALSE(mockJsManger.IsEventIgnoredByWorkMode(event));
}

/**
 * @tc.name: ConvertActionTypeToBoolen001
 * @tc.desc: ConvertActionTypeToBoolen
 * @tc.type: FUNC
 */
 HWTEST_F(JsAccessibilityManagerTest, ConvertActionTypeToBoolen001, TestSize.Level1)
 {
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<MockJsAccessibilityManager>();
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto context = NG::PipelineContext::GetCurrentContext();
    int64_t elementId = 0;
    auto eventHub = frameNode->GetOrCreateEventHub<NG::EventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->GetOrCreateGestureEventHub();
    auto gesture = eventHub->GetGestureEventHub();
    ASSERT_NE(gesture, nullptr);
    std::string nodeName = "Click";
    frameNode->SetNodeName(nodeName);
    /**
    * @tc.steps: step2. test do without any willClick or didClick, expect return nodeName Click
    */
    jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLICK,
        frameNode, elementId, context);
    EXPECT_EQ(frameNode->GetNodeName(), nodeName);
    /**
    * @tc.steps: step3. test do with willClick or didClick, expect return nodeName WillClick_Click_DidClick
    */
    NG::UIObserverHandler::WillClickHandleFunc willClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {
            if (frameNode) {
                std::string willClick = "WillClick_" + frameNode->GetNodeName();
                frameNode->SetNodeName(willClick);
            }
        };
    ASSERT_NE(willClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(willClickHandleFunc);

    NG::UIObserverHandler::DidClickHandleFunc didClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {
            if (frameNode) {
                std::string didClick = frameNode->GetNodeName() + "_DidClick";
                frameNode->SetNodeName(didClick);
            }
        };
    ASSERT_NE(didClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(didClickHandleFunc);
    jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLICK,
        frameNode, elementId, context);
    EXPECT_EQ(frameNode->GetNodeName(), "WillClick_Click_DidClick");
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(nullptr);
}

/**
 * @tc.name: ConvertActionTypeToBoolen002
 * @tc.desc: ConvertActionTypeToBoolen
 * @tc.type: FUNC
 */
 HWTEST_F(JsAccessibilityManagerTest, ConvertActionTypeToBoolen002, TestSize.Level1)
 {
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<MockJsAccessibilityManager>();
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto context = NG::PipelineContext::GetCurrentContext();
    int64_t elementId = 0;
    auto eventHub = frameNode->GetOrCreateEventHub<NG::EventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->GetOrCreateGestureEventHub();
    auto gesture = eventHub->GetGestureEventHub();
    ASSERT_NE(gesture, nullptr);
    std::string nodeName = "Click";
    frameNode->SetNodeName(nodeName);
    /**
    * @tc.steps: step2. test only with willClick, expect return nodeName WillClick_Click
    */
    NG::UIObserverHandler::WillClickHandleFunc willClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {
            if (frameNode) {
                std::string willClick = "WillClick_" + frameNode->GetNodeName();
                frameNode->SetNodeName(willClick);
            }
        };
    ASSERT_NE(willClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(willClickHandleFunc);

    NG::UIObserverHandler::DidClickHandleFunc didClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {};
    ASSERT_NE(didClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(didClickHandleFunc);
    jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLICK,
        frameNode, elementId, context);
    EXPECT_EQ(frameNode->GetNodeName(), "WillClick_Click");
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(nullptr);
}

/**
 * @tc.name: ConvertActionTypeToBoolen003
 * @tc.desc: ConvertActionTypeToBoolen
 * @tc.type: FUNC
 */
 HWTEST_F(JsAccessibilityManagerTest, ConvertActionTypeToBoolen003, TestSize.Level1)
 {
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<MockJsAccessibilityManager>();
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto context = NG::PipelineContext::GetCurrentContext();
    int64_t elementId = 0;
    auto eventHub = frameNode->GetOrCreateEventHub<NG::EventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->GetOrCreateGestureEventHub();
    auto gesture = eventHub->GetGestureEventHub();
    ASSERT_NE(gesture, nullptr);
    std::string nodeName = "Click";
    frameNode->SetNodeName(nodeName);

    /**
    * @tc.steps: step3. test do with didClick, expect return nodeName Click_DidClick
    */
    NG::UIObserverHandler::WillClickHandleFunc willClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {};
    ASSERT_NE(willClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(willClickHandleFunc);

    NG::UIObserverHandler::DidClickHandleFunc didClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {
            if (frameNode) {
                std::string didClick = frameNode->GetNodeName() + "_DidClick";
                frameNode->SetNodeName(didClick);
            }
        };
    ASSERT_NE(didClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(didClickHandleFunc);
    jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLICK,
        frameNode, elementId, context);
    EXPECT_EQ(frameNode->GetNodeName(), "Click_DidClick");
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(nullptr);
}

/**
 * @tc.name: ConvertActionTypeToBoolen004
 * @tc.desc: Test clear currentFocusNodeId when elementId equals to currentFocusNodeId
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, ConvertActionTypeToBoolen004, TestSize.Level1)
{
    int64_t elementId = 2LL;
    auto context = NG::PipelineContext::GetCurrentContext();
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    ASSERT_NE(frameNode, nullptr);
    ASSERT_NE(frameNode->GetRenderContext(), nullptr);

    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    jsAccessibilityManager->currentFocusNodeId_ = 2LL;
    jsAccessibilityManager->currentFocusVirtualNodeParentId_ = 3LL;

    bool ret = jsAccessibilityManager->ConvertActionTypeToBoolen(
        ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, frameNode, elementId, context);
    EXPECT_EQ(jsAccessibilityManager->currentFocusNodeId_, -1LL);
    EXPECT_EQ(jsAccessibilityManager->currentFocusVirtualNodeParentId_, 3LL);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ConvertActionTypeToBoolen005
 * @tc.desc: Test clear currentFocusNodeId when elementId not equals to currentFocusNodeId
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, ConvertActionTypeToBoolen005, TestSize.Level1)
{
    int64_t elementId = 6LL;
    auto context = NG::PipelineContext::GetCurrentContext();
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    ASSERT_NE(frameNode, nullptr);
    ASSERT_NE(frameNode->GetRenderContext(), nullptr);

    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    jsAccessibilityManager->currentFocusNodeId_ = 4LL;
    jsAccessibilityManager->currentFocusVirtualNodeParentId_ = 5LL;

    bool ret = jsAccessibilityManager->ConvertActionTypeToBoolen(
        ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, frameNode, elementId, context);
    EXPECT_EQ(jsAccessibilityManager->currentFocusNodeId_, 4LL);
    EXPECT_EQ(jsAccessibilityManager->currentFocusVirtualNodeParentId_, 5LL);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ConvertActionTypeToBoolen006
 * @tc.desc: Test focusing action
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, ConvertActionTypeToBoolen006, TestSize.Level1)
{
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    int64_t elementId = 0;
    auto jsAccessibilityManager = AceType::MakeRefPtr<MockJsAccessibilityManager>();
    auto frameNode = FrameNode::CreateFrameNode("framenode", 1, AceType::MakeRefPtr<Pattern>(), true);
    auto context = NG::PipelineContext::GetCurrentContext();
    auto eventHub = frameNode->GetOrCreateEventHub<NG::EventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->GetOrCreateGestureEventHub();
    auto gesture = eventHub->GetGestureEventHub();
    ASSERT_NE(gesture, nullptr);
    std::string nodeName = "Click";
    frameNode->SetNodeName(nodeName);

    /**
    * @tc.steps: step2. test with nothing, expect return nodeName Click
    */
    NG::UIObserverHandler::WillClickHandleFunc willClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {};
    ASSERT_NE(willClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(willClickHandleFunc);

    NG::UIObserverHandler::DidClickHandleFunc didClickHandleFunc = [](
        AbilityContextInfo&, const GestureEvent&, const ClickInfo&,
        const RefPtr<FrameNode>& frameNode) -> void {};
    ASSERT_NE(didClickHandleFunc, nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(didClickHandleFunc);
    jsAccessibilityManager->ConvertActionTypeToBoolen(ActionType::ACCESSIBILITY_ACTION_CLICK,
        frameNode, elementId, context);
    EXPECT_EQ(frameNode->GetNodeName(), "Click");
    NG::UIObserverHandler::GetInstance().SetWillClickFunc(nullptr);
    NG::UIObserverHandler::GetInstance().SetDidClickFunc(nullptr);
}

/**
* @tc.name: JsAccessibilityManager031
* @tc.desc: IsUpdateWindowSceneInfo
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager031, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct jsAccessibilityManager
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    
    /**
     * @tc.steps: step2. node is nullptr, expect return false
     */
    RefPtr<FrameNode> node = nullptr;
    NG::WindowSceneInfo windowSceneInfo;
    bool ret = jsAccessibilityManager->IsUpdateWindowSceneInfo(node, windowSceneInfo);
    EXPECT_FALSE(ret);
}

/**
* @tc.name: JsAccessibilityManager032
* @tc.desc: IsUpdateWindowSceneInfo
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager032, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct jsAccessibilityManager, test node
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    
    auto rootNode = FrameNode::CreateFrameNode(V2::WINDOW_SCENE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), true);
    auto frameNode1 = FrameNode::CreateFrameNode(V2::WINDOW_SCENE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), false);
    rootNode->AddChild(frameNode1);
    auto frameNode2 = FrameNode::CreateFrameNode("frameNode",
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), false);
    frameNode1->AddChild(frameNode2);
    NG::WindowSceneInfo windowSceneInfo;
    /**
     * @tc.steps: step2. check root node, expect return false
     */
    bool ret1 = jsAccessibilityManager->IsUpdateWindowSceneInfo(rootNode, windowSceneInfo);
    EXPECT_FALSE(ret1);
    
    bool ret2 = jsAccessibilityManager->IsUpdateWindowSceneInfo(frameNode2, windowSceneInfo);
    EXPECT_FALSE(ret2);
}

/**
* @tc.name: JsAccessibilityManager033
* @tc.desc: IsUpdateWindowSceneInfo
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager033, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct jsAccessibilityManager, test node
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    
    auto rootNode = FrameNode::CreateFrameNode("frameNode0",
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), true);
    auto frameNode1 = FrameNode::CreateFrameNode("frameNode1",
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), false);
    rootNode->AddChild(frameNode1);
    NG::WindowSceneInfo windowSceneInfo;
    /**
     * @tc.steps: step2. check frameNode1, expect return false
     */
    bool ret1 = jsAccessibilityManager->IsUpdateWindowSceneInfo(frameNode1, windowSceneInfo);
    EXPECT_FALSE(ret1);
}

/**
* @tc.name: JsAccessibilityManager034
* @tc.desc: UpdateElementInfosTreeId
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager034, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct jsAccessibilityManager, test node
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    /**
     * @tc.steps: step2. check empty infos
     */
    std::list<Accessibility::AccessibilityElementInfo> infos;
    jsAccessibilityManager->UpdateElementInfosTreeId(infos);
    EXPECT_TRUE(infos.empty());

    /**
     * @tc.steps: step3. check info with treeId < 0
     */
    Accessibility::AccessibilityElementInfo info;
    info.SetBelongTreeId(0);
    info.SetAccessibilityId(100);
    infos.push_back(info);
    jsAccessibilityManager->UpdateElementInfosTreeId(infos);

    /**
     * @tc.steps: step4. check info with treeId > 0
     */
    infos.clear();
    info.SetBelongTreeId(1);
    info.SetAccessibilityId(100);
    info.SetParent(10);
    infos.push_back(info);
    jsAccessibilityManager->UpdateElementInfosTreeId(infos);
}
/**
* @tc.name: JsAccessibilityManager035
* @tc.desc: UpdateVirtualNodeChildAccessibilityElementInfo
* @tc.type: FUNC
*/
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager035, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct jsAccessibilityManager, test node
     */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    ASSERT_NE(jsAccessibilityManager, nullptr);
    auto context = NG::PipelineContext::GetCurrentContext();
    jsAccessibilityManager->SetPipelineContext(context);
    jsAccessibilityManager->Register(true);

    Framework::CommonProperty commonProperty;
    commonProperty.innerWindowId = 10;
    AccessibilityElementInfo nodeParentInfo;
    AccessibilityElementInfo nodeInfo;
    auto frameNode = FrameNode::CreateFrameNode("frameNode",
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), false);
    
    /**
     * @tc.steps: step2. test with frameNode nullptr
     */
    jsAccessibilityManager->UpdateVirtualNodeChildAccessibilityElementInfo(nullptr,
        commonProperty, nodeParentInfo, nodeInfo, context);
    
    /**
     * @tc.steps: step3. test with frameNode
     */
    jsAccessibilityManager->UpdateVirtualNodeChildAccessibilityElementInfo(frameNode,
        commonProperty, nodeParentInfo, nodeInfo, context);
    EXPECT_EQ(nodeInfo.GetInnerWindowId(), 10);

    /**
     * @tc.steps: step3. test UpdateVirtualNodeAccessibilityElementInfo
     */;
    AccessibilityElementInfo nodeInfoNew;
    auto rootNode = FrameNode::CreateFrameNode("rootNode",
    ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>(), true);
    rootNode->AddChild(frameNode);
    jsAccessibilityManager->UpdateVirtualNodeAccessibilityElementInfo(nullptr, nullptr,
        commonProperty, nodeInfoNew, context);
    jsAccessibilityManager->UpdateVirtualNodeAccessibilityElementInfo(rootNode, nullptr,
        commonProperty, nodeInfoNew, context);
    jsAccessibilityManager->UpdateVirtualNodeAccessibilityElementInfo(rootNode, frameNode,
        commonProperty, nodeInfoNew, context);
    EXPECT_EQ(nodeInfoNew.GetInnerWindowId(), 10);
}

/**
 * @tc.name: JsAccessibilityManager036
 * @tc.desc: UpdateVirtualNodeFocus
 * @tc.type: FUNC
 */
 HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager036, TestSize.Level1)
 {
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto frameNode = FrameNode::CreateFrameNode("framenode", ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<Pattern>(), false);
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();

    /**
    * @tc.steps: step2. test UpdateVirtualNodeFocus
    */
    Accessibility::AccessibilityElementInfo nodeInfo;
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    frameNode->SetGeometryNode(geometryNode);
    frameNode->SetAccessibilityNodeVirtual();
    jsAccessibilityManager->SaveLast(0, frameNode);
    /**
    * @tc.steps: step3. expect GetAccessibilityFocusState true.
    */
    jsAccessibilityManager->UpdateVirtualNodeFocus();
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    ASSERT_NE(accessibilityProperty, nullptr);
    EXPECT_FALSE(accessibilityProperty->GetAccessibilityFocusState());
 }

/**
 * @tc.name: JsAccessibilityManager037
 * @tc.desc: UpdateAccessibilityNextFocusIdMap
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager037, TestSize.Level1)
{
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    /**
    * @tc.steps: step2. test UpdateAccessibilityNextFocusIdMap expect update success
    */
    const int32_t containerIdA = 1;
    const std::string nextFocusKeyA = "test_key";
    const int64_t preAccessibilityIdA = 100;
    jsAccessibilityManager->UpdateAccessibilityNextFocusIdMap(containerIdA, nextFocusKeyA, preAccessibilityIdA);
    EXPECT_EQ(jsAccessibilityManager->nextFocusMapWithSubWindow_.size(), 1);
    /**
    * @tc.steps: step3. test UpdateAccessibilityNextFocusIdMap expect update fail
    */
    const int32_t containerIdB = 2;
    const std::string nextFocusKeyB = "";
    const int64_t preAccessibilityIdB = 200;
    jsAccessibilityManager->UpdateAccessibilityNextFocusIdMap(containerIdA, nextFocusKeyA, preAccessibilityIdA);
    EXPECT_EQ(jsAccessibilityManager->nextFocusMapWithSubWindow_.size(), 1);
    /**
    * @tc.steps: step4. test UpdateAccessibilityNextFocusIdMap expect update contained data
    */
    const int32_t containerIdC = 3;
    const std::string nextFocusKeyC = "key3";
    const int64_t preAccessibilityIdC = 301;
    jsAccessibilityManager->UpdateAccessibilityNextFocusIdMap(containerIdC, "key1", 300);
    jsAccessibilityManager->UpdateAccessibilityNextFocusIdMap(containerIdC, nextFocusKeyC, preAccessibilityIdC);
    EXPECT_EQ(jsAccessibilityManager->nextFocusMapWithSubWindow_[containerIdC][nextFocusKeyC], preAccessibilityIdC);
    EXPECT_EQ(jsAccessibilityManager->nextFocusMapWithSubWindow_.size(), 2);
}

/**
 * @tc.name: JsAccessibilityManager038
 * @tc.desc: IsSendAccessibilityEvent
 * @tc.type: FUNC
 */
HWTEST_F(JsAccessibilityManagerTest, JsAccessibilityManager038, TestSize.Level1)
{
    /**
    * @tc.steps: step1. construct JsAccessibilityManager
    */
    auto jsAccessibilityManager = AceType::MakeRefPtr<Framework::JsAccessibilityManager>();
    auto pipelineContext = MockContainer::Current()->GetPipelineContext();
    ASSERT_NE(pipelineContext, nullptr);
    jsAccessibilityManager->SetPipelineContext(pipelineContext);
    /**
    * @tc.steps: step2. test IsSendAccessibilityEvent accessibilityEvent type with default value
    */
    AccessibilityEvent accessibilityEvent;
    EXPECT_TRUE(jsAccessibilityManager->IsSendAccessibilityEvent(accessibilityEvent));
}
} // namespace OHOS::Ace::NG
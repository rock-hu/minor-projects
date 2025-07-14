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
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/render/mock_paragraph.h"
#include "text_base.h"

#include "core/components/select/select_theme.h"
#include "core/components/text_overlay/text_overlay_theme.h"
#include "core/components_ng/pattern/flex/flex_layout_pattern.h"
#include "core/components_ng/pattern/rich_editor/paragraph_manager.h"
#include "core/components_ng/pattern/rich_editor_drag/preview_menu_controller.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string APP_NAME_MAP = "app.name";
const std::string CALENDAR_ABILITY_NAME = "AgendaPreviewUIExtensionAbility";
const std::string UIEXTENSION_PARAM = "ability.want.params.uiExtensionType";
const std::string UIEXTENSION_PARAM_VALUE = "sys/commonUI";
constexpr Dimension PADDING_SIZE = 12.0_vp;
} // namespace
class PreviewMenuControllerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        MockPipelineContext::SetUp();
    }

    static void TearDownTestCase()
    {
        MockPipelineContext::TearDown();
    }

    void SetUp() override
    {
        // Initialize test environment
    }

    void TearDown() override
    {
        // Clean up
    }
};

/**
 * @tc.name: PreviewMenuControllerConstructorTest
 * @tc.desc: Test PreviewMenuController constructor initializes correctly
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, PreviewMenuControllerConstructorTest, TestSize.Level1)
{
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    auto frameNode = FrameNode::CreateFrameNode("Test", 1, pattern);
    MockPipelineContext::SetUp();
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    PreviewMenuController controller(AceType::WeakClaim(AceType::RawPtr(pattern)));

    EXPECT_EQ(controller.menuParam_.type, MenuType::CONTEXT_MENU);
    EXPECT_EQ(controller.menuParam_.contextMenuRegisterType, ContextMenuRegisterType::CUSTOM_TYPE);
    EXPECT_EQ(controller.menuParam_.previewMode, MenuPreviewMode::CUSTOM);
    EXPECT_TRUE(controller.menuParam_.isShowHoverImage);
    EXPECT_NE(controller.menuParam_.onDisappear, nullptr);
}

/**
 * @tc.name: CreateAIEntityMenuTest
 * @tc.desc: Test CreateAIEntityMenu creates menu node correctly
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, CreateAIEntityMenuTest, TestSize.Level1)
{
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    auto frameNode = FrameNode::CreateFrameNode("Test", 1, pattern);
    PreviewMenuController controller(AceType::WeakClaim(AceType::RawPtr(pattern)));
    controller.CreateAIEntityMenu();
    auto stack = ViewStackProcessor::GetInstance()->Finish();
    EXPECT_NE(stack, nullptr);
    EXPECT_EQ(stack->GetTag(), V2::FLEX_ETS_TAG);
}

/**
 * @tc.name: GetDisappearCallbackTest
 * @tc.desc: Test GetDisappearCallback returns valid callback
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, GetDisappearCallbackTest, TestSize.Level1)
{
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    // Mock target node
    auto targetNode = FrameNode::CreateFrameNode("Test", 1, AceType::MakeRefPtr<TextPattern>());
    pattern->dragNode_ = targetNode;
    PreviewMenuController controller(AceType::WeakClaim(AceType::RawPtr(pattern)));

    auto callback = controller.GetDisappearCallback();
    callback();
    EXPECT_NE(callback, nullptr);

    EXPECT_EQ(controller.menuParam_.isShow, false);
    EXPECT_EQ(controller.isShow_, false);
}

/**
 * @tc.name: GetLinkingCallbackTest
 * @tc.desc: Test GetLinkingCallback returns valid callback for different app names
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, GetLinkingCallbackTest, TestSize.Level1)
{
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    PreviewMenuController controller(AceType::WeakClaim(AceType::RawPtr(pattern)));

    auto callback = controller.GetLinkingCallback(APP_NAME_MAP);
    EXPECT_NE(callback, nullptr);

    // Should test with mock PipelineContext to verify StartAbilityOnInstallAppInStore is called
}

/**
 * @tc.name: CreatePreviewMenuTest
 * @tc.desc: Test CreatePreviewMenu creates preview node correctly
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, CreatePreviewMenuTest, TestSize.Level1)
{
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    PreviewMenuController controller(AceType::WeakClaim(AceType::RawPtr(pattern)));

    auto targetNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, 1, AceType::MakeRefPtr<TextPattern>());
    auto* stack = ViewStackProcessor::GetInstance();
    auto beforeSize = stack->elementsStack_.size();

    // Test with different TextDataDetectType
    pattern->textSelector_.aiStart = 0;
    pattern->textSelector_.aiEnd = 5;

    std::map<int32_t, AISpan> aiSpanMap;
    AISpan aiSpan1;
    aiSpan1.start = 0;
    aiSpan1.end = 5;
    aiSpan1.content = SPAN_PHONE;
    aiSpan1.type = TextDataDetectType::PHONE_NUMBER;
    aiSpanMap[0] = aiSpan1;
    pattern->dataDetectorAdapter_->aiSpanMap_ = aiSpanMap;
    auto textForAI = u"Test1234";
    pattern->dataDetectorAdapter_->textForAI_ = textForAI;
    pattern->dataDetectorAdapter_->enablePreviewMenu_ = true;
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    auto func = controller.GetDisappearCallback();
    controller.CreatePreviewMenu(aiSpan1.type, aiSpan1.content, std::move(func));

    auto afterSize = stack->elementsStack_.size();
    EXPECT_GT(afterSize, beforeSize);
    auto node = stack->Finish();
    EXPECT_NE(node, nullptr);
}

/**
 * @tc.name: CreateLinkingNodeTest
 * @tc.desc: Test CreateLinkingNode creates node with correct properties for different types
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, CreateLinkingNodeTest, TestSize.Level1)
{
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    PreviewMenuController controller(AceType::WeakClaim(AceType::RawPtr(pattern)));

    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<TextOverlayTheme>()));

    // Test with URL type
    auto urlNode = controller.CreatePreview(TextDataDetectType::URL);
    EXPECT_NE(urlNode, nullptr);

    // Test with PHONE_NUMBER type
    auto node = controller.CreatePreview(TextDataDetectType::PHONE_NUMBER);
    EXPECT_NE(node, nullptr);

    auto flexLayoutProperty = node->GetLayoutProperty<FlexLayoutProperty>();

    // Verify flex layout properties
    EXPECT_EQ(flexLayoutProperty->GetFlexDirection(), FlexDirection::ROW);
    EXPECT_EQ(flexLayoutProperty->GetCrossAxisAlign(), FlexAlign::CENTER);

    // Verify padding
    const auto& padding = flexLayoutProperty->GetPaddingProperty();
    EXPECT_NE(padding, nullptr);
    EXPECT_EQ(padding->left.value_or(CalcLength(Dimension())), CalcLength(PADDING_SIZE));
    EXPECT_EQ(padding->right.value_or(CalcLength(Dimension())), CalcLength(PADDING_SIZE));

    // Test with ADDRESS type
    auto addrNode = controller.CreatePreview(TextDataDetectType::ADDRESS);
    EXPECT_NE(addrNode, nullptr);
}

/**
 * @tc.name: CreateContactErrorNodeTest001
 * @tc.desc: Test layout properties are correctly set
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, CreateContactErrorNodeTest001, TestSize.Level1)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::FLEX_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<FlexLayoutPattern>(false); });
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    PreviewMenuController::CreateContactErrorNode(frameNode, "Test", nullptr);
    auto avatarNode = frameNode->GetChildByIndex(0);
    EXPECT_NE(avatarNode, nullptr);
    auto textNode = frameNode->GetChildByIndex(1);
    EXPECT_NE(textNode, nullptr);
}

/**
 * @tc.name:  GetErrorCallback001
 * @tc.desc: Test GetErrorCallback creates node with correct properties for different types
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, GetErrorCallbackTest001, TestSize.Level1)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto previewNode = FrameNode::GetOrCreateFrameNode(
        V2::COLUMN_ETS_TAG, stack->ClaimNodeId(), []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
    auto callback = []() {};
    const std::string testContent = "test_content";
    auto errorCallback = PreviewMenuController::GetErrorCallback(
        previewNode, TextDataDetectType::PHONE_NUMBER, testContent, std::move(callback));

    // 模拟调用errorCallback
    errorCallback(0, "test_error", "test_message");
    // 验证previewNode挂孩子节点
    EXPECT_FALSE(previewNode->GetChildren().empty());
}

/**
 * @tc.name: DateTimeClickCallbackTest001
 * @tc.desc: Test DATE_TIME click callback triggers ability correctly
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, DateTimeClickCallbackTest001, TestSize.Level1)
{
    // 执行测试
    auto* stack = ViewStackProcessor::GetInstance();
    auto previewNode = FrameNode::GetOrCreateFrameNode(
        V2::COLUMN_ETS_TAG, stack->ClaimNodeId(), []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
    auto pipeline = previewNode->GetContext();
    ASSERT_NE(pipeline, nullptr);

    std::map<std::string, std::string> aiParams = { { "date", "2023-01-01" } };
    PreviewMenuController::PreviewNodeClickCallback(TextDataDetectType::DATE_TIME, previewNode, aiParams);

    // 触发点击事件
    auto gestureHub = previewNode->GetOrCreateGestureEventHub();
    GestureEvent dummyEvent;
    auto click = gestureHub->GetClickEvent();
    EXPECT_NE(click, nullptr);
}

/**
 * @tc.name: CreateWantConfigDateTimeTest001
 * @tc.desc: Test CreateWantConfig sets correct parameters for DATE_TIME type
 * @tc.type: FUNC
 */
HWTEST_F(PreviewMenuControllerTest, CreateWantConfigDateTimeTest001, TestSize.Level1)
{
    // 1. 准备测试数据
    TextDataDetectType type = TextDataDetectType::DATE_TIME;
    std::string bundleName;
    std::string abilityName;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> AIparams = { { "date", "2023-01-01" }, { "time", "15:30" } };

    // 2. 执行测试
    PreviewMenuController::CreateWantConfig(type, bundleName, abilityName, params, AIparams);

    // 3. 验证结果
    // 验证 abilityName
    EXPECT_EQ(abilityName, CALENDAR_ABILITY_NAME);

    // 验证基础参数
    EXPECT_EQ(params[UIEXTENSION_PARAM], UIEXTENSION_PARAM_VALUE);

    // 验证 AIparams 合并
    EXPECT_EQ(params["date"], "2023-01-01");
    EXPECT_EQ(params["time"], "15:30");

    // 验证参数总数 = 基础参数 + AIparams
    EXPECT_EQ(params.size(), 3);
}

/**
 * @tc.name: GetCopyAndSelectableWhenTextEffect001
 * @tc.desc: Verify copy and selectable status when textEffect_ is not null
 * @tc.require: AR000H0F6A
 */
HWTEST_F(PreviewMenuControllerTest, GetCopyAndSelectableWhenTextEffect001, TestSize.Level1)
{
    // 1. Prepare textPattern with textEffect_
    auto textPattern = AceType::MakeRefPtr<TextPattern>();
    auto frameNode = FrameNode::CreateFrameNode("Test", 1, textPattern);

    // 2. Set different copyOption modes (should be overridden by textEffect_)
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    
    // Case 1: copyOption = InApp (should return true)
    textPattern->copyOption_ = CopyOptions::InApp;
    auto result1 = textPattern->GetCopyAndSelectable();
    EXPECT_TRUE(result1.first);  // isShowCopy

    // Case 2: copyOption = InApp (should still return false due to textEffect_)
    textPattern->textEffect_ = TextEffect::CreateTextEffect(); // Set textEffect_
    auto result2 = textPattern->GetCopyAndSelectable();
    EXPECT_FALSE(result2.first);
}
} // namespace OHOS::Ace::NG
/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "test/unittest/core/pattern/rich_editor/rich_editor_common_test_ng.h"
#include "test/mock/core/common/mock_udmf.h"
#include "test/mock/core/render/mock_paragraph.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/base/mock_task_executor.h"
#include "core/components_ng/pattern/rich_editor/rich_editor_theme.h"
#include "core/components_ng/pattern/rich_editor/rich_editor_model_ng.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
int32_t testOnReadyEvent = 0;
int32_t testAboutToIMEInput = 0;
int32_t testOnIMEInputComplete = 0;
int32_t testAboutToDelete = 0;
int32_t testOnDeleteComplete = 0;
} // namespace

class RichEditorKeyboardShortcutTestNg : public RichEditorCommonTestNg {
public:
    void SetUp() override;
    void TearDown() override;
    static void TearDownTestSuite();
};

void RichEditorKeyboardShortcutTestNg::SetUp()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
    MockContainer::Current()->taskExecutor_ = AceType::MakeRefPtr<MockTaskExecutor>();
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    richEditorNode_ = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    richEditorPattern->InitScrollablePattern();
    richEditorPattern->SetRichEditorController(AceType::MakeRefPtr<RichEditorController>());
    richEditorPattern->GetRichEditorController()->SetPattern(AceType::WeakClaim(AceType::RawPtr(richEditorPattern)));
    richEditorPattern->CreateNodePaintMethod();
    richEditorNode_->GetGeometryNode()->SetContentSize({});
}

void RichEditorKeyboardShortcutTestNg::TearDown()
{
    richEditorNode_ = nullptr;
    testOnReadyEvent = 0;
    testAboutToIMEInput = 0;
    testOnIMEInputComplete = 0;
    testAboutToDelete = 0;
    testOnDeleteComplete = 0;
    MockParagraph::TearDown();
}

void RichEditorKeyboardShortcutTestNg::TearDownTestSuite()
{
    TestNG::TearDownTestSuite();
}

/**
 * @tc.name: OnKeyEvent001
 * @tc.desc: test OnKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, OnKeyEvent001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    KeyEvent keyE;

    keyE.action = KeyAction::DOWN;
    keyE.code = KeyCode::KEY_TAB;
    EXPECT_FALSE(richEditorPattern->OnKeyEvent(keyE));

    // 2012 2015
    keyE.action = KeyAction::DOWN;
    std::vector<KeyCode> eventCodes = {
        KeyCode::KEY_DPAD_LEFT,
        KeyCode::KEY_DPAD_UP,
        KeyCode::KEY_DPAD_RIGHT,
        KeyCode::KEY_DPAD_DOWN,
    };
    std::vector<KeyCode> presscodes = {};
    keyE.pressedCodes = presscodes;
    for (auto eventCode : eventCodes) {
        keyE.pressedCodes.clear();
        keyE.pressedCodes.emplace_back(eventCode);
        keyE.code = eventCode;
        auto ret = richEditorPattern->OnKeyEvent(keyE);
        EXPECT_TRUE(ret) << "KeyCode: " + std::to_string(static_cast<int>(eventCode));
    }
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts001 about cursor move
 * @tc.desc: test the cursor move line start
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    /**
     * @tc.steps: step2. get richEditor controller
     */
    auto richEditorController = richEditorPattern->GetRichEditorController();
    ASSERT_NE(richEditorController, nullptr);
    /**
     * @tc.steps: step2. add text span
     */
    richEditorPattern->paragraphs_.minParagraphFontSize = 19.0;
    EXPECT_EQ(richEditorPattern->paragraphs_.minParagraphFontSize.value(), 19.0);
    TextSpanOptions textOptions;
    textOptions.value = INIT_VALUE_3;
    richEditorController->AddTextSpan(textOptions);
    EXPECT_EQ(textOptions.value.length(), richEditorPattern->GetTextContentLength());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->CursorMove(CaretMoveIntent::Left);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 19);
    richEditorPattern->CursorMove(CaretMoveIntent::LeftWord);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 19);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->CursorMove(CaretMoveIntent::RightWord);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 20);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->CursorMove(CaretMoveIntent::ParagraghBegin);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 0);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->CursorMove(CaretMoveIntent::ParagraghEnd);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), textOptions.value.length());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->CursorMove(CaretMoveIntent::Home);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 0);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->CursorMove(CaretMoveIntent::End);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), textOptions.value.length());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->CursorMove(CaretMoveIntent::LineBegin);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 0);
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts002 about cursor move
 * @tc.desc: test the cursor move line end and down
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->paragraphs_.minParagraphFontSize = 19.0;
    EXPECT_EQ(richEditorPattern->paragraphs_.minParagraphFontSize.value(), 19.0);
    AddSpan(INIT_VALUE_1);
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    TestParagraphItem testParagraphItem = {
        .start = 0, .end = 6, .indexOffsetMap = { { 0, Offset(0, 5) }, { 6, Offset(50, 0) } }
    };
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 6 });
    CaretMetricsF metricsDown;
    CaretMetricsF metricsUp;
    for (const auto& [index, offset] : testParagraphItem.indexOffsetMap) {
        metricsDown.offset.SetX(offset.GetX());
        metricsDown.offset.SetY(offset.GetY());
        metricsUp.offset.SetX(offset.GetX());
        metricsUp.offset.SetY(offset.GetY());
        EXPECT_CALL(*paragraph, GetGlyphIndexByCoordinate(_, _)).WillRepeatedly(Return(6));
        EXPECT_CALL(*paragraph, GetMaxWidth).WillRepeatedly(Return(150));
        EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
        EXPECT_CALL(*paragraph, ComputeOffsetForCaretDownstream(index, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(metricsDown), Return(true)));
        EXPECT_CALL(*paragraph, ComputeOffsetForCaretUpstream(index, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(metricsUp), Return(true)));
    }
    richEditorPattern->SetCaretPosition(0);
    richEditorPattern->CursorMove(CaretMoveIntent::LineEnd);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 6);
    richEditorPattern->SetCaretPosition(0);
    richEditorPattern->HandleSelect(CaretMoveIntent::LineEnd);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextEnd(), 6);
    richEditorPattern->SetCaretPosition(0);
    richEditorPattern->HandleSelect(CaretMoveIntent::Down);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextEnd(), 6);
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts101 about Handle select
 * @tc.desc: test the select move position
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts101, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    /**
     * @tc.steps: step2. get richEditor controller
     */
    auto richEditorController = richEditorPattern->GetRichEditorController();
    ASSERT_NE(richEditorController, nullptr);
    richEditorPattern->paragraphs_.minParagraphFontSize = 19.0;
    EXPECT_EQ(richEditorPattern->paragraphs_.minParagraphFontSize.value(), 19.0);
    /**
     * @tc.steps: step2. add text span
     */
    TextSpanOptions textOptions;
    textOptions.value = INIT_VALUE_3;
    richEditorController->AddTextSpan(textOptions);
    EXPECT_EQ(textOptions.value.length(), richEditorPattern->GetTextContentLength());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleSelect(CaretMoveIntent::Left);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextStart(), 19);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleSelect(CaretMoveIntent::Right);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextEnd(), 21);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleSelect(CaretMoveIntent::LeftWord);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextStart(), 19);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleSelect(CaretMoveIntent::RightWord);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextEnd(), 21);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleSelect(CaretMoveIntent::ParagraghBegin);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextStart(), 0);
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleSelect(CaretMoveIntent::ParagraghEnd);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextEnd(), textOptions.value.length());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleSelect(CaretMoveIntent::LineBegin);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextStart(), 0);
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts101 about Handle select
 * @tc.desc: test the select move position, up
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts102, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    auto layoutAlgorithm = AceType::DynamicCast<RichEditorLayoutAlgorithm>(richEditorPattern->CreateLayoutAlgorithm());
    ASSERT_NE(richEditorPattern, nullptr);
    /**
     * @tc.steps: step2. get richEditor controller
     */
    auto richEditorController = richEditorPattern->GetRichEditorController();
    ASSERT_NE(richEditorController, nullptr);
    /**
     * @tc.steps: step2. add text span
     */
    TextStyle style;
    style.SetTextColor(TEXT_COLOR_VALUE);
    style.SetFontSize(FONT_SIZE_VALUE);
    style.SetFontStyle(ITALIC_FONT_STYLE_VALUE);
    style.SetFontWeight(FONT_WEIGHT_VALUE);
    style.SetFontFamilies(FONT_FAMILY_VALUE);
    style.SetTextDecoration(TEXT_DECORATION_VALUE);
    style.SetTextDecorationColor(TEXT_DECORATION_COLOR_VALUE);
    TextSpanOptions textOptions;
    textOptions.value = INIT_VALUE_3;
    textOptions.style = style;
    richEditorController->AddTextSpan(textOptions);
    EXPECT_EQ(textOptions.value.length(), richEditorPattern->GetTextContentLength());
    richEditorPattern->SetCaretPosition(20);
    EXPECT_EQ(richEditorPattern->caretPosition_, 20);
    richEditorPattern->paragraphs_.minParagraphFontSize = style.GetFontSize().ConvertToPx();
    richEditorPattern->HandleSelect(CaretMoveIntent::Up);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextStart(), 0);
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts201 about Handle delete comb
 * @tc.desc: test the delete comb
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts201, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    /**
     * @tc.steps: step2. get richEditor controller
     */
    auto richEditorController = richEditorPattern->GetRichEditorController();
    ASSERT_NE(richEditorController, nullptr);
    /**
     * @tc.steps: step2. add text span
     */
    TextSpanOptions textOptions;
    textOptions.value = INIT_VALUE_3;
    richEditorController->AddTextSpan(textOptions);
    EXPECT_EQ(textOptions.value.length(), richEditorPattern->GetTextContentLength());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->HandleOnDeleteComb(true);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 20);
    richEditorPattern->HandleOnDeleteComb(false);
    EXPECT_EQ(richEditorPattern->GetCaretPosition(), 20);
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts202 about Handle Select FontStyle
 * @tc.desc: test the text font style
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts202, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    /**
     * @tc.steps: step2. get richEditor controller
     */
    auto richEditorController = richEditorPattern->GetRichEditorController();
    ASSERT_NE(richEditorController, nullptr);
    /**
     * @tc.steps: step2. add text span
     */
    TextStyle style;
    TextSpanOptions textOptions;
    textOptions.value = INIT_VALUE_3;
    textOptions.style = style;
    richEditorController->AddTextSpan(textOptions);
    EXPECT_EQ(textOptions.value.length(), richEditorPattern->GetTextContentLength());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->textSelector_.Update(4, 20);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextStart(), 4);
    richEditorPattern->HandleSelectFontStyle(KeyCode::KEY_B);
    EXPECT_EQ(richEditorPattern->GetUpdateSpanStyle().updateFontWeight, Ace::FontWeight::BOLD);
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts203 about Handle Select FontStyle
 * @tc.desc: test the text font style
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts203, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    /**
     * @tc.steps: step2. get richEditor controller
     */
    auto richEditorController = richEditorPattern->GetRichEditorController();
    ASSERT_NE(richEditorController, nullptr);
    /**
     * @tc.steps: step2. add text span
     */
    TextStyle style;
    TextSpanOptions textOptions;
    textOptions.value = INIT_VALUE_3;
    textOptions.style = style;
    richEditorController->AddTextSpan(textOptions);
    EXPECT_EQ(textOptions.value.length(), richEditorPattern->GetTextContentLength());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->textSelector_.Update(4, 20);
    EXPECT_EQ(richEditorPattern->textSelector_.GetTextEnd(), 20);
    richEditorPattern->HandleSelectFontStyle(KeyCode::KEY_I);
    EXPECT_EQ(richEditorPattern->GetUpdateSpanStyle().updateItalicFontStyle, OHOS::Ace::FontStyle::ITALIC);
}

/**
 * @tc.name: RichEditorKeyBoardShortCuts204 about Handle Select FontStyle
 * @tc.desc: test the text font style
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, RichEditorKeyBoardShortCuts204, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get richEditor pattern
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    /**
     * @tc.steps: step2. get richEditor controller
     */
    auto richEditorController = richEditorPattern->GetRichEditorController();
    ASSERT_NE(richEditorController, nullptr);
    /**
     * @tc.steps: step2. add text span
     */
    TextStyle style;
    TextSpanOptions textOptions;
    textOptions.value = INIT_VALUE_3;
    textOptions.style = style;
    richEditorController->AddTextSpan(textOptions);
    EXPECT_EQ(textOptions.value.length(), richEditorPattern->GetTextContentLength());
    richEditorPattern->SetCaretPosition(20);
    richEditorPattern->textSelector_.Update(4, 20);
    richEditorPattern->HandleSelectFontStyle(KeyCode::KEY_U);
    EXPECT_EQ(richEditorPattern->GetUpdateSpanStyle().updateTextDecoration, TextDecoration::UNDERLINE);
}

/**
 * @tc.name: BeforeAddImage101
 * @tc.desc: test BeforeAddImage
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, BeforeAddImage101, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateEventHub();
    auto eventHub = richEditorPattern->GetEventHub<RichEditorEventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto tempFunc1 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func1 = tempFunc1;
    eventHub->onWillChange_ = func1;
    auto tempFunc2 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func2 = tempFunc2;
    eventHub->onDidChange_ = func2;
    RichEditorChangeValue changeValue;
    ImageSpanOptions options;
    void* voidPtr = static_cast<void*>(new char[0]);
    RefPtr<PixelMap> pixelMap = PixelMap::CreatePixelMap(voidPtr);
    ASSERT_NE(pixelMap, nullptr);
    options.imagePixelMap = pixelMap;
    auto ret = richEditorPattern->BeforeAddImage(changeValue, options, 0);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: BeforeChangeText101
 * @tc.desc: test BeforeChangeText
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, BeforeChangeText101, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateEventHub();
    auto eventHub = richEditorPattern->GetEventHub<RichEditorEventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto tempFunc1 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func1 = tempFunc1;
    eventHub->onWillChange_ = func1;
    auto tempFunc2 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func2 = tempFunc2;
    eventHub->onDidChange_ = func2;
    RichEditorChangeValue changeValue;
    RichEditorPattern::OperationRecord record;
    RecordType type = RecordType::DEL_BACKWARD;
    int32_t delLength = 0;
    auto ret = richEditorPattern->BeforeChangeText(changeValue, record, type, delLength);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: GetTextThemeFontSize101
 * @tc.desc: test GetTextThemeFontSize
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, GetTextThemeFontSize101, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto context = PipelineContext::GetCurrentContext();
    ASSERT_NE(context, nullptr);
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    context->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<TextTheme>()));
    auto theme = context->GetTheme<TextTheme>();
    ASSERT_NE(theme, nullptr);
    auto ret = richEditorPattern->GetTextThemeFontSize();
    EXPECT_NE(ret, 0.0f);
}

/**
 * @tc.name: HandleSelectWrapper101
 * @tc.desc: test HandleSelectWrapper
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleSelectWrapper101, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    OHOS::Ace::CaretMoveIntent direction = OHOS::Ace::CaretMoveIntent::Home;
    int32_t fixedPos = 0;
    auto ret = richEditorPattern->HandleSelectWrapper(direction, fixedPos);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: HandleSelectFontStyleWrapper101
 * @tc.desc: test HandleSelectFontStyleWrapper
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleSelectFontStyleWrapper101, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    KeyCode code;
    TextStyle spanStyle;
    /**
     * @tc.steps: step2. change parameters and call function.
     */
    code = KeyCode::KEY_B;
    spanStyle.SetFontWeight(Ace::FontWeight::BOLD);
    richEditorPattern->HandleSelectFontStyleWrapper(code, spanStyle);
    EXPECT_EQ(spanStyle.GetFontWeight(), Ace::FontWeight::NORMAL);
    /**
     * @tc.steps: step3. change parameters and call function.
     */
    spanStyle.SetFontWeight(Ace::FontWeight::NORMAL);
    code = KeyCode::KEY_I;
    spanStyle.SetFontStyle(OHOS::Ace::FontStyle::ITALIC);
    richEditorPattern->HandleSelectFontStyleWrapper(code, spanStyle);
    EXPECT_EQ(spanStyle.GetFontStyle(), OHOS::Ace::FontStyle::NORMAL);
    /**
     * @tc.steps: step4. change parameters and call function.
     */
    spanStyle.SetFontStyle(OHOS::Ace::FontStyle::NORMAL);
    code = KeyCode::KEY_U;
    spanStyle.SetTextDecoration(TextDecoration::UNDERLINE);
    richEditorPattern->HandleSelectFontStyleWrapper(code, spanStyle);
    EXPECT_EQ(spanStyle.GetTextDecoration(), TextDecoration::NONE);
}

/**
 * @tc.name: HandleTripleClickEvent
 * @tc.desc: test HandleTripleClickEvent001
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleTripleClickEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    TestParagraphRect paragraphRect = { .start = 0, .end = 2, .rects = { { -400.0, -400.0, 200.0, 200.0 } } };
    TestParagraphItem paragraphItem = { .start = 0, .end = 2,
        .indexOffsetMap = { { 0, Offset(0, 0) }, { 6, Offset(50, 0) } },
        .testParagraphRects = { paragraphRect } };
    AddParagraph(paragraphItem);
    GestureEvent info;
    int32_t start = 0;
    int32_t end = 0;
    for (auto item : paragraphItem.indexOffsetMap) {
        info.localLocation_ = item.second;
        richEditorPattern->HandleTripleClickEvent(info);
        start = std::min(richEditorPattern->GetTextContentLength(), paragraphItem.start);
        end = std::min(richEditorPattern->GetTextContentLength(), paragraphItem.start);
        EXPECT_TRUE(start == richEditorPattern->textSelector_.baseOffset);
        EXPECT_TRUE(end == richEditorPattern->textSelector_.destinationOffset);
    }
}

/**
 * @tc.name: TestRichEditorShowCaretNoTwinkling001
 * @tc.desc: test ShowCaretNoTwinkling
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, TestRichEditorShowCaretNoTwinkling001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    auto richOffset = Offset(44, 50);
    richEditorPattern->textSelector_.baseOffset = 1;
    richEditorPattern->textSelector_.destinationOffset = 2;
    richEditorPattern->ShowCaretNoTwinkling(richOffset);
    EXPECT_EQ(richEditorPattern->textSelector_.baseOffset, -1);
    EXPECT_EQ(richEditorPattern->textSelector_.destinationOffset, -1);
}

/**
 * @tc.name: PreferredParagraph001
 * @tc.desc: test PreferredParagraph
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, PreferredParagraph001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    EXPECT_CALL(*paragraph, PushStyle(_)).Times(AnyNumber());
    EXPECT_CALL(*paragraph, AddText(_)).Times(AnyNumber());
    EXPECT_CALL(*paragraph, PopStyle()).Times(AnyNumber());
    EXPECT_CALL(*paragraph, Build()).Times(AnyNumber());
    EXPECT_CALL(*paragraph, Layout(_)).Times(AnyNumber());
    TestParagraphItem testParagraphItem = {
        .start = 0, .end = 6, .indexOffsetMap = { { 0, Offset(0, 5) }, { 6, Offset(50, 0) } }
    };
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 6 });
    CaretMetricsF metricsDown;
    CaretMetricsF metricsUp;
    for (const auto& [index, offset] : testParagraphItem.indexOffsetMap) {
        metricsDown.offset.SetX(offset.GetX());
        metricsDown.offset.SetY(offset.GetY());
        metricsUp.offset.SetX(offset.GetX());
        metricsUp.offset.SetY(offset.GetY());
        EXPECT_CALL(*paragraph, GetGlyphIndexByCoordinate(_, _)).WillRepeatedly(Return(6));
        EXPECT_CALL(*paragraph, GetMaxWidth).WillRepeatedly(Return(150));
        EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
        EXPECT_CALL(*paragraph, ComputeOffsetForCaretDownstream(index, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(metricsDown), Return(true)));
        EXPECT_CALL(*paragraph, ComputeOffsetForCaretUpstream(index, _, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(metricsUp), Return(true)));
    }

    TextStyle style;
    style.SetLineHeight(LINE_HEIGHT_VALUE);
    style.SetLetterSpacing(LETTER_SPACING);
    style.SetFontFeatures(TEXT_FONTFEATURE);
    richEditorPattern->CalculateEmptyValueCaretRect();
    richEditorPattern->typingTextStyle_ = style;
    richEditorPattern->PreferredParagraph();
    EXPECT_NE(richEditorPattern->presetParagraph_, nullptr);
    richEditorPattern->PreferredParagraph();
    EXPECT_NE(richEditorPattern->presetParagraph_, nullptr);
}
/**
 * @tc.name: GetTextColorInJson001
 * @tc.desc: test GetTextColorInJson
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, GetTextColorInJson001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    ASSERT_NE(themeManager, nullptr);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<RichEditorTheme>()));
    PipelineBase::GetCurrentContext()->themeManager_ = themeManager;
    std::optional<Color> value;
    auto ret = richEditorPattern->GetTextColorInJson(value);
    EXPECT_FALSE(ret.empty());
}

/**
 * @tc.name: GetPlaceHolderInJson001
 * @tc.desc: test GetPlaceHolderInJson
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, GetPlaceHolderInJson001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto host = richEditorPattern->GetHost();
    EXPECT_NE(host, nullptr);
    auto layoutProperty = host->GetLayoutProperty<TextLayoutProperty>();
    EXPECT_NE(layoutProperty, nullptr);
    auto textframeNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    ASSERT_NE(textframeNode, nullptr);
    host->AddChild(textframeNode);
    auto ret = richEditorPattern->GetPlaceHolderInJson();
    EXPECT_TRUE(ret.empty());
}

/**
 * @tc.name: HandleOnDragDropStyledString001
 * @tc.desc: test HandleOnDragDropStyledString
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleOnDragDropStyledString001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RefPtr<OHOS::Ace::DragEvent> event = AceType::MakeRefPtr<OHOS::Ace::DragEvent>();
    ASSERT_NE(event, nullptr);
    RefPtr<UnifiedData> unifiedData = AceType::MakeRefPtr<MockUnifiedData>();
    ASSERT_NE(unifiedData, nullptr);
    std::string selectedStr = "test123";
    OHOS::Ace::UdmfClient::GetInstance()->AddPlainTextRecord(unifiedData, selectedStr);
    event->SetData(unifiedData);
    richEditorPattern->HandleOnDragDropStyledString(event);
    EXPECT_NE(event->GetData(), nullptr);
}
/**
 * @tc.name: CheckTripClickEvent001
 * @tc.desc: test CheckTripClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, CheckTripClickEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    TimeStamp timeStamp1(std::chrono::nanoseconds(1000));
    richEditorPattern->clickInfo_.push_back(timeStamp1);
    TimeStamp timeStamp2(std::chrono::nanoseconds(2000));
    richEditorPattern->clickInfo_.push_back(timeStamp2);
    TimeStamp timeStamp3(std::chrono::nanoseconds(3000));
    richEditorPattern->clickInfo_.push_back(timeStamp3);
    GestureEvent info;
    TimeStamp timeStamp4(std::chrono::nanoseconds(4000));
    info.SetTimeStamp(timeStamp4);
    richEditorPattern->CheckTripClickEvent(info);
    EXPECT_NE(richEditorPattern->clickInfo_.size(), 4);
}

/**
 * @tc.name: HandleOnDragDrop001
 * @tc.desc: test HandleOnDragDrop
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleOnDragDrop001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto eventHub = richEditorPattern->GetEventHub<RichEditorEventHub>();
    ASSERT_NE(eventHub, nullptr);

    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    ASSERT_NE(themeManager, nullptr);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<RichEditorTheme>()));
    PipelineBase::GetCurrentContext()->themeManager_ = themeManager;

    RefPtr<OHOS::Ace::DragEvent> event = AceType::MakeRefPtr<OHOS::Ace::DragEvent>();
    ASSERT_NE(event, nullptr);
    RefPtr<UnifiedData> unifiedData = AceType::MakeRefPtr<MockUnifiedData>();
    ASSERT_NE(unifiedData, nullptr);
    std::string selectedStr = "test123";
    OHOS::Ace::UdmfClient::GetInstance()->AddPlainTextRecord(unifiedData, selectedStr);
    event->SetData(unifiedData);

    auto focusHub = richEditorPattern->GetFocusHub();
    EXPECT_NE(focusHub, nullptr);
    focusHub->currentFocus_ = true;

    richEditorPattern->HandleOnDragDrop(event);
    EXPECT_NE(event->GetData(), nullptr);
}

/**
 * @tc.name: HandleSelectParagraghPos001
 * @tc.desc: test HandleSelectParagraghPos
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleSelectParagraghPos001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->spans_.clear();
    richEditorPattern->spans_.push_front(AceType::MakeRefPtr<SpanItem>());
    auto it = richEditorPattern->spans_.front();
    it->content = u"test\n123";
    it->position = 4;
    richEditorPattern->caretPosition_ = 1;
    richEditorPattern->isSpanStringMode_ = true;
    auto ret = richEditorPattern->HandleSelectParagraghPos(true);
    EXPECT_EQ(ret, 0);
    ret = richEditorPattern->HandleSelectParagraghPos(false);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleSelectPosition001
 * @tc.desc: test HandleSelectPosition
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleSelectPosition001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->textSelector_.baseOffset = 0;
    richEditorPattern->textSelector_.destinationOffset = 1;
    richEditorPattern->caretPosition_ = richEditorPattern->textSelector_.GetTextEnd();
    auto ret = richEditorPattern->HandleKbVerticalSelection(true);
    EXPECT_EQ(ret, 0);
    ret = richEditorPattern->HandleKbVerticalSelection(false);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: CalcLineEndPosition001
 * @tc.desc: test CalcLineEndPosition
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, CalcLineEndPosition001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    /**
     * @tc.steps: step2. change parameter and call function.
     */
    richEditorPattern->richTextRect_.y_ = 10.0f;
    richEditorPattern->contentRect_.y_ = 20.0f;
    auto ret = richEditorPattern->CalcLineEndPosition();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: GetDeletedSpan001
 * @tc.desc: test GetDeletedSpan
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, GetDeletedSpan001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);

    RichEditorChangeValue changeValue;
    int32_t innerPosition = 10;
    int32_t length = 1;
    RichEditorDeleteDirection direction = RichEditorDeleteDirection::BACKWARD;
    richEditorPattern->textSelector_.baseOffset = 1;
    richEditorPattern->textSelector_.destinationOffset = 1;
    richEditorPattern->GetDeletedSpan(changeValue, innerPosition, length, direction);
    EXPECT_NE(innerPosition, 10);
}

/**
 * @tc.name: HandleMouseLeftButtonRelease001
 * @tc.desc: test HandleMouseLeftButtonRelease
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleMouseLeftButtonRelease001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->dataDetectorAdapter_->pressedByLeftMouse_ = true;
    richEditorPattern->mouseStatus_ = MouseStatus::PRESSED;
    richEditorPattern->status_ = Status::ON_DROP;
    MouseInfo info;
    richEditorPattern->HandleMouseLeftButtonRelease(info);
    EXPECT_FALSE(richEditorPattern->dataDetectorAdapter_->pressedByLeftMouse_);
}

/**
 * @tc.name: HandleMouseRightButton001
 * @tc.desc: test HandleMouseRightButton
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleMouseRightButton001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto focusHub = richEditorPattern->GetFocusHub();
    ASSERT_NE(focusHub, nullptr);
    focusHub->focusType_ = FocusType::DISABLE;
    MouseInfo info;
    richEditorPattern->HandleMouseRightButton(info);
    EXPECT_FALSE(focusHub->IsFocusable());
}

/**
 * @tc.name: BeforeAddImage102
 * @tc.desc: test BeforeAddImage
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, BeforeAddImage102, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateEventHub();
    auto eventHub = richEditorPattern->GetEventHub<RichEditorEventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto tempFunc1 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func1 = tempFunc1;
    eventHub->onWillChange_ = func1;
    auto tempFunc2 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func2 = tempFunc2;
    eventHub->onDidChange_ = func2;
    RichEditorChangeValue changeValue;
    ImageSpanOptions options;
    void* voidPtr = static_cast<void*>(new char[0]);
    RefPtr<PixelMap> pixelMap = PixelMap::CreatePixelMap(voidPtr);
    ASSERT_NE(pixelMap, nullptr);
    options.imagePixelMap = pixelMap;
    options.image = IMAGE_VALUE;
    options.bundleName = BUNDLE_NAME;
    options.moduleName = MODULE_NAME;
    options.offset = 1;
    ImageSpanAttribute imageStyle;
    std::optional<Ace::NG::MarginProperty> marginProp = std::nullopt;
    std::optional<Ace::NG::BorderRadiusProperty> borderRadius = std::nullopt;
    marginProp = { CALC_LENGTH_CALC, CALC_LENGTH_CALC, CALC_LENGTH_CALC, CALC_LENGTH_CALC };
    borderRadius = { CALC_TEST, CALC_TEST, CALC_TEST, CALC_TEST };
    imageStyle.marginProp = marginProp;
    imageStyle.borderRadius = borderRadius;
    options.imageAttribute = imageStyle;
    auto ret = richEditorPattern->BeforeAddImage(changeValue, options, 0);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: BeforeChangeText102
 * @tc.desc: test BeforeChangeText
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, BeforeChangeText102, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateEventHub();
    auto eventHub = richEditorPattern->GetEventHub<RichEditorEventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto tempFunc1 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func1 = tempFunc1;
    eventHub->onWillChange_ = func1;
    auto tempFunc2 = [](const RichEditorChangeValue&) { return true; };
    std::function<bool(const RichEditorChangeValue&)> func2 = tempFunc2;
    eventHub->onDidChange_ = func2;
    AddSpan(INIT_VALUE_1);
    RichEditorChangeValue changeValue;
    TextSpanOptions options;
    options.style = std::optional<TextStyle>(TextStyle(5));
    options.offset = 100;
    auto ret = richEditorPattern->BeforeChangeText(changeValue, options);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: GetDelPartiallySpanItem002
 * @tc.desc: test GetDelPartiallySpanItem
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, GetDelPartiallySpanItem002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init and call function.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->spans_.clear();
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem1 = AceType::MakeRefPtr<SpanItem>();
    spanItem1->unicode = 0;
    richEditorPattern->spans_.emplace_back(spanItem1);
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem2 = AceType::MakeRefPtr<SpanItem>();
    spanItem2->unicode = 0;
    richEditorPattern->spans_.emplace_back(spanItem2);
    RichEditorChangeValue changeValue;
    RichEditorAbstractSpanResult span1;
    changeValue.originalSpans_.emplace_back(span1);
    RichEditorAbstractSpanResult span2;
    changeValue.originalSpans_.emplace_back(span2);
    RichEditorAbstractSpanResult& firstInfo = changeValue.originalSpans_.front();
    firstInfo.SetValue("test123\n");
    /**
     * @tc.steps: step2. change parameter and call function.
     */
    std::u16string originalStr;
    int32_t originalPos = 0;
    int32_t firstLength = static_cast<int32_t>(StringUtils::ToWstring(firstInfo.GetValue()).length());
    firstInfo.SetEraseLength(firstLength);
    auto ret = richEditorPattern->GetDelPartiallySpanItem(changeValue, originalStr, originalPos);
    EXPECT_NE(ret, nullptr);
    /**
     * @tc.steps: step3. change parameter and call function.
     */
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem3 = AceType::MakeRefPtr<SpanItem>();
    spanItem3->unicode = 0;
    richEditorPattern->spans_.emplace_back(spanItem3);
    ret = richEditorPattern->GetDelPartiallySpanItem(changeValue, originalStr, originalPos);
    EXPECT_NE(ret, nullptr);
    /**
     * @tc.steps: step4. change parameter and call function.
     */
    firstInfo.SetSpanIndex(1);
    ret = richEditorPattern->GetDelPartiallySpanItem(changeValue, originalStr, originalPos);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.name: GetDeletedSpan002
 * @tc.desc: test GetDeletedSpan
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, GetDeletedSpan002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    richEditorPattern->spans_.clear();
    richEditorPattern->spans_.push_front(AceType::MakeRefPtr<SpanItem>());
    auto it = richEditorPattern->spans_.front();
    it->content = INIT_U16VALUE_3;
    it->position = 0;
    richEditorPattern->caretPosition_ = 1;

    RichEditorChangeValue changeValue;
    int32_t innerPosition = 10;
    int32_t length = 1;
    RichEditorDeleteDirection direction = RichEditorDeleteDirection::BACKWARD;
    richEditorPattern->textSelector_.baseOffset = 2;
    richEditorPattern->textSelector_.destinationOffset = 2;
    richEditorPattern->GetDeletedSpan(changeValue, innerPosition, length, direction);
    EXPECT_EQ(length, 1);
}

/**
 * @tc.name: GetSelectArea101
 * @tc.desc: test GetSelectArea
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, GetSelectArea101, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    /**
     * @tc.steps: step1. add text and paragraph
     */
    TestParagraphRect paragraphRect = { .start = 0, .end = 6, .rects = { { 0.0, 10.0, 200.0, 200.0 } } };
    TestParagraphItem paragraphItem = { .start = 0, .end = 6, .testParagraphRects = { paragraphRect } };
    AddParagraph(paragraphItem);
    richEditorPattern->textSelector_ = TextSelector(0, 6);
    richEditorPattern->contentRect_ = { 0.0, 10.0, 500.0, 500.0 };
    richEditorPattern->isShowPlaceholder_ = true;
    auto res = richEditorPattern->GetSelectArea();
    EXPECT_TRUE(res.IsValid());
}

/**
 * @tc.name: HandleMouseRightButton002
 * @tc.desc: test HandleMouseRightButton
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleMouseRightButton002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto focusHub = richEditorPattern->GetFocusHub();
    ASSERT_NE(focusHub, nullptr);
    focusHub->focusType_ = FocusType::NODE;
    focusHub->focusable_ = true;
    focusHub->parentFocusable_ = true;
    EXPECT_NE(richEditorPattern->GetFocusHub(), nullptr);
    EXPECT_EQ(richEditorPattern->GetFocusHub()->IsFocusable(), true);
    /**
     * @tc.steps: step2. change parameter and call function.
     */
    MouseInfo info;
    info.action_ = MouseAction::RELEASE;
    info.button_ = MouseButton::EXTRA_BUTTON;
    richEditorPattern->textSelector_.baseOffset = -2;
    richEditorPattern->textSelector_.destinationOffset = -3;
    richEditorPattern->dataDetectorAdapter_->hasClickedAISpan_ = true;
    richEditorPattern->HandleMouseRightButton(info);
    EXPECT_FALSE(richEditorPattern->dataDetectorAdapter_->hasClickedAISpan_);
    /**
     * @tc.steps: step3. change parameter and call function.
     */
    info.globalLocation_.deltaX_ = 50.0;
    info.globalLocation_.deltaY_ = 150.0;
    richEditorPattern->textSelector_.baseOffset = 10;
    richEditorPattern->textSelector_.destinationOffset = 50;
    richEditorPattern->dataDetectorAdapter_->hasClickedAISpan_ = true;
    richEditorPattern->copyOption_ = CopyOptions::InApp;
    richEditorPattern->HandleMouseRightButton(info);
    EXPECT_FALSE(richEditorPattern->dataDetectorAdapter_->hasClickedAISpan_);
}

/**
 * @tc.name: HandleMouseLeftButtonMove001
 * @tc.desc: test HandleMouseLeftButtonMove
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleMouseLeftButtonMove001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto focusHub = richEditorPattern->GetFocusHub();
    ASSERT_NE(focusHub, nullptr);
    EXPECT_NE(richEditorPattern->GetFocusHub(), nullptr);
    EXPECT_EQ(richEditorPattern->GetFocusHub()->IsFocusable(), true);
    /**
     * @tc.steps: step2. change parameter and call function.
     */
    MouseInfo info;
    info.action_ = MouseAction::RELEASE;
    info.button_ = MouseButton::EXTRA_BUTTON;
    richEditorPattern->blockPress_ = true;
    richEditorPattern->leftMousePress_ = false;
    richEditorPattern->HandleMouseLeftButtonMove(info);
    EXPECT_FALSE(richEditorPattern->isMouseSelect_);
    /**
     * @tc.steps: step3. change parameter and call function.
     */
    richEditorPattern->dataDetectorAdapter_->pressedByLeftMouse_ = true;
    richEditorPattern->blockPress_ = false;
    richEditorPattern->leftMousePress_ = true;
    richEditorPattern->HandleMouseLeftButtonMove(info);
    EXPECT_TRUE(richEditorPattern->isMouseSelect_);
    /**
     * @tc.steps: step4. change parameter and call function.
     */
    richEditorPattern->dataDetectorAdapter_->pressedByLeftMouse_ = false;
    focusHub->currentFocus_ = false;
    richEditorPattern->blockPress_ = false;
    richEditorPattern->leftMousePress_ = true;
    richEditorPattern->HandleMouseLeftButtonMove(info);
    EXPECT_TRUE(richEditorPattern->isMouseSelect_);
    /**
     * @tc.steps: step5. change parameter and call function.
     */
    richEditorPattern->dataDetectorAdapter_->pressedByLeftMouse_ = true;
    focusHub->currentFocus_ = true;
    richEditorPattern->blockPress_ = false;
    richEditorPattern->leftMousePress_ = true;
    richEditorPattern->textSelector_.baseOffset = 2;
    richEditorPattern->textSelector_.destinationOffset = 3;
    richEditorPattern->HandleMouseLeftButtonMove(info);
    EXPECT_TRUE(richEditorPattern->isMouseSelect_);
}

/**
 * @tc.name: HandleTouchMove001
 * @tc.desc: test HandleTouchMove
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleTouchMove001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    TouchLocationInfo touchLocationInfo(0);
    touchLocationInfo.touchType_ = TouchType::MOVE;
    touchLocationInfo.localLocation_ = Offset(10.0f, 20.0f);
    /**
     * @tc.steps: step2. change parameter and call function.
     */
    richEditorPattern->textSelector_.baseOffset = richEditorPattern->caretPosition_ - 1;
    richEditorPattern->isLongPress_ = true;
    richEditorPattern->previewLongPress_ = true;
    richEditorPattern->HandleTouchMove(touchLocationInfo);
    EXPECT_EQ(richEditorPattern->textSelector_.baseOffset, richEditorPattern->caretPosition_);
    /**
     * @tc.steps: step3. change parameter and call function.
     */
    richEditorPattern->isLongPress_ = true;
    richEditorPattern->previewLongPress_ = false;
    richEditorPattern->isMoveCaretAnywhere_ = true;
    richEditorPattern->HandleTouchMove(touchLocationInfo);
    EXPECT_EQ(richEditorPattern->textSelector_.baseOffset, richEditorPattern->caretPosition_);
    /**
     * @tc.steps: step4. change parameter and call function.
     */
    richEditorPattern->isLongPress_ = true;
    richEditorPattern->previewLongPress_ = false;
    richEditorPattern->isMoveCaretAnywhere_ = false;
    richEditorPattern->HandleTouchMove(touchLocationInfo);
    EXPECT_EQ(richEditorPattern->textSelector_.baseOffset, richEditorPattern->caretPosition_);
    /**
     * @tc.steps: step5. change parameter and call function.
     */
    richEditorPattern->isLongPress_ = false;
    richEditorPattern->selectOverlay_->ProcessOverlay({ .animation = false });
    auto manager = richEditorPattern->selectOverlay_->GetManager<SelectContentOverlayManager>();
    ASSERT_NE(manager, nullptr);
    SelectOverlayInfo info;
    manager->CreateSelectOverlay(info);
    ASSERT_NE(manager->shareOverlayInfo_, nullptr);
    manager->shareOverlayInfo_->isSingleHandle = true;
    richEditorPattern->HandleTouchMove(touchLocationInfo);
    EXPECT_EQ(richEditorPattern->textSelector_.baseOffset, richEditorPattern->caretPosition_);
}

/**
 * @tc.name: HandleTouchUp001
 * @tc.desc: test HandleTouchUp
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleTouchUp001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. declare and init variables.
     */
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->isLongPress_ = true;
    richEditorPattern->moveCaretState_.isTouchCaret = true;
    richEditorPattern->previewLongPress_ = true;
    richEditorPattern->isMoveCaretAnywhere_ = true;
    richEditorPattern->magnifierController_->isShowMagnifier_ = true;
    richEditorPattern->HandleTouchUp();
    EXPECT_FALSE(richEditorPattern->isMoveCaretAnywhere_);
}

/**
 * @tc.name: SetCustomKeyboard001
 * @tc.desc: test SetCustomKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, SetCustomKeyboard001, TestSize.Level1)
{
    RichEditorModelNG richEditorModel;
    richEditorModel.Create();
    auto func = []() {};
    richEditorModel.SetCustomKeyboard(func, true);
    bool result =
        ViewStackProcessor::GetInstance()->GetMainFrameNode()->GetPattern<RichEditorPattern>()->keyboardAvoidance_;
    EXPECT_TRUE(result);
}

/**
 * @tc.name: HandleTouchMove002
 * @tc.desc: test HandleTouchMove
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleTouchMove002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->previewLongPress_ = false;
    richEditorPattern->editingLongPress_ = false;
    richEditorPattern->moveCaretState_.isTouchCaret = true;
    richEditorPattern->moveCaretState_.isMoveCaret = false;
    richEditorPattern->moveCaretState_.touchDownOffset = Offset(5.0f, 5.0f);
    Offset offset(5.0f, 5.0f);
    auto moveDistance = (offset - richEditorPattern->moveCaretState_.touchDownOffset).GetDistance();
    ASSERT_GT(richEditorPattern->moveCaretState_.minDistance.ConvertToPx(), moveDistance);
    TouchLocationInfo touchLocationInfo(1);
    touchLocationInfo.SetGlobalLocation(offset);
    richEditorPattern->HandleTouchMove(touchLocationInfo);
    EXPECT_EQ(richEditorPattern->moveCaretState_.touchFingerId, touchLocationInfo.fingerId_);
}

/**
 * @tc.name: HandleTouchMove003
 * @tc.desc: test HandleTouchMove
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorKeyboardShortcutTestNg, HandleTouchMove003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->previewLongPress_ = false;
    richEditorPattern->editingLongPress_ = false;
    richEditorPattern->moveCaretState_.isTouchCaret = true;
    richEditorPattern->moveCaretState_.isMoveCaret = true;
    richEditorPattern->moveCaretState_.touchDownOffset = Offset(5.0f, 5.0f);
    Offset offset(5.01f, 5.01f);
    auto moveDistance = (offset - richEditorPattern->moveCaretState_.touchDownOffset).GetDistance();
    ASSERT_LE(moveDistance, richEditorPattern->moveCaretState_.minDistance.ConvertToPx() + 0.001f);
    TouchLocationInfo touchLocationInfo(1);
    touchLocationInfo.SetGlobalLocation(offset);
    richEditorPattern->HandleTouchMove(touchLocationInfo);
    EXPECT_NE(richEditorPattern->moveCaretState_.touchFingerId, touchLocationInfo.fingerId_);
}
} // namespace OHOS::Ace::NG
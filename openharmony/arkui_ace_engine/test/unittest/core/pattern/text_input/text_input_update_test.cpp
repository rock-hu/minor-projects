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

#include "text_input_base.h"

namespace OHOS::Ace::NG {

namespace {} // namespace

class TextInputUpdateTestNg : public TextInputBases {
public:
};

class TextInputAutoFillTest : public TextInputBases {
public:
};
/**
 * @tc.name: HandleLongPress001
 * @tc.desc: Test textinput handle action.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, HandleLongPress001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetType(TextInputType::NUMBER_PASSWORD);
    });
    GetFocus();
    GestureEvent gestureEvent;

    /**
     * @tc.steps: step2. Set secondhandle. Call function HandleLongPress.
     * @tc.expected: Check if return true.
     */
    pattern_->obscureTickCountDown_ = 1;
    pattern_->longPressEvent_->operator()(gestureEvent);
    EXPECT_TRUE(pattern_->showSelect_);
}

/**
 * @tc.name: DeleteBackwardWhenHasMaxLength001
 * @tc.desc: Test the soft keyboard interface
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, DeleteBackwardWhenHasMaxLength001, TestSize.Level1)
{
    /**
     * @tc.steps: Initialize text input node and call delete backward
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetMaxLength(1000023);
    });

    GetFocus();
    pattern_->DeleteBackward(5);
    FlushLayoutTask(frameNode_);

    /**
     * @tc.expected: Check if the new text and cursor position are correct
     */
    EXPECT_EQ(pattern_->GetTextValue().compare("abcdefghijklmnopqrstu"),
        0) << "Text is " + pattern_->GetTextValue();
    EXPECT_EQ(pattern_->GetCaretIndex(), static_cast<int>(DEFAULT_TEXT.length() - 5));
}

/**
 * @tc.name: GetWordLengthOutlier001
 * @tc.desc: GetWordLength passes in abnormal parameters.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetWordLengthOutlier001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT);
    GetFocus();

    /**
     * @tc.steps: step2. Set originCaretPosition outlier. Call function GetWordLength.
     * @tc.expected: set originCaretPosition less then wideTextLong, return 0.
     */
    auto originCaretPosition = -1;
    auto directionMove = 2;
    EXPECT_EQ(pattern_->GetWordLength(originCaretPosition, directionMove), 0);

    /**
     * @tc.expected: set originCaretPosition greater then wideTextLong
     * @tc.expected: return 0
     */
    originCaretPosition = 27;
    EXPECT_EQ(pattern_->GetWordLength(originCaretPosition, directionMove), 0);
}

/**
 * @tc.name: GetWordLengthOutlier002
 * @tc.desc: GetWordLength passes in abnormal parameters.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetWordLengthOutlier002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT);
    GetFocus();

    /**
     * @tc.steps: step2. Set originCaretPosition outlier. Call function GetWordLength.
     * @tc.expected: set originCaretPosition less then wideTextLong, return 0.
     */
    auto originCaretPosition = 0;
    auto directionMove = 0;
    EXPECT_EQ(pattern_->GetWordLength(originCaretPosition, directionMove), 0);

    /**
     * @tc.expected: set originCaretPosition greater then wideTextLong
     * @tc.expected: return 0
     */
    originCaretPosition = 26;
    directionMove = 1;
    EXPECT_EQ(pattern_->GetWordLength(originCaretPosition, directionMove), 0);
}

/**
 * @tc.name: GetLineBeginPositionOutlier001
 * @tc.desc: GetLineBeginPosition passes in abnormal parameters.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetLineBeginPositionOutlier001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT);
    GetFocus();

    /**
     * @tc.steps: step2. Set originCaretPosition outlier. Call function GetLineBeginPosition.
     * @tc.expected: set originCaretPosition less then wideTextLong, return 0.
     */
    auto originCaretPosition = -1;
    EXPECT_EQ(pattern_->GetLineBeginPosition(originCaretPosition, false), 0);

    /**
     * @tc.expected: set originCaretPosition greater then wideTextLong
     * @tc.expected: return 0
     */
    originCaretPosition = 27;
    EXPECT_EQ(pattern_->GetLineBeginPosition(originCaretPosition, false), 0);

    originCaretPosition = 0;
    EXPECT_EQ(pattern_->GetLineBeginPosition(originCaretPosition, false), 0);
}

/**
 * @tc.name: GetLineEndPositionOutlier001
 * @tc.desc: GetLineEndPosition passes in abnormal parameters.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetLineEndPositionOutlier001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT);
    GetFocus();

    /**
     * @tc.steps: step2. Set originCaretPosition outlier. Call function GetLineEndPosition.
     * @tc.expected: set originCaretPosition less then wideTextLong, return 0.
     */
    auto originCaretPosition = -1;
    EXPECT_EQ(pattern_->GetLineEndPosition(originCaretPosition, false), originCaretPosition);

    /**
     * @tc.expected: set originCaretPosition greater then wideTextLong
     * @tc.expected: return 0
     */
    originCaretPosition = 27;
    EXPECT_EQ(pattern_->GetLineEndPosition(originCaretPosition, false), originCaretPosition);

    originCaretPosition = 26;
    pattern_->HandleOnDelete(true);
    EXPECT_EQ(pattern_->GetLineEndPosition(originCaretPosition, false), originCaretPosition);
}

/**
 * @tc.name: CursorMoveLeftAfterSelected001
 * @tc.desc: Call function CursorMoveLeft.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, CursorMoveLeftAfterSelected001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT);
    GetFocus();

    /**
     * @tc.steps: step2. Set Select. Call function CursorMoveLeft.
     * @tc.expected:
     */
    pattern_->HandleSetSelection(5, 10, false);
    auto ret = pattern_->CursorMoveLeft();
    EXPECT_TRUE(ret);
    EXPECT_EQ(pattern_->GetCaretIndex(), 5) << "Text is " + pattern_->GetTextValue() +
        ", CaretIndex is " + std::to_string(pattern_->GetCaretIndex());
}

/**
 * @tc.name: CursorMoveLeftWord001
 * @tc.desc: Call function CursorMoveLeftWord.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, CursorMoveLeftWord001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_PLACE_HOLDER);
    GetFocus();

    /**
     * @tc.steps: step2. Set cursor at the beginning of the line.
     * @tc.expected:return true.
     */
    pattern_->SetCaretPosition(0);
    EXPECT_TRUE(pattern_->CursorMoveLeftWord());
}

/**
 * @tc.name: CursorMoveRightAfterSelected001
 * @tc.desc: Call function CursorMoveRight.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, CursorMoveRightAfterSelected001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT);
    GetFocus();
    pattern_->focusIndex_ = FocuseIndex::UNIT;
    EXPECT_FALSE(pattern_->UpdateFocusForward());
    pattern_->CursorMoveRight();
    FlushLayoutTask(frameNode_);

    /**
     * @tc.steps: step2. Set Select. Call function CursorMoveRight.
     * @tc.expected:
     */
    pattern_->HandleSetSelection(5, 10, false);
    auto ret = pattern_->CursorMoveRight();
    EXPECT_FALSE(ret);
    EXPECT_EQ(pattern_->GetCaretIndex(), 10) << "Text is " + pattern_->GetTextValue() +
        ", CaretIndex is " + std::to_string(pattern_->GetCaretIndex());
}

/**
 * @tc.name: HandleCounterBorder001
 * @tc.desc: test testIput maxLength
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, HandleCounterBorder001, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text filed node with set maxLength 0
     * @tc.expected: maxLength is 0
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetMaxLength(26);
    });
    GetFocus();

    /**
     * @tc.step: step2. test maxLength
     */
    pattern_->HandleCounterBorder();
    pattern_->HandleOnDelete(true);
    EXPECT_EQ(pattern_->GetMaxLength(), 26);
    EXPECT_EQ(pattern_->GetTextIndexAtCursor(), 26);
}

/**
 * @tc.name: GetShowPasswordIconString001
 * @tc.desc: Test GetShowPasswordIconString
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetShowPasswordIconString001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. show password icon string.
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetShowPasswordIcon(true);
    });

    EXPECT_EQ(pattern_->GetShowPasswordIconString(), "true");
}

/**
 * @tc.name: GetShowPasswordIconString002
 * @tc.desc: Test GetShowPasswordIconString
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetShowPasswordIconString002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. show password icon string.
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetShowPasswordIcon(false);
    });

    EXPECT_EQ(pattern_->GetShowPasswordIconString(), "false");
}

/**
 * @tc.name: GetErrorTextString001
 * @tc.desc: Test GetErrorTextString
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetErrorTextString001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. show password icon string.
     */
    CreateTextField(DEFAULT_TEXT);

    pattern_->SearchRequestKeyboard();
    EXPECT_EQ(pattern_->GetErrorTextString(), "");
}

/**
 * @tc.name: GetBarStateString001
 * @tc.desc: test GetBarStateString
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetBarStateString001, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text field node
     * @tc.expected: barState is DisplayMode::ON
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetBarState(DisplayMode::ON);
    });

    /**
     * @tc.step: step2. Test barState
     */
    frameNode_->MarkModifyDone();
    EXPECT_EQ(pattern_->GetBarStateString(), "BarState.ON");
}

/**
 * @tc.name: GetBarStateString002
 * @tc.desc: test GetBarStateString
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetBarStateString002, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text field node
     * @tc.expected: barState is DisplayMode::OFF
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetBarState(DisplayMode::OFF);
    });

    /**
     * @tc.step: step2. Test barState
     */
    frameNode_->MarkModifyDone();
    EXPECT_EQ(pattern_->GetBarStateString(), "BarState.OFF");
}

/**
 * @tc.name: GetScrollBarWidth001
 * @tc.desc: test GetScrollBarWidth
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, GetScrollBarWidth001, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text field node
     * @tc.expected: scrollBarWidth is 0.
     */
    CreateTextField(DEFAULT_TEXT);

    /**
     * @tc.step: step2. Test Width
     */
    EXPECT_EQ(pattern_->GetScrollBarWidth(), 0);
}

/**
 * @tc.name: AddCounterNode001
 * @tc.desc: test AddCounterNode
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, AddCounterNode001, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text field node
     * @tc.expected: counterTextNode_ is created
     */
    CreateTextField(DEFAULT_TEXT);

    /**
     * @tc.step: step2. Test counterTextNode_ is null
     */
    EXPECT_FALSE(pattern_->counterTextNode_.Upgrade());

    /**
     * @tc.step: step3. call AddCounterNode
     */
    pattern_->AddCounterNode();
    EXPECT_TRUE(pattern_->counterTextNode_.Upgrade());
    pattern_->ClearCounterNode();
}

/**
 * @tc.name: AdjustTextInReasonableArea
 * @tc.desc: test AdjustTextInReasonableArea
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, AdjustTextInReasonableArea, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text field node
     */
    CreateTextField(DEFAULT_TEXT);

    /**
     * @tc.steps:set textRect
     */
    RectF textRect = RectF(0.0f, 0.0f, 800.0f, 60.0f);
    pattern_->SetTextRect(textRect);
    pattern_->AdjustTextInReasonableArea();
    EXPECT_EQ(pattern_->GetTextRect().GetOffset(), OffsetF(0.0f, 0.0f));

    /**
     * @tc.steps:set contentOffset, contentY > textY.
     */
    OffsetF contentOffset1(0.0f, 20.0f);
    pattern_->contentRect_.SetOffset(contentOffset1);
    pattern_->AdjustTextInReasonableArea();
    EXPECT_EQ(pattern_->GetTextRect().GetOffset(), OffsetF(0.0f, 10.0f));

    /**
     * @tc.steps:set contentOffset, contentX > textX.
     */
    OffsetF contentOffset2(90.0f, 0.0f);
    pattern_->contentRect_.SetOffset(contentOffset2);
    pattern_->AdjustTextInReasonableArea();
    EXPECT_EQ(pattern_->GetTextRect().GetOffset(), OffsetF(10.0f, 0.0f));
}

/**
 * @tc.name: AdjustTextInReasonableArea002
 * @tc.desc: test AdjustTextInReasonableArea002
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, AdjustTextInReasonableArea002, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text field node
     */
    CreateTextField(DEFAULT_TEXT);

    /**
     * @tc.steps:set textRect
     */
    RectF textRect = RectF(5.0f, 5.0f, 800.0f, 60.0f);
    pattern_->SetTextRect(textRect);
    std::cout<<pattern_->textRect_.GetY()<<", "<<pattern_->contentRect_.GetY()<<std::endl;
    pattern_->AdjustTextInReasonableArea();
    EXPECT_EQ(pattern_->GetTextRect().GetOffset(), OffsetF(0.0f, 0.0f));
}

/**
 * @tc.name: ProcessOverlayAfterLayout
 * @tc.desc: Test function ProcessOverlayAfterLayout.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, ProcessOverlayAfterLayout, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create node.
     */
    CreateTextField(DEFAULT_TEXT);

    pattern_->ProcessOverlayAfterLayout(OffsetF());
    GetFocus();
    pattern_->HandleSetSelection(5, 10, true);
    pattern_->needToRefreshSelectOverlay_ = true;
    pattern_->ProcessOverlayAfterLayout(OffsetF());
    EXPECT_FALSE(pattern_->needToRefreshSelectOverlay_);
}

/**
 * @tc.name: OnDragStart
 * @tc.desc: Test function OnModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, OnDragStart, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create node.
     */
    CreateTextField(DEFAULT_TEXT);

    /**
     * @tc.steps: step2. callback the InitDragDrop in OnModifyDone.
     * @tc.expected: Check if return true.
     */
    auto dragEvent = AceType::MakeRefPtr<OHOS::Ace::DragEvent>();

    /**
     * @tc.steps: step3. mock drag start.
     */
    auto gestureHub = pattern_->
        GetHost()->GetEventHub<EventHub>()->GetOrCreateGestureEventHub();
    gestureHub->SetIsTextDraggable(true);
    pattern_->OnDragStart().operator()(dragEvent, "");
    EXPECT_FALSE(pattern_->showSelect_);
}

/**
 * @tc.name: HandleTouchUp
 * @tc.desc: test HandleTouchUp
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, HandleTouchUp, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField();
    GetFocus();

    /**
     * @tc.steps: step2. create location info, touch type DOWN
     */
    TouchLocationInfo touchLocationInfo1(0);
    touchLocationInfo1.touchType_ = TouchType::UP;
    touchLocationInfo1.localLocation_ = Offset(0.0f, 0.0f);
    TouchLocationInfo touchLocationInfo11(0);
    touchLocationInfo11.touchType_ = TouchType::UP;
    touchLocationInfo11.localLocation_ = Offset(0.0f, 0.0f);

    /**
     * @tc.steps: step3. create touch info, touch type DOWN
     */
    TouchEventInfo touchInfo1("");
    touchInfo1.AddTouchLocationInfo(std::move(touchLocationInfo1));
    touchInfo1.AddChangedTouchLocationInfo(std::move(touchLocationInfo11));

    /**
     * @tc.steps: step4. test touch down
     */
    pattern_->isMousePressed_ = true;
    pattern_->HandleTouchEvent(touchInfo1);
    frameNode_->UpdateInspectorId("123");
    pattern_->OnAfterModifyDone();
    EXPECT_FALSE(pattern_->isMousePressed_);
}

/**
 * @tc.name: HandleSpaceEvent001
 * @tc.desc: test for password response keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, HandleSpaceEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create node.
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetShowPasswordIcon(true);
        model.SetType(TextInputType::NEW_PASSWORD);
    });
    GetFocus();

    pattern_->focusIndex_ = FocuseIndex::UNIT;

    /**
     * @tc.steps: step2. call PasswordResponseKeyEvent.
     */
    EXPECT_TRUE(pattern_->HandleSpaceEvent());
}

/**
 * @tc.name: HandleSpaceEvent002
 * @tc.desc: test for unit response keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, HandleSpaceEvent002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create node.
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetShowUnderline(true);
        model.SetType(TextInputType::TEXT);
    });
    GetFocus();

    pattern_->focusIndex_ = FocuseIndex::UNIT;

    /**
     * @tc.steps: step2. call PasswordResponseKeyEvent.
     */
    EXPECT_TRUE(pattern_->HandleSpaceEvent());
}

/**
 * @tc.name: HandleSpaceEvent003
 * @tc.desc: test for unit response keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, HandleSpaceEvent003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create node.
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetShowUnderline(true);
        model.SetType(TextInputType::TEXT);
    });
    GetFocus();

    pattern_->focusIndex_ = FocuseIndex::CANCEL;
    pattern_->CreateHandles();

    /**
     * @tc.steps: step2. call cleanNode response keyEvent.
     */
    EXPECT_TRUE(pattern_->HandleSpaceEvent());
}

/**
 * @tc.name: OnTextInputScroll001
 * @tc.desc: test for unit response keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, OnTextInputScroll001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize text input.
     */
    CreateTextField(DEFAULT_TEXT);
    GetFocus();

    pattern_->ProcessOverlay();
    EXPECT_TRUE(pattern_->SelectOverlayIsOn());
    pattern_->OnTextInputScroll(2.0f);
    EXPECT_EQ(pattern_->selectController_->GetCaretRect().GetX(), 0.0f);
}

/**
 * @tc.name: FromJson
 * @tc.desc: test for text from json
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, FromJson, TestSize.Level1)
{
    /**
     * @tc.steps: Create Text filed node with default attrs
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetWidthAuto(true);
        model.SetType(TextInputType::TEXT);
        model.SetPlaceholderColor(DEFAULT_PLACE_HODER_COLOR);
        model.SetTextColor(DEFAULT_TEXT_COLOR);
        model.SetEnterKeyType(DEFAULT_ENTER_KEY_TYPE);
        model.SetTextAlign(DEFAULT_TEXT_ALIGN);
        model.SetCaretColor(DEFAULT_CARET_COLOR);
        model.SetCaretStyle(DEFAULT_CARET_STYLE);
        model.SetSelectedBackgroundColor(DEFAULT_SELECTED_BACKFROUND_COLOR);
        model.SetMaxLength(DEFAULT_MAX_LENGTH);
        model.SetMaxLines(DEFAULT_MAX_LINES);
        model.SetFontSize(DEFAULT_FONT_SIZE);
        model.SetFontWeight(DEFAULT_FONT_WEIGHT);
        model.SetTextColor(DEFAULT_TEXT_COLOR);
        model.SetInputFilter(DEFAULT_INPUT_FILTER, nullptr);
        model.SetCopyOption(DEFAULT_COPY_OPTIONS);
        model.SetBarState(DEFAULT_DISPLAY_MODE);
        model.SetInputStyle(DEFAULT_INPUT_STYLE);
        model.SetShowUnderline(true);
        model.SetSelectAllValue(true);
        model.SetShowCounterBorder(true);
        model.SetWordBreak(WordBreak::BREAK_ALL);
        model.SetTextOverflow(TextOverflow::CLIP);
        model.SetTextIndent(DEFAULT_INDENT_SIZE);
    });

    /**
     * @tc.expected: Check if all set properties are displayed in the TextArea JSON
     */
    auto json = JsonUtil::Create(true);
    pattern_->ToJsonValue(json, filter);
    pattern_->FromJson(json);

    /**
     * @tc.expected: check decoration in TextArea JSON is NONE, BLACK, SOLID
     */
    EXPECT_EQ(json->GetString("text"), DEFAULT_TEXT.c_str());
    EXPECT_EQ(json->GetString("type"), "InputType.Normal");
    EXPECT_EQ(json->GetString("caretColor"), "#FF000000");
    EXPECT_EQ(json->GetString("placeholderColor"), "#FFFF0000");
    EXPECT_EQ(json->GetString("textAlign"), "TextAlign.Left");
    EXPECT_EQ(json->GetString("enterKeyType"), "EnterKeyType.Done");
    EXPECT_EQ(json->GetString("maxLength"), "30");
    EXPECT_EQ(json->GetString("inputFilter"), "[a-z]");
    EXPECT_EQ(json->GetString("copyOption"), "CopyOptions.InApp");
    EXPECT_EQ(json->GetString("style"), "TextInputStyle.Inline");
    EXPECT_EQ(json->GetString("maxLines"), "3");
    EXPECT_EQ(json->GetString("barState"), "BarState.AUTO");
    json = JsonUtil::Create(true);
    layoutProperty_->ToJsonValue(json, filter);
    EXPECT_EQ(json->GetString("caretPosition"), "");
    EXPECT_TRUE(json->GetBool("showUnderline"));
    EXPECT_TRUE(json->GetBool("selectAll"));
    EXPECT_EQ(json->GetString("wordBreak"), "break-all");
}

/**
 * @tc.name: UpdateRectByTextAlign001
 * @tc.desc: test rect change by textalign
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, UpdateRectByTextAlign001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField();
    GetFocus();

    RectF textRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
    pattern_->UpdateRectByTextAlign(textRect);

    EXPECT_EQ(textRect, RectF(0.0f, 0.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: UpdateRectByTextAlign002
 * @tc.desc: test rect change by textalign
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, UpdateRectByTextAlign002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetTextAlign(TextAlign::START);
    });
    GetFocus();

    RectF textRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
    pattern_->UpdateRectByTextAlign(textRect);

    EXPECT_EQ(textRect, RectF(0.0f, 0.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: UpdateRectByTextAlign003
 * @tc.desc: test rect change by textalign
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, UpdateRectByTextAlign003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetTextAlign(TextAlign::CENTER);
    });
    GetFocus();

    RectF textRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
    pattern_->UpdateRectByTextAlign(textRect);

    EXPECT_EQ(textRect, RectF(130.0f, 0.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: UpdateRectByTextAlign004
 * @tc.desc: test rect change by textalign
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, UpdateRectByTextAlign004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetTextAlign(TextAlign::END);
    });
    GetFocus();

    RectF textRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
    pattern_->UpdateRectByTextAlign(textRect);

    EXPECT_EQ(textRect, RectF(260.0f, 0.0f, 0.0f, 0.0f));
}

/**
 * @tc.name: NotifyFillRequestSuccess
 * @tc.desc: Test NotifyFillRequestSuccess
 * @tc.type: FUNC
 */
HWTEST_F(TextInputAutoFillTest, NotifyFillRequestSuccess, TestSize.Level1)
{
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetType(TextInputType::VISIBLE_PASSWORD);
    });

    pattern_->SetFillRequestFinish(false);
    pattern_->NotifyFillRequestSuccess(nullptr, nullptr, AceAutoFillType::ACE_PASSWORD);
    EXPECT_EQ(pattern_->IsFillRequestFinish(), true);
}

/**
 * @tc.name: NotifyFillRequestFailed
 * @tc.desc: Test NotifyFillRequestFailed
 * @tc.type: FUNC
 */
HWTEST_F(TextInputAutoFillTest, NotifyFillRequestFailed, TestSize.Level1)
{
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetType(TextInputType::VISIBLE_PASSWORD);
    });

    pattern_->SetFillRequestFinish(false);
    pattern_->NotifyFillRequestFailed(1);
    EXPECT_EQ(pattern_->IsFillRequestFinish(), true);
}

/**
 * @tc.name: CheckAutoSave
 * @tc.desc: Test CheckAutoSave
 * @tc.type: FUNC
 */
HWTEST_F(TextInputAutoFillTest, CheckAutoSave, TestSize.Level1)
{
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetType(TextInputType::VISIBLE_PASSWORD);
        model.SetEnableAutoFill(false);
    });

    EXPECT_EQ(pattern_->CheckAutoSave(), false);
}

/**
 * @tc.name: ConvertToAceAutoFillType
 * @tc.desc: Test ConvertToAceAutoFillType
 * @tc.type: FUNC
 */
HWTEST_F(TextInputAutoFillTest, ConvertToAceAutoFillType, TestSize.Level1)
{
    CreateTextField(DEFAULT_TEXT);

    EXPECT_EQ(pattern_->ConvertToAceAutoFillType(TextInputType::VISIBLE_PASSWORD),
        AceAutoFillType::ACE_PASSWORD);
    EXPECT_EQ(pattern_->ConvertToAceAutoFillType(TextInputType::USER_NAME),
        AceAutoFillType::ACE_USER_NAME);
    EXPECT_EQ(pattern_->ConvertToAceAutoFillType(TextInputType::NEW_PASSWORD),
        AceAutoFillType::ACE_NEW_PASSWORD);
}

/**
 * @tc.name: CheckAutoFill
 * @tc.desc: Test CheckAutoFill
 * @tc.type: FUNC
 */
HWTEST_F(TextInputAutoFillTest, CheckAutoFill, TestSize.Level1)
{
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetType(TextInputType::VISIBLE_PASSWORD);
        model.SetEnableAutoFill(false);
    });

    EXPECT_EQ(pattern_->CheckAutoFill(), false);
}

/**
 * @tc.name: ProcessAutoFill
 * @tc.desc: Test ProcessAutoFill
 * @tc.type: FUNC
 */
HWTEST_F(TextInputAutoFillTest, ProcessAutoFill, TestSize.Level1)
{
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) {
        model.SetType(TextInputType::VISIBLE_PASSWORD);
        model.SetEnableAutoFill(false);
    });

    bool isPopup = false;
    EXPECT_EQ(pattern_->ProcessAutoFill(isPopup), false);
}

/**
 * @tc.name: ChangeTextCallbackTest001
 * @tc.desc: test for callback SetOnWillInsertValueEvent/SetOnDidInsertValueEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, ChangeTextCallbackTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField();
    GetFocus();
    int32_t offset = 0;
    std::string value = "";

    auto onWillChange = [&offset, &value](const InsertValueInfo& info) {
        offset = info.insertOffset;
        value = info.insertValue;
        return true;
    };
    eventHub_->SetOnWillInsertValueEvent(std::move(onWillChange));

    int32_t didOffset = 0;
    std::string didValue = "";
    auto onDidChange = [&didOffset, &didValue](const InsertValueInfo& info) {
        didOffset = info.insertOffset;
        didValue = info.insertValue;
    };
    eventHub_->SetOnDidInsertValueEvent(std::move(onDidChange));

    /**
     * @tc.steps: step2. change text with HandleOnUndoAction
     * @tc.expected: return value is valid
     */
    SourceAndValueInfo info;
    info.insertValue = "2";
    info.isIME = true;
    pattern_->InsertValueOperation(info);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(value, "2");
    EXPECT_EQ(didOffset, 1);
    EXPECT_EQ(didValue, "2");
}

/**
 * @tc.name: ChangeTextCallbackTest002
 * @tc.desc: test for callback SetOnWillDeleteEvent/SetOnDidDeleteEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, ChangeTextCallbackTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) { model.SetMaxLength(1000023); });
    GetFocus();
    TextDeleteDirection direction = TextDeleteDirection::BACKWARD;
    int32_t offset = 0;
    std::string value = "";

    auto onWillChange = [&offset, &value, &direction](const DeleteValueInfo& info) {
        offset = info.deleteOffset;
        value = info.deleteValue;
        direction = info.direction;
        return true;
    };
    eventHub_->SetOnWillDeleteEvent(std::move(onWillChange));

    TextDeleteDirection didDirection = TextDeleteDirection::BACKWARD;
    int32_t didOffset = 0;
    std::string didValue = "";
    auto onDidChange = [&didOffset, &didValue, &didDirection](const DeleteValueInfo& info) {
        didOffset = info.deleteOffset;
        didValue = info.deleteValue;
        didDirection = info.direction;
    };
    eventHub_->SetOnDidDeleteEvent(std::move(onDidChange));

    /**
     * @tc.steps: step2. change text with HandleOnUndoAction
     * @tc.expected: return value is valid
     */
    pattern_->SetCaretPosition(1);
    pattern_->DeleteBackwardOperation(1);
    EXPECT_EQ(direction, TextDeleteDirection::BACKWARD);
    EXPECT_EQ(offset, 1);
    EXPECT_EQ(value, "a");
    EXPECT_EQ(didDirection, TextDeleteDirection::BACKWARD);
    EXPECT_EQ(didOffset, 0);
    EXPECT_EQ(didValue, "a");
}

/**
 * @tc.name: ChangeTextCallbackTest003
 * @tc.desc: test for callback SetOnWillInsertValueEvent/SetOnDidInsertValueEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, ChangeTextCallbackTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField();
    GetFocus();
    int32_t offset = 0;
    std::string value = "";

    auto onWillChange = [&offset, &value](const InsertValueInfo& info) {
        offset = info.insertOffset;
        value = info.insertValue;
        return false;
    };
    eventHub_->SetOnWillInsertValueEvent(std::move(onWillChange));

    int32_t didOffset = -1;
    std::string didValue = "";
    auto onDidChange = [&didOffset, &didValue](const InsertValueInfo& info) {
        didOffset = info.insertOffset;
        didValue = info.insertValue;
    };
    eventHub_->SetOnDidInsertValueEvent(std::move(onDidChange));

    /**
     * @tc.steps: step2. change text with HandleOnUndoAction
     * @tc.expected: return value is valid
     */
    SourceAndValueInfo info;
    info.insertValue = "2";
    info.isIME = true;
    pattern_->InsertValueOperation(info);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(value, "2");
    EXPECT_EQ(didOffset, -1);
    EXPECT_EQ(didValue, "");
}

/**
 * @tc.name: ChangeTextCallbackTest04
 * @tc.desc: test for callback SetOnWillDeleteEvent/SetOnDidDeleteEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, ChangeTextCallbackTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) { model.SetMaxLength(1000023); });
    GetFocus();
    TextDeleteDirection direction = TextDeleteDirection::BACKWARD;
    int32_t offset = 0;
    std::string value = "";

    auto onWillChange = [&offset, &value, &direction](const DeleteValueInfo& info) {
        offset = info.deleteOffset;
        value = info.deleteValue;
        direction = info.direction;
        return false;
    };
    eventHub_->SetOnWillDeleteEvent(std::move(onWillChange));

    TextDeleteDirection didDirection = TextDeleteDirection::BACKWARD;
    int32_t didOffset = 0;
    std::string didValue = "";
    auto onDidChange = [&didOffset, &didValue, &didDirection](const DeleteValueInfo& info) {
        didOffset = info.deleteOffset;
        didValue = info.deleteValue;
        didDirection = info.direction;
    };
    eventHub_->SetOnDidDeleteEvent(std::move(onDidChange));

    /**
     * @tc.steps: step2. change text with HandleOnUndoAction
     * @tc.expected: return value is valid
     */
    pattern_->SetCaretPosition(1);
    pattern_->DeleteBackwardOperation(1);
    EXPECT_EQ(direction, TextDeleteDirection::BACKWARD);
    EXPECT_EQ(offset, 1);
    EXPECT_EQ(value, "a");
    EXPECT_EQ(didDirection, TextDeleteDirection::BACKWARD);
    EXPECT_EQ(didOffset, 0);
    EXPECT_EQ(didValue, "");
}

/**
 * @tc.name: ChangeTextCallbackTest005
 * @tc.desc: test for callback SetOnWillDeleteEvent/SetOnDidDeleteEvent
 * @tc.type: FUNC
 */
HWTEST_F(TextInputUpdateTestNg, ChangeTextCallbackTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize textInput and focusHub
     */
    CreateTextField(DEFAULT_TEXT, "", [](TextFieldModelNG model) { model.SetMaxLength(1000023); });
    GetFocus();
    TextDeleteDirection direction = TextDeleteDirection::BACKWARD;
    int32_t offset = 0;
    std::string value = "";

    auto onWillChange = [&offset, &value, &direction](const DeleteValueInfo& info) {
        offset = info.deleteOffset;
        value = info.deleteValue;
        direction = info.direction;
        return true;
    };
    eventHub_->SetOnWillDeleteEvent(std::move(onWillChange));

    TextDeleteDirection didDirection = TextDeleteDirection::BACKWARD;
    int32_t didOffset = 0;
    std::string didValue = "";
    auto onDidChange = [&didOffset, &didValue, &didDirection](const DeleteValueInfo& info) {
        didOffset = info.deleteOffset;
        didValue = info.deleteValue;
        didDirection = info.direction;
    };
    eventHub_->SetOnDidDeleteEvent(std::move(onDidChange));

    /**
     * @tc.steps: step2. change text with HandleOnUndoAction
     * @tc.expected: return value is valid
     */
    pattern_->SetCaretPosition(1);
    pattern_->DeleteForwardOperation(1);
    EXPECT_EQ(direction, TextDeleteDirection::FORWARD);
    EXPECT_EQ(offset, 1);
    EXPECT_EQ(value, "b");
    EXPECT_EQ(didDirection, TextDeleteDirection::FORWARD);
    EXPECT_EQ(didOffset, 1);
    EXPECT_EQ(didValue, "b");
}
} // namespace OHOS::Ace::NG

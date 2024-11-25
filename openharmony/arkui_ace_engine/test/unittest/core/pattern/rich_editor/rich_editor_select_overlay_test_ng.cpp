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

#include <memory>

#include "test/unittest/core/pattern/rich_editor/rich_editor_common_test_ng.h"

using namespace testing::ext;

namespace OHOS::Ace::NG {

class RichEditorSelectOverlayTestNg : public RichEditorCommonTestNg {
public:
    void SetUp() override;
    void TearDown() override;
    static void TearDownTestSuite();
};

void RichEditorSelectOverlayTestNg::SetUp()
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

void RichEditorSelectOverlayTestNg::TearDown()
{
    richEditorNode_.Reset();
    MockParagraph::TearDown();
}

void RichEditorSelectOverlayTestNg::TearDownTestSuite()
{
    TestNG::TearDownTestSuite();
}

/**
 * @tc.name: GetSelectArea
 * @tc.desc: test GetSelectArea
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, GetSelectArea, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->selectOverlay_->hasTransform_ = true;
    RectF rect(10, 10, 100, 100);
    auto selectArea = richEditorPattern->selectOverlay_->GetSelectArea();
    EXPECT_NE(selectArea.GetX(), rect.GetX());
    EXPECT_NE(selectArea.GetY(), rect.GetY());
    EXPECT_NE(selectArea.Width(), rect.Width());
    EXPECT_NE(selectArea.Height(), rect.Height());
}

/**
 * @tc.name: OnMenuItemAction
 * @tc.desc: test OnMenuItemAction
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnMenuItemAction, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->selectOverlay_->OnMenuItemAction(OptionMenuActionId::AI_WRITE, OptionMenuType::MOUSE_MENU);
    EXPECT_NE(richEditorPattern->aiWriteAdapter_, nullptr);
}

/**
 * @tc.name: OnHandleMoveStart001
 * @tc.desc: test OnHandleMoveStart
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMoveStart001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent info;
    auto manager = SelectContentOverlayManager::GetOverlayManager();
    ASSERT_NE(manager, nullptr);
    richEditorPattern->selectOverlay_->hasTransform_ = true;
    richEditorPattern->selectOverlay_->OnBind(manager);
    richEditorPattern->selectOverlay_->OnHandleMoveStart(info, true);
    EXPECT_FALSE(richEditorPattern->isCursorAlwaysDisplayed_);
}

/**
 * @tc.name: OnHandleMoveStart002
 * @tc.desc: test OnHandleMoveStart
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMoveStart002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent info;
    auto manager = SelectContentOverlayManager::GetOverlayManager();
    ASSERT_NE(manager, nullptr);
    richEditorPattern->selectOverlay_->hasTransform_ = true;
    richEditorPattern->selectOverlay_->OnBind(manager);
    richEditorPattern->selectOverlay_->isSingleHandle_ = true;
    richEditorPattern->selectOverlay_->OnHandleMoveStart(info, true);
    EXPECT_TRUE(richEditorPattern->isCursorAlwaysDisplayed_);
}

/**
 * @tc.name: OnOverlayTouchDown001
 * @tc.desc: test OnOverlayTouchDown
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnOverlayTouchDown001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    TouchEventInfo info("text");
    richEditorPattern->selectOverlay_->OnOverlayTouchDown(info);
    EXPECT_FALSE(richEditorPattern->isOnlyRequestFocus_);
}

/**
 * @tc.name: OnOverlayTouchDown002
 * @tc.desc: test OnOverlayTouchDown
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnOverlayTouchDown002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    TouchEventInfo info("text");
    info.SetSourceTool(SourceTool::MOUSE);
    richEditorPattern->selectOverlay_->OnOverlayTouchDown(info);
    EXPECT_EQ(info.GetSourceTool(), SourceTool::MOUSE);
    EXPECT_FALSE(richEditorPattern->isOnlyRequestFocus_);
}

/**
 * @tc.name: SwitchCaretState001
 * @tc.desc: test SwitchCaretState
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, SwitchCaretState001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    SelectOverlayInfo info;
    auto infoPar = std::make_shared<SelectOverlayInfo>(info);
    ASSERT_NE(infoPar, nullptr);
    richEditorPattern->selectOverlay_->isSingleHandle_ = true;
    richEditorPattern->selectOverlay_->SwitchCaretState(infoPar);
    EXPECT_FALSE(richEditorPattern->caretTwinkling_);
}

/**
 * @tc.name: SwitchCaretState002
 * @tc.desc: test SwitchCaretState
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, SwitchCaretState002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    SelectOverlayInfo info;
    auto infoPar = std::make_shared<SelectOverlayInfo>(info);
    ASSERT_NE(infoPar, nullptr);
    richEditorPattern->selectOverlay_->isSingleHandle_ = true;
    richEditorPattern->selectOverlay_->handleIsHidden_ = false;
    richEditorPattern->caretTwinkling_ = true;
    richEditorPattern->selectOverlay_->SwitchCaretState(infoPar);
    EXPECT_FALSE(richEditorPattern->caretTwinkling_);
}

/**
 * @tc.name: OnOverlayClick001
 * @tc.desc: test OnOverlayClick
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnOverlayClick001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent event;
    richEditorPattern->isEditing_ = true;
    richEditorPattern->selectOverlay_->OnOverlayClick(event, true);
    EXPECT_FALSE(richEditorPattern->selectOverlay_->needRefreshMenu_);
}

/**
 * @tc.name: OnOverlayClick002
 * @tc.desc: test OnOverlayClick
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnOverlayClick002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent event;
    richEditorPattern->selectOverlay_->OnOverlayClick(event, true);
    EXPECT_FALSE(richEditorPattern->selectOverlay_->needRefreshMenu_);
}

/**
 * @tc.name: OnOverlayClick003
 * @tc.desc: test OnOverlayClick
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnOverlayClick003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent event;
    richEditorPattern->isEditing_ = true;
    richEditorPattern->selectOverlay_->isSingleHandle_ = true;
    richEditorPattern->selectOverlay_->OnOverlayClick(event, true);
    EXPECT_FALSE(richEditorPattern->selectOverlay_->needRefreshMenu_);
}

/**
 * @tc.name: OnOverlayClick004
 * @tc.desc: test OnOverlayClick
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnOverlayClick004, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent event;
    richEditorPattern->selectOverlay_->isSingleHandle_ = true;
    richEditorPattern->selectOverlay_->OnOverlayClick(event, true);
    EXPECT_FALSE(richEditorPattern->selectOverlay_->needRefreshMenu_);
}

/**
 * @tc.name: OnHandleMouseEvent001
 * @tc.desc: test OnHandleMouseEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMouseEvent001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    MouseInfo event;
    event.SetAction(MouseAction::PRESS);
    richEditorPattern->selectOverlay_->OnHandleMouseEvent(event);
    EXPECT_EQ(event.GetAction(), MouseAction::PRESS);
}

/**
 * @tc.name: OnHandleMouseEvent002
 * @tc.desc: test OnHandleMouseEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMouseEvent002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    MouseInfo event;
    event.SetAction(MouseAction::RELEASE);
    richEditorPattern->selectOverlay_->OnHandleMouseEvent(event);
    EXPECT_EQ(event.GetAction(), MouseAction::RELEASE);
}

/**
 * @tc.name: OnAncestorNodeChanged
 * @tc.desc: test OnAncestorNodeChanged
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnAncestorNodeChanged, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    RefPtr<FrameNode> frameNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    richEditorNode_->parent_ = frameNode;
    FrameNodeChangeInfoFlag flag = FRAME_NODE_CHANGE_GEOMETRY_CHANGE;
    richEditorNode_->changeInfoFlag_ = FRAME_NODE_CHANGE_END_SCROLL;
    richEditorPattern->selectOverlay_->OnAncestorNodeChanged(flag);
    EXPECT_NE(richEditorNode_->changeInfoFlag_, FRAME_NODE_CHANGE_INFO_NONE);
}

/**
 * @tc.name: OnHandleMove001
 * @tc.desc: test OnHandleMove
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMove001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto focusHub = richEditorPattern->GetHost()->GetOrCreateFocusHub();
    ASSERT_NE(focusHub, nullptr);
    focusHub->currentFocus_ = true;
    AddImageSpan();
    richEditorPattern->selectOverlay_->hasTransform_ = true;
    richEditorPattern->ShowSelectOverlay(
        richEditorPattern->textSelector_.firstHandle, richEditorPattern->textSelector_.secondHandle, false);
    richEditorPattern->selectOverlay_->SetHandleLevelMode(HandleLevelMode::EMBED);
    richEditorPattern->selectOverlay_->OnHandleMove(RectF(1.0f, 0.0f, 10.0f, 10.0f), false);
    EXPECT_NE(richEditorPattern->textSelector_.firstHandle.GetOffset().GetX(), 1.0f);
}

/**
 * @tc.name: OnHandleMove002
 * @tc.desc: test OnHandleMove
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMove002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto focusHub = richEditorPattern->GetHost()->GetOrCreateFocusHub();
    ASSERT_NE(focusHub, nullptr);
    focusHub->currentFocus_ = true;
    AddImageSpan();
    richEditorPattern->selectOverlay_->handleLevelMode_ = HandleLevelMode::EMBED;
    richEditorPattern->selectOverlay_->hasTransform_ = true;
    richEditorPattern->ShowSelectOverlay(
        richEditorPattern->textSelector_.firstHandle, richEditorPattern->textSelector_.secondHandle, false);
    richEditorPattern->selectOverlay_->SetHandleLevelMode(HandleLevelMode::EMBED);
    richEditorPattern->selectOverlay_->OnHandleMove(RectF(1.0f, 0.0f, 10.0f, 10.0f), false);
    EXPECT_NE(richEditorPattern->textSelector_.firstHandle.GetOffset().GetX(), 1.0f);
}

/**
 * @tc.name: OnHandleMoveDone001
 * @tc.desc: test OnHandleMoveDone
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMoveDone001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto manager = SelectContentOverlayManager::GetOverlayManager();
    ASSERT_NE(manager, nullptr);
    richEditorPattern->selectOverlay_->OnBind(manager);
    RectF handleRect;
    richEditorPattern->selectOverlay_->isSingleHandle_ = true;
    richEditorPattern->selectOverlay_->OnHandleMoveDone(handleRect, false);
    EXPECT_TRUE(richEditorPattern->selectOverlay_->originalMenuIsShow_);
}

/**
 * @tc.name: OnHandleMoveDone002
 * @tc.desc: test OnHandleMoveDone
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorSelectOverlayTestNg, OnHandleMoveDone002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto manager = SelectContentOverlayManager::GetOverlayManager();
    ASSERT_NE(manager, nullptr);
    richEditorPattern->selectOverlay_->OnBind(manager);
    RectF handleRect;
    richEditorPattern->selectOverlay_->isSingleHandle_ = false;
    richEditorPattern->selectOverlay_->OnHandleMoveDone(handleRect, true);
    EXPECT_FALSE(richEditorPattern->selectOverlay_->originalMenuIsShow_);
}
} // namespace OHOS::Ace::NG

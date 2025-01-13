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
#include "test/unittest/core/pattern/rich_editor/rich_editor_common_test_ng.h"
#include "test/mock/core/render/mock_canvas_image.h"
#include "test/mock/core/render/mock_paragraph.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/base/mock_task_executor.h"
#include "test/mock/core/common/mock_data_url_analyzer_mgr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {} // namespace
class RichEditorPatternTestSixNg : public RichEditorCommonTestNg {
public:
    void SetUp() override;
    void TearDown() override;
    void InitDataUrlAnalyzer(MockDataUrlAnalyzerMgr& mockDataUrlAnalyzerMgr);
    static void TearDownTestSuite();
};

void RichEditorPatternTestSixNg::SetUp()
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

void RichEditorPatternTestSixNg::TearDown()
{
    richEditorNode_ = nullptr;
    MockParagraph::TearDown();
}

void RichEditorPatternTestSixNg::TearDownTestSuite()
{
    TestNG::TearDownTestSuite();
}

void RichEditorPatternTestSixNg::InitDataUrlAnalyzer(MockDataUrlAnalyzerMgr& mockDataUrlAnalyzerMgr)
{
    EXPECT_CALL(mockDataUrlAnalyzerMgr, AnalyzeUrls(_))
        .WillRepeatedly([](const std::string& text) -> std::vector<UrlEntity> {
            std::vector<UrlEntity> data;
            if (text.empty()) {
                return data;
            }
            UrlEntity urlEntity;
            urlEntity.text = text;
            urlEntity.charOffset = text.length();
            data.push_back(urlEntity);
            return data;
        });
}

/*
 * @tc.name: AnalyzeUrls001
 * @tc.desc: test url Analyzer
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, AnalyzeUrls001, TestSize.Level1)
{
    MockDataUrlAnalyzerMgr mockDataUrlAnalyzerMgr;
    InitDataUrlAnalyzer(mockDataUrlAnalyzerMgr);

    std::string text = "";
    std::vector<UrlEntity> data = mockDataUrlAnalyzerMgr.AnalyzeUrls(text);
    EXPECT_TRUE(data.empty());

    text = "test1";
    data = mockDataUrlAnalyzerMgr.AnalyzeUrls(text);
    EXPECT_FALSE(data.empty());
}

/**
 * @tc.name: GetSelectSpanInfo001
 * @tc.desc: Test GetSelectSpanInfo function
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, ClickAISpan001, TestSize.Level1)
{
    PointF textOffset = {100.0, 100.0};
    AISpan aiSpan = {0, 10, "Test", TextDataDetectType::PHONE_NUMBER};
    ParagraphManager::ParagraphInfo info;
    RefPtr<MockParagraph> mockParagraph = AceType::MakeRefPtr<MockParagraph>();
    info.paragraph = mockParagraph;
    info.start = 3;
    info.end = 10;
    EXPECT_CALL(*mockParagraph, GetHeight())
        .WillRepeatedly(Return(10.0f));
    EXPECT_CALL(*mockParagraph, GetRectsForRange(_, _, _))
        .WillRepeatedly(Invoke([](int32_t start, int32_t end, std::vector<RectF>& selectedRects) {
            selectedRects.emplace_back(RectF(0, 0, 100, 100));
        }));
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    richEditorPattern->paragraphs_.paragraphs_.emplace_back(info);
    ASSERT_NE(richEditorPattern, nullptr);
    EXPECT_TRUE(richEditorPattern->ClickAISpan(textOffset, aiSpan));
}

/**
 * @tc.name: GetSelectSpanInfo001
 * @tc.desc: Test GetSelectSpanInfo function
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, ClickAISpan002, TestSize.Level1)
{
    PointF textOffset = {100.0, 100.0};
    AISpan aiSpan = {0, 10, "Test", TextDataDetectType::PHONE_NUMBER};
    ParagraphManager::ParagraphInfo info;
    RefPtr<MockParagraph> mockParagraph = AceType::MakeRefPtr<MockParagraph>();
    info.paragraph = mockParagraph;
    info.start = 3;
    info.end = 10;
    EXPECT_CALL(*mockParagraph, GetHeight())
        .WillRepeatedly(Return(10.0f));
    EXPECT_CALL(*mockParagraph, GetRectsForRange(_, _, _))
        .WillRepeatedly(Invoke([](int32_t start, int32_t end, std::vector<RectF>& selectedRects) {
            selectedRects.emplace_back(RectF(0, 0, 100, 100));
        }));
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    richEditorPattern->paragraphs_.paragraphs_.emplace_back(info);
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->leftMousePress_ = true;
    EXPECT_TRUE(richEditorPattern->ClickAISpan(textOffset, aiSpan));
}

/**
 * @tc.name: GetSelectSpanInfo001
 * @tc.desc: Test GetSelectSpanInfo function
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, ClickAISpan003, TestSize.Level1)
{
    PointF textOffset = {100.0, 100.0};
    AISpan aiSpan = {0, 10, "Test", TextDataDetectType::PHONE_NUMBER};
    ParagraphManager::ParagraphInfo info;
    RefPtr<MockParagraph> mockParagraph = AceType::MakeRefPtr<MockParagraph>();
    info.paragraph = mockParagraph;
    info.start = 3;
    info.end = 10;
    EXPECT_CALL(*mockParagraph, GetHeight())
        .WillRepeatedly(Return(10.0f));
    EXPECT_CALL(*mockParagraph, GetRectsForRange(_, _, _))
        .WillRepeatedly(Invoke([](int32_t start, int32_t end, std::vector<RectF>& selectedRects) {
            selectedRects.emplace_back(RectF(0, 0, 100, 20));
        }));
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    richEditorPattern->paragraphs_.paragraphs_.emplace_back(info);
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->leftMousePress_ = true;
    EXPECT_FALSE(richEditorPattern->ClickAISpan(textOffset, aiSpan));
}

/**
 * @tc.name: GetSelectSpanInfo001
 * @tc.desc: Test GetSelectSpanInfo function
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, ClickAISpan004, TestSize.Level1)
{
    PointF textOffset = {100.0, 100.0};
    AISpan aiSpan = {0, 10, "Test", TextDataDetectType::PHONE_NUMBER};
    ParagraphManager::ParagraphInfo info;
    RefPtr<MockParagraph> mockParagraph = AceType::MakeRefPtr<MockParagraph>();
    info.paragraph = mockParagraph;
    info.start = 3;
    info.end = 10;
    EXPECT_CALL(*mockParagraph, GetHeight())
        .WillRepeatedly(Return(10.0f));
    EXPECT_CALL(*mockParagraph, GetRectsForRange(_, _, _))
        .WillRepeatedly(Invoke([](int32_t start, int32_t end, std::vector<RectF>& selectedRects) {
            selectedRects.emplace_back(RectF(0, 0, 100, 20));
        }));
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    richEditorPattern->paragraphs_.paragraphs_.emplace_back(info);
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->leftMousePress_ = false;
    EXPECT_FALSE(richEditorPattern->ClickAISpan(textOffset, aiSpan));
}

/**
 * @tc.name: InsertValueInStyledString001
 * @tc.desc: test InsertValueInStyledString
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, InsertValueInStyledString001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->styledString_ = AceType::MakeRefPtr<MutableSpanString>(INIT_VALUE_3);
    richEditorPattern->InsertValueInStyledString(PREVIEW_TEXT_VALUE1);
    EXPECT_FALSE(richEditorPattern->textSelector_.IsValid());
}

/**
 * @tc.name: DeleteValueInStyledString001
 * @tc.desc: test DeleteValueInStyledString
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteValueInStyledString001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->styledString_ = AceType::MakeRefPtr<MutableSpanString>(INIT_VALUE_3);
    richEditorPattern->previewLongPress_ = true;
    auto focusHub = richEditorPattern->GetFocusHub();
    focusHub->currentFocus_ = true;
    richEditorPattern->DeleteValueInStyledString(0, 10, true, false);
    EXPECT_FALSE(richEditorPattern->previewLongPress_);
}

/**
 * @tc.name: OnModifyDone001
 * @tc.desc: test OnModifyDone
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, OnModifyDone001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->clipboard_ = nullptr;
    richEditorPattern->OnModifyDone();
    EXPECT_TRUE(richEditorPattern->clipboard_);
}

/**
 * @tc.name: SetSelfAndChildDraggableFalse001
 * @tc.desc: test SetSelfAndChildDraggableFalse
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, SetSelfAndChildDraggableFalse001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    int32_t nodeId = 2;
    RefPtr<Pattern> pattern = AceType::MakeRefPtr<Pattern>();
    RefPtr<FrameNode> customNode = AceType::MakeRefPtr<FrameNode>(V2::IMAGE_ETS_TAG, nodeId, pattern, false, false);
    auto eventHub = customNode->GetEventHub<EventHub>();
    RefPtr<EventHub> sharedEventHub = RefPtr<EventHub>(new EventHub());
    WeakPtr<EventHub> weakEventHub(sharedEventHub);
    eventHub->gestureEventHub_ = AceType::MakeRefPtr<GestureEventHub>(weakEventHub);
    eventHub->gestureEventHub_->isDragForbiddenForWholeSubTree_ = false;
    richEditorPattern->SetSelfAndChildDraggableFalse(customNode);
    EXPECT_TRUE(eventHub->gestureEventHub_->isDragForbiddenForWholeSubTree_);
}

/**
 * @tc.name: AfterContentChange001
 * @tc.desc: test AfterContentChange
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, AfterContentChange001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorNode_.Reset();
    RichEditorChangeValue changeValue;
    richEditorPattern->AfterContentChange(changeValue);
    EXPECT_EQ(richEditorNode_, nullptr);
}

/**
 * @tc.name: DeleteSpans001
 * @tc.desc: test DeleteSpans
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteSpans001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RangeOptions options;
    options.start = 0;
    options.end = 1;
    richEditorPattern->isSpanStringMode_ = true;
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    richEditorPattern->spans_.push_back(spanItem);
    richEditorPattern->styledString_ = AceType::MakeRefPtr<MutableSpanString>(INIT_VALUE_3);
    auto eventHub = richEditorNode_->GetEventHub<RichEditorEventHub>();
    eventHub->SetOnWillChange([](const RichEditorChangeValue& value) -> bool { return false; });
    richEditorPattern->redoOperationRecords_.emplace_back();
    richEditorPattern->DeleteSpans(options);
    EXPECT_FALSE(richEditorPattern->redoOperationRecords_.empty());
}

/**
 * @tc.name: DeleteSpansOperation001
 * @tc.desc: test DeleteSpansOperation
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteSpansOperation001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->isSpanStringMode_ = true;
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    richEditorPattern->spans_.push_back(spanItem);
    richEditorPattern->styledString_ = AceType::MakeRefPtr<MutableSpanString>(u"");
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto newFrameNode = ImageSpanNode::GetOrCreateSpanNode(
        V2::IMAGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    richEditorNode_->children_.push_back(newFrameNode);
    richEditorPattern->DeleteSpansOperation(0, 5);
    EXPECT_EQ(richEditorPattern->textForDisplay_.size(), 0);
}

/**
 * @tc.name: RemoveEmptySpanNodes001
 * @tc.desc: test RemoveEmptySpanNodes
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, RemoveEmptySpanNodes001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto newFrameNode = ImageSpanNode::GetOrCreateSpanNode(
        V2::IMAGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    auto newAddFrameNode = ImageSpanNode::GetOrCreateSpanNode(
        V2::IMAGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    richEditorNode_->children_.push_back(newFrameNode);
    richEditorNode_->children_.push_back(newAddFrameNode);
    richEditorPattern->RemoveEmptySpanNodes();
    bool nonSpanNodeStillExists = false;
    for (const auto& child : richEditorNode_->children_) {
        if (child == newFrameNode) {
            nonSpanNodeStillExists = true;
            break;
        }
    }
    EXPECT_TRUE(nonSpanNodeStillExists);
}

/**
 * @tc.name: RemoveEmptySpanNodes002
 * @tc.desc: test RemoveEmptySpanNodes
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, RemoveEmptySpanNodes002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto newFrameNode = SpanNode::GetOrCreateSpanNode(V2::IMAGE_ETS_TAG, nodeId);
    auto newAddFrameNode = SpanNode::GetOrCreateSpanNode(V2::IMAGE_ETS_TAG, nodeId);
    newAddFrameNode->GetSpanItem()->content = INIT_VALUE_3;
    richEditorNode_->children_.push_back(newFrameNode);
    richEditorNode_->children_.push_back(newAddFrameNode);
    richEditorPattern->RemoveEmptySpanNodes();
    bool emptySpanNodeRemoved = true;
    for (const auto& child : richEditorNode_->children_) {
        auto spanNode = AceType::DynamicCast<SpanNode>(child);
        if (spanNode && spanNode->GetSpanItem()->content.empty()) {
            emptySpanNodeRemoved = false;
            break;
        }
    }
    EXPECT_TRUE(emptySpanNodeRemoved);
}

/**
 * @tc.name: ClearContent001
 * @tc.desc: test ClearContent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, ClearContent001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto child = SpanNode::GetOrCreateSpanNode(V2::SPAN_ETS_TAG, nodeId);
    richEditorPattern->ClearContent(child);
    EXPECT_EQ(child->spanItem_->content, u"");
}

/**
 * @tc.name: HandleSingleClickEvent002
 * @tc.desc: test RichEditorPattern HandleSingleClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleSingleClickEvent002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent info = GestureEvent();
    richEditorNode_->eventHub_->focusHub_->focusType_ = FocusType::DISABLE;
    richEditorPattern->caretVisible_ = false;
    richEditorPattern->HandleSingleClickEvent(info);
    EXPECT_FALSE(richEditorPattern->caretVisible_);
}

/**
 * @tc.name: HandleSingleClickEvent003
 * @tc.desc: test RichEditorPattern HandleSingleClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleSingleClickEvent003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent info = GestureEvent();
    richEditorNode_.Reset();
    richEditorPattern->caretVisible_ = false;
    richEditorPattern->HandleSingleClickEvent(info);
    EXPECT_FALSE(richEditorPattern->caretVisible_);
}

/**
 * @tc.name: HandleUserLongPressEvent001
 * @tc.desc: test HandleUserLongPressEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleUserLongPressEvent001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    auto gestureEventFunc = [](GestureEvent& info) { return true; };
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    spanItem->onLongPress = std::move(gestureEventFunc);
    richEditorPattern->spans_.push_back(spanItem);

    AddSpan(EXCEPT_VALUE);
    ASSERT_FALSE(richEditorPattern->spans_.empty());
    auto firstSpanItem = richEditorPattern->spans_.front();
    ASSERT_NE(firstSpanItem, nullptr);
    firstSpanItem->leadingMargin = std::make_optional<NG::LeadingMargin>();
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 10 });
    std::vector<RectF> rects { RectF(0, 0, 5, 5) };
    EXPECT_CALL(*paragraph, GetRectsForRange(_, _, _)).WillRepeatedly(SetArgReferee<THIRD_PARAM>(rects));
    EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
    GestureEvent info = GestureEvent();
    info.SetLocalLocation(Offset(3, 3));
    richEditorPattern->contentRect_ = RectF(0, 0, 20.0, 20.0);
    auto gestureFunc = [](RefPtr<SpanItem> item, GestureEvent& info) -> bool { return true; };
    richEditorPattern->HandleUserGestureEvent(info, std::move(gestureFunc));

    bool ret = richEditorPattern->HandleUserLongPressEvent(info);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: HandleUserLongPressEvent002
 * @tc.desc: test HandleUserLongPressEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleUserLongPressEvent002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    richEditorPattern->spans_.push_back(spanItem);

    AddSpan(EXCEPT_VALUE);
    ASSERT_FALSE(richEditorPattern->spans_.empty());
    auto firstSpanItem = richEditorPattern->spans_.front();
    ASSERT_NE(firstSpanItem, nullptr);
    firstSpanItem->leadingMargin = std::make_optional<NG::LeadingMargin>();
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 10 });
    std::vector<RectF> rects { RectF(0, 0, 5, 5) };
    EXPECT_CALL(*paragraph, GetRectsForRange(_, _, _)).WillRepeatedly(SetArgReferee<THIRD_PARAM>(rects));
    EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
    GestureEvent info = GestureEvent();
    info.SetLocalLocation(Offset(3, 3));
    richEditorPattern->contentRect_ = RectF(0, 0, 20.0, 20.0);
    auto gestureFunc = [](RefPtr<SpanItem> item, GestureEvent& info) -> bool { return true; };
    richEditorPattern->HandleUserGestureEvent(info, std::move(gestureFunc));

    bool ret = richEditorPattern->HandleUserLongPressEvent(info);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: HandleUserLongPressEvent003
 * @tc.desc: test HandleUserLongPressEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleUserLongPressEvent003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    AddSpan(EXCEPT_VALUE);
    ASSERT_FALSE(richEditorPattern->spans_.empty());
    auto firstSpanItem = richEditorPattern->spans_.front();
    ASSERT_NE(firstSpanItem, nullptr);
    firstSpanItem->leadingMargin = std::make_optional<NG::LeadingMargin>();
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 10 });
    std::vector<RectF> rects { RectF(0, 0, 5, 5) };
    EXPECT_CALL(*paragraph, GetRectsForRange(_, _, _)).WillRepeatedly(SetArgReferee<THIRD_PARAM>(rects));
    EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
    GestureEvent info = GestureEvent();
    info.SetLocalLocation(Offset(3, 3));
    richEditorPattern->contentRect_ = RectF(0, 0, 20.0, 20.0);
    auto gestureFunc = [](RefPtr<SpanItem> item, GestureEvent& info) -> bool { return true; };
    richEditorPattern->HandleUserGestureEvent(info, std::move(gestureFunc));

    bool ret = richEditorPattern->HandleUserLongPressEvent(info);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: HandleUserClickEvent001
 * @tc.desc: test HandleUserClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleUserClickEvent001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    auto gestureEventFunc = [](GestureEvent& info) { return true; };
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    spanItem->onClick = std::move(gestureEventFunc);
    richEditorPattern->spans_.push_back(spanItem);

    AddSpan(EXCEPT_VALUE);
    ASSERT_FALSE(richEditorPattern->spans_.empty());
    auto firstSpanItem = richEditorPattern->spans_.front();
    ASSERT_NE(firstSpanItem, nullptr);
    firstSpanItem->leadingMargin = std::make_optional<NG::LeadingMargin>();
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 10 });
    std::vector<RectF> rects { RectF(0, 0, 5, 5) };
    EXPECT_CALL(*paragraph, GetRectsForRange(_, _, _)).WillRepeatedly(SetArgReferee<THIRD_PARAM>(rects));
    EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
    GestureEvent info = GestureEvent();
    info.SetLocalLocation(Offset(3, 3));
    richEditorPattern->contentRect_ = RectF(0, 0, 20.0, 20.0);
    auto gestureFunc = [](RefPtr<SpanItem> item, GestureEvent& info) -> bool { return true; };
    richEditorPattern->HandleUserGestureEvent(info, std::move(gestureFunc));

    bool ret = richEditorPattern->HandleUserClickEvent(info);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: HandleUserClickEvent002
 * @tc.desc: test HandleUserClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleUserClickEvent002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    richEditorPattern->spans_.push_back(spanItem);

    AddSpan(EXCEPT_VALUE);
    ASSERT_FALSE(richEditorPattern->spans_.empty());
    auto firstSpanItem = richEditorPattern->spans_.front();
    ASSERT_NE(firstSpanItem, nullptr);
    firstSpanItem->leadingMargin = std::make_optional<NG::LeadingMargin>();
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 10 });
    std::vector<RectF> rects { RectF(0, 0, 5, 5) };
    EXPECT_CALL(*paragraph, GetRectsForRange(_, _, _)).WillRepeatedly(SetArgReferee<THIRD_PARAM>(rects));
    EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
    GestureEvent info = GestureEvent();
    info.SetLocalLocation(Offset(3, 3));
    richEditorPattern->contentRect_ = RectF(0, 0, 20.0, 20.0);
    auto gestureFunc = [](RefPtr<SpanItem> item, GestureEvent& info) -> bool { return true; };
    richEditorPattern->HandleUserGestureEvent(info, std::move(gestureFunc));

    bool ret = richEditorPattern->HandleUserClickEvent(info);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: HandleUserClickEvent003
 * @tc.desc: test HandleUserClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleUserClickEvent003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    AddSpan(EXCEPT_VALUE);
    ASSERT_FALSE(richEditorPattern->spans_.empty());
    auto firstSpanItem = richEditorPattern->spans_.front();
    ASSERT_NE(firstSpanItem, nullptr);
    firstSpanItem->leadingMargin = std::make_optional<NG::LeadingMargin>();
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    richEditorPattern->paragraphs_.AddParagraph({ .paragraph = paragraph, .start = 0, .end = 10 });
    std::vector<RectF> rects { RectF(0, 0, 5, 5) };
    EXPECT_CALL(*paragraph, GetRectsForRange(_, _, _)).WillRepeatedly(SetArgReferee<THIRD_PARAM>(rects));
    EXPECT_CALL(*paragraph, GetHeight).WillRepeatedly(Return(50));
    GestureEvent info = GestureEvent();
    info.SetLocalLocation(Offset(3, 3));
    richEditorPattern->contentRect_ = RectF(0, 0, 20.0, 20.0);
    auto gestureFunc = [](RefPtr<SpanItem> item, GestureEvent& info) -> bool { return true; };
    richEditorPattern->HandleUserGestureEvent(info, std::move(gestureFunc));

    bool ret = richEditorPattern->HandleUserClickEvent(info);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ProcessInsertValue001
 * @tc.desc: test ProcessInsertValue
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, ProcessInsertValue001, TestSize.Level1)
{
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    std::u16string insertValue = u"abc";
    richEditorPattern->isEditing_ = true;
    richEditorPattern->isSpanStringMode_ = false;
    auto size = richEditorPattern->operationRecords_.size();
    richEditorPattern->ProcessInsertValue(insertValue, OperationType::IME, true);
    EXPECT_TRUE(richEditorPattern->IsEditing());
    EXPECT_NE(size, richEditorPattern->operationRecords_.size());
}

/**
 * @tc.name: AddPlaceholderSpan001
 * @tc.desc: test AddPlaceholderSpan
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, AddPlaceholderSpan001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    auto builderNode = FrameNode::GetOrCreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
    SpanOptionBase options = {};
    auto host = richEditorPattern->GetHost();
    EXPECT_NE(host, nullptr);
    AddSpan("test");
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem = AceType::MakeRefPtr<PlaceholderSpanItem>();
    richEditorPattern->spans_.emplace_back(spanItem);
    richEditorPattern->spans_.emplace_back(spanItem);
    richEditorPattern->spans_.emplace_back(spanItem);
    auto ret = richEditorPattern->AddPlaceholderSpan(builderNode, {});
    EXPECT_NE(ret, host->GetChildren().size());
}

/**
 * @tc.name: HandleDraggableFlag001
 * @tc.desc: test HandleDraggableFlag
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleDraggableFlag001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto eventHubRefPtr = AceType::MakeRefPtr<EventHub>();
    auto eventHubWeakPtr = AceType::WeakClaim(AceType::RawPtr(eventHubRefPtr));
    richEditorNode_->eventHub_->gestureEventHub_ = AceType::MakeRefPtr<GestureEventHub>(eventHubWeakPtr);
    richEditorPattern->copyOption_ = CopyOptions::InApp;
    richEditorNode_->eventHub_->gestureEventHub_->isTextDraggable_ = true;
    richEditorPattern->HandleDraggableFlag(true);
    EXPECT_FALSE(richEditorNode_->eventHub_->gestureEventHub_->isTextDraggable_);
}

/**
 * @tc.name: OnDragEnd001
 * @tc.desc: test OnDragEnd
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, OnDragEnd001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    bool isTestAddObject = false;
    ResultObject resultObject;
    if (richEditorPattern->recoverDragResultObjects_.empty()) {
        isTestAddObject = true;
        richEditorPattern->recoverDragResultObjects_.emplace_back(resultObject);
    }

    auto event = AceType::MakeRefPtr<Ace::DragEvent>();
    richEditorPattern->showSelect_ = false;
    richEditorNode_.Reset();
    richEditorPattern->OnDragEnd(event);
    EXPECT_FALSE(richEditorPattern->showSelect_);
}

/**
 * @tc.name: OnDragEnd002
 * @tc.desc: test OnDragEnd
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, OnDragEnd002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    bool isTestAddObject = false;
    ResultObject resultObject;
    if (richEditorPattern->recoverDragResultObjects_.empty()) {
        isTestAddObject = true;
        richEditorPattern->recoverDragResultObjects_.emplace_back(resultObject);
    }

    auto event = AceType::MakeRefPtr<Ace::DragEvent>();
    richEditorPattern->showSelect_ = false;
    event->dragRet_ = DragRet::DRAG_SUCCESS;
    richEditorPattern->OnDragEnd(event);
    EXPECT_FALSE(richEditorPattern->showSelect_);
}

/**
 * @tc.name: OnDragEnd003
 * @tc.desc: test OnDragEnd
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, OnDragEnd003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    bool isTestAddObject = false;
    ResultObject resultObject;
    if (richEditorPattern->recoverDragResultObjects_.empty()) {
        isTestAddObject = true;
        richEditorPattern->recoverDragResultObjects_.emplace_back(resultObject);
    }

    auto event = AceType::MakeRefPtr<Ace::DragEvent>();
    richEditorPattern->showSelect_ = false;
    richEditorNode_->eventHub_->focusHub_->focusType_ = FocusType::DISABLE;
    richEditorPattern->OnDragEnd(event);
    EXPECT_FALSE(richEditorPattern->showSelect_);
}

/**
 * @tc.name: GetImageStyleBySpanItem002
 * @tc.desc: test GetImageStyleBySpanItem
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, GetImageStyleBySpanItem002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    ImageSpanAttribute imageStyle;
    std::optional<Ace::NG::MarginProperty> marginProp = std::nullopt;
    std::optional<Ace::NG::BorderRadiusProperty> borderRadius = std::nullopt;
    marginProp = { CALC_LENGTH_CALC, CALC_LENGTH_CALC, CALC_LENGTH_CALC, CALC_LENGTH_CALC };
    borderRadius = { CALC_TEST, CALC_TEST, CALC_TEST, CALC_TEST };
    imageStyle.marginProp = marginProp;
    imageStyle.borderRadius = borderRadius;
    ImageSpanSize imageSize;
    imageSize.height = 1;
    imageStyle.size = imageSize;
    imageStyle.verticalAlign = VerticalAlign::TOP;
    imageStyle.objectFit = ImageFit::COVER;

    auto spanItem = AceType::MakeRefPtr<ImageSpanItem>();
    ImageSpanOptions option;
    option.imageAttribute = imageStyle;
    spanItem->options = option;
    auto imageSizeOp = spanItem->options.imageAttribute->size;
    auto ret = richEditorPattern->GetImageStyleBySpanItem(spanItem);
    EXPECT_NE(ret.size[RichEditorImageSize::SIZEHEIGHT], imageSizeOp->height->ConvertToPx());
}

/**
 * @tc.name: GetImageStyleBySpanItem003
 * @tc.desc: test GetImageStyleBySpanItem
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, GetImageStyleBySpanItem003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    ImageSpanAttribute imageStyle;
    std::optional<Ace::NG::MarginProperty> marginProp = std::nullopt;
    std::optional<Ace::NG::BorderRadiusProperty> borderRadius = std::nullopt;
    marginProp = { CALC_LENGTH_CALC, CALC_LENGTH_CALC, CALC_LENGTH_CALC, CALC_LENGTH_CALC };
    borderRadius = { CALC_TEST, CALC_TEST, CALC_TEST, CALC_TEST };
    imageStyle.marginProp = marginProp;
    imageStyle.borderRadius = borderRadius;
    ImageSpanSize imageSize;
    imageSize.width = 1;
    imageStyle.size = imageSize;
    imageStyle.verticalAlign = VerticalAlign::TOP;
    imageStyle.objectFit = ImageFit::COVER;

    auto spanItem = AceType::MakeRefPtr<ImageSpanItem>();
    ImageSpanOptions option;
    option.imageAttribute = imageStyle;
    spanItem->options = option;
    auto imageSizeOp = spanItem->options.imageAttribute->size;
    auto ret = richEditorPattern->GetImageStyleBySpanItem(spanItem);
    EXPECT_NE(ret.size[RichEditorImageSize::SIZEWIDTH], imageSizeOp->width->ConvertToPx());
}

/**
 * @tc.name: DeleteBackwardOperation002
 * @tc.desc: test DeleteBackwardOperation
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteBackwardOperation002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(EXCEPT_VALUE);
    richEditorPattern->caretPosition_ = 100;
    richEditorPattern->previewLongPress_ = true;
    richEditorNode_.Reset();
    auto ret = richEditorPattern->DeleteBackwardOperation(0);
    EXPECT_EQ(ret, u"");
}

/**
 * @tc.name: DeleteForwardOperation001
 * @tc.desc: test DeleteForwardOperation
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteForwardOperation001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    int32_t length = 1;
    AddSpan(EXCEPT_VALUE);
    richEditorNode_.Reset();
    auto ret = richEditorPattern->DeleteForwardOperation(length);
    EXPECT_EQ(ret, u"");
}

/**
 * @tc.name: DeleteForwardOperation002
 * @tc.desc: test DeleteForwardOperation
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteForwardOperation002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    int32_t length = 1;
    ClearSpan();
    richEditorPattern->caretPosition_ = 100;
    richEditorPattern->DeleteForwardOperation(length);
    EXPECT_TRUE(richEditorPattern->spans_.empty());
}

/**
 * @tc.name: GetRightWordPosition004
 * @tc.desc: test GetRightWordPosition
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, GetRightWordPosition004, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    ClearSpan();
    std::string firstText = "text";
    AddSpan(firstText);
    std::string space = " ";
    std::string secondText = "content";
    AddSpan(space + secondText);
    for (auto iter = richEditorPattern->spans_.cbegin(); iter != richEditorPattern->spans_.cend(); iter++) {
        auto span = *iter;
        span->placeholderIndex = 0;
    }
    int32_t initCaretPosition = firstText.size();
    auto position =
        std::clamp(initCaretPosition + 1, 0, static_cast<int32_t>(richEditorPattern->GetTextContentLength()));
    EXPECT_EQ(richEditorPattern->GetRightWordPosition(initCaretPosition), position);
}

/**
 * @tc.name: GetRightWordPosition005
 * @tc.desc: test GetRightWordPosition
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, GetRightWordPosition005, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    ClearSpan();
    std::string firstText = "text ";
    AddSpan(firstText);
    std::string secondText = "content";
    AddSpan(secondText);
    for (auto iter = richEditorPattern->spans_.cbegin(); iter != richEditorPattern->spans_.cend(); iter++) {
        auto span = *iter;
        span->placeholderIndex = 0;
    }
    int32_t initCaretPosition = firstText.size() + secondText.size() - 1;
    auto position =
        std::clamp(initCaretPosition + 1, 0, static_cast<int32_t>(richEditorPattern->GetTextContentLength()));
    EXPECT_EQ(richEditorPattern->GetRightWordPosition(initCaretPosition), position);
}

/**
 * @tc.name: CaretPositionSelectEmoji001
 * @tc.desc: test CaretPositionSelectEmoji
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, CaretPositionSelectEmoji001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    CaretMoveIntent direction = CaretMoveIntent::Left;
    auto ret = richEditorPattern->CaretPositionSelectEmoji(direction);
    EXPECT_EQ(ret, richEditorPattern->caretPosition_ - 1);
}

/**
 * @tc.name: CaretPositionSelectEmoji002
 * @tc.desc: test CaretPositionSelectEmoji
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, CaretPositionSelectEmoji002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    CaretMoveIntent direction = CaretMoveIntent::Right;
    auto ret = richEditorPattern->CaretPositionSelectEmoji(direction);
    EXPECT_EQ(ret, richEditorPattern->caretPosition_ + 1);
}

/**
 * @tc.name: CaretPositionSelectEmoji003
 * @tc.desc: test CaretPositionSelectEmoji
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, CaretPositionSelectEmoji003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    CaretMoveIntent direction = CaretMoveIntent::Up;
    auto ret = richEditorPattern->CaretPositionSelectEmoji(direction);
    EXPECT_EQ(ret, richEditorPattern->caretPosition_);
}

/**
 * @tc.name: DeleteValueSetImageSpan002
 * @tc.desc: test DeleteValueSetImageSpan
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteValueSetImageSpan002, TestSize.Level1)
{
    AddImageSpan();
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(richEditorNode_->GetLastChild());
    ASSERT_NE(imageNode, nullptr);
    auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    imageLayoutProperty->UpdateImageFit(ImageFit::FILL);
    imageLayoutProperty->UpdateVerticalAlign(VerticalAlign::CENTER);
    RichEditorAbstractSpanResult spanResult;
    spanResult.SetSpanIndex(richEditorNode_->GetChildIndexById(imageNode->GetId()));

    auto spans = richEditorPattern->GetSpanItemChildren();
    ASSERT_FALSE(spans.empty());
    auto imageSpanItem = AceType::DynamicCast<ImageSpanItem>(spans.back());
    ASSERT_NE(imageSpanItem, nullptr);
    void* voidPtr = static_cast<void*>(new char[0]);
    RefPtr<PixelMap> pixelMap = PixelMap::CreatePixelMap(voidPtr);

    ImageSourceInfo info;
    info.pixmap_ = pixelMap;
    imageLayoutProperty->UpdateImageSourceInfo(info);
    richEditorPattern->DeleteValueSetImageSpan(imageSpanItem, spanResult);
    EXPECT_EQ(spanResult.GetValuePixelMap(), pixelMap);
}

/**
 * @tc.name: ProcessDeleteNodes002
 * @tc.desc: test ProcessDeleteNodes
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, ProcessDeleteNodes002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto newFrameNode = SpanNode::GetOrCreateSpanNode(V2::IMAGE_ETS_TAG, nodeId);
    auto newAddFrameNode = SpanNode::GetOrCreateSpanNode(V2::IMAGE_ETS_TAG, nodeId);
    newAddFrameNode->GetSpanItem()->content = INIT_VALUE_3;
    richEditorNode_->children_.push_back(newFrameNode);
    richEditorNode_->children_.push_back(newAddFrameNode);

    auto span = RichEditorAbstractSpanResult();
    span.spanType_ = SpanResultType::TEXT;
    span.offsetInSpan_ = 1;
    auto ui_node = richEditorNode_->GetChildAtIndex(span.GetSpanIndex());
    auto spanNode = AceType::DynamicCast<SpanNode>(ui_node);
    auto spanItem = spanNode->GetSpanItem();
    auto textTemp = spanItem->content;
    std::list<RichEditorAbstractSpanResult> list;
    list.emplace_back(span);
    richEditorPattern->ProcessDeleteNodes(list);
    EXPECT_EQ(spanItem->content, textTemp);
}

/**
 * @tc.name: MouseRightFocus002
 * @tc.desc: test MouseRightFocus
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, MouseRightFocus002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    spanItem->content = PREVIEW_TEXT_VALUE2;
    spanItem->spanItemType = SpanItemType::IMAGE;
    spanItem->position = 0;
    richEditorPattern->spans_.push_back(spanItem);
    richEditorPattern->spans_.push_back(spanItem);
    richEditorPattern->spans_.push_back(spanItem);
    MouseInfo info;
    info.SetGlobalLocation({ 0, 0 });
    richEditorPattern->MouseRightFocus(info);
    EXPECT_NE(richEditorPattern->selectedType_, TextSpanType::IMAGE);
}

/**
 * @tc.name: FromStyledString002
 * @tc.desc: test FromStyledString
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, FromStyledString002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    Selection selection;
    RefPtr<SpanString> spanString;
    spanString = AceType::MakeRefPtr<SpanString>(INIT_VALUE_1);
    ASSERT_NE(spanString, nullptr);
    spanString->spans_.front() = nullptr;
    spanString->spans_.emplace_back();

    selection = richEditorPattern->FromStyledString(spanString).GetSelection();
    EXPECT_EQ(selection.selection[0], 0);
    EXPECT_EQ(selection.selection[1], 0);
}

/**
 * @tc.name: InitSelection003
 * @tc.desc: test InitSelection
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, InitSelection003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    ASSERT_NE(paragraph, nullptr);
    richEditorPattern->previewLongPress_ = true;
    auto offset = Offset(0, 0);
    AddSpan("hello1");
    richEditorPattern->previewLongPress_ = false;
    richEditorPattern->editingLongPress_ = false;
    PositionWithAffinity positionWithAffinity(3, TextAffinity::DOWNSTREAM);
    EXPECT_CALL(*paragraph, GetGlyphPositionAtCoordinate(_)).WillRepeatedly(Return(positionWithAffinity));
    ParagraphManager::ParagraphInfo info;
    info.paragraph = paragraph;
    richEditorPattern->paragraphs_.AddParagraph(std::move(info));

    richEditorPattern->InitSelection(offset);
    EXPECT_NE(richEditorPattern->textSelector_.baseOffset, 0);
    EXPECT_NE(richEditorPattern->textSelector_.destinationOffset, 0);
}

/**
 * @tc.name: BeforeChangeText001
 * @tc.desc: test BeforeChangeText
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, BeforeChangeText001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RichEditorChangeValue changeValue;
    RichEditorPattern::OperationRecord operationRecord;
    OHOS::Ace::NG::RecordType recodrType = OHOS::Ace::NG::RecordType::DEL_BACKWARD;
    auto eventHub = richEditorNode_->GetEventHub<RichEditorEventHub>();
    eventHub->SetOnDidChange([](const RichEditorChangeValue& value) -> bool { return false; });
    auto ret = richEditorPattern->BeforeChangeText(changeValue, operationRecord, recodrType, 100);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: BeforeChangeText002
 * @tc.desc: test BeforeChangeText
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, BeforeChangeText002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RichEditorChangeValue changeValue;
    TextSpanOptions options;
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    richEditorPattern->spans_.emplace_back(spanItem);
    options.offset = -1;
    auto eventHub = richEditorNode_->GetEventHub<RichEditorEventHub>();
    eventHub->SetOnDidChange([](const RichEditorChangeValue& value) -> bool { return false; });
    auto ret = richEditorPattern->BeforeChangeText(changeValue, options);
    EXPECT_FALSE(richEditorPattern->spans_.empty());
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: BeforeDrag002
 * @tc.desc: test BeforeDrag
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, BeforeDrag002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    struct UpdateSpanStyle typingStyle;
    TextStyle textStyle;
    richEditorPattern->SetTypingStyle(typingStyle, textStyle);
    richEditorPattern->typingTextStyle_.reset();
    RichEditorChangeValue changeValue;
    int32_t innerPosition = 0;
    RichEditorPattern::OperationRecord record;
    record.addText = u"test123\n";
    record.beforeCaretPosition = 20;

    richEditorPattern->BeforeDrag(changeValue, innerPosition, record);
    EXPECT_FALSE(richEditorPattern->typingTextStyle_.has_value());
    EXPECT_NE(innerPosition, 0);
}

/**
 * @tc.name: BeforeAddImage001
 * @tc.desc: test BeforeAddImage
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, BeforeAddImage001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RichEditorChangeValue changeValue;
    ImageSpanOptions options;
    auto eventHub = richEditorNode_->GetEventHub<RichEditorEventHub>();
    eventHub->SetOnDidChange([](const RichEditorChangeValue& value) -> bool { return false; });
    auto ret = richEditorPattern->BeforeAddImage(changeValue, options, 100);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: UndoDrag003
 * @tc.desc: test UndoDrag
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, UndoDrag003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RichEditorPattern::OperationRecord record;
    record.addText = u"test123\n";
    record.deleteCaretPostion = 1;
    auto caretPosition = richEditorPattern->caretPosition_;
    richEditorPattern->UndoDrag(record);
    EXPECT_NE(richEditorPattern->caretPosition_, caretPosition);
}

/**
 * @tc.name: RedoDrag003
 * @tc.desc: test RedoDrag
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, RedoDrag003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RichEditorPattern::OperationRecord record;
    record.deleteCaretPostion = 1;
    auto caretPosition = richEditorPattern->caretPosition_;
    richEditorPattern->RedoDrag(record);
    EXPECT_EQ(richEditorPattern->caretPosition_, caretPosition);
}

/**
 * @tc.name: SetPreviewMenuParam001
 * @tc.desc: test SetPreviewMenuParam
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, SetPreviewMenuParam001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    TextSpanType spanType = TextSpanType::IMAGE;
    std::function<void()> builder;
    SelectMenuParam menuParam;
    richEditorPattern->SetPreviewMenuParam(spanType, builder, menuParam);

    richEditorPattern->SetPreviewMenuParam(spanType, builder, menuParam);
    EXPECT_TRUE(richEditorPattern->oneStepDragController_);
}

/**
 * @tc.name: OnPlaceholderHover001
 * @tc.desc: test OnPlaceholderHover
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, OnPlaceholderHover001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->currentMouseStyle_ = MouseFormat::DEFAULT;
    richEditorPattern->OnPlaceholderHover(true);
    EXPECT_NE(richEditorPattern->currentMouseStyle_, MouseFormat::TEXT_CURSOR);

    richEditorPattern->OnPlaceholderHover(false);
    EXPECT_EQ(richEditorPattern->currentMouseStyle_, MouseFormat::TEXT_CURSOR);
}

/**
 * @tc.name: DeleteRange001
 * @tc.desc: test DeleteRange
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteRange001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 8;
    int32_t end = 7;
    richEditorPattern->DeleteRange(start, end);
    EXPECT_NE(start, richEditorPattern->caretPosition_);
}

/**
 * @tc.name: DeleteRange002
 * @tc.desc: test DeleteRange
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteRange002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = -2;
    int32_t end = -1;
    richEditorPattern->DeleteRange(start, end);
    EXPECT_NE(start, richEditorPattern->caretPosition_);
}

/**
 * @tc.name: DeleteRange003
 * @tc.desc: test DeleteRange
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteRange003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 1;
    int32_t end = 1;
    richEditorPattern->DeleteRange(start, end);
    EXPECT_NE(start, richEditorPattern->caretPosition_);
}

/**
 * @tc.name: DeleteRange004
 * @tc.desc: test DeleteRange
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteRange004, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 0;
    int32_t end = 2;
    richEditorPattern->isSpanStringMode_ = true;
    richEditorPattern->styledString_->text_ = u"test";
    auto styleString = richEditorPattern->styledString_;
    richEditorPattern->previewLongPress_ = true;

    auto focusHub = richEditorPattern->GetFocusHub();
    focusHub->currentFocus_ = true;

    richEditorPattern->DeleteRange(start, end);
    EXPECT_FALSE(richEditorPattern->previewLongPress_);
}

/**
 * @tc.name: DeleteRange005
 * @tc.desc: test DeleteRange
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, DeleteRange005, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 0;
    int32_t end = 2;
    richEditorPattern->DeleteRange(start, end);
    EXPECT_FALSE(richEditorPattern->spans_.empty());
}

/**
 * @tc.name: SetSelectSpanStyle001
 * @tc.desc: test SetSelectSpanStyle
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, SetSelectSpanStyle001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 1;
    int32_t end = 4;
    KeyCode code = KeyCode::KEY_B;
    auto updateSpanStyle = richEditorPattern->GetUpdateSpanStyle();
    auto fontWeight = updateSpanStyle.updateFontWeight;
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    EXPECT_NE(fontWeight, richEditorPattern->GetUpdateSpanStyle().updateFontWeight);
}

/**
 * @tc.name: SetSelectSpanStyle002
 * @tc.desc: test SetSelectSpanStyle
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, SetSelectSpanStyle002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 1;
    int32_t end = 4;
    KeyCode code = KeyCode::KEY_I;
    auto updateSpanStyle = richEditorPattern->GetUpdateSpanStyle();
    auto italicFontStyle = updateSpanStyle.updateItalicFontStyle;
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    EXPECT_NE(italicFontStyle, richEditorPattern->GetUpdateSpanStyle().updateItalicFontStyle);
}

/**
 * @tc.name: SetSelectSpanStyle003
 * @tc.desc: test SetSelectSpanStyle
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, SetSelectSpanStyle003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 1;
    int32_t end = 4;
    KeyCode code = KeyCode::KEY_U;
    auto updateSpanStyle = richEditorPattern->GetUpdateSpanStyle();
    auto textDecoration = updateSpanStyle.updateTextDecoration;
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    EXPECT_NE(textDecoration, richEditorPattern->GetUpdateSpanStyle().updateTextDecoration);
}

/**
 * @tc.name: SetSelectSpanStyle004
 * @tc.desc: test SetSelectSpanStyle
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, SetSelectSpanStyle004, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    int32_t start = 1;
    int32_t end = 4;
    KeyCode code = KeyCode::KEY_A;
    auto updateSpanStyle = richEditorPattern->GetUpdateSpanStyle();
    updateSpanStyle.updateTextDecoration = TextDecoration::NONE;
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    richEditorPattern->UpdateSelectSpanStyle(start, end, code);
    EXPECT_EQ(TextDecoration::NONE, richEditorPattern->GetUpdateSpanStyle().updateTextDecoration);
}

/**
 * @tc.name: SetSelectSpanStyle005
 * @tc.desc: test SetSelectSpanStyle
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, SetSelectSpanStyle005, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    int32_t start = 1;
    int32_t end = 4;
    KeyCode code = KeyCode::KEY_B;
    bool isStart = true;
    richEditorPattern->SetSelectSpanStyle(start, end, code, isStart);
    EXPECT_TRUE(richEditorPattern->spans_.empty());

    AddSpan(INIT_VALUE_1);
    richEditorPattern->spans_.front()->textStyle_ = TextStyle(1);
    auto size = richEditorPattern->spans_.front()->textStyle_->GetFontSize();
    richEditorPattern->SetSelectSpanStyle(start, end, code, isStart);
    EXPECT_EQ(size, richEditorPattern->GetUpdateSpanStyle().updateFontSize);
}

/**
 * @tc.name: OnScrollEndCallback002
 * @tc.desc: test OnScrollEndCallback
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, OnScrollEndCallback002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    TestParagraphRect paragraphRect = { .start = 0, .end = 6, .rects = { { -400.0, -400.0, 200.0, 200.0 } } };
    TestParagraphItem paragraphItem = { .start = 0, .end = 6, .testParagraphRects = { paragraphRect } };
    AddParagraph(paragraphItem);
    richEditorPattern->textSelector_.baseOffset = 0;
    richEditorPattern->textSelector_.destinationOffset = 6;
    richEditorPattern->contentRect_ = { -500.0, -500.0, 500.0, 500.0 };

    auto pipeline = richEditorNode_->GetContext();
    pipeline->safeAreaManager_ = nullptr;
    richEditorPattern->OnScrollEndCallback();
    auto res = richEditorPattern->IsSelectAreaVisible();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: OnScrollEndCallback003
 * @tc.desc: test OnScrollEndCallback
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, OnScrollEndCallback003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    TestParagraphRect paragraphRect = { .start = 0, .end = 6, .rects = { { -400.0, -400.0, 200.0, 200.0 } } };
    TestParagraphItem paragraphItem = { .start = 0, .end = 6, .testParagraphRects = { paragraphRect } };
    AddParagraph(paragraphItem);
    richEditorPattern->textSelector_.baseOffset = 0;
    richEditorPattern->textSelector_.destinationOffset = 6;
    richEditorPattern->contentRect_ = { -500.0, -500.0, 500.0, 500.0 };
    richEditorPattern->isAnimationStop_ = false;
    richEditorPattern->OnScrollEndCallback();
    auto res = richEditorPattern->IsSelectAreaVisible();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: UpdateChildrenOffset003
 * @tc.desc: test UpdateChildrenOffset
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, UpdateChildrenOffset003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->spans_.emplace_back();
    richEditorPattern->UpdateChildrenOffset();
    EXPECT_FALSE(richEditorPattern->spans_.empty());
}

/**
 * @tc.name: UpdateChildrenOffset004
 * @tc.desc: test UpdateChildrenOffset
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, UpdateChildrenOffset004, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    auto host = richEditorPattern->GetHost();
    ASSERT_NE(host, nullptr);

    AddSpan(INIT_VALUE_1);
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem1 = AceType::MakeRefPtr<ImageSpanItem>();
    richEditorPattern->spans_.emplace_back(spanItem1);
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem2 = AceType::MakeRefPtr<PlaceholderSpanItem>();
    richEditorPattern->spans_.emplace_back(spanItem2);
    richEditorPattern->isSpanStringMode_ = true;

    std::list<RefPtr<UINode>>& childrens = host->ModifyChildren();
    auto childNode2 = AceType::MakeRefPtr<ImageSpanNode>(V2::IMAGE_ETS_TAG, 2);
    childrens.emplace_back(childNode2);

    richEditorPattern->UpdateChildrenOffset();
    EXPECT_TRUE(childNode2->GetSpanItem());
}

/**
 * @tc.name: UpdateChildrenOffset005
 * @tc.desc: test UpdateChildrenOffset
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, UpdateChildrenOffset005, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->CreateNodePaintMethod();
    EXPECT_NE(richEditorPattern->contentMod_, nullptr);
    EXPECT_NE(richEditorPattern->overlayMod_, nullptr);
    auto host = richEditorPattern->GetHost();
    ASSERT_NE(host, nullptr);

    AddSpan(INIT_VALUE_1);
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem1 = AceType::MakeRefPtr<ImageSpanItem>();
    richEditorPattern->spans_.emplace_back(spanItem1);
    OHOS::Ace::RefPtr<OHOS::Ace::NG::SpanItem> spanItem2 = AceType::MakeRefPtr<PlaceholderSpanItem>();
    richEditorPattern->spans_.emplace_back(spanItem2);
    TestParagraphRect paragraphRect = { .start = 0, .end = 6, .rects = { { -400.0, -400.0, 200.0, 200.0 } } };
    TestParagraphItem paragraphItem = { .start = 0, .end = 6, .testParagraphRects = { paragraphRect } };
    AddParagraph(paragraphItem);
    richEditorPattern->isSpanStringMode_ = true;

    std::list<RefPtr<UINode>>& childrens = host->ModifyChildren();
    auto childNode2 = AceType::MakeRefPtr<ImageSpanNode>(V2::IMAGE_ETS_TAG, 2);
    childNode2->imageSpanItem_ = nullptr;
    childrens.emplace_back(childNode2);

    richEditorPattern->UpdateChildrenOffset();
    EXPECT_FALSE(childNode2->GetSpanItem());
}

/**
 * @tc.name: HandleSelectFontStyleWrapper001
 * @tc.desc: test HandleSelectFontStyleWrapper
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleSelectFontStyleWrapper001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    KeyCode code = KeyCode::KEY_B;
    TextStyle spanStyle;
    spanStyle.fontWeight_ = Ace::FontWeight::BOLD;
    richEditorPattern->HandleSelectFontStyleWrapper(code, spanStyle);
    EXPECT_EQ(spanStyle.GetFontWeight(), Ace::FontWeight::NORMAL);
}

/**
 * @tc.name: HandleSelectFontStyleWrapper002
 * @tc.desc: test HandleSelectFontStyleWrapper
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleSelectFontStyleWrapper002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    KeyCode code = KeyCode::KEY_I;
    TextStyle spanStyle;
    spanStyle.fontStyle_ = OHOS::Ace::FontStyle::ITALIC;
    richEditorPattern->HandleSelectFontStyleWrapper(code, spanStyle);
    EXPECT_EQ(spanStyle.GetFontStyle(), OHOS::Ace::FontStyle::NORMAL);
}

/**
 * @tc.name: HandleSelectFontStyleWrapper003
 * @tc.desc: test HandleSelectFontStyleWrapper
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleSelectFontStyleWrapper003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    AddSpan(INIT_VALUE_1);
    KeyCode code = KeyCode::KEY_U;
    TextStyle spanStyle;
    spanStyle.textDecoration_ = TextDecoration::UNDERLINE;
    richEditorPattern->HandleSelectFontStyleWrapper(code, spanStyle);
    EXPECT_EQ(spanStyle.GetTextDecoration(), TextDecoration::NONE);
}

/**
 * @tc.name: CursorMoveLineEnd001
 * @tc.desc: test CursorMoveLineEnd
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, CursorMoveLineEnd001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->textSelector_.baseOffset = 1;
    richEditorPattern->textSelector_.destinationOffset = 2;
    richEditorPattern->CursorMoveLineEnd();
    EXPECT_EQ(richEditorPattern->caretPosition_, 0);
}

/**
 * @tc.name: CursorMoveLineEnd002
 * @tc.desc: test CursorMoveLineEnd
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, CursorMoveLineEnd002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    richEditorPattern->textSelector_.baseOffset = 0;
    richEditorPattern->textSelector_.destinationOffset = 1;
    richEditorPattern->CursorMoveLineEnd();
    EXPECT_EQ(richEditorPattern->caretPosition_, 0);
}

/**
 * @tc.name: HandleKbVerticalSelection003
 * @tc.desc: test HandleKbVerticalSelection
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleKbVerticalSelection003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->textSelector_.baseOffset = 1;
    richEditorPattern->textSelector_.destinationOffset = 0;
    richEditorPattern->caretPosition_ = 0;

    EXPECT_EQ(richEditorPattern->HandleKbVerticalSelection(false), 1);
}

/**
 * @tc.name: HandleKbVerticalSelection004
 * @tc.desc: test HandleKbVerticalSelection
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleKbVerticalSelection004, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->textSelector_.baseOffset = 1;
    richEditorPattern->textSelector_.destinationOffset = 1;
    richEditorPattern->caretPosition_ = 0;

    EXPECT_EQ(richEditorPattern->HandleKbVerticalSelection(false), 0);
}

/**
 * @tc.name: HandleKbVerticalSelection005
 * @tc.desc: test HandleKbVerticalSelection
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleKbVerticalSelection005, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    richEditorPattern->textSelector_.baseOffset = 1;
    richEditorPattern->textSelector_.destinationOffset = 0;
    richEditorPattern->caretPosition_ = 1;

    EXPECT_EQ(richEditorPattern->HandleKbVerticalSelection(false), 0);
}

/**
 * @tc.name: CopyGestureOption002
 * @tc.desc: test CopyGestureOption
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, CopyGestureOption002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    RefPtr<SpanNode> source = OHOS::Ace::NG::SpanNode::CreateSpanNode(1);
    RefPtr<SpanNode> target = OHOS::Ace::NG::SpanNode::CreateSpanNode(2);
    GestureEventFunc func1 = [](GestureEvent& info) {};
    OnHoverFunc func2 = [](bool, HoverInfo& info) {};
    source->GetSpanItem()->SetOnClickEvent(std::move(func1));
    source->GetSpanItem()->SetLongPressEvent(std::move(func1));
    source->GetSpanItem()->SetDoubleClickEvent(std::move(func1));
    source->GetSpanItem()->SetHoverEvent(std::move(func2));
    richEditorPattern->CopyGestureOption(source, target);
    EXPECT_TRUE(target->GetSpanItem()->onDoubleClick);
    EXPECT_TRUE(target->GetSpanItem()->onHover);
}

/**
 * @tc.name: GetSameSpanItem001
 * @tc.desc: test GetSameSpanItem
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, GetSameSpanItem001, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto spanItem = AceType::MakeRefPtr<ImageSpanItem>();
    spanItem->spanItemType = SpanItemType::IMAGE;
    auto childNode = AceType::MakeRefPtr<ImageSpanNode>(V2::IMAGE_ETS_TAG, 2);
    childNode->imageSpanItem_ = spanItem;
    richEditorNode_->children_.emplace_back(childNode);
    auto image = AceType::MakeRefPtr<MockCanvasImage>();
    auto imageNode = richEditorPattern->GetImageSpanNodeBySpanItem(spanItem);
    auto pattern = imageNode->GetPattern<ImagePattern>();
    pattern->image_ = image;
    auto pixelMap = image->GetPixelMap();
    richEditorPattern->GetSameSpanItem(spanItem);
    EXPECT_FALSE(pixelMap);
}

/**
 * @tc.name: GetSameSpanItem002
 * @tc.desc: test GetSameSpanItem
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, GetSameSpanItem002, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    auto spanItem = AceType::MakeRefPtr<ImageSpanItem>();
    spanItem->spanItemType = SpanItemType::IMAGE;
    auto childNode = AceType::MakeRefPtr<ImageSpanNode>(V2::IMAGE_ETS_TAG, 2);
    childNode->imageSpanItem_ = spanItem;
    richEditorNode_->children_.emplace_back(childNode);
    auto image = AceType::MakeRefPtr<MockCanvasImage>();
    image->needPixelMap = true;
    auto imageNode = richEditorPattern->GetImageSpanNodeBySpanItem(spanItem);
    auto pattern = imageNode->GetPattern<ImagePattern>();
    pattern->image_ = image;
    auto pixelMap = image->GetPixelMap();
    richEditorPattern->GetSameSpanItem(spanItem);
    EXPECT_TRUE(pixelMap);
}

/**
 * @tc.name: HandleClickEvent002
 * @tc.desc: test RichEditorPattern HandleClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorPatternTestSixNg, HandleClickEvent003, TestSize.Level1)
{
    ASSERT_NE(richEditorNode_, nullptr);
    auto richEditorPattern = richEditorNode_->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);
    GestureEvent info;
    info.localLocation_ = Offset(0, 0);
    ParagraphStyle paragraphStyle;
    auto paragraph = Paragraph::Create(paragraphStyle, FontCollection::Current());
    richEditorPattern->pManager_->AddParagraph({ .paragraph = paragraph, .paragraphStyle = paragraphStyle });
    auto focusHub = richEditorNode_->GetOrCreateFocusHub();
    ASSERT_NE(focusHub, nullptr);
    richEditorPattern->GetFocusHub()->focusType_ = FocusType::NODE;
    richEditorPattern->dataDetectorAdapter_->hasClickedAISpan_ = true;
    richEditorPattern->HandleClickEvent(info);
    EXPECT_FALSE(richEditorPattern->dataDetectorAdapter_->hasClickedAISpan_);
}
} // namespace OHOS::Ace::NG
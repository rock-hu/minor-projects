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

#include "rich_editor_drag_test_ng.h"

#include "gtest/gtest.h"
#include "test/mock/base/mock_pixel_map.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/render/mock_canvas_image.h"
#include "test/mock/core/render/mock_paragraph.h"
#include "test/mock/core/rosen/mock_canvas.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/render/adapter/pixelmap_image.h"
#include "frameworks/core/components_ng/pattern/rich_editor_drag/rich_editor_drag_overlay_modifier.h"

#define private public
#define protected public

namespace OHOS::Ace::NG {
namespace {
using namespace testing;
using namespace testing::ext;
constexpr float ROOT_WIDTH = 400.f;
constexpr float ROOT_HEIGHT = 800.f;
const std::string IMAGE_VALUE = "image1";
const std::string BUNDLE_NAME = "bundleName";
const std::string MODULE_NAME = "moduleName";
} // namespace

void RichEditorDragTestNG::SetUpTestSuite()
{
    TestNG::SetUpTestSuite();
    MockPipelineContext::GetCurrent()->SetUseFlushUITasks(true);
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
}

void RichEditorDragTestNG::TearDownTestSuite()
{
    TestNG::TearDownTestSuite();
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
}

void RichEditorDragTestNG::SetUp() {}

void RichEditorDragTestNG::TearDown()
{
    frameNode_ = nullptr;
    pattern_ = nullptr;
    dragNode_ = nullptr;
    dragPattern_ = nullptr;
}

void RichEditorDragTestNG::CreateRichEditor(Callback&& callback, uint32_t childSpanNum)
{
    ViewAbstract::SetWidth(CalcLength(ROOT_WIDTH));
    ViewAbstract::SetHeight(CalcLength(ROOT_HEIGHT));
    RichEditorModelNG model;
    model.Create();
    if (callback) {
        callback(model);
    }
    CreateDragNodeWithImageSpan(childSpanNum);
    FlushUITasks(frameNode_);
}

void RichEditorDragTestNG::CreateDragNodeWithImageSpan(uint32_t childSpanNum)
{
    frameNode_ = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    pattern_ = frameNode_->GetPattern<RichEditorPattern>();
    for (uint32_t index = 0; index < childSpanNum; ++index) {
        AddImageSpan();
    }

    auto children = frameNode_->GetChildren();
    std::list<RefPtr<FrameNode>> imageChildren;
    for (const auto& child : children) {
        auto node = AceType::DynamicCast<FrameNode>(child);
        if (!node) {
            continue;
        }
        auto image = node->GetPattern<ImagePattern>();
        if (image) {
            imageChildren.emplace_back(node);
        }
    }
    pattern_->dragNode_ = RichEditorDragPattern::CreateDragNode(frameNode_, imageChildren);
    dragNode_ = pattern_->dragNode_;
}

void RichEditorDragTestNG::AddImageSpan()
{
    ASSERT_NE(pattern_, nullptr);
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto imageNode = FrameNode::GetOrCreateFrameNode(
        V2::IMAGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    ImageSourceInfo imageInfo(IMAGE_VALUE, BUNDLE_NAME, MODULE_NAME);
    imageLayoutProperty->UpdateImageSourceInfo(imageInfo);
    imageNode->MountToParent(frameNode_, frameNode_->children_.size());
    auto spanItem = AceType::MakeRefPtr<ImageSpanItem>();
    spanItem->content = u" ";
    spanItem->placeholderIndex = 0;
    pattern_->spans_.emplace_back(spanItem);
    int32_t spanTextLength = 0;
    for (auto& span : pattern_->spans_) {
        spanTextLength += span->content.length();
        span->position = spanTextLength;
    }
}

void RichEditorDragTestNG::ClearSpan()
{
    ASSERT_NE(frameNode_, nullptr);
    ASSERT_NE(pattern_, nullptr);
    frameNode_->children_.clear();
    pattern_->spans_.clear();
    pattern_->caretPosition_ = 0;
}

/**
 * @tc.name: RichEditorDragTestNG001
 * @tc.desc: Test FolderStackTestNG properties & ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, RichEditorDragTestNG001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init FolderStack node with Aniamtion/AutoHalfFold props false, itemId not null.
     */
    CreateRichEditor([](RichEditorModelNG model) { model.SetTextDetectEnable(true); });
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RichEditorDragOverlayModifierTestNG001
 * @tc.desc: Test onDraw.
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, RichEditorDragOverlayModifierTestNG001, TestSize.Level1)
{
    auto richEditorNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, 1, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(richEditorNode, nullptr);
    TextDragInfo info;
    auto dragNode = RichEditorDragPattern::CreateDragNode(richEditorNode, info);
    ASSERT_NE(dragNode, nullptr);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    ASSERT_NE(richEditorDragPattern, nullptr);
    auto richEditorPattern = richEditorNode->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    WeakPtr<RichEditorDragPattern> pattern = std::move(richEditorDragPattern);
    WeakPtr<TextPattern> hostPattern = std::move(richEditorPattern);
    auto dragOverlayModifier = AceType::MakeRefPtr<RichEditorDragOverlayModifier>(pattern, hostPattern);
    ASSERT_NE(dragOverlayModifier, nullptr);

    Testing::MockCanvas rsCanvas;
    EXPECT_CALL(rsCanvas, AttachBrush(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DetachBrush()).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, AttachPen(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DetachPen()).WillRepeatedly(ReturnRef(rsCanvas));

    DrawingContext context { rsCanvas, 50.0f, 50.0f };

    dragOverlayModifier->onDraw(context);
    dragOverlayModifier->isAnimating_ = true;
    dragOverlayModifier->onDraw(context);
    dragOverlayModifier->type_ = DragAnimType::FLOATING;
    dragOverlayModifier->onDraw(context);
    EXPECT_EQ(dragOverlayModifier->type_, DragAnimType::FLOATING);

    auto firstHandle = AceType::MakeRefPtr<PropertyRectF>(RectF(10.0f, 10.0f, 10.0f, 10.0f));
    auto secondHandle = AceType::MakeRefPtr<PropertyRectF>(RectF(10.0f, 10.0f, 10.0f, 10.0f));
    auto handleOpacity = AceType::MakeRefPtr<AnimatablePropertyFloat>(5.0f);

    dragOverlayModifier->handleOpacity_ = handleOpacity;
    dragOverlayModifier->firstHandle_ = firstHandle;
    dragOverlayModifier->secondHandle_ = secondHandle;
    dragOverlayModifier->onDraw(context);
    EXPECT_EQ(dragOverlayModifier->handleRadius_->Get(), 0.0f);

    auto selectedBackgroundOpacity = AceType::MakeRefPtr<AnimatablePropertyFloat>(7.0f);
    dragOverlayModifier->selectedBackgroundOpacity_ = selectedBackgroundOpacity;

    dragOverlayModifier->onDraw(context);
    dragOverlayModifier->isAnimating_ = false;
    dragOverlayModifier->onDraw(context);
    dragOverlayModifier->firstHandle_ = nullptr;
    dragOverlayModifier->secondHandle_ = nullptr;
    dragOverlayModifier->onDraw(context);
    EXPECT_EQ(dragOverlayModifier->handleColor_->Get(), Color::BLACK);
}

/**
 * @tc.name: RichEditorDragOverlayModifierTestNG002
 * @tc.desc: Test onDraw.
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, RichEditorDragOverlayModifierTestNG002, TestSize.Level1)
{
    auto richEditorNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, 1, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(richEditorNode, nullptr);
    TextDragInfo info;
    auto dragNode = RichEditorDragPattern::CreateDragNode(richEditorNode, info);
    ASSERT_NE(dragNode, nullptr);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    ASSERT_NE(richEditorDragPattern, nullptr);

    WeakPtr<RichEditorDragPattern> pattern = std::move(richEditorDragPattern);
    WeakPtr<TextPattern> hostPattern;
    auto dragOverlayModifier = AceType::MakeRefPtr<RichEditorDragOverlayModifier>(pattern, hostPattern);
    ASSERT_NE(dragOverlayModifier, nullptr);

    Testing::MockCanvas rsCanvas;
    EXPECT_CALL(rsCanvas, AttachBrush(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DetachBrush()).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, AttachPen(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DetachPen()).WillRepeatedly(ReturnRef(rsCanvas));

    DrawingContext context { rsCanvas, 50.0f, 50.0f };
    dragOverlayModifier->isAnimating_ = true;

    dragOverlayModifier->type_ = DragAnimType::FLOATING_CANCEL;
    dragOverlayModifier->onDraw(context);
    EXPECT_EQ(dragOverlayModifier->handleOpacity_->Get(), 0.0f);
}

/**
 * @tc.name: RichEditorDragOverlayModifierTestNG003
 * @tc.desc: Test PaintFrameNode.
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, RichEditorDragOverlayModifierTestNG003, TestSize.Level1)
{
    auto richEditorNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, 1, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(richEditorNode, nullptr);
    TextDragInfo info;
    auto dragNode = RichEditorDragPattern::CreateDragNode(richEditorNode, info);
    ASSERT_NE(dragNode, nullptr);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    ASSERT_NE(richEditorDragPattern, nullptr);
    richEditorDragPattern->CreateNodePaintMethod();
    ASSERT_NE(richEditorDragPattern->overlayModifier_, nullptr);
    auto richEditorDragOverlayModifier =
        AceType::DynamicCast<RichEditorDragOverlayModifier>(richEditorDragPattern->overlayModifier_);
    ASSERT_NE(richEditorDragOverlayModifier, nullptr);

    Testing::MockCanvas rsCanvas;
    EXPECT_CALL(rsCanvas, AttachBrush(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DetachBrush()).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, AttachPen(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DetachPen()).WillRepeatedly(ReturnRef(rsCanvas));

    DrawingContext context { rsCanvas, 50.0f, 50.0f };
    OffsetF Offset = { 1.0f, 1.0f };

    auto imageNode =
        FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, 2, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);

    void* voidPtr = static_cast<void*>(new char[0]);
    auto pixelMap = MockPixelMap::CreatePixelMap(voidPtr);
    ASSERT_NE(pixelMap, nullptr);

    auto gestureHub = imageNode->GetOrCreateGestureEventHub();
    ASSERT_NE(gestureHub, nullptr);
    gestureHub->SetPixelMap(pixelMap);

    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);

    imagePattern->image_ = AceType::MakeRefPtr<PixelMapImage>(pixelMap);
    richEditorDragOverlayModifier->PaintImageNode(context, imageNode, imagePattern, Offset);
    richEditorDragOverlayModifier->PaintFrameNode(context, imageNode, imagePattern, Offset);
    EXPECT_FALSE(imageLayoutProperty->GetMarginProperty());

    PaddingProperty testPadding;
    auto marginProperty = std::make_unique<MarginProperty>(std::move(testPadding));
    ASSERT_NE(marginProperty, nullptr);
    imageLayoutProperty->margin_ = std::move(marginProperty);
    richEditorDragOverlayModifier->PaintImageNode(context, imageNode, imagePattern, Offset);
    richEditorDragOverlayModifier->PaintFrameNode(context, imageNode, imagePattern, Offset);
    EXPECT_TRUE(imageLayoutProperty->GetMarginProperty());
}

/**
 * @tc.name: RichEditorDragOverlayModifierTestNG004
 * @tc.desc: Test StartFloatingAnimate.
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, RichEditorDragOverlayModifierTestNG004, TestSize.Level1)
{
    auto richEditorNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, 1, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(richEditorNode, nullptr);
    TextDragInfo info;
    auto dragNode = RichEditorDragPattern::CreateDragNode(richEditorNode, info);
    ASSERT_NE(dragNode, nullptr);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    ASSERT_NE(richEditorDragPattern, nullptr);
    auto richEditorPattern = richEditorNode->GetPattern<RichEditorPattern>();
    ASSERT_NE(richEditorPattern, nullptr);

    WeakPtr<RichEditorDragPattern> pattern1 = richEditorDragPattern;
    WeakPtr<TextPattern> hostPattern1;
    auto dragOverlayModifier1 = AceType::MakeRefPtr<RichEditorDragOverlayModifier>(pattern1, hostPattern1);
    ASSERT_NE(dragOverlayModifier1, nullptr);
    dragOverlayModifier1->StartFloatingAnimate();
    EXPECT_EQ(dragOverlayModifier1->type_, DragAnimType::FLOATING);

    WeakPtr<RichEditorDragPattern> pattern2 = richEditorDragPattern;
    WeakPtr<TextPattern> hostPattern2 = std::move(richEditorPattern);
    auto dragOverlayModifier2 = AceType::MakeRefPtr<RichEditorDragOverlayModifier>(pattern2, hostPattern2);
    ASSERT_NE(dragOverlayModifier2, nullptr);
    dragOverlayModifier2->StartFloatingAnimate();
    EXPECT_EQ(dragOverlayModifier2->type_, DragAnimType::FLOATING);
}

/**
 * @tc.name: CreateDragNode001
 * @tc.desc: test CreateDragNode
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, CreateDragNode001, TestSize.Level1)
{
    auto richEditorNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, 1, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(richEditorNode, nullptr);
    TextDragInfo info;
    auto textPattern = richEditorNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    std::list<RefPtr<FrameNode>> imageChildren;
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto newFrameNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    auto newAddFrameNodeOne = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    auto newAddFrameNodeTwo = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    imageChildren.push_back(newFrameNode);
    imageChildren.push_back(newAddFrameNodeOne);
    imageChildren.push_back(newAddFrameNodeTwo);
    textPattern->placeholderIndex_ = { 1, 2, 3, 4 };
    textPattern->rectsForPlaceholders_ = { RectF(0.0f, 0.0f, 100.0f, 100.0f) };
    RichEditorDragPattern::CreateDragNode(richEditorNode, imageChildren, info);
    auto dragNode = RichEditorDragPattern::CreateDragNode(richEditorNode, imageChildren, info);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    EXPECT_EQ(richEditorDragPattern->rectsForPlaceholders_.size(), 0);
}

/**
 * @tc.name: CreateDragNode002
 * @tc.desc: test CreateDragNode
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, CreateDragNode002, TestSize.Level1)
{
    auto hostNode =
        FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, 1, []() { return AceType::MakeRefPtr<TextPattern>(); });
    ASSERT_NE(hostNode, nullptr);
    TextDragInfo info;
    auto textPattern = hostNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    std::list<RefPtr<FrameNode>> imageChildren;
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto newFrameNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    auto newAddFrameNodeOne = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    imageChildren.push_back(newFrameNode);
    imageChildren.push_back(newAddFrameNodeOne);
    textPattern->placeholderIndex_ = { 1, 2, 3 };
    textPattern->rectsForPlaceholders_ = { RectF(0.0f, 0.0f, 100.0f, 100.0f), RectF(20.0f, 20.0f, 200.0f, 200.0f),
        RectF(0.0f, 0.0f, 50.0f, 50.0f) };
    textPattern->textSelector_.baseOffset = 0;
    textPattern->textSelector_.destinationOffset = 10;
    ParagraphManager::ParagraphInfo paragraphInfo;
    ParagraphManager::ParagraphInfo paragraphInfo1;
    RefPtr<MockParagraph> mockParagraph = AceType::MakeRefPtr<MockParagraph>();
    EXPECT_CALL(*mockParagraph, GetRectsForRange(_, _, _))
        .WillRepeatedly(Invoke([](int32_t start, int32_t end, std::vector<RectF>& selectedRects) {
            selectedRects.emplace_back(RectF(0, 0, 100, 20));
        }));
    paragraphInfo.paragraph = mockParagraph;
    paragraphInfo1.paragraph = mockParagraph;
    paragraphInfo.start = 0;
    paragraphInfo.end = 10;
    paragraphInfo1.end = 10;
    textPattern->pManager_->paragraphs_.emplace_back(paragraphInfo);
    textPattern->pManager_->paragraphs_.emplace_back(paragraphInfo1);
    auto dragNode = RichEditorDragPattern::CreateDragNode(hostNode, imageChildren, info);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    EXPECT_EQ(richEditorDragPattern->rectsForPlaceholders_.size(), 0);
}

/**
 * @tc.name: CreateDragNode003
 * @tc.desc: test CreateDragNode
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, CreateDragNode003, TestSize.Level1)
{
    auto hostNode =
        FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, 1, []() { return AceType::MakeRefPtr<TextPattern>(); });
    ASSERT_NE(hostNode, nullptr);
    TextDragInfo info;
    auto textPattern = hostNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    std::list<RefPtr<FrameNode>> imageChildren;
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto newFrameNode = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    auto newAddFrameNodeOne = FrameNode::GetOrCreateFrameNode(
        V2::RICH_EDITOR_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    imageChildren.push_back(newFrameNode);
    imageChildren.push_back(newAddFrameNodeOne);
    textPattern->placeholderIndex_ = { 1, 2, 3 };
    textPattern->rectsForPlaceholders_ = { RectF(0.0f, 0.0f, 100.0f, 100.0f), RectF(20.0f, 20.0f, 200.0f, 200.0f),
        RectF(0.0f, 0.0f, 50.0f, 50.0f) };
    textPattern->textSelector_.baseOffset = 0;
    textPattern->textSelector_.destinationOffset = 10;
    ParagraphManager::ParagraphInfo paragraphInfo;
    ParagraphManager::ParagraphInfo paragraphInfo1;
    RefPtr<MockParagraph> mockParagraph = AceType::MakeRefPtr<MockParagraph>();
    EXPECT_CALL(*mockParagraph, GetRectsForRange(_, _, _))
        .WillRepeatedly(Invoke([](int32_t start, int32_t end, std::vector<RectF>& selectedRects) {
            selectedRects.emplace_back(RectF(10, 10, 250, 250));
        }));
    paragraphInfo.paragraph = mockParagraph;
    paragraphInfo1.paragraph = mockParagraph;
    paragraphInfo.start = 0;
    paragraphInfo.end = 10;
    paragraphInfo1.end = 10;
    textPattern->pManager_->paragraphs_.emplace_back(paragraphInfo);
    textPattern->pManager_->paragraphs_.emplace_back(paragraphInfo1);
    auto dragNode = RichEditorDragPattern::CreateDragNode(hostNode, imageChildren, info);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    EXPECT_EQ(richEditorDragPattern->rectsForPlaceholders_.size(), 4);
}

/**
 * @tc.name: CreateDragNode004
 * @tc.desc: test CreateDragNode
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, CreateDragNode004, TestSize.Level1)
{
    auto hostNode = FrameNode::GetOrCreateFrameNode(V2::RICH_EDITOR_ETS_TAG, 1,
        []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(hostNode, nullptr);
    auto richPattern = hostNode->GetPattern<RichEditorPattern>();
    ASSERT_NE(richPattern, nullptr);
    std::list<RefPtr<FrameNode>> imageChildren;
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto imageNode = FrameNode::GetOrCreateFrameNode(
        V2::IMAGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    auto imageNode1 = FrameNode::GetOrCreateFrameNode(
        V2::IMAGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    imageChildren.push_back(imageNode);
    imageChildren.push_back(imageNode1);
    richPattern->placeholderIndex_ = { 1, 2, 3 };
    richPattern->rectsForPlaceholders_ = { RectF(0.0f, 0.0f, 100.0f, 100.0f), RectF(20.0f, 20.0f, 200.0f, 200.0f),
        RectF(0.0f, 0.0f, 50.0f, 50.0f) };
    richPattern->textSelector_.baseOffset = 0;
    richPattern->textSelector_.destinationOffset = 10;
    ParagraphManager::ParagraphInfo paragraphInfo;
    ParagraphManager::ParagraphInfo paragraphInfo1;
    RefPtr<MockParagraph> mockParagraph = AceType::MakeRefPtr<MockParagraph>();
    EXPECT_CALL(*mockParagraph, GetRectsForRange(_, _, _))
        .WillRepeatedly(Invoke([](int32_t start, int32_t end, std::vector<RectF>& selectedRects) {
            selectedRects.emplace_back(RectF(10, 10, 250, 250));
        }));
    const OHOS::Ace::NG::ParagraphStyle expectedStyle;
    EXPECT_CALL(*mockParagraph, GetParagraphStyle()).WillRepeatedly(ReturnRef(expectedStyle));
    paragraphInfo.paragraph = mockParagraph;
    paragraphInfo1.paragraph = mockParagraph;
    paragraphInfo.start = 0;
    paragraphInfo.end = 10;
    paragraphInfo1.end = 10;
    richPattern->paragraphs_.paragraphs_.emplace_back(paragraphInfo);
    richPattern->paragraphs_.paragraphs_.emplace_back(paragraphInfo1);
    TextDragInfo info;
    info.isDragShadowNeeded = false;
    auto dragNode = RichEditorDragPattern::CreateDragNode(hostNode, imageChildren, info);
    ASSERT_NE(dragNode, nullptr);
    auto richEditorDragPattern = dragNode->GetPattern<RichEditorDragPattern>();
    EXPECT_FALSE(richEditorDragPattern->info_->dragBackgroundColor.has_value());
    EXPECT_FALSE(richEditorDragPattern->info_->isDragShadowNeeded);
}

/**
 * @tc.name: InitDragShadow001
 * @tc.desc: test InitDragShadow
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, InitDragShadow001, TestSize.Level1)
{
    auto hostNode = FrameNode::GetOrCreateFrameNode(V2::RICH_EDITOR_ETS_TAG, 1,
        []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(hostNode, nullptr);
    auto richPattern = hostNode->GetPattern<RichEditorPattern>();
    ASSERT_NE(richPattern, nullptr);
    richPattern->InitDragShadow(hostNode, nullptr, false, false);
    auto option = hostNode->GetDragPreviewOption();
    EXPECT_TRUE(option.options.shadowPath.empty());
    EXPECT_FALSE(option.options.shadow.has_value());
}

/**
 * @tc.name: GetMaxSelectedWidth001
 * @tc.desc: test GetMaxSelectedWidth
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, GetMaxSelectedWidth001, TestSize.Level1)
{
    auto hostNode = FrameNode::GetOrCreateFrameNode(V2::RICH_EDITOR_ETS_TAG, 1,
        []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(hostNode, nullptr);
    auto richPattern = hostNode->GetPattern<RichEditorPattern>();
    ASSERT_NE(richPattern, nullptr);
    auto ret = richPattern->GetMaxSelectedWidth();
    EXPECT_EQ(ret, 0.0f);
}

/**
 * @tc.name: RichEditorDragStart001
 * @tc.desc: test dragstart
 * @tc.type: FUNC
 */
HWTEST_F(RichEditorDragTestNG, RichEditorDragStart001, TestSize.Level1)
{
    auto hostNode = FrameNode::GetOrCreateFrameNode(V2::RICH_EDITOR_ETS_TAG, 1,
        []() { return AceType::MakeRefPtr<RichEditorPattern>(); });
    ASSERT_NE(hostNode, nullptr);
    auto richPattern = hostNode->GetPattern<RichEditorPattern>();
    ASSERT_NE(richPattern, nullptr);
    richPattern->HandleDragStart(nullptr, "");
    EXPECT_TRUE(richPattern->isDragSponsor_);
}
} // namespace OHOS::Ace::NG
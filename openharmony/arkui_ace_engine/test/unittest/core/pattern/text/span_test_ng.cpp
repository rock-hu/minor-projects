/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "test/mock/core/render/mock_paragraph.h"

#include "core/components_ng/pattern/text/image_span_view.h"
#include "core/components_ng/pattern/text/span_model_ng.h"
#include "core/components_ng/pattern/text/symbol_span_model_ng.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/image/image_model_ng.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const InspectorFilter filter;
const std::string CREATE_VALUE = "Hello World";
const std::u16string CREATE_VALUE_W = u"Hello World";
const Dimension FONT_SIZE_VALUE = Dimension(20.1, DimensionUnit::PX);
const Color TEXT_COLOR_VALUE = Color::FromRGB(255, 100, 100);
const Ace::FontStyle ITALIC_FONT_STYLE_VALUE = Ace::FontStyle::ITALIC;
const std::vector<std::string> FONT_FAMILY_VALUE = { "cursive" };
const Ace::TextDecoration TEXT_DECORATION_VALUE = Ace::TextDecoration::INHERIT;
const Color TEXT_DECORATION_COLOR_VALUE = Color::FromRGB(255, 100, 100);
const Shadow TEXT_SHADOW1 = Shadow(0, 0, Offset(), Color::RED);
const Shadow TEXT_SHADOW2 = Shadow(0, 0, Offset(), Color::WHITE);
const std::vector<Shadow> TEXT_SHADOWS { TEXT_SHADOW1, TEXT_SHADOW2 };
const Ace::TextCase TEXT_CASE_VALUE = Ace::TextCase::LOWERCASE;
const Dimension LETTER_SPACING = Dimension(10, DimensionUnit::PX);
void onClickFunc(const BaseEventInfo* info) {};
const std::string FONT_SIZE = "fontSize";
const std::string FONT_DEFAULT_VALUE = "{\"style\":\"FontStyle.Normal\",\"size\":\"16.00fp\",\"weight\":"
                                       "\"FontWeight.Normal\",\"family\":\"HarmonyOS Sans\"}";
const std::string FONT_EQUALS_VALUE =
    R"({"style":"FontStyle.Italic","size":"20.10px","weight":"FontWeight.Bold","family":"cursive"})";
const std::string TEXT_FOR_AI = "phone: 12345678900,url: www.baidu.com";
const std::u16string U16TEXT_FOR_AI = u"phone: 12345678900,url: www.baidu.com";
const std::string SPAN_PHONE = "12345678900";
const std::string SPAN_URL = "www.baidu.com";
constexpr int32_t AI_SPAN_START = 7;
constexpr int32_t AI_SPAN_END = 18;
constexpr int32_t AI_SPAN_START_II = 24;
constexpr int32_t AI_SPAN_END_II = 37;
const uint32_t SYMBOL_ID = 1;
std::vector<Color> SYMBOL_COLOR = { Color::FromRGB(255, 100, 100) };
std::vector<Color> SYMBOL_COLOR_LIST = { Color::FromRGB(255, 100, 100), Color::FromRGB(255, 255, 100) };
const uint32_t RENDER_STRATEGY_SINGLE = 0;
const uint32_t RENDER_STRATEGY_MULTI_COLOR = 1;
const uint32_t RENDER_STRATEGY_MULTI_OPACITY = 2;
const uint32_t EFFECT_STRATEGY_NONE = 0;
const uint32_t EFFECT_STRATEGY_SCALE = 1;
const uint32_t EFFECT_STRATEGY_HIERARCHICAL = 2;
const std::string IMAGE_SRC_URL = "file://data/data/com.example.test/res/example.svg";
const std::string BUNDLE_NAME;
const std::string MODULE_NAME;
constexpr double IMAGE_SOURCESIZE_WIDTH = 300.0;
constexpr double IMAGE_SOURCESIZE_HEIGHT = 200.0;
constexpr double WIDTH = 400.0;
constexpr double HEIGHT = 500.0;
} // namespace

class SpanTestNg : public testing::Test {};

class TestNode : public UINode {
    DECLARE_ACE_TYPE(TestNode, UINode);

public:
    static RefPtr<TestNode> CreateTestNode(int32_t nodeId)
    {
        auto spanNode = MakeRefPtr<TestNode>(nodeId);
        return spanNode;
    };

    bool IsAtomicNode() const override
    {
        return true;
    }

    explicit TestNode(int32_t nodeId) : UINode("TestNode", nodeId) {}
    ~TestNode() override = default;
};

/**
 * @tc.name: SpanFrameNodeCreator001
 * @tc.desc: Test all the properties of text.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanFrameNodeCreator001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    spanModelNG.SetFontSize(FONT_SIZE_VALUE);
    spanModelNG.SetTextColor(TEXT_COLOR_VALUE);
    spanModelNG.SetItalicFontStyle(ITALIC_FONT_STYLE_VALUE);
    spanModelNG.SetFontFamily(FONT_FAMILY_VALUE);
    EXPECT_EQ(spanNode->GetFontSize(), FONT_SIZE_VALUE);
    EXPECT_EQ(spanNode->GetTextColor(), TEXT_COLOR_VALUE);
    EXPECT_EQ(spanNode->GetItalicFontStyle(), ITALIC_FONT_STYLE_VALUE);
    EXPECT_EQ(spanNode->GetFontFamily(), FONT_FAMILY_VALUE);

    spanModelNG.SetTextDecoration(TEXT_DECORATION_VALUE);
    spanModelNG.SetTextDecorationColor(TEXT_DECORATION_COLOR_VALUE);
    spanModelNG.SetTextCase(TEXT_CASE_VALUE);
    spanModelNG.SetLetterSpacing(LETTER_SPACING);
    EXPECT_EQ(spanNode->GetTextDecoration(), TEXT_DECORATION_VALUE);
    EXPECT_EQ(spanNode->GetTextDecorationColor(), TEXT_DECORATION_COLOR_VALUE);
    EXPECT_EQ(spanNode->GetTextCase(), TEXT_CASE_VALUE);
    EXPECT_EQ(spanNode->GetLetterSpacing(), LETTER_SPACING);

    spanModelNG.SetFontWeight(FontWeight::LIGHTER);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::LIGHTER);
    spanModelNG.SetFontWeight(FontWeight::REGULAR);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::REGULAR);
    spanModelNG.SetFontWeight(FontWeight::NORMAL);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::NORMAL);
    spanModelNG.SetFontWeight(FontWeight::MEDIUM);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::MEDIUM);
    spanModelNG.SetFontWeight(FontWeight::BOLD);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::BOLD);
    spanModelNG.SetFontWeight(FontWeight::BOLDER);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::BOLDER);
    spanModelNG.SetFontWeight(FontWeight::W100);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W100);
    spanModelNG.SetOnClick(onClickFunc);
}

/**
 * @tc.name: SpanItemToJsonValue001
 * @tc.desc: Test SpanItem ToJsonValue when fontStyle is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemToJsonValue001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto json = std::make_unique<JsonValue>();
    spanNode->spanItem_->content = u"";
    spanNode->spanItem_->fontStyle = nullptr;
    spanNode->spanItem_->ToJsonValue(json, filter);
    bool ret = json->Contains(FONT_SIZE);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: SpanItemToJsonValue002
 * @tc.desc: Test SpanItem ToJsonValue when fontStyle is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemToJsonValue002, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto json = std::make_unique<JsonValue>();
    spanNode->spanItem_->content = u"";
    spanNode->spanItem_->fontStyle = std::make_unique<FontStyle>();
    spanNode->spanItem_->ToJsonValue(json, filter);
    bool ret = json->Contains(FONT_SIZE);
    EXPECT_EQ(ret, false);
    ASSERT_NE(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: SpanNodeGetOrCreateSpanNode001
 * @tc.desc: Test SpanNode GetOrCreateSpanNode when span node is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeGetOrCreateSpanNode001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = spanNode->GetOrCreateSpanNode(-1);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SpanNodeGetOrCreateSpanNode002
 * @tc.desc: Test SpanNode GetOrCreateSpanNode when span node is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeGetOrCreateSpanNode002, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = spanNode->GetOrCreateSpanNode(1);
    ASSERT_NE(node, nullptr);
    node = spanNode->GetOrCreateSpanNode(1);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SpanNodeGetOrCreateSpanNode003
 * @tc.desc: Test SpanNode GetOrCreateSpanNode when spannode is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeGetOrCreateSpanNode003, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = AceType::MakeRefPtr<SpanNode>(1);
    ElementRegister::GetInstance()->AddUINode(node);
    node = spanNode->GetOrCreateSpanNode(1);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SpanNodeMountToParagraph001
 * @tc.desc: Test SpanNode MountToParagraph when parent is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeMountToParagraph001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    spanNode->SetParent(nullptr);
    spanNode->MountToParagraph();
    EXPECT_EQ(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeMountToParagraph002
 * @tc.desc: Test SpanNode MountToParagraph when parent is valid.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeMountToParagraph002, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = AceType::MakeRefPtr<SpanNode>(1);
    spanNode->SetParent(node);
    spanNode->MountToParagraph();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeMountToParagraph003
 * @tc.desc: Test SpanNode MountToParagraph when parent is FrameNode.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeMountToParagraph003, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto pattern = AceType::MakeRefPtr<Pattern>();
    auto node = FrameNode::CreateFrameNode("Test", 1, pattern);
    spanNode->SetParent(node);
    spanNode->MountToParagraph();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeMountToParagraph004
 * @tc.desc: Test SpanNode MountToParagraph when textPattern is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeMountToParagraph004, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    auto node = FrameNode::CreateFrameNode("Test", 1, pattern);
    spanNode->SetParent(node);
    spanNode->MountToParagraph();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeMountToParagraph005
 * @tc.desc: Test SpanNode MountToParagraph when parent is CustomNode.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeMountToParagraph005, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = TestNode::CreateTestNode(1);
    spanNode->SetParent(node);
    spanNode->MountToParagraph();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeRequestTextFlushDirty001
 * @tc.desc: Test SpanNode RequestTextFlushDirty when parent is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeRequestTextFlushDirty001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    spanNode->SetParent(nullptr);
    spanNode->RequestTextFlushDirty();
    EXPECT_EQ(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeRequestTextFlushDirty002
 * @tc.desc: Test SpanNode RequestTextFlushDirty when parent is invalid.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeRequestTextFlushDirty002, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = AceType::MakeRefPtr<SpanNode>(1);
    spanNode->SetParent(node);
    spanNode->RequestTextFlushDirty();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeRequestTextFlushDirty003
 * @tc.desc: Test SpanNode RequestTextFlushDirty when parent is FrameNode.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeRequestTextFlushDirty003, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto pattern = AceType::MakeRefPtr<Pattern>();
    auto node = FrameNode::CreateFrameNode("Test", 1, pattern);
    spanNode->SetParent(node);
    spanNode->RequestTextFlushDirty();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeRequestTextFlushDirty004
 * @tc.desc: Test SpanNode RequestTextFlushDirty when TextPattern is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeRequestTextFlushDirty004, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    auto node = FrameNode::CreateFrameNode("Test", 1, pattern);
    spanNode->SetParent(node);
    spanNode->RequestTextFlushDirty();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanNodeRequestTextFlushDirty005
 * @tc.desc: Test SpanNode RequestTextFlushDirty when parent is CustomNode.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeRequestTextFlushDirty005, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = TestNode::CreateTestNode(1);
    spanNode->SetParent(node);
    spanNode->RequestTextFlushDirty();
    ASSERT_NE(spanNode->GetParent(), nullptr);
}

/**
 * @tc.name: SpanItemUpdateParagraph001
 * @tc.desc: Test SpanItem UpdateParagraph when fontStyle is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemUpdateParagraph001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto json = std::make_unique<JsonValue>();
    spanNode->spanItem_->content = u"";
    spanNode->spanItem_->fontStyle = std::make_unique<FontStyle>();
    TextStyle textStyle;
    ParagraphStyle paraStyle = { .direction = TextDirection::LTR,
        .align = textStyle.GetTextAlign(),
        .maxLines = textStyle.GetMaxLines(),
        .fontLocale = "zh-CN",
        .wordBreak = textStyle.GetWordBreak(),
        .textOverflow = textStyle.GetTextOverflow() };
    auto paragraph = Paragraph::Create(paraStyle, FontCollection::Current());
    spanNode->spanItem_->UpdateParagraph(nullptr, paragraph, TextStyle());
    ASSERT_NE(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: SpanItemUpdateParagraph002
 * @tc.desc: Test SpanItem UpdateParagraph when fontStyle is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemUpdateParagraph002, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto json = std::make_unique<JsonValue>();
    spanNode->spanItem_->content = u"";
    spanNode->spanItem_->fontStyle = nullptr;
    TextStyle textStyle;
    ParagraphStyle paraStyle = { .direction = TextDirection::LTR,
        .align = textStyle.GetTextAlign(),
        .maxLines = textStyle.GetMaxLines(),
        .fontLocale = "zh-CN",
        .wordBreak = textStyle.GetWordBreak(),
        .textOverflow = textStyle.GetTextOverflow() };
    auto paragraph = Paragraph::Create(paraStyle, FontCollection::Current());
    spanNode->spanItem_->UpdateParagraph(nullptr, paragraph, TextStyle());
    EXPECT_EQ(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: SpanItemUpdateParagraph003
 * @tc.desc: Test SpanItem UpdateParagraph when Paragraph is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemUpdateParagraph003, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto json = std::make_unique<JsonValue>();
    spanNode->spanItem_->content = u"";
    spanNode->spanItem_->fontStyle = nullptr;
    spanNode->spanItem_->UpdateParagraph(nullptr, nullptr, TextStyle());
    EXPECT_EQ(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: SpanItemUpdateParagraph004
 * @tc.desc: Test SpanItem UpdateParagraph when children is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemUpdateParagraph004, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto json = std::make_unique<JsonValue>();
    spanNode->spanItem_->content = u"";
    spanNode->spanItem_->fontStyle = nullptr;
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    spanNode->spanItem_->children.push_back(spanItem);
    TextStyle textStyle;
    ParagraphStyle paraStyle = { .direction = TextDirection::LTR,
        .align = textStyle.GetTextAlign(),
        .maxLines = textStyle.GetMaxLines(),
        .fontLocale = "zh-CN",
        .wordBreak = textStyle.GetWordBreak(),
        .textOverflow = textStyle.GetTextOverflow() };
    auto paragraph = Paragraph::Create(paraStyle, FontCollection::Current());
    spanNode->spanItem_->UpdateParagraph(nullptr, paragraph, TextStyle());
    EXPECT_EQ(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: SpanItemUpdateParagraph005
 * @tc.desc: Test SpanItem UpdateParagraph when children is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemUpdateParagraph005, TestSize.Level1)
{
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<ImageSpanItem>();
    ASSERT_NE(spanItem, nullptr);
    TextStyle textStyle;
    ParagraphStyle paraStyle = { .direction = TextDirection::LTR,
        .align = textStyle.GetTextAlign(),
        .maxLines = textStyle.GetMaxLines(),
        .fontLocale = "zh-CN",
        .wordBreak = textStyle.GetWordBreak(),
        .textOverflow = textStyle.GetTextOverflow() };

    auto paragraph = MockParagraph::GetOrCreateMockParagraph();
    EXPECT_CALL(*paragraph, PushStyle).Times(5);
    EXPECT_CALL(*paragraph, AddPlaceholder).Times(5);
    EXPECT_CALL(*paragraph, PopStyle).Times(5);

    PlaceholderStyle placeholderStyle;
    placeholderStyle.width = 9.0;
    placeholderStyle.height = 10.0;
    placeholderStyle.verticalAlign = VerticalAlign::TOP;
    auto index = spanItem->UpdateParagraph(nullptr, paragraph, false, placeholderStyle);
    placeholderStyle.verticalAlign = VerticalAlign::CENTER;
    index = spanItem->UpdateParagraph(nullptr, paragraph, false, placeholderStyle);
    placeholderStyle.verticalAlign = VerticalAlign::BOTTOM;
    index = spanItem->UpdateParagraph(nullptr, paragraph, false, placeholderStyle);
    placeholderStyle.verticalAlign = VerticalAlign::BASELINE;
    index = spanItem->UpdateParagraph(nullptr, paragraph, false, placeholderStyle);
    placeholderStyle.verticalAlign = VerticalAlign::NONE;
    index = spanItem->UpdateParagraph(nullptr, paragraph, false, placeholderStyle);

    MockParagraph::TearDown();
}

/**
 * @tc.name: ImageSpanViewCreat001
 * @tc.desc: Test SpanItem UpdateParagraph when children is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, Create001, TestSize.Level1)
{
    ImageModelNG imageSpan;
    ImageInfoConfig imageInfoConfig;
    RefPtr<PixelMap> pixMap = nullptr;
    imageSpan.Create(imageInfoConfig, pixMap);
    ImageSpanView::SetObjectFit(ImageFit::FILL);
    ImageSpanView::SetVerticalAlign(VerticalAlign::TOP);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    EXPECT_EQ(layoutProperty->GetImageFitValue(ImageFit::COVER), ImageFit::FILL);
    EXPECT_EQ(layoutProperty->GetVerticalAlignValue(VerticalAlign::BOTTOM), VerticalAlign::TOP);
}

/**
 * @tc.name: SpanSetBaselineOffsetTest001
 * @tc.desc: Test baseline offset of span.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanSetBaselineOffsetTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create span node
     */
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);

    /**
     * @tc.steps: step2. get span node
     */
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);

    /**
     * @tc.steps: step3. test baseline offset
     */
    Dimension offset = Dimension(10.0, DimensionUnit::PX);
    spanModelNG.SetBaselineOffset(offset);
    EXPECT_EQ(spanNode->GetBaselineOffset(), Dimension(10.0, DimensionUnit::PX));

    offset = Dimension(-5.0, DimensionUnit::VP);
    spanModelNG.SetBaselineOffset(offset);
    EXPECT_EQ(spanNode->GetBaselineOffset(), Dimension(-5.0, DimensionUnit::VP));
}

/**
 * @tc.name: ImageSpanSetBaselineOffset001
 * @tc.desc: Test ImageSpanView set baseline offset
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, ImageSpanSetBaselineOffset001, TestSize.Level1)
{
    ImageModelNG imageSpan;
    ImageInfoConfig imageInfoConfig;
    RefPtr<PixelMap> pixMap = nullptr;
    imageSpan.Create(imageInfoConfig, pixMap);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);

    Dimension offset = Dimension(10.0, DimensionUnit::VP);
    layoutProperty->UpdateBaselineOffset(offset);
    EXPECT_TRUE(layoutProperty->HasBaselineOffset());
    EXPECT_EQ(layoutProperty->GetBaselineOffset(), Dimension(10.0, DimensionUnit::VP));

    offset = Dimension(-5.0, DimensionUnit::VP);
    layoutProperty->UpdateBaselineOffset(offset);
    EXPECT_EQ(layoutProperty->GetBaselineOffset(), Dimension(-5.0, DimensionUnit::VP));
}

/**
 * @tc.name: SpanModelSetFont001
 * @tc.desc: Test if SetFont is successful
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanModelSetFont001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize SpanModelNG
     */
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);

    /**
     * @tc.steps: step2. Set Font, call SetFont
     */
    Font font;
    font.fontSize = FONT_SIZE_VALUE;
    font.fontWeight = FontWeight::BOLD;
    font.fontFamilies = FONT_FAMILY_VALUE;
    font.fontStyle = ITALIC_FONT_STYLE_VALUE;
    spanModelNG.SetFont(font);
    /**
     * @tc.steps: step3. Gets the relevant properties of the Font
     * @tc.expected: step3. Check the font value
     */
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);
    EXPECT_EQ(spanNode->GetFontSize(), FONT_SIZE_VALUE);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::BOLD);
    EXPECT_EQ(spanNode->GetFontFamily(), FONT_FAMILY_VALUE);
    EXPECT_EQ(spanNode->GetItalicFontStyle(), ITALIC_FONT_STYLE_VALUE);
}

/**
 * @tc.name: SpanItemSetFont001GetFontSize
 * @tc.desc: Test if GetFont is successful
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemGetFont001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize SpanModelNG and SpanNode
     */
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);
    spanModelNG.SetFontWeight(FontWeight::NORMAL);

    /**
     * @tc.steps: step2. not set Gets the relevant properties of the Font
     * @tc.expected: step2. Check the font value
     */
    EXPECT_EQ(spanNode->spanItem_->GetFont(), FONT_DEFAULT_VALUE);

    /**
     * @tc.steps: step2. set and Gets the relevant properties of the Font
     * @tc.expected: step2. Check the font value
     */
    spanModelNG.SetFontSize(FONT_SIZE_VALUE);
    spanModelNG.SetFontWeight(FontWeight::BOLD);
    spanModelNG.SetFontFamily(FONT_FAMILY_VALUE);
    spanModelNG.SetItalicFontStyle(ITALIC_FONT_STYLE_VALUE);
    EXPECT_EQ(spanNode->spanItem_->GetFont(), FONT_EQUALS_VALUE);
}

/**
 * @tc.name: SpanTextShadowTest002
 * @tc.desc: test span_model_ng.cpp SetTextShadow
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanTextShadowTest002, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    spanModelNG.SetTextShadow(TEXT_SHADOWS);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_EQ(spanNode->GetTextShadow(), TEXT_SHADOWS);
}
/**
 * @tc.name: SpanDecorationStyleTest001
 * @tc.desc: test span_model_ng.cpp SetTextDecorationStyle
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanDecorationStyleTest001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    spanModelNG.SetTextDecorationStyle(Ace::TextDecorationStyle::WAVY);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_EQ(spanNode->GetTextDecorationStyle(), Ace::TextDecorationStyle::WAVY);
}

/**
 * @tc.name: SpanDecorationToJsonValue001
 * @tc.desc: Test Span Decoration ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanDecorationToJsonValue001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    spanModelNG.SetTextDecoration(TextDecoration::LINE_THROUGH);
    spanModelNG.SetTextDecorationStyle(Ace::TextDecorationStyle::DOUBLE);
    spanModelNG.SetTextDecorationColor(TEXT_DECORATION_COLOR_VALUE);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(spanNode, nullptr);
    auto json = JsonUtil::Create(true);
    spanNode->ToJsonValue(json, filter);
    EXPECT_TRUE(json->Contains("content"));
    EXPECT_TRUE(json->GetValue("content")->GetString() == CREATE_VALUE);
    EXPECT_TRUE(json->Contains("decoration"));
    std::string decorationStr = json->GetValue("decoration")->GetString();
    auto decorationJson = JsonUtil::ParseJsonString(decorationStr);
    ASSERT_NE(decorationJson, nullptr);
    EXPECT_TRUE(decorationJson->Contains("type"));
    EXPECT_TRUE(decorationJson->GetValue("type")->GetString() ==
                V2::ConvertWrapTextDecorationToStirng(Ace::TextDecoration::LINE_THROUGH));
    EXPECT_TRUE(decorationJson->Contains("color"));
    EXPECT_TRUE(decorationJson->GetValue("color")->GetString() == TEXT_DECORATION_COLOR_VALUE.ColorToString());
    EXPECT_TRUE(decorationJson->Contains("style"));
    EXPECT_TRUE(decorationJson->GetValue("style")->GetString() ==
                V2::ConvertWrapTextDecorationStyleToString(Ace::TextDecorationStyle::DOUBLE));
}

/**
 * @tc.name: SpanDecorationToJsonValue002
 * @tc.desc: Test Span Decoration ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanDecorationToJsonValue002, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    spanModelNG.SetFontSize(FONT_SIZE_VALUE);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(spanNode, nullptr);
    auto json = JsonUtil::Create(true);
    spanNode->ToJsonValue(json, filter);
    EXPECT_TRUE(json->Contains("content"));
    EXPECT_TRUE(json->GetValue("content")->GetString() == CREATE_VALUE);
    EXPECT_TRUE(json->Contains("fontSize"));
    EXPECT_TRUE(json->GetValue("fontSize")->GetString() == FONT_SIZE_VALUE.ToString());
    EXPECT_TRUE(json->Contains("decoration"));
    std::string decorationStr = json->GetValue("decoration")->GetString();
    auto decorationJson = JsonUtil::ParseJsonString(decorationStr);
    ASSERT_NE(decorationJson, nullptr);
    EXPECT_TRUE(decorationJson->Contains("type"));
    EXPECT_TRUE(decorationJson->GetValue("type")->GetString() ==
                V2::ConvertWrapTextDecorationToStirng(Ace::TextDecoration::NONE));
    EXPECT_TRUE(decorationJson->Contains("color"));
    EXPECT_TRUE(decorationJson->GetValue("color")->GetString() == Color::BLACK.ColorToString());
    EXPECT_TRUE(decorationJson->Contains("style"));
    EXPECT_TRUE(decorationJson->GetValue("style")->GetString() ==
                V2::ConvertWrapTextDecorationStyleToString(Ace::TextDecorationStyle::SOLID));
}

/**
 * @tc.name: SpanDecorationToJsonValue003
 * @tc.desc: Test Span Decoration ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanDecorationToJsonValue003, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(spanNode, nullptr);
    auto json = JsonUtil::Create(true);
    spanNode->ToJsonValue(json, filter);
    EXPECT_TRUE(json->Contains("content"));
    EXPECT_TRUE(json->GetValue("content")->GetString() == CREATE_VALUE);
    EXPECT_TRUE(json->Contains("decoration"));
}

/**
 * @tc.name: SpanItemUpdateParagraph006
 * @tc.desc: Test SpanItem UpdateParagraph when with ai span.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanItemUpdateParagraph006, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    pattern->SetTextDetectEnable(true);
    auto node = FrameNode::CreateFrameNode("Test", 1, pattern);
    spanNode->SetParent(node);
    spanNode->MountToParagraph();
    ASSERT_NE(spanNode->GetParent(), nullptr);

    auto json = std::make_unique<JsonValue>();
    AISpan aiSpan1;
    aiSpan1.start = AI_SPAN_START;
    aiSpan1.end = AI_SPAN_END;
    aiSpan1.content = SPAN_PHONE;
    aiSpan1.type = TextDataDetectType::PHONE_NUMBER;
    AISpan aiSpan2;
    aiSpan2.start = AI_SPAN_START_II;
    aiSpan2.end = AI_SPAN_END_II;
    aiSpan2.content = SPAN_URL;
    aiSpan2.type = TextDataDetectType::URL;
    std::map<int32_t, AISpan> aiSpanMap;
    aiSpanMap[AI_SPAN_START] = aiSpan1;
    aiSpanMap[AI_SPAN_START_II] = aiSpan2;
    spanNode->spanItem_->aiSpanMap = aiSpanMap;
    spanNode->spanItem_->content = U16TEXT_FOR_AI;
    spanNode->spanItem_->fontStyle = nullptr;
    RefPtr<SpanItem> spanItem = AceType::MakeRefPtr<SpanItem>();
    spanNode->spanItem_->children.push_back(spanItem);
    TextStyle textStyle;
    ParagraphStyle paraStyle = { .direction = TextDirection::LTR,
        .align = textStyle.GetTextAlign(),
        .maxLines = textStyle.GetMaxLines(),
        .fontLocale = "zh-CN",
        .wordBreak = textStyle.GetWordBreak(),
        .textOverflow = textStyle.GetTextOverflow() };
    auto paragraph = Paragraph::Create(paraStyle, FontCollection::Current());
    spanNode->spanItem_->UpdateParagraph(nullptr, paragraph, TextStyle());
    EXPECT_EQ(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: UpdateTextStyleForAISpan001
 * @tc.desc: Test SpanItem UpdateTextStyleForAISpan when children is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, UpdateTextStyleForAISpan001, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    
    auto pattern = AceType::MakeRefPtr<TextPattern>();
    pattern->SetTextDetectEnable(true);
    auto node = FrameNode::CreateFrameNode("Test", 1, pattern);
    spanNode->SetParent(node);
    spanNode->MountToParagraph();
    ASSERT_NE(spanNode->GetParent(), nullptr);

    Ace::AISpan aiSpan1;
    aiSpan1.start = AI_SPAN_START;
    aiSpan1.end = AI_SPAN_END;
    aiSpan1.content = SPAN_PHONE;
    aiSpan1.type = TextDataDetectType::PHONE_NUMBER;
    Ace::AISpan aiSpan2;
    aiSpan2.start = AI_SPAN_START_II;
    aiSpan2.end = AI_SPAN_END_II;
    aiSpan2.content = SPAN_URL;
    aiSpan2.type = TextDataDetectType::URL;
    std::map<int32_t, Ace::AISpan> aiSpanMap;
    aiSpanMap[AI_SPAN_START] = aiSpan1;
    aiSpanMap[AI_SPAN_START_II] = aiSpan2;
    spanNode->spanItem_->aiSpanMap = aiSpanMap;
    spanNode->spanItem_->fontStyle = nullptr;

    std::u16string spanContent = U16TEXT_FOR_AI;
    spanNode->spanItem_->position = spanContent.length();
    TextStyle textStyle;
    ParagraphStyle paraStyle = { .direction = TextDirection::LTR,
        .align = textStyle.GetTextAlign(),
        .maxLines = textStyle.GetMaxLines(),
        .fontLocale = "zh-CN",
        .wordBreak = textStyle.GetWordBreak(),
        .textOverflow = textStyle.GetTextOverflow() };
    auto paragraph= MockParagraph::GetOrCreateMockParagraph();

    auto aiSpanStyle = textStyle;
    pattern->ModifyAISpanStyle(aiSpanStyle);
    spanNode->spanItem_->UpdateTextStyleForAISpan(spanContent, paragraph, textStyle, aiSpanStyle);
    EXPECT_EQ(spanNode->spanItem_->fontStyle, nullptr);
}

/**
 * @tc.name: SymbolSpanPropertyTest001
 * @tc.desc: Test fontSize and fonColor property of symbolspan.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SymbolSpanPropertyTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create symbol span node
     */
    SymbolSpanModelNG symbolSpanModelNG;
    symbolSpanModelNG.Create(SYMBOL_ID);

    /**
     * @tc.steps: step2. get span node
     */
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);

    /**
     * @tc.steps: step3. test fontSize property
     */
    symbolSpanModelNG.SetFontSize(FONT_SIZE_VALUE);
    EXPECT_EQ(spanNode->GetFontSize(), FONT_SIZE_VALUE);

    /**
     * @tc.steps: step4. test fontColor property
     */
    symbolSpanModelNG.SetFontColor(SYMBOL_COLOR);
    EXPECT_EQ(spanNode->GetSymbolColorList(), SYMBOL_COLOR);

    symbolSpanModelNG.SetFontColor(SYMBOL_COLOR_LIST);
    EXPECT_EQ(spanNode->GetSymbolColorList(), SYMBOL_COLOR_LIST);
}

/**
 * @tc.name: SymbolSpanPropertyTest002
 * @tc.desc: Test fontWeight property of symbolspan.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SymbolSpanPropertyTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create symbol span node
     */
    SymbolSpanModelNG symbolSpanModelNG;
    symbolSpanModelNG.Create(SYMBOL_ID);

    /**
     * @tc.steps: step2. get span node
     */
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);

    /**
     * @tc.steps: step3. test fontWeight property
     */
    symbolSpanModelNG.SetFontWeight(FontWeight::LIGHTER);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::LIGHTER);
    symbolSpanModelNG.SetFontWeight(FontWeight::REGULAR);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::REGULAR);
    symbolSpanModelNG.SetFontWeight(FontWeight::NORMAL);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::NORMAL);
    symbolSpanModelNG.SetFontWeight(FontWeight::MEDIUM);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::MEDIUM);
    symbolSpanModelNG.SetFontWeight(FontWeight::BOLD);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::BOLD);
    symbolSpanModelNG.SetFontWeight(FontWeight::BOLDER);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::BOLDER);

    symbolSpanModelNG.SetFontWeight(FontWeight::W100);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W100);
    symbolSpanModelNG.SetFontWeight(FontWeight::W200);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W200);
    symbolSpanModelNG.SetFontWeight(FontWeight::W300);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W300);
    symbolSpanModelNG.SetFontWeight(FontWeight::W400);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W400);
    symbolSpanModelNG.SetFontWeight(FontWeight::W500);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W500);
    symbolSpanModelNG.SetFontWeight(FontWeight::W600);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W600);
    symbolSpanModelNG.SetFontWeight(FontWeight::W700);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W700);
    symbolSpanModelNG.SetFontWeight(FontWeight::W800);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W800);
    symbolSpanModelNG.SetFontWeight(FontWeight::W900);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::W900);
}

/**
 * @tc.name: SymbolSpanPropertyTest003
 * @tc.desc: Test render strategy and effect strategy of symbolspan.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SymbolSpanPropertyTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create symbol span node
     */
    SymbolSpanModelNG symbolSpanModelNG;
    symbolSpanModelNG.Create(SYMBOL_ID);

    /**
     * @tc.steps: step2. get span node
     */
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);

    /**
     * @tc.steps: step3. test symbol rendering strategy property
     */
    symbolSpanModelNG.SetSymbolRenderingStrategy(RENDER_STRATEGY_SINGLE);
    EXPECT_EQ(spanNode->GetSymbolRenderingStrategy(), RENDER_STRATEGY_SINGLE);
    symbolSpanModelNG.SetSymbolRenderingStrategy(RENDER_STRATEGY_MULTI_COLOR);
    EXPECT_EQ(spanNode->GetSymbolRenderingStrategy(), RENDER_STRATEGY_MULTI_COLOR);
    symbolSpanModelNG.SetSymbolRenderingStrategy(RENDER_STRATEGY_MULTI_OPACITY);
    EXPECT_EQ(spanNode->GetSymbolRenderingStrategy(), RENDER_STRATEGY_MULTI_OPACITY);

    /**
     * @tc.steps: step4. test symbol effect strategy property
     */
    symbolSpanModelNG.SetSymbolEffect(EFFECT_STRATEGY_NONE);
    EXPECT_EQ(spanNode->GetSymbolEffectStrategy(), EFFECT_STRATEGY_NONE);
    symbolSpanModelNG.SetSymbolEffect(EFFECT_STRATEGY_SCALE);
    EXPECT_EQ(spanNode->GetSymbolEffectStrategy(), EFFECT_STRATEGY_SCALE);
    symbolSpanModelNG.SetSymbolEffect(EFFECT_STRATEGY_HIERARCHICAL);
    EXPECT_EQ(spanNode->GetSymbolEffectStrategy(), EFFECT_STRATEGY_HIERARCHICAL);
}

/**
 * @tc.name: SymbolSpanCreateTest001
 * @tc.desc: Test render strategy and effect strategy of symbolspan.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SymbolSpanCreateTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create symbol span node
     */
    SymbolSpanModelNG symbolSpanModelNG;
    symbolSpanModelNG.Create(SYMBOL_ID);

    /**
     * @tc.steps: step2. get span node
     */
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);

    /**
     * @tc.steps: step3. test symbol id
     */
    auto symbolId = spanNode->spanItem_->GetSymbolUnicode();
    EXPECT_EQ(symbolId, SYMBOL_ID);
}

/**
 * @tc.name: ImageSpanEventTest001
 * @tc.desc: Test ImageSpan onComplete event.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, ImageSpanEventTest001, TestSize.Level1)
{
    ImageModelNG imageSpan;
    ImageInfoConfig imageInfoConfig;
    imageInfoConfig.src = std::make_shared<std::string>(IMAGE_SRC_URL);
    imageInfoConfig.bundleName = BUNDLE_NAME;
    imageInfoConfig.moduleName = MODULE_NAME;
    RefPtr<PixelMap> pixMap = nullptr;
    imageSpan.Create(imageInfoConfig, pixMap);
    NG::ImageSpanView::Create();
    bool isTrigger = false;
    auto onComplete = [&isTrigger](const LoadImageSuccessEvent& info) { isTrigger = true; };
    imageSpan.SetOnComplete(std::move(onComplete));
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    EXPECT_EQ(frameNode->GetTag(), V2::IMAGE_ETS_TAG);
    auto eventHub = frameNode->GetEventHub<NG::ImageEventHub>();
    ASSERT_NE(eventHub, nullptr);
    LoadImageSuccessEvent loadImageSuccessEvent(IMAGE_SOURCESIZE_WIDTH, IMAGE_SOURCESIZE_HEIGHT, WIDTH, HEIGHT, 1);
    eventHub->FireCompleteEvent(loadImageSuccessEvent);
    EXPECT_EQ(isTrigger, true);
}

/**
 * @tc.name: ImageSpanEventTest002
 * @tc.desc: Test ImageSpan onError event.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, ImageSpanEventTest002, TestSize.Level1)
{
    ImageModelNG imageSpan;
    ImageInfoConfig imageInfoConfig;
    imageInfoConfig.src = std::make_shared<std::string>(IMAGE_SRC_URL);
    imageInfoConfig.bundleName = BUNDLE_NAME;
    imageInfoConfig.moduleName = MODULE_NAME;
    RefPtr<PixelMap> pixMap = nullptr;
    imageSpan.Create(imageInfoConfig, pixMap);
    NG::ImageSpanView::Create();
    bool isTrigger = false;
    auto onError = [&isTrigger](const LoadImageFailEvent& info) { isTrigger = true; };
    imageSpan.SetOnError(std::move(onError));
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    EXPECT_EQ(frameNode->GetTag(), V2::IMAGE_ETS_TAG);
    auto eventHub = frameNode->GetEventHub<NG::ImageEventHub>();
    ASSERT_NE(eventHub, nullptr);
    LoadImageFailEvent loadImageFailEvent(WIDTH, HEIGHT, "image load error!");
    eventHub->FireErrorEvent(loadImageFailEvent);
    EXPECT_EQ(isTrigger, true);
}

/**
 * @tc.name: SpanModelSetFont002
 * @tc.desc: Test SetFont.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanModelSetFont002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize SpanModelNG
     */
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    ASSERT_NE(spanNode, nullptr);
    /**
     * @tc.steps: step2. Set Font, call SetFont
     * @tc.expected: cover branch font is null
     */
    Font font;
    std::optional<Dimension> fontSize;
    UINode* uiNode = ViewStackProcessor::GetInstance()->GetMainElementNode().GetRawPtr();
    spanModelNG.SetFont(font);
    spanModelNG.SetFont(uiNode, font);
    EXPECT_EQ(spanNode->GetFontSize(), fontSize);
    /**
     * @tc.steps: step3. Set Font, call SetFont
     * @tc.expected: cover branch font has value
     */
    font.fontSize = FONT_SIZE_VALUE;
    font.fontWeight = FontWeight::BOLD;
    font.fontFamilies = FONT_FAMILY_VALUE;
    font.fontStyle = ITALIC_FONT_STYLE_VALUE;
    spanModelNG.SetFont(uiNode, font);
    /**
     * @tc.steps: step4. Gets the relevant properties of the Font
     * @tc.expected: Check the font value
     */
    EXPECT_EQ(spanNode->GetFontSize(), FONT_SIZE_VALUE);
    EXPECT_EQ(spanNode->GetFontWeight().value(), FontWeight::BOLD);
    EXPECT_EQ(spanNode->GetFontFamily(), FONT_FAMILY_VALUE);
    EXPECT_EQ(spanNode->GetItalicFontStyle(), ITALIC_FONT_STYLE_VALUE);
}

/**
 * @tc.name: SpanNodeGetOrCreateSpanNode004
 * @tc.desc: Test SpanNode GetOrCreateSpanNode when span node is nut nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeGetOrCreateSpanNode004, TestSize.Level1)
{
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto node = spanNode->GetOrCreateSpanNode(V2::SYMBOL_SPAN_ETS_TAG, 1);
    ASSERT_NE(node, nullptr);
    node = spanNode->GetOrCreateSpanNode(V2::SYMBOL_SPAN_ETS_TAG, 1);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(node->GetId(), 1);
}

/**
 * @tc.name: SpanNodeDumpInfo001
 * @tc.desc: Test SpanNode DumpInfo.
 * @tc.type: FUNC
 */
HWTEST_F(SpanTestNg, SpanNodeDumpInfo001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialize SpanNode
     */
    DumpLog::GetInstance().description_.clear();
    SpanModelNG spanModelNG;
    spanModelNG.Create(CREATE_VALUE_W);
    auto spanNode = AceType::DynamicCast<SpanNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
    auto symbolNode = spanNode->GetOrCreateSpanNode(V2::SYMBOL_SPAN_ETS_TAG, 1);
    /**
     * @tc.steps: step2. call DumpInfo
     * @tc.expected: cover branch textStyle is null
     */
    std::optional<TextStyle> textStyle;
    spanNode->DumpInfo();
    EXPECT_NE(DumpLog::GetInstance().description_.size(), 0);
    /**
     * @tc.steps: step3. call DumpInfo
     * @tc.expected: cover branch textStyle has value
     */
    DumpLog::GetInstance().description_.clear();
    textStyle = std::optional<TextStyle>(TextStyle());
    spanNode->spanItem_->content = u"";
    spanNode->spanItem_->SetTextStyle(textStyle);
    spanNode->DumpInfo();
    EXPECT_NE(DumpLog::GetInstance().description_.size(), 1);
    /**
     * @tc.steps: step4. call DumpInfo
     * @tc.expected: cover branch Tag is SYMBOL_SPAN_ETS_TAG
     */
    DumpLog::GetInstance().description_.clear();
    symbolNode->spanItem_->SetTextStyle(textStyle);
    symbolNode->DumpInfo();
    EXPECT_EQ(symbolNode->GetTag(), V2::SYMBOL_SPAN_ETS_TAG);
    EXPECT_NE(DumpLog::GetInstance().description_.size(), 1);
}
} // namespace OHOS::Ace::NG

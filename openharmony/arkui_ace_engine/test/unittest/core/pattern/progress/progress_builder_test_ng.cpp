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

#include <cmath>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "gtest/gtest.h"

#include "base/memory/referenced.h"
#define private public
#define protected public

#include "test/mock/base/mock_task_executor.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/rosen/mock_canvas.h"
#include "test/unittest/core/pattern/test_ng.h"

#include "base/memory/ace_type.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/theme/app_theme.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/base/view_abstract_model_ng.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/progress/progress_layout_algorithm.h"
#include "core/components_ng/pattern/progress/progress_layout_property.h"
#include "core/components_ng/pattern/progress/progress_model_ng.h"
#include "core/components_ng/pattern/progress/progress_paint_property.h"
#include "core/components_ng/pattern/progress/progress_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/property/progress_mask_property.h"
#include "core/components_ng/render/render_context.h"

namespace OHOS::Ace::NG {
namespace {
using namespace testing;
using namespace testing::ext;
const InspectorFilter filter;
constexpr double MAX_VALUE = 1.0;
constexpr double MIN_VALUE = 0.0;
constexpr double NEG_VALUE = -1.0;
constexpr double VALUE_OF_PROGRESS = 100.0;
constexpr double NEG_VALUE_OF_PROGRESS = -100.0;
constexpr double MAX_VALUE_OF_PROGRESS = 100000.0;
constexpr double MAX_NEG_VALUE_OF_PROGRESS = -100000.0;
constexpr ProgressType PROGRESS_TYPE_LINEAR = ProgressType::LINEAR;
constexpr ProgressType PROGRESS_TYPE_SCALE = ProgressType::SCALE;
constexpr ProgressType PROGRESS_TYPE_MOON = ProgressType::MOON;
constexpr int32_t SCALE_COUNT = 120;
constexpr Color FRONT_COLOR = Color(0xff0000ff);
constexpr Color BG_COLOR = Color(0xffc0c0c0);
constexpr float PROGRESS_COMPONENT_WIDTH = 200.0f;
constexpr float PROGRESS_COMPONENT_HEIGHT = 200.0f;
constexpr Dimension TEST_PROGRESS_THICKNESS = 4.0_vp;
constexpr Dimension TEST_PROGRESS_STROKE_WIDTH = 10.0_vp;
constexpr Dimension TEST_PROGRESS_DEFAULT_DIAMETER = 72.0_vp;
constexpr Dimension TEST_PROGRESS_SCALE_WIDTH = 2.0_vp;
const Color TEST_COLOR = Color::BLUE;
const LinearColor TEST_LINEARCOLOR = LinearColor(TEST_COLOR);
const std::vector<std::string> FONT_FAMILY = { "serif" };

ProgressModelNG progressModel;
RefPtr<ProgressTheme> progressTheme;
RefPtr<MockThemeManager> themeManager;
} // namespace

class ProgressBuilderTestNg : public TestNG {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    void SetUp() override;
    void TearDown() override;
    void GetProgress();
    ProgressModelNG CreateProgress(double value, double max, NG::ProgressType type);

    RefPtr<FrameNode> frameNode_;
    RefPtr<ProgressPattern> pattern_;
    RefPtr<EventHub> eventHub_;
    RefPtr<ProgressLayoutProperty> layoutProperty_;
    RefPtr<ProgressPaintProperty> paintProperty_;
    RefPtr<ProgressAccessibilityProperty> accessibilityProperty_;
};

void ProgressBuilderTestNg::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
    auto pipeline = PipelineContext::GetCurrentContext();
    pipeline->SetMinPlatformVersion(static_cast<int32_t>(PlatformVersion::VERSION_TEN));
    themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    progressTheme = AceType::MakeRefPtr<ProgressTheme>();
    progressTheme->trackThickness_ = TEST_PROGRESS_THICKNESS;
    progressTheme->scaleLength_ = TEST_PROGRESS_STROKE_WIDTH;
    progressTheme->ringDiameter_ = TEST_PROGRESS_DEFAULT_DIAMETER;
    progressTheme->trackBgColor_ = BG_COLOR;
    progressTheme->trackSelectedColor_ = FRONT_COLOR;
    progressTheme->scaleNumber_ = SCALE_COUNT;
    progressTheme->scaleWidth_ = TEST_PROGRESS_SCALE_WIDTH;
}

void ProgressBuilderTestNg::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
    progressTheme = nullptr;
    themeManager = nullptr;
}

void ProgressBuilderTestNg::SetUp()
{
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(progressTheme));
}

void ProgressBuilderTestNg::TearDown()
{
    frameNode_ = nullptr;
    pattern_ = nullptr;
    eventHub_ = nullptr;
    layoutProperty_ = nullptr;
    paintProperty_ = nullptr;
    accessibilityProperty_ = nullptr;
    ClearOldNodes(); // Each testcase will create new node at begin
}

void ProgressBuilderTestNg::GetProgress()
{
    RefPtr<UINode> element = ViewStackProcessor::GetInstance()->GetMainElementNode();
    frameNode_ = AceType::DynamicCast<FrameNode>(element);
    pattern_ = frameNode_->GetPattern<ProgressPattern>();
    eventHub_ = frameNode_->GetEventHub<EventHub>();
    layoutProperty_ = frameNode_->GetLayoutProperty<ProgressLayoutProperty>();
    paintProperty_ = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    accessibilityProperty_ = frameNode_->GetAccessibilityProperty<ProgressAccessibilityProperty>();
}

ProgressModelNG ProgressBuilderTestNg::CreateProgress(double value, double max, NG::ProgressType type)
{
    ResetElmtId();
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(GetElmtId());
    ProgressModelNG model;
    model.Create(MIN_VALUE, value, MIN_VALUE, max, type);
    ViewAbstract::SetWidth(CalcLength(PROGRESS_COMPONENT_WIDTH));
    ViewAbstract::SetHeight(CalcLength(PROGRESS_COMPONENT_HEIGHT));
    GetProgress();
    return model;
}

/**
 * @tc.name: ProgressConPatternTest001
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    auto model = CreateProgress(MAX_VALUE_OF_PROGRESS, MAX_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetShowText(true);
    model.SetBackgroundColor(Color::GRAY);
    CreateDone(frameNode_);
    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest002
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    auto model = CreateProgress(MAX_NEG_VALUE_OF_PROGRESS, MAX_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetShowText(true);
    model.SetBackgroundColor(Color::GRAY);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest003
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    auto model = CreateProgress(MAX_VALUE, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetShowText(true);
    model.SetBackgroundColor(Color::GRAY);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 1.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_VALUE);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest004
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress.
     */
    CreateProgress(MIN_VALUE, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 0.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MIN_VALUE);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest005
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(NEG_VALUE, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to -1.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, NEG_VALUE);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest006
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest007
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(NEG_VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to -100.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest008
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(MAX_VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest009
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(MAX_NEG_VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_LINEAR);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest010
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    auto model = CreateProgress(MAX_VALUE_OF_PROGRESS, MAX_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetShowText(true);
    model.SetBackgroundColor(Color::GRAY);
    CreateDone(frameNode_);
    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest011
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    auto model = CreateProgress(MAX_NEG_VALUE_OF_PROGRESS, MAX_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetShowText(true);
    model.SetBackgroundColor(Color::GRAY);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest012
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    auto model = CreateProgress(MAX_VALUE, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetShowText(true);
    model.SetBackgroundColor(Color::GRAY);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 1.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_VALUE);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest013
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress.
     */
    CreateProgress(MIN_VALUE, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 0.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MIN_VALUE);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest014
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(NEG_VALUE, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to -1.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, NEG_VALUE);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest015
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest016
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(NEG_VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to -100.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest017
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(MAX_VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest018
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress model and set relative properties.
     */
    ProgressModelNG model = CreateProgress(MAX_NEG_VALUE_OF_PROGRESS, MAX_NEG_VALUE_OF_PROGRESS, PROGRESS_TYPE_SCALE);
    model.SetStrokeWidth(500.0_vp);
    model.SetColor(FRONT_COLOR);
    model.SetBackgroundColor(BG_COLOR);
    CreateDone(frameNode_);

    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_NEG_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to 100000.0.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_NEG_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}

/**
 * @tc.name: ProgressConPatternTest019
 * @tc.desc: SetBuilderFunc and get value
 * @tc.type: FUNC
 */
HWTEST_F(ProgressBuilderTestNg, ProgressConPatternTest019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create progress.
     */
    CreateProgress(MAX_VALUE_OF_PROGRESS, MAX_VALUE_OF_PROGRESS, PROGRESS_TYPE_MOON);
    CreateDone(frameNode_);
    auto eventHub = frameNode_->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetEnabled(true);

    auto progressPaintProperty = frameNode_->GetPaintProperty<ProgressPaintProperty>();
    ASSERT_NE(progressPaintProperty, nullptr);
    progressPaintProperty->UpdateMaxValue(MAX_VALUE_OF_PROGRESS);
    auto pattern = frameNode_->GetPattern<ProgressPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Init Progress node.
     * @tc.expected: step2. Check the value_ of config is equal to MAX_VALUE_OF_PROGRESS.
     */
    auto node = [](ProgressConfiguration config) -> RefPtr<FrameNode> {
        EXPECT_EQ(config.value_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.total_, MAX_VALUE_OF_PROGRESS);
        EXPECT_EQ(config.enabled_, true);
        return nullptr;
    };

    /**
     * @tc.steps: step3. Set parameters to pattern builderFunc
     */
    pattern_->SetBuilderFunc(node);
    pattern_->BuildContentModifierNode();
}
} // namespace OHOS::Ace::NG
/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <functional>
#include <optional>
#include <utility>

#include "gtest/gtest-message.h"
#include "gtest/gtest.h"

#define private public
#define protected public
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/common/mock_theme_default.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "base/geometry/point.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/animation/curves.h"
#include "core/components/common/properties/color.h"
#include "core/components/picker/picker_data.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/event/click_event.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/event/focus_hub.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/dialog/dialog_pattern.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/select_overlay/select_overlay_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/time_picker/timepicker_column_pattern.h"
#include "core/components_ng/pattern/time_picker/timepicker_dialog_view.h"
#include "core/components_ng/pattern/time_picker/timepicker_model_ng.h"
#include "core/components_ng/pattern/time_picker/timepicker_row_pattern.h"
#include "core/components_ng/pattern/time_picker/toss_animation_controller.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/event/key_event.h"
#include "core/event/touch_event.h"
#include "core/gestures/gesture_info.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

} // namespace OHOS::Ace

namespace OHOS::Ace::NG {
namespace {
const InspectorFilter filter;
const int SHOW_COUNT = 7;
const Dimension FONT_VALUE_NOMARL = Dimension(10);
const Dimension FONT_SIZE_LIMIT = Dimension(10);
const Dimension FONT_SIZE_VP = Dimension(10.0_vp);
const Dimension FONT_SIZE_LIMIT_MORE = Dimension(40);
} // namespace
class TimePickerDialogViewTestNg : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    void SetUp() override;
    void TearDown() override;
    void CreateTimePickerColumnNode();

    RefPtr<FrameNode> columnNode_;
    RefPtr<TimePickerColumnPattern> columnPattern_;
};

class TestNode : public UINode {
    DECLARE_ACE_TYPE(TestNode, UINode);

public:
    static RefPtr<TestNode> CreateTestNode(int32_t nodeId)
    {
        auto spanNode = MakeRefPtr<TestNode>(nodeId);
        return spanNode;
    }

    bool IsAtomicNode() const override
    {
        return true;
    }

    explicit TestNode(int32_t nodeId) : UINode("TestNode", nodeId) {}
    ~TestNode() override = default;
};

void TimePickerDialogViewTestNg::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
}

void TimePickerDialogViewTestNg::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
}

void TimePickerDialogViewTestNg::SetUp()
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    EXPECT_CALL(*themeManager, GetTheme(_))
        .WillRepeatedly([](ThemeType type) -> RefPtr<Theme> {
            if (type == IconTheme::TypeId()) {
                return AceType::MakeRefPtr<IconTheme>();
            } else if (type == DialogTheme::TypeId()) {
                return AceType::MakeRefPtr<DialogTheme>();
            } else if (type == PickerTheme::TypeId()) {
                return MockThemeDefault::GetPickerTheme();
            } else if (type == ButtonTheme::TypeId()) {
                return AceType::MakeRefPtr<ButtonTheme>();
            } else {
                return nullptr;
            }
        });
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
}

void TimePickerDialogViewTestNg::TearDown()
{
    MockPipelineContext::GetCurrent()->themeManager_ = nullptr;
    ViewStackProcessor::GetInstance()->ClearStack();
}

void TimePickerDialogViewTestNg::CreateTimePickerColumnNode()
{
    auto theme = MockPipelineContext::GetCurrent()->GetTheme<PickerTheme>();
    ASSERT_NE(theme, nullptr);
    TimePickerModelNG::GetInstance()->CreateTimePicker(theme);
    auto pickerFrameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(pickerFrameNode, nullptr);
    pickerFrameNode->MarkModifyDone();

    auto timePickerRowPattern = pickerFrameNode->GetPattern<TimePickerRowPattern>();
    ASSERT_NE(timePickerRowPattern, nullptr);
    timePickerRowPattern->UpdateAllChildNode();
    auto allChildNode = timePickerRowPattern->GetAllChildNode();
    columnNode_ = allChildNode["minute"].Upgrade();
    ASSERT_NE(columnNode_, nullptr);
    columnPattern_ = columnNode_->GetPattern<TimePickerColumnPattern>();
    ASSERT_NE(columnPattern_, nullptr);
    columnPattern_->SetShowCount(SHOW_COUNT);
    columnPattern_->OnAttachToFrameNode();
    auto host = timePickerRowPattern->GetHost();
    EXPECT_NE(host, nullptr);
}

/**
 * @tc.name: TimePickerDialogViewTest001
 * @tc.desc: Test TimePickerDialogView::ConvertFontScaleValue.
 * @tc.type: FUNC
 */
HWTEST_F(TimePickerDialogViewTestNg, TimePickerDialogViewTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get real pipeline and theme, set fontscale.
     * @tc.expected: call GetIsUserSetTextProperties and NeedAdaptForAging is false.
     */
    auto pipeline = PipelineContext::GetCurrentContext();
    ASSERT_TRUE(pipeline);
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();
    ASSERT_TRUE(pickerTheme);
    pipeline->SetFollowSystem(true);
    pipeline->SetMaxAppFontScale(3.2f);
    pipeline->SetFontScale(0);
    /**
     * @tc.steps: step2. fontSizeValue's unit is VP,isUserSetFont is true,and
     * @tc.expected: call GetIsUserSetTextProperties and result expect equal FONT_SIZE_LIMIT.
     */
    auto ret = TimePickerDialogView::ConvertFontScaleValue(FONT_SIZE_VP, FONT_SIZE_LIMIT, true);
    EXPECT_EQ(ret.Value(), FONT_SIZE_LIMIT.Value());
    /**
     * @tc.steps: step2+. fontSizeValue's unit is not VP,isUserSetFont is false,and
     * @tc.expected: call GetIsUserSetTextProperties and result expect FONT_VALUE_NOMARL / FontScale.
     */
    pipeline->SetFontScale(10);
    pipeline->rootHeight_ = 0;
    pickerTheme->pickerDialogMaxOneFontScale_ = 5;
    ret = TimePickerDialogView::ConvertFontScaleValue(FONT_VALUE_NOMARL, FONT_SIZE_LIMIT, false);
    EXPECT_EQ(ret, Dimension(FONT_VALUE_NOMARL / pipeline->GetMaxAppFontScale()));
    /**
     * @tc.steps: step3. set fontscale,fontSizeValue's unit is VP,isUserSetFont is true/false.
     * @tc.expected: call GetIsUserSetTextProperties and NeedAdaptForAging is true.
     */
    pipeline->SetFontScale(10);
    pickerTheme->pickerDialogNormalFontScale_ = 10;
    pickerTheme->pickerDialogMaxOneFontScale_ = 5;
    pipeline->rootHeight_ = 1000;
    ret = TimePickerDialogView::ConvertFontScaleValue(FONT_VALUE_NOMARL, FONT_SIZE_LIMIT, false);
    auto scale = pickerTheme->GetMaxTwoFontScale() / pickerTheme->GetMaxThirdFontScale();
    EXPECT_EQ(ret, FONT_VALUE_NOMARL * scale);
    /**
     * @tc.steps: step3+. fontSizeValue's unit is VP,isUserSetFont is true/false.
     * @tc.expected: call GetIsUserSetTextProperties and result expect FONT_SIZE_LIMIT/FONT_SIZE_VP.
     */
    pickerTheme->pickerDialogMaxTwoFontScale_ = 10;
    pickerTheme->pickerDialogMaxThirdFontScale_ = 5;
    ret = TimePickerDialogView::ConvertFontScaleValue(FONT_SIZE_VP, FONT_SIZE_LIMIT, true);
    EXPECT_EQ(ret.Value(), FONT_SIZE_LIMIT.Value());
    ret = TimePickerDialogView::ConvertFontScaleValue(FONT_SIZE_VP, FONT_SIZE_LIMIT, false);
    EXPECT_EQ(ret.Value(), FONT_SIZE_VP.Value());
}

/**
 * @tc.name: TimePickerDialogViewTest002
 * @tc.desc: Test TimePickerDialogView::ConvertFontSizeLimit.
 * @tc.type: FUNC
 */
HWTEST_F(TimePickerDialogViewTestNg, TimePickerDialogViewTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get real pipeline and theme, set fontscale.
     * @tc.expected: call ConvertFontSizeLimit and isUserSetFont is false.
     */
    auto pipeline = PipelineContext::GetCurrentContext();
    ASSERT_TRUE(pipeline);
    pipeline->SetFollowSystem(true);
    pipeline->SetMaxAppFontScale(3.2f);
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();
    ASSERT_TRUE(pickerTheme);
    pipeline->SetFontScale(0);
    auto ret = TimePickerDialogView::ConvertFontSizeLimit(FONT_SIZE_VP, FONT_SIZE_LIMIT, false);
    EXPECT_EQ(ret.Value(), FONT_SIZE_VP.Value());
    /**
     * @tc.steps: step2. fontSizeValue's unit is VP,isUserSetFont is true
     * @tc.expected: call ConvertFontSizeLimit and result expect FONT_SIZE_LIMIT_MORE/FONT_SIZE_VP.
     */
    auto nFontScale = 5;
    pipeline->SetFontScale(nFontScale);
    ret = TimePickerDialogView::ConvertFontSizeLimit(FONT_SIZE_VP, FONT_SIZE_LIMIT, true);
    EXPECT_EQ(ret.Value(), FONT_SIZE_LIMIT.Value() / pipeline->GetMaxAppFontScale());
    pipeline->SetFontScale(0);
    ret = TimePickerDialogView::ConvertFontSizeLimit(FONT_SIZE_VP, FONT_SIZE_LIMIT_MORE, true);
    EXPECT_EQ(ret.Value(), FONT_SIZE_VP.Value());
    /**
     * @tc.steps: step2+. fontSizeValue's unit is not VP,isUserSetFont is true
     * @tc.expected: call ConvertFontSizeLimit and result expect FONT_SIZE_LIMIT_MORE/FONT_VALUE_NOMARL.
     */
    ret = TimePickerDialogView::ConvertFontSizeLimit(FONT_VALUE_NOMARL, FONT_SIZE_LIMIT, true);
    EXPECT_EQ(ret.Value(), FONT_SIZE_LIMIT.Value());
    ret = TimePickerDialogView::ConvertFontSizeLimit(FONT_VALUE_NOMARL, FONT_SIZE_LIMIT_MORE, true);
    EXPECT_EQ(ret.Value(), FONT_VALUE_NOMARL.Value());
}

/**
 * @tc.name: TimePickerDialogViewTest003
 * @tc.desc: Test TimePickerDialogView::ConvertTitleFontScaleValue.
 * @tc.type: FUNC
 */
HWTEST_F(TimePickerDialogViewTestNg, TimePickerDialogViewTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get real pipeline and theme, set fontscale.
     */
    auto pipeline = PipelineContext::GetCurrentContext();
    ASSERT_TRUE(pipeline);
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();
    ASSERT_TRUE(pickerTheme);
    pipeline->SetFollowSystem(true);
    pipeline->SetMaxAppFontScale(10.0f);
    pipeline->SetFontScale(0);
    /**
     * @tc.steps: step2. set NeedAdaptForAging is false,
     * fontSizeValue's unit is VP,fontScale < GetMaxOneFontScale();
     * @tc.expected: call GetIsUserSetTextProperties and result expect FONT_SIZE_VP.
     */
    auto ret = TimePickerDialogView::ConvertTitleFontScaleValue(FONT_SIZE_VP);
    EXPECT_EQ(ret, FONT_SIZE_VP);
    /**
     * @tc.steps: step2+. fontSizeValue's unit is not VP,fontScale > GetMaxOneFontScale();
     * @tc.expected: call GetIsUserSetTextProperties and result expect FONT_VALUE_NOMARL / FontScale.
     */
    pipeline->SetFontScale(10);
    pipeline->rootHeight_ = 0;
    pickerTheme->pickerDialogMaxOneFontScale_ = 5;
    ret = TimePickerDialogView::ConvertTitleFontScaleValue(FONT_VALUE_NOMARL);
    EXPECT_EQ(ret, Dimension(FONT_VALUE_NOMARL.Value() / pipeline->GetFontScale()));
    /**
     * @tc.steps: step3. set NeedAdaptForAging is true.
     * fontSizeValue's unit is VP
     * @tc.expected: call GetIsUserSetTextProperties and result expect FONT_VALUE_NOMARL * adjustScale.
     */
    pipeline->SetFontScale(10);
    pipeline->rootHeight_ = 1000;
    ret = TimePickerDialogView::ConvertTitleFontScaleValue(FONT_SIZE_VP);
    EXPECT_EQ(ret, FONT_SIZE_VP * pickerTheme->GetTitleFontScaleLimit());
    /**
     * @tc.steps: step3+. fontSizeValue's unit is not VP.GetFontScale() < GetTitleFontScaleLimit()
     * @tc.expected: call GetIsUserSetTextProperties and result expect FONT_SIZE_LIMIT.
     */
    ret = TimePickerDialogView::ConvertTitleFontScaleValue(FONT_VALUE_NOMARL);
    auto scale = pickerTheme->GetTitleFontScaleLimit() / pipeline->GetMaxAppFontScale();
    EXPECT_EQ(ret, FONT_VALUE_NOMARL * scale);
}

} // namespace OHOS::Ace::NG

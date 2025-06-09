/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#define private public
#define protected public
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

#include "base/memory/ref_counter.h"
#include "base/memory/referenced.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/event/gesture_event_hub.h"
#include "core/components_ng/pattern/button/button_layout_property.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/custom/custom_node.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/progress/progress_paint_property.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_layout_property.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_model_ng.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_paint_method.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_pattern.h"
#include "core/components_ng/pattern/side_bar/side_bar_theme.h"
#include "core/components_v2/extensions/extension.h"
#include "core/components_v2/inspector/inspector_constants.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
constexpr float UPDATE_SIDE_BAR_WIDTH = 240.0f;
} // namespace

class SideBarPatternTestTwoNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void SideBarPatternTestTwoNg::SetUpTestCase()
{
    MockPipelineContext::SetUp();
}

void SideBarPatternTestTwoNg::TearDownTestCase()
{
    MockPipelineContext::TearDown();
}

/**
 * @tc.name: SideBarToolBarManager_001
 * @tc.desc: Test ToolbarManager SetToolBarChangeCallback and SetModifyDoneCallback.
 * @tc.type: FUNC
 */
HWTEST_F(SideBarPatternTestTwoNg, SideBarToolBarManager_001, TestSize.Level1)
{
    auto pattern = AceType::MakeRefPtr<SideBarContainerPattern>();
    ASSERT_NE(pattern, nullptr);

    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    pattern->frameNode_ = frameNode;
    auto backButton = FrameNode::CreateFrameNode("BackButton", 33, AceType::MakeRefPtr<SideBarContainerPattern>());
    frameNode->children_.push_back(backButton);

    auto themeManagerOne = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManagerOne);
    EXPECT_CALL(*themeManagerOne, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SideBarTheme>()));
    EXPECT_CALL(*themeManagerOne, GetTheme(_, _)).WillRepeatedly(Return(AceType::MakeRefPtr<SideBarTheme>()));

    auto pipelineContext = frameNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto toolBarManager = pipelineContext->GetToolbarManager();
    ASSERT_NE(toolBarManager, nullptr);

    int32_t modifyDoneCallbackCount = 0;
    int32_t toolBarChangeCallback = 0;
    pattern->realDividerWidth_ = 10;
    pattern->realSideBarWidth_ = Dimension(120);
    auto host = pattern->GetHost();
    ASSERT_NE(host, nullptr);
    auto geometryNode = host->GetGeometryNode();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetFrameSize(SizeF(200, 200));

    toolBarManager->SetModifyDoneCallback([&modifyDoneCallbackCount]() { modifyDoneCallbackCount++; });
    toolBarManager->SetToolBarChangeCallback([&toolBarChangeCallback]() { toolBarChangeCallback++; });
    pattern->InitToolBarManager();

    auto layoutProperty = pattern->GetLayoutProperty<SideBarContainerLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    pattern->HandleDragUpdate(UPDATE_SIDE_BAR_WIDTH);
    pattern->realSideBarWidth_ = Dimension(120);
    pattern->OnModifyDone();
    pattern->FireSideBarWidthChangeEvent();
    ASSERT_EQ(modifyDoneCallbackCount, 1);
    ASSERT_EQ(toolBarChangeCallback, 4);
    auto sideBarInfo = toolBarManager->GetSideBarInfo();
    ASSERT_TRUE(sideBarInfo.isShow);
    ASSERT_EQ(sideBarInfo.width, 120);
    ASSERT_TRUE(toolBarManager->HasSideBar());
}
} // namespace OHOS::Ace::NG
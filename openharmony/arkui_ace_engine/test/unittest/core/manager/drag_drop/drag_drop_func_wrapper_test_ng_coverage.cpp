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

#include <optional>

#include "gtest/gtest.h"
#define private public

#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/render/mock_render_context.h"
#include "test/mock/base/mock_drag_window.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/common/mock_interaction_interface.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/unittest/core/event/drag_event/drag_event_test_ng_issue_utils.h"

#include "base/image/pixel_map.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/subwindow/subwindow_manager.h"
#include "core/common/ace_engine.h"
#include "core/common/interaction/interaction_interface.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/theme/blur_style_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/geometry_node.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/manager/drag_drop/drag_drop_func_wrapper.h"
#include "core/components_ng/manager/drag_drop/drag_drop_global_controller.h"
#include "core/components_ng/manager/drag_drop/drag_drop_manager.h"
#include "core/components_ng/manager/drag_drop/drag_drop_proxy.h"
#include "core/components_ng/pattern/grid/grid_item_pattern.h"
#include "core/components_ng/pattern/grid/grid_pattern.h"
#include "ui/base/geometry/ng/offset_t.h"
#include "core/components/select/select_theme.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
RefPtr<DragWindow> MOCK_DRAG_WINDOW;
constexpr size_t DEFAULT_CHILD_COUNT = 4;
constexpr float GRID_WIDTH = 480.0f;
constexpr float GRID_HEIGHT = 800.0f;
constexpr float ITEM_WIDTH = 120.0f;
constexpr float ITEM_HEIGHT = 200.0f;
} // namespace

// test case
struct DragMultiCase {
    bool isAllowDrag = false;
    bool isSelected = false;
    bool isMultiSelectionEnabled = false;
    bool expectResult = false;
};

const std::vector<DragMultiCase> DRAG_MULTI_CASES = {
    { false, false, false, false},
    { false, false, true, false},
    { false, true, false, false},
    { false, true, true, false},
    { true, false, false, false},
    { true, false, true, false},
    { true, true, false, false},
    { true, true, true, true},
};

class DragDropFuncWrapperTestNgCoverage : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void DragDropFuncWrapperTestNgCoverage::SetUpTestCase()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
    MOCK_DRAG_WINDOW = DragWindow::CreateDragWindow("", 0, 0, 0, 0);
}

void DragDropFuncWrapperTestNgCoverage::TearDownTestCase()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
    MOCK_DRAG_WINDOW = nullptr;
}

RefPtr<FrameNode> CreateGridNodeWithChild(size_t childCount)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::GRID_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() {return AceType::MakeRefPtr<GridPattern>(); });
    ViewAbstract::SetWidth(Referenced::RawPtr(frameNode), CalcLength(GRID_WIDTH));
    ViewAbstract::SetHeight(Referenced::RawPtr(frameNode), CalcLength(GRID_HEIGHT));

    for (size_t i = 0; i < childCount; ++i) {
        auto chidNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto childNode = FrameNode::GetOrCreateFrameNode(V2::GRID_ITEM_ETS_TAG, chidNodeId,
            []() { return AceType::MakeRefPtr<GridItemPattern>(nullptr, GridItemStyle::NONE); });
        ViewAbstract::SetWidth(Referenced::RawPtr(childNode), CalcLength(ITEM_WIDTH));
        ViewAbstract::SetHeight(Referenced::RawPtr(childNode), CalcLength(ITEM_HEIGHT));
        childNode->MountToParent(frameNode);
    }
    return frameNode;
}


RefPtr<FrameNode> ProcessDragItemGroupScene()
{
    auto gridNode = CreateGridNodeWithChild(DEFAULT_CHILD_COUNT);
    CHECK_NULL_RETURN(gridNode, nullptr);
    auto pattern = gridNode->GetPattern<GridPattern>();
    CHECK_NULL_RETURN(pattern, nullptr);
    pattern->info_.endIndex_ = DEFAULT_CHILD_COUNT;
    auto gestureEventHub = gridNode->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(gestureEventHub, nullptr);
    gestureEventHub->InitDragDropEvent();
    return gridNode;
}

void SetFrameNodeAllowDrag(RefPtr<FrameNode>& frameNode)
{
    CHECK_NULL_VOID(frameNode);
    frameNode->draggable_ = true;
    auto gestureHub = frameNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    auto eventHub = gestureHub->eventHub_.Upgrade();
    CHECK_NULL_VOID(eventHub);
    auto func = [](const RefPtr<OHOS::Ace::DragEvent>&, const std::string&) { return DragDropInfo(); };
    eventHub->onDragStart_ = func;
}
/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage001
 * @tc.desc: Test DecideWhetherToStopDragging with valid parameters
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage001, TestSize.Level1)
{
    DragPointerEvent pointerEvent;
    std::string extraParams = "test";
    int32_t currentPointerId = 1;
    int32_t containerId = 100;

    auto pipelineContext = PipelineContext::GetContextByContainerId(containerId);
    ASSERT_NE(pipelineContext, nullptr);
    auto manager = pipelineContext->GetDragDropManager();
    ASSERT_NE(manager, nullptr);
    manager->SetDraggingPressedState(false);

    DragDropFuncWrapper::DecideWhetherToStopDragging(pointerEvent, extraParams, currentPointerId, containerId);

    EXPECT_FALSE(manager->IsDraggingPressed(currentPointerId));
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage002
 * @tc.desc: Test DecideWhetherToStopDragging with invalid containerId
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage002, TestSize.Level1)
{
    DragPointerEvent pointerEvent;
    std::string extraParams = "test";
    int32_t currentPointerId = 1;
    int32_t invalidContainerId = -1;

    DragDropFuncWrapper::DecideWhetherToStopDragging(pointerEvent, extraParams, currentPointerId, invalidContainerId);

    // No assertion needed as it should safely exit with CHECK_NULL_VOID
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage003
 * @tc.desc: Test UpdateDragPreviewOptionsFromModifier with various opacity values
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage003, TestSize.Level1)
{
    auto applyOnNodeSync = [](WeakPtr<FrameNode> frameNode) {
        auto node = frameNode.Upgrade();
        CHECK_NULL_VOID(node);
        node->GetRenderContext()->UpdateOpacity(0.5f);
    };

    DragPreviewOption option;
    DragDropFuncWrapper::UpdateDragPreviewOptionsFromModifier(applyOnNodeSync, option);

    EXPECT_EQ(option.options.opacity, 0.5f);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage004
 * @tc.desc: Test UpdateDragPreviewOptionsFromModifier with default opacity
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage004, TestSize.Level1)
{
    auto applyOnNodeSync = [](WeakPtr<FrameNode> frameNode) {
        auto node = frameNode.Upgrade();
        CHECK_NULL_VOID(node);
        node->GetRenderContext()->UpdateOpacity(10.0f); // Invalid value
    };

    DragPreviewOption option;
    DragDropFuncWrapper::UpdateDragPreviewOptionsFromModifier(applyOnNodeSync, option);

    EXPECT_EQ(option.options.opacity, 0.95f); // Default opacity
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage005
 * @tc.desc: Test UpdatePreviewOptionDefaultAttr without default shadow and radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage005, TestSize.Level1)
{
    DragPreviewOption option;
    option.isDefaultShadowEnabled = false;
    option.isDefaultRadiusEnabled = false;

    DragDropFuncWrapper::UpdatePreviewOptionDefaultAttr(option);

    EXPECT_EQ(option.options.opacity, 0.95f);
    EXPECT_FALSE(option.options.shadow.has_value());
    EXPECT_FALSE(option.options.borderRadius.has_value());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage006
 * @tc.desc: Test PrepareRadiusParametersForDragData with empty radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage006, TestSize.Level1)
{
    DragPreviewOption option;
    option.options.borderRadius = std::nullopt;

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::PrepareRadiusParametersForDragData(arkExtraInfoJson, option);

    // No assertion needed as it should safely exit with CHECK_NULL_VOID
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage007
 * @tc.desc: Test PrepareShadowParametersForDragData with empty shadow
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage007, TestSize.Level1)
{
    DragPreviewOption option;
    option.options.shadow = std::nullopt;

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::PrepareShadowParametersForDragData(arkExtraInfoJson, option);

    EXPECT_FALSE(arkExtraInfoJson->GetBool("shadow_enable"));
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage008
 * @tc.desc: Test GetDefaultShadow with invalid pipeline context
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage008, TestSize.Level1)
{
    auto shadow = DragDropFuncWrapper::GetDefaultShadow();
    EXPECT_FALSE(shadow.has_value());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage009
 * @tc.desc: Test RadiusToSigma with valid radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage009, TestSize.Level1)
{
    float radius = 5.0f;
    float sigma = DragDropFuncWrapper::RadiusToSigma(radius);
    EXPECT_GT(sigma, 0.0f);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage010
 * @tc.desc: Test RadiusToSigma with zero radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage010, TestSize.Level1)
{
    float radius = 0.0f;
    float sigma = DragDropFuncWrapper::RadiusToSigma(radius);
    EXPECT_EQ(sigma, 0.0f);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage011
 * @tc.desc: Test BlurStyleToEffection with invalid pipeline context
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage011, TestSize.Level1)
{
    BlurStyleOption blurStyleOp;
    blurStyleOp.blurStyle = BlurStyle::BACKGROUND_THICK;
    blurStyleOp.scale = 0.5;
    blurStyleOp.colorMode = ThemeColorMode::LIGHT;

    auto effection = DragDropFuncWrapper::BlurStyleToEffection(blurStyleOp);
    EXPECT_FALSE(effection.has_value());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage012
 * @tc.desc: Test BlurStyleToEffection with invalid blurStyleOp
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage012, TestSize.Level1)
{
    std::optional<BlurStyleOption> blurStyleOp = std::nullopt;

    auto effection = DragDropFuncWrapper::BlurStyleToEffection(blurStyleOp);
    EXPECT_FALSE(effection.has_value());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage013
 * @tc.desc: Test UpdatePreviewOptionDefaultAttr with default shadow and radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage013, TestSize.Level1)
{
    DragPreviewOption option;
    option.isDefaultShadowEnabled = true;
    option.isDefaultRadiusEnabled = true;

    DragDropFuncWrapper::UpdatePreviewOptionDefaultAttr(option);

    EXPECT_EQ(option.options.opacity, 0.95f);
    EXPECT_EQ(option.options.shadow, DragDropFuncWrapper::GetDefaultShadow());
    EXPECT_EQ(option.options.borderRadius, DragDropFuncWrapper::GetDefaultBorderRadius());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage014
 * @tc.desc: Test UpdateExtraInfo with blur background effect
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage014, TestSize.Level1)
{
    DragPreviewOption option;
    option.options.opacity = 0.8f;
    option.options.blurbgEffect.backGroundEffect = EffectOption();

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::UpdateExtraInfo(arkExtraInfoJson, option);

    EXPECT_EQ(arkExtraInfoJson->GetDouble("dip_opacity"), 0);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage015
 * @tc.desc: Test PrepareRadiusParametersForDragData with valid radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage015, TestSize.Level1)
{
    DragPreviewOption option;
    option.options.borderRadius->radiusTopLeft = 12.0_vp;
    option.options.borderRadius->radiusTopRight = 12.0_vp;
    option.options.borderRadius->radiusBottomRight = 12.0_vp;
    option.options.borderRadius->radiusBottomLeft = 12.0_vp;

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::PrepareRadiusParametersForDragData(arkExtraInfoJson, option);

    EXPECT_EQ(arkExtraInfoJson->GetDouble("drag_corner_radius1"), 0);
    EXPECT_EQ(arkExtraInfoJson->GetDouble("drag_corner_radius2"), 0);
    EXPECT_EQ(arkExtraInfoJson->GetDouble("drag_corner_radius3"), 0);
    EXPECT_EQ(arkExtraInfoJson->GetDouble("drag_corner_radius4"), 0);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage016
 * @tc.desc: Test PrepareShadowParametersForDragData with valid shadow
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage016, TestSize.Level1)
{
    DragPreviewOption option;
    Shadow shadow;
    shadow.SetIsFilled(true);
    option.options.shadow = shadow;

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::PrepareShadowParametersForDragData(arkExtraInfoJson, option);

    EXPECT_FALSE(arkExtraInfoJson->GetBool("shadow_enable"));
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage017
 * @tc.desc: Test ParseShadowInfo with valid shadow
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage017, TestSize.Level1)
{
    Shadow shadow;
    shadow.SetIsFilled(true);
    shadow.SetOffset(Offset(5, 5));
    shadow.SetBlurRadius(10.0);
    shadow.SetColor(Color::FromARGB(255, 255, 0, 0));

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::ParseShadowInfo(shadow, arkExtraInfoJson);

    EXPECT_FALSE(arkExtraInfoJson->GetBool("shadow_is_filled"));
    EXPECT_EQ(arkExtraInfoJson->GetDouble("drag_shadow_OffsetX"), 0);
    EXPECT_EQ(arkExtraInfoJson->GetDouble("drag_shadow_OffsetY"), 0);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage018
 * @tc.desc: Test GetDefaultShadow with valid pipeline context
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage018, TestSize.Level1)
{
    auto shadow = DragDropFuncWrapper::GetDefaultShadow();
    EXPECT_FALSE(shadow.has_value());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage019
 * @tc.desc: Test BlurStyleToEffection with valid BlurStyleOption
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage019, TestSize.Level1)
{
    BlurStyleOption blurStyleOp;
    blurStyleOp.blurStyle = BlurStyle::BACKGROUND_THICK;
    blurStyleOp.scale = 0.5;
    blurStyleOp.colorMode = ThemeColorMode::LIGHT;

    auto effection = DragDropFuncWrapper::BlurStyleToEffection(blurStyleOp);
    EXPECT_FALSE(effection.has_value());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage020
 * @tc.desc: Test DecideWhetherToStopDragging
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage020, TestSize.Level1)
{
    int32_t instanceId = -1;
    int32_t globalX = -1;
    int32_t globalY = -1;
    std::string extraParams;
    int32_t pointerId = -1;

    ContainerScope scope(instanceId);
    auto container = Container::CurrentSafely();
    CHECK_NULL_VOID(container);
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);

    taskExecutor->PostTask(
    [instanceId, globalX, globalY, extraParams, pointerId, context = pipelineContext]() {
        context->OnDragEvent({globalX, globalY }, DragEventAction::DRAG_EVENT_START_FOR_CONTROLLER);
        NG::DragDropFuncWrapper::DecideWhetherToStopDragging(
            { globalX, globalY }, extraParams, pointerId, instanceId);
    },
    TaskExecutor::TaskType::UI, "ArkUIDragHandleDragEventStart");
    EXPECT_EQ(instanceId, -1);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage022
 * @tc.desc: Test PrepareShadowParametersForDragData with Invalid shadow
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage022, TestSize.Level1)
{
    DragPreviewOption option;
    Shadow shadow;
    shadow.SetIsFilled(true);
    shadow.SetOffset(Offset(5, 5));
    shadow.SetBlurRadius(10.0);
    shadow.SetColor(Color::FromARGB(255, 255, 0, 0));
    option.options.shadow = shadow;

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::PrepareShadowParametersForDragData(arkExtraInfoJson, option);

    EXPECT_TRUE(arkExtraInfoJson->GetValue("shadow_enable"));
}


/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage023
 * @tc.desc: Test PrepareShadowParametersForDragData with empty shadow
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create frameNode.
     */
    auto frameNode = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    /**
     * @tc.steps: step2. Test GetDefaultBorderRadius
     */
    NG::DragPreviewOption dragPreviewOptions { false, false, false, false, true };
    dragPreviewOptions.options.borderRadius = DragDropFuncWrapper::GetDefaultBorderRadius();
    frameNode->SetDragPreviewOptions(dragPreviewOptions);
    auto dragPreviewOption = frameNode->GetDragPreviewOption();
    auto borderRadius = dragPreviewOption.options.borderRadius;
    EXPECT_EQ(borderRadius.value().radiusTopLeft.value().Value(), 12.0);
    EXPECT_EQ(borderRadius.value().radiusTopRight.value().Value(), 12.0);
    EXPECT_EQ(borderRadius.value().radiusBottomRight.value().Value(), 12.0);
    EXPECT_EQ(borderRadius.value().radiusBottomLeft.value().Value(), 12.0);
    /**
     * @tc.steps: step3. Test PrepareRadiusParametersForDragData
     */
    auto arkExtraInfoJson = JsonUtil::Create(true);
    DragDropFuncWrapper::PrepareRadiusParametersForDragData(arkExtraInfoJson, dragPreviewOption);
    auto radiusTopLeft = arkExtraInfoJson->GetDouble("drag_corner_radius1", -1);
    auto radiusTopRight = arkExtraInfoJson->GetDouble("drag_corner_radius2", -1);
    auto radiusBottomRight = arkExtraInfoJson->GetDouble("drag_corner_radius3", -1);
    auto radiusBottomLeft = arkExtraInfoJson->GetDouble("drag_corner_radius4", -1);
    EXPECT_EQ(radiusTopLeft, 12.0);
    EXPECT_EQ(radiusTopRight, 12.0);
    EXPECT_EQ(radiusBottomRight, 12.0);
    EXPECT_EQ(radiusBottomLeft, 12.0);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage024
 * @tc.desc: Test PrepareRadiusParametersForDragData with valid radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create frameNode.
     */
    auto frameNode = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    /**
     * @tc.steps: step2. Test GetDefaultBorderRadius
     */
    NG::DragPreviewOption dragPreviewOptions { false, false, false, false, true };
    dragPreviewOptions.options.borderRadius = DragDropFuncWrapper::GetDefaultBorderRadius();
    frameNode->SetDragPreviewOptions(dragPreviewOptions);
    auto dragPreviewOption = frameNode->GetDragPreviewOption();
    dragPreviewOption.options.borderRadius->radiusTopLeft = std::nullopt;
    dragPreviewOption.options.borderRadius->radiusTopRight = std::nullopt;
    dragPreviewOption.options.borderRadius->radiusBottomRight = std::nullopt;
    dragPreviewOption.options.borderRadius->radiusBottomLeft = std::nullopt;
    /**
     * @tc.steps: step3. Test PrepareRadiusParametersForDragData
     */
    auto arkExtraInfoJson = JsonUtil::Create(true);
    DragDropFuncWrapper::PrepareRadiusParametersForDragData(arkExtraInfoJson, dragPreviewOption);
    auto radiusTopLeft = arkExtraInfoJson->GetDouble("drag_corner_radius1", -1);
    auto radiusTopRight = arkExtraInfoJson->GetDouble("drag_corner_radius2", -1);
    auto radiusBottomRight = arkExtraInfoJson->GetDouble("drag_corner_radius3", -1);
    auto radiusBottomLeft = arkExtraInfoJson->GetDouble("drag_corner_radius4", -1);
    EXPECT_EQ(radiusTopLeft, -1);
    EXPECT_EQ(radiusTopRight, -1);
    EXPECT_EQ(radiusBottomRight, -1);
    EXPECT_EQ(radiusBottomLeft, -1);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage026
 * @tc.desc: Test UpdateExtraInfo with invalid radius
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage026, TestSize.Level1)
{
    DragPreviewOption option;
    option.options.opacity = 0.8f;
    option.options.blurbgEffect.backGroundEffect = EffectOption();
    option.options.blurbgEffect.backGroundEffect.radius.SetValue(1.0F);

    auto arkExtraInfoJson = std::make_unique<JsonValue>();
    DragDropFuncWrapper::UpdateExtraInfo(arkExtraInfoJson, option);

    EXPECT_EQ(arkExtraInfoJson->GetDouble("dip_opacity"), 0);
}
/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage027
 * @tc.desc: Test UpdateDragPreviewOptionsFromModifier with BackgroundEffect values
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage027, TestSize.Level1)
{
    auto applyOnNodeSync = [](WeakPtr<FrameNode> frameNode) {
        auto node = frameNode.Upgrade();
        CHECK_NULL_VOID(node);
        CalcDimension radius;
        radius.SetValue(80.0f);
        Color color = Color::FromARGB(13, 255, 255, 255);
        EffectOption effoption = { radius, 1.0, 1.08, color };
        node->GetRenderContext()->UpdateBackgroundEffect(effoption);
    };

    DragPreviewOption option;
    DragDropFuncWrapper::UpdateDragPreviewOptionsFromModifier(applyOnNodeSync, option);

    EXPECT_TRUE(option.options.blurbgEffect.backGroundEffect.radius.IsValid());
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage028
 * @tc.desc: Test UpdateDragPreviewOptionsFromModifier with  BackShadow ,BorderRadius,without,bgEffect,with
 * BackBlurStyle ;
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage028, TestSize.Level1)
{
    auto applyOnNodeSync = [](WeakPtr<FrameNode> frameNode) {
        auto node = frameNode.Upgrade();
        CHECK_NULL_VOID(node);
        Shadow shadow;
        shadow.SetIsFilled(true);
        shadow.SetOffset(Offset(5, 5));
        shadow.SetBlurRadius(10.0);
        shadow.SetColor(Color::FromARGB(255, 255, 0, 0));
        BlurStyleOption styleOption;
        styleOption.blurStyle = BlurStyle::COMPONENT_THICK;
        styleOption.scale = 0.5;
        styleOption.colorMode = ThemeColorMode::LIGHT;
        BorderRadiusProperty borderRadius;
        borderRadius.SetRadius(Dimension(50.0));
        node->GetRenderContext()->UpdateBorderRadius(borderRadius);
        node->GetRenderContext()->UpdateBackShadow(shadow);
        node->GetRenderContext()->UpdateBackBlurStyle(styleOption);
        node->GetRenderContext()->UpdateBackgroundEffect(std::nullopt);


    };

    DragPreviewOption option;
    DragDropFuncWrapper::UpdateDragPreviewOptionsFromModifier(applyOnNodeSync, option);

    EXPECT_TRUE(option.options.shadow.has_value());
    EXPECT_TRUE(option.options.borderRadius.has_value());
    EXPECT_FALSE(option.options.blurbgEffect.backGroundEffect.radius.IsValid());
}

/**
 * @tc.name: TestIsSelectedItemNode
 * @tc.desc: Test IsSelectedItemNode func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, TestIsSelectedItemNode, TestSize.Level1)
{
    int32_t caseNum = 0;
    for (const auto& testCase : DRAG_MULTI_CASES) {
        /**
         * @tc.steps: step1. Create grid with gridItem frame node tree.
         * @tc.expected: instance is not null.
         */
        auto gridNode = ProcessDragItemGroupScene();
        ASSERT_NE(gridNode, nullptr);
        auto gridItem = AceType::DynamicCast<FrameNode>(gridNode->GetChildByIndex(0));
        ASSERT_NE(gridItem, nullptr);
        auto gridItemPattern = gridItem->GetPattern<GridItemPattern>();
        ASSERT_NE(gridItemPattern, nullptr);

        /**
         * @tc.steps: step2. Set GridItemNode selected
         */
        gridItemPattern->SetSelected(testCase.isSelected);

        /**
         * @tc.steps: step3. Set GridItemNode isMultiSelectionEnabled
         */
        auto dragPreviewOption = gridItem->GetDragPreviewOption();
        dragPreviewOption.isMultiSelectionEnabled = testCase.isMultiSelectionEnabled;
        gridItem->SetDragPreviewOptions(dragPreviewOption);

        /**
         * @tc.steps: step4. Set GridItemNode isAllowDrag
         */
        if (testCase.isAllowDrag) {
            SetFrameNodeAllowDrag(gridItem);
        }
        EXPECT_TRUE(IsDragEventStateEqual(caseNum, DragDropFuncWrapper::IsSelectedItemNode(gridItem),
            testCase.expectResult));
        caseNum++;
    }
}

/**
 * @tc.name: Test IsBelongToMultiItemNode
 * @tc.desc: Test IsBelongToMultiItemNode func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, TestIsBelongToMultiItemNode, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create grid with gridItem frame node tree.
     * @tc.expected: instance is not null.
     */
    auto gridNode = ProcessDragItemGroupScene();
    ASSERT_NE(gridNode, nullptr);
    auto gridItem = AceType::DynamicCast<FrameNode>(gridNode->GetChildByIndex(0));
    ASSERT_NE(gridItem, nullptr);

    /**
     * @tc.steps: step2. Create frameNode.
     * @tc.expected: IsBelongToMultiItemNode return false.
     */
    auto frameNode = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MountToParent(gridItem);
    EXPECT_FALSE(DragDropFuncWrapper::IsBelongToMultiItemNode(frameNode));

    /**
     * @tc.steps: step3. Set frameNode is selected and allowDrag.
     * @tc.expected: IsBelongToMultiItemNode return true.
     */
    auto gridItemPattern = gridItem->GetPattern<GridItemPattern>();
    ASSERT_NE(gridItemPattern, nullptr);
    gridItemPattern->SetSelected(true);
    auto dragPreviewOption = gridItem->GetDragPreviewOption();
    dragPreviewOption.isMultiSelectionEnabled = true;
    gridItem->SetDragPreviewOptions(dragPreviewOption);
    SetFrameNodeAllowDrag(gridItem);

    EXPECT_TRUE(DragDropFuncWrapper::IsBelongToMultiItemNode(frameNode));
}

/**
 * @tc.name: Test CheckIsNeedGather
 * @tc.desc: Test CheckIsNeedGather func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, CheckIsNeedGather, TestSize.Level1)
{
    int32_t caseNum = 0;
    for (const auto& testCase : DRAG_MULTI_CASES) {
        /**
         * @tc.steps: step1. Create grid with gridItem frame node tree.
         * @tc.expected: instance is not null.
         */
        auto gridNode = ProcessDragItemGroupScene();
        ASSERT_NE(gridNode, nullptr);
        auto gridItem = AceType::DynamicCast<FrameNode>(gridNode->GetChildByIndex(0));
        ASSERT_NE(gridItem, nullptr);
        auto gridItemPattern = gridItem->GetPattern<GridItemPattern>();
        ASSERT_NE(gridItemPattern, nullptr);

        /**
         * @tc.steps: step2. Set GridItemNode selected
         */
        gridItemPattern->SetSelected(testCase.isSelected);

        /**
         * @tc.steps: step3. Set GridItemNode isMultiSelectionEnabled
         */
        auto dragPreviewOption = gridItem->GetDragPreviewOption();
        dragPreviewOption.isMultiSelectionEnabled = testCase.isMultiSelectionEnabled;
        gridItem->SetDragPreviewOptions(dragPreviewOption);

        /**
         * @tc.steps: step4. Set GridItemNode isAllowDrag
         */
        if (testCase.isAllowDrag) {
            SetFrameNodeAllowDrag(gridItem);
        }
        EXPECT_TRUE(IsDragEventStateEqual(caseNum, DragDropFuncWrapper::CheckIsNeedGather(gridItem),
            testCase.expectResult));
        caseNum++;
    }
}

/**
 * @tc.name: Test FindItemParentNode
 * @tc.desc: Test FindItemParentNode func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, FindItemParentNode, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create grid with gridItem frame node tree.
     * @tc.expected: instance is not null.
     * @tc.expected: FindItemParentNode return false.
     */
    auto gridNode = ProcessDragItemGroupScene();
    ASSERT_NE(gridNode, nullptr);
    auto gridItem = AceType::DynamicCast<FrameNode>(gridNode->GetChildByIndex(0));
    ASSERT_NE(gridItem, nullptr);

    /**
     * @tc.steps: step2.run FindItemParentNode func
     * @tc.expected: FindItemParentNode return gridNode.
     */
    SetFrameNodeAllowDrag(gridItem);
    EXPECT_EQ(DragDropFuncWrapper::FindItemParentNode(gridItem), gridNode);
    
    /**
     * @tc.steps: step3.test other class frameNode
     * @tc.expected: FindItemParentNode return nullptr.
     */
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() {return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNode, nullptr);
    frameNode->MountToParent(gridNode);
    EXPECT_EQ(DragDropFuncWrapper::FindItemParentNode(frameNode), nullptr);
}

/**
 * @tc.name: Test RequestDragEndPending
 * @tc.desc: Test RequestDragEndPending func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, RequestDragEndPending, TestSize.Level1)
{
    int32_t requestId = DragDropFuncWrapper::RequestDragEndPending();
    EXPECT_EQ(requestId, -1);

    DragDropGlobalController::GetInstance().SetIsOnOnDropPhase(true);
    requestId = DragDropFuncWrapper::RequestDragEndPending();
    EXPECT_NE(requestId, -1);

    DragDropGlobalController::GetInstance().SetIsOnOnDropPhase(false);
    requestId = DragDropFuncWrapper::RequestDragEndPending();
    EXPECT_EQ(requestId, -1);
}

/**
 * @tc.name: Test NotifyDragResult
 * @tc.desc: Test NotifyDragResult func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, NotifyDragResult, TestSize.Level1)
{
    int32_t requestId = 1;
    DragDropGlobalController::GetInstance().requestId_ = 0;
    int32_t ret = DragDropFuncWrapper::NotifyDragResult(requestId, static_cast<int32_t>(DragRet::DRAG_SUCCESS));
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(DragDropGlobalController::GetInstance().dragResult_, DragRet::DRAG_FAIL);

    DragDropGlobalController::GetInstance().SetIsOnOnDropPhase(true);
    ret = DragDropFuncWrapper::NotifyDragResult(requestId, static_cast<int32_t>(DragRet::DRAG_SUCCESS));
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(DragDropGlobalController::GetInstance().dragResult_, DragRet::DRAG_FAIL);

    DragDropGlobalController::GetInstance().requestId_ = requestId;
    ret = DragDropFuncWrapper::NotifyDragResult(requestId, static_cast<int32_t>(DragRet::DRAG_SUCCESS));
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(DragDropGlobalController::GetInstance().dragResult_, DragRet::DRAG_SUCCESS);
    DragDropGlobalController::GetInstance().SetIsOnOnDropPhase(false);
}

/**
 * @tc.name: Test NotifyDragEndPendingDone
 * @tc.desc: Test NotifyDragEndPendingDone func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, NotifyDragEndPendingDone, TestSize.Level1)
{
    int32_t requestId = 1;
    DragDropGlobalController::GetInstance().requestId_ = 0;
    DragDropGlobalController::GetInstance().dragResult_ = DragRet::DRAG_SUCCESS;
    int32_t ret = DragDropFuncWrapper::NotifyDragEndPendingDone(requestId);
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(DragDropGlobalController::GetInstance().dragResult_, DragRet::DRAG_SUCCESS);

    DragDropGlobalController::GetInstance().SetIsOnOnDropPhase(true);
    ret = DragDropFuncWrapper::NotifyDragEndPendingDone(requestId);
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(DragDropGlobalController::GetInstance().dragResult_, DragRet::DRAG_SUCCESS);

    DragDropGlobalController::GetInstance().requestId_ = requestId;
    ret = DragDropFuncWrapper::NotifyDragEndPendingDone(requestId);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(DragDropGlobalController::GetInstance().dragResult_, DragRet::DRAG_FAIL);
    DragDropGlobalController::GetInstance().SetIsOnOnDropPhase(false);
}

/**
 * @tc.name: Test GetAnonyString
 * @tc.desc: Test GetAnonyString func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, GetAnonyString, TestSize.Level1)
{
    std::string fullString = "";
    std::string anonyStr = "";
    std::string ret = NG::DragDropFuncWrapper::GetAnonyString(fullString);
    EXPECT_EQ(ret, anonyStr);

    fullString = "test";
    anonyStr = "t******t";
    ret = NG::DragDropFuncWrapper::GetAnonyString(fullString);
    EXPECT_EQ(ret, anonyStr);

    fullString = "testFullString";
    anonyStr = "test******ring";
    ret = NG::DragDropFuncWrapper::GetAnonyString(fullString);
    EXPECT_EQ(ret, anonyStr);
}

/**
 * @tc.name: Test GetPointerEventAction
 * @tc.desc: Test GetPointerEventAction func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, GetPointerEventAction, TestSize.Level1)
{
    TouchEvent touchPoint;
    DragPointerEvent event;

    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::UNKNOWN);

    touchPoint.type = TouchType::CANCEL;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::CANCEL);

    touchPoint.type = TouchType::DOWN;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::DOWN);

    touchPoint.type = TouchType::MOVE;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::MOVE);

    touchPoint.type = TouchType::UP;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::UP);

    touchPoint.type = TouchType::PULL_MOVE;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::PULL_MOVE);

    touchPoint.type = TouchType::PULL_UP;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::PULL_UP);

    touchPoint.type = TouchType::PULL_IN_WINDOW;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::PULL_IN_WINDOW);

    touchPoint.type = TouchType::PULL_OUT_WINDOW;
    NG::DragDropFuncWrapper::GetPointerEventAction(touchPoint, event);
    EXPECT_EQ(event.action, PointerAction::PULL_OUT_WINDOW);
}

/**
 * @tc.name: Test GetFrameNodeByInspectorId
 * @tc.desc: Test GetFrameNodeByInspectorId func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, GetFrameNodeByInspectorId, TestSize.Level1)
{
    std::string inspectorId = "";
    RefPtr<FrameNode> ret = NG::DragDropFuncWrapper::GetFrameNodeByInspectorId(inspectorId);
    EXPECT_EQ(ret, nullptr);

    inspectorId = "test";
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    auto frameNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    frameNode->UpdateInspectorId(inspectorId);
    auto context = PipelineContext::GetCurrentContext();
    context->rootNode_ = rootNode;
    rootNode->AddChild(frameNode);
    ret = NG::DragDropFuncWrapper::GetFrameNodeByInspectorId(inspectorId);
    EXPECT_EQ(ret, frameNode);

    frameNode->GetLayoutProperty()->UpdateVisibility(VisibleType::GONE);
    ret = NG::DragDropFuncWrapper::GetFrameNodeByInspectorId(inspectorId);
    EXPECT_EQ(ret, nullptr);

    frameNode->GetLayoutProperty()->UpdateVisibility(VisibleType::INVISIBLE);
    ret = NG::DragDropFuncWrapper::GetFrameNodeByInspectorId(inspectorId);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: Test GetPointRelativeToMainWindow
 * @tc.desc: Test GetPointRelativeToMainWindow func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, GetPointRelativeToMainWindow, TestSize.Level1)
{
    Point point = { 0, 0 };
    OffsetF ret = NG::DragDropFuncWrapper::GetPointRelativeToMainWindow(point);
    EXPECT_EQ(ret.GetX(), point.GetX());
    EXPECT_EQ(ret.GetY(), point.GetY());
}

/**
 * @tc.name: Test GetFrameNodeByKey
 * @tc.desc: Test GetFrameNodeByKey func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, GetFrameNodeByKey, TestSize.Level1)
{
    auto frameNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    std::string key = "";
    RefPtr<FrameNode> ret = NG::DragDropFuncWrapper::GetFrameNodeByKey(frameNode, key);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.name: StartDragAction
 * @tc.desc: Test StartDragAction func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, StartDragAction, TestSize.Level1)
{
    auto dragAction = std::make_shared<OHOS::Ace::NG::ArkUIInteralDragAction>();
    auto container = Container::Current();
    AceEngine& aceEngine = AceEngine::Get();
    aceEngine.AddContainer(0, container);
    dragAction->instanceId = 0;
    int32_t ret = DragDropFuncWrapper::StartDragAction(dragAction);
    EXPECT_EQ(ret, -1);

    MockContainer::Current()->pipelineContext_ = MockPipelineContext::GetCurrentContext();
    aceEngine.AddContainer(1, MockContainer::container_);
    dragAction->instanceId = 1;
    ret = DragDropFuncWrapper::StartDragAction(dragAction);
    EXPECT_EQ(ret, -1);

    dragAction->dragState = DragAdapterState::SENDING;
    ret = DragDropFuncWrapper::StartDragAction(dragAction);
    EXPECT_EQ(ret, -1);

    MockContainer::Current()->pipelineContext_->SetIsDragging(true);
    ret = DragDropFuncWrapper::StartDragAction(dragAction);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: HandleOnDragEvent
 * @tc.desc: Test HandleOnDragEvent func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, HandleOnDragEvent, TestSize.Level1)
{
    auto dragAction = std::make_shared<OHOS::Ace::NG::ArkUIInteralDragAction>();
    MockContainer::Current()->pipelineContext_ = MockPipelineContext::GetCurrentContext();
    AceEngine& aceEngine = AceEngine::Get();
    aceEngine.AddContainer(0, MockContainer::container_);
    dragAction->instanceId = 0;
    dragAction->dragState = DragAdapterState::INIT;
    DragDropFuncWrapper::HandleOnDragEvent(dragAction);
    EXPECT_EQ(dragAction->dragState, DragAdapterState::INIT);

    dragAction->dragState = DragAdapterState::SENDING;
    DragDropFuncWrapper::HandleOnDragEvent(dragAction);
    EXPECT_EQ(dragAction->dragState, DragAdapterState::SUCCESS);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage029
 * @tc.desc: Test GetPaintRectCenter Func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage029, TestSize.Level1)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNode, nullptr);
    frameNode->parent_ = nullptr;
    auto actualResults = DragDropFuncWrapper::GetPaintRectCenter(frameNode, false);
    auto context = frameNode->GetRenderContext();
    ASSERT_NE(context, nullptr);
    auto paintRect = context->GetPaintRectWithoutTransform();
    auto offset = paintRect.GetOffset();
    PointF pointNode(offset.GetX() + paintRect.Width() / 2.0f, offset.GetY() + paintRect.Height() / 2.0f);
    auto expectResult = OffsetF(pointNode.GetX(), pointNode.GetY());
    EXPECT_EQ(expectResult, actualResults);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage030
 * @tc.desc: Test GetPaintRectCenter Func When checkWindowBoundary Is False IsWindowBoundary() Is True
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage030, TestSize.Level1)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    auto frameNodeCopy = FrameNode::GetOrCreateFrameNode(V2::NAVDESTINATION_TITLE_BAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNode, nullptr);
    ASSERT_NE(frameNodeCopy, nullptr);
    frameNodeCopy->SetWindowBoundary(true);
    frameNode->parent_ = frameNodeCopy;
    DragDropFuncWrapper::GetPaintRectCenter(frameNode, true);
    auto actualResults = DragDropFuncWrapper::GetPaintRectCenter(frameNode, false);
    auto context = frameNode->GetRenderContext();
    ASSERT_NE(context, nullptr);
    auto paintRect = context->GetPaintRectWithoutTransform();
    auto offset = paintRect.GetOffset();
    PointF pointNode(offset.GetX() + paintRect.Width() / 2.0f, offset.GetY() + paintRect.Height() / 2.0f);
    auto expectResult = OffsetF(pointNode.GetX(), pointNode.GetY());
    EXPECT_EQ(expectResult, actualResults);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage031
 * @tc.desc: Test GetPaintRectCenter Func When checkWindowBoundary Is True IsWindowBoundary() Is False
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage031, TestSize.Level1)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    auto frameNodeCopy = FrameNode::GetOrCreateFrameNode(V2::NAVDESTINATION_TITLE_BAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNode, nullptr);
    ASSERT_NE(frameNodeCopy, nullptr);
    frameNodeCopy->SetWindowBoundary(false);
    frameNode->parent_ = frameNodeCopy;
    DragDropFuncWrapper::GetPaintRectCenter(frameNode, true);
    auto actualResults = DragDropFuncWrapper::GetPaintRectCenter(frameNode, false);
    auto context = frameNode->GetRenderContext();
    ASSERT_NE(context, nullptr);
    auto paintRect = context->GetPaintRectWithoutTransform();
    auto offset = paintRect.GetOffset();
    PointF pointNode(offset.GetX() + paintRect.Width() / 2.0f, offset.GetY() + paintRect.Height() / 2.0f);
    auto expectResult = OffsetF(pointNode.GetX(), pointNode.GetY());
    EXPECT_EQ(expectResult, actualResults);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage032
 * @tc.desc: Test GetPaintRectCenter Func When checkWindowBoundary Is True IsWindowBoundary() Is True
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage032, TestSize.Level1)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    auto frameNodeCopy = FrameNode::GetOrCreateFrameNode(V2::NAVDESTINATION_TITLE_BAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNode, nullptr);
    ASSERT_NE(frameNodeCopy, nullptr);
    frameNodeCopy->SetWindowBoundary(true);
    frameNode->parent_ = frameNodeCopy;
    DragDropFuncWrapper::GetPaintRectCenter(frameNode, false);
    auto actualResults = DragDropFuncWrapper::GetPaintRectCenter(frameNode, false);
    auto context = frameNode->GetRenderContext();
    ASSERT_NE(context, nullptr);
    auto paintRect = context->GetPaintRectWithoutTransform();
    auto offset = paintRect.GetOffset();
    PointF pointNode(offset.GetX() + paintRect.Width() / 2.0f, offset.GetY() + paintRect.Height() / 2.0f);
    auto expectResult = OffsetF(pointNode.GetX(), pointNode.GetY());
    EXPECT_EQ(expectResult, actualResults);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage033
 * @tc.desc: Test GetPaintRectCenter Func When checkWindowBoundary Is True IsWindowBoundary() Is True
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage033, TestSize.Level1)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    auto frameNodeCopy = FrameNode::GetOrCreateFrameNode(V2::NAVDESTINATION_TITLE_BAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNode, nullptr);
    ASSERT_NE(frameNodeCopy, nullptr);
    frameNodeCopy->SetWindowBoundary(false);
    frameNode->parent_ = frameNodeCopy;
    DragDropFuncWrapper::GetPaintRectCenter(frameNode, false);
    auto actualResults = DragDropFuncWrapper::GetPaintRectCenter(frameNode, false);
    auto context = frameNode->GetRenderContext();
    ASSERT_NE(context, nullptr);
    auto paintRect = context->GetPaintRectWithoutTransform();
    auto offset = paintRect.GetOffset();
    PointF pointNode(offset.GetX() + paintRect.Width() / 2.0f, offset.GetY() + paintRect.Height() / 2.0f);
    auto expectResult = OffsetF(pointNode.GetX(), pointNode.GetY());
    EXPECT_EQ(expectResult, actualResults);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage034
 * @tc.desc: Test IsExpandDisplay func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage034, TestSize.Level1)
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    ASSERT_NE(themeManager, nullptr);
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    auto selectTheme = AceType::MakeRefPtr<SelectTheme>();
    ASSERT_NE(selectTheme, nullptr);
    selectTheme->expandDisplay_ = true;
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(selectTheme));
    auto result = DragDropFuncWrapper::IsExpandDisplay(MockPipelineContext::GetCurrent());
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage035
 * @tc.desc: Test GetPaintRectCenter func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage035, TestSize.Level1)
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    ASSERT_NE(themeManager, nullptr);
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    auto selectTheme = AceType::MakeRefPtr<SelectTheme>();
    ASSERT_NE(selectTheme, nullptr);
    selectTheme->expandDisplay_ = false;
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(selectTheme));
    auto result = DragDropFuncWrapper::IsExpandDisplay(MockPipelineContext::GetCurrent());
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage036
 * @tc.desc: Test GetCurrentWindowOffset Func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage036, TestSize.Level1)
{
    auto context = PipelineBase::GetCurrentContext();
    ASSERT_NE(context, nullptr);
    auto expectResult = DragDropFuncWrapper::GetCurrentWindowOffset(context);
    EXPECT_EQ(OffsetF(), expectResult);
}

/**
 * @tc.name: DragDropFuncWrapperTestNgCoverage037
 * @tc.desc: Test GetCurrentWindowOffset func
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage037, TestSize.Level1)
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    ASSERT_NE(themeManager, nullptr);
    auto pipeline = MockPipelineContext::GetCurrent();
    ASSERT_NE(pipeline, nullptr);
    pipeline->SetThemeManager(themeManager);
    auto selectTheme = AceType::MakeRefPtr<SelectTheme>();
    ASSERT_NE(pipeline, nullptr);
    selectTheme->expandDisplay_ = true;
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(selectTheme));
    auto expectResult = DragDropFuncWrapper::GetCurrentWindowOffset(pipeline);
    EXPECT_EQ(expectResult, OffsetF());
}

/**
 * @tc.name: Test DragDropFuncWrapperTestNgCoverage038
 * @tc.desc: Test UpdateNodePositionToWindow func parentNode is  nullptr
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage038, TestSize.Level1)
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    auto offset = OffsetF(1.0f, 1.0f);
    RefPtr<FrameNode> parentNode = frameNode->GetAncestorNodeOfFrame(true);
    EXPECT_EQ(parentNode, nullptr);
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNode->renderContext_, nullptr);
    auto renderContext = frameNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
    frameNode->renderContext_->UpdatePaintRect(RectT(1.0f, 1.0f, 1.0f, 1.0f));
    DragDropFuncWrapper::UpdateNodePositionToWindow(frameNode, offset);
    EXPECT_EQ(renderContext->GetPosition(), OffsetT<Dimension>(Dimension(offset.GetX()), Dimension(offset.GetY())));
}

/**
 * @tc.name: Test DragDropFuncWrapperTestNgCoverage039
 * @tc.desc: Test UpdateNodePositionToWindow func parentNode is not nullptr
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage039, TestSize.Level1)
{
    auto offset = OffsetF(5.0f, 5.0f);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNode, nullptr);
    auto frameNodeCopy = FrameNode::GetOrCreateFrameNode(V2::NAVDESTINATION_TITLE_BAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(frameNodeCopy, nullptr);
    frameNode->parent_ = frameNodeCopy;
    ASSERT_NE(frameNode->parent_.Upgrade(), nullptr);
    frameNode->isWindowBoundary_ = false;
    RefPtr<FrameNode> parentNode = frameNode->GetAncestorNodeOfFrame(true);
    ASSERT_NE(parentNode, nullptr);
    auto renderContext = frameNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNode->renderContext_, nullptr);
    frameNode->renderContext_->UpdatePaintRect(RectT(3.0f, 2.0f, 3.0f, 4.0f));
    DragDropFuncWrapper::UpdateNodePositionToWindow(frameNode, offset);
    EXPECT_NE(renderContext->GetPosition(), OffsetT<Dimension>(Dimension(5.0f), Dimension(5.0f)));
}

/**
 * @tc.name: Test DragDropFuncWrapperTestNgCoverage040
 * @tc.desc: Test HandleBackPressHideMenu
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropFuncWrapperTestNgCoverage, DragDropFuncWrapperTestNgCoverage040, TestSize.Level1)
{
    auto mainPipeline = PipelineContext::GetMainPipelineContext();
    ASSERT_NE(mainPipeline, nullptr);
    auto dragDropManager = mainPipeline->GetDragDropManager();
    ASSERT_NE(dragDropManager, nullptr);
    dragDropManager->SetIsDragNodeNeedClean(false);
    auto overlayManager = mainPipeline->GetOverlayManager();
    ASSERT_NE(overlayManager, nullptr);
    auto gatherNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<Pattern>(); });
    ASSERT_NE(gatherNode, nullptr);
    std::vector<GatherNodeChildInfo> gatherNodeInfo;
    overlayManager->MountGatherNodeToRootNode(gatherNode, gatherNodeInfo);
    EXPECT_EQ(dragDropManager->IsDragNodeNeedClean(), false);

    auto container = MockContainer::Current();
    ASSERT_NE(container, nullptr);
    container->isUIExtensionWindow_ = false;
    DragDropFuncWrapper::HandleBackPressHideMenu();
    EXPECT_EQ(dragDropManager->IsDragNodeNeedClean(), true);
    EXPECT_EQ(overlayManager->hasGatherNode_, true);

    dragDropManager->SetIsDragNodeNeedClean(false);
    container->isUIExtensionWindow_ = true;
    DragDropFuncWrapper::HandleBackPressHideMenu();
    EXPECT_EQ(dragDropManager->IsDragNodeNeedClean(), true);
    EXPECT_EQ(overlayManager->hasGatherNode_, false);
}
} // namespace OHOS::Ace::NG

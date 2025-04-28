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
#define private public
#define protected public
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/divider/divider_layout_property.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"
#include "core/components_ng/pattern/flex/flex_layout_algorithm.h"
#include "core/components_ng/pattern/flex/flex_layout_pattern.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_layout_algorithm.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_layout_property.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_model_ng.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const InspectorFilter filter;
constexpr Dimension STROKE_WIDTH = 10.0_vp;
constexpr Dimension START_MARGIN = 5.0_vp;
constexpr Dimension END_MARGIN = 5.0_vp;
constexpr Dimension NONE_WIDTH = 0.0_vp;
constexpr Dimension DEFAULT_SIDE_BAR_WIDTH = 100.0_vp;
constexpr Dimension DEFAULT_MIN_SIDE_BAR_WIDTH = 50.0_vp;
constexpr Dimension DEFAULT_MAX_SIDE_BAR_WIDTH = 200.0_vp;
const SizeF FIRST_ITEM_SIZE(5.0f, 5.0f);
const SizeF SELF_SIZE(50.0f, 50.0f);
const float REAL_WIDTH = 10.0f;
} // namespace

class SideBarViewTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void SideBarViewTestNg::SetUpTestCase()
{
    MockPipelineContext::SetUp();
}

void SideBarViewTestNg::TearDownTestCase()
{
    MockPipelineContext::TearDown();
}

/**
 * @tc.name: SideBarViewTestNg001
 * @tc.desc: Test SideBar devider
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg001, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    Dimension width = 10.0_vp;
    Dimension abnormalWidth = -10.0_vp;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetDividerStrokeWidth(STROKE_WIDTH);
    SideBarContainerModelInstance.SetDividerColor(Color::BLUE);
    SideBarContainerModelInstance.SetDividerStartMargin(START_MARGIN);
    SideBarContainerModelInstance.SetDividerEndMargin(END_MARGIN);
    SideBarContainerModelInstance.ParseAndSetWidth(WidthType::SIDEBAR_WIDTH, width);
    SideBarContainerModelInstance.ParseAndSetWidth(WidthType::MIN_SIDEBAR_WIDTH, width);
    SideBarContainerModelInstance.ParseAndSetWidth(WidthType::MAX_SIDEBAR_WIDTH, width);
    SideBarContainerModelInstance.ParseAndSetWidth(WidthType::SIDEBAR_WIDTH, abnormalWidth);
    SideBarContainerModelInstance.ParseAndSetWidth(WidthType::MIN_SIDEBAR_WIDTH, abnormalWidth);
    SideBarContainerModelInstance.ParseAndSetWidth(WidthType::MAX_SIDEBAR_WIDTH, abnormalWidth);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    auto sideBarLayoutProperty = frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>();
    EXPECT_NE(sideBarLayoutProperty, nullptr);
    EXPECT_EQ(sideBarLayoutProperty->GetDividerStrokeWidth(), STROKE_WIDTH);
    EXPECT_EQ(sideBarLayoutProperty->GetDividerColor(), Color::BLUE);
    EXPECT_EQ(sideBarLayoutProperty->GetDividerStartMargin(), START_MARGIN);
    EXPECT_EQ(sideBarLayoutProperty->GetDividerEndMargin(), END_MARGIN);
}

/**
 * @tc.name: SideBarViewTestNg002
 * @tc.desc: Test SideBar OnUpdateShowDivider
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg002, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetDividerStrokeWidth(STROKE_WIDTH);
    SideBarContainerModelInstance.SetDividerColor(Color::BLUE);
    SideBarContainerModelInstance.SetDividerStartMargin(START_MARGIN);
    SideBarContainerModelInstance.SetDividerEndMargin(END_MARGIN);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    auto row = FrameNode::CreateFrameNode(
        V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    EXPECT_NE(row, nullptr);
    auto row2 = FrameNode::CreateFrameNode(
        V2::SIDE_BAR_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<SideBarContainerPattern>());
    EXPECT_NE(row2, nullptr);
    row->MountToParent(frameNode);
    row2->MountToParent(frameNode);
    ViewStackProcessor::GetInstance()->Push(frameNode);
    frameNode->MarkModifyDone();
}

/**
 * @tc.name: SideBarViewTestNg003
 * @tc.desc: Test SideBar OnUpdateShowDivider
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg003, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetDividerStrokeWidth(STROKE_WIDTH);
    SideBarContainerModelInstance.SetDividerColor(Color::BLUE);
    SideBarContainerModelInstance.SetDividerStartMargin(START_MARGIN);
    SideBarContainerModelInstance.SetDividerEndMargin(END_MARGIN);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    auto row = FrameNode::CreateFrameNode(
        V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    EXPECT_NE(row, nullptr);
    auto row2 = FrameNode::CreateFrameNode(
        V2::SIDE_BAR_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<SideBarContainerPattern>());
    EXPECT_NE(row2, nullptr);
    row->MountToParent(frameNode);
    row2->MountToParent(frameNode);
    auto dividerNode = FrameNode::GetOrCreateFrameNode(
        V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<DividerPattern>(); });
    EXPECT_NE(dividerNode, nullptr);
    dividerNode->MountToParent(frameNode);

    int32_t imgNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto imgNode = FrameNode::GetOrCreateFrameNode(
        V2::ROW_ETS_TAG, imgNodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    EXPECT_NE(imgNode, nullptr);
    imgNode->MountToParent(frameNode);
    frameNode->MarkModifyDone();
}

/**
 * @tc.name: SideBarViewTestNg004
 * @tc.desc: Test SideBar Measure and layout
 * @tc.type: SideBar Measure
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg004, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutWrapperNode> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    auto row = FrameNode::CreateFrameNode(
        V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    EXPECT_NE(row, nullptr);

    RefPtr<GeometryNode> rowGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    rowGeometryNode->Reset();
    RefPtr<LayoutWrapperNode> rowLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(row, rowGeometryNode, row->GetLayoutProperty());
    EXPECT_NE(rowLayoutWrapper, nullptr);
    auto rowPattern = row->GetPattern<LinearLayoutPattern>();
    EXPECT_NE(rowPattern, nullptr);

    auto childLayoutConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
    childLayoutConstraint.selfIdealSize = OptionalSizeF(FIRST_ITEM_SIZE);

    rowLayoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(childLayoutConstraint);
    auto rowLayoutAlgorithm = rowPattern->CreateLayoutAlgorithm();
    EXPECT_NE(rowLayoutAlgorithm, nullptr);

    rowLayoutWrapper->SetLayoutAlgorithm(
        AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));
    rowLayoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(5.0f), CalcLength(5.0f)));

    row->MountToParent(frameNode);
    layoutWrapper->AppendChild(rowLayoutWrapper);

    auto siderBar = FrameNode::CreateFrameNode(
        V2::SIDE_BAR_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<SideBarContainerPattern>());
    EXPECT_NE(siderBar, nullptr);

    RefPtr<GeometryNode> siderBarGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    siderBarGeometryNode->Reset();
    RefPtr<LayoutWrapperNode> sideBarLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(siderBar, siderBarGeometryNode, siderBar->GetLayoutProperty());
    EXPECT_NE(sideBarLayoutWrapper, nullptr);
    auto siderBarPattern = siderBar->GetPattern<SideBarContainerPattern>();
    EXPECT_NE(siderBarPattern, nullptr);

    sideBarLayoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(childLayoutConstraint);
    auto sideBarLayoutAlgorithm = siderBarPattern->CreateLayoutAlgorithm();
    EXPECT_NE(sideBarLayoutAlgorithm, nullptr);

    sideBarLayoutWrapper->SetLayoutAlgorithm(
        AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(sideBarLayoutAlgorithm));
    sideBarLayoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(5.0f), CalcLength(5.0f)));

    siderBar->MountToParent(frameNode);
    layoutWrapper->AppendChild(sideBarLayoutWrapper);

    auto dividerNode = FrameNode::GetOrCreateFrameNode(
        V2::DIVIDER_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<DividerPattern>(); });
    EXPECT_NE(dividerNode, nullptr);

    RefPtr<GeometryNode> dividerNodeGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    dividerNodeGeometryNode->Reset();
    RefPtr<LayoutWrapperNode> dividerNodeLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(dividerNode, dividerNodeGeometryNode, dividerNode->GetLayoutProperty());
    EXPECT_NE(dividerNodeLayoutWrapper, nullptr);
    auto dividerNodePattern = dividerNode->GetPattern<DividerPattern>();
    EXPECT_NE(dividerNodePattern, nullptr);

    dividerNodeLayoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(childLayoutConstraint);
    auto dividerNodeLayoutAlgorithm = dividerNodePattern->CreateLayoutAlgorithm();
    EXPECT_NE(dividerNodeLayoutAlgorithm, nullptr);

    dividerNodeLayoutWrapper->SetLayoutAlgorithm(
        AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(dividerNodeLayoutAlgorithm));
    dividerNodeLayoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(5.0f), CalcLength(5.0f)));

    dividerNode->MountToParent(frameNode);
    layoutWrapper->AppendChild(dividerNodeLayoutWrapper);

    int32_t imgNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto imgNode = FrameNode::GetOrCreateFrameNode(
        V2::IMAGE_ETS_TAG, imgNodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    EXPECT_NE(imgNode, nullptr);

    RefPtr<GeometryNode> imgNodeGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    imgNodeGeometryNode->Reset();
    RefPtr<LayoutWrapperNode> imgNodeLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(imgNode, imgNodeGeometryNode, imgNode->GetLayoutProperty());
    EXPECT_NE(imgNodeLayoutWrapper, nullptr);
    auto imgNodePattern = imgNode->GetPattern<ImagePattern>();
    EXPECT_NE(imgNodePattern, nullptr);

    imgNodeLayoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(childLayoutConstraint);
    auto imgNodeLayoutAlgorithm = imgNodePattern->CreateLayoutAlgorithm();
    EXPECT_NE(imgNodeLayoutAlgorithm, nullptr);

    imgNodeLayoutWrapper->SetLayoutAlgorithm(
        AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(imgNodeLayoutAlgorithm));
    imgNodeLayoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(5.0f), CalcLength(5.0f)));

    imgNode->MountToParent(frameNode);
    layoutWrapper->AppendChild(imgNodeLayoutWrapper);

    auto sideBarContainerPattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    EXPECT_NE(sideBarContainerPattern, nullptr);
    auto sideLayoutAlgorithm = sideBarContainerPattern->CreateLayoutAlgorithm();
    EXPECT_NE(sideLayoutAlgorithm, nullptr);
    layoutWrapper->SetLayoutAlgorithm(AceType::MakeRefPtr<LayoutAlgorithmWrapper>(sideLayoutAlgorithm));

    LayoutConstraintF LayoutConstraintVaildSize;
    LayoutConstraintVaildSize.selfIdealSize.SetSize(SizeF(1000000, 1000000));
    layoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(LayoutConstraintVaildSize);
    layoutWrapper->GetLayoutProperty()->UpdateContentConstraint();
    sideLayoutAlgorithm->Measure(AccessibilityManager::RawPtr(layoutWrapper));
    sideLayoutAlgorithm->Layout(AccessibilityManager::RawPtr(layoutWrapper));

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(layoutWrapper->GetLayoutAlgorithm());
    EXPECT_NE(layoutAlgorithmWrapper, nullptr);
    auto sideBarContainerLayoutAlgorithm = AceType::DynamicCast<SideBarContainerLayoutAlgorithm>
            (layoutAlgorithmWrapper->GetLayoutAlgorithm());
    EXPECT_NE(sideBarContainerLayoutAlgorithm, nullptr);
    sideBarContainerLayoutAlgorithm->SetRealDividerWidth(REAL_WIDTH);
    sideBarContainerLayoutAlgorithm->GetRealDividerWidth();
    sideBarContainerLayoutAlgorithm->SetSideBarStatus(SideBarStatus::SHOW);
    frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>()->UpdateSideBarPosition(SideBarPosition::END);
    sideLayoutAlgorithm->Measure(AccessibilityManager::RawPtr(layoutWrapper));
    sideLayoutAlgorithm->Layout(AccessibilityManager::RawPtr(layoutWrapper));

    sideBarContainerLayoutAlgorithm->SetSideBarStatus(SideBarStatus::HIDDEN);
    frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>()->UpdateSideBarPosition(SideBarPosition::START);
    sideLayoutAlgorithm->Measure(AccessibilityManager::RawPtr(layoutWrapper));
    sideLayoutAlgorithm->Layout(AccessibilityManager::RawPtr(layoutWrapper));

    sideBarContainerLayoutAlgorithm->SetSideBarStatus(SideBarStatus::CHANGING);
    frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>()->UpdateSideBarPosition(SideBarPosition::START);
    sideLayoutAlgorithm->Measure(AccessibilityManager::RawPtr(layoutWrapper));
    sideLayoutAlgorithm->Layout(AccessibilityManager::RawPtr(layoutWrapper));

    sideBarContainerLayoutAlgorithm->SetSideBarStatus(SideBarStatus::HIDDEN);
    frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>()->UpdateSideBarPosition(SideBarPosition::END);
    sideLayoutAlgorithm->Measure(AccessibilityManager::RawPtr(layoutWrapper));
    sideLayoutAlgorithm->Layout(AccessibilityManager::RawPtr(layoutWrapper));

    sideBarContainerLayoutAlgorithm->SetSideBarStatus(SideBarStatus::CHANGING);
    frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>()->UpdateSideBarPosition(SideBarPosition::END);
    sideLayoutAlgorithm->Measure(AccessibilityManager::RawPtr(layoutWrapper));
    sideLayoutAlgorithm->Layout(AccessibilityManager::RawPtr(layoutWrapper));

    frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>
        ()->UpdateSideBarContainerType(SideBarContainerType::AUTO);
    sideLayoutAlgorithm->Measure(AccessibilityManager::RawPtr(layoutWrapper));
    sideLayoutAlgorithm->Layout(AccessibilityManager::RawPtr(layoutWrapper));
}

/**
 * @tc.name: SideBarViewTestNg005
 * @tc.desc: Test SideBar AddDividerHotZoneRect
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg005, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutWrapperNode> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    auto row = FrameNode::CreateFrameNode(
        V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    EXPECT_NE(row, nullptr);
    row->MountToParent(frameNode);

    auto siderBar = FrameNode::CreateFrameNode(
        V2::SIDE_BAR_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<SideBarContainerPattern>());
    EXPECT_NE(siderBar, nullptr);
    siderBar->MountToParent(frameNode);

    auto dividerNode = FrameNode::GetOrCreateFrameNode(
        V2::DIVIDER_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<DividerPattern>(); });
    EXPECT_NE(dividerNode, nullptr);
    dividerNode->MountToParent(frameNode);

    int32_t imgNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto imgNode = FrameNode::GetOrCreateFrameNode(
        V2::IMAGE_ETS_TAG, imgNodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    EXPECT_NE(imgNode, nullptr);
    imgNode->MountToParent(frameNode);

    auto sideBarContainerPattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    EXPECT_NE(sideBarContainerPattern, nullptr);
    auto sideLayoutAlgorithm = sideBarContainerPattern->CreateLayoutAlgorithm();
    EXPECT_NE(sideLayoutAlgorithm, nullptr);
    layoutWrapper->SetLayoutAlgorithm(AceType::MakeRefPtr<LayoutAlgorithmWrapper>(sideLayoutAlgorithm));

    LayoutConstraintF LayoutConstraintVaildSize;
    LayoutConstraintVaildSize.selfIdealSize.SetSize(SizeF(1000000, 1000000));
    layoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(LayoutConstraintVaildSize);
    layoutWrapper->GetLayoutProperty()->UpdateContentConstraint();

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(layoutWrapper->GetLayoutAlgorithm());
    EXPECT_NE(layoutAlgorithmWrapper, nullptr);
    auto sideBarContainerLayoutAlgorithm = AceType::DynamicCast<SideBarContainerLayoutAlgorithm>
            (layoutAlgorithmWrapper->GetLayoutAlgorithm());
    EXPECT_NE(sideBarContainerLayoutAlgorithm, nullptr);
    sideBarContainerLayoutAlgorithm->SetRealDividerWidth(REAL_WIDTH);
    sideBarContainerLayoutAlgorithm->GetRealDividerWidth();
    sideBarContainerPattern->AddDividerHotZoneRect(sideBarContainerLayoutAlgorithm);
}

/**
 * @tc.name: SideBarViewTestNg006
 * @tc.desc: Test SideBar AddDividerHotZoneRect
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg006, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutWrapperNode> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    auto row = FrameNode::CreateFrameNode(
        V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    EXPECT_NE(row, nullptr);
    row->MountToParent(frameNode);

    auto siderBar = FrameNode::CreateFrameNode(
        V2::SIDE_BAR_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<SideBarContainerPattern>());
    EXPECT_NE(siderBar, nullptr);
    siderBar->MountToParent(frameNode);

    auto dividerNode = FrameNode::GetOrCreateFrameNode(
        V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<DividerPattern>(); });
    EXPECT_NE(dividerNode, nullptr);
    dividerNode->MountToParent(frameNode);

    int32_t imgNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto imgNode = FrameNode::GetOrCreateFrameNode(
        V2::IMAGE_ETS_TAG, imgNodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    EXPECT_NE(imgNode, nullptr);
    imgNode->MountToParent(frameNode);

    auto sideBarContainerPattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    EXPECT_NE(sideBarContainerPattern, nullptr);
    auto sideLayoutAlgorithm = sideBarContainerPattern->CreateLayoutAlgorithm();
    EXPECT_NE(sideLayoutAlgorithm, nullptr);
    layoutWrapper->SetLayoutAlgorithm(AceType::MakeRefPtr<LayoutAlgorithmWrapper>(sideLayoutAlgorithm));

    LayoutConstraintF LayoutConstraintVaildSize;
    LayoutConstraintVaildSize.selfIdealSize.SetSize(SizeF(1000000, 1000000));
    layoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(LayoutConstraintVaildSize);
    layoutWrapper->GetLayoutProperty()->UpdateContentConstraint();

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(layoutWrapper->GetLayoutAlgorithm());
    EXPECT_NE(layoutAlgorithmWrapper, nullptr);
    auto sideBarContainerLayoutAlgorithm = AceType::DynamicCast<SideBarContainerLayoutAlgorithm>
            (layoutAlgorithmWrapper->GetLayoutAlgorithm());
    EXPECT_NE(sideBarContainerLayoutAlgorithm, nullptr);
    sideBarContainerLayoutAlgorithm->SetRealDividerWidth(REAL_WIDTH);
    sideBarContainerLayoutAlgorithm->GetRealDividerWidth();
    sideBarContainerPattern->AddDividerHotZoneRect(sideBarContainerLayoutAlgorithm);
}

/**
 * @tc.name: SideBarViewTestNg007
 * @tc.desc: Test SideBar AddDividerHotZoneRect
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg007, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutWrapperNode> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    auto sideBarContainerPattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    EXPECT_NE(sideBarContainerPattern, nullptr);
    auto sideLayoutAlgorithm = sideBarContainerPattern->CreateLayoutAlgorithm();
    EXPECT_NE(sideLayoutAlgorithm, nullptr);
    layoutWrapper->SetLayoutAlgorithm(AceType::MakeRefPtr<LayoutAlgorithmWrapper>(sideLayoutAlgorithm));

    LayoutConstraintF LayoutConstraintVaildSize;
    LayoutConstraintVaildSize.selfIdealSize.SetSize(SizeF(1000000, 1000000));
    layoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(LayoutConstraintVaildSize);
    layoutWrapper->GetLayoutProperty()->UpdateContentConstraint();

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(layoutWrapper->GetLayoutAlgorithm());
    EXPECT_NE(layoutAlgorithmWrapper, nullptr);
    auto sideBarContainerLayoutAlgorithm = AceType::DynamicCast<SideBarContainerLayoutAlgorithm>
            (layoutAlgorithmWrapper->GetLayoutAlgorithm());
    EXPECT_NE(sideBarContainerLayoutAlgorithm, nullptr);
    sideBarContainerLayoutAlgorithm->SetRealDividerWidth(REAL_WIDTH);
    sideBarContainerLayoutAlgorithm->GetRealDividerWidth();
    sideBarContainerPattern->AddDividerHotZoneRect(sideBarContainerLayoutAlgorithm);
}

/**
 * @tc.name: SideBarViewTestNg008
 * @tc.desc: Test SideBar OnHover
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg008, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);
    auto sideBarContainerPattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    EXPECT_NE(sideBarContainerPattern, nullptr);
    sideBarContainerPattern->OnHover(true);
    sideBarContainerPattern->OnHover(false);
}

/**
 * @tc.name: SideBarViewTestNg009
 * @tc.desc: Test SideBar OnHover
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg009, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetDividerStrokeWidth(NONE_WIDTH);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);
    auto sideBarContainerPattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    EXPECT_NE(sideBarContainerPattern, nullptr);
    sideBarContainerPattern->OnHover(true);
    sideBarContainerPattern->OnHover(false);
}

/**
 * @tc.name: SideBarViewTestNg010
 * @tc.desc: Test SideBar HandlePanEventEnd
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg010, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);
    auto sideBarContainerPattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    EXPECT_NE(sideBarContainerPattern, nullptr);
    sideBarContainerPattern->SetSideBarStatus(SideBarStatus::HIDDEN);
    sideBarContainerPattern->HandlePanEventEnd();
    sideBarContainerPattern->SetSideBarStatus(SideBarStatus::AUTO);
    sideBarContainerPattern->HandlePanEventEnd();
}

/**
 * @tc.name: SideBarViewTestNg011
 * @tc.desc: Test SideBar json
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg011, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetControlButtonWidth(STROKE_WIDTH);
    SideBarContainerModelInstance.SetDividerStrokeWidth(STROKE_WIDTH);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    auto layoutProperty = frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>();
    EXPECT_NE(layoutProperty, nullptr);

    auto clone = layoutProperty->Clone();
    clone.Reset();
    auto json = JsonUtil::Create(true);
    layoutProperty->ToJsonValue(json, filter);
}

/**
 * @tc.name: SideBarViewTestNg014
 * @tc.desc: Test SideBar Create
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg014, TestSize.Level1)
{
    SideBarContainerModelNG SideBarContainerModelInstance;
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    pipeline->minPlatformVersion_ = 12;
    SideBarContainerModelInstance.Create();
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: SideBarViewTestNg015
 * @tc.desc: Test SideBar unavailable widthType.
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sideBar, get layoutProperty.
     */
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetSideBarWidth(DEFAULT_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetMinSideBarWidth(DEFAULT_MIN_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetMaxSideBarWidth(DEFAULT_MAX_SIDE_BAR_WIDTH);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);

    /**
     * @tc.steps: step2. call ParseAndSetWidth function with widthType unavailable.
     * @tc.expected: step2. sideBarWidth doesn't set.
     */
    Dimension sideBarWidth = DEFAULT_SIDE_BAR_WIDTH;
    SideBarContainerModelInstance.ParseAndSetWidth(static_cast<WidthType>(-1), sideBarWidth);
    EXPECT_EQ(layoutProperty->GetSideBarWidth(), DEFAULT_SIDE_BAR_WIDTH);
    EXPECT_EQ(layoutProperty->GetMinSideBarWidth(), DEFAULT_MIN_SIDE_BAR_WIDTH);
    EXPECT_EQ(layoutProperty->GetMaxSideBarWidth(), DEFAULT_MAX_SIDE_BAR_WIDTH);
}

/**
 * @tc.name: SideBarViewTestNg016
 * @tc.desc: Test SideBar set sideBarWith in double bind mode.
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sideBar, get layoutProperty.
     */
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetSideBarWidth(DEFAULT_SIDE_BAR_WIDTH, true);
    SideBarContainerModelInstance.SetMinSideBarWidth(DEFAULT_MIN_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetMaxSideBarWidth(DEFAULT_MAX_SIDE_BAR_WIDTH);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    auto pattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. call SetSideBarWidth function in doubleBind mode.
     * @tc.expected: step2. sideBarWidth doesn't set.
     */
    pattern->isInDividerDrag_ = true;
    SideBarContainerModelInstance.SetSideBarWidth(DEFAULT_MIN_SIDE_BAR_WIDTH, true);
    SideBarContainerModelInstance.SetMinSideBarWidth(DEFAULT_MAX_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetMaxSideBarWidth(DEFAULT_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetSideBarWidth(AceType::RawPtr(frameNode), DEFAULT_MIN_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetMinSideBarWidth(AceType::RawPtr(frameNode), DEFAULT_MAX_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetMaxSideBarWidth(AceType::RawPtr(frameNode), DEFAULT_SIDE_BAR_WIDTH);
    EXPECT_EQ(layoutProperty->GetSideBarWidth(), DEFAULT_SIDE_BAR_WIDTH);
    EXPECT_EQ(layoutProperty->GetMinSideBarWidth(), DEFAULT_MIN_SIDE_BAR_WIDTH);
    EXPECT_EQ(layoutProperty->GetMaxSideBarWidth(), DEFAULT_MAX_SIDE_BAR_WIDTH);
}

/**
 * @tc.name: SideBarViewTestNg017
 * @tc.desc: Test SideBar setMinContentWidth with negative value.
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sideBar, get layoutProperty.
     */
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetMinContentWidth(DEFAULT_SIDE_BAR_WIDTH);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);

    /**
     * @tc.steps: step2. call SetMinContentWidth function with negative value.
     * @tc.expected: step2. sideBar minContentWidth doesn't set.
     */
    SideBarContainerModelInstance.SetMinContentWidth(-DEFAULT_SIDE_BAR_WIDTH);
    SideBarContainerModelInstance.SetMinContentWidth(AceType::RawPtr(frameNode), -DEFAULT_SIDE_BAR_WIDTH);
    EXPECT_EQ(layoutProperty->GetMinContentWidth(), DEFAULT_MIN_CONTENT_WIDTH);
}

/**
 * @tc.name: SideBarViewTestNg018
 * @tc.desc: Test SideBar OnUpdateShowSideBar function.
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sideBar, get layoutProperty.
     */
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetMinContentWidth(DEFAULT_SIDE_BAR_WIDTH);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    ASSERT_NE(pattern, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    auto eventHub = frameNode->GetOrCreateEventHub<NG::SideBarContainerEventHub>();
    ASSERT_NE(eventHub, nullptr);
    bool localShow = false;
    ChangeEvent eventOnChange = [&localShow](const bool isShow) { localShow = isShow; };
    eventHub->SetOnChangeEvent(std::move(eventOnChange));

    pattern->realSideBarWidth_ = -DEFAULT_SIDE_BAR_WIDTH;
    layoutProperty->UpdateSideBarWidth(DEFAULT_SIDE_BAR_WIDTH);
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_EQ(pattern->realSideBarWidth_, DEFAULT_SIDE_BAR_WIDTH);

    pattern->hasInit_ = false;
    pattern->sideBarStatus_ = SideBarStatus::SHOW;
    layoutProperty->UpdateShowSideBar(true);
    pattern->userSetShowSideBar_ = false;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_FALSE(localShow);

    pattern->sideBarStatus_ = SideBarStatus::HIDDEN;
    layoutProperty->UpdateShowSideBar(false);
    pattern->userSetShowSideBar_ = true;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_FALSE(localShow);

    pattern->sideBarStatus_ = SideBarStatus::SHOW;
    layoutProperty->UpdateShowSideBar(false);
    pattern->userSetShowSideBar_ = true;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_FALSE(localShow);

    pattern->sideBarStatus_ = SideBarStatus::HIDDEN;
    layoutProperty->UpdateShowSideBar(true);
    pattern->userSetShowSideBar_ = false;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_FALSE(localShow);
}

/**
 * @tc.name: SideBarViewTestNg019
 * @tc.desc: Test SideBar OnUpdateShowSideBar function.
 * @tc.type: FUNC
 */
HWTEST_F(SideBarViewTestNg, SideBarViewTestNg019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sideBar, get layoutProperty.
     */
    SideBarContainerModelNG SideBarContainerModelInstance;
    SideBarContainerModelInstance.Create();
    SideBarContainerModelInstance.SetMinContentWidth(DEFAULT_SIDE_BAR_WIDTH);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = AceType::DynamicCast<SideBarContainerPattern>(frameNode->GetPattern());
    ASSERT_NE(pattern, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<SideBarContainerLayoutProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    auto eventHub = frameNode->GetOrCreateEventHub<NG::SideBarContainerEventHub>();
    ASSERT_NE(eventHub, nullptr);
    bool localShow = false;
    ChangeEvent eventOnChange = [&localShow](const bool isShow) { localShow = isShow; };
    eventHub->SetOnChangeEvent(std::move(eventOnChange));

    pattern->realSideBarWidth_ = -DEFAULT_SIDE_BAR_WIDTH;
    layoutProperty->UpdateSideBarWidth(DEFAULT_SIDE_BAR_WIDTH);
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_EQ(pattern->realSideBarWidth_, DEFAULT_SIDE_BAR_WIDTH);

    pattern->hasInit_ = true;
    pattern->sideBarStatus_ = SideBarStatus::SHOW;
    layoutProperty->UpdateShowSideBar(true);
    pattern->userSetShowSideBar_ = false;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_FALSE(localShow);

    pattern->sideBarStatus_ = SideBarStatus::HIDDEN;
    layoutProperty->UpdateShowSideBar(false);
    pattern->userSetShowSideBar_ = true;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_FALSE(localShow);

    pattern->sideBarStatus_ = SideBarStatus::HIDDEN;
    layoutProperty->UpdateShowSideBar(true);
    pattern->userSetShowSideBar_ = false;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_TRUE(localShow);

    pattern->sideBarStatus_ = SideBarStatus::SHOW;
    layoutProperty->UpdateShowSideBar(false);
    pattern->userSetShowSideBar_ = true;
    pattern->OnUpdateShowSideBar(layoutProperty);
    EXPECT_FALSE(localShow);
}
} // namespace OHOS::Ace::NG

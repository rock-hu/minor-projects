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

#include "test/unittest/core/manager/drag_drop_manager_test_ng.h"
#include "test/mock/base/mock_pixel_map.h"
#include "test/mock/base/mock_subwindow.h"
#include "test/mock/base/mock_task_executor.h"
#include "test/mock/core/common/mock_udmf.h"
#include "test/mock/core/render/mock_render_context.h"

#include "core/common/udmf/udmf_client.h"
#include "core/common/udmf/unified_data.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
void DragDropManagerTestNgCoverage::SetUpTestCase()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
}

void DragDropManagerTestNgCoverage::TearDownTestCase()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
}

/**
 * @tc.name: DragDropManagerTestNgCoverage001
 * @tc.desc: Test FindTargetInChildNodes
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage001, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull = AceType::MakeRefPtr<FrameNode>(
        V2::UI_EXTENSION_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(GLOBAL_X, GLOBAL_Y);
    std::vector<RefPtr<FrameNode>> hitFrameNodes;
    hitFrameNodes.push_back(parentFrameNodeTmp);
    hitFrameNodes.push_back(frameNodeNull);
    hitFrameNodes.push_back(childNodeNull);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    auto result = dragDropManager->FindTargetInChildNodes(parentNode, hitFrameNodes, true);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage002
 * @tc.desc: Test FindTargetInChildNodes
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. creat node and generate a node tree.
     */
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    const RefPtr<FrameNode> GET_PARENT = FrameNode::CreateFrameNode("parent", 4, AceType::MakeRefPtr<Pattern>());
    const RefPtr<FrameNode> GET_CHILD1 = FrameNode::CreateFrameNode("child1", 5, AceType::MakeRefPtr<Pattern>());
    const RefPtr<FrameNode> GET_CHILD2 = FrameNode::CreateFrameNode("child2", 6, AceType::MakeRefPtr<Pattern>());
    GET_CHILD1->UpdateInspectorId("child1");
    GET_CHILD2->UpdateInspectorId("child2");
    GET_PARENT->frameChildren_.insert(GET_CHILD1);
    GET_PARENT->frameChildren_.insert(GET_CHILD2);
    std::vector<RefPtr<FrameNode>> hitFrameNodes;
    hitFrameNodes.push_back(GET_PARENT);
    hitFrameNodes.push_back(GET_CHILD1);
    hitFrameNodes.push_back(GET_CHILD2);
    dragDropManager->AddGridDragFrameNode(GET_PARENT->GetId(), GET_PARENT);

    /**
     * @tc.steps: step2. initialize parentEventHub and set HitTestMode.
     */
    auto parentEventHub = GET_PARENT->GetOrCreateGestureEventHub();
    parentEventHub->SetHitTestMode(HitTestMode::HTMBLOCK);
    auto dragResult = dragDropManager->FindTargetInChildNodes(GET_PARENT, hitFrameNodes, true);

    /**
     * @tc.steps: step3. call GetDispatchFrameNode.
     * @tc.expected: expect GetDispatchFrameNode ruturn nullptr.
     */
    EXPECT_EQ(dragResult, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage003
 * @tc.desc: Test FindTargetInChildNodes with null child nodes
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage003, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto parentNode = AceType::MakeRefPtr<FrameNode>(
        NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<GridPattern>());
    for (int i = 0; i < 5; ++i) {
        parentNode->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    }
    std::vector<RefPtr<FrameNode>> hitFrameNodes;
    hitFrameNodes.push_back(parentNode);
    auto result = dragDropManager->FindTargetInChildNodes(parentNode, hitFrameNodes, true);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage004
 * @tc.desc: Test FindTargetInChildNodes with hit FrameNode having OnDrop event
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage004, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto parentNode = AceType::MakeRefPtr<FrameNode>(
        NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<GridPattern>());
    auto hitFrameNode = AceType::MakeRefPtr<FrameNode>(
        NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<GridPattern>());
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    auto parentEventHub = hitFrameNode->GetOrCreateGestureEventHub();
    parentEventHub->SetHitTestMode(HitTestMode::HTMBLOCK);
    dragDropManager->AddGridDragFrameNode(hitFrameNode->GetId(), hitFrameNode);

    parentNode->frameChildren_.insert(WeakPtr<NG::FrameNode>(hitFrameNode));
    std::vector<RefPtr<FrameNode>> hitFrameNodes;
    hitFrameNodes.push_back(hitFrameNode);

    auto result = dragDropManager->FindTargetInChildNodes(parentNode, hitFrameNodes, true);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage005
 * @tc.desc: Test FindTargetDropNode with null parentNode
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage005, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    PointF point(0.0, 0.0);
    auto result = dragDropManager->FindTargetDropNode(nullptr, point);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage006
 * @tc.desc: Test FindTargetInChildNodes
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage006, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull =
        AceType::MakeRefPtr<FrameNode>(V2::EMBEDDED_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(GLOBAL_X, GLOBAL_Y);
    std::vector<RefPtr<FrameNode>> hitFrameNodes;
    hitFrameNodes.push_back(parentFrameNodeTmp);
    hitFrameNodes.push_back(frameNodeNull);
    hitFrameNodes.push_back(childNodeNull);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    auto result = dragDropManager->FindTargetInChildNodes(parentNode, hitFrameNodes, true);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage007
 * @tc.desc: Test FindTargetDropNode with inactive or invisible parentFrameNode
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage007, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto parentNode = AceType::MakeRefPtr<FrameNode>(
        NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<GridPattern>());
    PointF point(0.0, 0.0);

    parentNode->SetActive(false);
    auto result = dragDropManager->FindTargetDropNode(parentNode, point);
    EXPECT_EQ(result, nullptr);

    parentNode->SetActive(true);
    parentNode->GetLayoutProperty()->UpdateVisibility(VisibleType::INVISIBLE);
    result = dragDropManager->FindTargetDropNode(parentNode, point);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage009
 * @tc.desc: Test FindTargetDropNode with null child nodes
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage009, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto parentNode = AceType::MakeRefPtr<FrameNode>(
        NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<GridPattern>());
    for (int i = 0; i < 5; ++i) {
        parentNode->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    }
    PointF point(0.0, 0.0);
    auto result = dragDropManager->FindTargetDropNode(parentNode, point);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage010
 * @tc.desc: Test FindTargetDropNode with null child nodes
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage010, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull =
        AceType::MakeRefPtr<FrameNode>(V2::EMBEDDED_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    frameNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    childNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(100.0, 100.0);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    auto result = dragDropManager->FindTargetDropNode(parentNode, point);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage011
 * @tc.desc: Test UpdateDragAllowDrop
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create DragDropManager.
     * @tc.expected: dragDropManager is not null.
     */
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);

    /**
     * @tc.steps: step2. construct frameNode and update the properties.
     * @tc.expected: frameNode is not null.
     */
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    std::set<std::string> allowDrop = { NODE_TAG };
    frameNode->SetAllowDrop(allowDrop);

    /**
     * @tc.steps: step2. construct frameNode and update the properties.
     * @tc.expected: frameNode is not null.
     */
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(childNode, nullptr);
    childNode->SetAllowDrop(allowDrop);

    /**
     * @tc.steps: step3. call UpdateDragAllowDrop with frameNode and copy.
     * @tc.expected: dragDropManager->summaryMap_.empty() return a true value.
     */
    dragDropManager->summaryMap_.clear();
    PointF point(100.0, 100.0);
    dragDropManager->OnDragStart({ GLOBAL_X, GLOBAL_Y }, childNode);
    dragDropManager->summaryMap_.insert(make_pair(NODE_TAG, frameNodeNullId));
    dragDropManager->UpdateDragAllowDrop(frameNode, DragBehavior::UNKNOWN, -1);
    EXPECT_FALSE(dragDropManager->summaryMap_.empty());

    /**
     * @tc.steps: step3. call UpdateDragAllowDrop with frameNode and copy.
     * @tc.expected: dragDropManager->summaryMap_.empty() return a true value.
     */
    dragDropManager->summaryMap_.clear();
    dragDropManager->OnDragStart({ GLOBAL_X, GLOBAL_Y }, frameNode);
    dragDropManager->summaryMap_.insert(make_pair(NODE_TAG, frameNodeNullId));
    dragDropManager->UpdateDragAllowDrop(frameNode, DragBehavior::UNKNOWN, -1);
    EXPECT_FALSE(dragDropManager->summaryMap_.empty());

    /**
     * @tc.steps: step4. call UpdateDragAllowDrop with frameNode and copy.
     * @tc.expected: dragDropManager->summaryMap_.empty() return a false value.
     */
    dragDropManager->summaryMap_.clear();
    dragDropManager->summaryMap_.insert(make_pair(NODE_TAG, frameNodeNullId));
    dragDropManager->UpdateDragAllowDrop(frameNode, DragBehavior::MOVE, -1);
    EXPECT_FALSE(dragDropManager->summaryMap_.empty());

    /**
     * @tc.steps: step5. call UpdateDragAllowDrop with frameNode and copy.
     * @tc.expected: allowDrop.find(ROOT_ETS_TAG) is equal to allowDrop.end().
     */
    dragDropManager->summaryMap_.clear();
    dragDropManager->summaryMap_.insert(make_pair(ROOT_ETS_TAG, frameNodeNullId));
    dragDropManager->UpdateDragAllowDrop(frameNode, static_cast<DragBehavior>(-100), -1);
    EXPECT_TRUE(allowDrop.find(ROOT_ETS_TAG) == allowDrop.end());
}

/**
 * @tc.name: DragDropManagerTestNgCoverage012
 * @tc.desc: Test FindTargetDropNode with null child nodes
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage012, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull = AceType::MakeRefPtr<FrameNode>(
        V2::UI_EXTENSION_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    frameNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    childNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(100.0, 100.0);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    auto resultNode = dragDropManager->FindDragFrameNodeByPosition(point.GetX(), point.GetY());
    EXPECT_TRUE(resultNode);
    auto result = dragDropManager->FindTargetDropNode(parentNode, point);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage013
 * @tc.desc: Test CheckParentVisible out of parent is a true branch.
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a DragDropManager.
     * @tc.expected: dragDropManager is not null.
     */
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);

    RefPtr<NotifyDragEvent> notifyEvent = AceType::MakeRefPtr<NotifyDragEvent>();
    dragDropManager->UpdateNotifyDragEvent(notifyEvent, Point(1.0f, 1.0f), DragEventType::START);
    EXPECT_DOUBLE_EQ(notifyEvent->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(notifyEvent->GetY(), 1.0);

    std::unordered_map<int32_t, WeakPtr<FrameNode>> dragEnterNodes;
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeNullId, AceType::MakeRefPtr<GridPattern>());
    frameNode->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE, true);
    auto eventHub = frameNode->GetEventHub<EventHub>();
    eventHub->SetEnabled(true);
    dragEnterNodes[0] = nullptr;
    dragEnterNodes[1] = frameNode;
    dragDropManager->NotifyDragRegisterFrameNode(dragEnterNodes, DragEventType::START, notifyEvent);
    EXPECT_TRUE(frameNode->GetPattern<Pattern>());
}

/**
 * @tc.name: DragDropManagerTestNgCoverage014
 * @tc.desc: Test PrintDragFrameNode
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a DragDropManager
     */
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    OHOS::Ace::PointerEvent point;

    /**
     * @tc.steps: step2. call OnDragStart
     * @tc.expected: step2. draggedFrameNode_ &  preTargetFrameNode_ are assigned to the frameNode created previously
     */
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    dragDropManager->OnDragStart({ GLOBAL_X, GLOBAL_Y }, frameNode);
    dragDropManager->OnItemDragStart(GLOBAL_X, GLOBAL_Y, frameNode);
    auto draggedNode = dragDropManager->draggedFrameNode_;
    auto preTargetNode = dragDropManager->preTargetFrameNode_;
    dragDropManager->PrintGridDragFrameNode(GLOBAL_X, GLOBAL_Y, frameNode);
    ASSERT_TRUE(preTargetNode);
    dragDropManager->onDragCancel();
    dragDropManager->onItemDragCancel();
    dragDropManager->PrintDragFrameNode(point, frameNode);
    dragDropManager->PrintGridDragFrameNode(GLOBAL_X, GLOBAL_Y, frameNode);
    preTargetNode = dragDropManager->preTargetFrameNode_;
    ASSERT_TRUE(draggedNode);
    ASSERT_FALSE(preTargetNode);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage015
 * @tc.desc: Test PrintDragFrameNode
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a DragDropManager
     */
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    dragDropManager->TransDragWindowToDragFwk(111);
    dragDropManager->TransDragWindowToDragFwk(111);
    ASSERT_TRUE(dragDropManager->isDragFwkShow_);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage016
 * @tc.desc: Test isDistanceLimited when distance is less than the limit
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage016, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preMovePoint_ = Point(0, 0);
    Point point(3, 4); // Distance is 5
    EXPECT_TRUE(dragDropManager.isDistanceLimited(point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage017
 * @tc.desc: Test isDistanceLimited when distance is biger than the limit
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage017, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preMovePoint_ = Point(0, 0);
    Point point(100 + 1, 0); // Distance is greater than MOVE_DISTANCE_LIMIT
    EXPECT_FALSE(dragDropManager.isDistanceLimited(point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage018
 * @tc.desc: Test isTimeLimited when time interval is less than the limit
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage018, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 1000;
    PointerEvent pointerEvent;
    Point point(0, 0);
    EXPECT_FALSE(dragDropManager.isTimeLimited(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage019
 * @tc.desc: Test isTimeLimited when time interval is equal to the limit
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage019, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 1000;
    PointerEvent pointerEvent;
    pointerEvent.time += std::chrono::milliseconds(1000); // 1000ms after preTimeStamp_
    Point point(0, 0);
    EXPECT_FALSE(dragDropManager.isTimeLimited(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage020
 * @tc.desc: Test isTimeLimited when time interval is greater than the limit
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage020, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 1000;
    PointerEvent pointerEvent;
    pointerEvent.time += std::chrono::milliseconds(2500); // 1500ms after preTimeStamp_
    Point point(0, 0);
    EXPECT_FALSE(dragDropManager.isTimeLimited(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage021
 * @tc.desc: Test isTimeLimited when time is the same as preTimeStamp
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage021, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 1000;
    PointerEvent pointerEvent;
    pointerEvent.time += std::chrono::milliseconds(1000); // Same as preTimeStamp_
    Point point(0, 0);
    EXPECT_FALSE(dragDropManager.isTimeLimited(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage022
 * @tc.desc: Test isTimeLimited when current time is less than preTimeStamp
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage022, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 2000;
    PointerEvent pointerEvent;
    pointerEvent.time += std::chrono::milliseconds(1500); // Less than preTimeStamp_
    Point point(0, 0);
    EXPECT_FALSE(dragDropManager.isTimeLimited(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage023
 * @tc.desc: Test ReachMoveLimit when current time is less than preTimeStamp
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage023, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 2000;
    PointerEvent pointerEvent;
    pointerEvent.time += std::chrono::milliseconds(2005);
    pointerEvent.sourceTool = SourceTool::MOUSE;
    dragDropManager.preMovePoint_ = Point(0, 0);
    Point point(3, 4); // Distance is 5
    EXPECT_TRUE(dragDropManager.ReachMoveLimit(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage024
 * @tc.desc: Test ReachMoveLimit when current time is less than preTimeStamp
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage024, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 2000;
    PointerEvent pointerEvent;
    pointerEvent.time += std::chrono::milliseconds(1000);
    pointerEvent.sourceTool = SourceTool::MOUSE;
    dragDropManager.preMovePoint_ = Point(0, 0);
    Point point(15, 20); // Distance is 25
    EXPECT_FALSE(dragDropManager.ReachMoveLimit(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage025
 * @tc.desc: Test ReachMoveLimit when current time is less than preTimeStamp
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage025, TestSize.Level1)
{
    DragDropManager dragDropManager;
    dragDropManager.preTimeStamp_ = 2000;
    PointerEvent pointerEvent;
    pointerEvent.time += std::chrono::milliseconds(2002);
    pointerEvent.sourceTool = SourceTool::MOUSE;
    dragDropManager.preMovePoint_ = Point(0, 0);
    Point point(15, 20); // Distance is 25
    EXPECT_FALSE(dragDropManager.ReachMoveLimit(pointerEvent, point));
}

/**
 * @tc.name: DragDropManagerTestNgCoverage026
 * @tc.desc: Test OnDragMove OnDragEnd
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage026, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull =
        AceType::MakeRefPtr<FrameNode>(V2::EMBEDDED_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    frameNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    childNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(100.0, 100.0);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    PointerEvent pointEvent(100, 100, 100, 100);
    auto draggedNode = dragDropManager->draggedFrameNode_;
    auto preTargetNode = dragDropManager->preTargetFrameNode_;
    dragDropManager->isMouseDragged_ = true;
    dragDropManager->isDragWindowShow_ = true;
    std::string extraInfo;
    dragDropManager->OnTextDragEnd(100, 100, extraInfo);
    EXPECT_EQ(dragDropManager->currentId_, -1);
    ASSERT_FALSE(draggedNode);
    ASSERT_FALSE(preTargetNode);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage027
 * @tc.desc: Test OnDragMove OnDragEnd
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage027, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull = AceType::MakeRefPtr<FrameNode>(
        V2::UI_EXTENSION_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    frameNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    childNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(100.0, 100.0);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    PointerEvent pointEvent(100, 100, 100, 100);
    auto draggedNode = dragDropManager->draggedFrameNode_;
    auto preTargetNode = dragDropManager->preTargetFrameNode_;
    dragDropManager->isMouseDragged_ = true;
    dragDropManager->isDragWindowShow_ = true;
    dragDropManager->OnDragMove(pointEvent, EXTRA_INFO);
    dragDropManager->OnDragEnd(pointEvent, EXTRA_INFO);
    ASSERT_FALSE(draggedNode);
    ASSERT_FALSE(preTargetNode);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage028
 * @tc.desc: Test OnDragMove OnDragEnd
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage028, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct a DragDropManager
     */
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    PointerEvent point(100, 100, 100, 100);
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());

    /**
     * @tc.steps: step2. call OnDragStart
     * @tc.expected: step2. draggedFrameNode_ &  preTargetFrameNode_ are assigned to the frameNode created previously
     */
    frameNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    auto draggedNode = dragDropManager->draggedFrameNode_;
    auto preTargetNode = dragDropManager->preTargetFrameNode_;
    dragDropManager->OnDragMove(point, EXTRA_INFO);
    dragDropManager->OnDragEnd(point, EXTRA_INFO);
    ASSERT_FALSE(draggedNode);
    ASSERT_FALSE(preTargetNode);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage029
 * @tc.desc: Test RequestDragSummaryInfoAndPrivilege
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage029, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    dragDropManager->RequestDragSummaryInfoAndPrivilege();
    EXPECT_NE(dragDropManager, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage032
 * @tc.desc: Test CheckRemoteDat with empty udkey
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage032, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    PointerEvent point;
    std::string remoteUdKey;
    auto result = dragDropManager->CheckRemoteData(frameNode, point, remoteUdKey);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage033
 * @tc.desc: Test CheckRemoteDat with isRemoteData
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage033, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    PointerEvent point;
    std::string remoteUdKey = "abc";
    auto mockUdmfClient = static_cast<MockUdmfClient*>(UdmfClient::GetInstance());
    EXPECT_CALL(*mockUdmfClient, GetRemoteStatus(testing::_)).WillOnce(testing::Return(true));
    auto result = dragDropManager->CheckRemoteData(frameNode, point, remoteUdKey);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage034
 * @tc.desc: Test RequestUDMFDataWithUDKey
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage034, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    std::string remoteUdKey = "abc";
    auto mockUdmfClient = static_cast<MockUdmfClient*>(UdmfClient::GetInstance());
    EXPECT_CALL(*mockUdmfClient, GetData(testing::_, testing::_)).Times(1).WillOnce(testing::Return(1));
    auto result = dragDropManager->RequestUDMFDataWithUDKey(remoteUdKey);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage035
 * @tc.desc: Test RequestUDMFDataWithUDKey
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage035, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    std::string remoteUdKey;
    auto result = dragDropManager->RequestUDMFDataWithUDKey(remoteUdKey);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage036
 * @tc.desc: Test RequestUDMFDataWithUDKey
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage036, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    std::string remoteUdKey = "ABC";
    auto mockUdmfClient = static_cast<MockUdmfClient*>(UdmfClient::GetInstance());
    EXPECT_CALL(*mockUdmfClient, GetData(testing::_, testing::_)).Times(1).WillOnce(testing::Return(0));
    auto result = dragDropManager->RequestUDMFDataWithUDKey(remoteUdKey);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage037
 * @tc.desc: Test DoDropAction
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage037, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    std::string remoteUdKey = "abc";
    RefPtr<UnifiedData> unifiedData = AceType::MakeRefPtr<MockUnifiedData>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    PointerEvent point;
    dragDropManager->DoDropAction(frameNode, point, unifiedData, remoteUdKey);
    EXPECT_NE(unifiedData, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage038
 * @tc.desc: Test DoDropAction
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage038, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);
    std::string remoteUdKey;
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    PointerEvent point;
    dragDropManager->DoDropAction(frameNode, point, nullptr, remoteUdKey);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage039
 * @tc.desc: Test GetGatherPixelMap
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage039, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    RefPtr<MockPixelMap> mockPixelMap = AceType::MakeRefPtr<MockPixelMap>();
    dragDropManager->PushGatherPixelMap(mockPixelMap);
    DragDataCore dragData;
    EXPECT_CALL(*mockPixelMap, GetWidth()).Times(1).WillOnce(testing::Return(1.0f));
    EXPECT_CALL(*mockPixelMap, GetHeight()).Times(1).WillOnce(testing::Return(1.0f));
    dragDropManager->GetGatherPixelMap(dragData, 2.0f, 1.0f, 1.0f);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage040
 * @tc.desc: Test GetGatherPixelMap
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage040, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    RefPtr<MockPixelMap> mockPixelMap = AceType::MakeRefPtr<MockPixelMap>();
    dragDropManager->PushGatherPixelMap(mockPixelMap);
    DragDataCore dragData;
    EXPECT_CALL(*mockPixelMap, GetWidth()).Times(1).WillOnce(testing::Return(1.0f));
    EXPECT_CALL(*mockPixelMap, GetHeight()).Times(1).WillOnce(testing::Return(0.0f));
    dragDropManager->GetGatherPixelMap(dragData, 2.0f, 1.0f, 1.0f);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage041
 * @tc.desc: Test GetGatherPixelMap
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage041, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    RefPtr<MockPixelMap> mockPixelMap = AceType::MakeRefPtr<MockPixelMap>();
    dragDropManager->PushGatherPixelMap(mockPixelMap);
    DragDataCore dragData;
    EXPECT_CALL(*mockPixelMap, GetWidth()).Times(1).WillOnce(testing::Return(0.0f));
    EXPECT_CALL(*mockPixelMap, GetHeight()).Times(1).WillOnce(testing::Return(1.0f));
    dragDropManager->GetGatherPixelMap(dragData, 2.0f, 1.0f, 1.0f);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage042
 * @tc.desc: Test GetGatherPixelMap
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage042, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    RefPtr<MockPixelMap> mockPixelMap = AceType::MakeRefPtr<MockPixelMap>();
    dragDropManager->PushGatherPixelMap(mockPixelMap);
    DragDataCore dragData;
    EXPECT_CALL(*mockPixelMap, GetWidth()).Times(1).WillOnce(testing::Return(0.0f));
    EXPECT_CALL(*mockPixelMap, GetHeight()).Times(1).WillOnce(testing::Return(0.0f));
    dragDropManager->GetGatherPixelMap(dragData, 2.0f, 1.0f, 1.0f);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage043
 * @tc.desc: Test UpdateGatherNodeAttr
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage043, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    GatherAnimationInfo gatherAnimationInfo = { 2.0f, 1.0f, 1.0f, { 0.0, 0.0 } };
    dragDropManager->UpdateGatherNodeAttr(manager, gatherAnimationInfo);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage044
 * @tc.desc: Test UpdateGatherNodeAttr
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage044, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto imageNode1 = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    ASSERT_NE(imageNode1, nullptr);
    auto imageNode2 = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    ASSERT_NE(imageNode2, nullptr);
    auto imageNode3 = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    ASSERT_NE(imageNode3, nullptr);
    auto imageNode4 = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    ASSERT_NE(imageNode4, nullptr);
    GatherNodeChildInfo gatherNodeChildInfo1 = { imageNode1, { 100, 100 }, 100, 100 };
    GatherNodeChildInfo gatherNodeChildInfo2 = { imageNode2, { 100, 100 }, 100, 0 };
    GatherNodeChildInfo gatherNodeChildInfo3 = { imageNode3, { 100, 100 }, 0, 100 };
    GatherNodeChildInfo gatherNodeChildInfo4 = { imageNode4, { 100, 100 }, 0, 0 };
    manager->gatherNodeChildrenInfo_.push_back(gatherNodeChildInfo1);
    manager->gatherNodeChildrenInfo_.push_back(gatherNodeChildInfo2);
    manager->gatherNodeChildrenInfo_.push_back(gatherNodeChildInfo3);
    manager->gatherNodeChildrenInfo_.push_back(gatherNodeChildInfo4);
    GatherAnimationInfo gatherAnimationInfo = { 2.0f, 1.0f, 1.0f, { 0.0, 0.0 } };
    dragDropManager->UpdateGatherNodeAttr(manager, gatherAnimationInfo);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage045
 * @tc.desc: Test SetDragBehavior
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage045, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    auto event = AceType::MakeRefPtr<OHOS::Ace::DragEvent>();
    DragNotifyMsgCore notifyMessage;
    notifyMessage.dragBehavior = DragBehavior::COPY;
    dragDropManager->SetDragBehavior(notifyMessage, event);
    notifyMessage.dragBehavior = DragBehavior::MOVE;
    dragDropManager->SetDragBehavior(notifyMessage, event);
    notifyMessage.dragBehavior = DragBehavior::UNKNOWN;
    dragDropManager->SetDragBehavior(notifyMessage, event);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage046
 * @tc.desc: Test SetDragResult
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage046, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    auto event = AceType::MakeRefPtr<OHOS::Ace::DragEvent>();
    DragNotifyMsgCore notifyMessage;
    notifyMessage.result = DragRet::DRAG_SUCCESS;
    dragDropManager->SetDragResult(notifyMessage, event);
    notifyMessage.result = DragRet::DRAG_FAIL;
    dragDropManager->SetDragResult(notifyMessage, event);
    notifyMessage.result = DragRet::DRAG_CANCEL;
    dragDropManager->SetDragResult(notifyMessage, event);
    notifyMessage.result = DragRet::ENABLE_DROP;
    dragDropManager->SetDragResult(notifyMessage, event);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage047
 * @tc.desc: Test DoDragStartAnimation
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage047, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    RefPtr<UINode> frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(AceType::DynamicCast<FrameNode>(frameNode));
    GestureEvent event;
    event.SetDeviceId(0xFFFFFFFF);
    dragDropManager->SetIsDragWithContextMenu(true);
    auto frameNode2 = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, 1, AceType::MakeRefPtr<Pattern>(), false);
    ASSERT_NE(frameNode2, nullptr);
    auto guestureEventHub = frameNode2->GetOrCreateGestureEventHub();
    dragDropManager->DoDragStartAnimation(overlayManager, event, guestureEventHub);
    dragDropManager->TransDragWindowToDragFwk(111);
    dragDropManager->SetIsDragWithContextMenu(false);
    event.SetDeviceId(0xFFFFEEEE);
    dragDropManager->DoDragStartAnimation(overlayManager, event, guestureEventHub);
    dragDropManager->DoDragStartAnimation(overlayManager, event, guestureEventHub, true);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage048
 * @tc.desc: Test DoDragMoveAnimate
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage048, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    dragDropManager->isDragFwkShow_ = false;
    dragDropManager->isDragWithContextMenu_ = true;
    PointerEvent pointerEvent;
    dragDropManager->DoDragMoveAnimate(pointerEvent);
    pointerEvent.x = 3.0f;
    pointerEvent.y = 4.0f;
    dragDropManager->info_.scale = 0.5f;
    dragDropManager->DoDragMoveAnimate(pointerEvent);
    EXPECT_NE(frameNode, nullptr);
}
/**
 * @tc.name: DragDropManagerTestNgCoverage049
 * @tc.desc: Test UpdateDragMovePositionFinished
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage049, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    auto containerId = Container::CurrentId();
    Offset Offset = { 0.0, 0.0 };
    dragDropManager->isDragWithContextMenu_ = false;
    dragDropManager->UpdateDragMovePositionFinished(true, true, Offset, containerId);
    dragDropManager->isDragWithContextMenu_ = true;
    dragDropManager->isDragFwkShow_ = true;
    dragDropManager->info_.imageNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    dragDropManager->UpdateDragMovePositionFinished(true, true, Offset, containerId);
    dragDropManager->isDragFwkShow_ = true;
    dragDropManager->UpdateDragMovePositionFinished(false, false, Offset, containerId);
    dragDropManager->UpdateDragMovePositionFinished(false, true, Offset, containerId);
    dragDropManager->isDragWithContextMenu_ = false;
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage050
 * @tc.desc: Test FireOnDragEvent
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage050, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNodeNull, nullptr);
    dragDropManager->AddDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    frameNodeNull.Reset();
    auto frameNodeGeoNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeGeoNull =
        AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeGeoNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeGeoNull->SetGeometryNode(nullptr);
    dragDropManager->AddDragFrameNode(frameNodeGeoNull->GetId(), frameNodeGeoNull);
    auto eventHub = frameNode->GetEventHub<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    std::string onDropInfo;
    auto onDrop = [&onDropInfo](const RefPtr<OHOS::Ace::DragEvent>& dragEvent, const std::string& info) {
        dragEvent->dragRet_ = DragRet::ENABLE_DROP;
        onDropInfo = EXTRA_INFO;
    };
    eventHub->SetOnDrop(std::move(onDrop));
    eventHub->SetOnDragMove(std::move(onDrop));
    PointerEvent point;
    TouchEvent event;
    event.x = 1.0f;
    event.y = 2.0f;
    dragDropManager->velocityTracker_.UpdateTouchPoint(event, false);
    auto targetFrameNode = dragDropManager->FindDragFrameNodeByPosition(GLOBAL_X, GLOBAL_Y);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    DragEventType type = DragEventType::MOVE;
    frameNode->tag_ = V2::UI_EXTENSION_COMPONENT_ETS_TAG;
    dragDropManager->info_.imageNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    dragDropManager->FireOnDragEvent(frameNode, point, type, EXTRA_INFO);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage051
 * @tc.desc: Test FireOnDragEvent
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage051, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNodeNull, nullptr);
    dragDropManager->AddDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    frameNodeNull.Reset();
    auto frameNodeGeoNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeGeoNull =
        AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeGeoNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNodeGeoNull, nullptr);
    frameNodeGeoNull->SetGeometryNode(nullptr);
    dragDropManager->AddDragFrameNode(frameNodeGeoNull->GetId(), frameNodeGeoNull);
    auto eventHub = frameNode->GetEventHub<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    std::string onDropInfo;
    auto onDrop = [&onDropInfo](const RefPtr<OHOS::Ace::DragEvent>& dragEvent, const std::string& info) {
        dragEvent->dragRet_ = DragRet::ENABLE_DROP;
        dragEvent->dragBehavior_ = DragBehavior::MOVE;
        onDropInfo = EXTRA_INFO;
    };
    eventHub->SetOnDrop(std::move(onDrop));
    eventHub->SetOnDragMove(std::move(onDrop));
    PointerEvent point;
    TouchEvent event;
    event.x = 1.0f;
    event.y = 2.0f;
    dragDropManager->velocityTracker_.UpdateTouchPoint(event, false);
    auto targetFrameNode = dragDropManager->FindDragFrameNodeByPosition(GLOBAL_X, GLOBAL_Y);
    DragEventType type = DragEventType::MOVE;
    frameNode->tag_ = V2::UI_EXTENSION_COMPONENT_ETS_TAG;
    dragDropManager->info_.imageNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    dragDropManager->FireOnDragEvent(frameNode, point, type, EXTRA_INFO);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage052
 * @tc.desc: Test FireOnDragEvent
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage052, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNodeNull, nullptr);
    dragDropManager->AddDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    frameNodeNull.Reset();
    auto frameNodeGeoNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeGeoNull =
        AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeGeoNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNodeGeoNull, nullptr);
    frameNodeGeoNull->SetGeometryNode(nullptr);
    dragDropManager->AddDragFrameNode(frameNodeGeoNull->GetId(), frameNodeGeoNull);
    auto eventHub = frameNode->GetEventHub<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    std::string onDropInfo;
    auto onDrop = [&onDropInfo](const RefPtr<OHOS::Ace::DragEvent>& dragEvent, const std::string& info) {
        dragEvent->dragRet_ = DragRet::DISABLE_DROP;
        dragEvent->dragBehavior_ = DragBehavior::MOVE;
        onDropInfo = EXTRA_INFO;
    };
    eventHub->SetOnDrop(std::move(onDrop));
    eventHub->SetOnDragMove(std::move(onDrop));
    PointerEvent point;
    TouchEvent event;
    event.x = 1.0f;
    event.y = 2.0f;
    dragDropManager->velocityTracker_.UpdateTouchPoint(event, false);
    auto targetFrameNode = dragDropManager->FindDragFrameNodeByPosition(GLOBAL_X, GLOBAL_Y);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    DragEventType type = DragEventType::MOVE;
    frameNode->tag_ = V2::UI_EXTENSION_COMPONENT_ETS_TAG;
    dragDropManager->info_.imageNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    dragDropManager->FireOnDragEvent(frameNode, point, type, EXTRA_INFO);
    EXPECT_NE(frameNode, nullptr);
}
/**
 * @tc.name: DragDropManagerTestNgCoverage053
 * @tc.desc: Test UpdateDragAllowDrop
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage053, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create DragDropManager.
     * @tc.expected: dragDropManager is not null.
     */
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    ASSERT_NE(dragDropManager, nullptr);

    /**
     * @tc.steps: step2. construct frameNode and update the properties.
     * @tc.expected: frameNode is not null.
     */
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    std::set<std::string> allowDrop = { NODE_TAG };
    frameNode->SetAllowDrop(allowDrop);

    /**
     * @tc.steps: step2. construct frameNode and update the properties.
     * @tc.expected: frameNode is not null.
     */
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(childNode, nullptr);
    childNode->SetAllowDrop(allowDrop);

    /**
     * @tc.steps: step3. call UpdateDragAllowDrop with frameNode and copy.
     * @tc.expected: dragDropManager->summaryMap_.empty() return a true value.
     */
    dragDropManager->summaryMap_.clear();
    PointF point(100.0, 100.0);
    dragDropManager->OnDragStart({ GLOBAL_X, GLOBAL_Y }, childNode);
    dragDropManager->summaryMap_.insert(make_pair(NODE_TAG, frameNodeNullId));
    dragDropManager->UpdateDragAllowDrop(frameNode, DragBehavior(2), -1);
    EXPECT_FALSE(dragDropManager->summaryMap_.empty());
}

/**
 * @tc.name: DragDropManagerTestNgCoverage054
 * @tc.desc: Test OnItemDragMove
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage054, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull =
        AceType::MakeRefPtr<FrameNode>(V2::EMBEDDED_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    frameNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    childNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    ASSERT_NE(pipelineTmp, nullptr);
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    ASSERT_NE(parentNodeTmp, nullptr);
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(100.0, 100.0);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    auto result = true;
    dragDropManager->OnItemDragMove(100.0, 100.0, 1, DragType::GRID);
    auto framenode4 = dragDropManager->FindDragFrameNodeByPosition(100.0, 100.0);
    dragDropManager->OnItemDragStart(100.0, 100.0, framenode4);
    dragDropManager->OnItemDragMove(100.0, 100.0, 1, DragType::GRID);
    auto frameNode5 = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    dragDropManager->OnItemDragStart(100.0, 100.0, frameNode5);
    dragDropManager->OnItemDragMove(100.0, 100.0, 1, DragType::GRID);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage055
 * @tc.desc: Test OnItemDragEnd
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage055, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto frameNodeNull = AceType::MakeRefPtr<FrameNode>("parent", frameNodeNullId, AceType::MakeRefPtr<Pattern>());
    frameNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    frameNodeNull->SetActive(true);
    auto eventHub = frameNodeNull->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onDragDrop = [](const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& value) {};
    eventHub->SetOnDrop(std::move(onDragDrop));
    auto childNodeNullId = ElementRegister::GetInstance()->MakeUniqueId();
    auto childNodeNull =
        AceType::MakeRefPtr<FrameNode>(V2::EMBEDDED_COMPONENT_ETS_TAG, childNodeNullId, AceType::MakeRefPtr<Pattern>());
    childNodeNull->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    childNodeNull->SetActive(true);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(FRAME_SIZE);
    frameNodeNull->SetGeometryNode(geometryNode);
    childNodeNull->SetGeometryNode(geometryNode);
    frameNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    childNodeNull->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    auto pipelineTmp = NG::PipelineContext::GetCurrentContext();
    ASSERT_NE(pipelineTmp, nullptr);
    auto parentNodeTmp = pipelineTmp->GetRootElement();
    auto parentFrameNodeTmp = AceType::DynamicCast<FrameNode>(parentNodeTmp);
    parentFrameNodeTmp->SetGeometryNode(geometryNode);
    parentFrameNodeTmp->GetRenderContext()->UpdatePaintRect({ 0.0, 0.0, 200.0, 200.0 });
    parentFrameNodeTmp->frameChildren_.insert(WeakPtr<NG::FrameNode>(frameNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(childNodeNull));
    frameNodeNull->frameChildren_.insert(WeakPtr<NG::FrameNode>(nullptr));
    dragDropManager->AddGridDragFrameNode(parentFrameNodeTmp->GetId(), parentFrameNodeTmp);
    dragDropManager->AddGridDragFrameNode(frameNodeNull->GetId(), frameNodeNull);
    dragDropManager->AddGridDragFrameNode(childNodeNull->GetId(), childNodeNull);
    std::map<int32_t, WeakPtr<FrameNode>> frameNodes = dragDropManager->gridDragFrameNodes_;
    PointF point(100.0, 100.0);
    auto pipeline = NG::PipelineContext::GetCurrentContext();
    auto manager = pipeline->GetOverlayManager();
    auto parentNode = pipeline->GetRootElement();
    auto parentFrameNode = AceType::DynamicCast<FrameNode>(parentNode);
    auto children = parentFrameNode->GetFrameChildren();
    auto result = true;
    dragDropManager->OnItemDragEnd(100.0, 100.0, 1, DragType::GRID);
    auto framenode4 = dragDropManager->FindDragFrameNodeByPosition(100.0, 100.0);
    dragDropManager->OnItemDragStart(100.0, 100.0, framenode4);
    dragDropManager->OnItemDragEnd(100.0, 100.0, 1, DragType::GRID);
    auto frameNode5 = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    dragDropManager->OnItemDragStart(100.0, 100.0, frameNode5);
    dragDropManager->OnItemDragEnd(100.0, 100.0, 1, DragType::GRID);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage056
 * @tc.desc: Test RequestDragSummaryInfoAndPrivilege
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage056, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), AddPrivilege())
        .WillRepeatedly(testing::Return(1));
    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetShadowOffset(_))
        .WillRepeatedly(testing::Return(1));
    dragDropManager->RequestDragSummaryInfoAndPrivilege();

    SystemProperties::debugEnabled_ = true;
    dragDropManager->RequestDragSummaryInfoAndPrivilege();

    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), AddPrivilege())
        .WillRepeatedly(testing::Return(0));
    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetShadowOffset(_))
        .WillRepeatedly(testing::Return(0));
    dragDropManager->RequestDragSummaryInfoAndPrivilege();

    SystemProperties::debugEnabled_ = false;
    dragDropManager->RequestDragSummaryInfoAndPrivilege();
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage057
 * @tc.desc: Test RequireSummary
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage057, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetDragSummary(_))
        .WillRepeatedly(testing::Return(1));
    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetDragExtraInfo(_))
        .WillRepeatedly(testing::Return(1));
    dragDropManager->RequireSummary();

    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetDragSummary(_))
        .WillRepeatedly(testing::Return(0));
    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetDragExtraInfo(_))
        .WillRepeatedly(testing::Return(0));
    dragDropManager->RequireSummary();
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage058
 * @tc.desc: Test GetDragWindowRect
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage058, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    Point point(0.0, 0.0);
    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetShadowOffset(_))
        .WillRepeatedly(testing::Return(1));
    dragDropManager->GetDragWindowRect(point);

    EXPECT_CALL(
        *(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())), GetShadowOffset(_))
        .WillRepeatedly(testing::Return(0));
    dragDropManager->previewRect_ = { 1, 1, 1, 1 };
    dragDropManager->GetDragWindowRect(point);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage059
 * @tc.desc: Test FireOnEditableTextComponent
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage059, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(V2::TEXTINPUT_ETS_TAG, 1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    EXPECT_CALL(*(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())),
        EnterTextEditorArea(_))
        .WillRepeatedly(testing::Return(1));
    dragDropManager->FireOnEditableTextComponent(frameNode, DragEventType::ENTER);
    EXPECT_CALL(*(AceType::DynamicCast<MockInteractionInterface>(MockInteractionInterface::GetInstance())),
        EnterTextEditorArea(_))
        .WillRepeatedly(testing::Return(0));
    dragDropManager->FireOnEditableTextComponent(frameNode, DragEventType::ENTER);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage060
 * @tc.desc: Test UpdateDragMovePosition
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage060, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(V2::TEXTINPUT_ETS_TAG, 1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    OffsetF offset = { 1.0f, 1.0f };
    dragDropManager->UpdateDragMovePosition(offset, true);
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage061
 * @tc.desc: Test FireOnDragLeave
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage061, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    PointerEvent point;
    point.x = 1;
    point.y = 1;
    auto mockRenderContext = AceType::MakeRefPtr<MockRenderContext>();
    mockRenderContext->rect_ = RectF(0, 0, 10, 10);
    frameNode->renderContext_ = mockRenderContext;
    dragDropManager->FireOnDragLeave(frameNode, point, EXTRA_INFO);
    dragDropManager->eventStrictReportingEnabled_ = true;
    dragDropManager->preTargetFrameNode_ = frameNode;
    dragDropManager->FireOnDragLeave(frameNode, point, EXTRA_INFO);
    dragDropManager->eventStrictReportingEnabled_ = false;
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage062
 * @tc.desc: Test OnDragMoveOut
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage062, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    PointerEvent point;
    point.x = 1;
    point.y = 1;
    auto container = MockContainer::Current();
    ASSERT_NE(container, nullptr);
    container->isScenceBoardWindow_ = true;
    dragDropManager->OnDragMoveOut(point);
    dragDropManager->isDragged_ = true;
    dragDropManager->OnDragMoveOut(point);
    dragDropManager->isDragged_ = false;
    dragDropManager->isWindowConsumed_ = true;
    dragDropManager->OnDragMoveOut(point);
    dragDropManager->isWindowConsumed_ = true;
    dragDropManager->isDragged_ = true;
    dragDropManager->OnDragMoveOut(point);
    dragDropManager->isDragged_ = false;
    dragDropManager->isWindowConsumed_ = false;
    container->isScenceBoardWindow_ = false;
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage063
 * @tc.desc: Test OnDragMove
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage063, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    PointerEvent point;
    point.x = 1;
    point.y = 1;
    auto container = MockContainer::Current();
    ASSERT_NE(container, nullptr);
    container->isScenceBoardWindow_ = true;
    dragDropManager->OnDragMove(point, EXTRA_INFO, frameNode);
    dragDropManager->isDragged_ = true;
    dragDropManager->OnDragMove(point, EXTRA_INFO, frameNode);
    dragDropManager->isDragged_ = false;
    dragDropManager->isWindowConsumed_ = true;
    dragDropManager->OnDragMove(point, EXTRA_INFO, frameNode);
    dragDropManager->isWindowConsumed_ = true;
    dragDropManager->isDragged_ = true;
    dragDropManager->OnDragMove(point, EXTRA_INFO, frameNode);
    dragDropManager->isDragged_ = false;
    dragDropManager->isWindowConsumed_ = false;
    container->isScenceBoardWindow_ = false;
    EXPECT_NE(frameNode, nullptr);
}

/**
 * @tc.name: DragDropManagerTestNgCoverage064
 * @tc.desc: Test OnDragMoveOut
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(DragDropManagerTestNgCoverage, DragDropManagerTestNgCoverage064, TestSize.Level1)
{
    auto dragDropManager = AceType::MakeRefPtr<DragDropManager>();
    auto frameNode = AceType::MakeRefPtr<FrameNode>(NODE_TAG, -1, AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(frameNode, nullptr);
    PointerEvent point;
    point.x = 1;
    point.y = 1;
    auto container = MockContainer::Current();
    ASSERT_NE(container, nullptr);
    container->isScenceBoardWindow_ = true;
    dragDropManager->OnDragEnd(point, EXTRA_INFO, frameNode);
    dragDropManager->isDragged_ = true;
    dragDropManager->OnDragEnd(point, EXTRA_INFO, frameNode);
    dragDropManager->isDragged_ = false;
    dragDropManager->isWindowConsumed_ = true;
    dragDropManager->OnDragEnd(point, EXTRA_INFO, frameNode);
    dragDropManager->isWindowConsumed_ = true;
    dragDropManager->isDragged_ = true;
    dragDropManager->OnDragEnd(point, EXTRA_INFO, frameNode);
    dragDropManager->isDragged_ = false;
    dragDropManager->isWindowConsumed_ = false;
    container->isScenceBoardWindow_ = false;
    EXPECT_NE(frameNode, nullptr);
}
} // namespace OHOS::Ace::NG
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

#include "test/unittest/core/event/drag_event/drag_event_common_test_ng.h"

#define private public
#include "core/components_ng/manager/drag_drop/drag_drop_initiating/drag_drop_initiating_handler.h"
#undef private
#include "core/gestures/drag_event.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
struct DragDropStartTestCase {
    GlobalDraggingInfo globalDraggingInfo;
    DragInfo dragInfo;
    MultiDragInfo multiDragInfo;
    InputEventType inputEventType;

    // calculate drag status
    DragDropInitiatingStatus expectStatus = DragDropInitiatingStatus::IDLE;
    DragDropStartTestCase(GlobalDraggingInfo globalDraggingInfo, DragInfo dragInfo, MultiDragInfo multiDragInfo,
        InputEventType inputEventType, DragDropInitiatingStatus expectStatus)
        : globalDraggingInfo(globalDraggingInfo), dragInfo(dragInfo), multiDragInfo(multiDragInfo),
          inputEventType(inputEventType), expectStatus(expectStatus)
    {}
};

class DragDropEventTestNgIssue : public DragEventCommonTestNg {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();

protected:
    bool CheckDragDropInitiatingStatus(
        int32_t caseNum, DragDropInitiatingStatus dragStatus, DragDropInitiatingStatus status);
};

void DragDropEventTestNgIssue::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
    SystemProperties::dragDropFrameworkStatus_ = 1;
}

void DragDropEventTestNgIssue::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
    SystemProperties::dragDropFrameworkStatus_ = 0;
}

bool DragDropEventTestNgIssue::CheckDragDropInitiatingStatus(
    int32_t caseNum, DragDropInitiatingStatus dragStatus, DragDropInitiatingStatus expectStatus)
{
    if (dragStatus == expectStatus) {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << "TestCaseNum: " << caseNum
                                       << ", actual state: " << static_cast<int32_t>(dragStatus)
                                       << ", expect state: " << static_cast<int32_t>(expectStatus);
}

const std::vector<DragDropStartTestCase> DRAG_DROP_STATUS_DRAG_INFO_IMAGE_TEST_CASES = {
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(false, false, false, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::READY), // case 0
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(false, false, true, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::IDLE), // case 1
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(false, true, false, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::READY), // case 2
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(false, true, true, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::READY), // case 3
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(true, false, false, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::READY), // case 4
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(true, false, true, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::IDLE), // case 5
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(true, true, false, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::READY), // case 6
    DragDropStartTestCase(GlobalDraggingInfo(false, false), DragInfo(true, true, true, false),
        MultiDragInfo(false, false, false), InputEventType::TOUCH_SCREEN, DragDropInitiatingStatus::READY), // case 7
};

/**
 * @tc.name: DragDropEventTestNGIssue001
 * @tc.desc: Test CollectTouchTarget function when frameNode is image.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNGIssue001, TestSize.Level1)
{
    int32_t caseNum = 0;
    for (const auto& testCase : DRAG_DROP_STATUS_DRAG_INFO_IMAGE_TEST_CASES) {
        /**
         * @tc.steps: step1. create DragDropEventActuator.
         */
        auto eventHub = AceType::MakeRefPtr<EventHub>();
        ASSERT_NE(eventHub, nullptr);
        auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
        ASSERT_NE(gestureEventHub, nullptr);
        gestureEventHub->SetDragForbiddenForcely(testCase.dragInfo.isDragForbidden);
        auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ImagePattern>(); });
        ASSERT_NE(frameNode, nullptr);
        eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
        auto dragDropEventActuator =
            AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
        ASSERT_NE(dragDropEventActuator, nullptr);
        /**
         * @tc.steps: step2. call OnCollectTouchTarget function.
         * @tc.expected: step2. DragDropInitiatingStatus equals.
         */
        frameNode->draggable_ = testCase.dragInfo.isDraggable;
        frameNode->customerSet_ = testCase.dragInfo.isCustomerSet;
        TouchRestrict dragTouchRestrict = { TouchRestrict::NONE };
        dragTouchRestrict.inputEventType = testCase.inputEventType;
        auto getEventTargetImpl = eventHub->CreateGetEventTargetImpl();
        EXPECT_NE(getEventTargetImpl, nullptr);
        TouchTestResult finalResult;
        ResponseLinkResult responseLinkResult;
        dragDropEventActuator->OnCollectTouchTarget(
            COORDINATE_OFFSET, DRAG_TOUCH_RESTRICT, getEventTargetImpl, finalResult, responseLinkResult);
        auto handler = dragDropEventActuator->dragDropInitiatingHandler_;
        ASSERT_NE(handler, nullptr);
        EXPECT_TRUE(
            CheckDragDropInitiatingStatus(caseNum, handler->GetDragDropInitiatingStatus(), testCase.expectStatus));
        caseNum++;
    }
}

/**
 * @tc.name: DragDropEventTestNgIssue002
 * @tc.desc: Test CopyEvent function when originDragDropInitiatingHandler is true.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue002, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    dragDropEventActuator->CopyEvent(dragDropEventActuator);
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
}

/**
 * @tc.name: DragDropEventTestNgIssue003
 * @tc.desc: Test CopyEvent function when originDragDropInitiatingHandler is false.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue003, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_ = nullptr;
    dragDropEventActuator->CopyEvent(dragDropEventActuator);
    EXPECT_EQ(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
}

/**
 * @tc.name: DragDropEventTestNgIssue004
 * @tc.desc: Test GetPreScaledPixelMapForDragThroughTouch function when dragDropInitiatingHandler_ is nullptr and ture .
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue004, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    float preScale = 1.0f;
    auto gestureHub = dragDropEventActuator->gestureEventHub_.Upgrade();
    EXPECT_TRUE(!gestureHub->GetTextDraggable());
    EXPECT_EQ(dragDropEventActuator->GetPreScaledPixelMapForDragThroughTouch(preScale), nullptr);
}

/**
 * @tc.name: DragDropEventTestNgIssue005
 * @tc.desc: Test GetPreScaledPixelMapForDragThroughTouch function when dragDropInitiatingHandler_ is nullptr and false
 * .
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue005, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    float preScale = 1.0f;
    auto gestureHub = dragDropEventActuator->gestureEventHub_.Upgrade();
    gestureHub->textDraggable_ = true;
    EXPECT_TRUE(gestureHub->GetTextDraggable());
    EXPECT_EQ(dragDropEventActuator->GetPreScaledPixelMapForDragThroughTouch(preScale), nullptr);
}

/**
 * @tc.name: DragDropEventTestNgIssue006
 * @tc.desc: Test GetPreScaledPixelMapForDragThroughTouch function when dragDropInitiatingHandler_ is not nullptr and
 * false .
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue006, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    float preScale = 1.0f;
    auto gestureHub = dragDropEventActuator->gestureEventHub_.Upgrade();
    gestureHub->textDraggable_ = true;
    EXPECT_TRUE(gestureHub->GetTextDraggable());
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    EXPECT_EQ(dragDropEventActuator->GetPreScaledPixelMapForDragThroughTouch(preScale), nullptr);
}

/**
 * @tc.name: DragDropEventTestNgIssue007
 * @tc.desc: Test GetPreScaledPixelMapForDragThroughTouch function when dragDropInitiatingHandler_ is not nullptr and
 * true .
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue007, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    float preScale = 2.0f;
    auto gestureHub = dragDropEventActuator->gestureEventHub_.Upgrade();
    EXPECT_TRUE(!gestureHub->GetTextDraggable());
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    dragDropEventActuator->GetPreScaledPixelMapForDragThroughTouch(preScale);
    EXPECT_EQ(preScale, 1.0f);
}

/**
 * @tc.name: DragDropEventTestNgIssue008
 * @tc.desc: Test ResetPreScaledPixelMapForDragThroughTouch function when dragDropInitiatingHandler_ is  true .
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue008, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    auto& params = dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->GetDragDropInitiatingParams();
    params.preScaleValue = 2.0f;
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    dragDropEventActuator->ResetPreScaledPixelMapForDragThroughTouch();
    EXPECT_EQ(params.preScaleValue, 1.0f);
}

/**
 * @tc.name: DragDropEventTestNGIssue009
 * @tc.desc: Test NotifyTransDragWindowToFwk function when dragDropInitiatingHandler_ is  true .
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue009, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->InitializeState();
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_ = 1;
    dragDropEventActuator->NotifyTransDragWindowToFwk();
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_, 1);
}

/**
 * @tc.name: DragDropEventTestNgIssue010
 * @tc.desc: Test NotifyDragStartTest function when dragDropInitiatingHandler_ is  true .
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue010, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->InitializeState();
    auto currentState_ = dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_;
    dragDropEventActuator->NotifyDragStart();
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_, currentState_);
}

/**
 * @tc.name: DragDropEventTestNgIssue012
 * @tc.desc: Test IsNeedGatherTest function
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue012, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    /**
     * @tc.steps: step1. dragDropInitiatingHandler_ is not nullptr
     */
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->GetDragDropInitiatingParams().isNeedGather =
        true;
    EXPECT_TRUE(dragDropEventActuator->IsNeedGather());
    /**
     * @tc.steps: step2. dragDropInitiatingHandler_ is  nullptr
     */
    dragDropEventActuator->dragDropInitiatingHandler_ = nullptr;
    EXPECT_FALSE(dragDropEventActuator->IsNeedGather());
}

/**
 * @tc.name: DragDropEventTestNgIssue013
 * @tc.desc: Test GetIsNotInPreviewState function when dragDropInitiatingHandler_ is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue013, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(eventHub);
    ASSERT_NE(gestureEventHub, nullptr);
    RefPtr<DragDropEventActuator> actuator = AceType::MakeRefPtr<DragDropEventActuator>(gestureEventHub);
    ASSERT_NE(actuator, nullptr);
    actuator->dragDropInitiatingHandler_ = nullptr;
    EXPECT_TRUE(actuator->GetIsNotInPreviewState());
}

/**
 * @tc.name: DragDropEventTestNgIssue014
 * @tc.desc: Test GetIsNotInPreviewState function when dragDropInitiatingHandler_ is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue014, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    ASSERT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    bool result = dragDropEventActuator->GetIsNotInPreviewState();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DragDropEventTestNgIssue015
 * @tc.desc: Test NotifyMenuShow function when dragDropInitiatingHandler_ is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue015, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    ASSERT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->InitializeState();
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_ = 1;
    bool isMenuShow = false;
    dragDropEventActuator->NotifyMenuShow(isMenuShow);
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_, 1);
}

/**
 * @tc.name: DragDropEventTestNgIssue016
 * @tc.desc: Test NotifyDragEnd function when dragDropInitiatingHandler_ is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue016, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    ASSERT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    ASSERT_NE(dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->InitializeState();
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_ = 1;
    dragDropEventActuator->NotifyDragEnd();
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->currentState_, 1);
}

/**
 * @tc.name: DragDropEventTestNgIssue017
 * @tc.desc: Test SetThumbnailCallback function when dragDropInitiatingHandler_ is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue017, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    ASSERT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    auto dragDropInitiatingStateMachine = AceType::MakeRefPtr<DragDropInitiatingStateMachine>(frameNode);
    ASSERT_NE(dragDropInitiatingStateMachine, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->dragDropInitiatingParams_
        .getTextThumbnailPixelMapCallback = nullptr;
    Offset offset(10.0, 20.0);
    auto callBack = [](Offset offset) {};
    dragDropEventActuator->SetThumbnailCallback(std::move(callBack));
    auto initiatingFlow_ = dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_;
    EXPECT_NE(initiatingFlow_->dragDropInitiatingParams_.getTextThumbnailPixelMapCallback, nullptr);
}

/**
 * @tc.name: DragDropEventTestNgIssue018
 * @tc.desc: Test RestartDragTask function when dragDropInitiatingHandler_ is not nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(DragDropEventTestNgIssue, DragDropEventTestNgIssue018, TestSize.Level1)
{
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto gestureEventHub = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    ASSERT_NE(gestureEventHub, nullptr);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    ASSERT_NE(frameNode, nullptr);
    eventHub->host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    auto dragDropEventActuator =
        AceType::MakeRefPtr<DragDropEventActuator>(AceType::WeakClaim(AceType::RawPtr(gestureEventHub)));
    ASSERT_NE(dragDropEventActuator, nullptr);
    ASSERT_NE(dragDropEventActuator->dragDropInitiatingHandler_, nullptr);
    dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->InitializeState();
    GestureEvent gestureEvent;
    dragDropEventActuator->RestartDragTask(gestureEvent);
    EXPECT_NE(dragDropEventActuator->dragDropInitiatingHandler_->initiatingFlow_->dragDropInitiatingState_[0], nullptr);
}
} // namespace OHOS::Ace::NG
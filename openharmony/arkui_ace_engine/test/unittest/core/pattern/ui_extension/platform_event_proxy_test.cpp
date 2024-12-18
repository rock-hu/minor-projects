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
#include "base/memory/ace_type.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "adapter/ohos/osal/want_wrap_ohos.h"
#include "core/components_ng/pattern/ui_extension/isolated_pattern.h"
#include "core/components_ng/pattern/ui_extension/platform_event_proxy.h"
#include "core/components_ng/pattern/ui_extension/platform_pattern.h"
#include "frameworks/core/event/pointer_event.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
    const std::string ISOLATED_COMPONENT_ETS_TAG = "IsolatedComponent";
    const std::string EVENT_PROXT_EMPTY = "[]";
    const std::string EVENT_PROXT_CLICK = "[ClickEvent]";
    const std::string EVENT_PROXT_LONG_PRESS = "[LongPressEvent]";
    const std::string EVENT_PROXT_PAN_GESTURE_VERTICAL = "[PanGestureVertical]";
    const std::string EVENT_PROXT_PAN_GESTURE_HORIZONTAL = "[PanGestureHorizontal]";
    const std::string EVENT_PROXT_PAN_GESTURE_ALL = "[PanGestureVertical, PanGestureHorizontal]";
} // namespace

class PlatformEventProxyTestNg : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

    bool CheckClickEvent(const RefPtr<FrameNode> &host);
    bool CheckClickEvent(const RefPtr<PlatformEventProxy> &eventProxy);
    bool CheckLongPressEvent(const RefPtr<FrameNode> &host);
    bool CheckLongPressEvent(const RefPtr<PlatformEventProxy> &eventProxy);
    bool CheckPanGestureEvent(const RefPtr<FrameNode> &host);
    bool CheckPanGestureEvent(const RefPtr<PlatformEventProxy> &eventProxy, uint32_t panDirection);
    bool CheckPanGestureVerticalEvent(const RefPtr<PlatformEventProxy> &eventProxy);
    bool CheckPanGestureHorizontalEvent(const RefPtr<PlatformEventProxy> &eventProxy);
};

void PlatformEventProxyTestNg::SetUp() {}

void PlatformEventProxyTestNg::TearDown() {}

bool PlatformEventProxyTestNg::CheckClickEvent(
    const RefPtr<FrameNode> &host)
{
    CHECK_NULL_RETURN(host, true);
    auto hub = host->GetEventHub<EventHub>();
    CHECK_NULL_RETURN(hub, true);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(gestureHub, true);
    return !gestureHub->IsClickEventsEmpty();
}

bool PlatformEventProxyTestNg::CheckClickEvent(
    const RefPtr<PlatformEventProxy> &eventProxy)
{
    CHECK_NULL_RETURN(eventProxy, true);
    return eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_CLICK));
}

bool PlatformEventProxyTestNg::CheckLongPressEvent(
    const RefPtr<FrameNode> &host)
{
    CHECK_NULL_RETURN(host, true);
    auto hub = host->GetEventHub<EventHub>();
    CHECK_NULL_RETURN(hub, true);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(gestureHub, true);
    return gestureHub->IsLongClickable();
}

bool PlatformEventProxyTestNg::CheckLongPressEvent(
    const RefPtr<PlatformEventProxy> &eventProxy)
{
    CHECK_NULL_RETURN(eventProxy, true);
    return eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS));
}

bool PlatformEventProxyTestNg::CheckPanGestureEvent(
    const RefPtr<FrameNode> &host)
{
    CHECK_NULL_RETURN(host, true);
    auto hub = host->GetEventHub<EventHub>();
    CHECK_NULL_RETURN(hub, true);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(gestureHub, true);
    return !gestureHub->IsPanEventEmpty();
}

bool PlatformEventProxyTestNg::CheckPanGestureEvent(
    const RefPtr<PlatformEventProxy> &eventProxy, uint32_t panDirection)
{
    CHECK_NULL_RETURN(eventProxy, true);
    bool ret = true;
    switch (panDirection) {
        case PanDirection::VERTICAL:
            ret = eventProxy->HasEventProxy(
                static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL));
            break;
        case PanDirection::HORIZONTAL:
            ret = eventProxy->HasEventProxy(
                static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL));
            break;
        case PanDirection::ALL:
            ret = eventProxy->HasEventProxy(
                static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL
                | EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL));
            break;
        default:
            LOGI("panDirection is Node");
            break;
    }

    return ret;
}

bool PlatformEventProxyTestNg::CheckPanGestureVerticalEvent(
    const RefPtr<PlatformEventProxy> &eventProxy)
{
    return CheckPanGestureEvent(eventProxy, PanDirection::VERTICAL);
}

bool PlatformEventProxyTestNg::CheckPanGestureHorizontalEvent(
    const RefPtr<PlatformEventProxy> &eventProxy)
{
    return CheckPanGestureEvent(eventProxy, PanDirection::HORIZONTAL);
}

/**
 * @tc.name: AddEventProxy001
 * @tc.desc: Test PlatformEventProxy SetClickEventProxy
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId, []() {
            return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    ASSERT_EQ(CheckClickEvent(isolatedNode), false);
    ASSERT_EQ(CheckClickEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);

    /**
     * @tc.steps: step2. First add
     */
    EventProxyResultCode code = eventProxy->SetClickEventProxy(gestureHub, true);
    ASSERT_EQ(code, EventProxyResultCode::ADD_WHEN_ADDING);
    ASSERT_EQ(CheckClickEvent(isolatedNode), true);
    ASSERT_EQ(CheckClickEvent(eventProxy), true);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_CLICK);

    /**
     * @tc.steps: step3. Second add
     */
    code = eventProxy->SetClickEventProxy(gestureHub, true);
    ASSERT_EQ(code, EventProxyResultCode::EXIST_WHEN_ADDING);
    ASSERT_EQ(CheckClickEvent(isolatedNode), true);
    ASSERT_EQ(CheckClickEvent(eventProxy), true);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_CLICK);

    /**
     * @tc.steps: step4. First Delete
     */
    code = eventProxy->SetClickEventProxy(gestureHub, false);
    ASSERT_EQ(code, EventProxyResultCode::REMOVE_WHEN_DELETE);
    ASSERT_EQ(CheckClickEvent(isolatedNode), false);
    ASSERT_EQ(CheckClickEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);

    /**
     * @tc.steps: step5. Second Delete
     */
    code = eventProxy->SetClickEventProxy(gestureHub, false);
    ASSERT_EQ(code, EventProxyResultCode::NO_EXIST_WHEN_DELETE);
    ASSERT_EQ(CheckClickEvent(isolatedNode), false);
    ASSERT_EQ(CheckClickEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
#endif
}

/**
 * @tc.name: AddEventProxy002
 * @tc.desc: Test PlatformEventProxy SetLongPressEventProxy
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy002, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId,
        []() { return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    ASSERT_EQ(CheckLongPressEvent(isolatedNode), false);
    ASSERT_EQ(CheckLongPressEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);

    /**
     * @tc.steps: step2. First add
     */
    EventProxyResultCode code = eventProxy->SetLongPressEventProxy(gestureHub, true);
    ASSERT_EQ(code, EventProxyResultCode::ADD_WHEN_ADDING);
    ASSERT_EQ(CheckLongPressEvent(isolatedNode), true);
    ASSERT_EQ(CheckLongPressEvent(eventProxy), true);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_LONG_PRESS);

    /**
     * @tc.steps: step3. Second add
     */
    code = eventProxy->SetLongPressEventProxy(gestureHub, true);
    ASSERT_EQ(code, EventProxyResultCode::EXIST_WHEN_ADDING);
    ASSERT_EQ(CheckLongPressEvent(isolatedNode), true);
    ASSERT_EQ(CheckLongPressEvent(eventProxy), true);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_LONG_PRESS);

    /**
     * @tc.steps: step4. First Delete
     */
    code = eventProxy->SetLongPressEventProxy(gestureHub, false);
    ASSERT_EQ(code, EventProxyResultCode::REMOVE_WHEN_DELETE);
    ASSERT_EQ(CheckLongPressEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);

    /**
     * @tc.steps: step4. Second Delete
     */
    code = eventProxy->SetLongPressEventProxy(gestureHub, false);
    ASSERT_EQ(code, EventProxyResultCode::NO_EXIST_WHEN_DELETE);
    ASSERT_EQ(CheckLongPressEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
#endif
}

/**
 * @tc.name: AddEventProxy003
 * @tc.desc: Test PlatformEventProxy SetPanGestureEventProxy Vertical
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy003, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId, []() {
            return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    ASSERT_EQ(CheckPanGestureEvent(isolatedNode), false);
    ASSERT_EQ(CheckPanGestureVerticalEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step2. SetPanGestureEventProxy EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL
     */
    EventProxyResultCode code = eventProxy->SetPanGestureEventProxy(gestureHub,
        eventProxy->GetPanDirection(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL));
    ASSERT_EQ(code, EventProxyResultCode::ADD_WHEN_ADDING);
    ASSERT_EQ(CheckPanGestureEvent(isolatedNode), true);
    ASSERT_EQ(CheckPanGestureVerticalEvent(eventProxy), true);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_PAN_GESTURE_VERTICAL);
    /**
     * @tc.steps: step3. SetPanGestureEventProxy EventProxyFlag::EVENT_NONE
     */
    code = eventProxy->SetPanGestureEventProxy(gestureHub,
        eventProxy->GetPanDirection(EventProxyFlag::EVENT_NONE));
    ASSERT_EQ(code, EventProxyResultCode::REMOVE_WHEN_DELETE);
    ASSERT_EQ(CheckPanGestureEvent(isolatedNode), false);
    ASSERT_EQ(CheckPanGestureVerticalEvent(eventProxy), false);
    EXPECT_NE(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_PAN_GESTURE_VERTICAL);
#endif
}

/**
 * @tc.name: AddEventProxy004
 * @tc.desc: Test PlatformEventProxy SetPanGestureEventProxy Horizontal
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy004, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId, []() {
            return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    ASSERT_EQ(CheckPanGestureEvent(isolatedNode), false);
    ASSERT_EQ(CheckPanGestureHorizontalEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step2. SetPanGestureEventProxy EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL
     */
    EventProxyResultCode code = eventProxy->SetPanGestureEventProxy(gestureHub,
        eventProxy->GetPanDirection(EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL));
    ASSERT_EQ(code, EventProxyResultCode::ADD_WHEN_ADDING);
    ASSERT_EQ(CheckPanGestureEvent(isolatedNode), true);
    ASSERT_EQ(CheckPanGestureHorizontalEvent(eventProxy), true);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_PAN_GESTURE_HORIZONTAL);
    /**
     * @tc.steps: step3. SetPanGestureEventProxy EventProxyFlag::EVENT_NONE
     */
    code = eventProxy->SetPanGestureEventProxy(gestureHub,
        eventProxy->GetPanDirection(EventProxyFlag::EVENT_NONE));
    ASSERT_EQ(code, EventProxyResultCode::REMOVE_WHEN_DELETE);
    ASSERT_EQ(CheckPanGestureEvent(isolatedNode), false);
    ASSERT_EQ(CheckPanGestureHorizontalEvent(eventProxy), false);
    EXPECT_NE(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_PAN_GESTURE_HORIZONTAL);
#endif
}

/**
 * @tc.name: AddEventProxy005
 * @tc.desc: Test PlatformEventProxy GetPanDirection
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy005, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    /**
     * @tc.steps: step1. Test EventProxyFlag::EVENT_NONE
     */
    uint32_t panDirection = eventProxy->GetPanDirection(EventProxyFlag::EVENT_NONE);
    ASSERT_EQ(panDirection, PanDirection::NONE);
    /**
     * @tc.steps: step2. Test EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL
     */
    panDirection = eventProxy->GetPanDirection(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL);
    ASSERT_EQ(panDirection, PanDirection::VERTICAL);
    /**
     * @tc.steps: step3. Test EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL
     */
    panDirection = eventProxy->GetPanDirection(EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL);
    ASSERT_EQ(panDirection, PanDirection::HORIZONTAL);
    /**
     * @tc.steps: step4. Test EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL
     *  and EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL
     */
    panDirection = eventProxy->GetPanDirection(EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL
        | EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL);
    ASSERT_EQ(panDirection, PanDirection::ALL);
#endif
}

/**
 * @tc.name: AddEventProxy006
 * @tc.desc: Test PlatformEventProxy SetEventProxyFlag Normal Flag
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy006, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId,
        []() { return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step2. Only click
     */
    eventProxy->SetEventProxyFlag(1);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), false);
    /**
     * @tc.steps: step3. Only longPress
     */
    eventProxy->SetEventProxyFlag(2);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), false);
    /**
     * @tc.steps: step4. click、longPress
     */
    eventProxy->SetEventProxyFlag(3);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), false);
    /**
     * @tc.steps: step5. Only panGestureVertical
     */
    eventProxy->SetEventProxyFlag(4);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), true);
#endif
}


/**
 * @tc.name: AddEventProxy007
 * @tc.desc: Test PlatformEventProxy SetEventProxyFlag Normal Flag
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy007, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId,
        []() { return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step1. click、panGestureVertical
     */
    eventProxy->SetEventProxyFlag(5);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), true);
    /**
     * @tc.steps: step2. longPress、panGestureVertical
     */
    eventProxy->SetEventProxyFlag(6);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), true);
    /**
     * @tc.steps: step3. click、longPress、panGestureVertical
     */
    eventProxy->SetEventProxyFlag(7);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), true);
    /**
     * @tc.steps: step4. panGestureHorizontal
     */
    eventProxy->SetEventProxyFlag(8);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL)), true);
#endif
}

/**
 * @tc.name: AddEventProxy008
 * @tc.desc: Test PlatformEventProxy Delete EventProxy
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy008, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId,
        []() { return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step2. All
     */
    eventProxy->SetEventProxyFlag(15);
    EXPECT_EQ(eventProxy->HasEventProxy(15), true);

    /**
     * @tc.steps: step3. Delete LongPress
     */
    eventProxy->SetEventProxyFlag(5);
    EXPECT_EQ(eventProxy->HasEventProxy(5), true);

    /**
     * @tc.steps: step4. Delete All
     */
    eventProxy->SetEventProxyFlag(0);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
#endif
}

/**
 * @tc.name: AddEventProxy009
 * @tc.desc: Test PlatformEventProxy Exception Num
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy009, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId,
        []() { return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step2. 16
     */
    eventProxy->SetEventProxyFlag(16);
    EXPECT_EQ(eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step4. 17
     */
    eventProxy->SetEventProxyFlag(17);
    EXPECT_EQ(eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_CLICK)), true);
    EXPECT_EQ(eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL)), false);
    EXPECT_EQ(eventProxy->HasEventProxy(
        static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL)), false);
#endif
}

/**
 * @tc.name: AddEventProxy010
 * @tc.desc: Test PlatformEventProxy GetCurEventProxyToString
 * @tc.type: FUNC
 */
HWTEST_F(PlatformEventProxyTestNg, AddEventProxy010, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    /**
     * @tc.steps: step1. construct a IsolatedComponent Node
     */
    auto isolatedNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto isolatedNode = FrameNode::GetOrCreateFrameNode(
        ISOLATED_COMPONENT_ETS_TAG, isolatedNodeId, []() {
            return AceType::MakeRefPtr<IsolatedPattern>(); });
    CHECK_NULL_VOID(isolatedNode);
    RefPtr<PlatformEventProxy> eventProxy = AceType::MakeRefPtr<PlatformEventProxy>();
    CHECK_NULL_VOID(eventProxy);
    eventProxy->SetHostNode(isolatedNode);
    auto hub = isolatedNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    ASSERT_EQ(CheckClickEvent(isolatedNode), false);
    ASSERT_EQ(CheckClickEvent(eventProxy), false);
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_EMPTY);
    /**
     * @tc.steps: step2. Test click、longPress、PanGesture and so on
     */
    eventProxy->SetEventProxyFlag(static_cast<int32_t>(EventProxyFlag::EVENT_CLICK));
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_CLICK);

    eventProxy->SetEventProxyFlag(static_cast<int32_t>(EventProxyFlag::EVENT_LONG_PRESS));
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_LONG_PRESS);

    eventProxy->SetEventProxyFlag(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL));
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_PAN_GESTURE_VERTICAL);

    eventProxy->SetEventProxyFlag(static_cast<int32_t>(EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL));
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_PAN_GESTURE_HORIZONTAL);

    eventProxy->SetEventProxyFlag(static_cast<int32_t>(
        EventProxyFlag::EVENT_PAN_GESTURE_VERTICAL | EventProxyFlag::EVENT_PAN_GESTURE_HORIZONTAL));
    EXPECT_EQ(eventProxy->GetCurEventProxyToString(), EVENT_PROXT_PAN_GESTURE_ALL);
#endif
}
} // namespace OHOS::Ace::NG

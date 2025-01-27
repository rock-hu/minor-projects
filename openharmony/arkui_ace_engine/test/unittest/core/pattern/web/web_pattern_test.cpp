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

#include "gtest/gtest.h"

#define private public
#include "core/common/frontend.h"
#include "core/common/window.h"
#include "core/components/web/resource/web_delegate.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/web/web_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/event/touch_event.h"
#include "core/pipeline_ng/pipeline_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::NWeb;
using namespace OHOS::Ace;
using namespace OHOS::Rosen;

namespace OHOS::Ace::NG {
namespace {
#ifdef OHOS_STANDARD_SYSTEM
bool g_isEnable = false;
float g_alpha = 0;
int32_t g_Y = 1;
float g_height = 0;

int32_t g_editStateFlags = 0;

int32_t EF_CAN_CUT = 2;
int32_t EF_CAN_COPY = 4;
int32_t EF_CAN_PASTE = 8;
int32_t EF_CAN_SELECT_ALL = 16;
int32_t CONTRNT_WIDTH_SIZE = 1024;
int32_t CONTRNT_HEIGHT_SIZE = 99999;

std::shared_ptr<NWebTouchHandleState> g_insertHandle = nullptr;
std::shared_ptr<NWebTouchHandleState> g_startSelectionHandle = nullptr;
std::shared_ptr<NWebTouchHandleState> g_endSelectionHandle = nullptr;

RefPtr<WebPattern> g_webPattern = nullptr;
#endif
} // namespace

class WebPatternTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WebPatternTest::SetUpTestCase()
{
#ifdef OHOS_STANDARD_SYSTEM
    std::string src = "web_test";
    RefPtr<WebController> controller = AceType::MakeRefPtr<WebController>();
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::WEB_ETS_TAG, nodeId, [src, controller]() { return AceType::MakeRefPtr<WebPattern>(src, controller); });
    stack->Push(frameNode);

    g_webPattern = frameNode->GetPattern<WebPattern>();
    CHECK_NULL_VOID(g_webPattern);
    g_webPattern->SetWebSrc(src);
    g_webPattern->SetWebController(controller);
#endif
}
void WebPatternTest::TearDownTestCase()
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern = nullptr;
#endif
}

void WebPatternTest::SetUp() {}
void WebPatternTest::TearDown() {}

#ifdef OHOS_STANDARD_SYSTEM
class NWebTouchHandleStateMock : public NWebTouchHandleState {
public:
    int32_t GetTouchHandleId() override
    {
        return 0;
    }

    int32_t GetX() override
    {
        return 0;
    }

    int32_t GetY() override
    {
        return g_Y;
    }

    int32_t GetViewPortX() override
    {
        return 0;
    }

    int32_t GetViewPortY() override
    {
        return 0;
    }

    TouchHandleType GetTouchHandleType() override
    {
        return TouchHandleType::INSERT_HANDLE;
    }

    bool IsEnable() override
    {
        return g_isEnable;
    }

    float GetAlpha() override
    {
        return g_alpha;
    }

    float GetEdgeHeight() override
    {
        return g_height;
    }
};

class NWebQuickMenuParamsMock : public NWebQuickMenuParams {
public:
    int32_t GetXCoord() override
    {
        return 0;
    }

    int32_t GetYCoord() override
    {
        return 0;
    }

    int32_t GetWidth() override
    {
        return 0;
    }

    int32_t GetHeight() override
    {
        return 0;
    }

    int32_t GetEditStateFlags() override
    {
        return g_editStateFlags;
    }

    int32_t GetSelectX() override
    {
        return 0;
    }
    int32_t GetSelectY() override
    {
        return 0;
    }

    int32_t GetSelectWidth() override
    {
        return 0;
    }

    int32_t GetSelectXHeight() override
    {
        return 0;
    }

    std::shared_ptr<NWebTouchHandleState> GetTouchHandleState(NWebTouchHandleState::TouchHandleType type) override
    {
        if (type == NWebTouchHandleState::TouchHandleType::INSERT_HANDLE) {
            return g_insertHandle;
        } else if (type == NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE) {
            return g_startSelectionHandle;
        }
        return g_endSelectionHandle;
    }

    bool GetIsLongPressActived() override
    {
        return false;
    }
};

class NWebQuickMenuCallbackMock : public NWebQuickMenuCallback {
public:
    void Continue(int32_t commandId, MenuEventFlags flag) {}

    void Cancel() {}
};
#endif

/**
 * @tc.name: OnModifyDoneTest001
 * @tc.desc: Test OnModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, OnModifyDoneTest001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern->OnModifyDone();
    int32_t width = 1;
    int32_t height = 1;
    double keyboard = 0;
    g_webPattern->isFocus_ = false;
    bool result = g_webPattern->ProcessVirtualKeyBoard(width, height, keyboard);
    EXPECT_FALSE(result);
    g_webPattern->isVirtualKeyBoardShow_ = WebPattern::VkState::VK_SHOW;
    result = g_webPattern->ProcessVirtualKeyBoard(width, height, keyboard);
    EXPECT_FALSE(result);
    g_webPattern->isFocus_ = true;
    result = g_webPattern->ProcessVirtualKeyBoard(width, height, keyboard);
    EXPECT_FALSE(result);
    keyboard = 1;
    result = g_webPattern->ProcessVirtualKeyBoard(width, height, keyboard);
    EXPECT_FALSE(result);
    g_webPattern->isVirtualKeyBoardShow_ = WebPattern::VkState::VK_HIDE;
    result = g_webPattern->ProcessVirtualKeyBoard(width, height, keyboard);
    EXPECT_FALSE(result);
#endif
}

/**
 * @tc.name: HandleTouchDownTest002
 * @tc.desc: Test HandleTouchDownTest002.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, HandleTouchDownTest002, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    int32_t fingerId = 0;
    g_webPattern->OnModifyDone();
    int32_t width = 1;
    int32_t height = 1;
    double keyboard = 1;
    g_webPattern->isFocus_ = true;
    g_webPattern->isVirtualKeyBoardShow_ = WebPattern::VkState::VK_HIDE;
    auto drawSize = Size(CONTRNT_WIDTH_SIZE, CONTRNT_HEIGHT_SIZE);
    g_webPattern->drawSizeCache_ = drawSize;
    bool result = g_webPattern->ProcessVirtualKeyBoard(width, height, keyboard);
    EXPECT_FALSE(result);
    TouchLocationInfo info("webtest", fingerId);
    TouchEventInfo event("webtest");
    g_webPattern->HandleTouchUp(event, true);
    g_webPattern->HandleTouchDown(event, true);
    g_webPattern->HandleTouchMove(event, true);
    g_webPattern->isDragging_ = true;
    g_webPattern->HandleTouchMove(event, true);

    event.AddTouchLocationInfo(std::move(info));
    g_webPattern->HandleTouchUp(event, true);
    g_webPattern->HandleTouchDown(event, true);
    g_webPattern->HandleTouchMove(event, true);
    g_webPattern->HandleTouchCancel(event);
    g_webPattern->RequestFullScreen();
    g_webPattern->ExitFullScreen();
    g_webPattern->isFullScreen_ = true;
    g_webPattern->ExitFullScreen();
#endif
}

/**
 * @tc.name: IsTouchHandleValid003
 * @tc.desc: Test IsTouchHandleValid.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, IsTouchHandleValid003, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> handle = nullptr;
    bool result = g_webPattern->IsTouchHandleValid(handle);
    EXPECT_FALSE(result);
    EXPECT_FALSE(result);
    handle = std::make_shared<NWebTouchHandleStateMock>();
    result = g_webPattern->IsTouchHandleValid(handle);
    EXPECT_FALSE(result);
    g_isEnable = true;
    result = g_webPattern->IsTouchHandleValid(handle);
    EXPECT_TRUE(result);
#endif
}

/**
 * @tc.name: GetTouchHandleOverlayTypeTest004
 * @tc.desc: Test GetTouchHandleOverlayType.
 * @tc.type: FUNC
 */

HWTEST_F(WebPatternTest, GetTouchHandleOverlayTypeTest004, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    std::shared_ptr<NWebQuickMenuParams> params = std::make_shared<NWebQuickMenuParamsMock>();
    std::shared_ptr<NWebQuickMenuCallback> callback = std::make_shared<NWebQuickMenuCallbackMock>();
    bool result = g_webPattern->RunQuickMenu(params, callback);
    EXPECT_FALSE(result);

    g_insertHandle = std::make_shared<NWebTouchHandleStateMock>();
    result = g_webPattern->RunQuickMenu(params, callback);
    EXPECT_FALSE(result);

    g_endSelectionHandle = std::make_shared<NWebTouchHandleStateMock>();
    result = g_webPattern->RunQuickMenu(params, callback);
    EXPECT_FALSE(result);

    g_startSelectionHandle = std::make_shared<NWebTouchHandleStateMock>();
    result = g_webPattern->RunQuickMenu(params, callback);
    EXPECT_FALSE(result);

    int32_t selectOverlayId = 1;
    g_webPattern->selectOverlayProxy_ = AceType::MakeRefPtr<SelectOverlayProxy>(selectOverlayId);
    result = g_webPattern->RunQuickMenu(params, callback);
    EXPECT_FALSE(result);
    g_webPattern->selectOverlayProxy_->Close();
    g_webPattern->selectOverlayProxy_ = nullptr;

    g_insertHandle.reset();
    g_insertHandle = nullptr;
    result = g_webPattern->RunQuickMenu(params, callback);
    EXPECT_FALSE(result);

    g_endSelectionHandle.reset();
    g_endSelectionHandle = nullptr;
    result = g_webPattern->RunQuickMenu(params, callback);
    EXPECT_FALSE(result);
#endif
}

/**
 * @tc.name: RegisterSelectOverlayCallbackTest005
 * @tc.desc: Test RegisterSelectOverlayCallback.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, RegisterSelectOverlayCallbackTest005, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern->GetCoordinatePoint();
    std::shared_ptr<NWebTouchHandleState> touchHandle = std::make_shared<NWebTouchHandleStateMock>();
    g_webPattern->ComputeTouchHandleRect(touchHandle);
    g_Y = -1;
    g_webPattern->ComputeTouchHandleRect(touchHandle);

    std::shared_ptr<NWebQuickMenuParams> params = std::make_shared<NWebQuickMenuParamsMock>();
    std::shared_ptr<NWebQuickMenuCallback> callback = std::make_shared<NWebQuickMenuCallbackMock>();
    SelectOverlayInfo selectInfo;
    g_webPattern->RegisterSelectOverlayCallback(selectInfo, params, callback);
    EXPECT_FALSE(selectInfo.menuInfo.showCut);
    g_editStateFlags = EF_CAN_CUT;
    g_webPattern->RegisterSelectOverlayCallback(selectInfo, params, callback);
    g_editStateFlags = EF_CAN_COPY;
    g_webPattern->RegisterSelectOverlayCallback(selectInfo, params, callback);
    g_editStateFlags = EF_CAN_PASTE;
    g_webPattern->RegisterSelectOverlayCallback(selectInfo, params, callback);
    g_editStateFlags = EF_CAN_SELECT_ALL;
    g_webPattern->RegisterSelectOverlayCallback(selectInfo, params, callback);
#endif
}

/**
 * @tc.name: OnTouchSelectionChangedTest006
 * @tc.desc: Test OnTouchSelectionChanged.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, OnTouchSelectionChangedTest006, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern->OnTouchSelectionChanged(g_insertHandle, g_startSelectionHandle, g_endSelectionHandle);
    g_endSelectionHandle = std::make_shared<NWebTouchHandleStateMock>();
    g_webPattern->OnTouchSelectionChanged(g_insertHandle, g_startSelectionHandle, g_endSelectionHandle);
    EXPECT_EQ(g_webPattern->insertHandle_, nullptr);
    g_insertHandle = std::make_shared<NWebTouchHandleStateMock>();
    g_startSelectionHandle.reset();
    g_startSelectionHandle = nullptr;
    g_endSelectionHandle.reset();
    g_endSelectionHandle = nullptr;
    g_webPattern->OnTouchSelectionChanged(g_insertHandle, g_startSelectionHandle, g_endSelectionHandle);
    g_webPattern->OnTouchSelectionChanged(g_insertHandle, g_startSelectionHandle, g_endSelectionHandle);
    g_insertHandle.reset();
    g_insertHandle = nullptr;
    g_endSelectionHandle = std::make_shared<NWebTouchHandleStateMock>();
    g_startSelectionHandle = std::make_shared<NWebTouchHandleStateMock>();
    g_webPattern->OnTouchSelectionChanged(g_insertHandle, g_startSelectionHandle, g_endSelectionHandle);
#endif
}

/**
 * @tc.name: UpdateTouchHandleForOverlayTest007
 * @tc.desc: Test UpdateTouchHandleForOverlay.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, UpdateTouchHandleForOverlayTest007, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern->insertHandle_ = std::make_shared<NWebTouchHandleStateMock>();
    g_webPattern->startSelectionHandle_.reset();
    g_webPattern->endSelectionHandle_.reset();
    g_webPattern->UpdateTouchHandleForOverlay();
    EXPECT_EQ(g_webPattern->selectOverlayProxy_, nullptr);
    int32_t selectOverlayId = 1;
    g_webPattern->selectOverlayProxy_ = AceType::MakeRefPtr<SelectOverlayProxy>(selectOverlayId);
    g_webPattern->UpdateTouchHandleForOverlay();
    g_webPattern->insertHandle_.reset();
    g_webPattern->startSelectionHandle_ = std::make_shared<NWebTouchHandleStateMock>();
    g_webPattern->endSelectionHandle_ = std::make_shared<NWebTouchHandleStateMock>();
    g_webPattern->UpdateTouchHandleForOverlay();

    g_webPattern->startSelectionHandle_.reset();
    g_webPattern->endSelectionHandle_.reset();
    g_webPattern->UpdateTouchHandleForOverlay();
    g_webPattern->UpdateLocale();
#endif
}

/**
 * @tc.name: OnOverviewUpdateTest008
 * @tc.desc: Test OnOverviewModeAccessEnabledUpdate.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, OnOverviewUpdateTest008, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern->OnModifyDone();
    int32_t value = 0;
    float scale = 0;
    int x = 0;
    int y = 0;
    EXPECT_NE(g_webPattern->delegate_, nullptr);
    g_webPattern->OnPinchSmoothModeEnabledUpdate(true);
    g_webPattern->OnWebDebuggingAccessEnabledUpdate(true);
    g_webPattern->OnTextZoomRatioUpdate(value);
    g_webPattern->OnDatabaseAccessEnabledUpdate(true);
    g_webPattern->OnFileFromUrlAccessEnabledUpdate(true);
    g_webPattern->OnOverviewModeAccessEnabledUpdate(true);
    g_webPattern->OnBackgroundColorUpdate(value);
    g_webPattern->OnInitialScaleUpdate(scale);
    g_webPattern->OnMultiWindowAccessEnabledUpdate(true);
    g_webPattern->HandleDragStart(x, y);
    g_webPattern->isW3cDragEvent_ = true;
    DragDropInfo dragDropInfo;
    bool result = g_webPattern->GenerateDragDropInfo(dragDropInfo);
    EXPECT_FALSE(result);
    g_webPattern->delegate_->pixelMap_ = nullptr;
    result = g_webPattern->GenerateDragDropInfo(dragDropInfo);
    EXPECT_FALSE(result);
    RefPtr<WebController> controller = AceType::MakeRefPtr<WebController>();
    RefPtr<WebPattern> webPattern = AceType::MakeRefPtr<WebPattern>("test", controller);
    EXPECT_NE(webPattern, nullptr);
    webPattern->OnPinchSmoothModeEnabledUpdate(true);
    webPattern->OnWebDebuggingAccessEnabledUpdate(true);
    webPattern->OnTextZoomRatioUpdate(value);
    webPattern->OnDatabaseAccessEnabledUpdate(true);
    webPattern->OnFileFromUrlAccessEnabledUpdate(true);
    webPattern->OnOverviewModeAccessEnabledUpdate(true);
    webPattern->OnBackgroundColorUpdate(value);
    webPattern->OnInitialScaleUpdate(scale);
    webPattern->OnMultiWindowAccessEnabledUpdate(true);
    webPattern->isW3cDragEvent_ = false;
    result = webPattern->GenerateDragDropInfo(dragDropInfo);
    EXPECT_FALSE(result);
#endif
}

/**
 * @tc.name: HandleDoubleClickEventTest009
 * @tc.desc: Test HandleDoubleClickEvent.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, HandleDoubleClickEventTest009, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    MouseInfo info;
    info.SetButton(MouseButton::LEFT_BUTTON);
    info.SetAction(MouseAction::NONE);
    std::queue<MouseClickInfo> empty;
    swap(empty, g_webPattern->mouseClickQueue_);
    g_webPattern->HandleDoubleClickEvent(info);
    EXPECT_FALSE(g_webPattern->HandleDoubleClickEvent(info));
#endif
}

/**
 * @tc.name: HandleDragUpdateTest010
 * @tc.desc: Test HandleDragUpdate.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, HandleDragUpdateTest010, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern->OnModifyDone();
    GestureEvent info;
    int x = 0;
    int y = 0;
    g_webPattern->isW3cDragEvent_ = false;
    g_webPattern->HandleDragCancel();
    g_webPattern->HandleDragEnd(x, y);
    g_webPattern->isW3cDragEvent_ = true;
    g_webPattern->HandleDragCancel();
    g_webPattern->HandleDragEnd(x, y);
    g_webPattern->needUpdateWeb_ = false;
    auto pipelineContext = PipelineContext::GetCurrentContext();
    g_webPattern->RegistVirtualKeyBoardListener(pipelineContext);

    RefPtr<WebController> controller = AceType::MakeRefPtr<WebController>();
    RefPtr<WebPattern> webPattern = AceType::MakeRefPtr<WebPattern>("test", controller);
    EXPECT_NE(webPattern, nullptr);
    webPattern->isW3cDragEvent_ = true;
    webPattern->HandleDragEnd(x, y);
    g_webPattern->HandleDragCancel();
    webPattern->isW3cDragEvent_ = false;
    webPattern->HandleDragEnd(x, y);
    g_webPattern->HandleDragCancel();
#endif
}

/**
 * @tc.name: OnWindowShowTest011
 * @tc.desc: Test OnWindowShow.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTest, OnWindowShowTest011, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    g_webPattern->OnModifyDone();
    g_webPattern->isWindowShow_ = true;
    g_webPattern->OnWindowShow();
    g_webPattern->OnWindowHide();
    g_webPattern->isWindowShow_ = false;
    g_webPattern->OnWindowHide();
    g_webPattern->OnWindowShow();
    EXPECT_TRUE(g_webPattern->isWindowShow_);

    g_webPattern->isActive_ = true;
    g_webPattern->OnActive();
    g_webPattern->OnInActive();
    g_webPattern->isActive_ = false;
    g_webPattern->OnInActive();
    g_webPattern->OnActive();
    EXPECT_TRUE(g_webPattern->isActive_);

    g_webPattern->OnVisibleChange(false);
    g_webPattern->OnVisibleChange(true);
#endif
}
} // namespace OHOS::Ace::NG
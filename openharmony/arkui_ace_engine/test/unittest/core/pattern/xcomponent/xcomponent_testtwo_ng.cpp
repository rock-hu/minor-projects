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

#include <cstddef>
#include <optional>
#include <unistd.h>
#include <vector>

#include "gtest/gtest.h"

#define private public
#define protected public
#include "test/mock/core/common/mock_image_analyzer_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/render/mock_render_context.h"
#include "test/mock/core/render/mock_render_surface.h"

#include "base/memory/ace_type.h"
#include "base/utils/utils.h"
#include "core/common/ai/image_analyzer_mgr.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_controller_ng.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_layout_algorithm.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_model_ng.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_pattern.h"
#include "core/components_ng/property/measure_property.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/event/touch_event.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_ext_surface_callback_client.h"
#include "frameworks/core/gestures/press_recognizer.h"
#include "frameworks/core/components_ng/pattern/node_container/node_container_pattern.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
struct TestProperty {
    std::optional<std::string> xcId = std::nullopt;
    std::optional<XComponentType> xcType = std::nullopt;
    std::optional<std::string> libraryName = std::nullopt;
    std::optional<std::string> soPath = std::nullopt;
    std::optional<LoadEvent> loadEvent = std::nullopt;
    std::optional<DestroyEvent> destroyEvent = std::nullopt;
    std::optional<SurfaceCreatedEvent> surfaceCreatedEvent = std::nullopt;
    std::optional<SurfaceChangedEvent> surfaceChangedEvent = std::nullopt;
    std::optional<SurfaceDestroyedEvent> surfaceDestroyedEvent = std::nullopt;
};
namespace {
const std::string XCOMPONENT_ID = "xcomponent";
const std::string XCOMPONENT_LIBRARY_NAME = "native_render";
const std::string XCOMPONENT_SO_PATH = "com.example.xcomponentmultihap/entry";
constexpr XComponentType XCOMPONENT_SURFACE_TYPE_VALUE = XComponentType::SURFACE;
constexpr XComponentType XCOMPONENT_TEXTURE_TYPE_VALUE = XComponentType::TEXTURE;
constexpr float MAX_WIDTH = 400.0f;
constexpr float MAX_HEIGHT = 400.0f;
const SizeF MAX_SIZE(MAX_WIDTH, MAX_HEIGHT);
TestProperty g_testProperty;
} // namespace

class XComponentTestTwoNg : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();

protected:
    static RefPtr<FrameNode> CreateXComponentNode(TestProperty& testProperty);
};

void XComponentTestTwoNg::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
    g_testProperty.xcId = XCOMPONENT_ID;
    g_testProperty.libraryName = XCOMPONENT_LIBRARY_NAME;
    g_testProperty.soPath = XCOMPONENT_SO_PATH;
}

void XComponentTestTwoNg::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
}

void processImage(ImageAnalyzerState state) {}

void processPixelMap(const RefPtr<PixelMap>& pixelMap) {}

RefPtr<FrameNode> XComponentTestTwoNg::CreateXComponentNode(TestProperty& testProperty)
{
    auto xcId = testProperty.xcId;
    auto xcType = testProperty.xcType.value();
    auto libraryName = testProperty.libraryName;
    auto xcomponentController = std::make_shared<XComponentControllerNG>();
    XComponentModelNG().Create(xcId, xcType, libraryName, xcomponentController);

    if (testProperty.soPath.has_value()) {
        XComponentModelNG().SetSoPath(testProperty.soPath.value());
    }
    if (testProperty.loadEvent.has_value()) {
        XComponentModelNG().SetOnLoad(std::move(testProperty.loadEvent.value()));
    }
    if (testProperty.destroyEvent.has_value()) {
        XComponentModelNG().SetOnDestroy(std::move(testProperty.destroyEvent.value()));
    }
    if (testProperty.surfaceCreatedEvent.has_value()) {
        XComponentModelNG().SetControllerOnCreated(std::move(testProperty.surfaceCreatedEvent.value()));
    }
    if (testProperty.surfaceChangedEvent.has_value()) {
        XComponentModelNG().SetControllerOnChanged(std::move(testProperty.surfaceChangedEvent.value()));
    }
    if (testProperty.surfaceDestroyedEvent.has_value()) {
        XComponentModelNG().SetControllerOnDestroyed(std::move(testProperty.surfaceDestroyedEvent.value()));
    }
    RefPtr<UINode> element = ViewStackProcessor::GetInstance()->Finish(); // pop
    return AceType::DynamicCast<FrameNode>(element);
}

/**
 * @tc.name: XComponentTestTwoNg001
 * @tc.desc: Test DumpInfo Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg001, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_SURFACE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    pattern->SetType(XComponentType::COMPONENT);
    pattern->DumpInfo();
    EXPECT_EQ(pattern->GetType(), XComponentType::COMPONENT);

    pattern->SetType(XComponentType::NODE);
    pattern->DumpInfo();
    EXPECT_EQ(pattern->GetType(), XComponentType::NODE);

    pattern->SetType(XComponentType::SURFACE);
    pattern->DumpInfo();
    EXPECT_EQ(pattern->GetType(), XComponentType::SURFACE);

    pattern->SetType(XComponentType::TEXTURE);
    pattern->DumpInfo();
    EXPECT_EQ(pattern->GetType(), XComponentType::TEXTURE);

    pattern->SetType(XComponentType::UNKNOWN);
    pattern->DumpInfo();
    EXPECT_EQ(pattern->GetType(), XComponentType::UNKNOWN);

    pattern->SetType(static_cast<XComponentType>(-2));
    pattern->DumpInfo();
    EXPECT_EQ(pattern->GetType(), static_cast<XComponentType>(-2));
}

/**
 * @tc.name: XComponentTestTwoNg002
 * @tc.desc: Test RoundValueToPixelGrid Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg002, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_SURFACE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    float value = 1.0f;
    float ret = pattern->RoundValueToPixelGrid(value, true, true, true);
    EXPECT_EQ(ret, 2.0f);

    value = 1.5f;
    ret = pattern->RoundValueToPixelGrid(value, true, false, false);
    EXPECT_EQ(ret, 2.0f);

    value = -1.6f;
    ret = pattern->RoundValueToPixelGrid(value, true, false, false);
    EXPECT_EQ(ret, -2.0f);

    value = 1.0f;
    ret = pattern->RoundValueToPixelGrid(value, false, false, false);
    EXPECT_EQ(ret, 1.0f);
}

/**
 * @tc.name: XComponentTestTwoNg004
 * @tc.desc: Test OnModifyDone Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg004, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_SURFACE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    auto host = pattern->GetHost();
    ASSERT_TRUE(host);

    auto renderContext = host->GetRenderContext();
    ASSERT_TRUE(renderContext);
    renderContext->propBackgroundColor_ = Color::TRANSPARENT;
    pattern->OnModifyDone();
    EXPECT_EQ(pattern->handlingSurfaceRenderContext_->GetBackgroundColor(), Color::TRANSPARENT);

    renderContext->propBackgroundColor_ = Color::BLUE;
    pattern->OnModifyDone();
    EXPECT_EQ(pattern->handlingSurfaceRenderContext_->GetBackgroundColor(), Color::BLUE);

    renderContext->propBackgroundColor_ = Color::BLACK;
    pattern->OnModifyDone();
    EXPECT_EQ(pattern->handlingSurfaceRenderContext_->GetBackgroundColor(), Color::BLACK);
}

/**
 * @tc.name: XComponentTestTwoNg006
 * @tc.desc: Test BeforeSyncGeometryProperties Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg006, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_TEXTURE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);
    DirtySwapConfig config;
    auto xComponentLayoutAlgorithm = AceType::MakeRefPtr<XComponentLayoutAlgorithm>();
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(MAX_SIZE);
    geometryNode->SetContentSize(MAX_SIZE);
    frameNode->geometryNode_ = geometryNode;
    pattern->hasXComponentInit_ = false;
    pattern->isEnableAnalyzer_ = true;
    auto imageAnalyzerManager =
        std::make_shared<MockImageAnalyzerManager>(frameNode, ImageAnalyzerHolder::XCOMPONENT);
    bool extSurfaceEnabled = SystemProperties::extSurfaceEnabled_;
    SystemProperties::extSurfaceEnabled_ = true;
    imageAnalyzerManager->SetSupportImageAnalyzerFeature(true);
    pattern->imageAnalyzerManager_ = imageAnalyzerManager;
    pattern->BeforeSyncGeometryProperties(config);
    EXPECT_TRUE(pattern->hasXComponentInit_);
    SystemProperties::extSurfaceEnabled_ = extSurfaceEnabled;
}

/**
 * @tc.name: XComponentTestTwoNg008
 * @tc.desc: Test SetHandlingRenderContextForSurface Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg008, TestSize.Level1)
{
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto xComponentController = std::make_shared<XComponentControllerNG>();
    XComponentModelNG xComponent;
    xComponent.Create(XCOMPONENT_ID, XCOMPONENT_SURFACE_TYPE_VALUE, XCOMPONENT_LIBRARY_NAME, xComponentController);
    xComponent.SetSoPath(XCOMPONENT_SO_PATH);

    auto extXComponentController = std::make_shared<XComponentControllerNG>();
    XComponentModelNG extXComponent;
    extXComponent.Create(
        XCOMPONENT_ID, XCOMPONENT_SURFACE_TYPE_VALUE, XCOMPONENT_LIBRARY_NAME, extXComponentController);
    extXComponent.SetSoPath(XCOMPONENT_SO_PATH);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_TRUE(frameNode != nullptr && frameNode->GetTag() == V2::XCOMPONENT_ETS_TAG);

    auto result = xComponentController->SetExtController(extXComponentController);
    EXPECT_EQ(result, XCOMPONENT_CONTROLLER_NO_ERROR);
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(backupApiVersion));
}

/**
 * @tc.name: XComponentTestTwoNg009
 * @tc.desc: Test SetExtController Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg009, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_SURFACE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    auto result = pattern->SetExtController(nullptr);
    EXPECT_EQ(result, XCOMPONENT_CONTROLLER_BAD_PARAMETER);

    auto extFrameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(extFrameNode);
    auto extPattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(extPattern);

    pattern->extPattern_ = AceType::WeakClaim(AceType::RawPtr(extPattern));
    result = pattern->SetExtController(extPattern);
    EXPECT_EQ(result, XCOMPONENT_CONTROLLER_REPEAT_SET);
}

/**
 * @tc.name: XComponentTestTwoNg010
 * @tc.desc: Test ResetExtController Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg010, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_SURFACE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    auto result = pattern->ResetExtController(nullptr);
    EXPECT_EQ(result, XCOMPONENT_CONTROLLER_BAD_PARAMETER);

    auto extFrameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(extFrameNode);

    auto extPattern = extFrameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(extPattern);

    result = pattern->ResetExtController(extPattern);
    EXPECT_EQ(result, XCOMPONENT_CONTROLLER_RESET_ERROR);

    g_testProperty.xcType = XCOMPONENT_TEXTURE_TYPE_VALUE;
    auto frameNodeTmp = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNodeTmp);
    pattern->extPattern_ = AceType::WeakClaim(AceType::RawPtr(pattern));
    auto patternTmp = frameNodeTmp->GetPattern<XComponentPattern>();
    ASSERT_TRUE(patternTmp);
    result = pattern->ResetExtController(patternTmp);
    EXPECT_EQ(result, XCOMPONENT_CONTROLLER_RESET_ERROR);
}

/**
 * @tc.name: XComponentTestTwoNg012
 * @tc.desc: Test HandleSurfaceChangeEvent Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg012, TestSize.Level1)
{
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto xComponentController = std::make_shared<XComponentControllerNG>();
    XComponentModelNG xComponent;
    xComponent.Create(XCOMPONENT_ID, XCOMPONENT_SURFACE_TYPE_VALUE, XCOMPONENT_LIBRARY_NAME, xComponentController);
    xComponent.SetSoPath(XCOMPONENT_SO_PATH);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_TRUE(frameNode != nullptr && frameNode->GetTag() == V2::XCOMPONENT_ETS_TAG);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);
    DirtySwapConfig config;
    auto xComponentLayoutAlgorithm = AceType::MakeRefPtr<XComponentLayoutAlgorithm>();
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetFrameSize(MAX_SIZE);
    geometryNode->SetContentSize(MAX_SIZE);
    frameNode->geometryNode_ = geometryNode;
    EXPECT_FALSE(pattern->hasXComponentInit_);
    EXPECT_CALL(*AceType::DynamicCast<MockRenderSurface>(pattern->renderSurface_), IsSurfaceValid())
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_CALL(*AceType::DynamicCast<MockRenderSurface>(pattern->renderSurface_),
        AdjustNativeWindowSize(MAX_WIDTH, MAX_HEIGHT))
        .WillOnce(Return());

    pattern->SetRenderType(NodeRenderType::RENDER_TYPE_TEXTURE);
    pattern->BeforeSyncGeometryProperties(config);
    EXPECT_TRUE(pattern->hasXComponentInit_);
    EXPECT_TRUE(AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE));
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(backupApiVersion));
}

/**
 * @tc.name: XComponentTestTwoNg013
 * @tc.desc: Test UpdateSurfaceRect Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg013, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_TEXTURE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    pattern->drawSize_ = SizeF(0.0f, 0.0f);
    auto ret = pattern->UpdateSurfaceRect();
    std::tuple<bool, bool, bool> result(false, false, false);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name: XComponentTestTwoNg015
 * @tc.desc: Test StartImageAnalyzer Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg015, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_TEXTURE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    std::optional<std::function<void(ImageAnalyzerState)>> callback;
    callback = processImage;
    pattern->isEnableAnalyzer_ = true;
    auto imageAnalyzerManager =
        std::make_shared<MockImageAnalyzerManager>(frameNode, ImageAnalyzerHolder::XCOMPONENT);
    bool extSurfaceEnabled = SystemProperties::extSurfaceEnabled_;
    SystemProperties::extSurfaceEnabled_ = true;
    imageAnalyzerManager->SetSupportImageAnalyzerFeature(true);
    pattern->imageAnalyzerManager_ = imageAnalyzerManager;

    pattern->StartImageAnalyzer(nullptr, callback);
    ASSERT_TRUE(pattern->IsSupportImageAnalyzerFeature());

    pattern->isEnableAnalyzer_ = false;
    pattern->StartImageAnalyzer(nullptr, callback);
    ASSERT_FALSE(pattern->IsSupportImageAnalyzerFeature());
    SystemProperties::extSurfaceEnabled_ = extSurfaceEnabled;
}

/**
 * @tc.name: XComponentTestTwoNg016
 * @tc.desc: Test ChangeSurfaceCallbackMode.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set surface life cycle callback, set id&libraryname to null and create XComponent
     * @tc.expected: xcomponent frameNode create successfully
     */
    g_testProperty.xcType = XCOMPONENT_SURFACE_TYPE_VALUE;
    g_testProperty.xcId = std::nullopt;
    g_testProperty.libraryName = std::nullopt;
    int32_t onSurfaceCreatedSurfaceCount = 0;
    int32_t onSurfaceDestroyedSurfaceCount = 0;
    auto onSurfaceCreated = [&onSurfaceCreatedSurfaceCount](const std::string& surfaceId, const std::string& xcId) {
        ++onSurfaceCreatedSurfaceCount;
    };
    auto onSurfaceDestroyed = [&onSurfaceDestroyedSurfaceCount](const std::string& surfaceId, const std::string& xcId) {
        ++onSurfaceDestroyedSurfaceCount;
    };
    g_testProperty.surfaceCreatedEvent = std::move(onSurfaceCreated);
    g_testProperty.surfaceDestroyedEvent = std::move(onSurfaceDestroyed);
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);
    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);
    pattern->isTypedNode_ = true;
    pattern->RegisterSurfaceCallbackModeEvent();
    EXPECT_EQ(pattern->surfaceCallbackMode_, SurfaceCallbackMode::DEFAULT);

    /**
     * @tc.steps: step2. attachTo&detachFrom main tree in DEFAULT mode
     * @tc.expected: DEFAULT mode attach to(detach from) main tree will call onSurfaceCreated(onSurfaceDestroyed) once
     */
    frameNode->AttachToMainTree(false);
    EXPECT_EQ(onSurfaceCreatedSurfaceCount, 1);
    frameNode->DetachFromMainTree();
    EXPECT_EQ(onSurfaceDestroyedSurfaceCount, 1);

    /**
     * @tc.steps: step3. detachFromFrameNode(in frameNode's destructor) in DEFAULT mode
     * @tc.expected: will not call onSurfaceDestroyed
     */
    pattern->OnDetachFromFrameNode(AceType::RawPtr(frameNode));
    EXPECT_EQ(onSurfaceDestroyedSurfaceCount, 1);

    /**
     * @tc.steps: step4. ChangeSurfaceCallbackMode to PIP not in main tree
     * @tc.expected: not in main tree change to pip will call onSurfaceCreated once
     */
    pattern->ChangeSurfaceCallbackMode(SurfaceCallbackMode::PIP);
    EXPECT_EQ(onSurfaceCreatedSurfaceCount, 2);

    /**
     * @tc.steps: step5. attachTo&detachFrom main tree in PIP mode
     * @tc.expected: PIP mode attach to(detach from) main tree will not call onSurfaceCreated(onSurfaceDestroyed)
     */
    frameNode->AttachToMainTree(false);
    EXPECT_EQ(onSurfaceCreatedSurfaceCount, 2);
    frameNode->DetachFromMainTree();
    EXPECT_EQ(onSurfaceDestroyedSurfaceCount, 1);

    /**
     * @tc.steps: step6. detachFromFrameNode(in frameNode's destructor) in PIP mode
     * @tc.expected: call onSurfaceDestroyed once
     */
    pattern->OnDetachFromFrameNode(AceType::RawPtr(frameNode));
    EXPECT_EQ(onSurfaceDestroyedSurfaceCount, 2);

    /**
     * @tc.steps: step7. ChangeSurfaceCallbackMode in main tree
     * @tc.expected: will not call onSurfaceCreated(onSurfaceDestroyed)
     */
    frameNode->AttachToMainTree(false);
    pattern->ChangeSurfaceCallbackMode(SurfaceCallbackMode::DEFAULT);
    EXPECT_EQ(onSurfaceDestroyedSurfaceCount, 2);
    pattern->ChangeSurfaceCallbackMode(SurfaceCallbackMode::PIP);
    EXPECT_EQ(onSurfaceCreatedSurfaceCount, 2);
}

/**
 * @tc.name: XComponentTestTwoNg017
 * @tc.desc: Test EnableSecure Func.
 * @tc.type: FUNC
 */
HWTEST_F(XComponentTestTwoNg, XComponentTestTwoNg017, TestSize.Level1)
{
    g_testProperty.xcType = XCOMPONENT_SURFACE_TYPE_VALUE;
    auto frameNode = CreateXComponentNode(g_testProperty);
    ASSERT_TRUE(frameNode);

    auto pattern = frameNode->GetPattern<XComponentPattern>();
    ASSERT_TRUE(pattern);

    EXPECT_CALL(*AceType::DynamicCast<MockRenderContext>(pattern->renderContextForSurface_),
                SetSecurityLayer(true)).WillOnce(Return());
    pattern->EnableSecure(true);
    EXPECT_CALL(*AceType::DynamicCast<MockRenderContext>(pattern->renderContextForSurface_),
                SetSecurityLayer(false)).WillOnce(Return());
    pattern->EnableSecure(false);

    pattern->type_ = XCOMPONENT_TEXTURE_TYPE_VALUE;

    EXPECT_CALL(*AceType::DynamicCast<MockRenderContext>(pattern->renderContextForSurface_),
                SetSecurityLayer(true)).Times(0);
    pattern->EnableSecure(true);
    EXPECT_CALL(*AceType::DynamicCast<MockRenderContext>(pattern->renderContextForSurface_),
                SetSecurityLayer(false)).Times(0);
    pattern->EnableSecure(false);
}
} // namespace OHOS::Ace::NG

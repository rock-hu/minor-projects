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
#include <cstdint>
#include "gtest/gtest.h"
#define private public
#define protected public
#include "native_interface.h"
#include "native_node.h"
#include "native_type.h"
#include "native_styled_string.h"
#include "event_converter.h"
#include "interfaces/native/node/node_model.h"
#include "test/mock/base/mock_task_executor.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace {
class NativeNodeTest : public testing::Test {
public:
    static void SetUpTestSuite()
    {
        NG::MockPipelineContext::SetUp();
        MockContainer::SetUp();
        MockContainer::Current()->pipelineContext_ = NG::MockPipelineContext::GetCurrent();
        MockContainer::Current()->taskExecutor_ = AceType::MakeRefPtr<MockTaskExecutor>();
        MockContainer::Current()->pipelineContext_->taskExecutor_ = MockContainer::Current()->taskExecutor_;
        auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
        PipelineBase::GetCurrentContext()->SetThemeManager(themeManager);
    }
    static void TearDownTestSuite()
    {
        NG::MockPipelineContext::TearDown();
        MockContainer::TearDown();
    }
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: NativeNodeTest001
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetEventType function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest001, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, -1 };
    auto eventType = OH_ArkUI_NodeEvent_GetEventType(&event);
    EXPECT_EQ(eventType, ArkUI_NodeEventType::NODE_TOUCH_EVENT);
    event.kind = 0;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetEventType(&event), ArkUI_NodeEventType::NODE_TOUCH_EVENT);
}

/**
 * @tc.name: NativeNodeTest002
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetEventType function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest002, TestSize.Level1)
{
    auto eventType = OH_ArkUI_NodeEvent_GetEventType(nullptr);
    EXPECT_EQ(eventType, ArkUI_NodeEventType::NODE_TOUCH_EVENT);
}

/**
 * @tc.name: NativeNodeTest003
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetTargetId function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest003, TestSize.Level1)
{
    auto targetId = OH_ArkUI_NodeEvent_GetTargetId(nullptr);
    EXPECT_EQ(targetId, -1);
    ArkUI_NodeEvent event = { 0, 0 };
    event.eventId = 0;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetTargetId(&event), 0);
}

/**
 * @tc.name: NativeNodeTest004
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetNodeHandle function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest004, TestSize.Level1)
{
    auto handle = OH_ArkUI_NodeEvent_GetNodeHandle(nullptr);
    EXPECT_EQ(handle, nullptr);
    ArkUI_NodeEvent event = { 0, 0 };
    event.node = nullptr;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNodeHandle(&event), nullptr);
}

/**
 * @tc.name: NativeNodeTest005
 * @tc.desc: Test customNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest005, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_STACK);
    auto childNode = nodeAPI->createNode(ARKUI_NODE_STACK);
    ASSERT_NE(rootNode, nullptr);
    int32_t ret1 = nodeAPI->addChild(rootNode, childNode);
    EXPECT_EQ(ret1, ARKUI_ERROR_CODE_NO_ERROR);
    float size = 100.0f;
    float padding = 10.0f;
    float val0 = 0.0f;
    float val01 = 0.1f;
    float val05 = 0.5f;
    float val1 = 1.0f;
    float val10 = 10.0f;
    float val100 = 100.0f;
    float negativeFloat = -1.0f;
    uint32_t color = 0xFFFF0000;
    const char* pathCommands = "M100 0 L200 240 L0 240 Z";

    ArkUI_NumberValue value[] = {{.f32 = size}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value2[] = {{.f32 = val1}, {.f32 = val1}};
    ArkUI_AttributeItem item2 = {value2, sizeof(value2) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value3[] = {{.f32 = val0}, {.f32 = val0}, {.f32 = val0}};
    ArkUI_AttributeItem item3 = {value3, sizeof(value3) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value4[] = {{.f32 = padding}, {.f32 = padding}, {.f32 = padding}, {.f32 = padding}};
    ArkUI_AttributeItem item4 = {value4, sizeof(value4) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value5[] = {{.f32 = val0}, {.f32 = val0}, {.f32 = val0}, {.f32 = val0}, {.f32 = val0}};
    ArkUI_AttributeItem item5 = {value5, sizeof(value5) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_WIDTH, &item);
    auto widthVal = nodeAPI->getAttribute(rootNode, NODE_WIDTH);
    EXPECT_EQ(widthVal->value[0].f32, size);

    nodeAPI->setAttribute(rootNode, NODE_HEIGHT, &item);
    auto heightVal = nodeAPI->getAttribute(rootNode, NODE_HEIGHT);
    EXPECT_EQ(heightVal->value[0].f32, size);

    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_COLOR, &item);
    auto colorVal = nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_COLOR);
    EXPECT_EQ(colorVal->value[0].u32, color);

    item.string = "test";
    value[0].i32 = ARKUI_IMAGE_REPEAT_X;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE, &item);
    nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_IMAGE);

    value[0].f32 = padding;
    nodeAPI->setAttribute(rootNode, NODE_PADDING, &item);
    auto paddingVal = nodeAPI->getAttribute(rootNode, NODE_PADDING);
    EXPECT_EQ(paddingVal->value[0].f32, padding);
    nodeAPI->setAttribute(rootNode, NODE_PADDING, &item4);
    auto paddingVal4 = nodeAPI->getAttribute(rootNode, NODE_PADDING);
    EXPECT_EQ(paddingVal4->value[0].f32, padding);

    nodeAPI->setAttribute(rootNode, NODE_MARGIN, &item);
    auto marginVal = nodeAPI->getAttribute(rootNode, NODE_MARGIN);
    EXPECT_EQ(marginVal->value[0].f32, padding);
    nodeAPI->setAttribute(rootNode, NODE_MARGIN, &item4);
    auto marginVal4 = nodeAPI->getAttribute(rootNode, NODE_MARGIN);
    EXPECT_EQ(marginVal4->value[0].f32, padding);

    item.string = "test";
    ret1 = nodeAPI->setAttribute(rootNode, NODE_ID, &item);
    EXPECT_EQ(ret1, ARKUI_ERROR_CODE_NO_ERROR);
    auto keyVal = nodeAPI->getAttribute(rootNode, NODE_ID);
    EXPECT_NE(keyVal, nullptr);

    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_ENABLED, &item);
    auto enableVal = nodeAPI->getAttribute(rootNode, NODE_ENABLED);
    EXPECT_EQ(enableVal->value[0].i32, 1);

    nodeAPI->setAttribute(rootNode, NODE_TRANSLATE, &item3);
    auto translateVal = nodeAPI->getAttribute(rootNode, NODE_TRANSLATE);
    EXPECT_EQ(translateVal->value[0].f32, val0);

    nodeAPI->setAttribute(rootNode, NODE_SCALE, &item2);
    auto scaleVal = nodeAPI->getAttribute(rootNode, NODE_SCALE);
    EXPECT_EQ(scaleVal->value[0].f32, val1);
    nodeAPI->setAttribute(rootNode, NODE_SCALE, &item5);

    nodeAPI->setAttribute(rootNode, NODE_ROTATE, &item5);
    auto rotateVal = nodeAPI->getAttribute(rootNode, NODE_ROTATE);
    EXPECT_EQ(rotateVal->value[0].f32, val0);

    value[0].f32 = val0;
    nodeAPI->setAttribute(rootNode, NODE_BLUR, &item);
    auto blurVal = nodeAPI->getAttribute(rootNode, NODE_BLUR);
    EXPECT_EQ(blurVal->value[0].f32, val0);

    value[0].f32 = val0;
    nodeAPI->setAttribute(rootNode, NODE_SATURATION, &item);
    auto saturationVal = nodeAPI->getAttribute(rootNode, NODE_SATURATION);
    EXPECT_EQ(saturationVal->value[0].f32, val0);

    value[0].f32 = val0;
    nodeAPI->setAttribute(rootNode, NODE_BRIGHTNESS, &item);
    auto brightnessVal = nodeAPI->getAttribute(rootNode, NODE_BRIGHTNESS);
    EXPECT_EQ(brightnessVal->value[0].f32, val0);

    value3[0].f32 = val0;
    value3[1].i32 = ARKUI_LINEAR_GRADIENT_DIRECTION_LEFT;
    value3[2].i32 = false;
    uint32_t colors[] = {color, color};
    float stops[] = {val0, val05};
    ArkUI_ColorStop colorStop = {colors, stops, 2};
    ArkUI_ColorStop* colorStopPtr = &colorStop;
    item3.object = reinterpret_cast<void*>(colorStopPtr);
    nodeAPI->setAttribute(rootNode, NODE_LINEAR_GRADIENT, &item3);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LINEAR_GRADIENT), nullptr);
    
    ArkUI_NumberValue sweepGradient[] = {{.f32 = val0}, {.f32 = val0}, {.f32 = val0}, {.f32 = val0}, {.f32 = val0},
        {.i32 = true}};
    ArkUI_AttributeItem sweepGradientItem = {sweepGradient, sizeof(sweepGradient) / sizeof(ArkUI_NumberValue)};
    sweepGradientItem.object = reinterpret_cast<void*>(colorStopPtr);
    sweepGradientItem.size = -1;
    nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &sweepGradientItem);
    sweepGradientItem.size = 1;
    nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &sweepGradientItem);
    sweepGradientItem.size = 2;
    nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &sweepGradientItem);
    sweepGradientItem.size = 3;
    nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &sweepGradientItem);
    sweepGradientItem.size = 4;
    nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &sweepGradientItem);
    sweepGradientItem.size = sizeof(sweepGradient) / sizeof(ArkUI_NumberValue);
    nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &sweepGradientItem);
    auto sweepGradientVal = nodeAPI->getAttribute(rootNode, NODE_SWEEP_GRADIENT);
    EXPECT_EQ(sweepGradientVal->value[0].f32, val0);

    ArkUI_NumberValue radialGradient[] = {{.f32 = val0}, {.f32 = val0}, {.f32 = val0}, {.i32 = true}};
    ArkUI_AttributeItem radialGradientItem = {radialGradient, sizeof(radialGradient) / sizeof(ArkUI_NumberValue)};
    radialGradientItem.object = reinterpret_cast<void*>(colorStopPtr);
    radialGradientItem.size = -1;
    nodeAPI->setAttribute(rootNode, NODE_RADIAL_GRADIENT, &radialGradientItem);
    radialGradientItem.size = 1;
    nodeAPI->setAttribute(rootNode, NODE_RADIAL_GRADIENT, &radialGradientItem);
    radialGradientItem.size = 2;
    nodeAPI->setAttribute(rootNode, NODE_RADIAL_GRADIENT, &radialGradientItem);
    radialGradientItem.size = sizeof(radialGradient) / sizeof(ArkUI_NumberValue);
    nodeAPI->setAttribute(rootNode, NODE_RADIAL_GRADIENT, &radialGradientItem);
    auto radialGradientVal = nodeAPI->getAttribute(rootNode, NODE_RADIAL_GRADIENT);
    EXPECT_EQ(radialGradientVal->value[0].f32, val0);

    value[0].i32 = ARKUI_ALIGNMENT_CENTER;
    nodeAPI->setAttribute(rootNode, NODE_ALIGNMENT, &item);
    auto alignVal = nodeAPI->getAttribute(rootNode, NODE_ALIGNMENT);
    EXPECT_EQ(alignVal->value[0].i32, static_cast<int32_t>(ARKUI_ALIGNMENT_CENTER));

    value[0].f32 = val0;
    nodeAPI->setAttribute(rootNode, NODE_OPACITY, &item);
    auto opacityVal = nodeAPI->getAttribute(rootNode, NODE_OPACITY);
    EXPECT_EQ(opacityVal->value[0].f32, val0);

    value[0].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item);
    auto borderWidthVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH);
    EXPECT_EQ(borderWidthVal->value[0].f32, val10);
    value4[0].f32 = val10;
    value4[1].f32 = val10;
    value4[2].f32 = val10;
    value4[3].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item4);
    auto borderWidthVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH);
    EXPECT_EQ(borderWidthVal4->value[0].f32, val10);

    value[0].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item);
    borderWidthVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT);
    EXPECT_EQ(borderWidthVal->value[0].f32, val01);
    value4[0].f32 = val01;
    value4[1].f32 = val01;
    value4[2].f32 = val01;
    value4[3].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item4);
    borderWidthVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT);
    EXPECT_EQ(borderWidthVal4->value[0].f32, val01);

    value[0].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item);
    auto borderRadiusVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS);
    EXPECT_EQ(borderRadiusVal->value[0].f32, val10);
    value4[0].f32 = val10;
    value4[1].f32 = val10;
    value4[2].f32 = val10;
    value4[3].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item4);
    auto borderRadiusVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS);
    EXPECT_EQ(borderRadiusVal4->value[0].f32, val10);
    value[0].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item);
    borderRadiusVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT);
    EXPECT_EQ(borderRadiusVal->value[0].f32, val01);
    value4[0].f32 = val01;
    value4[1].f32 = val01;
    value4[2].f32 = val01;
    value4[3].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item4);
    borderRadiusVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT);
    EXPECT_EQ(borderRadiusVal4->value[0].f32, val01);

    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_COLOR, &item);
    auto borderColorVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_COLOR);
    EXPECT_EQ(borderColorVal->value[0].u32, color);
    value4[0].u32 = color;
    value4[1].u32 = color;
    value4[2].u32 = color;
    value4[3].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_COLOR, &item4);
    auto borderColorVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_COLOR);
    EXPECT_EQ(borderColorVal4->value[0].u32, color);

    value[0].i32 = ARKUI_BORDER_STYLE_SOLID;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &item);
    auto borderStyleVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_STYLE);
    EXPECT_EQ(borderStyleVal->value[0].i32, static_cast<int32_t>(ARKUI_BORDER_STYLE_SOLID));
    value4[0].i32 = ARKUI_BORDER_STYLE_SOLID;
    value4[1].i32 = ARKUI_BORDER_STYLE_SOLID;
    value4[2].i32 = ARKUI_BORDER_STYLE_SOLID;
    value4[3].i32 = ARKUI_BORDER_STYLE_SOLID;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &item4);
    auto borderStyleVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_STYLE);
    EXPECT_EQ(borderStyleVal4->value[0].i32, ARKUI_BORDER_STYLE_SOLID);

    value[0].f32 = val1;
    nodeAPI->setAttribute(rootNode, NODE_Z_INDEX, &item);
    auto zIndexVal = nodeAPI->getAttribute(rootNode, NODE_Z_INDEX);
    EXPECT_EQ(zIndexVal->value[0].f32, val1);

    value[0].i32 = ARKUI_VISIBILITY_VISIBLE;
    nodeAPI->setAttribute(rootNode, NODE_VISIBILITY, &item);
    auto visibilityVal = nodeAPI->getAttribute(rootNode, NODE_VISIBILITY);
    EXPECT_EQ(visibilityVal->value[0].i32, static_cast<int32_t>(ARKUI_VISIBILITY_VISIBLE));

    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_CLIP, &item);
    auto clipVal = nodeAPI->getAttribute(rootNode, NODE_CLIP);
    EXPECT_EQ(clipVal->value[0].i32, true);

    value5[0].i32 = ARKUI_CLIP_TYPE_RECTANGLE;
    value5[1].f32 = size;
    value5[2].f32 = size;
    value5[3].f32 = val10;
    value5[4].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item5);
    auto clipShapeVal = nodeAPI->getAttribute(rootNode, NODE_CLIP_SHAPE);
    EXPECT_EQ(clipShapeVal->value[0].i32, ARKUI_CLIP_TYPE_RECTANGLE);
    value3[0].i32 = ARKUI_CLIP_TYPE_CIRCLE;
    value3[1].f32 = size;
    value3[2].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item3);
    clipShapeVal = nodeAPI->getAttribute(rootNode, NODE_CLIP_SHAPE);
    EXPECT_EQ(clipShapeVal->value[0].i32, ARKUI_CLIP_TYPE_CIRCLE);
    value3[0].i32 = ARKUI_CLIP_TYPE_ELLIPSE;
    value3[1].f32 = size;
    value3[2].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item3);
    clipShapeVal = nodeAPI->getAttribute(rootNode, NODE_CLIP_SHAPE);
    EXPECT_EQ(clipShapeVal->value[0].i32, ARKUI_CLIP_TYPE_ELLIPSE);
    value3[1].f32 = negativeFloat;
    nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item3);

    value3[0].i32 = ARKUI_CLIP_TYPE_PATH;
    value3[1].f32 = size;
    value3[2].f32 = size;
    item3.string = nullptr;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item3), ARKUI_ERROR_CODE_PARAM_INVALID);
    item3.string = pathCommands;
    nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item3);
    clipShapeVal = nodeAPI->getAttribute(rootNode, NODE_CLIP_SHAPE);
    EXPECT_EQ(clipShapeVal->value[0].i32, ARKUI_CLIP_TYPE_PATH);
    value3[2].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item3), ARKUI_ERROR_CODE_PARAM_INVALID);
    value3[1].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item3), ARKUI_ERROR_CODE_PARAM_INVALID);

    ArkUI_NumberValue transformVal[] =
        {{.f32 = val1}, {.f32 = val0}, {.f32 = val0}, {.f32 = val0},
        {.f32 = val0}, {.f32 = val1}, {.f32 = val0}, {.f32 = val0},
        {.f32 = val0}, {.f32 = val0}, {.f32 = val1}, {.f32 = val0},
        {.f32 = val0}, {.f32 = val0}, {.f32 = val0}, {.f32 = val1}};
    ArkUI_AttributeItem transformItem = {transformVal, sizeof(transformVal) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM, &transformItem);
    auto transformResult = nodeAPI->getAttribute(rootNode, NODE_TRANSFORM);
    EXPECT_EQ(transformResult->value[0].f32, val1);

    value[0].i32 = ARKUI_HIT_TEST_MODE_DEFAULT;
    nodeAPI->setAttribute(rootNode, NODE_HIT_TEST_BEHAVIOR, &item);
    auto hitVal = nodeAPI->getAttribute(rootNode, NODE_HIT_TEST_BEHAVIOR);
    EXPECT_EQ(hitVal->value[0].i32, static_cast<int32_t>(ARKUI_HIT_TEST_MODE_DEFAULT));

    value2[0].f32 = val10;
    value2[1].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_POSITION, &item2);
    auto positionVal = nodeAPI->getAttribute(rootNode, NODE_POSITION);
    EXPECT_EQ(positionVal->value[0].f32, val10);

    value[0].i32 = ARKUI_SHADOW_STYLE_OUTER_DEFAULT_MD;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SHADOW, &item), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SHADOW), nullptr);
    

    ArkUI_NumberValue customShadowVal[] = {{.f32 = val10}, {.i32 = false}, {.f32 = val0}, {.f32 = val0},
        {.i32 = ARKUI_SHADOW_TYPE_COLOR}, {.u32 = color}, {.u32 = 1}};
    ArkUI_AttributeItem customShadowItem = {customShadowVal, sizeof(customShadowVal) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CUSTOM_SHADOW), nullptr);
    customShadowItem.size = 1;
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);
    customShadowItem.size = 2;
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);
    customShadowItem.size = 3;
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);
    customShadowItem.size = 4;
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);
    customShadowItem.size = 5;
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);
    customShadowVal[4].i32 = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    customShadowItem.size = 6;
    customShadowVal[4].i32 = ARKUI_SHADOW_TYPE_COLOR;
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);
    customShadowVal[1].i32 = true;
    customShadowVal[5].i32 = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    customShadowVal[5].i32 = 0;
    nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &customShadowItem);


    value2[0].f32 = size;
    value2[1].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE, &item2);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE), nullptr);

    value[0].i32 = ARKUI_IMAGE_SIZE_COVER;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE, &item);
    auto imagesizeStyleVal = nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE);
    EXPECT_EQ(imagesizeStyleVal->value[0].i32, static_cast<int32_t>(ARKUI_IMAGE_SIZE_COVER));
    value[0].i32 = ARKUI_IMAGE_SIZE_AUTO;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE, &item);
    value[0].i32 = ARKUI_IMAGE_SIZE_CONTAIN;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE, &item);

    ArkUI_NumberValue backgroundBlurVal[] = {{.i32 = ARKUI_BLUR_STYLE_THIN}, {.i32 = ARKUI_COLOR_MODE_SYSTEM},
        {.i32 = ARKUI_ADAPTIVE_COLOR_DEFAULT}, {.f32 = val05}, {.f32 = val0}, {.f32 = val0}};
    ArkUI_AttributeItem backgroundBlurItem = {backgroundBlurVal, sizeof(backgroundBlurVal) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem);
    auto backgroundBlurResult = nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE);
    EXPECT_NE(backgroundBlurResult, nullptr);
    nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_BLUR_STYLE, &backgroundBlurItem);
    auto foregroundBlurStyleVal = nodeAPI->getAttribute(rootNode, NODE_FOREGROUND_BLUR_STYLE);
    EXPECT_NE(foregroundBlurStyleVal, nullptr);
    backgroundBlurVal[0].i32 = ARKUI_BLUR_STYLE_NONE;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem);
    nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE);
    backgroundBlurVal[0].i32 = ARKUI_BLUR_STYLE_COMPONENT_ULTRA_THIN;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem);
    nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE);
    backgroundBlurItem.size = 1;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem);
    nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_BLUR_STYLE, &backgroundBlurItem);
    

    ArkUI_NumberValue transformCenter[] = {{.f32 = val0}, {.i32 = val0}, {.f32 = val0}, {.f32 = val05},
        {.f32 = val05}, {.f32 = val0}};
    ArkUI_AttributeItem transformCenterItem = {transformCenter, sizeof(transformCenter) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);
    auto transformCenterVal = nodeAPI->getAttribute(rootNode, NODE_TRANSFORM_CENTER);
    EXPECT_EQ(transformCenterVal->value[0].f32, val0);
    transformCenterItem.size = 0;
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);
    transformCenterItem.size = sizeof(transformCenter) / sizeof(ArkUI_NumberValue);
    transformCenter[5].f32 = -1.0f;
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);
    transformCenter[5].f32 = 2.0f;
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);
    transformCenter[4].f32 = -1.0f;
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);
    transformCenter[4].f32 = 2.0f;
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);
    transformCenter[3].f32 = -1.0f;
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);
    transformCenter[3].f32 = 2.0f;
    nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &transformCenterItem);

    ArkUI_NumberValue opacityTransition[] = {{.f32 = val0}, {.i32 = 1000}, {.i32 = ARKUI_CURVE_LINEAR}, {.i32 = 1000},
        {.i32 = 1}, {.i32 = ARKUI_ANIMATION_PLAY_MODE_NORMAL}, {.f32 = val1}};
    ArkUI_AttributeItem opacityTransitionItem =
        {opacityTransition, sizeof(opacityTransition) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem);
    auto opacityTransitionVal = nodeAPI->getAttribute(rootNode, NODE_OPACITY_TRANSITION);
    EXPECT_EQ(opacityTransitionVal->value[0].f32, val0);
    opacityTransition[5].i32 = ARKUI_ANIMATION_PLAY_MODE_REVERSE;
    nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem);
    nodeAPI->getAttribute(rootNode, NODE_OPACITY_TRANSITION);
    opacityTransition[5].i32 = ARKUI_ANIMATION_PLAY_MODE_ALTERNATE;
    nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem);
    nodeAPI->getAttribute(rootNode, NODE_OPACITY_TRANSITION);

    ArkUI_NumberValue rotateTransition[] = {{.f32 = val0}, {.f32 = val0}, {.f32 = val0}, {.f32 = val10},
        {.f32 = val0}, {.i32 = 1000}, {.i32 = ARKUI_CURVE_LINEAR}, {.i32 = 1000},
        {.i32 = 1}, {.i32 = ARKUI_ANIMATION_PLAY_MODE_NORMAL}, {.f32 = val1}};
    ArkUI_AttributeItem rotateTransitionItem =
        {rotateTransition, sizeof(rotateTransition) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_ROTATE_TRANSITION, &rotateTransitionItem);
    auto rotateTransitionVal = nodeAPI->getAttribute(rootNode, NODE_ROTATE_TRANSITION);
    EXPECT_EQ(rotateTransitionVal->value[0].f32, val0);
    rotateTransition[6].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_ROTATE_TRANSITION, &rotateTransitionItem);

    ArkUI_NumberValue scaleTransition[] = {{.f32 = val0}, {.f32 = val0}, {.f32 = val0},
        {.i32 = 1000}, {.i32 = ARKUI_CURVE_LINEAR}, {.i32 = 1000},
        {.i32 = 1}, {.i32 = ARKUI_ANIMATION_PLAY_MODE_NORMAL}, {.f32 = val1}};
    ArkUI_AttributeItem scaleTransitionItem =
        {scaleTransition, sizeof(scaleTransition) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_SCALE_TRANSITION, &scaleTransitionItem);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCALE_TRANSITION), nullptr);
    scaleTransition[4].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_SCALE_TRANSITION, &scaleTransitionItem);

    ArkUI_NumberValue translateTransition[] = {{.f32 = val0}, {.f32 = val0}, {.f32 = val0},
        {.i32 = 1000}, {.i32 = ARKUI_CURVE_LINEAR}, {.i32 = 1000},
        {.i32 = 1}, {.i32 = ARKUI_ANIMATION_PLAY_MODE_NORMAL}, {.f32 = val1}};
    ArkUI_AttributeItem translateTransitionItem =
        {translateTransition, sizeof(translateTransition) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_TRANSLATE_TRANSITION, &translateTransitionItem);
    auto translateTransitionVal = nodeAPI->getAttribute(rootNode, NODE_TRANSLATE_TRANSITION);
    EXPECT_EQ(translateTransitionVal->value[0].f32, val0);
    translateTransition[4].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_TRANSLATE_TRANSITION, &translateTransitionItem);

    ArkUI_NumberValue moveTransition[] = {{.i32 = ARKUI_TRANSITION_EDGE_TOP},
        {.i32 = 1000}, {.i32 = ARKUI_CURVE_LINEAR}, {.i32 = 1000},
        {.i32 = 1}, {.i32 = ARKUI_ANIMATION_PLAY_MODE_NORMAL}, {.f32 = val1}};
    ArkUI_AttributeItem moveTransitionItem =
        {moveTransition, sizeof(moveTransition) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_MOVE_TRANSITION, &moveTransitionItem);
    auto moveTransitionVal = nodeAPI->getAttribute(rootNode, NODE_MOVE_TRANSITION);
    EXPECT_EQ(moveTransitionVal->value[0].i32, static_cast<int32_t>(ARKUI_TRANSITION_EDGE_TOP));
    moveTransition[0].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_MOVE_TRANSITION, &moveTransitionItem);
    moveTransition[0].i32 = ARKUI_TRANSITION_EDGE_TOP;
    moveTransition[2].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_MOVE_TRANSITION, &moveTransitionItem);

    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_FOCUSABLE, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FOCUSABLE), nullptr);

    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_DEFAULT_FOCUS, &item);
    auto defaultFocusVal = nodeAPI->getAttribute(rootNode, NODE_DEFAULT_FOCUS);
    EXPECT_EQ(defaultFocusVal->value[0].i32, true);

    value4[0].f32 = val0;
    value4[1].f32 = val0;
    value4[2].f32 = val100;
    value4[3].f32 = val100;
    nodeAPI->setAttribute(rootNode, NODE_RESPONSE_REGION, &item4);
    auto responseRegionVal = nodeAPI->getAttribute(rootNode, NODE_RESPONSE_REGION);
    EXPECT_EQ(responseRegionVal->value[0].f32, val0);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RESPONSE_REGION, &item5), ARKUI_ERROR_CODE_PARAM_INVALID);

    value3[0].i32 = ARKUI_ALIGNMENT_TOP_START;
    value3[1].f32 = val0;
    value3[2].f32 = val0;
    item3.string = "test";
    item3.size = 0;
    nodeAPI->setAttribute(rootNode, NODE_OVERLAY, &item3);
    item3.size = 1;
    nodeAPI->setAttribute(rootNode, NODE_OVERLAY, &item3);
    item3.size = 2;
    nodeAPI->setAttribute(rootNode, NODE_OVERLAY, &item3);
    item3.size = 3;
    nodeAPI->setAttribute(rootNode, NODE_OVERLAY, &item3);
    auto overlayVal = nodeAPI->getAttribute(rootNode, NODE_OVERLAY);
    EXPECT_EQ(overlayVal->value[0].i32, static_cast<int32_t>(ARKUI_ALIGNMENT_TOP_START));

    ArkUI_NumberValue mask[] = {{.u32 = color}, {.u32 = color}, {.f32 = val10}, {.i32 = ARKUI_MASK_TYPE_RECTANGLE},
        {.f32 = size}, {.f32 = size}, {.f32 = val10}, {.f32 = val10}};
    ArkUI_AttributeItem maskItem = {mask, sizeof(mask) / sizeof(ArkUI_NumberValue)};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_NO_ERROR);
    auto maskVal = nodeAPI->getAttribute(rootNode, NODE_MASK);
    EXPECT_NE(maskVal, nullptr);
    mask[3].i32 = ARKUI_MASK_TYPE_CIRCLE;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_NO_ERROR);
    maskVal = nodeAPI->getAttribute(rootNode, NODE_MASK);
    EXPECT_NE(maskVal, nullptr);
    mask[3].i32 = ARKUI_MASK_TYPE_ELLIPSE;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_NO_ERROR);
    maskVal = nodeAPI->getAttribute(rootNode, NODE_MASK);
    EXPECT_NE(maskVal, nullptr);
    mask[3].i32 = ARKUI_MASK_TYPE_PATH;
    maskItem.string = pathCommands;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_NO_ERROR);
    maskVal = nodeAPI->getAttribute(rootNode, NODE_MASK);
    EXPECT_NE(maskVal, nullptr);
    value4[0].i32 = ARKUI_MASK_TYPE_PROGRESS;
    value4[1].f32 = val10;
    value4[2].f32 = val100;
    value4[3].u32 = color;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &item4), ARKUI_ERROR_CODE_NO_ERROR);
    maskVal = nodeAPI->getAttribute(rootNode, NODE_MASK);

    value2[0].i32 = ARKUI_BLEND_MODE_NONE;
    value2[1].i32 = BLEND_APPLY_TYPE_FAST;
    nodeAPI->setAttribute(rootNode, NODE_BLEND_MODE, &item2);
    auto blendModeVal = nodeAPI->getAttribute(rootNode, NODE_BLEND_MODE);
    EXPECT_EQ(blendModeVal->value[0].i32, static_cast<int32_t>(ARKUI_BLEND_MODE_NONE));

    value[0].i32 = ARKUI_DIRECTION_LTR;
    nodeAPI->setAttribute(rootNode, NODE_DIRECTION, &item);
    auto directionVal = nodeAPI->getAttribute(rootNode, NODE_DIRECTION);
    EXPECT_EQ(directionVal->value[0].i32, static_cast<int32_t>(ARKUI_DIRECTION_LTR));

    value4[0].f32 = val0;
    value4[1].f32 = size;
    value4[2].f32 = val0;
    value4[3].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_CONSTRAINT_SIZE, &item4);
    auto constraintSizeVal = nodeAPI->getAttribute(rootNode, NODE_CONSTRAINT_SIZE);
    EXPECT_EQ(constraintSizeVal->value[0].f32, val0);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_GRAY_SCALE, &item);
    auto grayScaleVal = nodeAPI->getAttribute(rootNode, NODE_GRAY_SCALE);
    EXPECT_EQ(grayScaleVal->value[0].f32, val05);
    value[0].f32 = val10;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRAY_SCALE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_INVERT, &item);
    auto invertVal = nodeAPI->getAttribute(rootNode, NODE_INVERT);
    EXPECT_EQ(invertVal->value[0].f32, val05);
    value[0].f32 = val10;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_INVERT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_SEPIA, &item);
    auto sepiaVal = nodeAPI->getAttribute(rootNode, NODE_SEPIA);
    EXPECT_EQ(sepiaVal->value[0].f32, val05);
    value[0].f32 = val10;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SEPIA, &item), ARKUI_ERROR_CODE_PARAM_INVALID);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_CONTRAST, &item);
    auto contrastVal = nodeAPI->getAttribute(rootNode, NODE_CONTRAST);
    EXPECT_EQ(contrastVal->value[0].f32, val05);
    value[0].f32 = val100;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CONTRAST, &item), ARKUI_ERROR_CODE_PARAM_INVALID);

    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_COLOR, &item);
    auto foregroundColorVal = nodeAPI->getAttribute(rootNode, NODE_FOREGROUND_COLOR);
    EXPECT_NE(foregroundColorVal, nullptr);
    value[0].i32 = ARKUI_COLOR_STRATEGY_INVERT;
    nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_COLOR, &item);
    value[0].i32 = ARKUI_COLOR_STRATEGY_AVERAGE;
    nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_COLOR, &item);
    value[0].i32 = ARKUI_COLOR_STRATEGY_PRIMARY;
    nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_COLOR, &item);

    value2[0].f32 = val10;
    value2[1].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_OFFSET, &item2);
    auto offsetVal = nodeAPI->getAttribute(rootNode, NODE_OFFSET);
    EXPECT_EQ(offsetVal->value[0].f32, val10);

    value2[0].f32 = val10;
    value2[1].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_MARK_ANCHOR, &item2);
    auto maskAnchorVal = nodeAPI->getAttribute(rootNode, NODE_MARK_ANCHOR);
    EXPECT_EQ(maskAnchorVal->value[0].f32, val10);

    value2[0].f32 = val10;
    value2[1].f32 = val10;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_POSITION, &item2);
    auto backgroundImagePositionVal = nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_IMAGE_POSITION);
    EXPECT_EQ(backgroundImagePositionVal->value[0].f32, val10);

    value[0].i32 = 0;
    nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_GROUP, &item);
    auto accessibilityGroupVal = nodeAPI->getAttribute(rootNode, NODE_ACCESSIBILITY_GROUP);
    EXPECT_EQ(accessibilityGroupVal->value[0].i32, 0);

    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_TEXT, &item);
    auto accessibilityTextVal = nodeAPI->getAttribute(rootNode, NODE_ACCESSIBILITY_TEXT);
    EXPECT_NE(accessibilityTextVal, nullptr);
    nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_DESCRIPTION, &item);
    auto accessibilityDescVal = nodeAPI->getAttribute(rootNode, NODE_ACCESSIBILITY_DESCRIPTION);
    EXPECT_NE(accessibilityDescVal, nullptr);

    value[0].i32 = ARKUI_ACCESSIBILITY_MODE_AUTO;
    nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_MODE, &item);
    auto accessibilityModeVal = nodeAPI->getAttribute(rootNode, NODE_ACCESSIBILITY_MODE);
    EXPECT_EQ(accessibilityModeVal->value[0].i32, static_cast<int32_t>(ARKUI_ACCESSIBILITY_MODE_AUTO));

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_ASPECT_RATIO, &item);
    auto aspectRatioVal = nodeAPI->getAttribute(rootNode, NODE_ASPECT_RATIO);
    EXPECT_EQ(aspectRatioVal->value[0].f32, val05);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_LAYOUT_WEIGHT, &item);
    auto layoutWeightVal = nodeAPI->getAttribute(rootNode, NODE_LAYOUT_WEIGHT);
    EXPECT_EQ(layoutWeightVal->value[0].f32, val05);

    value[0].u32 = 0;
    nodeAPI->setAttribute(rootNode, NODE_DISPLAY_PRIORITY, &item);
    auto displayPriorityVal = nodeAPI->getAttribute(rootNode, NODE_DISPLAY_PRIORITY);
    EXPECT_EQ(displayPriorityVal->value[0].u32, 0);

    value4[0].f32 = val10;
    value4[1].f32 = val10;
    value4[2].f32 = val10;
    value4[3].f32 = val10;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OUTLINE_WIDTH, &item4), ARKUI_ERROR_CODE_NO_ERROR);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_WIDTH_PERCENT, &item);
    auto widthPercentVal = nodeAPI->getAttribute(rootNode, NODE_WIDTH_PERCENT);
    EXPECT_EQ(widthPercentVal->value[0].f32, val05);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_HEIGHT_PERCENT, &item);
    auto heightPercentVal = nodeAPI->getAttribute(rootNode, NODE_HEIGHT_PERCENT);
    EXPECT_EQ(heightPercentVal->value[0].f32, val05);

    value[0].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_PADDING_PERCENT, &item);
    auto paddingPercentVal = nodeAPI->getAttribute(rootNode, NODE_PADDING_PERCENT);
    EXPECT_EQ(paddingPercentVal->value[0].f32, val01);
    value4[0].f32 = val01;
    value4[1].f32 = val01;
    value4[2].f32 = val01;
    value4[3].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_PADDING_PERCENT, &item4);
    auto paddingPercentVal4 = nodeAPI->getAttribute(rootNode, NODE_PADDING_PERCENT);
    EXPECT_EQ(paddingPercentVal4->value[0].f32, val01);

    nodeAPI->setAttribute(rootNode, NODE_MARGIN_PERCENT, &item);
    auto marginPercentVal = nodeAPI->getAttribute(rootNode, NODE_MARGIN_PERCENT);
    EXPECT_EQ(marginPercentVal->value[0].f32, val01);
    nodeAPI->setAttribute(rootNode, NODE_MARGIN_PERCENT, &item4);
    auto marginPercentVal4 = nodeAPI->getAttribute(rootNode, NODE_MARGIN_PERCENT);
    EXPECT_EQ(marginPercentVal4->value[0].f32, val01);
    
    value[0].i32 = false;
    item.string = "test";
    item.size = 0;
    nodeAPI->setAttribute(rootNode, NODE_GEOMETRY_TRANSITION, &item);
    item.size = 1;
    nodeAPI->setAttribute(rootNode, NODE_GEOMETRY_TRANSITION, &item);
    auto geometryTransitionVal = nodeAPI->getAttribute(rootNode, NODE_GEOMETRY_TRANSITION);
    EXPECT_EQ(geometryTransitionVal->value[0].i32, 0);

    value[0].i32 = ARKUI_RENDER_FIT_CENTER;
    nodeAPI->setAttribute(rootNode, NODE_RENDER_FIT, &item);
    auto renderFitVal = nodeAPI->getAttribute(rootNode, NODE_RENDER_FIT);
    EXPECT_EQ(renderFitVal->value[0].i32, static_cast<int32_t>(ARKUI_RENDER_FIT_CENTER));

    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_OUTLINE_COLOR, &item);
    auto outlineColorVal = nodeAPI->getAttribute(rootNode, NODE_OUTLINE_COLOR);
    EXPECT_EQ(outlineColorVal->value[0].u32, color);
    value4[0].u32 = color;
    value4[1].u32 = color;
    value4[2].u32 = color;
    value4[3].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_OUTLINE_COLOR, &item4);
    auto outlineColorVal4 = nodeAPI->getAttribute(rootNode, NODE_OUTLINE_COLOR);
    EXPECT_EQ(outlineColorVal4->value[0].u32, color);

    value2[0].f32 = size;
    value2[1].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_SIZE, &item2);
    auto sizeVal = nodeAPI->getAttribute(rootNode, NODE_SIZE);
    EXPECT_EQ(sizeVal->value[0].f32, size);

    value[0].i32 = false;
    nodeAPI->setAttribute(rootNode, NODE_RENDER_GROUP, &item);
    auto renderGroupVal = nodeAPI->getAttribute(rootNode, NODE_RENDER_GROUP);
    EXPECT_EQ(renderGroupVal->value[0].i32, 0);

    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_COLOR_BLEND, &item);
    auto colorBlendVal = nodeAPI->getAttribute(rootNode, NODE_COLOR_BLEND);
    EXPECT_EQ(colorBlendVal->value[0].u32, color);
    
    value4[0].i32 = 0;
    value4[1].i32 = 0;
    value4[2].i32 = 100;
    value4[3].i32 = 100;
    nodeAPI->setAttribute(rootNode, NODE_LAYOUT_RECT, &item4);
    auto layoutRectVal = nodeAPI->getAttribute(rootNode, NODE_LAYOUT_RECT);
    EXPECT_EQ(layoutRectVal->value[0].i32, 0);

    value[0].i32 = 1;
    nodeAPI->setAttribute(rootNode, NODE_FOCUS_ON_TOUCH, &item);
    auto onTouchVal = nodeAPI->getAttribute(rootNode, NODE_FOCUS_ON_TOUCH);
    EXPECT_EQ(onTouchVal->value[0].i32, 1);

    value[0].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item);
    auto borderWidthPercentVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT);
    EXPECT_EQ(borderWidthPercentVal->value[0].f32, val01);
    value4[0].f32 = val01;
    value4[1].f32 = val01;
    value4[2].f32 = val01;
    value4[3].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item4);
    auto borderWidthPercentVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT);
    EXPECT_EQ(borderWidthPercentVal4->value[0].f32, val01);

    value[0].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item);
    auto borderRadiusPercentVal = nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT);
    EXPECT_EQ(borderRadiusPercentVal->value[0].f32, val01);
    value4[0].f32 = val01;
    value4[1].f32 = val01;
    value4[2].f32 = val01;
    value4[3].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item4);
    auto borderRadiusPercentVal4 = nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT);
    EXPECT_EQ(borderRadiusPercentVal4->value[0].f32, val01);

    auto accessibilityIdVal = nodeAPI->getAttribute(rootNode, NODE_ACCESSIBILITY_ID);
    EXPECT_NE(accessibilityIdVal, nullptr);

    value[0].u32 = ARKUI_ACCESSIBILITY_ACTION_CLICK;
    nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_ACTIONS, &item);
    auto accessibilityActionVal = nodeAPI->getAttribute(rootNode, NODE_ACCESSIBILITY_ACTIONS);
    EXPECT_NE(accessibilityActionVal, nullptr);
    

    value[0].u32 = ARKUI_NODE_STACK;
    nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_ROLE, &item);
    auto accessibilityRoleVal = nodeAPI->getAttribute(rootNode, NODE_ACCESSIBILITY_ROLE);
    EXPECT_NE(accessibilityRoleVal, nullptr);

    value2[0].u32 = ARKUI_SAFE_AREA_TYPE_SYSTEM;
    value2[1].u32 = ARKUI_SAFE_AREA_EDGE_TOP;
    nodeAPI->setAttribute(rootNode, NODE_EXPAND_SAFE_AREA, &item2);
    auto extendSafeAreaVal = nodeAPI->getAttribute(rootNode, NODE_EXPAND_SAFE_AREA);
    EXPECT_EQ(extendSafeAreaVal->value[0].u32, ARKUI_SAFE_AREA_TYPE_SYSTEM);

    value[0].i32 = ARKUI_ALIGNMENT_CENTER;
    nodeAPI->setAttribute(rootNode, NODE_STACK_ALIGN_CONTENT, &item);
    auto stackAlignVal = nodeAPI->getAttribute(rootNode, NODE_STACK_ALIGN_CONTENT);
    EXPECT_EQ(stackAlignVal->value[0].i32, static_cast<int32_t>(ARKUI_ALIGNMENT_CENTER));

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TRANSITION), nullptr);

    value[0].f32 = val05;
    nodeAPI->setAttribute(rootNode, NODE_VISIBLE_AREA_CHANGE_RATIO, &item);
    value[0].f32 = val01;
    nodeAPI->setAttribute(rootNode, NODE_VISIBLE_AREA_CHANGE_RATIO, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_VISIBLE_AREA_CHANGE_RATIO), nullptr);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_HEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_PADDING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_MARGIN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ID), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ENABLED), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TRANSLATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCALE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ROTATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BRIGHTNESS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SATURATION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BLUR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LINEAR_GRADIENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ALIGNMENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_OPACITY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_RADIUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_Z_INDEX), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_VISIBILITY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CLIP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CLIP_SHAPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TRANSFORM), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_HIT_TEST_BEHAVIOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_POSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SHADOW), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CUSTOM_SHADOW), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_IMAGE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TRANSFORM_CENTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_OPACITY_TRANSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ROTATE_TRANSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCALE_TRANSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TRANSLATE_TRANSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_MOVE_TRANSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FOCUSABLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DEFAULT_FOCUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RESPONSE_REGION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_OVERLAY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWEEP_GRADIENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RADIAL_GRADIENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_MASK), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BLEND_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DIRECTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CONSTRAINT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GRAY_SCALE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_INVERT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SEPIA), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CONTRAST), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FOREGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_MARK_ANCHOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_IMAGE_POSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_GROUP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_TEXT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_DESCRIPTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FOCUS_STATUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ASPECT_RATIO), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LAYOUT_WEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DISPLAY_PRIORITY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_OUTLINE_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WIDTH_PERCENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_HEIGHT_PERCENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_PADDING_PERCENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_MARGIN_PERCENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GEOMETRY_TRANSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RENDER_FIT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_OUTLINE_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RENDER_GROUP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_COLOR_BLEND), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FOREGROUND_BLUR_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LAYOUT_RECT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FOCUS_ON_TOUCH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_ID), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_ACTIONS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_ROLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_STATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ACCESSIBILITY_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_EXPAND_SAFE_AREA), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_VISIBLE_AREA_CHANGE_RATIO), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TRANSITION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_STACK_ALIGN_CONTENT), ARKUI_ERROR_CODE_NO_ERROR);
}

/**
 * @tc.name: NativeNodeTest006
 * @tc.desc: Test ConvertOriginEventType function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest006, TestSize.Level1)
{
    int32_t ret;
    int32_t nodeType = static_cast<int32_t>(ARKUI_NODE_TEXT_INPUT);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_SUBMIT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_SUBMIT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_CUT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_CUT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_PASTE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_PASTE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_TEXT_SELECTION_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_TEXT_SELECTION_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_EDIT_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_EDIT_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_CONTENT_SIZE_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_CONTENT_SIZE_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_INPUT_FILTER_ERROR, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_INPUT_FILTER_ERROR));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_CONTENT_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_CONTENT_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_WILL_INSERT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_WILL_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_DID_INSERT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_DID_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_WILL_DELETE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_WILL_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_DID_DELETE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_DID_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_INPUT_ON_CHANGE_WITH_PREVIEW_TEXT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_INPUT_CHANGE_WITH_PREVIEW_TEXT));

    nodeType = static_cast<int32_t>(ARKUI_NODE_SCROLL);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_FRAME_BEGIN));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_WILL_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_WILL_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_DID_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_DID_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_STOP, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_STOP));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_EDGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_EDGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_REACH_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_REACH_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_REACH_END, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SCROLL_REACH_END));

    nodeType = static_cast<int32_t>(ARKUI_NODE_LIST);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_SCROLL_FRAME_BEGIN));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_SCROLL_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_STOP, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_SCROLL_STOP));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_REACH_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_REACH_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_REACH_END, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_REACH_END));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_LIST_ON_SCROLL_INDEX, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_SCROLL_INDEX));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_LIST_ON_DID_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_DID_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_LIST_ON_WILL_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_LIST_WILL_SCROLL));
    
    nodeType = static_cast<int32_t>(ARKUI_NODE_WATER_FLOW);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_SCROLL_FRAME_BEGIN));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_SCROLL_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_SCROLL_STOP, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_SCROLL_STOP));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_WILL_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_WILL_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_REACH_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_REACH_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SCROLL_EVENT_ON_REACH_END, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_REACH_END));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_WATER_FLOW_ON_DID_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_DID_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_WATER_FLOW_ON_SCROLL_INDEX, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_WATER_FLOW_SCROLL_INDEX));

    nodeType = static_cast<int32_t>(ARKUI_NODE_STACK);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_EVENT_ON_APPEAR, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_APPEAR));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_EVENT_ON_DISAPPEAR, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_DISAPPEAR));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_EVENT_ON_AREA_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_AREA_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_FOCUS, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_FOCUS));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_BLUR, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_BLUR));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TOUCH_EVENT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TOUCH));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_CLICK, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_CLICK));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_TOUCH_INTERCEPT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TOUCH_INTERCEPT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_EVENT_ON_VISIBLE_AREA_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_VISIBLE_AREA_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_HOVER, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_HOVER));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_MOUSE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_MOUSE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_EVENT_ON_ATTACH, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_ATTACH));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_EVENT_ON_DETACH, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_DETACH));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_ON_ACCESSIBILITY_ACTIONS, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_ACCESSIBILITY_ACTIONS));

    nodeType = static_cast<int32_t>(ARKUI_NODE_TEXT_AREA);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXTAREA_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_INPUT_FILTER_ERROR, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_AREA_INPUT_FILTER_ERROR));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_EDIT_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXTAREA_EDIT_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_SUBMIT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXTAREA_ON_SUBMIT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_CONTENT_SIZE_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXTAREA_CONTENT_SIZE_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_PASTE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXTAREA_PASTE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_TEXT_SELECTION_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXTAREA_TEXT_SELECTION_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_CONTENT_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_AREA_CONTENT_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_WILL_INSERT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_AREA_WILL_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_DID_INSERT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_AREA_DID_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_WILL_DELETE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_AREA_WILL_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_DID_DELETE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_AREA_DID_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_AREA_ON_CHANGE_WITH_PREVIEW_TEXT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_AREA_CHANGE_WITH_PREVIEW_TEXT));


    nodeType = static_cast<int32_t>(ARKUI_NODE_REFRESH);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_REFRESH_STATE_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_REFRESH_STATE_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_REFRESH_ON_REFRESH, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_REFRESH_REFRESHING));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_REFRESH_ON_OFFSET_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_REFRESH_ON_OFFSET_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_TOGGLE);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TOGGLE_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TOGGLE_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_IMAGE);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ON_COMPLETE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_COMPLETE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ON_ERROR, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_ERROR));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ON_SVG_PLAY_FINISH, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_SVG_PLAY_FINISH));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ON_DOWNLOAD_PROGRESS, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_DOWNLOAD_PROGRESS));

    nodeType = static_cast<int32_t>(ARKUI_NODE_DATE_PICKER);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_DATE_PICKER_EVENT_ON_DATE_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_DATE_PICKER_DATE_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_TIME_PICKER);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TIME_PICKER_EVENT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TIME_PICKER_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_TEXT_PICKER);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_PICKER_EVENT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_PICKER_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_PICKER_EVENT_ON_SCROLL_STOP, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_TEXT_PICKER_SCROLL_STOP));

    nodeType = static_cast<int32_t>(ARKUI_NODE_CALENDAR_PICKER);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_CALENDAR_PICKER_EVENT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_CALENDAR_PICKER_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_CHECKBOX);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_CHECKBOX_EVENT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_CHECKBOX_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_SLIDER);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SLIDER_EVENT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SLIDER_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_SWIPER);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SWIPER_EVENT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SWIPER_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SWIPER_EVENT_ON_ANIMATION_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SWIPER_ANIMATION_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SWIPER_EVENT_ON_ANIMATION_END, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SWIPER_ANIMATION_END));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SWIPER_EVENT_ON_GESTURE_SWIPE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SWIPER_GESTURE_SWIPE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_SWIPER_DID_CONTENT_SCROLL));

    nodeType = static_cast<int32_t>(ARKUI_NODE_TEXT);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_TEXT_ON_DETECT_RESULT_UPDATE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_DETECT_RESULT_UPDATE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_RADIO);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_RADIO_EVENT_ON_CHANGE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_RADIO_CHANGE));

    nodeType = static_cast<int32_t>(ARKUI_NODE_IMAGE_ANIMATOR);
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ANIMATOR_EVENT_ON_START, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_ANIMATOR_ON_START));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ANIMATOR_EVENT_ON_CANCEL, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_ANIMATOR_ON_CANCEL));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ANIMATOR_EVENT_ON_PAUSE, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_ANIMATOR_ON_PAUSE));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ANIMATOR_EVENT_ON_REPEAT, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_ANIMATOR_ON_REPEAT));
    ret = OHOS::Ace::NodeModel::ConvertOriginEventType(NODE_IMAGE_ANIMATOR_EVENT_ON_FINISH, nodeType);
    EXPECT_EQ(ret, static_cast<int32_t>(ON_IMAGE_ANIMATOR_ON_FINISH));
}

/**
 * @tc.name: NativeNodeTest007
 * @tc.desc: Test ConvertToNodeEventType function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest007, TestSize.Level1)
{
    int32_t ret;
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_SUBMIT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_SUBMIT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_CUT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_CUT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_PASTE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_PASTE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_TEXT_SELECTION_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_TEXT_SELECTION_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_EDIT_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_EDIT_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_CONTENT_SIZE_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_CONTENT_SIZE_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_INPUT_FILTER_ERROR);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_INPUT_FILTER_ERROR));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_CONTENT_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_CONTENT_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_WILL_INSERT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_WILL_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_DID_INSERT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_DID_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_WILL_DELETE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_WILL_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_DID_DELETE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_DID_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_INPUT_CHANGE_WITH_PREVIEW_TEXT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_INPUT_ON_CHANGE_WITH_PREVIEW_TEXT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_FRAME_BEGIN);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_WILL_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_WILL_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_DID_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_DID_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_STOP);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_STOP));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_EDGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_EDGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_REACH_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_REACH_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SCROLL_REACH_END);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_REACH_END));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_SCROLL_FRAME_BEGIN);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_SCROLL_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_SCROLL_STOP);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_STOP));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_REACH_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_REACH_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_REACH_END);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_REACH_END));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_SCROLL_INDEX);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_LIST_ON_SCROLL_INDEX));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_DID_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_LIST_ON_DID_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_LIST_WILL_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_LIST_ON_WILL_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_SCROLL_FRAME_BEGIN);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_SCROLL_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_SCROLL_STOP);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_SCROLL_STOP));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_WILL_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_WILL_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_REACH_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_REACH_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_REACH_END);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SCROLL_EVENT_ON_REACH_END));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_DID_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_WATER_FLOW_ON_DID_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_WATER_FLOW_SCROLL_INDEX);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_WATER_FLOW_ON_SCROLL_INDEX));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_APPEAR);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_EVENT_ON_APPEAR));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_DISAPPEAR);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_EVENT_ON_DISAPPEAR));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_AREA_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_EVENT_ON_AREA_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_FOCUS);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_FOCUS));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_BLUR);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_BLUR));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TOUCH);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TOUCH_EVENT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_CLICK);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_CLICK));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TOUCH_INTERCEPT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_TOUCH_INTERCEPT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_VISIBLE_AREA_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_EVENT_ON_VISIBLE_AREA_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_HOVER);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_HOVER));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_MOUSE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_MOUSE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_ATTACH);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_EVENT_ON_ATTACH));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_DETACH);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_EVENT_ON_DETACH));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_ACCESSIBILITY_ACTIONS);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_ON_ACCESSIBILITY_ACTIONS));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXTAREA_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_AREA_INPUT_FILTER_ERROR);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_INPUT_FILTER_ERROR));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXTAREA_EDIT_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_EDIT_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXTAREA_ON_SUBMIT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_SUBMIT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXTAREA_CONTENT_SIZE_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_CONTENT_SIZE_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXTAREA_PASTE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_PASTE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXTAREA_TEXT_SELECTION_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_TEXT_SELECTION_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_AREA_CONTENT_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_CONTENT_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_AREA_WILL_INSERT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_WILL_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_AREA_DID_INSERT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_DID_INSERT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_AREA_WILL_DELETE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_WILL_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_AREA_DID_DELETE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_DID_DELETE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_AREA_CHANGE_WITH_PREVIEW_TEXT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_AREA_ON_CHANGE_WITH_PREVIEW_TEXT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_REFRESH_STATE_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_REFRESH_STATE_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_REFRESH_REFRESHING);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_REFRESH_ON_REFRESH));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_REFRESH_ON_OFFSET_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_REFRESH_ON_OFFSET_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TOGGLE_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TOGGLE_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_COMPLETE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ON_COMPLETE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_ERROR);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ON_ERROR));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_SVG_PLAY_FINISH);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ON_SVG_PLAY_FINISH));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_DOWNLOAD_PROGRESS);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ON_DOWNLOAD_PROGRESS));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_DATE_PICKER_DATE_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_DATE_PICKER_EVENT_ON_DATE_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TIME_PICKER_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TIME_PICKER_EVENT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_PICKER_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_PICKER_EVENT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_TEXT_PICKER_SCROLL_STOP);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_PICKER_EVENT_ON_SCROLL_STOP));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_CALENDAR_PICKER_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_CALENDAR_PICKER_EVENT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_CHECKBOX_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_CHECKBOX_EVENT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SLIDER_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SLIDER_EVENT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SWIPER_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SWIPER_EVENT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SWIPER_ANIMATION_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SWIPER_EVENT_ON_ANIMATION_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SWIPER_ANIMATION_END);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SWIPER_EVENT_ON_ANIMATION_END));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SWIPER_GESTURE_SWIPE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SWIPER_EVENT_ON_GESTURE_SWIPE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_SWIPER_DID_CONTENT_SCROLL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_DETECT_RESULT_UPDATE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_TEXT_ON_DETECT_RESULT_UPDATE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_RADIO_CHANGE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_RADIO_EVENT_ON_CHANGE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_ANIMATOR_ON_START);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ANIMATOR_EVENT_ON_START));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_ANIMATOR_ON_CANCEL);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ANIMATOR_EVENT_ON_CANCEL));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_ANIMATOR_ON_PAUSE);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ANIMATOR_EVENT_ON_PAUSE));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_ANIMATOR_ON_REPEAT);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ANIMATOR_EVENT_ON_REPEAT));
    ret = OHOS::Ace::NodeModel::ConvertToNodeEventType(ON_IMAGE_ANIMATOR_ON_FINISH);
    EXPECT_EQ(ret, static_cast<int32_t>(NODE_IMAGE_ANIMATOR_EVENT_ON_FINISH));
}

/**
 * @tc.name: NativeNodeTest008
 * @tc.desc: Test textNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest008, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_TEXT, nullptr, true});
    uint32_t color = 0XFFFF0000;
    float size = 10.0f;
    ArkUI_NumberValue value[] = {{.u32 = color}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue shadow[] = {{.f32 = size}, {.i32 = ARKUI_SHADOW_TYPE_COLOR}, {.u32 = color}, {.f32 = size},
        {.f32 = size}};
    ArkUI_AttributeItem shadowItem = {shadow, sizeof(shadow) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue fontValue[] = {{.f32 = size}, {.i32 = ARKUI_FONT_WEIGHT_NORMAL},
        {.i32 = ARKUI_FONT_STYLE_NORMAL}};
    ArkUI_AttributeItem fontItem = {fontValue, sizeof(fontValue) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue decoration[] = {{.i32 = ARKUI_TEXT_DECORATION_TYPE_UNDERLINE}, {.u32 = color},
        {.i32 = ARKUI_TEXT_DECORATION_STYLE_SOLID}};
    ArkUI_AttributeItem decorationItem = {decoration, sizeof(decoration) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_FONT_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_SELECTED_BACKGROUND_COLOR, &item);
    value[0].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_BASELINE_OFFSET, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_HEIGHT, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_LETTER_SPACING, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INDENT, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_SPACING, &item);

    value[0].i32 = ARKUI_FONT_WEIGHT_W100;
    nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &item);
    value[0].i32 = ARKUI_FONT_STYLE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_FONT_STYLE, &item);
    value[0].i32 = ARKUI_TEXT_HEIGHT_ADAPTIVE_POLICY_MAX_LINES_FIRST;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_HEIGHT_ADAPTIVE_POLICY, &item);
    value[0].i32 = ARKUI_COPY_OPTIONS_NONE;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_COPY_OPTION, &item);
    value[0].i32 = ARKUI_TEXT_DECORATION_TYPE_NONE;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_DECORATION, &item);
    value[0].i32 = ARKUI_TEXT_CASE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_CASE, &item);
    value[0].i32 = 1;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_LINES, &item);
    value[0].i32 = ARKUI_TEXT_ALIGNMENT_START;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_ALIGN, &item);
    value[0].i32 = ARKUI_TEXT_OVERFLOW_NONE;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_OVERFLOW, &item);
    value[0].i32 = ARKUI_WORD_BREAK_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_WORD_BREAK, &item);
    value[0].i32 = ARKUI_ELLIPSIS_MODE_START;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_ELLIPSIS_MODE, &item);
    value[0].i32 = ARKUI_TEXT_DATA_DETECTOR_TYPE_URL;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_ENABLE_DATA_DETECTOR_CONFIG, &item);

    value[0].i32 = false;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_ENABLE_DATA_DETECTOR, &item);

    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT_FEATURE, &item);

    nodeAPI->setAttribute(rootNode, NODE_TEXT_TEXT_SHADOW, &shadowItem);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT, &fontItem);
    fontItem.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT, &fontItem);
    nodeAPI->setAttribute(rootNode, NODE_FONT_FAMILY, &fontItem);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_CONTENT, &fontItem);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_DECORATION, &decorationItem);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_HALF_LEADING, &item);
    decoration[2].i32 = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_DECORATION, &decorationItem), ARKUI_ERROR_CODE_PARAM_INVALID);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_CONTENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_WEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_DECORATION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_CASE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_LETTER_SPACING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MAX_LINES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_ALIGN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_OVERFLOW), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_FAMILY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_COPY_OPTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_BASELINE_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_TEXT_SHADOW), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_FONT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_HEIGHT_ADAPTIVE_POLICY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INDENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_WORD_BREAK), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_ELLIPSIS_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_LINE_SPACING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_FONT_FEATURE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_ENABLE_DATA_DETECTOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_ENABLE_DATA_DETECTOR_CONFIG), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_SELECTED_BACKGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_HALF_LEADING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_CONTENT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_WEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_DECORATION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_CASE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_LETTER_SPACING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MAX_LINES), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_ALIGN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_OVERFLOW), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_FAMILY), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_COPY_OPTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_BASELINE_OFFSET), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_TEXT_SHADOW), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_FONT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_HEIGHT_ADAPTIVE_POLICY), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INDENT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_WORD_BREAK), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_ELLIPSIS_MODE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_LINE_SPACING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_FONT_FEATURE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_ENABLE_DATA_DETECTOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_ENABLE_DATA_DETECTOR_CONFIG), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_SELECTED_BACKGROUND_COLOR), nullptr);
    EXPECT_EQ(nodeAPI->getAttribute(rootNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_HALF_LEADING), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest009
 * @tc.desc: Test spanNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest009, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_SPAN);
    ASSERT_NE(rootNode, nullptr);
    uint32_t color = 0XFFFF0000;
    float size = 10.0f;
    ArkUI_NumberValue value[] = {{.u32 = color}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue shadow[] = {{.f32 = size}, {.i32 = ARKUI_SHADOW_TYPE_COLOR}, {.u32 = color}, {.f32 = size},
        {.f32 = size}};
    ArkUI_AttributeItem shadowItem = {shadow, sizeof(shadow) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue backgroundStyle[] = {{.u32 = color}, {.f32 = size}, {.f32 = size}, {.f32 = size},
        {.f32 = size}};
    ArkUI_AttributeItem backgroundStyleItem = {backgroundStyle, sizeof(backgroundStyle) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_FONT_COLOR, &item);
    value[0].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_SPAN_BASELINE_OFFSET, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_HEIGHT, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_LETTER_SPACING, &item);

    value[0].i32 = ARKUI_FONT_WEIGHT_W100;
    nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &item);
    value[0].i32 = ARKUI_FONT_STYLE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_FONT_STYLE, &item);
    value[0].i32 = ARKUI_TEXT_DECORATION_TYPE_NONE;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_DECORATION, &item);
    value[0].i32 = ARKUI_TEXT_CASE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_CASE, &item);
    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_FONT_FAMILY, &item);
    nodeAPI->setAttribute(rootNode, NODE_SPAN_CONTENT, &item);

    nodeAPI->setAttribute(rootNode, NODE_TEXT_TEXT_SHADOW, &shadowItem);
    nodeAPI->setAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE, &backgroundStyleItem);
    backgroundStyle[2].f32 = -1.0f;
    nodeAPI->setAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE, &backgroundStyleItem);
    backgroundStyleItem.size = 2;
    nodeAPI->setAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE, &backgroundStyleItem);
    backgroundStyle[1].f32 = -1.0f;
    nodeAPI->setAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE, &backgroundStyleItem);


    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SPAN_CONTENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SPAN_BASELINE_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_DECORATION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_WEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_CASE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_LETTER_SPACING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_FAMILY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_TEXT_SHADOW), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WIDTH, nullptr), ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED);
    EXPECT_EQ(nodeAPI->getAttribute(rootNode, NODE_WIDTH), nullptr);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WIDTH), ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SPAN_CONTENT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SPAN_BASELINE_OFFSET), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_DECORATION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_WEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_CASE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_LETTER_SPACING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_FAMILY), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_TEXT_SHADOW), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest010
 * @tc.desc: Test imageSpanNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest010, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_IMAGE_SPAN, nullptr, true});
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_IMAGE_SPAN_ALIGNMENT_BASELINE}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_VERTICAL_ALIGNMENT, &item);
    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_SRC, &item);
    value[0].f32 = 10.0f;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_BASELINE_OFFSET, &item);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_SPAN_SRC), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_SPAN_VERTICAL_ALIGNMENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_SPAN_ALT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_SPAN_BASELINE_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_SPAN_SRC), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_SPAN_VERTICAL_ALIGNMENT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_SPAN_ALT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_SPAN_BASELINE_OFFSET), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest011
 * @tc.desc: Test imageNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest011, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_IMAGE, nullptr, true});
    
    float border = 10.0f;
    ArkUI_NumberValue value[] = {{.f32 = border}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value20[] = {{.f32 = 1.0f}, {.f32 = 0.0f}, {.f32 = 0.0f}, {.f32 = 0.0f}, {.f32 = 0.0f},
        {.f32 = 0.0f}, {.f32 = 1.0f}, {.f32 = 0.0f}, {.f32 = 0.0f}, {.f32 = 0.0f},
        {.f32 = 0.0f}, {.f32 = 0.0f}, {.f32 = 1.0f}, {.f32 = 0.0f}, {.f32 = 0.0f},
        {.f32 = 0.0f}, {.f32 = 0.0f}, {.f32 = 0.0f}, {.f32 = 1.0f}, {.f32 = 0.0f}};
      
    ArkUI_AttributeItem item20 = {value20, sizeof(value20) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item);
    value[0].i32 = ARKUI_OBJECT_FIT_CONTAIN;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_FIT, &item);
    value[0].i32 = ARKUI_IMAGE_INTERPOLATION_NONE;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_INTERPOLATION, &item);
    value[0].i32 = ARKUI_IMAGE_REPEAT_X;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_REPEAT, &item);
    value[0].i32 = ARKUI_IMAGE_RENDER_MODE_ORIGINAL;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_RENDER_MODE, &item);
    value[0].i32 = 1;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_FIT_ORIGINAL_SIZE, &item);

    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_AUTO_RESIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_RESIZABLE, &item);
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_DRAGGABLE, &item);

    value[0].u32 = 0xFFFF0000;
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_FILL_COLOR, &item);

    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_SRC, &item);
    nodeAPI->setAttribute(rootNode, NODE_IMAGE_ALT, &item);

    nodeAPI->setAttribute(rootNode, NODE_IMAGE_COLOR_FILTER, &item20);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_SRC), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_OBJECT_FIT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_INTERPOLATION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_OBJECT_REPEAT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_COLOR_FILTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_AUTO_RESIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ALT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_DRAGGABLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_RENDER_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_FIT_ORIGINAL_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_FILL_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_RESIZABLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_RADIUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_SRC), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_OBJECT_FIT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_INTERPOLATION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_OBJECT_REPEAT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_COLOR_FILTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_AUTO_RESIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ALT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_DRAGGABLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_RENDER_MODE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_FILL_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_RESIZABLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_FIT_ORIGINAL_SIZE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest012
 * @tc.desc: Test toggleNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest012, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_TOGGLE);
    ASSERT_NE(rootNode, nullptr);
    uint32_t color = 0XFFFF0000;
    ArkUI_NumberValue value[] = {{.u32 = color}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_TOGGLE_SELECTED_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TOGGLE_SWITCH_POINT_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TOGGLE_VALUE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TOGGLE_UNSELECTED_COLOR, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TOGGLE_SELECTED_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TOGGLE_SWITCH_POINT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TOGGLE_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TOGGLE_UNSELECTED_COLOR), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TOGGLE_SELECTED_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TOGGLE_SWITCH_POINT_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TOGGLE_VALUE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TOGGLE_UNSELECTED_COLOR), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest013
 * @tc.desc: Test loadingProgressNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest013, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_LOADING_PROGRESS, nullptr, true});
    ASSERT_NE(rootNode, nullptr);
    uint32_t color = 0XFFFF0000;
    ArkUI_NumberValue value[] = {{.u32 = color}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_LOADING_PROGRESS_COLOR, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_LOADING_PROGRESS_ENABLE_LOADING, &item);
    value[0].i32 = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LOADING_PROGRESS_ENABLE_LOADING, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LOADING_PROGRESS_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LOADING_PROGRESS_ENABLE_LOADING), ARKUI_ERROR_CODE_NO_ERROR);
    
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LOADING_PROGRESS_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LOADING_PROGRESS_ENABLE_LOADING), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest014
 * @tc.desc: Test textinputNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest014, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_TEXT_INPUT, nullptr, true});
    uint32_t color = 0XFFFF0000;
    float padding = 10.0f;
    ArkUI_NumberValue value[] = {{.u32 = color}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value4[] = {{.u32 = color}, {.u32 = color}, {.u32 = color}, {.u32 = color}};
    ArkUI_AttributeItem item4 = {value4, sizeof(value4) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue fontValue[] = {{.f32 = padding}, {.i32 = ARKUI_FONT_STYLE_NORMAL},
        {.i32 = ARKUI_FONT_WEIGHT_W100}};
    ArkUI_AttributeItem fontItem = {fontValue, sizeof(fontValue) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue cancelBtn[] = {{.i32 = ARKUI_CANCELBUTTON_STYLE_CONSTANT}, {.f32 = padding}, {.u32 = color}};
    ArkUI_AttributeItem cancelBtnItem = {cancelBtn, sizeof(cancelBtn) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue selection[] = {{.i32 = 0}, {.i32 = 1}};
    ArkUI_AttributeItem selectionItem = {selection, sizeof(selection) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_FONT_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CARET_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_SELECTED_BACKGROUND_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_UNDERLINE_COLOR, &item4);
    value[0].f32 = padding;
    nodeAPI->setAttribute(rootNode, NODE_PADDING, &item);
    nodeAPI->setAttribute(rootNode, NODE_MARGIN, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item);
    nodeAPI->setAttribute(rootNode, NODE_MARGIN_PERCENT, &item);
    nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CARET_STYLE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_HEIGHT, &item);
    value[0].i32 = ARKUI_BORDER_STYLE_SOLID;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &item);
    value[0].i32 = ARKUI_FONT_WEIGHT_W100;
    nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &item);
    value[0].i32 = ARKUI_FONT_STYLE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_FONT_STYLE, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_SHOW_UNDERLINE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_MAX_LENGTH, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_SHOW_PASSWORD_ICON, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_EDITING, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_SELECTION_MENU_HIDDEN, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_AUTO_FILL, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_SELECT_ALL, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CARET_OFFSET, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_NUMBER_OF_LINES, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_BLUR_ON_SUBMIT, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_SHOW_KEYBOARD_ON_FOCUS, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_LINES, &item);
    value[0].i32 = false;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_EDITING, &item);
    value[0].i32 = ARKUI_ENTER_KEY_TYPE_GO;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_ENTER_KEY_TYPE, &item);
    value[0].i32 = ARKUI_TEXTINPUT_TYPE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_TYPE, &item);
    value[0].i32 = ARKUI_TEXTINPUT_CONTENT_TYPE_USER_NAME;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CONTENT_TYPE, &item);
    value[0].i32 = ARKUI_TEXTINPUT_CONTENT_TYPE_FORMAT_ADDRESS;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CONTENT_TYPE, &item);
    value[0].i32 = ARKUI_TEXTINPUT_STYLE_DEFAULT;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_STYLE, &item);
    value[0].i32 = ARKUI_WORD_BREAK_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_WORD_BREAK, &item);
    value[0].i32 = ARKUI_TEXT_ALIGNMENT_START;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_ALIGN, &item);
    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_TEXT, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_PASSWORD_RULES, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_INPUT_FILTER, &item);
    nodeAPI->setAttribute(rootNode, NODE_FONT_FAMILY, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT_FEATURE, &item);
    item.object = rootNode;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CUSTOM_KEYBOARD, &item);

    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_FONT, &fontItem);
    cancelBtnItem.size = 1;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CANCEL_BUTTON, &cancelBtnItem);
    cancelBtnItem.size = 2;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CANCEL_BUTTON, &cancelBtnItem);
    cancelBtnItem.size = sizeof(cancelBtn) / sizeof(ArkUI_NumberValue);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CANCEL_BUTTON, &cancelBtnItem);
    cancelBtnItem.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CANCEL_BUTTON, &cancelBtnItem);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_TEXT_SELECTION, &selectionItem);
    selection[0].i32 = 2;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_TEXT_SELECTION, &selectionItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].f32 = 50;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_LETTER_SPACING, &item);
    value[0].i32 = false;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_PREVIEW_TEXT, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_TEXT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_CARET_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_CARET_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_SHOW_UNDERLINE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_MAX_LENGTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_ENTER_KEY_TYPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_FONT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_TYPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_SELECTED_BACKGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_SHOW_PASSWORD_ICON), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_CANCEL_BUTTON), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_TEXT_SELECTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_UNDERLINE_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_AUTO_FILL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_CONTENT_TYPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_PASSWORD_RULES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_SELECT_ALL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_INPUT_FILTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_CARET_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_SELECTION_MENU_HIDDEN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_BLUR_ON_SUBMIT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_CUSTOM_KEYBOARD), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_WORD_BREAK), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_SHOW_KEYBOARD_ON_FOCUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_NUMBER_OF_LINES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_PADDING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_MARGIN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_RADIUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_WEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_FAMILY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MAX_LINES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_ALIGN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_FONT_FEATURE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_LETTER_SPACING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_PREVIEW_TEXT), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_TEXT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CARET_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CARET_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_SHOW_UNDERLINE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_MAX_LENGTH), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_ENTER_KEY_TYPE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_FONT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_TYPE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_SELECTED_BACKGROUND_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_SHOW_PASSWORD_ICON), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_EDITING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CANCEL_BUTTON), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_TEXT_SELECTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_UNDERLINE_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_AUTO_FILL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CONTENT_TYPE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_PASSWORD_RULES), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_SELECT_ALL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_INPUT_FILTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CARET_OFFSET), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CONTENT_RECT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CONTENT_LINE_COUNT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_SELECTION_MENU_HIDDEN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_BLUR_ON_SUBMIT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_CUSTOM_KEYBOARD), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_WORD_BREAK), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_SHOW_KEYBOARD_ON_FOCUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_NUMBER_OF_LINES), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_PADDING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_MARGIN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_WEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_FAMILY), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MAX_LINES), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_ALIGN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_FONT_FEATURE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_LETTER_SPACING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_PREVIEW_TEXT), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest015
 * @tc.desc: Test textareaNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest015, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_TEXT_AREA, nullptr, true});
    uint32_t color = 0XFFFF0000;
    float padding = 10.0f;
    ArkUI_NumberValue value[] = {{.u32 = color}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue fontValue[] = {{.f32 = padding}, {.i32 = ARKUI_FONT_STYLE_NORMAL},
        {.i32 = ARKUI_FONT_WEIGHT_W100}};
    ArkUI_AttributeItem fontItem = {fontValue, sizeof(fontValue) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue showCounter[] = {{.i32 = true}, {.f32 = 50}, {.i32 = true}};
    ArkUI_AttributeItem showCounterItem = {fontValue, sizeof(showCounter) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_FONT_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_CARET_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER_COLOR, &item);
    value[0].f32 = padding;
    nodeAPI->setAttribute(rootNode, NODE_PADDING, &item);
    nodeAPI->setAttribute(rootNode, NODE_MARGIN, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item);
    nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item);
    nodeAPI->setAttribute(rootNode, NODE_MARGIN_PERCENT, &item);
    nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_HEIGHT, &item);
    value[0].i32 = ARKUI_BORDER_STYLE_SOLID;
    nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &item);
    value[0].i32 = ARKUI_FONT_WEIGHT_W100;
    nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &item);
    value[0].i32 = ARKUI_FONT_STYLE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_FONT_STYLE, &item);
    value[0].i32 = ARKUI_TEXTAREA_TYPE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_TYPE, &item);
    value[0].i32 = ARKUI_TEXT_ALIGNMENT_START;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_ALIGN, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_MAX_LENGTH, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_EDITING, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_SELECTION_MENU_HIDDEN, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_LINES, &item);
    value[0].i32 = false;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_EDITING, &item);

    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_TEXT, &item);
    nodeAPI->setAttribute(rootNode, NODE_FONT_FAMILY, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT_FEATURE, &item);

    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER_FONT, &fontItem);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_SHOW_COUNTER, &showCounterItem);
    value[0].f32 = 50;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_LETTER_SPACING, &item);
    value[0].i32 = false;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_ENABLE_PREVIEW_TEXT, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_TEXT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_MAX_LENGTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER_FONT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_CARET_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_TYPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_SHOW_COUNTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_SELECTION_MENU_HIDDEN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_BLUR_ON_SUBMIT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_INPUT_FILTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_SELECTED_BACKGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_ENTER_KEY_TYPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_ENABLE_KEYBOARD_ON_FOCUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_CARET_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_TEXT_SELECTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_ENABLE_AUTO_FILL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_CONTENT_TYPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_SHOW_KEYBOARD_ON_FOCUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_NUMBER_OF_LINES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_MARGIN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_RADIUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BORDER_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_WEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_FAMILY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_MAX_LINES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_ALIGN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_FONT_FEATURE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_LETTER_SPACING), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_AREA_ENABLE_PREVIEW_TEXT), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_TEXT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_MAX_LENGTH), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER_FONT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_CARET_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_EDITING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_TYPE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_SHOW_COUNTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_SELECTION_MENU_HIDDEN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_BLUR_ON_SUBMIT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_INPUT_FILTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_SELECTED_BACKGROUND_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_ENTER_KEY_TYPE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_ENABLE_KEYBOARD_ON_FOCUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_CARET_OFFSET), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_CONTENT_RECT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_CONTENT_LINE_COUNT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_TEXT_SELECTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_ENABLE_AUTO_FILL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_CONTENT_TYPE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_SHOW_KEYBOARD_ON_FOCUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_NUMBER_OF_LINES), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_PADDING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_MARGIN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_WIDTH), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_RADIUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BORDER_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_WEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_FAMILY), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_LINE_HEIGHT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_MAX_LINES), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_ALIGN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_FONT_FEATURE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_LETTER_SPACING), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_AREA_ENABLE_PREVIEW_TEXT), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest016
 * @tc.desc: Test buttonNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest016, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_BUTTON, nullptr, true});
    ArkUI_NumberValue value[] = {{.f32 = 10.0f}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), "test", nullptr};
    nodeAPI->setAttribute(rootNode, NODE_BUTTON_LABEL, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BUTTON_LABEL), nullptr);
    nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_SIZE), nullptr);
    value[0].i32 = ARKUI_BUTTON_TYPE_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_BUTTON_TYPE, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BUTTON_TYPE), nullptr);
    value[0].i32 = ARKUI_FONT_WEIGHT_W100;
    nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_WEIGHT), nullptr);
    value[0].u32 = 0xFFFF0000;
    nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_COLOR, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_BACKGROUND_COLOR), nullptr);
    nodeAPI->setAttribute(rootNode, NODE_FONT_COLOR, &item);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FONT_COLOR), nullptr);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BUTTON_LABEL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BUTTON_TYPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_BACKGROUND_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FONT_WEIGHT), ARKUI_ERROR_CODE_NO_ERROR);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest017
 * @tc.desc: Test progressNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest017, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_PROGRESS);
    ASSERT_NE(rootNode, nullptr);
    ArkUI_NumberValue value[] = {{.f32 = 10.0f}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_VALUE, &item);
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TOTAL, &item);
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_COLOR, &item);
    value[0].i32 = ARKUI_PROGRESS_TYPE_LINEAR;
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TYPE, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_PROGRESS_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_PROGRESS_TOTAL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_PROGRESS_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_PROGRESS_TYPE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_PROGRESS_VALUE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_PROGRESS_TOTAL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_PROGRESS_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_PROGRESS_TYPE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest018
 * @tc.desc: Test checkboxNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest018, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_CHECKBOX);
    ASSERT_NE(rootNode, nullptr);
    float size = 10.0f;
    uint32_t color = 0xFFFF0000;
    ArkUI_NumberValue value[] = {{.i32 = true}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue mark[] = {{.u32 = color}, {.f32 = size}, {.f32 = size}};
    ArkUI_AttributeItem markItem = {mark, sizeof(mark) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SELECT, &item);
    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SELECT_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_UNSELECT_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_MARK, &item);
    value[0].i32 = ArkUI_CHECKBOX_SHAPE_CIRCLE;
    nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SHAPE, &item);
    nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_MARK, &markItem);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CHECKBOX_SELECT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CHECKBOX_SELECT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CHECKBOX_UNSELECT_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CHECKBOX_MARK), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CHECKBOX_SHAPE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CHECKBOX_SELECT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CHECKBOX_SELECT_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CHECKBOX_UNSELECT_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CHECKBOX_MARK), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CHECKBOX_SHAPE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest019
 * @tc.desc: Test xcomponentNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest019, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_XCOMPONENT);
    ASSERT_NE(rootNode, nullptr);

    ArkUI_NumberValue value[] = {{.i32 = ARKUI_XCOMPONENT_TYPE_SURFACE}, {.f32 = 10.0f}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), "test"};
    nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_ID, &item);
    nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_TYPE, &item);
    value[0].i32 = ARKUI_XCOMPONENT_TYPE_TEXTURE;
    nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_TYPE, &item);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_XCOMPONENT_TYPE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_XCOMPONENT_ID), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_XCOMPONENT_TYPE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest020
 * @tc.desc: Test datePickerNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest020, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));

    auto rootNode = new ArkUI_Node({ARKUI_NODE_DATE_PICKER, nullptr, true});
    ArkUI_NumberValue value[] = {{.i32 = true}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_LUNAR, &item);
    item.string = "1970-1-1";
    nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_START, &item);
    nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_END, &item);
    nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED, &item);
    item.string = "#ff182431;14;normal;Arial;normal";
    nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_DISAPPEAR_TEXT_STYLE, &item);
    nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_TEXT_STYLE, &item);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_NO_ERROR);
    item.string = "#ff182431;14;normal;Arial;test";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DATE_PICKER_LUNAR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DATE_PICKER_START), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DATE_PICKER_END), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DATE_PICKER_SELECTED), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DATE_PICKER_DISAPPEAR_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DATE_PICKER_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_DATE_PICKER_LUNAR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_DATE_PICKER_START), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_DATE_PICKER_END), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_DATE_PICKER_SELECTED), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_DATE_PICKER_DISAPPEAR_TEXT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_DATE_PICKER_TEXT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest021
 * @tc.desc: Test timePickerNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest021, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_TIME_PICKER, nullptr, true});
    ArkUI_NumberValue value[] = {{.i32 = true}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    item.string = "11-59";
    nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED, &item);
    nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_USE_MILITARY_TIME, &item);
    item.string = "#ff182431;14;normal;Arial;normal";
    nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_DISAPPEAR_TEXT_STYLE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_TEXT_STYLE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED_TEXT_STYLE, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TIME_PICKER_SELECTED), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TIME_PICKER_USE_MILITARY_TIME), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TIME_PICKER_DISAPPEAR_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TIME_PICKER_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TIME_PICKER_SELECTED_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TIME_PICKER_SELECTED), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TIME_PICKER_USE_MILITARY_TIME), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TIME_PICKER_DISAPPEAR_TEXT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TIME_PICKER_TEXT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TIME_PICKER_SELECTED_TEXT_STYLE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest022
 * @tc.desc: Test textPickerNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest022, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_TEXT_PICKER, nullptr, true});
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_TEXTPICKER_RANGETYPE_SINGLE}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    item.string = "1;2;3";
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_OPTION_RANGE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_OPTION_VALUE, &item);
    item.string = "#ff182431;14;normal;Arial;normal";
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_DISAPPEAR_TEXT_STYLE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_TEXT_STYLE, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE, &item);

    value[0].u32 = 0;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_OPTION_SELECTED, &item);
    value[0].i32 = 0;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_CAN_LOOP, &item);
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_INDEX, &item);

    value[0].f32 = 10.0f;
    nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_DEFAULT_PICKER_ITEM_HEIGHT, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_OPTION_RANGE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_OPTION_SELECTED), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_OPTION_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_DISAPPEAR_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_INDEX), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_TEXT_PICKER_CAN_LOOP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(
        rootNode, NODE_TEXT_PICKER_DEFAULT_PICKER_ITEM_HEIGHT), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_OPTION_RANGE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_OPTION_SELECTED), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_OPTION_VALUE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_DISAPPEAR_TEXT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_TEXT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_INDEX), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_CAN_LOOP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_TEXT_PICKER_DEFAULT_PICKER_ITEM_HEIGHT), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest023
 * @tc.desc: Test calendarPickerNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest023, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_CALENDAR_PICKER, nullptr, true});
    float offset = 10.0f;
    ArkUI_NumberValue value[] = {{.f32 = offset}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue value3[] = {{.u32 = 2024}, {.u32 = 1}, {.u32 = 1}};
    ArkUI_AttributeItem item3 = {value3, sizeof(value3) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_HINT_RADIUS, &item);
    nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_SELECTED_DATE, &item3);

    value[0].i32 = ARKUI_CALENDAR_ALIGNMENT_START;
    nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_EDGE_ALIGNMENT, &item);
    value3[0].i32 = ARKUI_CALENDAR_ALIGNMENT_START;
    value3[1].f32 = offset;
    value3[2].f32 = offset;
    nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_EDGE_ALIGNMENT, &item3);
    value3[0].u32 = 0xFFFF0000;
    value3[1].f32 = offset;
    value3[2].i32 = ARKUI_FONT_WEIGHT_NORMAL;
    nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_TEXT_STYLE, &item3);
    item3.size = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_TEXT_STYLE, &item3), ARKUI_ERROR_CODE_PARAM_INVALID);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CALENDAR_PICKER_HINT_RADIUS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CALENDAR_PICKER_SELECTED_DATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CALENDAR_PICKER_EDGE_ALIGNMENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_CALENDAR_PICKER_TEXT_STYLE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CALENDAR_PICKER_HINT_RADIUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CALENDAR_PICKER_SELECTED_DATE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CALENDAR_PICKER_EDGE_ALIGNMENT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_CALENDAR_PICKER_TEXT_STYLE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest024
 * @tc.desc: Test sliderNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest024, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_SLIDER);
    ASSERT_NE(rootNode, nullptr);
    uint32_t color = 0xFFFF0000;
    float size = 10.0f;
    ArkUI_NumberValue value[] = {{.u32 = color}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue blockStyle[] = {{.i32 = ARKUI_SLIDER_BLOCK_STYLE_IMAGE }, {.i32 = ARKUI_CLIP_TYPE_PATH},
        {.f32 = size}, {.f32 = size}, {.f32 = size}};
    ArkUI_AttributeItem blockStyleItem = {blockStyle, sizeof(blockStyle) / sizeof(ArkUI_NumberValue), "test", nullptr};
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_TRACK_COLOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_SELECTED_COLOR, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_SHOW_STEPS, &item);
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_REVERSE, &item);
    value[0].i32 = ARKUI_SLIDER_DIRECTION_VERTICAL;
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_DIRECTION, &item);
    value[0].i32 = ARKUI_SLIDER_STYLE_OUT_SET;
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_STYLE, &item);
    value[0].f32 = size;
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_VALUE, &item);
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_MIN_VALUE, &item);
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_MAX_VALUE, &item);
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_STEP, &item);
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_TRACK_THICKNESS, &item);

    nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &blockStyleItem);
    blockStyle[0].i32 = ARKUI_SLIDER_BLOCK_STYLE_SHAPE;
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &blockStyleItem);
    blockStyle[1].i32 = ARKUI_CLIP_TYPE_RECTANGLE;
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &blockStyleItem);
    blockStyle[0].i32 = ARKUI_SLIDER_BLOCK_STYLE_DEFAULT;
    nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &blockStyleItem);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_BLOCK_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_TRACK_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_SELECTED_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_SHOW_STEPS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_MIN_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_MAX_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_STEP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_DIRECTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_REVERSE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SLIDER_TRACK_THICKNESS), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_BLOCK_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_TRACK_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_SELECTED_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_SHOW_STEPS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_VALUE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_MIN_VALUE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_MAX_VALUE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_STEP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_DIRECTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_REVERSE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SLIDER_STYLE), nullptr);
    EXPECT_EQ(nodeAPI->getAttribute(rootNode, NODE_SLIDER_TRACK_THICKNESS), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest025
 * @tc.desc: Test radioNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest025, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_RADIO);
    ASSERT_NE(rootNode, nullptr);
    uint32_t color = 0xFFFF0000;
    ArkUI_NumberValue value[] = {{.i32 = true}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue style[] = {{.u32 = color}, {.u32 = color}, {.u32 = color}};
    ArkUI_AttributeItem styleItem = {style, sizeof(style) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_RADIO_CHECKED, &item);
    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_RADIO_VALUE, &item);
    nodeAPI->setAttribute(rootNode, NODE_RADIO_GROUP, &item);
    nodeAPI->setAttribute(rootNode, NODE_RADIO_STYLE, &styleItem);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RADIO_CHECKED), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RADIO_STYLE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RADIO_VALUE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RADIO_GROUP), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_RADIO_CHECKED), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_RADIO_STYLE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_RADIO_VALUE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_RADIO_GROUP), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest026
 * @tc.desc: Test scrollNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest026, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_SCROLL);
    ASSERT_NE(rootNode, nullptr);
    float distance = 10.0f;
    uint32_t color = 0xFFFF0000;
    ArkUI_NumberValue value[] = {{.f32 = distance}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value2[] = {{.i32 = true}, {.i32 = true}};
    ArkUI_AttributeItem item2 = {value2, sizeof(value2) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue scrollsnap[] = {{.i32 = ARKUI_SCROLL_SNAP_ALIGN_NONE}, {.i32 = true}, {.i32 = true},
        {.f32 = distance}};
    ArkUI_AttributeItem scrollsnapItem = {scrollsnap, sizeof(scrollsnap) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue scrollnested[] = {{.i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY},
        {.i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY}};
    ArkUI_AttributeItem scrollnestedItem = {scrollnested, sizeof(scrollnested) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue scrolloffset[] = {{.f32 = distance}, {.f32 = distance}, { .i32 = 1000},
        {.i32 = ARKUI_CURVE_LINEAR}, {.i32 = true}};
    ArkUI_AttributeItem scrolloffsetItem = {scrolloffset, sizeof(scrolloffset) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_FRICTION, &item);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_WIDTH, &item);

    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_COLOR, &item);

    value[0].i32 = ARKUI_SCROLL_BAR_DISPLAY_MODE_AUTO;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE, &item);
    value[0].i32 = ARKUI_SCROLL_DIRECTION_VERTICAL;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_SCROLL_DIRECTION, &item);
    value[0].i32 = ARKUI_EDGE_EFFECT_NONE;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_PAGING, &item);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_PAGE, &item);
    value[0].i32 = ARKUI_SCROLL_EDGE_TOP;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE, &item);

    nodeAPI->setAttribute(rootNode, NODE_SCROLL_SNAP, &scrollsnapItem);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL, &scrollnestedItem);
    scrolloffsetItem.size = 2;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_OFFSET, &scrolloffsetItem);
    scrolloffsetItem.size = sizeof(scrolloffset) / sizeof(ArkUI_NumberValue);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_OFFSET, &scrolloffsetItem);
    scrolloffset[3].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_OFFSET, &scrolloffsetItem);

    nodeAPI->setAttribute(rootNode, NODE_SCROLL_PAGE, &item2);
    value2[1].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_PAGE, &item2);

    value2[0].f32 = distance;
    value2[1].f32 = distance;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BY, &item2);
    nodeAPI->setLengthMetricUnit(rootNode, ARKUI_LENGTH_METRIC_UNIT_PX);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BY, &item2);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_SCROLL_DIRECTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_FRICTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_SNAP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_EDGE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_ENABLE_PAGING), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_WIDTH), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_SCROLL_DIRECTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_FRICTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_SNAP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_OFFSET), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_EDGE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_ENABLE_PAGING), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest027
 * @tc.desc: Test listNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest027, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_LIST, nullptr, true});
    auto childNode = new ArkUI_Node({ARKUI_NODE_LIST_ITEM, nullptr, true});
    auto groupNode = new ArkUI_Node({ARKUI_NODE_LIST_ITEM_GROUP, nullptr, true});

    float distance = 10.0f;
    uint32_t color = 0xFFFF0000;
    ArkUI_NumberValue value[] = {{.f32 = distance}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value2[] = {{.f32 = distance}, {.f32 = distance}};
    ArkUI_AttributeItem item2 = {value2, sizeof(value2) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue scrollnested[] = {{.i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY},
        {.i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY}};
    ArkUI_AttributeItem scrollnestedItem = {scrollnested, sizeof(scrollnested) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue scrollToIndex[] = {{.i32 = 0}, {.i32 = 1}, {.i32 = ARKUI_SCROLL_ALIGNMENT_START}};
    ArkUI_AttributeItem scrollToIndexItem = {scrollToIndex, sizeof(scrollToIndex) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue divider[] = {{.u32 = color}, {.f32 = distance}, {.f32 = distance}, {.f32 = distance}};
    ArkUI_AttributeItem dividerItem = {divider, sizeof(divider) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_FRICTION, &item);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_WIDTH, &item);
    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_COLOR, &item);

    value[0].i32 = ARKUI_SCROLL_BAR_DISPLAY_MODE_AUTO;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE, &item);
    value[0].i32 = ARKUI_EDGE_EFFECT_NONE;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item);
    value[0].i32 = ARKUI_AXIS_VERTICAL;
    nodeAPI->setAttribute(rootNode, NODE_LIST_DIRECTION, &item);
    value[0].i32 = ARKUI_STICKY_STYLE_NONE;
    nodeAPI->setAttribute(rootNode, NODE_LIST_STICKY, &item);
    value[0].i32 = 1;
    nodeAPI->setAttribute(rootNode, NODE_LIST_SPACE, &item);
    nodeAPI->setAttribute(rootNode, NODE_LIST_CACHED_COUNT, &item);
    value[0].i32 = ARKUI_LIST_ITEM_ALIGNMENT_START;
    nodeAPI->setAttribute(rootNode, NODE_LIST_ALIGN_LIST_ITEM, &item);
    value[0].i32 = 0;
    nodeAPI->setAttribute(rootNode, NODE_LIST_SCROLL_TO_INDEX, &item);
    nodeAPI->setAttribute(rootNode, NODE_LIST_INITIAL_INDEX, &item);
    item.object = rootNode;
    nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_HEADER, &item);
    nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_FOOTER, &item);

    nodeAPI->setAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL, &scrollnestedItem);

    nodeAPI->setAttribute(rootNode, NODE_LIST_SCROLL_TO_INDEX, &scrollToIndexItem);
    scrollToIndex[2].i32 = ARKUI_SCROLL_ALIGNMENT_NONE;
    nodeAPI->setAttribute(rootNode, NODE_LIST_SCROLL_TO_INDEX, &scrollToIndexItem);

    nodeAPI->setAttribute(rootNode, NODE_LIST_DIVIDER, &dividerItem);
    nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_DIVIDER, &dividerItem);

    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BY, &item2);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_DIRECTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_STICKY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_SPACE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_NODE_ADAPTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_CACHED_COUNT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_ALIGN_LIST_ITEM), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_CHILDREN_MAIN_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_INITIAL_INDEX), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_LIST_DIVIDER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_FRICTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_DIRECTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_STICKY), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_SPACE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_NODE_ADAPTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_CACHED_COUNT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_ALIGN_LIST_ITEM), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_INITIAL_INDEX), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_LIST_DIVIDER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_FRICTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_WIDTH), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL), nullptr);

    EXPECT_EQ(nodeAPI->resetAttribute(childNode, NODE_LIST_ITEM_SWIPE_ACTION), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_EQ(nodeAPI->resetAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_HEADER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_FOOTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_DIVIDER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(groupNode, NODE_LIST_ITEM_GROUP_CHILDREN_MAIN_SIZE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_DIVIDER), nullptr);

    nodeAPI->disposeNode(rootNode);
    nodeAPI->disposeNode(childNode);
    nodeAPI->disposeNode(groupNode);
}

/**
 * @tc.name: NativeNodeTest028
 * @tc.desc: Test swiperNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest028, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_SWIPER, nullptr, true});
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_CURVE_LINEAR}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue value2[] = {{.i32 = 1}, {.i32 = 1}};
    ArkUI_AttributeItem item2 = {value2, sizeof(value2) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_EASE;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_EASE_IN;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_EASE_OUT;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_EASE_IN_OUT;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_FAST_OUT_SLOW_IN;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_LINEAR_OUT_SLOW_IN;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_FAST_OUT_LINEAR_IN;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_EXTREME_DECELERATION;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_SHARP;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_RHYTHM;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_SMOOTH;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_CURVE_FRICTION;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item);
    value[0].i32 = ARKUI_SWIPER_ARROW_HIDE;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_SHOW_DISPLAY_ARROW, &item);
    value[0].i32 = ARKUI_SWIPER_ARROW_SHOW_ON_HOVER;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_SHOW_DISPLAY_ARROW, &item);
    value[0].i32 = ARKUI_EDGE_EFFECT_SPRING;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_EDGE_EFFECT_MODE, &item);
    value[0].i32 = ARKUI_SWIPER_NESTED_SRCOLL_SELF_ONLY;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_NESTED_SCROLL, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_LOOP, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_AUTO_PLAY, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_SHOW_INDICATOR, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_VERTICAL, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDEX, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_DISPLAY_COUNT, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_DISABLE_SWIPE, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_CACHED_COUNT, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_SWIPE_TO_INDEX, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDICATOR_INTERACTIVE, &item);

    value[0].f32 = 10.0f;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_INTERVAL, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_DURATION, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_ITEM_SPACE, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_PREV_MARGIN, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_NEXT_MARGIN, &item);

    nodeAPI->setAttribute(rootNode, NODE_SWIPER_SWIPE_TO_INDEX, &item2);
    value2[1].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_SWIPE_TO_INDEX, &item2);

    value2[0].f32 = 10.0f;
    value2[1].i32 = 1;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_PREV_MARGIN, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_NEXT_MARGIN, &item);
    value2[1].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_PREV_MARGIN, &item);
    nodeAPI->setAttribute(rootNode, NODE_SWIPER_NEXT_MARGIN, &item);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_LOOP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_AUTO_PLAY), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_SHOW_INDICATOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_INTERVAL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_VERTICAL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_DURATION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_CURVE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_ITEM_SPACE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_INDEX), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_DISPLAY_COUNT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_DISABLE_SWIPE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_SHOW_DISPLAY_ARROW), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_EDGE_EFFECT_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_NODE_ADAPTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_CACHED_COUNT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_PREV_MARGIN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_NEXT_MARGIN), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_INDICATOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_NESTED_SCROLL), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SWIPER_INDICATOR_INTERACTIVE), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_LOOP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_AUTO_PLAY), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_SHOW_INDICATOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_INTERVAL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_VERTICAL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_DURATION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_CURVE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_ITEM_SPACE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_INDEX), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_DISPLAY_COUNT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_DISABLE_SWIPE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_SHOW_DISPLAY_ARROW), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_EDGE_EFFECT_MODE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_NODE_ADAPTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_CACHED_COUNT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_PREV_MARGIN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_NEXT_MARGIN), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_INDICATOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_NESTED_SCROLL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SWIPER_INDICATOR_INTERACTIVE), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest029
 * @tc.desc: Test columnNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest029, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_COLUMN);
    ASSERT_NE(rootNode, nullptr);
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_HORIZONTAL_ALIGNMENT_CENTER}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_COLUMN_ALIGN_ITEMS, &item);
    value[0].i32 = ARKUI_FLEX_ALIGNMENT_START;
    nodeAPI->setAttribute(rootNode, NODE_COLUMN_JUSTIFY_CONTENT, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_COLUMN_ALIGN_ITEMS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_COLUMN_JUSTIFY_CONTENT), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_COLUMN_ALIGN_ITEMS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_COLUMN_JUSTIFY_CONTENT), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest030
 * @tc.desc: Test rowNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest030, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_ROW);
    ASSERT_NE(rootNode, nullptr);
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_VERTICAL_ALIGNMENT_CENTER}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_ROW_ALIGN_ITEMS, &item);
    value[0].i32 = ARKUI_FLEX_ALIGNMENT_START;
    nodeAPI->setAttribute(rootNode, NODE_ROW_JUSTIFY_CONTENT, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ROW_ALIGN_ITEMS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ROW_JUSTIFY_CONTENT), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_ROW_ALIGN_ITEMS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_ROW_JUSTIFY_CONTENT), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest031
 * @tc.desc: Test flexNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest031, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_FLEX);
    auto childNode = nodeAPI->createNode(ARKUI_NODE_STACK);
    ASSERT_NE(rootNode, nullptr);
    ASSERT_NE(childNode, nullptr);
    nodeAPI->addChild(rootNode, childNode);

    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    int32_t negativeEnum = -1;
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeEnum}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    float negativeFloat = -1.0f;
    ArkUI_NumberValue valueFloat[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem itemFloat = {valueFloat, sizeof(valueFloat) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_ITEM_ALIGNMENT_AUTO}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_ALIGN_SELF, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_ALIGN_SELF, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_ALIGN_SELF, &item), ARKUI_ERROR_CODE_NO_ERROR);
    auto alignSelfVal = nodeAPI->getAttribute(childNode, NODE_ALIGN_SELF);
    EXPECT_EQ(alignSelfVal->value[0].i32, static_cast<int32_t>(ARKUI_ITEM_ALIGNMENT_AUTO));

    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_GROW, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_GROW, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    float flexGrowInput = 2.0f;
    value[0].f32 = flexGrowInput;
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_GROW, &item), ARKUI_ERROR_CODE_NO_ERROR);
    auto flexGrowVal = nodeAPI->getAttribute(childNode, NODE_FLEX_GROW);
    EXPECT_EQ(flexGrowVal->value[0].f32, flexGrowInput);

    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_SHRINK, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_SHRINK, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    float flexShrinkInput = 0.0f;
    value[0].f32 = flexShrinkInput;
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_SHRINK, &item), ARKUI_ERROR_CODE_NO_ERROR);
    auto flexShrinkVal = nodeAPI->getAttribute(childNode, NODE_FLEX_SHRINK);
    EXPECT_EQ(flexShrinkVal->value[0].f32, flexShrinkInput);

    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_BASIS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_BASIS, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    float flexBasisInput = 100.0f;
    value[0].f32 = flexBasisInput;
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_FLEX_BASIS, &item), ARKUI_ERROR_CODE_NO_ERROR);
    auto flexBasisVal = nodeAPI->getAttribute(childNode, NODE_FLEX_BASIS);
    EXPECT_EQ(flexBasisVal->value[0].f32, flexBasisInput);

    ArkUI_NumberValue flexOptVal[] = {{.i32 = ARKUI_FLEX_DIRECTION_ROW}, {.i32 = ARKUI_FLEX_WRAP_NO_WRAP},
        {.i32 = ARKUI_FLEX_ALIGNMENT_START}, {.i32 = ARKUI_ITEM_ALIGNMENT_START}, {.i32 = ARKUI_FLEX_ALIGNMENT_START}};
    ArkUI_AttributeItem flexOptItem = {flexOptVal, sizeof(flexOptVal) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &flexOptItem), ARKUI_ERROR_CODE_NO_ERROR);
    auto flexOptResult = nodeAPI->getAttribute(rootNode, NODE_FLEX_OPTION);
    EXPECT_EQ(flexOptResult->value[0].i32, static_cast<int32_t>(ARKUI_FLEX_DIRECTION_ROW));

    int32_t abnormalVal = 10;
    flexOptVal[4].i32 = abnormalVal;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &flexOptItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    flexOptVal[3].i32 = abnormalVal;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &flexOptItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    flexOptVal[2].i32 = abnormalVal;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &flexOptItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    flexOptVal[1].i32 = abnormalVal;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &flexOptItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    flexOptVal[0].i32 = abnormalVal;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &flexOptItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    flexOptItem.size = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FLEX_OPTION, &flexOptItem), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_EQ(nodeAPI->resetAttribute(childNode, NODE_ALIGN_SELF), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(childNode, NODE_FLEX_GROW), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(childNode, NODE_FLEX_SHRINK), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(childNode, NODE_FLEX_BASIS), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_FLEX_OPTION), ARKUI_ERROR_CODE_NO_ERROR);
    nodeAPI->disposeNode(childNode);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest032
 * @tc.desc: Test refreshNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest032, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_REFRESH, nullptr, true});
    ASSERT_NE(rootNode, nullptr);
    ArkUI_NumberValue value[] = {{.i32 = true}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    nodeAPI->setAttribute(rootNode, NODE_REFRESH_REFRESHING, &item);
    nodeAPI->setAttribute(rootNode, NODE_REFRESH_PULL_DOWN_RATIO, &item);
    value[0].f32 = 0.5f;
    nodeAPI->setAttribute(rootNode, NODE_REFRESH_PULL_DOWN_RATIO, &item);
    nodeAPI->setAttribute(rootNode, NODE_REFRESH_OFFSET, &item);
    item.object = rootNode;
    nodeAPI->setAttribute(rootNode, NODE_REFRESH_CONTENT, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_REFRESH_CONTENT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_REFRESH_PULL_DOWN_RATIO), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_REFRESH_OFFSET), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_REFRESH_PULL_TO_REFRESH), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_REFRESH_REFRESHING), nullptr);
    EXPECT_EQ(nodeAPI->getAttribute(rootNode, NODE_REFRESH_CONTENT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_REFRESH_PULL_DOWN_RATIO), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_REFRESH_OFFSET), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_REFRESH_PULL_TO_REFRESH), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest033
 * @tc.desc: Test waterflowNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest033, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_WATER_FLOW, nullptr, true});
    float distance = 10.0f;
    uint32_t color = 0xFFFF0000;
    ArkUI_NumberValue value[] = {{.f32 = distance}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value2[] = {{.f32 = distance}, {.f32 = distance}};
    ArkUI_AttributeItem item2 = {value2, sizeof(value2) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue scrollnested[] = {{.i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY},
        {.i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY}};
    ArkUI_AttributeItem scrollnestedItem = {scrollnested, sizeof(scrollnested) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue constraintSize[] = {{.f32 = distance}, {.f32 = distance}, {.f32 = distance}, {.f32 = distance}};
    ArkUI_AttributeItem constraintSizeItem = {constraintSize, sizeof(constraintSize) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue indexValue[] = {{.i32 = 0}, {.i32 = 0}, {.i32 = ARKUI_SCROLL_ALIGNMENT_START}};
    ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_FRICTION, &item);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_WIDTH, &item);
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_COLUMN_GAP, &item);
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ROW_GAP, &item);
    value[0].u32 = color;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_COLOR, &item);

    value[0].i32 = ARKUI_SCROLL_BAR_DISPLAY_MODE_AUTO;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE, &item);
    value[0].i32 = ARKUI_EDGE_EFFECT_NONE;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT, &item);
    value[0].i32 = ARKUI_FLEX_DIRECTION_ROW;
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_LAYOUT_DIRECTION, &item);
    value[0].i32 = true;
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item);
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_CACHED_COUNT, &item);
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SCROLL_TO_INDEX, &item);

    item.string = "test";
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_COLUMN_TEMPLATE, &item);
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ROW_TEMPLATE, &item);

    item.object = rootNode;
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_FOOTER, &item);

    nodeAPI->setAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL, &scrollnestedItem);
    nodeAPI->setAttribute(rootNode, NODE_SCROLL_BY, &item2);
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ITEM_CONSTRAINT_SIZE, &constraintSizeItem);
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SCROLL_TO_INDEX, &indexItem);
    indexValue[2].i32 = ARKUI_SCROLL_ALIGNMENT_NONE;
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SCROLL_TO_INDEX, &indexItem);
    indexValue[2].i32 = -1;
    indexValue[1].i32 = -1;
    nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SCROLL_TO_INDEX, &indexItem);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_LAYOUT_DIRECTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_COLUMN_TEMPLATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_ROW_TEMPLATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_COLUMN_GAP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_ROW_GAP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_SECTION_OPTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_NODE_ADAPTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_CACHED_COUNT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_FOOTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_WATER_FLOW_ITEM_CONSTRAINT_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_FRICTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_WIDTH), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_BAR_COLOR), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_LAYOUT_DIRECTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_COLUMN_TEMPLATE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_ROW_TEMPLATE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_COLUMN_GAP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_ROW_GAP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_NODE_ADAPTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_CACHED_COUNT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_ITEM_CONSTRAINT_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_FRICTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_WIDTH), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_BAR_COLOR), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_WATER_FLOW_SECTION_OPTION), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest034
 * @tc.desc: Test gridNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest034, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_GRID);
    ASSERT_NE(rootNode, nullptr);
    ArkUI_NumberValue value[] = {{.f32 = 1.0f}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), "test"};
    nodeAPI->setAttribute(rootNode, NODE_GRID_COLUMN_TEMPLATE, &item);
    nodeAPI->setAttribute(rootNode, NODE_GRID_ROW_TEMPLATE, &item);
    nodeAPI->setAttribute(rootNode, NODE_GRID_COLUMN_GAP, &item);
    nodeAPI->setAttribute(rootNode, NODE_GRID_ROW_GAP, &item);
    value[0].i32 = 1;
    nodeAPI->setAttribute(rootNode, NODE_GRID_CACHED_COUNT, &item);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GRID_COLUMN_TEMPLATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GRID_ROW_TEMPLATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GRID_COLUMN_GAP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GRID_ROW_GAP), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GRID_NODE_ADAPTER), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_GRID_CACHED_COUNT), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_GRID_COLUMN_TEMPLATE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_GRID_ROW_TEMPLATE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_GRID_COLUMN_GAP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_GRID_ROW_GAP), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_GRID_NODE_ADAPTER), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_GRID_CACHED_COUNT), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest035
 * @tc.desc: Test relativeContainerNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest035, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_RELATIVE_CONTAINER, nullptr, true});
    ASSERT_NE(rootNode, nullptr);

    auto alignRules = OH_ArkUI_AlignmentRuleOption_Create();
    float baisVal = 10.0f;
    ArkUI_AttributeItem alignRulesItem = {.size = 0, .object = alignRules};
    nodeAPI->setAttribute(rootNode, NODE_ALIGN_RULES, &alignRulesItem);
    OH_ArkUI_AlignmentRuleOption_SetStart(nullptr, nullptr, ARKUI_HORIZONTAL_ALIGNMENT_START);
    OH_ArkUI_AlignmentRuleOption_SetStart(alignRules, nullptr, ARKUI_HORIZONTAL_ALIGNMENT_START);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetStartId(alignRules), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetStartAlignment(alignRules), static_cast<ArkUI_HorizontalAlignment>(-1));
    OH_ArkUI_AlignmentRuleOption_SetStart(alignRules, "__container__", ARKUI_HORIZONTAL_ALIGNMENT_START);
    OH_ArkUI_AlignmentRuleOption_SetTop(nullptr, nullptr, ARKUI_VERTICAL_ALIGNMENT_TOP);
    OH_ArkUI_AlignmentRuleOption_SetTop(alignRules, nullptr, ARKUI_VERTICAL_ALIGNMENT_TOP);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetTopId(alignRules), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetTopAlignment(alignRules), static_cast<ArkUI_VerticalAlignment>(-1));
    OH_ArkUI_AlignmentRuleOption_SetTop(alignRules, "__container__", ARKUI_VERTICAL_ALIGNMENT_TOP);
    OH_ArkUI_AlignmentRuleOption_SetEnd(nullptr, nullptr, ARKUI_HORIZONTAL_ALIGNMENT_END);
    OH_ArkUI_AlignmentRuleOption_SetEnd(alignRules, nullptr, ARKUI_HORIZONTAL_ALIGNMENT_END);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetEndId(alignRules), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetEndAlignment(alignRules), static_cast<ArkUI_HorizontalAlignment>(-1));
    OH_ArkUI_AlignmentRuleOption_SetEnd(alignRules, "__container__", ARKUI_HORIZONTAL_ALIGNMENT_END);
    OH_ArkUI_AlignmentRuleOption_SetBottom(nullptr, nullptr, ARKUI_VERTICAL_ALIGNMENT_BOTTOM);
    OH_ArkUI_AlignmentRuleOption_SetBottom(alignRules, nullptr, ARKUI_VERTICAL_ALIGNMENT_BOTTOM);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetBottomId(alignRules), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetBottomAlignment(alignRules), static_cast<ArkUI_VerticalAlignment>(-1));
    OH_ArkUI_AlignmentRuleOption_SetBottom(alignRules, "__container__", ARKUI_VERTICAL_ALIGNMENT_BOTTOM);
    OH_ArkUI_AlignmentRuleOption_SetCenterHorizontal(nullptr, nullptr, ARKUI_HORIZONTAL_ALIGNMENT_CENTER);
    OH_ArkUI_AlignmentRuleOption_SetCenterHorizontal(alignRules, nullptr, ARKUI_HORIZONTAL_ALIGNMENT_CENTER);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterIdHorizontal(alignRules), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterAlignmentHorizontal(alignRules),
        static_cast<ArkUI_HorizontalAlignment>(-1));
    OH_ArkUI_AlignmentRuleOption_SetCenterHorizontal(alignRules, "__container__", ARKUI_HORIZONTAL_ALIGNMENT_CENTER);
    OH_ArkUI_AlignmentRuleOption_SetCenterVertical(nullptr, nullptr, ARKUI_VERTICAL_ALIGNMENT_CENTER);
    OH_ArkUI_AlignmentRuleOption_SetCenterVertical(alignRules, nullptr, ARKUI_VERTICAL_ALIGNMENT_CENTER);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterIdVertical(alignRules), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterAlignmentVertical(alignRules),
        static_cast<ArkUI_VerticalAlignment>(-1));
    OH_ArkUI_AlignmentRuleOption_SetCenterVertical(alignRules, "__container__", ARKUI_VERTICAL_ALIGNMENT_CENTER);
    OH_ArkUI_AlignmentRuleOption_SetBiasHorizontal(nullptr, baisVal);
    OH_ArkUI_AlignmentRuleOption_SetBiasHorizontal(alignRules, baisVal);
    OH_ArkUI_AlignmentRuleOption_SetBiasVertical(nullptr, baisVal);
    OH_ArkUI_AlignmentRuleOption_SetBiasVertical(alignRules, baisVal);
    alignRulesItem.object = alignRules;
    nodeAPI->setAttribute(rootNode, NODE_ALIGN_RULES, &alignRulesItem);
    auto alignRulesGetVal = nodeAPI->getAttribute(rootNode, NODE_ALIGN_RULES);
    ASSERT_NE(alignRulesGetVal, nullptr);
    OH_ArkUI_AlignmentRuleOption_GetStartId(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetStartAlignment(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetEndId(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetEndAlignment(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetCenterIdHorizontal(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetCenterAlignmentHorizontal(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetTopId(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetTopAlignment(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetBottomId(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetBottomAlignment(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetCenterIdVertical(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetCenterAlignmentVertical(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetBiasHorizontal(alignRules);
    OH_ArkUI_AlignmentRuleOption_GetBiasVertical(alignRules);

    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetStartId(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetStartAlignment(nullptr), static_cast<ArkUI_HorizontalAlignment>(-1));
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetEndId(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetEndAlignment(nullptr), static_cast<ArkUI_HorizontalAlignment>(-1));
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterIdHorizontal(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterAlignmentHorizontal(nullptr),
        static_cast<ArkUI_HorizontalAlignment>(-1));
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetTopId(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetTopAlignment(nullptr), static_cast<ArkUI_VerticalAlignment>(-1));
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetBottomId(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetBottomAlignment(nullptr), static_cast<ArkUI_VerticalAlignment>(-1));
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterIdVertical(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetCenterAlignmentVertical(nullptr),
        static_cast<ArkUI_VerticalAlignment>(-1));
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetBiasHorizontal(nullptr), -1.0f);
    EXPECT_EQ(OH_ArkUI_AlignmentRuleOption_GetBiasVertical(nullptr), -1.0f);
    

    ArkUI_NumberValue chainModeVal[] = {{.i32 = ARKUI_AXIS_HORIZONTAL},
        {.i32 = ARKUI_RELATIVE_LAYOUT_CHAIN_STYLE_PACKED}};
    ArkUI_AttributeItem chainModeItem = {chainModeVal, sizeof(chainModeVal) / sizeof(ArkUI_NumberValue)};
    auto chainModRet = nodeAPI->setAttribute(rootNode, NODE_RELATIVE_LAYOUT_CHAIN_MODE, &chainModeItem);
    EXPECT_EQ(chainModRet, ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->setAttribute(
        rootNode, NODE_RELATIVE_LAYOUT_CHAIN_MODE, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    auto chainModResult = nodeAPI->getAttribute(rootNode, NODE_RELATIVE_LAYOUT_CHAIN_MODE);
    EXPECT_NE(chainModResult, nullptr);

    int32_t guideLineOptSize = 1;
    float positionStart = 50.0f;
    float positionEnd = 60.0f;
    auto guidelineOption = OH_ArkUI_GuidelineOption_Create(guideLineOptSize);
    OH_ArkUI_GuidelineOption_SetId(nullptr, nullptr, 0);
    OH_ArkUI_GuidelineOption_SetId(guidelineOption, nullptr, -1);
    OH_ArkUI_GuidelineOption_SetId(guidelineOption, nullptr, 2);
    OH_ArkUI_GuidelineOption_SetId(guidelineOption, "__container__", 0);
    OH_ArkUI_GuidelineOption_SetDirection(nullptr, ARKUI_AXIS_VERTICAL, 0);
    OH_ArkUI_GuidelineOption_SetDirection(guidelineOption, ARKUI_AXIS_VERTICAL, -1);
    OH_ArkUI_GuidelineOption_SetDirection(guidelineOption, ARKUI_AXIS_VERTICAL, 2);
    OH_ArkUI_GuidelineOption_SetDirection(guidelineOption, ARKUI_AXIS_VERTICAL, 0);
    OH_ArkUI_GuidelineOption_SetPositionStart(nullptr, positionStart, 0);
    OH_ArkUI_GuidelineOption_SetPositionStart(guidelineOption, positionStart, -1);
    OH_ArkUI_GuidelineOption_SetPositionStart(guidelineOption, positionStart, 2);
    OH_ArkUI_GuidelineOption_SetPositionStart(guidelineOption, positionStart, 0);
    OH_ArkUI_GuidelineOption_SetPositionEnd(nullptr, positionEnd, 0);
    OH_ArkUI_GuidelineOption_SetPositionEnd(guidelineOption, positionEnd, -1);
    OH_ArkUI_GuidelineOption_SetPositionEnd(guidelineOption, positionEnd, 2);
    OH_ArkUI_GuidelineOption_SetPositionEnd(guidelineOption, positionEnd, 0);
    ArkUI_AttributeItem guidelineOptItem = {.object = guidelineOption};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RELATIVE_CONTAINER_GUIDE_LINE, &guidelineOptItem),
        ARKUI_ERROR_CODE_NO_ERROR);
    auto guidelineOptResult = nodeAPI->getAttribute(rootNode, NODE_RELATIVE_CONTAINER_GUIDE_LINE);
    ASSERT_NE(guidelineOptResult, nullptr);
    OH_ArkUI_GuidelineOption_GetId(guidelineOption, 0);
    OH_ArkUI_GuidelineOption_GetDirection(guidelineOption, 0);
    OH_ArkUI_GuidelineOption_GetPositionStart(guidelineOption, 0);
    OH_ArkUI_GuidelineOption_GetPositionEnd(guidelineOption, 0);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetId(nullptr, 0), nullptr);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetId(guidelineOption, -1), nullptr);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetId(guidelineOption, 2), nullptr);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetDirection(nullptr, 0), static_cast<ArkUI_Axis>(-1));
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetDirection(guidelineOption, -1), static_cast<ArkUI_Axis>(-1));
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetDirection(guidelineOption, 2), static_cast<ArkUI_Axis>(-1));
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetPositionStart(nullptr, 0), 0.0f);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetPositionStart(guidelineOption, -1), 0.0f);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetPositionStart(guidelineOption, 2), 0.0f);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetPositionEnd(nullptr, 0), 0.0f);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetPositionEnd(guidelineOption, -1), 0.0f);
    EXPECT_EQ(OH_ArkUI_GuidelineOption_GetPositionEnd(guidelineOption, 2), 0.0f);
    
    auto abnormalBarrierOption = OH_ArkUI_BarrierOption_Create(-1);
    auto barrierOption = OH_ArkUI_BarrierOption_Create(guideLineOptSize);
    OH_ArkUI_BarrierOption_SetId(nullptr, nullptr, 0);
    OH_ArkUI_BarrierOption_SetId(barrierOption, nullptr, -1);
    OH_ArkUI_BarrierOption_SetId(barrierOption, nullptr, 2);
    OH_ArkUI_BarrierOption_SetId(barrierOption, "__container__", 0);
    OH_ArkUI_BarrierOption_SetDirection(nullptr, ARKUI_BARRIER_DIRECTION_TOP, 0);
    OH_ArkUI_BarrierOption_SetDirection(barrierOption, ARKUI_BARRIER_DIRECTION_TOP, -1);
    OH_ArkUI_BarrierOption_SetDirection(barrierOption, ARKUI_BARRIER_DIRECTION_TOP, 2);
    OH_ArkUI_BarrierOption_SetDirection(barrierOption, ARKUI_BARRIER_DIRECTION_TOP, 0);
    OH_ArkUI_BarrierOption_SetReferencedId(nullptr, nullptr, 0);
    OH_ArkUI_BarrierOption_SetReferencedId(barrierOption, nullptr, -1);
    OH_ArkUI_BarrierOption_SetReferencedId(barrierOption, nullptr, 2);
    OH_ArkUI_BarrierOption_SetReferencedId(barrierOption, "__container__", 0);
    ArkUI_AttributeItem barrierOptItem = {.object = barrierOption};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RELATIVE_CONTAINER_BARRIER, &barrierOptItem),
        ARKUI_ERROR_CODE_NO_ERROR);
    auto barrierOptResult = nodeAPI->getAttribute(rootNode, NODE_RELATIVE_CONTAINER_BARRIER);
    ASSERT_NE(barrierOptResult, nullptr);
    OH_ArkUI_BarrierOption_GetId(barrierOption, 0);
    OH_ArkUI_BarrierOption_GetDirection(barrierOption, 0);
    OH_ArkUI_BarrierOption_GetReferencedId(barrierOption, 0, 0);
    OH_ArkUI_BarrierOption_GetReferencedIdSize(barrierOption, 0);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetId(nullptr, 0), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetId(barrierOption, -1), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetId(barrierOption, 2), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetDirection(nullptr, 0), ARKUI_BARRIER_DIRECTION_TOP);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetDirection(barrierOption, -1), ARKUI_BARRIER_DIRECTION_TOP);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetDirection(barrierOption, 2), ARKUI_BARRIER_DIRECTION_TOP);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedId(nullptr, 0, 0), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedId(barrierOption, -1, 0), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedId(barrierOption, 2, 0), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedId(barrierOption, 0, -1), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedId(barrierOption, 0, 2), nullptr);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedIdSize(nullptr, 0), 0);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedIdSize(barrierOption, -1), 0);
    EXPECT_EQ(OH_ArkUI_BarrierOption_GetReferencedIdSize(barrierOption, 2), 0);

    OH_ArkUI_AlignmentRuleOption_Dispose(alignRules);
    OH_ArkUI_GuidelineOption_Dispose(guidelineOption);
    OH_ArkUI_BarrierOption_Dispose(barrierOption);
    OH_ArkUI_BarrierOption_Dispose(abnormalBarrierOption);
    
    OH_ArkUI_AlignmentRuleOption_Dispose(nullptr);
    OH_ArkUI_GuidelineOption_Dispose(nullptr);
    OH_ArkUI_BarrierOption_Dispose(nullptr);

    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_ALIGN_RULES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RELATIVE_LAYOUT_CHAIN_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RELATIVE_CONTAINER_GUIDE_LINE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_RELATIVE_CONTAINER_BARRIER), ARKUI_ERROR_CODE_NO_ERROR);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest036
 * @tc.desc: Test imageAnimatorNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest036, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_IMAGE_ANIMATOR, nullptr, true});
    ASSERT_NE(rootNode, nullptr);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ANIMATOR_IMAGES), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ANIMATOR_STATE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ANIMATOR_DURATION), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ANIMATOR_REVERSE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ANIMATOR_FIXED_SIZE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ANIMATOR_FILL_MODE), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->resetAttribute(rootNode, NODE_IMAGE_ANIMATOR_ITERATION), ARKUI_ERROR_CODE_NO_ERROR);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ANIMATOR_IMAGES), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ANIMATOR_STATE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ANIMATOR_DURATION), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ANIMATOR_REVERSE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ANIMATOR_FIXED_SIZE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ANIMATOR_FILL_MODE), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_IMAGE_ANIMATOR_ITERATION), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest037
 * @tc.desc: Test abnormalInputForCommon function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest037, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_STACK, nullptr, true});
    float negativeFloat = -1.0f;
    int32_t negativeEnum = -1;
    uint32_t color = 0XFFFF0000;
    float val0 = 0.0f;
    float val1 = 1.0f;
    float val10 = 10.0f;
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value4[] = {{.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat},
        {.f32 = negativeFloat}};
    ArkUI_AttributeItem item4 = {value4, sizeof(value4) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeEnum}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, "test", nullptr};
    ArkUI_NumberValue valueEnum4[] = {{.i32 = negativeEnum}, {.i32 = negativeEnum}, {.i32 = negativeEnum},
        {.i32 = negativeEnum}};
    ArkUI_AttributeItem itemEnum4 = {valueEnum4, sizeof(valueEnum4) / sizeof(ArkUI_NumberValue), nullptr, nullptr};

    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WIDTH, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WIDTH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_HEIGHT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_HEIGHT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WIDTH_PERCENT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WIDTH_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_HEIGHT_PERCENT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_HEIGHT_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PADDING, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PADDING_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ID, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ENABLED, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ENABLED, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MARGIN, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MARGIN, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MARGIN_PERCENT, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MARGIN_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_COLOR, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TRANSLATE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCALE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ROTATE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BRIGHTNESS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BRIGHTNESS, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SATURATION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SATURATION, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BLUR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LINEAR_GRADIENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LINEAR_GRADIENT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ALIGNMENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ALIGNMENT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &itemEnum4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_Z_INDEX, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_VISIBILITY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_VISIBILITY, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP_SHAPE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TRANSFORM, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_HIT_TEST_BEHAVIOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_HIT_TEST_BEHAVIOR, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_POSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SHADOW, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SHADOW, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOCUSABLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOCUSABLE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_GROUP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_GROUP, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_TEXT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_MODE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_MODE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_DESCRIPTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_ACTIONS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_ROLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DEFAULT_FOCUS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DEFAULT_FOCUS, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RESPONSE_REGION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OVERLAY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OVERLAY, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_POSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWEEP_GRADIENT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIAL_GRADIENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIAL_GRADIENT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    ArkUI_NumberValue mask[] = {{.u32 = color}, {.u32 = color}, {.f32 = val10}, {.i32 = ARKUI_MASK_TYPE_RECTANGLE},
        {.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat}};
    ArkUI_AttributeItem maskItem = {mask, sizeof(mask) / sizeof(ArkUI_NumberValue)};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    maskItem.size = 5;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    mask[3].i32 = ARKUI_MASK_TYPE_PATH;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    mask[0].i32 = ARKUI_MASK_TYPE_PROGRESS;
    mask[1].f32 = negativeFloat;
    mask[3].u32 = color;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    mask[1].f32 = val10;
    mask[2].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MASK, &maskItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BLEND_MODE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BLEND_MODE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);

    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DIRECTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DIRECTION, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CONSTRAINT_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CONSTRAINT_SIZE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CONSTRAINT_SIZE, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRAY_SCALE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRAY_SCALE, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_INVERT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_INVERT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SEPIA, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SEPIA, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CONTRAST, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CONTRAST, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OFFSET, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MARK_ANCHOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ALIGN_RULES, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOCUS_STATUS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOCUS_STATUS, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ASPECT_RATIO, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ASPECT_RATIO, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LAYOUT_WEIGHT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DISPLAY_PRIORITY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OUTLINE_WIDTH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GEOMETRY_TRANSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RENDER_FIT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RENDER_FIT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OUTLINE_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SIZE, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RENDER_GROUP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RENDER_GROUP, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_COLOR_BLEND, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    ArkUI_NumberValue backgroundBlurVal[] = {{.i32 = ARKUI_BLUR_STYLE_THIN}, {.i32 = ARKUI_COLOR_MODE_SYSTEM},
        {.i32 = ARKUI_ADAPTIVE_COLOR_DEFAULT}, {.f32 = 0}, {.f32 = 0}, {.f32 = negativeFloat}};
    ArkUI_AttributeItem backgroundBlurItem = {backgroundBlurVal, sizeof(backgroundBlurVal) / sizeof(ArkUI_NumberValue)};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_BLUR_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOREGROUND_BLUR_STYLE, &backgroundBlurItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    backgroundBlurVal[4].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    backgroundBlurVal[3].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    backgroundBlurVal[2].i32 = negativeEnum;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    backgroundBlurVal[1].i32 = negativeEnum;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    backgroundBlurVal[0].i32 = negativeEnum;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BACKGROUND_BLUR_STYLE, &backgroundBlurItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ROTATE_TRANSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCALE_TRANSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TRANSLATE_TRANSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_MOVE_TRANSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LAYOUT_RECT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LAYOUT_RECT, &itemEnum4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOCUS_ON_TOUCH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOCUS_ON_TOUCH, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_STATE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_VALUE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_EXPAND_SAFE_AREA, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_VISIBLE_AREA_CHANGE_RATIO, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_VISIBLE_AREA_CHANGE_RATIO, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TRANSITION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);

    ArkUI_NumberValue opacityTransition[] = {{.f32 = val0}, {.i32 = 1000}, {.i32 = ARKUI_CURVE_LINEAR}, {.i32 = 1000},
        {.i32 = 1}, {.i32 = ARKUI_ANIMATION_PLAY_MODE_NORMAL}, {.f32 = val1}};
    ArkUI_AttributeItem opacityTransitionItem =
        {opacityTransition, sizeof(opacityTransition) / sizeof(ArkUI_NumberValue)};
    opacityTransition[6].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    opacityTransition[5].i32 = negativeEnum;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    opacityTransition[5].i32 = static_cast<int32_t>(ARKUI_ANIMATION_PLAY_MODE_ALTERNATE_REVERSE) + 1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    opacityTransition[2].i32 = negativeEnum;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    opacityTransition[2].i32 = static_cast<int32_t>(ARKUI_CURVE_FRICTION) + 1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY_TRANSITION, &opacityTransitionItem),
        ARKUI_ERROR_CODE_PARAM_INVALID);

    valueEnum4[0].i32 = 0;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BLEND_MODE, &itemEnum4), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum4[2].i32 = 0;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LAYOUT_RECT, &itemEnum4), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum4[0].u32 = 0;
    valueEnum4[1].u32 = 0;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_EXPAND_SAFE_AREA, &itemEnum4), ARKUI_ERROR_CODE_NO_ERROR);
    valueEnum4[1].u32 = 20;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_EXPAND_SAFE_AREA, &itemEnum4), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum4[0].u32 = 20;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_EXPAND_SAFE_AREA, &itemEnum4), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum[0].u32 = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_ROLE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    item0.size = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TRANSFORM_CENTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_WIDTH_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_RADIUS_PERCENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CUSTOM_SHADOW, &item0), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OUTLINE_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_EXPAND_SAFE_AREA, &item0), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_FONT, &item0), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_PLACEHOLDER_FONT, &item0), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_SHOW_COUNTER, &item0), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ALIGN_RULES, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    
    value[0].f32 = 2.0f;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_OPACITY, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_VISIBLE_AREA_CHANGE_RATIO, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum[0].i32 = static_cast<int32_t>(MAX_NODE_SCOPE_NUM);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_VISIBILITY, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CLIP, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_HIT_TEST_BEHAVIOR, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SHADOW, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DIRECTION, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ALIGN_SELF, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RENDER_FIT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_STYLE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_MAX_LENGTH, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_ENTER_KEY_TYPE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_TYPE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_TYPE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_SHOW_PASSWORD_ICON, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_EDITING, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_AREA_EDITING, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CANCEL_BUTTON, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_STACK_ALIGN_CONTENT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_EDGE_EFFECT_MODE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_DECORATION, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_CASE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_ALIGN, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_OVERFLOW, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_VERTICAL_ALIGNMENT, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_FIT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_INTERPOLATION, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_REPEAT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_RENDER_MODE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_FIT_ORIGINAL_SIZE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_EDGE_ALIGNMENT, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum4[0].i32 = static_cast<int32_t>(MAX_NODE_SCOPE_NUM);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BORDER_STYLE, &itemEnum4), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum[0].u32 = static_cast<int32_t>(MAX_NODE_SCOPE_NUM);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ACCESSIBILITY_ACTIONS, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_CARET_COLOR, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INPUT_PLACEHOLDER_COLOR, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum[0].i32 = true;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FOCUS_STATUS, &itemEnum), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_STYLE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_COPY_OPTION, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);

    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_FOCUS_STATUS), nullptr);
    EXPECT_NE(nodeAPI->getAttribute(rootNode, NODE_OUTLINE_WIDTH), nullptr);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest038
 * @tc.desc: Test abnormalInputForText function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest038, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_TEXT, nullptr, true});
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value4[] = {{.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat},
        {.f32 = negativeFloat}};
    ArkUI_AttributeItem item4 = {value4, sizeof(value4) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, sizeof(valueEnum) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_CONTENT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_SIZE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_WEIGHT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_HEIGHT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_DECORATION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_DECORATION, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_CASE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_CASE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_LETTER_SPACING, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_LINES, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_ALIGN, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_ALIGN, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_OVERFLOW, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_OVERFLOW, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_FONT_FAMILY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_COPY_OPTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_BASELINE_OFFSET, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_TEXT_SHADOW, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_MIN_FONT_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_MAX_FONT_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value4[1].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    value4[1].i32 = 0;
    value4[2].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_HEIGHT_ADAPTIVE_POLICY, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_HEIGHT_ADAPTIVE_POLICY, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_INDENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_WORD_BREAK, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_ELLIPSIS_MODE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_SPACING, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_LINE_SPACING, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_FONT_FEATURE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_ENABLE_DATA_DETECTOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_SELECTED_BACKGROUND_COLOR, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING, nullptr),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest039
 * @tc.desc: Test abnormalInputForTextField function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest039, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto textinput = new ArkUI_Node({ARKUI_NODE_TEXT_INPUT, nullptr, true});
    auto textarea = new ArkUI_Node({ARKUI_NODE_TEXT_AREA, nullptr, true});
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value4[] = {{.f32 = 0.0f}, {.i32 = negativeInt}, {.i32 = negativeInt},
        {.i32 = negativeInt}};
    ArkUI_AttributeItem item4 = {value4, sizeof(value4) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, sizeof(valueEnum) / sizeof(ArkUI_NumberValue), nullptr, nullptr};

    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_CARET_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_CARET_STYLE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SHOW_UNDERLINE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SHOW_UNDERLINE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_MAX_LENGTH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_MAX_LENGTH, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_ENTER_KEY_TYPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_ENTER_KEY_TYPE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_PLACEHOLDER_COLOR, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(
        textinput, NODE_TEXT_INPUT_PLACEHOLDER_FONT, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(
        textinput, NODE_TEXT_INPUT_PLACEHOLDER_FONT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_TYPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_TYPE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SELECTED_BACKGROUND_COLOR, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SHOW_PASSWORD_ICON, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SHOW_PASSWORD_ICON, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_EDITING, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_EDITING, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_CANCEL_BUTTON, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_CANCEL_BUTTON, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_TEXT_SELECTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_UNDERLINE_COLOR, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_ENABLE_AUTO_FILL, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_CONTENT_TYPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SELECT_ALL, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_CARET_OFFSET, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SELECTION_MENU_HIDDEN, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SELECTION_MENU_HIDDEN, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_BLUR_ON_SUBMIT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_CUSTOM_KEYBOARD, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_WORD_BREAK, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_SHOW_KEYBOARD_ON_FOCUS, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textinput, NODE_TEXT_INPUT_NUMBER_OF_LINES, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);

    EXPECT_EQ(nodeAPI->setAttribute(
        textarea, NODE_TEXT_AREA_PLACEHOLDER_FONT, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(
        textarea, NODE_TEXT_AREA_PLACEHOLDER_FONT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(
        textarea, NODE_TEXT_AREA_PLACEHOLDER_FONT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    value4[1].i32 = ARKUI_FONT_STYLE_ITALIC;
    EXPECT_EQ(nodeAPI->setAttribute(
        textarea, NODE_TEXT_AREA_PLACEHOLDER_FONT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(
        textinput, NODE_TEXT_INPUT_PLACEHOLDER_FONT, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textarea, NODE_TEXT_AREA_EDITING, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textarea, NODE_TEXT_AREA_TYPE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(textarea, NODE_TEXT_AREA_SHOW_COUNTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value4[0].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(textarea, NODE_TEXT_AREA_SHOW_COUNTER, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    value4[0].i32 = 0;
    value4[1].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(textarea, NODE_TEXT_AREA_SHOW_COUNTER, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    value4[1].f32 = 1.0f;
    value4[2].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(textarea, NODE_TEXT_AREA_SHOW_COUNTER, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(
        textarea, NODE_TEXT_AREA_SELECTION_MENU_HIDDEN, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(
        textarea, NODE_TEXT_AREA_SELECTION_MENU_HIDDEN, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(textinput);
    nodeAPI->disposeNode(textarea);
}

/**
 * @tc.name: NativeNodeTest040
 * @tc.desc: Test abnormalInputForWaterflow function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest040, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_WATER_FLOW);
    float negativeFloat = -1.0f;
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_COLUMN_GAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ROW_GAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest041
 * @tc.desc: Test abnormalInputForGrid function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest041, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_GRID);
    float negativeFloat = -1.0f;
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_COLUMN_GAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_ROW_GAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest042
 * @tc.desc: Test abnormalInputForSpan function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest042, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_SPAN);
    float negativeFloat = -1.0f;
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value4[] = {{.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat},
        {.f32 = negativeFloat}};
    ArkUI_AttributeItem item4 = {value4, sizeof(value4) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SPAN_CONTENT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SPAN_TEXT_BACKGROUND_STYLE, &item4), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest043
 * @tc.desc: Test abnormalInputForImageSpan function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest043, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_IMAGE_SPAN, nullptr, true});
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, sizeof(valueEnum) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_SRC, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_VERTICAL_ALIGNMENT, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_VERTICAL_ALIGNMENT, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_SPAN_ALT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest044
 * @tc.desc: Test abnormalInputForImage function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest044, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_IMAGE, nullptr, true});
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, sizeof(valueEnum) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_SRC, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_FIT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_FIT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_INTERPOLATION, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_INTERPOLATION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_REPEAT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_OBJECT_REPEAT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_COLOR_FILTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_AUTO_RESIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ALT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_DRAGGABLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_RENDER_MODE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_RENDER_MODE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_FIT_ORIGINAL_SIZE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_FIT_ORIGINAL_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_FILL_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_RESIZABLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest045
 * @tc.desc: Test abnormalInputForToggle function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest045, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_TOGGLE);
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TOGGLE_SELECTED_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TOGGLE_SWITCH_POINT_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TOGGLE_VALUE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TOGGLE_UNSELECTED_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest046
 * @tc.desc: Test abnormalInputForLoadingProgress function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest046, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_LOADING_PROGRESS);
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LOADING_PROGRESS_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LOADING_PROGRESS_ENABLE_LOADING, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest047
 * @tc.desc: Test abnormalInputForButton function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest047, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_BUTTON, nullptr, true});
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, sizeof(valueEnum) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BUTTON_LABEL, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BUTTON_LABEL, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BUTTON_TYPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_BUTTON_TYPE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest048
 * @tc.desc: Test abnormalInputForProgress function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest048, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_PROGRESS);
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.f32 = -1.0f}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PROGRESS_VALUE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PROGRESS_VALUE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TOTAL, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TOTAL, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PROGRESS_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TYPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TYPE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest049
 * @tc.desc: Test abnormalInputForCheckbox function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest049, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_CHECKBOX);
    uint32_t color = 0xFFFF0000;
    float negativeFloat = -1.0f;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.i32 = -1}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue mark[] = {{.u32 = color}, {.f32 = negativeFloat}, {.f32 = negativeFloat}};
    ArkUI_AttributeItem markItem = {mark, sizeof(mark) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SELECT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SELECT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SELECT_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_UNSELECT_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_MARK, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_MARK, &markItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    mark[1].f32 = 0.0f;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_MARK, &markItem), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SHAPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CHECKBOX_SHAPE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest050
 * @tc.desc: Test abnormalInputForXComponent function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest050, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_XCOMPONENT);
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.i32 = -1}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_ID, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_ID, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_TYPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_TYPE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_XCOMPONENT_SURFACE_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest051
 * @tc.desc: Test abnormalInputForDatePicker function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest051, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_STACK);
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), "test", nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, sizeof(valueEnum) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_LUNAR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_LUNAR, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_START, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_START, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_END, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_END, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_DISAPPEAR_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_DISAPPEAR_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_TEXT_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_TEXT_STYLE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    item.string = "test;10;normal;test;italic";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_NO_ERROR);
    item.string = "#ff182431;test;normal;test;italic";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_TEXT_STYLE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_DATE_PICKER_DISAPPEAR_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest052
 * @tc.desc: Test abnormalInputForTimePicker function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest052, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_STACK);
    float negativeFloat = -1.0f;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), "test", nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_USE_MILITARY_TIME, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_DISAPPEAR_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_DISAPPEAR_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    item.string = "#ff182431;test;normal;test;italic";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_DISAPPEAR_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    item.string = "50-60";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    item.string = "10-60";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TIME_PICKER_SELECTED, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest053
 * @tc.desc: Test abnormalInputForTextPicker function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest053, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_STACK);
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_TEXTPICKER_RANGETYPE_SINGLE}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_OPTION_RANGE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_OPTION_RANGE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_OPTION_SELECTED, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_OPTION_VALUE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_DISAPPEAR_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_INDEX, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_CAN_LOOP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_DEFAULT_PICKER_ITEM_HEIGHT, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    item.string = "test";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_DISAPPEAR_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    item.string = "#ff182431;test;normal;test;italic";
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_DISAPPEAR_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest054
 * @tc.desc: Test abnormalInputForCalendarPicker function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest054, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_CALENDAR_PICKER);
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue value3[] = {{.u32 = 2024}, {.u32 = 13}, {.u32 = 1}};
    ArkUI_AttributeItem item3 = {value3, sizeof(value3) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_HINT_RADIUS, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_HINT_RADIUS, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_SELECTED_DATE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_SELECTED_DATE, &item3),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    value3[1].u32 = 12;
    value3[2].u32 = 40;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_SELECTED_DATE, &item3),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_EDGE_ALIGNMENT, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_EDGE_ALIGNMENT, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_TEXT_STYLE, &item0),
        ARKUI_ERROR_CODE_NO_ERROR);
    value3[1].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_TEXT_STYLE, &item3),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    value3[1].f32 = 1.0f;
    value3[2].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_CALENDAR_PICKER_TEXT_STYLE, &item3),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest055
 * @tc.desc: Test abnormalInputForSlider function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest055, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_SLIDER);
    float negativeFloat = -1.0f;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.i32 = ARKUI_SLIDER_BLOCK_STYLE_IMAGE }, {.i32 = ARKUI_CLIP_TYPE_PATH},
        {.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_TRACK_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_SELECTED_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_SHOW_STEPS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_VALUE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_MIN_VALUE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_MAX_VALUE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_STEP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_DIRECTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_REVERSE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_TRACK_THICKNESS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = ARKUI_SLIDER_BLOCK_STYLE_SHAPE;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[1].i32 = ARKUI_CLIP_TYPE_RECTANGLE;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_BLOCK_STYLE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_SHOW_STEPS, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_DIRECTION, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_REVERSE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].f32 = -1.0f;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_STEP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SLIDER_TRACK_THICKNESS, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest056
 * @tc.desc: Test abnormalInputForRadio function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest056, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_RADIO);
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.i32 = -1 }};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIO_CHECKED, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIO_CHECKED, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIO_STYLE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIO_VALUE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIO_GROUP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    item0.size = -1;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_RADIO_STYLE, &item0), ARKUI_ERROR_CODE_NO_ERROR);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest057
 * @tc.desc: Test abnormalInputForStack function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest057, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_STACK);
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_STACK_ALIGN_CONTENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest058
 * @tc.desc: Test abnormalInputForScroll function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest058, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_SCROLL);
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.i32 = negativeInt }, {.i32 = negativeInt},
        {.i32 = negativeInt}, {.f32 = negativeFloat}, {.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_DISPLAY_MODE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_WIDTH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_BAR_COLOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_SCROLL_DIRECTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_SCROLL_DIRECTION, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_FRICTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_SNAP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_SNAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = ARKUI_SCROLL_SNAP_ALIGN_NONE;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_SNAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[1].i32 = true;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_SNAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[2].i32 = true;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_SNAP, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = ARKUI_SCROLL_NESTED_MODE_SELF_ONLY;
    value[1].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_NESTED_SCROLL, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE_EFFECT, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_OFFSET, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_EDGE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_PAGING, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].i32 = negativeInt;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_ENABLE_PAGING, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_PAGE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_PAGE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_BY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SCROLL_FRICTION, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest059
 * @tc.desc: Test abnormalInputForList function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest059, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_LIST, nullptr, true});
    auto childNode = new ArkUI_Node({ARKUI_NODE_LIST_ITEM, nullptr, true});
    auto childNodeNew = new ArkUI_Node({ARKUI_NODE_LIST_ITEM, nullptr, true});
    auto groupNode = new ArkUI_Node({ARKUI_NODE_LIST_ITEM_GROUP, nullptr, true});

    nodeAPI->insertChildAt(rootNode, groupNode, 0);
    nodeAPI->insertChildAfter(groupNode, childNodeNew, nullptr);
    nodeAPI->insertChildBefore(groupNode, childNode, childNodeNew);
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue value[] = {{.i32 = negativeInt }, {.i32 = negativeInt},
        {.i32 = negativeInt}, {.f32 = negativeFloat}, {.f32 = negativeFloat}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_DIRECTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_DIRECTION, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_STICKY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_STICKY, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_SPACE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[0].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_SPACE, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_NODE_ADAPTER, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_NODE_ADAPTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_CACHED_COUNT, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_CACHED_COUNT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_CACHED_COUNT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_SCROLL_TO_INDEX, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_ALIGN_LIST_ITEM, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_ALIGN_LIST_ITEM, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_ALIGN_LIST_ITEM, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_CHILDREN_MAIN_SIZE, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_CHILDREN_MAIN_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_INITIAL_INDEX, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_INITIAL_INDEX, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_INITIAL_INDEX, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_DIVIDER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    value[1].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_DIVIDER, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_DIVIDER, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    value[1].f32 = 0.0f;
    value[2].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_DIVIDER, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_DIVIDER, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    value[2].f32 = 0.0f;
    value[3].f32 = negativeFloat;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_LIST_DIVIDER, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_DIVIDER, &item),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_LIST_ITEM_SWIPE_ACTION, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(childNode, NODE_LIST_ITEM_SWIPE_ACTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_HEADER, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_FOOTER, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_SET_DIVIDER, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_CHILDREN_MAIN_SIZE, nullptr),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(groupNode, NODE_LIST_ITEM_GROUP_CHILDREN_MAIN_SIZE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->removeChild(groupNode, childNode);
    nodeAPI->disposeNode(rootNode);
    nodeAPI->disposeNode(groupNode);
    nodeAPI->disposeNode(childNode);
}

/**
 * @tc.name: NativeNodeTest060
 * @tc.desc: Test abnormalInputForSwiper function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest060, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_SWIPER, nullptr, true});
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    ArkUI_NumberValue valueFloat[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem itemFloat = {valueFloat, 1, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_LOOP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_LOOP, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_AUTO_PLAY, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_AUTO_PLAY, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_SHOW_INDICATOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_SHOW_INDICATOR, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INTERVAL, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INTERVAL, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_VERTICAL, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_VERTICAL, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_DURATION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_DURATION, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_CURVE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_ITEM_SPACE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_ITEM_SPACE, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDEX, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDEX, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_DISPLAY_COUNT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_DISABLE_SWIPE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_DISABLE_SWIPE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_SHOW_DISPLAY_ARROW, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_SHOW_DISPLAY_ARROW, &itemFloat),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_EDGE_EFFECT_MODE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_EDGE_EFFECT_MODE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_NODE_ADAPTER, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_NODE_ADAPTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_CACHED_COUNT, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_CACHED_COUNT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_CACHED_COUNT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_PREV_MARGIN, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_PREV_MARGIN, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_NEXT_MARGIN, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_NEXT_MARGIN, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDICATOR, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDICATOR, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDICATOR_INTERACTIVE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_NESTED_SCROLL, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_NESTED_SCROLL, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_SWIPE_TO_INDEX, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum[0].i32 = ARKUI_SWIPER_INDICATOR_TYPE_DOT;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_SWIPER_INDICATOR, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest061
 * @tc.desc: Test abnormalInputForColumn function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest061, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_COLUMN);
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_COLUMN_ALIGN_ITEMS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_COLUMN_ALIGN_ITEMS, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_COLUMN_JUSTIFY_CONTENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_COLUMN_JUSTIFY_CONTENT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest062
 * @tc.desc: Test abnormalInputForRow function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest062, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_ROW);
    ASSERT_NE(rootNode, nullptr);
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ROW_ALIGN_ITEMS, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ROW_ALIGN_ITEMS, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ROW_JUSTIFY_CONTENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_ROW_JUSTIFY_CONTENT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest063
 * @tc.desc: Test abnormalInputForRefresh function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest063, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_REFRESH);
    ASSERT_NE(rootNode, nullptr);
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    ArkUI_NumberValue valueFloat[] = {{.f32 = negativeFloat}};
    ArkUI_AttributeItem itemFloat = {valueFloat, 1, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_REFRESHING, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_REFRESHING, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_CONTENT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_PULL_DOWN_RATIO, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_PULL_DOWN_RATIO, &itemFloat),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_OFFSET, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_OFFSET, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_PULL_TO_REFRESH, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_PULL_TO_REFRESH, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueFloat[0].f32 = 2.0f;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_REFRESH_PULL_DOWN_RATIO, &itemFloat),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest064
 * @tc.desc: Test abnormalInputForWaterflow function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest064, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_WATER_FLOW);
    ASSERT_NE(rootNode, nullptr);
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    ArkUI_NumberValue valueFloat[] = {{.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat},
        {.f32 = negativeFloat}};
    ArkUI_AttributeItem itemFloat = {valueFloat, sizeof(valueFloat) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_LAYOUT_DIRECTION, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_LAYOUT_DIRECTION, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_COLUMN_TEMPLATE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ROW_TEMPLATE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_COLUMN_GAP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_COLUMN_GAP, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ROW_GAP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ROW_GAP, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SECTION_OPTION, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SECTION_OPTION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SECTION_OPTION, &itemFloat),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_NODE_ADAPTER, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_NODE_ADAPTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_CACHED_COUNT, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_CACHED_COUNT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_CACHED_COUNT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_FOOTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_SCROLL_TO_INDEX, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ITEM_CONSTRAINT_SIZE, &item0),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ITEM_CONSTRAINT_SIZE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_WATER_FLOW_ITEM_CONSTRAINT_SIZE, &itemFloat),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest065
 * @tc.desc: Test abnormalInputForGrid function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest065, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_GRID);
    ASSERT_NE(rootNode, nullptr);
    float negativeFloat = -1.0f;
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    ArkUI_NumberValue valueFloat[] = {{.f32 = negativeFloat}, {.f32 = negativeFloat}, {.f32 = negativeFloat},
        {.f32 = negativeFloat}};
    ArkUI_AttributeItem itemFloat = {valueFloat, sizeof(valueFloat) / sizeof(ArkUI_NumberValue), nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_COLUMN_TEMPLATE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_ROW_TEMPLATE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_COLUMN_GAP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_COLUMN_GAP, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_ROW_GAP, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_ROW_GAP, &itemFloat), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_NODE_ADAPTER, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_NODE_ADAPTER, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_CACHED_COUNT, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_CACHED_COUNT, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_GRID_CACHED_COUNT, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest066
 * @tc.desc: Test abnormalInputForImageAnimator function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest066, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_IMAGE_ANIMATOR);
    ASSERT_NE(rootNode, nullptr);
    int32_t negativeInt = -1;
    ArkUI_NumberValue value0[] = {};
    ArkUI_AttributeItem item0 = {value0, 0, nullptr, nullptr};
    ArkUI_NumberValue valueEnum[] = {{.i32 = negativeInt}};
    ArkUI_AttributeItem itemEnum = {valueEnum, 1, nullptr, nullptr};
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_IMAGES, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_IMAGES, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_STATE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_STATE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_DURATION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_REVERSE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_REVERSE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_FIXED_SIZE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_FIXED_SIZE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_FILL_MODE, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_FILL_MODE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_ITERATION, &item0), ARKUI_ERROR_CODE_PARAM_INVALID);
    valueEnum[0].i32 = 4;
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_STATE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_REVERSE, &itemEnum), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_FIXED_SIZE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(rootNode, NODE_IMAGE_ANIMATOR_FILL_MODE, &itemEnum),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest067
 * @tc.desc: Test attributeAbnormal function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest067, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    int32_t abnormalType = static_cast<int32_t>(ARKUI_NODE_CUSTOM_SPAN) + 1;
    EXPECT_EQ(nodeAPI->createNode(static_cast<ArkUI_NodeType>(abnormalType)), nullptr);
    nodeAPI->disposeNode(nullptr);
    EXPECT_EQ(nodeAPI->addChild(nullptr, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->removeChild(nullptr, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->insertChildAfter(nullptr, nullptr, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->insertChildBefore(nullptr, nullptr, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->insertChildAt(nullptr, nullptr, 0), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setAttribute(nullptr, NODE_WIDTH, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->resetAttribute(nullptr, NODE_WIDTH), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->getAttribute(nullptr, NODE_WIDTH), nullptr);
    auto abnormalNode = new ArkUI_Node({-1, nullptr, true});
    EXPECT_EQ(nodeAPI->addChild(abnormalNode, abnormalNode), ARKUI_ERROR_CODE_NOT_SUPPROTED_FOR_ARKTS_NODE);
    EXPECT_EQ(nodeAPI->removeChild(abnormalNode, abnormalNode), ARKUI_ERROR_CODE_NOT_SUPPROTED_FOR_ARKTS_NODE);
    EXPECT_EQ(nodeAPI->insertChildAfter(abnormalNode, abnormalNode, nullptr),
        ARKUI_ERROR_CODE_NOT_SUPPROTED_FOR_ARKTS_NODE);
    EXPECT_EQ(nodeAPI->insertChildBefore(abnormalNode, abnormalNode, nullptr),
        ARKUI_ERROR_CODE_NOT_SUPPROTED_FOR_ARKTS_NODE);
    EXPECT_EQ(nodeAPI->insertChildAt(abnormalNode, abnormalNode, 0), ARKUI_ERROR_CODE_NOT_SUPPROTED_FOR_ARKTS_NODE);
    EXPECT_EQ(nodeAPI->setAttribute(abnormalNode, NODE_WIDTH, nullptr), ARKUI_ERROR_CODE_NOT_SUPPROTED_FOR_ARKTS_NODE);
    EXPECT_EQ(nodeAPI->resetAttribute(abnormalNode, NODE_WIDTH), ARKUI_ERROR_CODE_NOT_SUPPROTED_FOR_ARKTS_NODE);
    int32_t abnormalAttribute = 999 * MAX_NODE_SCOPE_NUM;
    auto node = nodeAPI->createNode(ARKUI_NODE_STACK);
    EXPECT_EQ(nodeAPI->setAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute), nullptr),
        ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED);
    EXPECT_EQ(nodeAPI->resetAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)),
        ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED);
    EXPECT_EQ(nodeAPI->getAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)), nullptr);
    for (int32_t i = 0; i <= static_cast<int32_t>(ARKUI_NODE_IMAGE_ANIMATOR); i++) {
        abnormalAttribute = (i + 1) * MAX_NODE_SCOPE_NUM - 1;
        EXPECT_EQ(nodeAPI->setAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute), nullptr),
            ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED);
        EXPECT_EQ(nodeAPI->resetAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)),
            ARKUI_ERROR_CODE_NO_ERROR);
        if (i == ARKUI_NODE_DATE_PICKER || i == ARKUI_NODE_TEXT_PICKER || i == ARKUI_NODE_TIME_PICKER ||
            i == ARKUI_NODE_LOADING_PROGRESS) {
            EXPECT_NE(nodeAPI->getAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)), nullptr);
        } else {
            EXPECT_EQ(nodeAPI->getAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)), nullptr);
        }
    }
    for (int32_t j = static_cast<int32_t>(ARKUI_NODE_STACK); j < static_cast<int32_t>(ARKUI_NODE_GRID_ITEM); j++) {
        abnormalAttribute = (j + 1) * MAX_NODE_SCOPE_NUM - 1;
        if (j == ARKUI_NODE_FLOW_ITEM) {
            EXPECT_EQ(nodeAPI->setAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute), nullptr),
                ARKUI_ERROR_CODE_PARAM_INVALID);
            EXPECT_EQ(nodeAPI->resetAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)),
                ARKUI_ERROR_CODE_PARAM_INVALID);
            EXPECT_EQ(nodeAPI->getAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)), nullptr);
        } else {
            EXPECT_EQ(nodeAPI->setAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute), nullptr),
                ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED);
            EXPECT_EQ(nodeAPI->resetAttribute(node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)),
                ARKUI_ERROR_CODE_NO_ERROR);
            if (j == ARKUI_NODE_LIST) {
                EXPECT_NE(nodeAPI->getAttribute(
                    node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)), nullptr);
            } else {
                EXPECT_EQ(nodeAPI->getAttribute(
                    node, static_cast<ArkUI_NodeAttributeType>(abnormalAttribute)), nullptr);
            }
        }
    }
    delete abnormalNode;
    abnormalNode = nullptr;
    nodeAPI->disposeNode(node);
}

/**
 * @tc.name: NativeNodeTest068
 * @tc.desc: Test eventAbnormal function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest068, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto node = nodeAPI->createNode(ARKUI_NODE_STACK);
    EXPECT_EQ(nodeAPI->registerNodeEvent(nullptr, NODE_TOUCH_EVENT, 0, nullptr), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->registerNodeEvent(
        node, static_cast<ArkUI_NodeEventType>(-1), 0, nullptr), ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED);
    nodeAPI->unregisterNodeEvent(nullptr, NODE_TOUCH_EVENT);
    nodeAPI->unregisterNodeEvent(node, NODE_TOUCH_EVENT);
    nodeAPI->disposeNode(node);
}

/**
 * @tc.name: NativeNodeTest069
 * @tc.desc: Test lengthMetricUnit function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest069, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto node = nodeAPI->createNode(ARKUI_NODE_STACK);
    EXPECT_EQ(nodeAPI->setLengthMetricUnit(nullptr, ARKUI_LENGTH_METRIC_UNIT_PX), ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setLengthMetricUnit(node, static_cast<ArkUI_LengthMetricUnit>(-2)),
        ARKUI_ERROR_CODE_PARAM_INVALID);
    EXPECT_EQ(nodeAPI->setLengthMetricUnit(node, ARKUI_LENGTH_METRIC_UNIT_PX), ARKUI_ERROR_CODE_NO_ERROR);
    ArkUI_NumberValue value[] = {{.f32 = 10.0f}, {.f32 = 10.0f}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(node, NODE_WIDTH, &item);
    nodeAPI->setLengthMetricUnit(node, ARKUI_LENGTH_METRIC_UNIT_FP);
    EXPECT_EQ(nodeAPI->setAttribute(node, NODE_SCROLL_BY, &item), ARKUI_ERROR_CODE_PARAM_INVALID);
    nodeAPI->disposeNode(node);
}

/**
 * @tc.name: NativeNodeTest070
 * @tc.desc: Test convertEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest070, TestSize.Level1)
{
    ArkUINodeEvent origin;
    ArkUI_NodeEvent event;
    ArkUI_CompatibleNodeEvent compatibleEvent;
    const std::string str = "test";
    origin.kind = ArkUIEventCategory::COMPONENT_ASYNC_EVENT;
    origin.componentAsyncEvent.subKind = ArkUIEventSubKind::ON_VISIBLE_AREA_CHANGE;
    origin.componentAsyncEvent.data[0].i32 = 0;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &event), true);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    origin.kind = ArkUIEventCategory::TEXT_INPUT;
    origin.textInputEvent.subKind = ArkUIEventSubKind::ON_TEXT_INPUT_CHANGE;
    origin.textInputEvent.nativeStringPtr = reinterpret_cast<intptr_t>(str.c_str());
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &event), true);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    origin.kind = ArkUIEventCategory::TOUCH_EVENT;
    origin.touchEvent.subKind = ArkUIEventSubKind::ON_TOUCH;
    origin.touchEvent.sourceType = 2;
    origin.touchEvent.action = 0;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &event), true);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    origin.touchEvent.sourceType = 0;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    origin.touchEvent.action = 1;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    origin.touchEvent.action = 2;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    origin.touchEvent.action = 3;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    origin.touchEvent.action = 4;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), true);
    
    origin.kind = ArkUIEventCategory::MOUSE_INPUT_EVENT;
    origin.mouseEvent.subKind = ArkUIEventSubKind::ON_MOUSE;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &event), true);
    origin.kind = ArkUIEventCategory::MIXED_EVENT;
    origin.mouseEvent.subKind = ArkUIEventSubKind::ON_TEXT_AREA_WILL_INSERT;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &event), true);
    origin.kind = ArkUIEventCategory::INVALID;
    origin.mouseEvent.subKind = ArkUIEventSubKind::ON_TEXT_AREA_WILL_INSERT;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &event), false);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEvent(&origin, &compatibleEvent), false);
}

/**
 * @tc.name: NativeNodeTest071
 * @tc.desc: Test ConvertToCTouch function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest071, TestSize.Level1)
{
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCTouchActionType(0), 1);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCTouchActionType(1), 3);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCTouchActionType(2), 2);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCTouchActionType(3), 0);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCTouchActionType(-1), -1);

    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCInputEventToolType(1), 1);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCInputEventToolType(2), 2);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCInputEventToolType(7), 3);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCInputEventToolType(9), 4);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCInputEventToolType(0), 0);

    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseActionType(1), 1);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseActionType(2), 2);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseActionType(3), 3);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseActionType(0), 0);

    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseEventButtonType(1), 1);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseEventButtonType(2), 2);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseEventButtonType(4), 3);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseEventButtonType(8), 4);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseEventButtonType(16), 5);
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertToCMouseEventButtonType(-1), 0);
}

/**
 * @tc.name: NativeNodeTest072
 * @tc.desc: Test convertEventResult function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest072, TestSize.Level1)
{
    ArkUINodeEvent event;
    ArkUI_CompatibleNodeEvent origin;
    origin.kind = ArkUI_NodeEventType::NODE_TOUCH_EVENT;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_ON_TOUCH_INTERCEPT;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CHANGE;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CUT;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_PASTE;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_TEXT_AREA_ON_CHANGE;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_TEXT_AREA_ON_PASTE;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_EVENT_ON_APPEAR;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CHANGE_WITH_PREVIEW_TEXT;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
    origin.kind = ArkUI_NodeEventType::NODE_TEXT_AREA_ON_CHANGE_WITH_PREVIEW_TEXT;
    EXPECT_EQ(OHOS::Ace::NodeModel::ConvertEventResult(&origin, &event), true);
}

/**
 * @tc.name: NativeNodeTest073
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetInputEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest073, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 1, 0 };
    event.origin = nullptr;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetInputEvent(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetInputEvent(nullptr), nullptr);
    event.category = 0;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetInputEvent(&event), nullptr);
}

/**
 * @tc.name: NativeNodeTest074
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetNodeComponentEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest074, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, 0 };
    event.origin = nullptr;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNodeComponentEvent(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNodeComponentEvent(&event), nullptr);
    event.category = 1;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNodeComponentEvent(&event), nullptr);
    ArkUINodeEvent origin = { 0, 0, 0};
    origin.componentAsyncEvent.subKind = ArkUIEventSubKind::ON_VISIBLE_AREA_CHANGE;
    origin.componentAsyncEvent.data[0].i32 = 0;
    event.origin = &origin;
    EXPECT_NE(OH_ArkUI_NodeEvent_GetNodeComponentEvent(&event), nullptr);
}

/**
 * @tc.name: NativeNodeTest075
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetStringAsyncEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest075, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, 0 };
    event.origin = nullptr;
    const std::string str = "test";
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringAsyncEvent(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringAsyncEvent(&event), nullptr);
    event.category = 2;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringAsyncEvent(&event), nullptr);
    ArkUINodeEvent origin = { 0, 0, 0};
    origin.textInputEvent.subKind = ArkUIEventSubKind::ON_TEXT_INPUT_CHANGE;
    origin.textInputEvent.nativeStringPtr = reinterpret_cast<intptr_t>(str.c_str());
    event.origin = &origin;
    EXPECT_NE(OH_ArkUI_NodeEvent_GetStringAsyncEvent(&event), nullptr);
}

/**
 * @tc.name: NativeNodeTest076
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetUserData function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest076, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, 0 };
    event.userData = nullptr;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetUserData(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetUserData(&event), nullptr);
}

/**
 * @tc.name: NativeNodeTest077
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetNumberValue function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest077, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, 0 };
    event.origin = nullptr;
    int32_t index = 0;
    ArkUI_NumberValue value[] = {{.f32 = 10.0f}};
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNumberValue(nullptr, index, value), 106108);
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNumberValue(&event, index, value), 106108);
    event.category = 3;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNumberValue(&event, index, value), 106108);
    ArkUINodeEvent mixEvent;
    mixEvent.kind = ArkUIEventCategory::MIXED_EVENT;
    mixEvent.extraParam = reinterpret_cast<intptr_t>(nullptr);
    mixEvent.mixedEvent.subKind = ArkUIEventSubKind::ON_TEXT_AREA_WILL_INSERT;
    mixEvent.mixedEvent.numberData[0].f32 = 0.0f;
    mixEvent.mixedEvent.numberDataLength = 1;
    event.origin = &mixEvent;
    index = 2;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNumberValue(&event, index, value), 106107);
    index = -1;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNumberValue(&event, index, value), 106107);
    index = 0;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetNumberValue(&event, index, value), ARKUI_ERROR_CODE_NO_ERROR);
}

/**
 * @tc.name: NativeNodeTest078
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetStringValue function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest078, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, 0 };
    event.origin = nullptr;
    int32_t index = 0;
    int32_t size = 0;
    char* value[size];
    const std::string str1 = "";
    const std::string str2 = "test";
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringValue(nullptr, index, value, &size), 106108);
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringValue(&event, index, value, &size), 106108);
    event.category = 3;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringValue(&event, index, value, &size), 106108);
    ArkUINodeEvent mixEvent;
    mixEvent.kind = ArkUIEventCategory::MIXED_EVENT;
    mixEvent.extraParam = reinterpret_cast<intptr_t>(nullptr);
    mixEvent.mixedEvent.subKind = ArkUIEventSubKind::ON_TEXT_AREA_WILL_INSERT;
    mixEvent.mixedEvent.stringPtrData[0] = reinterpret_cast<intptr_t>(str1.c_str());
    mixEvent.mixedEvent.stringPtrDataLength = 1;
    event.origin = &mixEvent;
    index = 2;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringValue(&event, index, value, &size), 106107);
    index = 0;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringValue(&event, index, value, &size), 106108);
    mixEvent.mixedEvent.stringPtrData[0] = reinterpret_cast<intptr_t>(str2.c_str());
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetStringValue(&event, index, value, &size), 106108);
}

/**
 * @tc.name: NativeNodeTest079
 * @tc.desc: Test OH_ArkUI_NodeEvent_SetReturnNumberValue function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest079, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, 0 };
    event.origin = nullptr;
    int32_t size = 1;
    ArkUI_NumberValue value[] = {{.i32 = 10}};
    EXPECT_EQ(OH_ArkUI_NodeEvent_SetReturnNumberValue(nullptr, value, size), 106108);
    EXPECT_EQ(OH_ArkUI_NodeEvent_SetReturnNumberValue(&event, value, size), 106108);
    event.category = 3;
    EXPECT_EQ(OH_ArkUI_NodeEvent_SetReturnNumberValue(&event, value, size), 106108);
    ArkUINodeEvent mixEvent;
    mixEvent.kind = ArkUIEventCategory::MIXED_EVENT;
    mixEvent.extraParam = reinterpret_cast<intptr_t>(nullptr);
    mixEvent.mixedEvent.subKind = ArkUIEventSubKind::ON_TEXT_AREA_WILL_INSERT;
    event.origin = &mixEvent;
    EXPECT_EQ(OH_ArkUI_NodeEvent_SetReturnNumberValue(&event, value, size), ARKUI_ERROR_CODE_NO_ERROR);
}

/**
 * @tc.name: NativeNodeTest080
 * @tc.desc: Test customSpanNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest080, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = new ArkUI_Node({ARKUI_NODE_CUSTOM_SPAN, nullptr, true});

    EXPECT_EQ(nodeAPI->registerNodeCustomEvent(
        rootNode, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE, 0, nullptr), ARKUI_ERROR_CODE_NO_ERROR);
    EXPECT_EQ(nodeAPI->registerNodeCustomEvent(
        rootNode, ARKUI_NODE_CUSTOM_EVENT_ON_DRAW, 1, nullptr), ARKUI_ERROR_CODE_NO_ERROR);
    nodeAPI->unregisterNodeCustomEvent(rootNode, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE);
    nodeAPI->unregisterNodeCustomEvent(rootNode, ARKUI_NODE_CUSTOM_EVENT_ON_DRAW);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeTest080
 * @tc.desc: Test native style string.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest081, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* style = OH_Drawing_CreateTypographyStyle();
    ArkUI_StyledString* styleString = OH_ArkUI_StyledString_Create(style, nullptr);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_ArkUI_StyledString_PushTextStyle(styleString, textStyle);
    OH_Drawing_PlaceholderSpan* placeholder = new OH_Drawing_PlaceholderSpan();
    OH_ArkUI_StyledString_AddPlaceholder(styleString, placeholder);

    OH_Drawing_DestroyTextStyle(textStyle);
    delete placeholder;
    OH_ArkUI_StyledString_PushTextStyle(styleString, textStyle);
    OH_ArkUI_StyledString_AddPlaceholder(styleString, placeholder);
}

/**
 * @tc.name: NativeNodeTest081
 * @tc.desc: Test OH_ArkUI_NodeEvent_GetTextChangeEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest082, TestSize.Level1)
{
    ArkUI_NodeEvent event = { 0, 0 };
    event.origin = nullptr;
    const std::string str = "test";
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetTextChangeEvent(nullptr), nullptr);
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetTextChangeEvent(&event), nullptr);
    event.category = 2;
    EXPECT_EQ(OH_ArkUI_NodeEvent_GetTextChangeEvent(&event), nullptr);
    ArkUINodeEvent origin = { 0, 0, 0};
    origin.textChangeEvent.subKind = ArkUIEventSubKind::ON_TEXT_AREA_CHANGE_WITH_PREVIEW_TEXT;
    origin.textChangeEvent.nativeStringPtr = reinterpret_cast<intptr_t>(str.c_str());
    origin.textChangeEvent.extendStringPtr = reinterpret_cast<intptr_t>(str.c_str());
    origin.textChangeEvent.numArgs = 0;
    event.origin = &origin;
    EXPECT_NE(OH_ArkUI_NodeEvent_GetTextChangeEvent(&event), nullptr);
}

/**
 * @tc.name: NativeNodeTest083
 * @tc.desc: Test progressNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeTest083, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto rootNode = nodeAPI->createNode(ARKUI_NODE_PROGRESS);
    ASSERT_NE(rootNode, nullptr);
    ArkUI_NumberValue value[] = {{.f32 = 10.0f}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_VALUE, &item);
    value[0].i32 = 100.0f;
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TOTAL, &item);
    value[0].i32 = ARKUI_PROGRESS_TYPE_LINEAR;
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_TYPE, &item);

    auto linearStyleOption = OH_ArkUI_ProgressLinearStyleOption_Create();
    ArkUI_AttributeItem linearStyleItem = {.size = 0, .object = linearStyleOption};
    OH_ArkUI_ProgressLinearStyleOption_SetSmoothEffectEnabled(linearStyleOption, true);
    OH_ArkUI_ProgressLinearStyleOption_SetScanEffectEnabled(linearStyleOption, true);
    OH_ArkUI_ProgressLinearStyleOption_SetStrokeWidth(linearStyleOption, 50.0f);
    OH_ArkUI_ProgressLinearStyleOption_SetStrokeRadius(linearStyleOption, 20.0f);
    nodeAPI->setAttribute(rootNode, NODE_PROGRESS_LINEAR_STYLE, &linearStyleItem);

    EXPECT_EQ(OH_ArkUI_ProgressLinearStyleOption_GetSmoothEffectEnabled(linearStyleOption), true);
    EXPECT_EQ(OH_ArkUI_ProgressLinearStyleOption_GetScanEffectEnabled(linearStyleOption), true);
    EXPECT_EQ(OH_ArkUI_ProgressLinearStyleOption_GetStrokeWidth(linearStyleOption), 50.0f);
    EXPECT_EQ(OH_ArkUI_ProgressLinearStyleOption_GetStrokeRadius(linearStyleOption), 20.0f);

    OH_ArkUI_ProgressLinearStyleOption_Destroy(linearStyleOption);
    nodeAPI->disposeNode(rootNode);
}

/**
 * @tc.name: NativeNodeIssueTest001
 * @tc.desc: Test customSpanNode function.
 * @tc.type: FUNC
 */
HWTEST_F(NativeNodeTest, NativeNodeIssueTest001, TestSize.Level1)
{
    auto nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1*>(
        OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_NODE, "ArkUI_NativeNodeAPI_1"));
    auto node = nodeAPI->createNode(ARKUI_NODE_STACK);
    auto ret = OHOS::Ace::NodeModel::AddNodeEventReceiver(node, [](ArkUI_NodeEvent* event) {
        auto node = OH_ArkUI_NodeEvent_GetNodeHandle(event);
        OHOS::Ace::NodeModel::DisposeNode(node);
    });
    EXPECT_EQ(ret, ARKUI_ERROR_CODE_NO_ERROR);
    ArkUI_NodeEvent event = { 0, 0 };
    event.node = node;
    OHOS::Ace::NodeModel::HandleNodeEvent(&event);
}
} // namespace OHOS::Ace
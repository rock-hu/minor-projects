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

#include <type_traits>
#include "gtest/gtest.h"

#define private public
#define protected public

#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/render/mock_render_context.h"
#include "test/mock/core/rosen/mock_canvas.h"
#include "test/mock/core/rosen/testing_canvas.h"

#include "core/components/common/layout/constants.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/container_modal/container_modal_constants.h"
#include "core/components/select/select_theme.h"
#include "core/components/theme/shadow_theme.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_model_ng.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_pattern.h"
#include "core/components_ng/pattern/menu/menu_item_group/menu_item_group_pattern.h"
#include "core/components_ng/pattern/menu/menu_item_group/menu_item_group_view.h"
#include "core/components_ng/pattern/menu/menu_model_ng.h"
#include "core/components_ng/pattern/menu/menu_pattern.h"
#include "core/components_ng/pattern/menu/menu_theme.h"
#include "core/components_ng/pattern/menu/menu_view.h"
#include "core/components_ng/pattern/menu/multi_menu_layout_algorithm.h"
#include "core/components_ng/pattern/menu/preview/menu_preview_layout_algorithm.h"
#include "core/components_ng/pattern/menu/preview/menu_preview_pattern.h"
#include "core/components_ng/pattern/menu/sub_menu_layout_algorithm.h"
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_pattern.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/root/root_pattern.h"
#include "core/components_ng/pattern/scroll/scroll_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/property/border_property.h"
#include "core/components_ng/property/measure_property.h"
#include "core/components_ng/syntax/lazy_for_each_model.h"
#include "core/components_ng/syntax/lazy_layout_wrapper_builder.h"
#include "core/event/touch_event.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Ace::Framework;

namespace OHOS::Ace::NG {
namespace {
const InspectorFilter filter;
constexpr int32_t TARGET_ID = 3;
constexpr MenuType TYPE = MenuType::MENU;
const std::string MENU_TAG = "menu";
const std::string MENU_TOUCH_EVENT_TYPE = "1";
constexpr float TARGET_SIZE_WIDTH = 100.0f;
constexpr float TARGET_SIZE_HEIGHT = 100.0f;

RefPtr<Theme> GetTheme(ThemeType type)
{
    if (type == TextTheme::TypeId()) {
        return AceType::MakeRefPtr<TextTheme>();
    } else if (type == IconTheme::TypeId()) {
        return AceType::MakeRefPtr<IconTheme>();
    } else if (type == SelectTheme::TypeId()) {
        return AceType::MakeRefPtr<SelectTheme>();
    } else {
        return AceType::MakeRefPtr<MenuTheme>();
    }
}
} // namespace
class CustomMenuItemPatternTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void InitCustomMenuItemPatternTestNg();
    void InitMenuItemTestNg();
    void MockPipelineContextGetTheme();
    PaintWrapper* GetPaintWrapper(RefPtr<MenuPaintProperty> paintProperty);
    RefPtr<FrameNode> GetPreviewMenuWrapper(
        SizeF itemSize = SizeF(0.0f, 0.0f), std::optional<MenuPreviewAnimationOptions> scaleOptions = std::nullopt);
    RefPtr<FrameNode> menuFrameNode_;
    RefPtr<MenuAccessibilityProperty> menuAccessibilityProperty_;
    RefPtr<FrameNode> menuItemFrameNode_;
    RefPtr<MenuItemPattern> menuItemPattern_;
    RefPtr<MenuItemAccessibilityProperty> menuItemAccessibilityProperty_;
};

void CustomMenuItemPatternTestNg::SetUpTestCase() {}

void CustomMenuItemPatternTestNg::TearDownTestCase() {}

void CustomMenuItemPatternTestNg::SetUp()
{
    MockPipelineContext::SetUp();
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    EXPECT_CALL(*themeManager, GetTheme(_, _)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    MockContainer::SetUp();
}

void CustomMenuItemPatternTestNg::MockPipelineContextGetTheme()
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly([](ThemeType type) -> RefPtr<Theme> {
        return GetTheme(type);
    });
    EXPECT_CALL(*themeManager, GetTheme(_, _))
        .WillRepeatedly([](ThemeType type, int32_t themeScopeId) -> RefPtr<Theme> { return GetTheme(type); });
}

void CustomMenuItemPatternTestNg::TearDown()
{
    MockPipelineContext::TearDown();
    menuFrameNode_ = nullptr;
    menuAccessibilityProperty_ = nullptr;
    menuItemFrameNode_ = nullptr;
    menuItemPattern_ = nullptr;
    menuItemAccessibilityProperty_ = nullptr;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    ScreenSystemManager::GetInstance().dipScale_ = 1.0;
    SystemProperties::orientation_ = DeviceOrientation::PORTRAIT;
    MockContainer::TearDown();
}

void CustomMenuItemPatternTestNg::InitCustomMenuItemPatternTestNg()
{
    menuFrameNode_ = FrameNode::GetOrCreateFrameNode(V2::MENU_TAG, ViewStackProcessor::GetInstance()->ClaimNodeId(),
        []() { return AceType::MakeRefPtr<MenuPattern>(TARGET_ID, "", TYPE); });
    ASSERT_NE(menuFrameNode_, nullptr);

    menuAccessibilityProperty_ = menuFrameNode_->GetAccessibilityProperty<MenuAccessibilityProperty>();
    ASSERT_NE(menuAccessibilityProperty_, nullptr);
}

void CustomMenuItemPatternTestNg::InitMenuItemTestNg()
{
    menuItemFrameNode_ = FrameNode::GetOrCreateFrameNode(V2::MENU_ITEM_ETS_TAG,
        ViewStackProcessor::GetInstance()->ClaimNodeId(), []() { return AceType::MakeRefPtr<MenuItemPattern>(); });
    ASSERT_NE(menuItemFrameNode_, nullptr);

    menuItemPattern_ = menuItemFrameNode_->GetPattern<MenuItemPattern>();
    ASSERT_NE(menuItemPattern_, nullptr);

    menuItemAccessibilityProperty_ = menuItemFrameNode_->GetAccessibilityProperty<MenuItemAccessibilityProperty>();
    ASSERT_NE(menuItemAccessibilityProperty_, nullptr);
}

PaintWrapper* CustomMenuItemPatternTestNg::GetPaintWrapper(RefPtr<MenuPaintProperty> paintProperty)
{
    WeakPtr<RenderContext> renderContext;
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    PaintWrapper* paintWrapper = new PaintWrapper(renderContext, geometryNode, paintProperty);
    return paintWrapper;
}

RefPtr<FrameNode> CustomMenuItemPatternTestNg::GetPreviewMenuWrapper(
    SizeF itemSize, std::optional<MenuPreviewAnimationOptions> scaleOptions)
{
    auto rootNode = FrameNode::CreateFrameNode(
        V2::ROOT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<RootPattern>());
    CHECK_NULL_RETURN(rootNode, nullptr);
    auto targetNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    CHECK_NULL_RETURN(targetNode, nullptr);
    auto textNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    CHECK_NULL_RETURN(textNode, nullptr);
    if (!(LessOrEqual(itemSize.Width(), 0.0) || LessOrEqual(itemSize.Height(), 0.0))) {
        auto itemGeometryNode = textNode->GetGeometryNode();
        CHECK_NULL_RETURN(itemGeometryNode, nullptr);
        itemGeometryNode->SetFrameSize(itemSize);
    }
    targetNode->MountToParent(rootNode);
    MenuParam menuParam;
    menuParam.type = MenuType::CONTEXT_MENU;
    menuParam.previewMode = MenuPreviewMode::CUSTOM;
    if (scaleOptions != std::nullopt) {
        menuParam.previewAnimationOptions = scaleOptions.value();
    }
    auto customNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    CHECK_NULL_RETURN(customNode, nullptr);
    auto customGeometryNode = customNode->GetGeometryNode();
    CHECK_NULL_RETURN(customGeometryNode, nullptr);
    customGeometryNode->SetFrameSize(SizeF(TARGET_SIZE_WIDTH, TARGET_SIZE_HEIGHT));
    auto menuWrapperNode =
        MenuView::Create(textNode, targetNode->GetId(), V2::TEXT_ETS_TAG, menuParam, true, customNode);
    return menuWrapperNode;
}

/**
 * @tc.name: Create001
 * @tc.desc: Test CustomMenuItem creation
 * @tc.type: FUNC
 */
HWTEST_F(CustomMenuItemPatternTestNg, Create001, TestSize.Level1)
{
    MenuItemModelNG model;
    auto customNode = FrameNode::CreateFrameNode("", -1, AceType::MakeRefPtr<Pattern>());
    model.Create(customNode);
    auto itemNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_TRUE(itemNode);
    auto pattern = itemNode->GetPattern<CustomMenuItemPattern>();
    ASSERT_TRUE(pattern);
    ASSERT_TRUE(itemNode->GetEventHub<EventHub>());
    auto touch = itemNode->GetOrCreateGestureEventHub()->touchEventActuator_;
    ASSERT_TRUE(touch);
    ASSERT_FALSE(touch->touchEvents_.empty());
}

/**
 * @tc.name: OnTouch001
 * @tc.desc: Test CustomMenuItem OnTouch
 * @tc.type: FUNC
 */
HWTEST_F(CustomMenuItemPatternTestNg, OnTouch001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create menuItem.
     */
    MenuItemModelNG model;
    auto customNode = FrameNode::CreateFrameNode("", -1, AceType::MakeRefPtr<Pattern>());
    model.Create(customNode);
    auto itemNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_TRUE(itemNode);
    auto pattern = itemNode->GetPattern<CustomMenuItemPattern>();
    ASSERT_TRUE(pattern);

    pattern->lastTouchOffset_ = nullptr;
    /**
     * @tc.steps: step2. execute OnTouch
     * @tc.expected: result as expected
     */
    // execute touch down event
    TouchEventInfo info(MENU_TOUCH_EVENT_TYPE);
    TouchLocationInfo locationInfo(TARGET_ID);
    Offset location(1, 1);
    locationInfo.SetTouchType(TouchType::DOWN);
    locationInfo.SetLocalLocation(location);
    info.touches_.emplace_back(locationInfo);
    pattern->OnTouch(info);
    EXPECT_EQ(info.touches_.size(), 1);

    info.touches_.pop_front();
    locationInfo.SetTouchType(TouchType::UP);
    info.touches_.emplace_back(locationInfo);
    pattern->OnTouch(info);
    EXPECT_EQ(info.touches_.size(), 1);

    pattern->lastTouchOffset_ = std::make_unique<Offset>(Offset(1, 0));
    info.touches_.pop_front();
    locationInfo.SetTouchType(TouchType::UP);
    info.touches_.emplace_back(locationInfo);
    pattern->OnTouch(info);
    EXPECT_EQ(info.touches_.size(), 1);

    info.touches_.pop_front();
    locationInfo.SetTouchType(TouchType::CANCEL);
    info.touches_.emplace_back(locationInfo);
    pattern->OnTouch(info);
    EXPECT_EQ(info.touches_.size(), 1);
}

/**
 * @tc.name: OnKeyEvent001
 * @tc.desc: Test CustomMenuItem OnKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(CustomMenuItemPatternTestNg, OnKeyEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create menuItem.
     */
    MenuItemModelNG model;
    auto customNode = FrameNode::CreateFrameNode("", -1, AceType::MakeRefPtr<Pattern>());
    model.Create(customNode);
    auto itemNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_TRUE(itemNode);
    auto pattern = itemNode->GetPattern<CustomMenuItemPattern>();
    ASSERT_TRUE(pattern);

    /**
     * @tc.steps: step2. execute OnKeyEvent
     * @tc.expected: result as expected
     */
    KeyEvent keyEvent(KeyCode::KEY_ESCAPE, KeyAction::DOWN);
    auto ret = pattern->OnKeyEvent(keyEvent);
    EXPECT_FALSE(ret);

    keyEvent.code = KeyCode::KEY_ENTER;
    ret = pattern->OnKeyEvent(keyEvent);
    EXPECT_TRUE(ret);

    keyEvent.action = KeyAction::UP;
    ret = pattern->OnKeyEvent(keyEvent);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MenuItemPatternTestBasicNg028
 * @tc.desc: Verify MenuItemModelNG::Create when version is 12.
 * @tc.type: FUNC
 */
HWTEST_F(CustomMenuItemPatternTestNg, MenuItemPatternTestBasicNg028, TestSize.Level1)
{
    int32_t setApiVersion = 12;
    int32_t rollbackApiVersion = MockContainer::Current()->GetApiTargetVersion();
    MockContainer::Current()->SetApiTargetVersion(setApiVersion);
    MenuItemModelNG model;
    auto customNode = FrameNode::CreateFrameNode("", -1, AceType::MakeRefPtr<Pattern>());
    model.Create(customNode);
    auto itemNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_TRUE(itemNode);
    auto pattern = itemNode->GetPattern<CustomMenuItemPattern>();
    ASSERT_TRUE(pattern);
    ASSERT_TRUE(itemNode->GetEventHub<EventHub>());
    auto touch = itemNode->GetOrCreateGestureEventHub()->touchEventActuator_;
    ASSERT_TRUE(touch);
    ASSERT_FALSE(touch->touchEvents_.empty());
    MockContainer::Current()->SetApiTargetVersion(rollbackApiVersion);
}
} // namespace OHOS::Ace::NG

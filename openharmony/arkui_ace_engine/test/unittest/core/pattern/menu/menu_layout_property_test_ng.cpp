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

#include "base/memory/ace_type.h"
#include "test/mock/base/mock_pixel_map.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "test/mock/core/render/mock_render_context.h"
#include "test/mock/core/rosen/mock_canvas.h"
#include "test/mock/core/rosen/testing_canvas.h"

#include "core/components/common/layout/constants.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/container_modal/container_modal_constants.h"
#include "core/components/button/button_theme.h"
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
const std::string EMPTY_TEXT = "";
const std::string TEXT_TAG = "text";
const std::string MENU_TAG = "menu";
const std::string MENU_ITEM_TEXT = "menuItem";
const std::string MENU_ITEM_GROUP_TEXT = "menuItemGroup";
const std::string MENU_TOUCH_EVENT_TYPE = "1";
const std::string IMAGE_SRC_URL = "file://data/data/com.example.test/res/example.svg";

constexpr float FULL_SCREEN_WIDTH = 720.0f;
constexpr float FULL_SCREEN_HEIGHT = 1136.0f;
constexpr float TARGET_SIZE_WIDTH = 100.0f;
constexpr float TARGET_SIZE_HEIGHT = 100.0f;
constexpr float OFFSET_X = 50.0f;
constexpr float OFFSET_Y = 100.0f;
constexpr float ONE_FLOAT = 1.0f;
constexpr float TWO_FLOAT = 2.0f;
constexpr float FIVE_FLOAT = 5.0f;
constexpr float TEN_FLOAT = 10.0f;
constexpr double MENU_OFFSET_X = 10.0;
constexpr double MENU_OFFSET_Y = 10.0;
const SizeF FULL_SCREEN_SIZE(FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT);
const std::vector<std::string> FONT_FAMILY_VALUE = {"cursive"};
const std::vector<SelectParam> CREATE_VALUE = { { "content1", "icon1" }, { "content2", "" },
    { "", "icon3" }, { "", "" } };
const std::vector<SelectParam> CREATE_VALUE_NEW = { { "content1_new", "" }, { "", "icon4_new" },
    { "", "" }, { "", "icon4_new" } };
const V2::ItemDivider ITEM_DIVIDER = { Dimension(5.f), Dimension(10), Dimension(20), Color(0x000000) };
} // namespace
class MenuLayoutPropertyTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void MockPipelineContextGetTheme();
    void InitMenuLayoutPropertyTestNg();
    void InitMenuItemTestNg();
    PaintWrapper* GetPaintWrapper(RefPtr<MenuPaintProperty> paintProperty);
    RefPtr<FrameNode> GetPreviewMenuWrapper(
        SizeF itemSize = SizeF(0.0f, 0.0f), std::optional<MenuPreviewAnimationOptions> scaleOptions = std::nullopt);
    RefPtr<FrameNode> menuFrameNode_;
    RefPtr<MenuAccessibilityProperty> menuAccessibilityProperty_;
    RefPtr<FrameNode> menuItemFrameNode_;
    RefPtr<MenuItemPattern> menuItemPattern_;
    RefPtr<MenuItemAccessibilityProperty> menuItemAccessibilityProperty_;
};

void MenuLayoutPropertyTestNg::SetUpTestCase() {}

void MenuLayoutPropertyTestNg::TearDownTestCase() {}

void MenuLayoutPropertyTestNg::SetUp()
{
    MockPipelineContext::SetUp();
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    MockContainer::SetUp();
}

void MenuLayoutPropertyTestNg::TearDown()
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

void MenuLayoutPropertyTestNg::MockPipelineContextGetTheme()
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly([](ThemeType type) -> RefPtr<Theme> {
        if (type == TextTheme::TypeId()) {
            return AceType::MakeRefPtr<TextTheme>();
        } else if (type == IconTheme::TypeId()) {
            return AceType::MakeRefPtr<IconTheme>();
        } else if (type == SelectTheme::TypeId()) {
            return AceType::MakeRefPtr<SelectTheme>();
        } else if (type == ButtonTheme::TypeId()) {
            return AceType::MakeRefPtr<ButtonTheme>();
        } else {
            return AceType::MakeRefPtr<MenuTheme>();
        }
    });
}

void MenuLayoutPropertyTestNg::InitMenuLayoutPropertyTestNg()
{
    menuFrameNode_ = FrameNode::GetOrCreateFrameNode(V2::MENU_TAG, ViewStackProcessor::GetInstance()->ClaimNodeId(),
        []() { return AceType::MakeRefPtr<MenuPattern>(TARGET_ID, "", TYPE); });
    ASSERT_NE(menuFrameNode_, nullptr);

    menuAccessibilityProperty_ = menuFrameNode_->GetAccessibilityProperty<MenuAccessibilityProperty>();
    ASSERT_NE(menuAccessibilityProperty_, nullptr);
}

void MenuLayoutPropertyTestNg::InitMenuItemTestNg()
{
    menuItemFrameNode_ = FrameNode::GetOrCreateFrameNode(V2::MENU_ITEM_ETS_TAG,
        ViewStackProcessor::GetInstance()->ClaimNodeId(), []() { return AceType::MakeRefPtr<MenuItemPattern>(); });
    ASSERT_NE(menuItemFrameNode_, nullptr);

    menuItemPattern_ = menuItemFrameNode_->GetPattern<MenuItemPattern>();
    ASSERT_NE(menuItemPattern_, nullptr);

    menuItemAccessibilityProperty_ = menuItemFrameNode_->GetAccessibilityProperty<MenuItemAccessibilityProperty>();
    ASSERT_NE(menuItemAccessibilityProperty_, nullptr);
}

PaintWrapper* MenuLayoutPropertyTestNg::GetPaintWrapper(RefPtr<MenuPaintProperty> paintProperty)
{
    WeakPtr<RenderContext> renderContext;
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    PaintWrapper* paintWrapper = new PaintWrapper(renderContext, geometryNode, paintProperty);
    return paintWrapper;
}

RefPtr<FrameNode> MenuLayoutPropertyTestNg::GetPreviewMenuWrapper(
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
 * @tc.name: GetPositionOffset001
 * @tc.desc: Verify GetPositionOffset.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetPositionOffset001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetPositionOffset().has_value());
    property.UpdatePositionOffset(OffsetF(25.0f, 30.0f));
    ASSERT_TRUE(property.GetPositionOffset().has_value());
    EXPECT_EQ(property.GetPositionOffset().value(), OffsetF(25.0f, 30.0f));
}

/**
 * @tc.name: GetTitle001
 * @tc.desc: Verify GetTitle.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetTitle001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetTitle().has_value());
    property.UpdateTitle("title");
    ASSERT_TRUE(property.GetTitle().has_value());
    EXPECT_EQ(property.GetTitle().value(), "title");
}

/**
 * @tc.name: GetFontSize001
 * @tc.desc: Verify GetFontSize.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetFontSize001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetFontSize().has_value());
    property.UpdateFontSize(Dimension(25.0f));
    ASSERT_TRUE(property.GetFontSize().has_value());
    EXPECT_EQ(property.GetFontSize().value(), Dimension(25.0f));
}

/**
 * @tc.name: GetFontSize002
 * @tc.desc: Verify GetFontSize.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetFontSize002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    MneuModelInstance.SetFontSize(Dimension(50.0));
    ASSERT_FALSE(property->GetFontSize().has_value());
}

/**
 * @tc.name: GetFontColor001
 * @tc.desc: Verify GetFontColor.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetFontColor001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetFontColor().has_value());
    property.UpdateFontColor(Color::RED);
    ASSERT_TRUE(property.GetFontColor().has_value());
    EXPECT_EQ(property.GetFontColor().value(), Color::RED);
}

/**
 * @tc.name: GetFontWeight001
 * @tc.desc: Verify GetFontWeight.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetFontWeight001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetFontWeight().has_value());
    property.UpdateFontWeight(FontWeight::BOLD);
    ASSERT_TRUE(property.GetFontWeight().has_value());
    EXPECT_EQ(property.GetFontWeight().value(), FontWeight::BOLD);
}

/**
 * @tc.name: GetFontWeight002
 * @tc.desc: Verify GetFontWeight.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetFontWeight002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    ViewStackProcessor::GetInstance()->Push(menuNode);
    MneuModelInstance.SetFontWeight(FontWeight::BOLDER);
    ASSERT_TRUE(property->GetFontWeight().has_value());
    EXPECT_EQ(property->GetFontWeight().value(), FontWeight::BOLDER);
    ViewStackProcessor::GetInstance()->Finish();
}

/**
 * @tc.name: GetFontWeight003
 * @tc.desc: Verify GetFontWeight.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetFontWeight003, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    MneuModelInstance.SetFontWeight(FontWeight::BOLDER);
    ASSERT_FALSE(property->GetFontWeight().has_value());
}

/**
 * @tc.name: GetMenuOffset001
 * @tc.desc: Verify GetMenuOffset.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetMenuOffset001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetMenuOffset().has_value());
    property.UpdateMenuOffset(OffsetF(25.0f, 30.0f));
    ASSERT_TRUE(property.GetMenuOffset().has_value());
    EXPECT_EQ(property.GetMenuOffset().value(), OffsetF(25.0f, 30.0f));
}

/**
 * @tc.name: Reset001
 * @tc.desc: Verify Reset.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, Reset001, TestSize.Level1)
{
    MenuLayoutProperty property;
    property.UpdateMenuOffset(OffsetF(25.0f, 30.0f));
    property.UpdatePositionOffset(OffsetF(25.0f, 30.0f));
    property.UpdateTitle("title");
    property.UpdateFontSize(Dimension(25.0f));
    property.UpdateFontColor(Color::RED);
    property.UpdateFontWeight(FontWeight::BOLD);
    property.UpdateAlignType(MenuAlignType::START);
    property.UpdateOffset(DimensionOffset(Dimension(0, DimensionUnit::VP), Dimension(0, DimensionUnit::VP)));
    EXPECT_TRUE(property.GetMenuOffset().has_value());
    EXPECT_TRUE(property.GetPositionOffset().has_value());
    EXPECT_TRUE(property.GetTitle().has_value());
    EXPECT_TRUE(property.GetFontSize().has_value());
    EXPECT_TRUE(property.GetFontColor().has_value());
    EXPECT_TRUE(property.GetFontWeight().has_value());
    EXPECT_TRUE(property.GetAlignType().has_value());
    EXPECT_TRUE(property.GetOffset().has_value());
    property.Reset();
    EXPECT_FALSE(property.GetMenuOffset().has_value());
    EXPECT_FALSE(property.GetPositionOffset().has_value());
    EXPECT_FALSE(property.GetTitle().has_value());
    EXPECT_FALSE(property.GetFontSize().has_value());
    EXPECT_FALSE(property.GetFontColor().has_value());
    EXPECT_FALSE(property.GetFontWeight().has_value());
    EXPECT_FALSE(property.GetAlignType().has_value());
    EXPECT_FALSE(property.GetOffset().has_value());
}

/**
 * @tc.name: Reset002
 * @tc.desc: Verify Reset.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, Reset002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty, update property
     * @tc.expected: property value are as expected.
     */
    MenuPaintProperty property;
    property.UpdateEnableArrow(true);
    property.UpdateArrowOffset(Dimension(10.0f));
    property.UpdateArrowPosition(OffsetF(10.0f, 10.0f));
    property.UpdateArrowPlacement(Placement::TOP);
    EXPECT_TRUE(property.GetEnableArrow().has_value());
    EXPECT_TRUE(property.GetArrowOffset().has_value());
    EXPECT_TRUE(property.GetArrowPosition().has_value());
    EXPECT_TRUE(property.GetArrowPlacement().has_value());
    /**
     * @tc.steps: step2. reset property
     * @tc.expected: property value are as expected.
     */
    property.Reset();
    EXPECT_FALSE(property.GetEnableArrow().has_value());
    EXPECT_FALSE(property.GetArrowOffset().has_value());
    EXPECT_FALSE(property.GetArrowPosition().has_value());
    EXPECT_FALSE(property.GetArrowPlacement().has_value());
}

/**
 * @tc.name: Clone001
 * @tc.desc: Verify Clone.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, Clone001, TestSize.Level1)
{
    MenuLayoutProperty property;
    property.UpdateMenuOffset(OffsetF(25.0f, 30.0f));
    property.UpdatePositionOffset(OffsetF(25.0f, 30.0f));
    property.UpdateTitle("title");
    property.UpdateFontSize(Dimension(25.0f));
    property.UpdateFontColor(Color::RED);
    property.UpdateFontWeight(FontWeight::BOLD);
    property.UpdateAlignType(MenuAlignType::START);
    property.UpdateOffset(DimensionOffset(Dimension(10.0, DimensionUnit::VP), Dimension(10.0, DimensionUnit::VP)));

    auto cloneProperty = AceType::DynamicCast<MenuLayoutProperty>(property.Clone());
    ASSERT_NE(cloneProperty, nullptr);
    EXPECT_EQ(property.GetMenuOffset().value(), cloneProperty->GetMenuOffset().value());
    EXPECT_EQ(property.GetPositionOffset().value(), cloneProperty->GetPositionOffset().value());
    EXPECT_EQ(property.GetTitle().value(), cloneProperty->GetTitle().value());
    EXPECT_EQ(property.GetFontSize().value(), cloneProperty->GetFontSize().value());
    EXPECT_EQ(property.GetFontColor().value(), cloneProperty->GetFontColor().value());
    EXPECT_EQ(property.GetFontWeight().value(), cloneProperty->GetFontWeight().value());
    EXPECT_EQ(property.GetAlignType().value(), cloneProperty->GetAlignType().value());
    EXPECT_EQ(property.GetOffset().value(), cloneProperty->GetOffset().value());
}

/**
 * @tc.name: Clone002
 * @tc.desc: Verify Clone.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, Clone002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty, update property
     */
    MenuPaintProperty property;
    property.UpdateEnableArrow(true);
    property.UpdateArrowOffset(Dimension(10.0f));
    property.UpdateArrowPosition(OffsetF(10.0f, 10.0f));
    property.UpdateArrowPlacement(Placement::TOP);
    /**
     * @tc.steps: step2. clone property
     * @tc.expected: property value are as expected.
     */
    auto cloneProperty = AceType::DynamicCast<MenuPaintProperty>(property.Clone());
    ASSERT_NE(cloneProperty, nullptr);
    EXPECT_EQ(property.GetEnableArrow().value(), cloneProperty->GetEnableArrow().value());
    EXPECT_EQ(property.GetArrowOffset().value(), cloneProperty->GetArrowOffset().value());
    EXPECT_EQ(property.GetArrowPosition().value(), cloneProperty->GetArrowPosition().value());
    EXPECT_EQ(property.GetArrowPlacement().value(), cloneProperty->GetArrowPlacement().value());
}

/**
 * @tc.name: ToJsonValue001
 * @tc.desc: Verify MenuLayoutProperty::ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, ToJsonValue001, TestSize.Level1)
{
    MenuLayoutProperty property;
    property.UpdateMenuOffset(OffsetF(25.0f, 30.0f));
    property.UpdatePositionOffset(OffsetF(25.0f, 30.0f));
    property.UpdateTitle("title");
    property.UpdateFontSize(Dimension(25.0f));
    property.UpdateFontColor(Color::RED);
    property.UpdateFontWeight(FontWeight::BOLD);
    property.UpdateItemDivider(ITEM_DIVIDER);
    property.UpdateItemGroupDivider(ITEM_DIVIDER);
    property.UpdateExpandingMode(SubMenuExpandingMode::EMBEDDED);

    auto node = FrameNode::CreateFrameNode(
        V2::SYMBOL_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    property.SetHost(node);
    auto json = JsonUtil::Create(true);
    property.ToJsonValue(json, filter);
    auto fontJsonObject = json->GetObject("font");
    EXPECT_EQ(json->GetString("title"), "title");
    EXPECT_EQ(json->GetString("offset"), OffsetF(25.0f, 30.0f).ToString());
    EXPECT_EQ(json->GetString("fontSize"), Dimension(25.0f).ToString());
    EXPECT_EQ(json->GetString("fontColor"), Color::RED.ColorToString());
    EXPECT_EQ(fontJsonObject->GetString("weight"), V2::ConvertWrapFontWeightToStirng(FontWeight::BOLD));
    property.UpdateExpandingMode(SubMenuExpandingMode::STACK);
    property.ToJsonValue(json, filter);
    EXPECT_EQ(json->GetString("title"), "title");
    EXPECT_EQ(json->GetString("offset"), OffsetF(25.0f, 30.0f).ToString());
    EXPECT_EQ(json->GetString("fontSize"), Dimension(25.0f).ToString());
    EXPECT_EQ(json->GetString("fontColor"), Color::RED.ColorToString());
    EXPECT_EQ(fontJsonObject->GetString("weight"), V2::ConvertWrapFontWeightToStirng(FontWeight::BOLD));
}

/**
 * @tc.name: ToJsonValue002
 * @tc.desc: Verify ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, ToJsonValue002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem1", "fakeIcon", nullptr);
    optionParams.emplace_back("MenuItem2", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);

    auto json = JsonUtil::Create(true);
    property->ToJsonValue(json, filter);
    auto itemArray = json->GetValue("bindMenu");
    ASSERT_EQ(itemArray->GetArraySize(), 2u);
    auto item1 = itemArray->GetArrayItem(0);
    EXPECT_EQ(item1->GetString("value"), "MenuItem1");
    EXPECT_EQ(item1->GetString("icon"), "fakeIcon");
    auto item2 = itemArray->GetArrayItem(1);
    EXPECT_EQ(item2->GetString("value"), "MenuItem2");
    EXPECT_EQ(item2->GetString("icon"), "");
    EXPECT_EQ(json->GetString("uniRender"), "true");
}


/**
 * @tc.name: ToJsonValue003
 * @tc.desc: Verify ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, ToJsonValue003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty, update property
     */
    MenuPaintProperty property;
    property.UpdateEnableArrow(true);
    property.UpdateArrowOffset(Dimension(10.0f));
    property.UpdateArrowPosition(OffsetF(10.0f, 10.0f));
    property.UpdateArrowPlacement(Placement::TOP);
    /**
     * @tc.steps: step2. property to json
     * @tc.expected: property jsonValue are as expected.
     */
    auto json = JsonUtil::Create(true);
    property.ToJsonValue(json, filter);
    EXPECT_EQ(json->GetString("enableArrow"), V2::ConvertBoolToString(true).c_str());
    EXPECT_EQ(json->GetString("arrowOffset"), Dimension(10.0f).ToString().c_str());
    EXPECT_EQ(json->GetString("arrowPosition"), OffsetF(10.0f, 10.0f).ToString().c_str());
    EXPECT_EQ(json->GetString("arrowPlacement"), property.ConvertPlacementToString(Placement::TOP).c_str());
}

/**
 * @tc.name: ToJsonValue004
 * @tc.desc: Verify ToJsonValue default value.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, ToJsonValue004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty
     */
    MenuPaintProperty property;
    /**
     * @tc.steps: step2. property to json
     * @tc.expected: property jsonValue are as expected.
     */
    auto json = JsonUtil::Create(true);
    property.ToJsonValue(json, filter);
    EXPECT_EQ(json->GetString("enableArrow"), V2::ConvertBoolToString(false).c_str());
    EXPECT_EQ(json->GetString("arrowOffset"), Dimension(0.0, DimensionUnit::VP).ToString().c_str());
    EXPECT_EQ(json->GetString("arrowPosition"), OffsetF(0.0f, 0.0f).ToString().c_str());
    EXPECT_EQ(json->GetString("arrowPlacement"), property.ConvertPlacementToString(Placement::NONE).c_str());
}

/**
 * @tc.name: GetAlignType001
 * @tc.desc: Verify SelectMenuAlignOption AlignType.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetAlignType001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetAlignType().has_value());
    /**
     * @tc.cases: case1. verify the alignType property.
     */
    property.UpdateAlignType(MenuAlignType::CENTER);
    ASSERT_TRUE(property.GetAlignType().has_value());
    EXPECT_EQ(property.GetAlignType().value(), MenuAlignType::CENTER);
}

/**
 * @tc.name: GetOffset001
 * @tc.desc: Verify SelectMenuAlignOption Offset.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetOffset001, TestSize.Level1)
{
    MenuLayoutProperty property;
    EXPECT_FALSE(property.GetOffset().has_value());
    DimensionOffset offset(Dimension(MENU_OFFSET_X, DimensionUnit::VP), Dimension(MENU_OFFSET_Y, DimensionUnit::VP));
    /**
     * @tc.cases: case1. verify the offset property.
     */
    property.UpdateOffset(offset);
    ASSERT_TRUE(property.GetOffset().has_value());
    EXPECT_EQ(property.GetOffset().value(), offset);
}

/**
 * @tc.name: SetFontSize001
 * @tc.desc: Verify SetFontSize.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, SetFontSize001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    ViewStackProcessor::GetInstance()->Push(menuNode);
    MneuModelInstance.SetFontSize(Dimension(50.0));
    ASSERT_TRUE(property->GetFontSize().has_value());
    EXPECT_EQ(property->GetFontSize().value(), Dimension(50.0));
    ViewStackProcessor::GetInstance()->Finish();
}

/**
 * @tc.name: SetFontSize002
 * @tc.desc: Verify SetFontSize.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, SetFontSize002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    ViewStackProcessor::GetInstance()->Push(menuNode);
    MneuModelInstance.SetFontSize(Dimension());
    EXPECT_FALSE(property->GetFontSize().has_value());
    ViewStackProcessor::GetInstance()->Finish();
}

/**
 * @tc.name: SetFontColor001
 * @tc.desc: Verify SetFontColor.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, SetFontColor001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    ViewStackProcessor::GetInstance()->Push(menuNode);
    MneuModelInstance.SetFontColor(Color::RED);
    ASSERT_TRUE(property->GetFontColor().has_value());
    EXPECT_EQ(property->GetFontColor().value(), Color::RED);
    ViewStackProcessor::GetInstance()->Finish();
}

/**
 * @tc.name: SetFontColor002
 * @tc.desc: Verify SetFontColor.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, SetFontColor002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    MneuModelInstance.SetFontColor(Color::RED);
    ASSERT_FALSE(property->GetFontColor().has_value());
}

/**
 * @tc.name: SetFontColor003
 * @tc.desc: Verify SetFontColor.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, SetFontColor003, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    MenuModelNG MneuModelInstance;
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    ViewStackProcessor::GetInstance()->Push(menuNode);
    MneuModelInstance.SetFontColor(Color::RED);
    ASSERT_TRUE(property->GetFontColor().has_value());
    EXPECT_EQ(property->GetFontColor().value(), Color::RED);
    MneuModelInstance.SetFontColor(std::nullopt);
    ASSERT_FALSE(property->GetFontColor().has_value());
    ViewStackProcessor::GetInstance()->Finish();
}

/**
 * @tc.name: GetMenuPlacement001
 * @tc.desc: Verify GetMenuPlacement.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetMenuPlacement001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create menuWrapperNode with menuItems and set MenuPlacement to Placement::TOP
     * @tc.expected: step1. Get menuPlacement is Placement::TOP
     */
    MockPipelineContextGetTheme();
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem", "", nullptr);
    MenuParam menuParam;
    menuParam.placement = OHOS::Ace::Placement::TOP;
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), TARGET_ID, "", TYPE, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    EXPECT_TRUE(property->GetMenuPlacement().has_value());
    EXPECT_EQ(property->GetMenuPlacement().value(), OHOS::Ace::Placement::TOP);
}

/**
 * @tc.name: GetMenuPlacement002
 * @tc.desc: Verify GetMenuPlacement.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetMenuPlacement002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create menuWrapperNode with custom node from a builder and set MenuPlacement to
     * Placement::BOTTOM
     * @tc.expected: step1. Get menuPlacement is Placement::BOTTOM
     */
    auto textNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    ASSERT_NE(textNode, nullptr);
    MenuParam menuParam;
    menuParam.placement = OHOS::Ace::Placement::BOTTOM;
    menuParam.type = TYPE;
    auto menuWrapperNode = MenuView::Create(textNode, TARGET_ID, "", menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    EXPECT_TRUE(property->GetMenuPlacement().has_value());
    EXPECT_EQ(property->GetMenuPlacement().value(), OHOS::Ace::Placement::BOTTOM);
}

/**
 * @tc.name: GetEnableArrow001
 * @tc.desc: Verify GetEnableArrow.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetEnableArrow001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty.
     * @tc.expected: enableArrow has not value.
     */
    MenuPaintProperty property;
    EXPECT_FALSE(property.GetEnableArrow().has_value());
    /**
     * @tc.steps: step2. update enableArrow
     * @tc.expected: enableArrow value are as expected.
     */
    property.UpdateEnableArrow(true);
    ASSERT_TRUE(property.GetEnableArrow().has_value());
    EXPECT_TRUE(property.GetEnableArrow().value());
}

/**
 * @tc.name: GetArrowOffset001
 * @tc.desc: Verify GetArrowOffset.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetArrowOffset001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty.
     * @tc.expected: arrowOffset has not value.
     */
    MenuPaintProperty property;
    EXPECT_FALSE(property.GetArrowOffset().has_value());
    /**
     * @tc.steps: step2. update arrowOffset
     * @tc.expected: arrowOffset value are as expected.
     */
    property.UpdateArrowOffset(Dimension(10.0f));
    ASSERT_TRUE(property.GetArrowOffset().has_value());
    EXPECT_EQ(property.GetArrowOffset().value(), Dimension(10.0f));
}

/**
 * @tc.name: GetArrowPosition001
 * @tc.desc: Verify GetArrowPosition.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetArrowPosition001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty.
     * @tc.expected: arrowPosition has not value.
     */
    MenuPaintProperty property;
    EXPECT_FALSE(property.GetArrowPosition().has_value());
    /**
     * @tc.steps: step2. update arrowPosition
     * @tc.expected: arrowPosition value are as expected.
     */
    property.UpdateArrowPosition(OffsetF(10.0f, 10.0f));
    ASSERT_TRUE(property.GetArrowPosition().has_value());
    EXPECT_EQ(property.GetArrowPosition().value(), OffsetF(10.0f, 10.0f));
}

/**
 * @tc.name: GetArrowPlacement001
 * @tc.desc: Verify GetArrowPlacement.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetArrowPlacement001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a object MenuPaintProperty.
     * @tc.expected: arrowPlacement has not value.
     */
    MenuPaintProperty property;
    EXPECT_FALSE(property.GetArrowPlacement().has_value());
    /**
     * @tc.steps: step2. update arrowPlacement
     * @tc.expected: arrowPlacement value are as expected.
     */
    property.UpdateArrowPlacement(Placement::TOP);
    ASSERT_TRUE(property.GetArrowPlacement().has_value());
    EXPECT_EQ(property.GetArrowPlacement().value(), Placement::TOP);
}

/**
 * @tc.name: ExpandSymbol001
 * @tc.desc: Verify SetExpandSymbol and SetExpandSymbol.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, ExpandSymbol001, TestSize.Level1)
{
    MenuLayoutProperty property;

    EXPECT_EQ(property.GetExpandSymbol(), nullptr);
    property.SetExpandSymbol([](WeakPtr<NG::FrameNode> weakPtr) {});
    EXPECT_NE(property.GetExpandSymbol(), nullptr);
}

/**
 * @tc.name: ToJsonValue005
 * @tc.desc: Verify MaskToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, ToJsonValue005, TestSize.Level1)
{
    auto wrapperNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 1, AceType::MakeRefPtr<MenuWrapperPattern>(1));
    ASSERT_NE(wrapperNode, nullptr);
    auto menuNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, TEXT_TAG, MenuType::MENU));
    ASSERT_NE(menuNode, nullptr);
    menuNode->MountToParent(wrapperNode);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    auto json = JsonUtil::Create(true);
    property->MaskToJsonValue(json, filter);
    auto maskJsonObject = json->GetObject("mask");
    EXPECT_EQ(maskJsonObject->GetString("mask"), "false");

    auto columnNode = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, 3, AceType::MakeRefPtr<LinearLayoutPattern>(true));
    ASSERT_NE(columnNode, nullptr);
    auto menuWrapperPattern = wrapperNode->GetPattern<MenuWrapperPattern>();
    ASSERT_NE(menuWrapperPattern, nullptr);
    menuWrapperPattern->SetFilterColumnNode(columnNode);
    json = JsonUtil::Create(true);
    property->MaskToJsonValue(json, filter);
    maskJsonObject = json->GetObject("mask");
    EXPECT_EQ(maskJsonObject->GetString("mask"), "true");

    auto renderContext = columnNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
    BlurStyleOption styleOption;
    styleOption.blurStyle = BlurStyle::BACKGROUND_THIN;
    renderContext->UpdateBackgroundColor(Color::RED);
    renderContext->UpdateBackBlurStyle(styleOption);
    json = JsonUtil::Create(true);
    property->MaskToJsonValue(json, filter);
    maskJsonObject = json->GetObject("mask");
    auto backgroundBlurStyleJsonObject = maskJsonObject->GetObject("backgroundBlurStyle");
    EXPECT_EQ(maskJsonObject->GetString("mask"), "true");
    EXPECT_EQ(maskJsonObject->GetString("color"), Color::RED.ToString().c_str());
    EXPECT_EQ(backgroundBlurStyleJsonObject->GetString("value"), "BlurStyle.BACKGROUND_THIN");
}

/**
 * @tc.name: CheckPreviewConstraintForConstant001
 * @tc.desc: Verify CheckPreviewConstraintForConstant.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, CheckPreviewConstraintForConstant001, TestSize.Level1)
{
    RefPtr<GeometryNode> previewGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    MenuLayoutAlgorithm menuLayoutAlgorithm;
    menuLayoutAlgorithm.CheckPreviewConstraintForConstant(previewGeometryNode);
    menuLayoutAlgorithm.previewScaleMode_ = PreviewScaleMode::CONSTANT;
    auto frameNode = FrameNode::CreateFrameNode(
        V2::MENU_PREVIEW_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, TEXT_TAG, MenuType::MENU));
    Rect menuWindowRect(TARGET_SIZE_WIDTH, TARGET_SIZE_WIDTH, TARGET_SIZE_WIDTH, TARGET_SIZE_WIDTH);
    menuLayoutAlgorithm.CheckPreviewConstraint(frameNode, menuWindowRect);
    auto geometryNode = frameNode->GetGeometryNode();
    EXPECT_EQ(geometryNode->frame_.rect_.width_, 0);
}

/**
 * @tc.name: LayoutNormalTopPreviewBottomMenuGreateThan001
 * @tc.desc: Verify LayoutNormalTopPreviewBottomMenuGreateThan.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, LayoutNormalTopPreviewBottomMenuGreateThan001, TestSize.Level1)
{
    RefPtr<GeometryNode> previewGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<GeometryNode> menuGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    SizeF totalSize(TARGET_SIZE_WIDTH, TARGET_SIZE_HEIGHT);
    MenuLayoutAlgorithm menuLayoutAlgorithm;
    menuLayoutAlgorithm.previewScaleMode_ = PreviewScaleMode::CONSTANT;
    menuLayoutAlgorithm.targetOffset_ = OffsetF(OFFSET_X, OFFSET_Y);
    menuLayoutAlgorithm.targetSize_ = SizeF(TARGET_SIZE_WIDTH, TARGET_SIZE_HEIGHT);
    menuLayoutAlgorithm.LayoutNormalTopPreviewBottomMenuGreateThan(previewGeometryNode, menuGeometryNode, totalSize);
    menuLayoutAlgorithm.LayoutNormalBottomPreviewTopMenuGreateThan(previewGeometryNode, menuGeometryNode, totalSize);
    menuLayoutAlgorithm.LayoutOtherDeviceLeftPreviewRightMenuGreateThan(
        previewGeometryNode, menuGeometryNode, totalSize);
    EXPECT_EQ(menuLayoutAlgorithm.targetCenterOffset_.x_, 100);
}

/**
 * @tc.name: LayoutPreviewMenuGreateThanForConstant001
 * @tc.desc: Verify LayoutPreviewMenuGreateThanForConstant.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, LayoutPreviewMenuGreateThanForConstant001, TestSize.Level1)
{
    RefPtr<GeometryNode> previewGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<GeometryNode> menuGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    MenuLayoutAlgorithm menuLayoutAlgorithm;
    menuLayoutAlgorithm.placement_ = Placement::BOTTOM_LEFT;
    menuLayoutAlgorithm.LayoutPreviewMenuGreateThanForConstant(previewGeometryNode, menuGeometryNode);
    menuLayoutAlgorithm.placement_ = Placement::BOTTOM;
    menuLayoutAlgorithm.LayoutPreviewMenuGreateThanForConstant(previewGeometryNode, menuGeometryNode);
    menuLayoutAlgorithm.placement_ = Placement::BOTTOM_RIGHT;
    menuLayoutAlgorithm.LayoutPreviewMenuGreateThanForConstant(previewGeometryNode, menuGeometryNode);
    menuLayoutAlgorithm.placement_ = Placement::TOP_LEFT;
    menuLayoutAlgorithm.targetOffset_ = OffsetF(OFFSET_X, OFFSET_Y);
    menuLayoutAlgorithm.wrapperRect_.SetRect(OFFSET_X, OFFSET_Y, TARGET_SIZE_WIDTH, TARGET_SIZE_HEIGHT);
    menuLayoutAlgorithm.LayoutPreviewMenuGreateThanForConstant(previewGeometryNode, menuGeometryNode);
    menuLayoutAlgorithm.placement_ = Placement::TOP;
    menuLayoutAlgorithm.LayoutPreviewMenuGreateThanForConstant(previewGeometryNode, menuGeometryNode);
    menuLayoutAlgorithm.placement_ = Placement::TOP_RIGHT;
    menuLayoutAlgorithm.LayoutPreviewMenuGreateThanForConstant(previewGeometryNode, menuGeometryNode);
    menuLayoutAlgorithm.placement_ = Placement::LEFT;
    menuGeometryNode->frame_.rect_.height_ = 200.0f;
    menuLayoutAlgorithm.LayoutPreviewMenuGreateThanForConstant(previewGeometryNode, menuGeometryNode);
    EXPECT_EQ(previewGeometryNode->frame_.rect_.x_, OFFSET_X);
}

/**
 * @tc.name: UpdateLayoutConstraintForPreview001
 * @tc.desc: Verify UpdateLayoutConstraintForPreview.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, UpdateLayoutConstraintForPreview001, TestSize.Level1)
{
    auto wrapperNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 1, AceType::MakeRefPtr<MenuWrapperPattern>(1));
    auto frameNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(frameNode, nullptr);
    frameNode->MountToParent(wrapperNode);
    auto refLayoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(refLayoutWrapper, nullptr);
    LayoutWrapper* layoutWrapper = Referenced::RawPtr(refLayoutWrapper);
    RefPtr<FrameNode> node =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 3, AceType::MakeRefPtr<MenuPreviewPattern>());
    ASSERT_NE(node, nullptr);
    RefPtr<FrameNode> node1 =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 4, AceType::MakeRefPtr<MenuWrapperPattern>(2));
    ASSERT_NE(node1, nullptr);
    RefPtr<FrameNode> node2 =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 5, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(node2, nullptr);
    node2->MountToParent(node1);
    node->MountToParent(node1);
    layoutWrapper->hostNode_ = std::move(node);
    MenuPreviewLayoutAlgorithm menuPreviewLayoutAlgorithm;
    menuPreviewLayoutAlgorithm.UpdateLayoutConstraintForPreview(layoutWrapper);
    EXPECT_EQ(layoutWrapper->GetLayoutProperty()->contentConstraint_, std::nullopt);
}

/**
 * @tc.name: UpdateLayoutConstraintForPreview002
 * @tc.desc: Verify UpdateLayoutConstraintForPreview.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, UpdateLayoutConstraintForPreview002, TestSize.Level1)
{
    auto wrapperNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 1, AceType::MakeRefPtr<MenuWrapperPattern>(1));
    auto frameNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(frameNode, nullptr);
    frameNode->MountToParent(wrapperNode);
    auto refLayoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(refLayoutWrapper, nullptr);
    LayoutWrapper* layoutWrapper = Referenced::RawPtr(refLayoutWrapper);
    RefPtr<FrameNode> node =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 3, AceType::MakeRefPtr<MenuPreviewPattern>());
    ASSERT_NE(node, nullptr);
    RefPtr<FrameNode> node1 =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 4, AceType::MakeRefPtr<MenuWrapperPattern>(2));
    ASSERT_NE(node1, nullptr);
    RefPtr<FrameNode> node2 =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 5, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(node2, nullptr);
    auto menuPattern = node2->GetPattern<MenuPattern>();
    ASSERT_NE(menuPattern, nullptr);
    menuPattern->targetSize_ = SizeF(OFFSET_X, OFFSET_Y);
    node2->MountToParent(node1);
    node->MountToParent(node1);
    auto menuWrapperPattern = node1->GetPattern<MenuWrapperPattern>();
    ASSERT_NE(menuWrapperPattern, nullptr);
    menuWrapperPattern->menuParam_.isPreviewContainScale = true;
    layoutWrapper->hostNode_ = std::move(node);
    auto layoutProperty = layoutWrapper->GetLayoutProperty();
    ASSERT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraintF = {
        .minSize = { ONE_FLOAT, ONE_FLOAT },
        .maxSize = { TEN_FLOAT, TEN_FLOAT },
        .percentReference = { FIVE_FLOAT, FIVE_FLOAT },
        .parentIdealSize = { TWO_FLOAT, TWO_FLOAT },
    };
    layoutProperty->contentConstraint_ = layoutConstraintF;
    MenuPreviewLayoutAlgorithm menuPreviewLayoutAlgorithm;
    menuPreviewLayoutAlgorithm.UpdateLayoutConstraintForPreview(layoutWrapper);
    EXPECT_EQ(layoutProperty->contentConstraint_.value().maxSize.width_, TEN_FLOAT);
}

/**
 * @tc.name: UpdateLayoutConstraintForPreview003
 * @tc.desc: Verify UpdateLayoutConstraintForPreview.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, UpdateLayoutConstraintForPreview003, TestSize.Level1)
{
    auto wrapperNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 1, AceType::MakeRefPtr<MenuWrapperPattern>(1));
    auto frameNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(frameNode, nullptr);
    frameNode->MountToParent(wrapperNode);
    auto refLayoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(refLayoutWrapper, nullptr);
    LayoutWrapper* layoutWrapper = Referenced::RawPtr(refLayoutWrapper);
    RefPtr<FrameNode> node =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 3, AceType::MakeRefPtr<MenuPreviewPattern>());
    ASSERT_NE(node, nullptr);
    RefPtr<FrameNode> node1 =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 4, AceType::MakeRefPtr<MenuWrapperPattern>(2));
    ASSERT_NE(node1, nullptr);
    RefPtr<FrameNode> node2 =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 5, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(node2, nullptr);
    node2->MountToParent(node1);
    node->MountToParent(node1);
    auto menuWrapperPattern = node1->GetPattern<MenuWrapperPattern>();
    ASSERT_NE(menuWrapperPattern, nullptr);
    menuWrapperPattern->menuParam_.isPreviewContainScale = true;
    layoutWrapper->hostNode_ = std::move(node);
    MenuPreviewLayoutAlgorithm menuPreviewLayoutAlgorithm;
    menuPreviewLayoutAlgorithm.UpdateLayoutConstraintForPreview(layoutWrapper);
    EXPECT_EQ(layoutWrapper->GetLayoutProperty()->contentConstraint_, std::nullopt);
}

/**
 * @tc.name: CheckLayoutConstraint001
 * @tc.desc: Verify CheckLayoutConstraint.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, CheckLayoutConstraint001, TestSize.Level1)
{
    auto wrapperNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 1, AceType::MakeRefPtr<MenuWrapperPattern>(1));
    auto frameNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(frameNode, nullptr);
    frameNode->MountToParent(wrapperNode);
    auto refLayoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(refLayoutWrapper, nullptr);
    LayoutWrapper* layoutWrapper = Referenced::RawPtr(refLayoutWrapper);
    RefPtr<MenuPattern> menuPattern = AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU);
    MenuParam menuParam;
    auto layoutProperty = layoutWrapper->GetLayoutProperty();
    ASSERT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraintF = {
        .minSize = { ONE_FLOAT, ONE_FLOAT },
        .maxSize = { TEN_FLOAT, TEN_FLOAT },
        .percentReference = { FIVE_FLOAT, FIVE_FLOAT },
        .parentIdealSize = { TWO_FLOAT, TWO_FLOAT },
    };
    layoutProperty->contentConstraint_ = layoutConstraintF;
    menuParam.availableLayoutAreaMode = static_cast<AvailableLayoutAreaMode>(999);
    MenuPreviewLayoutAlgorithm menuPreviewLayoutAlgorithm;
    menuPreviewLayoutAlgorithm.CheckLayoutConstraint(layoutWrapper, menuParam, menuPattern);
    EXPECT_EQ(layoutProperty->contentConstraint_->percentReference.width_, FIVE_FLOAT);
}

/**
 * @tc.name: CheckLayoutConstraint002
 * @tc.desc: Verify CheckLayoutConstraint.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, CheckLayoutConstraint002, TestSize.Level1)
{
    auto wrapperNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 1, AceType::MakeRefPtr<MenuWrapperPattern>(1));
    auto frameNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(frameNode, nullptr);
    frameNode->MountToParent(wrapperNode);
    auto refLayoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(refLayoutWrapper, nullptr);
    LayoutWrapper* layoutWrapper = Referenced::RawPtr(refLayoutWrapper);
    RefPtr<MenuPattern> menuPattern = AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU);
    MenuParam menuParam;
    auto layoutProperty = layoutWrapper->GetLayoutProperty();
    ASSERT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraintF = {
        .minSize = { ONE_FLOAT, ONE_FLOAT },
        .maxSize = { TEN_FLOAT, TEN_FLOAT },
        .percentReference = { FIVE_FLOAT, FIVE_FLOAT },
        .parentIdealSize = { TWO_FLOAT, TWO_FLOAT },
    };
    layoutProperty->contentConstraint_ = layoutConstraintF;
    menuParam.availableLayoutAreaMode = AvailableLayoutAreaMode::SAFE_AREA;
    menuParam.previewScaleMode = PreviewScaleMode::MAINTAIN;
    MenuPreviewLayoutAlgorithm menuPreviewLayoutAlgorithm;
    menuPreviewLayoutAlgorithm.CheckLayoutConstraint(layoutWrapper, menuParam, menuPattern);
    EXPECT_EQ(layoutProperty->contentConstraint_->percentReference.width_, 0);
}

/**
 * @tc.name: GetPreviewNodeTotalSize001
 * @tc.desc: Verify GetPreviewNodeTotalSize in different PreviewScaleMode.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutPropertyTestNg, GetPreviewNodeTotalSize001, TestSize.Level1)
{
    auto wrapperNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, 1, AceType::MakeRefPtr<MenuWrapperPattern>(1));
    ASSERT_NE(wrapperNode, nullptr);
    auto imageNode = FrameNode::CreateFrameNode(V2::IMAGE_ETS_TAG, 2, AceType::MakeRefPtr<ImagePattern>());
    ASSERT_NE(imageNode, nullptr);
    auto imageLayoutWrapper = imageNode->CreateLayoutWrapper();
    ASSERT_NE(imageLayoutWrapper, nullptr);
    auto menuNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, 2, AceType::MakeRefPtr<MenuPattern>(1, "test", MenuType::MENU));
    ASSERT_NE(menuNode, nullptr);
    auto menuLayoutWrapper = menuNode->CreateLayoutWrapper();
    ASSERT_NE(menuLayoutWrapper, nullptr);
    imageNode->MountToParent(wrapperNode);
    menuNode->MountToParent(wrapperNode);
    
    MenuLayoutAlgorithm menuLayoutAlgorithm;
    SizeF size;
    Rect menuWindowRect(TARGET_SIZE_WIDTH, TARGET_SIZE_WIDTH, TARGET_SIZE_WIDTH, TARGET_SIZE_WIDTH);
    RefPtr<LayoutWrapper> previewLayoutWrapper;
    menuLayoutAlgorithm.GetPreviewNodeTotalSize(
        imageLayoutWrapper, menuWindowRect, previewLayoutWrapper, size, menuLayoutWrapper);
    EXPECT_NE(previewLayoutWrapper, nullptr);
    previewLayoutWrapper = nullptr;
    menuLayoutAlgorithm.previewScaleMode_ = PreviewScaleMode::CONSTANT;
    menuLayoutAlgorithm.GetPreviewNodeTotalSize(
        imageLayoutWrapper, menuWindowRect, previewLayoutWrapper, size, menuLayoutWrapper);
    EXPECT_NE(previewLayoutWrapper, nullptr);
}
} // namespace OHOS::Ace::NG

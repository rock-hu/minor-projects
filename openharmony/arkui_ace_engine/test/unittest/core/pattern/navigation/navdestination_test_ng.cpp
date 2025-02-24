/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#define protected public
#define private public
#include "core/common/agingadapation/aging_adapation_dialog_theme.h"
#include "core/common/agingadapation/aging_adapation_dialog_util.h"
#include "core/components/dialog/dialog_theme.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/navigation/navigation_title_util.h"
#include "core/components_ng/pattern/navigation/title_bar_layout_algorithm.h"
#include "core/components_ng/pattern/navigation/title_bar_layout_property.h"
#include "core/components_ng/pattern/navigation/title_bar_node.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navrouter/navdestination_layout_property.h"
#include "core/components_ng/pattern/navrouter/navdestination_model_ng.h"
#include "core/components_ng/pattern/navrouter/navdestination_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/pipeline/pipeline_base.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "mock_navigation_stack.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string NAVIGATION_TITLE = "NavdestinationTestNg";
const std::string NAVIGATION_SUBTITLE = "NavdestinationSubtitle";
constexpr float TITLEBAR_WIDTH = 480.0f;
constexpr float TITLEBAR_HEIGHT = 100.0f;
constexpr Dimension MAX_PADDING_START = 28.0_vp;
constexpr float BACK_BUTTON_FRAME_SIZE = 32.0f;
constexpr float TITLE_FRAME_WIDTH = 60.0f;
constexpr float TITLE_FRAME_HEIGHT = 30.0f;
constexpr float SUBTITLE_FRAME_WIDTH = 60.0f;
constexpr float SUBTITLE_FRAME_HEIGHT = 20.0f;
const CalcDimension DEFAULT_PADDING = 24.0_vp;
const Dimension DEFAULT_MENU_BUTTON_PADDING = 8.0_vp;

struct UIComponents {
    RefPtr<LayoutWrapperNode> layoutWrapper = nullptr;
    RefPtr<FrameNode> frameNode = nullptr;
    RefPtr<LayoutWrapperNode> titleBarLayoutWrapper = nullptr;
    RefPtr<NavDestinationGroupNode> navDestinationGroupNode = nullptr;
    RefPtr<TitleBarNode> titleBarNode = nullptr;
    RefPtr<TitleBarLayoutProperty> titleBarLayoutProperty = nullptr;
    RefPtr<TitleBarPattern> titleBarPattern = nullptr;
    RefPtr<TitleBarLayoutAlgorithm> titleBarLayoutAlgorithm = nullptr;
    RefPtr<GeometryNode> backButtonGeometryNode = nullptr;
    RefPtr<GeometryNode> titleGeometryNode = nullptr;
    RefPtr<GeometryNode> subtitleGeometryNode = nullptr;
};

RefPtr<Theme> GetTheme(ThemeType type)
{
    if (type == NavigationBarTheme::TypeId()) {
        return AceType::MakeRefPtr<NavigationBarTheme>();
    } else if (type == AgingAdapationDialogTheme::TypeId()) {
        return AceType::MakeRefPtr<AgingAdapationDialogTheme>();
    } else {
        return AceType::MakeRefPtr<DialogTheme>();
    }
}
} // namespace

class NavdestinationTestNg : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    void MockPipelineContextGetTheme();
    RefPtr<LayoutWrapperNode> CreateBackButtonWrapper(
        const RefPtr<TitleBarNode>& titleBarNode, const RefPtr<LayoutWrapperNode>& titleBarLayoutWrapper);
    RefPtr<LayoutWrapperNode> CreateTitleWrapper(
        const RefPtr<TitleBarNode>& titleBarNode, const RefPtr<LayoutWrapperNode>& titleBarLayoutWrapper);
    RefPtr<LayoutWrapperNode> CreateSubtitleWrapper(
        const RefPtr<TitleBarNode>& titleBarNode, const RefPtr<LayoutWrapperNode>& titleBarLayoutWrapper);
    RefPtr<LayoutWrapperNode> CreateTitleBarWrapper(
        const RefPtr<FrameNode>& frameNode, const RefPtr<LayoutWrapperNode>& layoutWrapper);
    RefPtr<LayoutWrapperNode> CreateNavDestinationWrapper();
    void InitChildrenComponent(UIComponents& ui);
    void InitMenuNodeItemInfos(std::vector<NG::BarItem>& menuItems);
};

void NavdestinationTestNg::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
}

void NavdestinationTestNg::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
}

void NavdestinationTestNg::MockPipelineContextGetTheme()
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly([](ThemeType type) -> RefPtr<Theme> {
        return GetTheme(type);
    });
    EXPECT_CALL(*themeManager, GetTheme(_, _))
        .WillRepeatedly([](ThemeType type, int32_t themeScopeId) -> RefPtr<Theme> { return GetTheme(type); });
}

RefPtr<LayoutWrapperNode> NavdestinationTestNg::CreateNavDestinationWrapper()
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    navDestinationModelNG.SetTitle(NAVIGATION_TITLE, true);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_RETURN(frameNode, nullptr);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    CHECK_NULL_RETURN(geometryNode, nullptr);
    auto layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(frameNode, geometryNode, frameNode->GetLayoutProperty());
    CHECK_NULL_RETURN(layoutWrapper, nullptr);
    return layoutWrapper;
}

RefPtr<LayoutWrapperNode> NavdestinationTestNg::CreateTitleBarWrapper(
    const RefPtr<FrameNode>& frameNode, const RefPtr<LayoutWrapperNode>& layoutWrapper)
{
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    CHECK_NULL_RETURN(navDestinationNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, nullptr);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_RETURN(titleBarLayoutProperty, nullptr);
    auto titleBarGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    CHECK_NULL_RETURN(titleBarGeometryNode, nullptr);
    auto titleBarLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(titleBarNode, titleBarGeometryNode, titleBarLayoutProperty);
    CHECK_NULL_RETURN(titleBarLayoutWrapper, nullptr);
    layoutWrapper->AppendChild(titleBarLayoutWrapper);
    titleBarGeometryNode->SetFrameSize(SizeF(TITLEBAR_WIDTH, TITLEBAR_HEIGHT));
    return titleBarLayoutWrapper;
}

RefPtr<LayoutWrapperNode> NavdestinationTestNg::CreateBackButtonWrapper(
    const RefPtr<TitleBarNode>& titleBarNode, const RefPtr<LayoutWrapperNode>& titleBarLayoutWrapper)
{
    auto backButtonNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetBackButton());
    CHECK_NULL_RETURN(backButtonNode, nullptr);
    auto backButtonLayoutProperty = backButtonNode->GetLayoutProperty();
    CHECK_NULL_RETURN(backButtonLayoutProperty, nullptr);
    auto backButtonGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    CHECK_NULL_RETURN(backButtonGeometryNode, nullptr);
    auto backButtonLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(backButtonNode, backButtonGeometryNode, backButtonLayoutProperty);
    CHECK_NULL_RETURN(backButtonLayoutWrapper, nullptr);
    backButtonGeometryNode->SetFrameSize(SizeF(BACK_BUTTON_FRAME_SIZE, BACK_BUTTON_FRAME_SIZE));
    titleBarLayoutWrapper->AppendChild(backButtonLayoutWrapper);
    return backButtonLayoutWrapper;
}

RefPtr<LayoutWrapperNode> NavdestinationTestNg::CreateTitleWrapper(
    const RefPtr<TitleBarNode>& titleBarNode, const RefPtr<LayoutWrapperNode>& titleBarLayoutWrapper)
{
    auto titleNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetTitle());
    CHECK_NULL_RETURN(titleNode, nullptr);
    auto titleLayoutProperty = titleNode->GetLayoutProperty();
    CHECK_NULL_RETURN(titleLayoutProperty, nullptr);
    auto titleGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    CHECK_NULL_RETURN(titleGeometryNode, nullptr);
    auto titleLayoutWrapper = AceType::MakeRefPtr<LayoutWrapperNode>(titleNode, titleGeometryNode, titleLayoutProperty);
    CHECK_NULL_RETURN(titleLayoutWrapper, nullptr);
    titleGeometryNode->SetFrameSize(SizeF(TITLE_FRAME_WIDTH, TITLE_FRAME_HEIGHT));
    titleBarLayoutWrapper->AppendChild(titleLayoutWrapper);
    return titleLayoutWrapper;
}

RefPtr<LayoutWrapperNode> NavdestinationTestNg::CreateSubtitleWrapper(
    const RefPtr<TitleBarNode>& titleBarNode, const RefPtr<LayoutWrapperNode>& titleBarLayoutWrapper)
{
    auto subtitleNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetSubtitle());
    auto subtitleLayoutProperty = subtitleNode->GetLayoutProperty();
    CHECK_NULL_RETURN(subtitleLayoutProperty, nullptr);
    auto subtitleGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    CHECK_NULL_RETURN(subtitleGeometryNode, nullptr);
    auto subtitleLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapperNode>(subtitleNode, subtitleGeometryNode, subtitleLayoutProperty);
    CHECK_NULL_RETURN(subtitleLayoutWrapper, nullptr);
    subtitleGeometryNode->SetFrameSize(SizeF(SUBTITLE_FRAME_WIDTH, SUBTITLE_FRAME_HEIGHT));
    titleBarLayoutWrapper->AppendChild(subtitleLayoutWrapper);
    return subtitleLayoutWrapper;
}

void NavdestinationTestNg::InitChildrenComponent(UIComponents& ui)
{
    AceApplicationInfo::GetInstance().isRightToLeft_ = false;
    ui.layoutWrapper = CreateNavDestinationWrapper();
    ASSERT_NE(ui.layoutWrapper, nullptr);
    ui.frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(ui.frameNode, nullptr);
    ui.titleBarLayoutWrapper = CreateTitleBarWrapper(ui.frameNode, ui.layoutWrapper);
    ASSERT_NE(ui.titleBarLayoutWrapper, nullptr);
    ui.navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(ui.navDestinationGroupNode, nullptr);
    ui.titleBarNode = AceType::DynamicCast<TitleBarNode>(ui.navDestinationGroupNode->GetTitleBarNode());
    ASSERT_NE(ui.titleBarNode, nullptr);
    auto backButtonLayoutWrapper = CreateBackButtonWrapper(ui.titleBarNode, ui.titleBarLayoutWrapper);
    ASSERT_NE(backButtonLayoutWrapper, nullptr);
    auto titleLayoutWrapper = CreateTitleWrapper(ui.titleBarNode, ui.titleBarLayoutWrapper);
    ASSERT_NE(titleLayoutWrapper, nullptr);
    auto subtitleLayoutWrapper = CreateSubtitleWrapper(ui.titleBarNode, ui.titleBarLayoutWrapper);
    ASSERT_NE(subtitleLayoutWrapper, nullptr);
    ui.titleBarLayoutProperty = ui.titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    ASSERT_NE(ui.titleBarLayoutProperty, nullptr);
    ui.titleBarPattern = AceType::DynamicCast<TitleBarPattern>(ui.titleBarNode->GetPattern());
    ASSERT_NE(ui.titleBarPattern, nullptr);
    ui.titleBarLayoutAlgorithm =
        AccessibilityManager::DynamicCast<TitleBarLayoutAlgorithm>(ui.titleBarPattern->CreateLayoutAlgorithm());
    ASSERT_NE(ui.titleBarLayoutAlgorithm, nullptr);
    ui.backButtonGeometryNode = backButtonLayoutWrapper->GetGeometryNode();
    ASSERT_NE(ui.backButtonGeometryNode, nullptr);
    ui.titleGeometryNode = titleLayoutWrapper->GetGeometryNode();
    ASSERT_NE(ui.titleGeometryNode, nullptr);
    ui.subtitleGeometryNode = subtitleLayoutWrapper->GetGeometryNode();
    ASSERT_NE(ui.subtitleGeometryNode, nullptr);
    ui.titleBarLayoutAlgorithm->showBackButton_ = true;
    ui.titleBarLayoutAlgorithm->maxPaddingStart_ = MAX_PADDING_START;
    ui.titleBarLayoutAlgorithm->LayoutBackButton(
        AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode, ui.titleBarLayoutProperty);
    ui.titleBarLayoutAlgorithm->LayoutTitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, SUBTITLE_FRAME_HEIGHT);
    ui.titleBarLayoutAlgorithm->LayoutSubtitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, TITLE_FRAME_HEIGHT);
}

void NavdestinationTestNg::InitMenuNodeItemInfos(std::vector<NG::BarItem>& menuItems)
{
    bool isEnabled = true;
    std::function<void()> menuAction = [&isEnabled]() {
        isEnabled = !isEnabled;
    };
    BarItem menuItemOne = {
        .text = "menuItemOne",
        .icon = "file://data/data/com.example.test/res/example.svg",
        .isEnabled = isEnabled,
        .action = menuAction };
    menuItems.push_back(menuItemOne);
    BarItem menuItemTwo = {
        .text = "menuItemTwo",
        .icon = "file://data/data/com.example.test/res/example.svg",
        .isEnabled = isEnabled,
        .action = menuAction };
    menuItems.push_back(menuItemTwo);
    BarItem menuItemThree = {
        .text = "menuItemThree",
        .icon = "file://data/data/com.example.test/res/example.svg",
        .isEnabled = isEnabled,
        .action = menuAction };
    menuItems.push_back(menuItemThree);
    BarItem menuItemFour = {
        .text = "menuItemFour",
        .icon = "file://data/data/com.example.test/res/example.svg",
        .isEnabled = isEnabled,
        .action = menuAction };
    menuItems.push_back(menuItemFour);
    BarItem menuItemFive = {
        .text = "menuItemFive",
        .icon = "file://data/data/com.example.test/res/example.svg",
        .isEnabled = isEnabled,
        .action = menuAction };
    menuItems.push_back(menuItemFive);
    BarItem menuItemSix = {
        .text = "menuItemSix",
        .icon = "file://data/data/com.example.test/res/example.svg",
        .isEnabled = isEnabled,
        .action = menuAction };
    menuItems.push_back(menuItemSix);
}

/**
 * @tc.name: NavDestinationTest001
 * @tc.desc: Test Create.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationTest001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    NavDestinationModelNG.Create();
    NavDestinationModelNG.SetHideTitleBar(true, false);
    NavDestinationModelNG.SetTitle(NAVIGATION_TITLE, true);
    NavDestinationModelNG.SetSubtitle(NAVIGATION_SUBTITLE);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navDestinationPattern = navigationGroupNode->GetPattern<NavDestinationPattern>();
    ASSERT_NE(navDestinationPattern, nullptr);
    auto navDestinationLayoutProperty = navDestinationPattern->GetLayoutProperty<NavDestinationLayoutProperty>();
    ASSERT_NE(navDestinationLayoutProperty, nullptr);
}

/**
 * @tc.name: NavDestinationTest003
 * @tc.desc: Test Create.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationTest003, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    auto builderFunc = []() {};
    NavDestinationModelNG.Create(std::move(builderFunc));
    NavDestinationModelNG.SetHideTitleBar(false, false);
    NavDestinationModelNG.SetTitle(NAVIGATION_TITLE, false);
    NavDestinationModelNG.SetSubtitle(NAVIGATION_SUBTITLE);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
}

/**
 * @tc.name: NavDestinationTest004
 * @tc.desc: Test CustomTitle.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationTest004, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    auto builderFunc = []() {};
    auto onBackPressed = []() -> bool { return true; };
    NavDestinationModelNG.Create(std::move(builderFunc));
    NavDestinationModelNG.SetHideTitleBar(false, false);
    NavDestinationModelNG.SetTitle(NAVIGATION_TITLE, false);
    NavDestinationModelNG.SetSubtitle(NAVIGATION_SUBTITLE);
    NavDestinationModelNG.SetOnShown(std::move(builderFunc));
    NavDestinationModelNG.SetOnHidden(std::move(builderFunc));
    NavDestinationModelNG.SetOnBackPressed(std::move(onBackPressed));

    RefPtr<NG::UINode> customNode;
    NavDestinationModelNG.SetCustomTitle(customNode);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
}

/**
 * @tc.name: NavDestinationTest005
 * @tc.desc: Test SetBackButtonIcon.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationTest005, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    auto builderFunc = []() {};
    std::string imageSource = "src";
    bool noPixMap = true;
    RefPtr<PixelMap> pixMap = nullptr;
    std::vector<std::string> nameList;
    ImageOption imageOption;
    nameList.push_back("");
    nameList.push_back("");
    imageOption.noPixMap = noPixMap;
    imageOption.isValidImage = true;
    auto onApply = [](WeakPtr<NG::FrameNode> frameNode) {
        auto node = frameNode.Upgrade();
        CHECK_NULL_VOID(node);
    };
    std::function<void(WeakPtr<NG::FrameNode>)> iconSymbol = onApply;
    navDestinationModelNG.Create(std::move(builderFunc));
    navDestinationModelNG.SetBackButtonIcon(iconSymbol, imageSource, imageOption, pixMap, nameList);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    ASSERT_NE(titleBarLayoutProperty, nullptr);
    ASSERT_EQ(titleBarLayoutProperty->GetPixelMap(), nullptr);
    ASSERT_TRUE(titleBarLayoutProperty->GetNoPixMap());
    ASSERT_NE(titleBarLayoutProperty->GetBackIconSymbol(), nullptr);
    ImageSourceInfo imageSourceInfo = titleBarLayoutProperty->GetImageSourceValue();
    ASSERT_EQ(imageSourceInfo.GetSrc(), imageSource);
}

/**
 * @tc.name: NavDestinationOnReadyTest001
 * @tc.desc: Test onReady of NavDestination
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationOnReadyTest001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(ElementRegister::GetInstance()->MakeUniqueId());

    /**
     * @tc.steps: step1. create navdestination and set onReady callback
     */
    NavDestinationModelNG navDestinationModel;
    navDestinationModel.Create([]() {}, AceType::MakeRefPtr<NavDestinationContext>());
    bool onReadyFired = false;
    navDestinationModel.SetOnReady([&onReadyFired](RefPtr<NG::NavDestinationContext>) { onReadyFired = true; });

    /**
     * @tc.steps: step2. process shallowBuilder
     * @tc.expected: check if onReady was called
     */
    auto groupNode = AceType::DynamicCast<NavDestinationGroupNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(groupNode, nullptr);
    groupNode->ProcessShallowBuilder();
    EXPECT_TRUE(onReadyFired);
}

/**
 * @tc.name: NavdestinationOnLanguageConfigurationUpdateTest001
 * @tc.desc: Test OnLanguageConfigurationUpdate of NavDestination
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationOnLanguageConfigurationUpdateTest001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    /**
     * @tc.steps: step1. create NavDestination
     */
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    NavDestinationModelNG.Create();
    NavDestinationModelNG.SetHideTitleBar(true, false);
    NavDestinationModelNG.SetTitle(NAVIGATION_TITLE, true);
    NavDestinationModelNG.SetSubtitle(NAVIGATION_SUBTITLE);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navDestinationPattern = navigationGroupNode->GetPattern<NavDestinationPattern>();
    ASSERT_NE(navDestinationPattern, nullptr);
    /**
     * @tc.steps: step2. set the system language to right to left and call OnLanguageConfigurationUpdate
     * @tc.expected: NavDestination's isRightToLeft_ status is true
     */
    AceApplicationInfo::GetInstance().isRightToLeft_ = true;
    navDestinationPattern->OnLanguageConfigurationUpdate();
    EXPECT_TRUE(navDestinationPattern->isRightToLeft_);
    /**
     * @tc.steps: step3. set the system language to left to right and call OnLanguageConfigurationUpdate
     * @tc.expected: NavDestination's isRightToLeft_ status is false
     */
    AceApplicationInfo::GetInstance().isRightToLeft_ = false;
    navDestinationPattern->OnLanguageConfigurationUpdate();
    EXPECT_FALSE(navDestinationPattern->isRightToLeft_);
}

/**
 * @tc.name: NavdestinationMirrorLayoutTest001
 * @tc.desc: test VERSION_NINE mirror layout of NavDestination title bar
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationMirrorLayoutTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_NINE and create NavDestination.
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    MockPipelineContext::GetCurrent()->SetMinPlatformVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    /**
     * @tc.steps: step2. set the system language to left to right and initialize children component.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    auto ltrBackButton = ui.backButtonGeometryNode->GetMarginFrameOffset().GetX();
    auto ltrTitle = ui.titleGeometryNode->GetMarginFrameOffset().GetX();
    auto ltrSubtitle = ui.subtitleGeometryNode->GetMarginFrameOffset().GetX();
    /**
     * @tc.steps: step3. set the system language to right to left, start layout and compare coordinates.
     * @tc.expected: NewOffsetX = titleBarWidth - componentWidth - offsetX.
     */
    AceApplicationInfo::GetInstance().isRightToLeft_ = true;
    ui.titleBarLayoutAlgorithm->LayoutBackButton(
        AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode, ui.titleBarLayoutProperty);
    ui.titleBarLayoutAlgorithm->LayoutTitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, SUBTITLE_FRAME_HEIGHT);
    ui.titleBarLayoutAlgorithm->LayoutSubtitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, TITLE_FRAME_HEIGHT);
    EXPECT_EQ(ui.backButtonGeometryNode->GetMarginFrameOffset().GetX(),
        TITLEBAR_WIDTH - BACK_BUTTON_FRAME_SIZE - ltrBackButton);
    EXPECT_EQ(ui.titleGeometryNode->GetMarginFrameOffset().GetX(), TITLEBAR_WIDTH - TITLE_FRAME_WIDTH - ltrTitle);
    EXPECT_EQ(
        ui.subtitleGeometryNode->GetMarginFrameOffset().GetX(), TITLEBAR_WIDTH - SUBTITLE_FRAME_WIDTH - ltrSubtitle);
}

/**
 * @tc.name: NavdestinationMirrorLayoutTest002
 * @tc.desc: test VERSION_TEN mirror layout of NavDestination title bar
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationMirrorLayoutTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_TEN and create NavDestination.
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TEN));
    MockPipelineContext::GetCurrent()->SetMinPlatformVersion(static_cast<int32_t>(PlatformVersion::VERSION_TEN));
    /**
     * @tc.steps: step2. initialize children component and get default offset.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    auto ltrBackButton = ui.backButtonGeometryNode->GetMarginFrameOffset().GetX();
    auto ltrTitle = ui.titleGeometryNode->GetMarginFrameOffset().GetX();
    auto ltrSubtitle = ui.subtitleGeometryNode->GetMarginFrameOffset().GetX();
    /**
     * @tc.steps: step3. set the system language to right to left, start layout and compare coordinates.
     * @tc.expected: NewOffsetX = titleBarWidth - componentWidth - offsetX.
     */
    AceApplicationInfo::GetInstance().isRightToLeft_ = true;
    ui.titleBarLayoutAlgorithm->LayoutBackButton(
        AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode, ui.titleBarLayoutProperty);
    ui.titleBarLayoutAlgorithm->LayoutTitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, SUBTITLE_FRAME_HEIGHT);
    ui.titleBarLayoutAlgorithm->LayoutSubtitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, TITLE_FRAME_HEIGHT);
    EXPECT_EQ(ui.backButtonGeometryNode->GetMarginFrameOffset().GetX(),
        TITLEBAR_WIDTH - BACK_BUTTON_FRAME_SIZE - ltrBackButton);
    EXPECT_EQ(ui.titleGeometryNode->GetMarginFrameOffset().GetX(), TITLEBAR_WIDTH - TITLE_FRAME_WIDTH - ltrTitle);
    EXPECT_EQ(
        ui.subtitleGeometryNode->GetMarginFrameOffset().GetX(), TITLEBAR_WIDTH - SUBTITLE_FRAME_WIDTH - ltrSubtitle);
}

/**
 * @tc.name: NavdestinationMirrorLayoutTest003
 * @tc.desc: test VERSION_TWELVE mirror layout of NavDestination title bar
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationMirrorLayoutTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_TWELVE and create NavDestination.
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    MockPipelineContext::GetCurrent()->SetMinPlatformVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    /**
     * @tc.steps: step2. set the system language to left to right and initialize children component.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    auto ltrBackButton = ui.backButtonGeometryNode->GetMarginFrameOffset().GetX();
    auto ltrTitle = ui.titleGeometryNode->GetMarginFrameOffset().GetX();
    auto ltrSubtitle = ui.subtitleGeometryNode->GetMarginFrameOffset().GetX();
    /**
     * @tc.steps: step3. set the system language to right to left, start layout and compare coordinates.
     * @tc.expected: NewOffsetX = titleBarWidth - componentWidth - offsetX.
     */
    AceApplicationInfo::GetInstance().isRightToLeft_ = true;
    ui.titleBarLayoutAlgorithm->LayoutBackButton(
        AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode, ui.titleBarLayoutProperty);
    ui.titleBarLayoutAlgorithm->LayoutTitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, SUBTITLE_FRAME_HEIGHT);
    ui.titleBarLayoutAlgorithm->LayoutSubtitle(AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode,
        ui.titleBarLayoutProperty, TITLE_FRAME_HEIGHT);
    EXPECT_EQ(ui.backButtonGeometryNode->GetMarginFrameOffset().GetX(),
        TITLEBAR_WIDTH - BACK_BUTTON_FRAME_SIZE - ltrBackButton);
    EXPECT_EQ(ui.titleGeometryNode->GetMarginFrameOffset().GetX(), TITLEBAR_WIDTH - TITLE_FRAME_WIDTH - ltrTitle);
    EXPECT_EQ(
        ui.subtitleGeometryNode->GetMarginFrameOffset().GetX(), TITLEBAR_WIDTH - SUBTITLE_FRAME_WIDTH - ltrSubtitle);
}

/**
 * @tc.name: MenuHandleLongPressTest001
 * @tc.desc: test 1.75 scale of Menu long press event
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, MenuHandleLongPressTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_NINE and set font scale to 1.75.
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 1.75f;
    /**
     * @tc.steps: step2. create NavDestination and initialize children component.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    BarItem menuItem;
    std::vector<NG::BarItem> menuItems;
    menuItems.insert(menuItems.begin(), menuItem);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    ui.titleBarNode->AddChild(navDestinationMenuItems);
    /**
     * @tc.steps: step2. call HandleMenuLongPress.
     * @tc.expected: dialog_ != nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);
    /**
     * @tc.steps: step3. call HandleMenuLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);
}
/**
 * @tc.name: MenuHandleLongPressTest002
 * @tc.desc: test 2.0 scale of Menu long press event
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, MenuHandleLongPressTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_NINE and set font scale to 2.0.
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 2.0f;
    /**
     * @tc.steps: step2. create NavDestination and initialize children component.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    BarItem menuItem;
    std::vector<NG::BarItem> menuItems;
    menuItems.insert(menuItems.begin(), menuItem);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    ui.titleBarNode->AddChild(navDestinationMenuItems);
    /**
     * @tc.steps: step2. call HandleMenuLongPress.
     * @tc.expected: dialog_ != nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);
    /**
     * @tc.steps: step3. call HandleMenuLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);
}

/**
 * @tc.name: MenuHandleLongPressTest003
 * @tc.desc: test 3.2 scale of Menu long press event
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, MenuHandleLongPressTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_NINE and set font scale to 3.2.
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 3.2f;
    /**
     * @tc.steps: step2. create NavDestination and initialize children component.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    BarItem menuItem;
    std::vector<NG::BarItem> menuItems;
    menuItems.insert(menuItems.begin(), menuItem);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    ui.titleBarNode->AddChild(navDestinationMenuItems);
    /**
     * @tc.steps: step2. call HandleMenuLongPress.
     * @tc.expected: dialog_ != nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);
    /**
     * @tc.steps: step3. call HandleMenuLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);
}

/**
 * @tc.name: NavigationTitleUtilHandleLongPressTest004
 * @tc.desc: test 1.75 scale of NavigationTitleUtil with more button menuItems above VersionTen
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavigationTitleUtilHandleLongPressTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_TWELVE and set font scale to 1.75.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    MockContainer::SetUp();
    int32_t apiTargetVersion = MockContainer::Current()->GetApiTargetVersion();
    MockContainer::Current()->SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 1.75f;

    /**
     * @tc.steps: step2. create NavDestination and initialize children component. create more than five menu node.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    std::vector<NG::BarItem> menuItems;
    InitMenuNodeItemInfos(menuItems);
    EXPECT_EQ(menuItems.size(), 6);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    for (const auto& child : navDestinationMenuItems->GetChildren()) {
        child->SetActive(true);
    }
    ui.titleBarNode->AddChild(navDestinationMenuItems);

    /**
     * @tc.steps: step3. call HandleLongPress.
     * @tc.expected: dialog_ == nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step4. call HandleLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step5. recover api version and relative environment
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
    MockContainer::Current()->SetApiTargetVersion(apiTargetVersion);
    MockContainer::TearDown();
}

/**
 * @tc.name: NavigationTitleUtilHandleLongPressTest005
 * @tc.desc: test 1.75 scale of NavigationTitleUtil with more button menuItems below VersionTen
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavigationTitleUtilHandleLongPressTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_NINE and set font scale to 1.75.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    MockContainer::SetUp();
    int32_t apiTargetVersion = MockContainer::Current()->GetApiTargetVersion();
    MockContainer::Current()->SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 1.75f;

    /**
     * @tc.steps: step2. create NavDestination and initialize children component. create more than five menu node.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    std::vector<NG::BarItem> menuItems;
    InitMenuNodeItemInfos(menuItems);
    EXPECT_EQ(menuItems.size(), 6);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    for (const auto& child : navDestinationMenuItems->GetChildren()) {
        child->SetActive(true);
    }
    ui.titleBarNode->AddChild(navDestinationMenuItems);

    /**
     * @tc.steps: step3. call HandleLongPress.
     * @tc.expected: dialog_ == nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step4. call HandleLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step5. recover api version and relative environment
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
    MockContainer::Current()->SetApiTargetVersion(apiTargetVersion);
    MockContainer::TearDown();
}

/**
 * @tc.name: NavigationTitleUtilHandleLongPressTest006
 * @tc.desc: test 2.0 scale of NavigationTitleUtil with more button menuItems above VersionTen
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavigationTitleUtilHandleLongPressTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_TWELVE and set font scale to 2.0.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    MockContainer::SetUp();
    int32_t apiTargetVersion = MockContainer::Current()->GetApiTargetVersion();
    MockContainer::Current()->SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 2.0f;

    /**
     * @tc.steps: step2. create NavDestination and initialize children component. create more than five menu node.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    std::vector<NG::BarItem> menuItems;
    InitMenuNodeItemInfos(menuItems);
    EXPECT_EQ(menuItems.size(), 6);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    for (const auto& child : navDestinationMenuItems->GetChildren()) {
        child->SetActive(true);
    }
    ui.titleBarNode->AddChild(navDestinationMenuItems);

    /**
     * @tc.steps: step3. call HandleLongPress.
     * @tc.expected: dialog_ == nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step4. call HandleLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step5. recover api version and relative environment
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
    MockContainer::Current()->SetApiTargetVersion(apiTargetVersion);
    MockContainer::TearDown();
}

/**
 * @tc.name: NavigationTitleUtilHandleLongPressTest007
 * @tc.desc: test 2.0 scale of NavigationTitleUtil with more button menuItems below VersionTen
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavigationTitleUtilHandleLongPressTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_NINE and set font scale to 2.0.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    MockContainer::SetUp();
    int32_t apiTargetVersion = MockContainer::Current()->GetApiTargetVersion();
    MockContainer::Current()->SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 2.0f;

    /**
     * @tc.steps: step2. create NavDestination and initialize children component. create more than five menu node.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    std::vector<NG::BarItem> menuItems;
    InitMenuNodeItemInfos(menuItems);
    EXPECT_EQ(menuItems.size(), 6);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    for (const auto& child : navDestinationMenuItems->GetChildren()) {
        child->SetActive(true);
    }
    ui.titleBarNode->AddChild(navDestinationMenuItems);

    /**
     * @tc.steps: step3. call HandleLongPress.
     * @tc.expected: dialog_ == nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step4. call HandleLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step5. recover api version and relative environment
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
    MockContainer::Current()->SetApiTargetVersion(apiTargetVersion);
    MockContainer::TearDown();
}

/**
 * @tc.name: NavigationTitleUtilHandleLongPressTest008
 * @tc.desc: test 3.2 scale of NavigationTitleUtil with more button menuItems above VersionTen
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavigationTitleUtilHandleLongPressTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_TWELVE and set font scale to 3.2.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    MockContainer::SetUp();
    int32_t apiTargetVersion = MockContainer::Current()->GetApiTargetVersion();
    MockContainer::Current()->SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 3.2f;

    /**
     * @tc.steps: step2. create NavDestination and initialize children component. create more than five menu node.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    std::vector<NG::BarItem> menuItems;
    InitMenuNodeItemInfos(menuItems);
    EXPECT_EQ(menuItems.size(), 6);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    for (const auto& child : navDestinationMenuItems->GetChildren()) {
        child->SetActive(true);
    }
    ui.titleBarNode->AddChild(navDestinationMenuItems);

    /**
     * @tc.steps: step3. call HandleLongPress.
     * @tc.expected: dialog_ == nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step4. call HandleLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step5. recover api version and relative environment
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
    MockContainer::Current()->SetApiTargetVersion(apiTargetVersion);
    MockContainer::TearDown();
}

/**
 * @tc.name: NavigationTitleUtilHandleLongPressTest009
 * @tc.desc: test 3.2 scale of NavigationTitleUtil with more button menuItems below VersionTen
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavigationTitleUtilHandleLongPressTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set platform version to VERSION_NINE and set font scale to 3.2.
     */
    int32_t backupApiVersion = AceApplicationInfo::GetInstance().GetApiTargetVersion();
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    MockContainer::SetUp();
    int32_t apiTargetVersion = MockContainer::Current()->GetApiTargetVersion();
    MockContainer::Current()->SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_NINE));
    auto context = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->fontScale_ = 3.2f;

    /**
     * @tc.steps: step2. create NavDestination and initialize children component. create more than five menu node.
     */
    UIComponents ui;
    InitChildrenComponent(ui);
    std::vector<NG::BarItem> menuItems;
    InitMenuNodeItemInfos(menuItems);
    EXPECT_EQ(menuItems.size(), 6);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(ui.frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto navDestinationMenuItems = NavigationTitleUtil::CreateMenuItems(ElementRegister::GetInstance()->MakeUniqueId(),
        menuItems, navDestinationNode, true, DES_FIELD, ui.titleBarNode->GetInnerParentId());
    ASSERT_NE(navDestinationMenuItems, nullptr);
    for (const auto& child : navDestinationMenuItems->GetChildren()) {
        child->SetActive(true);
    }
    ui.titleBarNode->AddChild(navDestinationMenuItems);

    /**
     * @tc.steps: step3. call HandleLongPress.
     * @tc.expected: dialog_ == nullptr
     */
    GestureEvent info;
    info.globalLocation_.deltaX_ = 0.0f;
    info.globalLocation_.deltaY_ = 0.0f;
    ui.titleBarPattern->HandleMenuLongPress(info, navDestinationMenuItems, menuItems);
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step4. call HandleLongPressActionEnd.
     * @tc.expected: dialog_ == nullptr
     */
    ui.titleBarPattern->HandleMenuLongPressActionEnd();
    EXPECT_EQ(ui.titleBarPattern->GetLargeFontPopUpDialogNode(), nullptr);

    /**
     * @tc.steps: step5. recover api version and relative environment
     */
    AceApplicationInfo::GetInstance().SetApiTargetVersion(backupApiVersion);
    MockContainer::Current()->SetApiTargetVersion(apiTargetVersion);
    MockContainer::TearDown();
}

/**
 * @tc.name: TitleBarLayoutAlgorithmGetFullModeTitleOffsetYTest001
 * @tc.desc: test GetFullModeTitleOffsetY function
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, TitleBarLayoutAlgorithmGetFullModeTitleOffsetYTest001, TestSize.Level1)
{
    UIComponents ui;
    InitChildrenComponent(ui);
    float titleHeight = 1.0f;
    float subtitleHeight = 0.0f;
    ui.titleBarLayoutAlgorithm->menuOccupiedHeight_ = 2.0f;
    auto titleBarGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(titleBarGeometryNode, nullptr);
    titleBarGeometryNode->SetFrameSize(SizeF(100, 100));
    float offsetY = 48.5f;
    EXPECT_EQ(ui.titleBarLayoutAlgorithm->GetFullModeTitleOffsetY(
        titleHeight, subtitleHeight, titleBarGeometryNode), offsetY);
}

/**
 * @tc.name: TitleBarLayoutAlgorithmGetFullModeTitleOffsetYTest002
 * @tc.desc: test GetFullModeTitleOffsetY function
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, TitleBarLayoutAlgorithmGetFullModeTitleOffsetYTest002, TestSize.Level1)
{
    UIComponents ui;
    InitChildrenComponent(ui);
    float titleHeight = 100.0f;
    float subtitleHeight = 1.0f;
    ui.titleBarLayoutAlgorithm->menuOccupiedHeight_  = 2.0f;
    auto titleBarGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(titleBarGeometryNode, nullptr);
    titleBarGeometryNode->SetFrameSize(SizeF(100, 100));
    ui.titleBarLayoutAlgorithm->navTitleSpaceVertical_ = 1.0f;
    float offsetY = -4.0f;
    EXPECT_EQ(ui.titleBarLayoutAlgorithm->GetFullModeTitleOffsetY(
        titleHeight, subtitleHeight, titleBarGeometryNode), offsetY);
}

/**
 * @tc.name: TitleBarLayoutAlgorithmGetFullModeTitleOffsetYTest003
 * @tc.desc: test GetFullModeTitleOffsetY function
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, TitleBarLayoutAlgorithmGetFullModeTitleOffsetYTest003, TestSize.Level1)
{
    UIComponents ui;
    InitChildrenComponent(ui);
    float titleHeight = 99.0f;
    float subtitleHeight = 1.0f;
    ui.titleBarLayoutAlgorithm->menuOccupiedHeight_  = 0.0f;
    auto titleBarGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(titleBarGeometryNode, nullptr);
    titleBarGeometryNode->SetFrameSize(SizeF(100, 100));
    ui.titleBarLayoutAlgorithm->navTitleSpaceVertical_ = 0.0f;
    float offsetY = 0.0f;
    EXPECT_EQ(ui.titleBarLayoutAlgorithm->GetFullModeTitleOffsetY(
        titleHeight, subtitleHeight, titleBarGeometryNode), offsetY);
}

/**
 * @tc.name: SetHideBackButton001
 * @tc.desc: Test SetHideBackButton function.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetHideBackButton001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    NavDestinationModelNG.Create();
    NavDestinationModelNG.SetHideBackButton(true);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    ASSERT_NE(navDestinationLayoutProperty, nullptr);
    auto hideBackButtonValue = navDestinationLayoutProperty->GetHideBackButtonValue(false);
    ASSERT_EQ(hideBackButtonValue, true);
}

/**
 * @tc.name: SetHideBackButton002
 * @tc.desc: Test SetHideBackButton function.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetHideBackButton002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    NavDestinationModelNG.Create();
    NavDestinationModelNG.SetHideBackButton(false);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    ASSERT_NE(navDestinationLayoutProperty, nullptr);
    auto hideBackButtonValue = navDestinationLayoutProperty->GetHideBackButtonValue(false);
    ASSERT_EQ(hideBackButtonValue, false);
}

/**
 * @tc.name: SetTitlebarOptions001
 * @tc.desc: Test SetTitlebarOptions function.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetTitlebarOptions001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    navDestinationModelNG.SetTitle("navDestinationModel", true);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationGroupNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);

    NavigationTitlebarOptions opt;
    opt.bgOptions.color = std::make_optional(Color(0xff0000ff));
    opt.bgOptions.blurStyle = std::make_optional(BlurStyle::NO_MATERIAL);
    opt.brOptions.barStyle = std::make_optional(BarStyle::STACK);
    opt.brOptions.paddingStart = std::make_optional(DEFAULT_PADDING);
    opt.brOptions.paddingEnd = std::make_optional(DEFAULT_PADDING);

    navDestinationModelNG.SetTitlebarOptions(std::move(opt));

    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

    auto options = titleBarPattern->GetTitleBarOptions();
    EXPECT_TRUE(options.bgOptions.color.has_value());
    EXPECT_EQ(options.bgOptions.color.value(), Color(0xff0000ff));

    EXPECT_TRUE(options.bgOptions.blurStyle.has_value());
    EXPECT_EQ(options.bgOptions.blurStyle.value(), BlurStyle::NO_MATERIAL);

    EXPECT_TRUE(options.brOptions.barStyle.has_value());
    EXPECT_EQ(options.brOptions.barStyle.value(), BarStyle::STACK);

    EXPECT_TRUE(options.brOptions.paddingStart.has_value());
    EXPECT_EQ(options.brOptions.paddingStart.value(), DEFAULT_PADDING);
    
    EXPECT_TRUE(options.brOptions.paddingEnd.has_value());
    EXPECT_EQ(options.brOptions.paddingEnd.value(), DEFAULT_PADDING);
}

/**
 * @tc.name: SetTitlebarOptions002
 * @tc.desc: Test SetTitlebarOptions function with specific node.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetTitlebarOptions002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    navDestinationModelNG.SetTitle("navDestinationModel", true);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationGroupNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);

    NavigationTitlebarOptions opt;
    opt.bgOptions.color = std::make_optional(Color(0xff00ff00));
    opt.bgOptions.blurStyle = std::make_optional(BlurStyle::REGULAR);
    opt.brOptions.barStyle = std::make_optional(BarStyle::STANDARD);
    opt.brOptions.paddingStart = std::make_optional(DEFAULT_PADDING);
    opt.brOptions.paddingEnd = std::make_optional(DEFAULT_PADDING);
    
    NavDestinationModelNG::SetTitlebarOptions(&(*frameNode), std::move(opt));

    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

    auto options = titleBarPattern->GetTitleBarOptions();
    EXPECT_TRUE(options.bgOptions.color.has_value());
    EXPECT_EQ(options.bgOptions.color.value(), Color(0xff00ff00));

    EXPECT_TRUE(options.bgOptions.blurStyle.has_value());
    EXPECT_EQ(options.bgOptions.blurStyle.value(), BlurStyle::REGULAR);

    EXPECT_TRUE(options.brOptions.barStyle.has_value());
    EXPECT_EQ(options.brOptions.barStyle.value(), BarStyle::STANDARD);

    EXPECT_TRUE(options.brOptions.paddingStart.has_value());
    EXPECT_EQ(options.brOptions.paddingStart.value(), DEFAULT_PADDING);

    EXPECT_TRUE(options.brOptions.paddingEnd.has_value());
    EXPECT_EQ(options.brOptions.paddingEnd.value(), DEFAULT_PADDING);
}

/**
 * @tc.name: SetToolbarConfiguration001
 * @tc.desc: Test SetToolbarConfiguration and cover all conditions of "GetPrevToolBarIsCustom.value_or".
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetToolbarConfiguration001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    navDestinationModelNG.SetTitle("navDestinationModelNG", false);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    // Make GetPrevToolBarIsCustom.value_or return false
    navDestinationGroupNode->propPrevToolBarIsCustom_ = false;
    EXPECT_FALSE(navDestinationGroupNode->GetPrevToolBarIsCustom().value_or(false));
    auto toolbarNode = AceType::DynamicCast<NavToolbarNode>(navDestinationGroupNode->GetPreToolBarNode());
    ASSERT_NE(toolbarNode, nullptr);
    EXPECT_EQ(toolbarNode->GetToolbarContainerNode(), nullptr);
    std::vector<NG::BarItem> toolBarItems;
    navDestinationModelNG.SetToolbarConfiguration(std::move(toolBarItems));

    // Make containerNode not NULL
    toolbarNode->toolbarContainerNode_ = FrameNode::CreateFrameNode("text", 101, AceType::MakeRefPtr<TextPattern>());
    EXPECT_NE(toolbarNode->GetToolbarContainerNode(), nullptr);
    navDestinationModelNG.SetToolbarConfiguration(std::move(toolBarItems));

    navDestinationGroupNode->propPrevToolBarIsCustom_ = true;
    EXPECT_TRUE(navDestinationGroupNode->GetPrevToolBarIsCustom().value_or(false));
    navDestinationModelNG.SetToolbarConfiguration(std::move(toolBarItems));
}

/**
 * @tc.name: SetToolbarConfiguration002
 * @tc.desc: Test SetToolbarConfiguration and cover all conditions of "LessThanAPIVersion".
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetToolbarConfiguration002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    navDestinationModelNG.SetTitle("navDestinationModelNG", false);

    std::vector<NG::BarItem> toolBarItems;
    auto context = PipelineBase::GetCurrentContext();
    ASSERT_NE(context, nullptr);
    int32_t preVersion = context->GetMinPlatformVersion();
    context->minPlatformVersion_ = static_cast<int32_t>(PlatformVersion::VERSION_ELEVEN) - 1;
    EXPECT_TRUE(Container::LessThanAPIVersion(PlatformVersion::VERSION_ELEVEN));
    navDestinationModelNG.SetToolbarConfiguration(std::move(toolBarItems));

    context->minPlatformVersion_ = static_cast<int32_t>(PlatformVersion::VERSION_ELEVEN) + 1;
    EXPECT_FALSE(Container::LessThanAPIVersion(PlatformVersion::VERSION_ELEVEN));
    navDestinationModelNG.SetToolbarConfiguration(std::move(toolBarItems));
    context->minPlatformVersion_ = preVersion;
    EXPECT_EQ(preVersion, context->GetMinPlatformVersion());
}

/**
 * @tc.name: SetToolbarConfiguration003
 * @tc.desc: Test SetToolbarConfiguration and cover all conditions of "LessThanAPIVersion".
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetToolbarConfiguration003, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    navDestinationModelNG.SetTitle("navDestinationModelNG", false);

    std::vector<NG::BarItem> toolBarItems;
    NG::BarItem newBar1;
    toolBarItems.push_back(newBar1);
    EXPECT_TRUE(toolBarItems.size() <= MAXIMUM_TOOLBAR_ITEMS_IN_BAR);
    navDestinationModelNG.SetToolbarConfiguration(std::move(toolBarItems));

    NG::BarItem newBars[MAXIMUM_TOOLBAR_ITEMS_IN_BAR];
    toolBarItems.insert(toolBarItems.end(), std::begin(newBars), std::end(newBars));
    EXPECT_TRUE(toolBarItems.size() > MAXIMUM_TOOLBAR_ITEMS_IN_BAR);
    navDestinationModelNG.SetToolbarConfiguration(std::move(toolBarItems));
}

/**
 * @tc.name: SetHideToolBar001
 * @tc.desc: Test Create.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetHideToolBar001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    NavDestinationModelNG.Create();
    NavDestinationModelNG.SetHideToolBar(true, false);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    ASSERT_NE(navDestinationLayoutProperty, nullptr);
    auto hideToolBarValue = navDestinationLayoutProperty->GetHideToolBarValue(false);
    ASSERT_EQ(hideToolBarValue, true);
}

/**
 * @tc.name: SetHideToolBar002
 * @tc.desc: Test Create.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetHideToolBar002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG NavDestinationModelNG;
    NavDestinationModelNG.Create();
    NavDestinationModelNG.SetHideToolBar(false, false);
    auto frameNode = AceType::Claim(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    auto navDestinationLayoutProperty = navDestinationGroupNode->GetLayoutPropertyPtr<NavDestinationLayoutProperty>();
    ASSERT_NE(navDestinationLayoutProperty, nullptr);
    auto hideToolBarValue = navDestinationLayoutProperty->GetHideToolBarValue(false);
    ASSERT_EQ(hideToolBarValue, false);
}

/**
 * @tc.name: SetSystemTransitionType001
 * @tc.desc: Test SetTitlebarOptions function with specific node.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, SetSystemTransitionType001, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationGroupNode, nullptr);
    navDestinationModelNG.SetSystemTransitionType(NavigationSystemTransitionType::DEFAULT);
    EXPECT_EQ(navDestinationGroupNode->GetSystemTransitionType(), NavigationSystemTransitionType::DEFAULT);

    navDestinationModelNG.SetSystemTransitionType(NavigationSystemTransitionType::CONTENT);
    EXPECT_EQ(navDestinationGroupNode->GetSystemTransitionType(), NavigationSystemTransitionType::CONTENT);
}

/**
 * @tc.name: GetSwipeContext001
 * @tc.desc: Test GetSwipeContext function of NavDestinationPattern.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, GetSwipeContext001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create NavDestinationGroupNode & NavDestinationPattern.
     * @tc.expected: Success to create node & pattern.
     */
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(node, nullptr);
    auto pattern = node->GetPattern<NavDestinationPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. call GetSwipeContext with parameter true.
     * @tc.expected: All fields of TitleContext has default value.
     */
    auto& titleContext = pattern->GetSwipeContext(true);
    auto* titleContextPtr = &titleContext;
    ASSERT_EQ(titleContextPtr, &pattern->titleBarSwipeContext_);
    EXPECT_FALSE(titleContext.isBarShowing);
    EXPECT_FALSE(titleContext.isBarHiding);
    EXPECT_FALSE(titleContext.showBarTask);

    /**
     * @tc.steps: step3. call GetSwipeContext with parameter false.
     * @tc.expected: All fields of ToolContext has default value.
     */
    auto& toolContext = pattern->GetSwipeContext(false);
    auto* toolContextPtr = &toolContext;
    ASSERT_EQ(toolContextPtr, &pattern->toolBarSwipeContext_);
    EXPECT_FALSE(toolContext.isBarShowing);
    EXPECT_FALSE(toolContext.isBarHiding);
    EXPECT_FALSE(toolContext.showBarTask);
}

/**
 * @tc.name: GetBarNode001
 * @tc.desc: Test GetBarNode function of NavDestinationPattern.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, GetBarNode001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create NavDestinationGroupNode & NavDestinationPattern.
     * @tc.expected: Success to create node & pattern.
     */
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(node, nullptr);
    auto nodeBase = AceType::WeakClaim(node).Upgrade();
    ASSERT_NE(nodeBase, nullptr);
    auto pattern = node->GetPattern<NavDestinationPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step2. Call GetBarNode with parameter nullptr.
     * @tc.expected: failed to get BarNode.
     */
    auto barNode = pattern->GetBarNode(nullptr, true);
    ASSERT_EQ(barNode, nullptr);
    barNode = pattern->GetBarNode(nullptr, false);
    ASSERT_EQ(barNode, nullptr);

    /**
     * @tc.steps: step3. Call GetBarNode with node.
     * @tc.expected: success to get titleBarNode.
     */
    barNode = pattern->GetBarNode(nodeBase, true);
    auto titleBarNode = AceType::DynamicCast<FrameNode>(node->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    ASSERT_EQ(barNode, titleBarNode);

    /**
     * @tc.steps: step4. Call GetBarNode with node.
     * @tc.expected: success to get toolBarNode.
     */
    barNode = pattern->GetBarNode(nodeBase, false);
    auto toolBarNode = AceType::DynamicCast<FrameNode>(node->GetToolBarNode());
    ASSERT_NE(toolBarNode, nullptr);
    ASSERT_EQ(barNode, toolBarNode);
}

/**
 * @tc.name: EnableTitleBarSwipe001
 * @tc.desc: Test EnableTitleBarSwipe function of NavDestinationPattern.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, EnableTitleBarSwipe001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create NavDestinationGroupNode & NavDestinationPattern.
     * @tc.expected: Success to create node & pattern.
     */
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(node, nullptr);
    auto nodeBase = AceType::WeakClaim(node).Upgrade();
    ASSERT_NE(nodeBase, nullptr);
    auto pattern = node->GetPattern<NavDestinationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto property = node->GetLayoutProperty<NavDestinationLayoutProperty>();
    ASSERT_NE(property, nullptr);

    /**
     * @tc.steps: step2. Call EnableTitleBarSwipe with parameter nullptr.
     * @tc.expected: disable titleBarSwipe.
     */
    bool enable = pattern->EnableTitleBarSwipe(nullptr);
    ASSERT_FALSE(enable);

    /**
     * @tc.steps: step3. Hide titlebar.
     * @tc.expected: disable titleBarSwipe.
     */
    property->UpdateHideTitleBar(true);
    enable = pattern->EnableTitleBarSwipe(nodeBase);
    ASSERT_FALSE(enable);

    /**
     * @tc.steps: step4. Show titlebar.
     * @tc.expected: enable titleBarSwipe.
     */
    property->UpdateHideTitleBar(false);
    enable = pattern->EnableTitleBarSwipe(nodeBase);
    ASSERT_TRUE(enable);
}

/**
 * @tc.name: EnableToolBarSwipe001
 * @tc.desc: Test EnableToolBarSwipe function of NavDestinationPattern.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, EnableToolBarSwipe001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create NavDestinationGroupNode & NavDestinationPattern.
     * @tc.expected: Success to create node & pattern.
     */
    MockPipelineContextGetTheme();
    NavDestinationModelNG navDestinationModelNG;
    navDestinationModelNG.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto node = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(node, nullptr);
    auto nodeBase = AceType::WeakClaim(node).Upgrade();
    ASSERT_NE(nodeBase, nullptr);
    auto pattern = node->GetPattern<NavDestinationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto property = node->GetLayoutProperty<NavDestinationLayoutProperty>();
    ASSERT_NE(property, nullptr);

    /**
     * @tc.steps: step2. Call EnableToolBarSwipe with parameter nullptr.
     * @tc.expected: disable toolBarSwipe.
     */
    bool enable = pattern->EnableToolBarSwipe(nullptr);
    ASSERT_FALSE(enable);

    /**
     * @tc.steps: step3. Hide toolbar.
     * @tc.expected: disable toolBarSwipe.
     */
    property->UpdateHideToolBar(true);
    enable = pattern->EnableToolBarSwipe(nodeBase);
    ASSERT_FALSE(enable);

    /**
     * @tc.steps: step4. Show toolbar.
     * @tc.expected: enable toolBarSwipe.
     */
    property->UpdateHideToolBar(false);
    enable = pattern->EnableToolBarSwipe(nodeBase);
    ASSERT_TRUE(enable);
}


/**
 * @tc.name: NavDestinationTest015
 * @tc.desc: Test LayoutBackButton Default padding.
 * @tc.type: FUNC
 */
HWTEST_F(NavdestinationTestNg, NavdestinationTest015, TestSize.Level1)
{
    AceApplicationInfo::GetInstance().SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    MockPipelineContextGetTheme();

    UIComponents ui;
    InitChildrenComponent(ui);
    ui.titleBarLayoutAlgorithm->LayoutBackButton(
        AccessibilityManager::RawPtr(ui.titleBarLayoutWrapper), ui.titleBarNode, ui.titleBarLayoutProperty);

    ASSERT_NE(ui.titleBarNode, nullptr);
    auto backButtonNode = AceType::DynamicCast<FrameNode>(ui.titleBarNode->GetBackButton());
    CHECK_NULL_VOID(backButtonNode);
    auto backButtonNodeLayoutProperty = backButtonNode->GetLayoutProperty();
    CHECK_NULL_VOID(backButtonNodeLayoutProperty);
    auto& padding = backButtonNodeLayoutProperty->GetPaddingProperty();
    CHECK_NULL_VOID(padding);
    EXPECT_TRUE(padding->left.has_value());
    EXPECT_TRUE(padding->right.has_value());
    EXPECT_TRUE(padding->top.has_value());
    EXPECT_TRUE(padding->bottom.has_value());

    EXPECT_EQ(padding->left.value().GetDimension(), DEFAULT_MENU_BUTTON_PADDING);
    EXPECT_EQ(padding->right.value().GetDimension(), DEFAULT_MENU_BUTTON_PADDING);
    EXPECT_EQ(padding->top.value().GetDimension(), DEFAULT_MENU_BUTTON_PADDING);
    EXPECT_EQ(padding->bottom.value().GetDimension(), DEFAULT_MENU_BUTTON_PADDING);
}
} // namespace OHOS::Ace::NG

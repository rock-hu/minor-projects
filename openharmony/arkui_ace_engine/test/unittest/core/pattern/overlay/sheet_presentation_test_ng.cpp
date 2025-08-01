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
#include <mutex>
#include <optional>
#include <string>

#include "gtest/gtest.h"

#define private public
#define protected public

#include "test/mock/base/mock_foldable_window.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/common/mock_window.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

#include "core/components_ng/pattern/overlay/sheet_drag_bar_pattern.h"
#include "core/components_ng/pattern/overlay/sheet_presentation_pattern.h"
#include "core/components_ng/pattern/overlay/sheet_style.h"
#include "core/components_ng/pattern/overlay/sheet_view.h"
#include "core/components_ng/pattern/root/root_pattern.h"
#include "core/components_ng/pattern/scroll/scroll_pattern.h"
#include "core/components_ng/pattern/stage/page_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const NG::BorderWidthProperty BORDER_WIDTH_TEST = { 1.0_vp, 1.0_vp, 1.0_vp, 0.0_vp };
const std::string MESSAGE = "hello world";
} // namespace

class SheetPresentationTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void SetSheetTheme(RefPtr<SheetTheme> sheetTheme);
    static void SetSheetType(RefPtr<SheetPresentationPattern> sheetPattern, SheetType sheetType);
    static void TearDownTestCase();
    static void SetApiVersion(int32_t apiTargetVersion);
};

void SheetPresentationTestNg::SetUpTestCase()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly([](ThemeType type) -> RefPtr<Theme> {
        if (type == SheetTheme::TypeId()) {
            auto  sheetTheme = AceType::MakeRefPtr<SheetTheme>();
            sheetTheme->closeIconButtonWidth_ = SHEET_CLOSE_ICON_WIDTH;
            sheetTheme->centerDefaultWidth_ = SHEET_LANDSCAPE_WIDTH;
            sheetTheme->sheetCloseIconTitleSpaceNew_ = SHEET_CLOSE_ICON_TITLE_SPACE_NEW;
            return sheetTheme;
        } else {
            return nullptr;
        }
    });
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
}

void SheetPresentationTestNg::SetSheetTheme(RefPtr<SheetTheme> sheetTheme)
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(
        [sheetTheme = AceType::WeakClaim(AceType::RawPtr(sheetTheme))](ThemeType type) -> RefPtr<Theme> {
        if (type == SheetTheme::TypeId()) {
            return sheetTheme.Upgrade();
        } else {
            return nullptr;
        }
    });
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
}

void SheetPresentationTestNg::SetSheetType(RefPtr<SheetPresentationPattern> sheetPattern, SheetType sheetType)
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    pipelineContext->displayWindowRectInfo_.width_ = SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    sheetStyle.sheetType = sheetType;
    layoutProperty->propSheetStyle_ = sheetStyle;
    sheetPattern->sheetThemeType_ = "popup";
    Rect windowRect = { 0.0f, 0.0f, SHEET_PC_DEVICE_WIDTH_BREAKPOINT.ConvertToPx(), 0.0f };
    MockPipelineContext::SetCurrentWindowRect(windowRect);
    sheetPattern->sheetKey_.hasValidTargetNode = true;
    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->sheetType_ = "popup";
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
}

void SheetPresentationTestNg::SetApiVersion(int32_t apiTargetVersion)
{
    auto container = Container::Current();
    ASSERT_NE(container, nullptr);
    container->SetApiTargetVersion(apiTargetVersion);
}

void SheetPresentationTestNg::TearDownTestCase()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
}

/**
 * @tc.name: OnScrollStartRecursive001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::OnScrollStartRecursive function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, OnScrollStartRecursive001, TestSize.Level1)
{
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    sheetPattern->animation_ = nullptr;
    EXPECT_EQ(sheetPattern->animation_, nullptr);
    sheetPattern->InitSheetObject();
    auto sheetObject = sheetPattern->GetSheetObject();
    ASSERT_NE(sheetObject, nullptr);
    sheetObject->OnScrollStartRecursive(0.0f, 0.0f);

    AnimationOption option;
    sheetPattern->animation_ = AnimationUtils::StartAnimation(option, []() {}, []() {});
    sheetPattern->isAnimationProcess_ = false;
    EXPECT_NE(sheetPattern->animation_, nullptr);
    EXPECT_FALSE(sheetPattern->isAnimationProcess_);
    sheetObject->OnScrollStartRecursive(0.0f, 0.0f);

    sheetPattern->isAnimationProcess_ = true;
    EXPECT_TRUE(sheetPattern->isAnimationProcess_);
    sheetObject->OnScrollStartRecursive(0.0f, 0.0f);
}

/**
 * @tc.name: HandleScrollWithSheet001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::HandleScrollWithSheet function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, HandleScrollWithSheet001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->InitSheetObject();
    auto object = sheetPattern->sheetObject_;
    ASSERT_NE(object, nullptr);
    float scrollOffset = 0.0f;
    PipelineBase::GetCurrentContext()->minPlatformVersion_ = static_cast<int32_t>(PlatformVersion::VERSION_TEN);
    sheetPattern->sheetDetentHeight_.emplace_back(1);
    sheetPattern->currentOffset_ = 1.0f;
    sheetPattern->height_ = 0.0f;
    sheetPattern->sheetHeightUp_ = 0.0f;
    sheetPattern->sheetMaxHeight_ = 0.0f;
    sheetPattern->scrollSizeMode_= ScrollSizeMode::FOLLOW_DETENT;
    EXPECT_FALSE(NearZero(sheetPattern->currentOffset_));
    object->HandleScrollWithSheet(scrollOffset);

    scrollOffset = 0.0f;
    sheetPattern->currentOffset_ = 0.0f;
    EXPECT_TRUE(NearZero(sheetPattern->currentOffset_));
    sheetPattern->sheetObject_->HandleScrollWithSheet(scrollOffset);

    scrollOffset = -1.0f;
    sheetPattern->scrollSizeMode_= ScrollSizeMode::CONTINUOUS;
    object->HandleScrollWithSheet(scrollOffset);
    sheetPattern->sheetDetentHeight_.emplace_back(0);
    object->HandleScrollWithSheet(scrollOffset);

    scrollOffset = -20.0f;
    sheetPattern->height_ = 100.0f;
    sheetPattern->pageHeight_ = 100.0f;
    sheetPattern->sheetOffsetY_ = 100.0f;
    sheetPattern->sheetMaxHeight_ = 50.0f;
    sheetPattern->currentOffset_ = 1.0f;
    object->HandleScrollWithSheet(scrollOffset);
    EXPECT_TRUE(object->isSheetPosChanged_);
    EXPECT_FALSE(NearZero(sheetPattern->currentOffset_));
    sheetPattern->sheetDetentHeight_.clear();
    EXPECT_EQ(sheetPattern->sheetDetentHeight_.size(), 0);
    auto ret = object->HandleScrollWithSheet(scrollOffset);
    EXPECT_FLOAT_EQ(ret.remain, scrollOffset);
    EXPECT_FALSE(object->isSheetNeedScroll_);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: HandleScrollWithSheet002
 * @tc.desc: Increase the coverage of SheetPresentationPattern::HandleScrollWithSheet function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, HandleScrollWithSheet002, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->sheetObject_, nullptr);
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_CENTER);
    sheetPattern->scrollSizeMode_= ScrollSizeMode::CONTINUOUS;
    sheetPattern->sheetDetentHeight_.emplace_back(1800.0f);
    sheetPattern->height_ = 2000.0f;
    sheetPattern->sheetMaxHeight_ = 1800.0f;
    float scrollOffset = 0.0f;
    sheetPattern->sheetObject_->HandleScrollWithSheet(scrollOffset);
    EXPECT_FALSE(LessNotEqual(scrollOffset, 0.0f));
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: OnDirtyLayoutWrapperSwap001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::OnDirtyLayoutWrapperSwap function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, OnDirtyLayoutWrapperSwap001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->UpdateSheetType();
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    layoutProperty->propSheetStyle_ = SheetStyle();
    auto dirty = sheetNode->CreateLayoutWrapper();
    DirtySwapConfig config;
    config.skipMeasure = false;
    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    ASSERT_NE(layoutAlgorithmWrapper, nullptr);
    auto sheetLayoutAlgorithm =
        AceType::DynamicCast<SheetPresentationLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    ASSERT_NE(sheetLayoutAlgorithm, nullptr);
    sheetLayoutAlgorithm->sheetMaxHeight_ = 1.0f;
    sheetPattern->sheetOffsetX_ = sheetLayoutAlgorithm->sheetOffsetX_;
    sheetPattern->sheetOffsetY_ = sheetLayoutAlgorithm->sheetOffsetY_;
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_POPUP);
    EXPECT_FALSE(config.skipMeasure);
    EXPECT_TRUE(sheetLayoutAlgorithm->GetSheetMaxHeight() > 0);
    EXPECT_TRUE(NearEqual(sheetPattern->sheetOffsetX_, sheetLayoutAlgorithm->GetSheetOffsetX()));
    EXPECT_TRUE(NearEqual(sheetPattern->sheetOffsetY_, sheetLayoutAlgorithm->GetSheetOffsetY()));
    EXPECT_EQ(sheetPattern->GetSheetType(), SheetType::SHEET_POPUP);
    sheetPattern->OnDirtyLayoutWrapperSwap(dirty, config);

    config.skipMeasure = true;
    config.skipLayout = false;
    sheetPattern->sheetOffsetY_ = sheetLayoutAlgorithm->sheetOffsetY_ + 1.0f;
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_BOTTOM);
    sheetPattern->windowRotate_ = false;
    AceApplicationInfo::GetInstance().packageName_ = "com.hua";
    AceApplicationInfo::GetInstance().packageName_.append("wei.hms.hua");
    AceApplicationInfo::GetInstance().packageName_.append("weiid");
    EXPECT_TRUE(config.skipMeasure && !config.skipLayout);
    EXPECT_TRUE(!NearEqual(sheetPattern->sheetOffsetY_, sheetLayoutAlgorithm->GetSheetOffsetY()));
    EXPECT_EQ(sheetPattern->GetSheetType(), SheetType::SHEET_BOTTOM);
    sheetPattern->OnDirtyLayoutWrapperSwap(dirty, config);

    sheetPattern->sheetOffsetY_ = sheetLayoutAlgorithm->sheetOffsetY_ + 1.0f;
    sheetPattern->sheetOffsetX_ = sheetLayoutAlgorithm->sheetOffsetX_ + 1.0f;
    EXPECT_TRUE(!NearEqual(sheetPattern->sheetOffsetX_, sheetLayoutAlgorithm->GetSheetOffsetX()));
    sheetPattern->OnDirtyLayoutWrapperSwap(dirty, config);

    config.skipLayout = true;
    EXPECT_TRUE(config.skipMeasure && config.skipLayout);
    sheetPattern->OnDirtyLayoutWrapperSwap(dirty, config);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: CheckBuilderChange001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::CheckBuilderChange function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, CheckBuilderChange001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto rootNode = FrameNode::CreateFrameNode("Root", 301, AceType::MakeRefPtr<RootPattern>());
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    sheetNode->MountToParent(rootNode);
    auto dragBarNode = FrameNode::CreateFrameNode("SheetDragBar", 401, AceType::MakeRefPtr<SheetDragBarPattern>());
    dragBarNode->MountToParent(sheetNode);
    auto scrollNode = FrameNode::CreateFrameNode("Scroll", 501, AceType::MakeRefPtr<ScrollPattern>());
    auto contentNode = FrameNode::GetOrCreateFrameNode("SheetContent", 601,
        []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
    contentNode->MountToParent(scrollNode);
    scrollNode->MountToParent(sheetNode);
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    sheetPattern->SetScrollNode(WeakPtr<FrameNode>(scrollNode));
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    layoutProperty->propSheetStyle_ = sheetStyle;
    auto eventHub = contentNode->GetOrCreateEventHub<EventHub>();
    RectF oldRect, rect;
    OffsetF oldOrigin, origin;
    sheetPattern->CheckBuilderChange();
    EXPECT_NE(sheetStyle.sheetHeight.sheetMode, SheetMode::AUTO);
    auto innerCallbackInfo = eventHub->onAreaChangedInnerCallbacks_[contentNode->GetId()];
    ASSERT_NE(innerCallbackInfo, nullptr);
    innerCallbackInfo(oldRect, oldOrigin, rect, origin);

    sheetStyle.sheetHeight.sheetMode = SheetMode::AUTO;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_EQ(sheetStyle.sheetHeight.sheetMode, SheetMode::AUTO);
    innerCallbackInfo(oldRect, oldOrigin, rect, origin);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: OnAttachToFrameNode001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::OnAttachToFrameNode function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, OnAttachToFrameNode001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    sheetPattern->targetTag_ = "Sheet";
    sheetPattern->targetId_ = 101;
    auto targetNode = FrameNode::GetFrameNode(sheetPattern->targetTag_, sheetPattern->targetId_);
    ASSERT_NE(targetNode, nullptr);
    sheetPattern->OnAttachToFrameNode();
    auto eventHub = targetNode->GetOrCreateEventHub<EventHub>();
    ASSERT_NE(eventHub, nullptr);
    auto innerOnAreaChangeCallback = eventHub->onAreaChangedInnerCallbacks_[sheetNode->GetId()];
    ASSERT_NE(innerOnAreaChangeCallback, nullptr);
    RectF oldRect, rect;
    OffsetF oldOrigin, origin;
    EXPECT_NE(sheetPattern->GetSheetType(), SheetType::SHEET_POPUP);
    innerOnAreaChangeCallback(oldRect, oldOrigin, rect, origin);

    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_POPUP);
    sheetPattern->OnAttachToFrameNode();
    EXPECT_EQ(sheetPattern->GetSheetType(), SheetType::SHEET_POPUP);
    EXPECT_EQ(sheetPattern->sheetThemeType_, "popup");
    innerOnAreaChangeCallback(oldRect, oldOrigin, rect, origin);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: InitPanEvent001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::InitPanEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, InitPanEvent001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->UpdateSheetType();
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->InitPanEvent();
    GestureEvent event;
    auto panEvent = sheetPattern->panEvent_;
    ASSERT_NE(panEvent, nullptr);
    panEvent->actionStart_(event);
    panEvent->actionUpdate_(event);
    panEvent->actionEnd_(event);
    panEvent->actionCancel_();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: HandleDragStart001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::HandleDragStart function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, HandleDragStart001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->UpdateSheetType();
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->animation_ = nullptr;
    EXPECT_EQ(sheetPattern->animation_, nullptr);
    sheetPattern->HandleDragStart();

    sheetPattern->animation_ = AnimationUtils::StartAnimation(AnimationOption(), []() {}, []() {});
    sheetPattern->isAnimationProcess_ = false;
    EXPECT_NE(sheetPattern->animation_, nullptr);
    EXPECT_FALSE(sheetPattern->isAnimationProcess_);
    sheetPattern->HandleDragStart();

    sheetPattern->isAnimationProcess_ = true;
    EXPECT_NE(sheetPattern->animation_, nullptr);
    EXPECT_TRUE(sheetPattern->isAnimationProcess_);
    sheetPattern->HandleDragStart();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: OnCoordScrollStart001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::OnCoordScrollStart function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, OnCoordScrollStart001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    sheetPattern->animation_ = nullptr;
    EXPECT_EQ(sheetPattern->animation_, nullptr);
    sheetPattern->OnCoordScrollStart();

    sheetPattern->animation_ = AnimationUtils::StartAnimation(AnimationOption(), []() {}, []() {});
    sheetPattern->isAnimationProcess_ = false;
    EXPECT_NE(sheetPattern->animation_, nullptr);
    EXPECT_FALSE(sheetPattern->isAnimationProcess_);
    sheetPattern->OnCoordScrollStart();

    sheetPattern->isAnimationProcess_ = true;
    EXPECT_NE(sheetPattern->animation_, nullptr);
    EXPECT_TRUE(sheetPattern->isAnimationProcess_);
    sheetPattern->OnCoordScrollStart();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetSheetType001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::GetSheetType function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetSheetType001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    int32_t minPlatformVersion = PipelineBase::GetCurrentContext()->minPlatformVersion_;
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    layoutProperty->propSheetStyle_ = sheetStyle;
    PipelineBase::GetCurrentContext()->minPlatformVersion_ = static_cast<int32_t>(PlatformVersion::VERSION_TWELVE);
    auto pipelineContext = PipelineContext::GetCurrentContext();
    pipelineContext->displayWindowRectInfo_.width_ = SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx();
    sheetPattern->sheetThemeType_ = "center";
    EXPECT_TRUE(pipelineContext->GetDisplayWindowRectInfo().Width() >= SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx());
    EXPECT_FALSE(sheetStyle.sheetType.has_value());
    EXPECT_NE(sheetPattern->sheetThemeType_, "auto");
    EXPECT_NE(sheetPattern->sheetThemeType_, "popup");
    sheetPattern->GetSheetType();

    sheetStyle.sheetType = SheetType::SHEET_CENTER;
    layoutProperty->propSheetStyle_ = sheetStyle;
    sheetPattern->sheetThemeType_ = "popup";
    EXPECT_TRUE(sheetStyle.sheetType.has_value());
    EXPECT_NE(sheetStyle.sheetType.value(), SheetType::SHEET_BOTTOM);
    EXPECT_EQ(sheetPattern->sheetThemeType_, "popup");
    sheetPattern->GetSheetType();

    sheetPattern->sheetThemeType_ = "auto";
    EXPECT_EQ(sheetPattern->sheetThemeType_, "auto");
    sheetPattern->GetSheetType();

    sheetStyle.sheetType = SheetType::SHEET_BOTTOM;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_EQ(sheetStyle.sheetType.value(), SheetType::SHEET_BOTTOM);
    sheetPattern->GetSheetType();

    sheetStyle.sheetType = SheetType::SHEET_BOTTOM;
    layoutProperty->propSheetStyle_ = sheetStyle;
    pipelineContext->displayWindowRectInfo_.width_ = SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx() - 5.0f;
    EXPECT_FALSE(pipelineContext->GetDisplayWindowRectInfo().Width() >= SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx());
    sheetPattern->GetSheetType();
    PipelineBase::GetCurrentContext()->minPlatformVersion_ = minPlatformVersion;
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetSheetTypeWithAuto001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::GetSheetTypeWithAuto function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetSheetTypeWithAuto001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();

    /**
     * @tc.steps: step1. set API14.
     */
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_FOURTEEN));

    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    layoutProperty->propSheetStyle_ = sheetStyle;

    RefPtr<DisplayInfo> displayInfo = AceType::MakeRefPtr<DisplayInfo>();
    displayInfo->SetFoldStatus(FoldStatus::FOLDED);
    MockContainer::Current()->SetDisplayInfo(displayInfo);
    MockPipelineContext::GetCurrent()->rootHeight_ = 6.0f;
    MockPipelineContext::GetCurrent()->rootWidth_ = 5.0f;
    EXPECT_FALSE(sheetPattern->IsFoldExpand());
    EXPECT_FALSE(LessNotEqual(PipelineContext::GetCurrentRootHeight(), PipelineContext::GetCurrentRootWidth()));
    SheetType sheetType;
    sheetPattern->GetSheetTypeWithAuto(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_BOTTOM);

    displayInfo->SetFoldStatus(FoldStatus::EXPAND);
    MockContainer::Current()->SetDisplayInfo(displayInfo);

    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->sheetBottom_ = "bottom";
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
    MockPipelineContext::GetCurrent()->rootHeight_ = 4.0f;
    EXPECT_TRUE(sheetPattern->IsFoldExpand());
    EXPECT_TRUE(sheetTheme->IsOnlyBottom());
    EXPECT_TRUE(LessNotEqual(PipelineContext::GetCurrentRootHeight(), PipelineContext::GetCurrentRootWidth()));
    sheetPattern->GetSheetTypeWithAuto(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_BOTTOMLANDSPACE);

    sheetTheme->sheetBottom_ = "undefined";
    EXPECT_FALSE(sheetTheme->IsOnlyBottom());
    sheetPattern->GetSheetTypeWithAuto(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_CENTER);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetSheetTypeWithAuto002
 * @tc.desc: Increase the coverage of SheetPresentationPattern::GetSheetTypeWithAuto function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetSheetTypeWithAuto002, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();

    /**
     * @tc.steps: step1. set API14.
     */
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_FOURTEEN));

    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    layoutProperty->propSheetStyle_ = sheetStyle;

    RefPtr<DisplayInfo> displayInfo = AceType::MakeRefPtr<DisplayInfo>();
    displayInfo->SetFoldStatus(FoldStatus::EXPAND);
    MockContainer::Current()->SetDisplayInfo(displayInfo);
    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->sheetBottom_ = "undefined";
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
    AceApplicationInfo::GetInstance().packageName_ = "com.ohos.useriam.authwidget";
    EXPECT_TRUE(sheetPattern->IsFoldExpand());
    EXPECT_FALSE(sheetTheme->IsOnlyBottom());
    EXPECT_FALSE(sheetStyle.sheetType.has_value());
    SheetType sheetType;
    sheetPattern->GetSheetTypeWithAuto(sheetType);

    sheetStyle.sheetType = SheetType::SHEET_BOTTOM;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_TRUE(sheetStyle.sheetType.has_value());
    EXPECT_NE(sheetStyle.sheetType.value(), SheetType::SHEET_CENTER);
    sheetPattern->GetSheetTypeWithAuto(sheetType);

    sheetStyle.sheetType = SheetType::SHEET_CENTER;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_TRUE(sheetStyle.sheetType.has_value());
    EXPECT_EQ(sheetStyle.sheetType.value(), SheetType::SHEET_CENTER);
    sheetPattern->GetSheetTypeWithAuto(sheetType);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: IsFoldExpand001
 * @tc.desc: Branch: if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_FIFTEEN))
 *           Condition: SetFoldStatus(FoldStatus::FOLDED)
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, IsFoldExpand001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();

    /**
     * @tc.steps: step1. set container FoldStatus FOLDED.
     */
    RefPtr<DisplayInfo> displayInfo = AceType::MakeRefPtr<DisplayInfo>();
    displayInfo->SetFoldStatus(FoldStatus::FOLDED);
    MockContainer::Current()->SetDisplayInfo(displayInfo);

    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);

    /**
     * @tc.steps: step2. excute IsFoldExpand func.
     * @tc.expected: false
     */
    EXPECT_FALSE(sheetPattern->IsFoldExpand());
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetSheetTypeWithPopup001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::GetSheetTypeWithPopup function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetSheetTypeWithPopup001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    layoutProperty->propSheetStyle_ = sheetStyle;
    auto pipelineContext = PipelineContext::GetCurrentContext();
    MockPipelineContext::GetCurrent()->rootWidth_ = 100.0f;
    EXPECT_FALSE(GreatOrEqual(PipelineContext::GetCurrentRootWidth(), SHEET_PC_DEVICE_WIDTH_BREAKPOINT.ConvertToPx()));
    EXPECT_FALSE(GreatOrEqual(PipelineContext::GetCurrentRootWidth(), SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx()));
    SheetType sheetType;
    sheetPattern->GetSheetTypeWithPopup(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_BOTTOM_FREE_WINDOW);

    MockPipelineContext::GetCurrent()->rootWidth_ = SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx();
    AceApplicationInfo::GetInstance().packageName_ = "";
    EXPECT_TRUE(GreatOrEqual(PipelineContext::GetCurrentRootWidth(), SHEET_DEVICE_WIDTH_BREAKPOINT.ConvertToPx()));
    EXPECT_TRUE(LessNotEqual(PipelineContext::GetCurrentRootWidth(), SHEET_PC_DEVICE_WIDTH_BREAKPOINT.ConvertToPx()));
    EXPECT_FALSE(sheetStyle.sheetType.has_value());
    sheetPattern->GetSheetTypeWithPopup(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_CENTER);

    AceApplicationInfo::GetInstance().packageName_ = "com.ohos.useriam.authwidget";
    sheetPattern->GetSheetTypeWithPopup(sheetType);

    sheetStyle.sheetType = SheetType::SHEET_POPUP;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_TRUE(sheetStyle.sheetType.has_value());
    EXPECT_TRUE(sheetPattern->sheetKey_.hasValidTargetNode);
    sheetPattern->GetSheetTypeWithPopup(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_POPUP);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetSheetTypeWithPopup002
 * @tc.desc: Increase the coverage of SheetPresentationPattern::GetSheetTypeWithPopup function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetSheetTypeWithPopup002, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 101,
        AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    layoutProperty->propSheetStyle_ = sheetStyle;
    auto pipelineContext = PipelineContext::GetCurrentContext();
    MockPipelineContext::GetCurrent()->rootWidth_ = SHEET_PC_DEVICE_WIDTH_BREAKPOINT.ConvertToPx();
    AceApplicationInfo::GetInstance().packageName_ = "";
    EXPECT_TRUE(GreatOrEqual(PipelineContext::GetCurrentRootWidth(), SHEET_PC_DEVICE_WIDTH_BREAKPOINT.ConvertToPx()));
    EXPECT_FALSE(sheetStyle.sheetType.has_value());
    SheetType sheetType;
    sheetPattern->GetSheetTypeWithPopup(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_POPUP);

    AceApplicationInfo::GetInstance().packageName_ = "com.ohos.useriam.authwidget";
    sheetPattern->GetSheetTypeWithPopup(sheetType);

    sheetStyle.sheetType = SheetType::SHEET_POPUP;
    layoutProperty->propSheetStyle_ = sheetStyle;
    sheetPattern->sheetKey_.hasValidTargetNode = false;
    EXPECT_TRUE(sheetStyle.sheetType.has_value());
    EXPECT_FALSE(sheetPattern->sheetKey_.hasValidTargetNode);
    sheetPattern->GetSheetTypeWithPopup(sheetType);
    EXPECT_EQ(sheetType, SheetType::SHEET_CENTER);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: IsTypeNeedAvoidAiBar001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::IsTypeNeedAvoidAiBar function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, IsTypeNeedAvoidAiBar001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    SheetStyle sheetStyle;
    sheetStyle.showInPage = false;
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    layoutProperty->propSheetStyle_ = sheetStyle;
    auto pipelineContext = PipelineContext::GetCurrentContext();
    auto overlayManager = pipelineContext->overlayManager_;
    pipelineContext->overlayManager_ = nullptr;
    sheetPattern->sheetType_ = SheetType::SHEET_BOTTOM;
    EXPECT_EQ(sheetPattern->GetOverlayManager(), nullptr);
    bool ret = sheetPattern->IsTypeNeedAvoidAiBar();
    EXPECT_TRUE(ret);

    layoutProperty->propSheetStyle_ = sheetStyle;
    pipelineContext->overlayManager_ = overlayManager;
    ASSERT_NE(sheetPattern->GetOverlayManager(), nullptr);
    EXPECT_FALSE(sheetPattern->GetOverlayManager()->IsRootExpansive());
    ret = sheetPattern->IsTypeNeedAvoidAiBar();
    EXPECT_TRUE(ret);

    auto rootLayoutProperty = AceType::DynamicCast<FrameNode>(pipelineContext->rootNode_)->GetLayoutProperty();
    rootLayoutProperty->safeAreaExpandOpts_ = std::make_unique<SafeAreaExpandOpts>();
    rootLayoutProperty->safeAreaExpandOpts_->type = SAFE_AREA_TYPE_SYSTEM;
    rootLayoutProperty->safeAreaExpandOpts_->edges = SAFE_AREA_EDGE_TOP;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_TRUE(sheetPattern->GetOverlayManager()->IsRootExpansive());
    EXPECT_FALSE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    ret = sheetPattern->IsTypeNeedAvoidAiBar();
    EXPECT_TRUE(ret);

    sheetPattern->overlayManager_ = AceType::WeakClaim(AceType::RawPtr(overlayManager));
    auto rootNode = FrameNode::CreateFrameNode("Root", 101, AceType::MakeRefPtr<RootPattern>());
    auto stageNode = FrameNode::CreateFrameNode("Stage", 201, AceType::MakeRefPtr<StagePattern>());
    stageNode->MountToParent(rootNode);
    sheetNode->MountToParent(stageNode);
    sheetStyle.showInPage = true;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_TRUE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    ret = sheetPattern->IsTypeNeedAvoidAiBar();
    EXPECT_FALSE(ret);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: OnScrollEndRecursive001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::OnScrollEndRecursive function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, OnScrollEndRecursive001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->UpdateSheetType();
    sheetPattern->InitSheetObject();
    auto sheetObject = sheetPattern->GetSheetObject();
    ASSERT_NE(sheetObject, nullptr);
    sheetObject->isSheetPosChanged_ = false;
    sheetPattern->OnScrollEndRecursive(std::nullopt);
    EXPECT_FALSE(sheetObject->isSheetPosChanged_);

    sheetObject->isSheetPosChanged_ = true;
    sheetPattern->OnScrollEndRecursive(std::nullopt);
    EXPECT_FALSE(sheetObject->isSheetPosChanged_);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: HandleScrollVelocity001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::HandleScrollVelocity function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, HandleScrollVelocity001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->UpdateSheetType();
    sheetPattern->InitSheetObject();
    auto sheetObject = sheetPattern->GetSheetObject();
    ASSERT_NE(sheetObject, nullptr);
    sheetObject->isSheetPosChanged_ = false;
    sheetObject->isSheetNeedScroll_ = true;
    bool ret = sheetPattern->HandleScrollVelocity(1.0f, sheetPattern);
    EXPECT_FALSE(sheetObject->isSheetPosChanged_);
    EXPECT_TRUE(ret);

    sheetObject->isSheetPosChanged_ = true;
    sheetObject->isSheetNeedScroll_ = false;
    ret = sheetPattern->HandleScrollVelocity(1.0f, sheetPattern);
    EXPECT_FALSE(sheetObject->isSheetPosChanged_);
    EXPECT_FALSE(ret);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: FireOnTypeDidChange001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::FireOnTypeDidChange function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, FireOnTypeDidChange001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    sheetPattern->sheetType_ = SheetType::SHEET_CENTER;
    sheetPattern->preType_ = SheetType::SHEET_CENTER;
    EXPECT_EQ(sheetPattern->sheetType_, SheetType::SHEET_CENTER);
    EXPECT_EQ(sheetPattern->preType_, sheetPattern->sheetType_);
    sheetPattern->FireOnTypeDidChange();

    sheetPattern->sheetType_ = SheetType::SHEET_BOTTOM_FREE_WINDOW;
    EXPECT_EQ(sheetPattern->sheetType_, SheetType::SHEET_BOTTOM_FREE_WINDOW);
    EXPECT_NE(sheetPattern->preType_, sheetPattern->sheetType_);
    sheetPattern->FireOnTypeDidChange();

    sheetPattern->sheetType_ = SheetType::SHEET_BOTTOMLANDSPACE;
    EXPECT_EQ(sheetPattern->sheetType_, SheetType::SHEET_BOTTOMLANDSPACE);
    sheetPattern->FireOnTypeDidChange();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetOverlayRoot001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::GetOverlayRoot function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetOverlayRoot001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    SheetStyle sheetStyle;
    sheetStyle.showInPage = true;
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_TRUE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    sheetPattern->GetOverlayRoot();

    sheetStyle.showInPage = false;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_FALSE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    sheetPattern->GetOverlayRoot();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetOverlayManager001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::GetOverlayManager function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetOverlayManager001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    SheetStyle sheetStyle;
    sheetStyle.showInPage = true;
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    layoutProperty->propSheetStyle_ = sheetStyle;
    auto rootNode = FrameNode::CreateFrameNode("Root", 101, AceType::MakeRefPtr<RootPattern>());
    auto stageNode = FrameNode::CreateFrameNode("Stage", 201, AceType::MakeRefPtr<StagePattern>());
    stageNode->MountToParent(rootNode);
    sheetNode->MountToParent(stageNode);
    EXPECT_TRUE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    EXPECT_NE(rootNode->GetTag(), V2::PAGE_ETS_TAG);
    EXPECT_NE(rootNode->GetTag(), V2::NAVDESTINATION_VIEW_ETS_TAG);
    sheetPattern->GetOverlayManager();

    rootNode->tag_ = V2::NAVDESTINATION_VIEW_ETS_TAG;
    EXPECT_TRUE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    EXPECT_EQ(rootNode->GetTag(), V2::NAVDESTINATION_VIEW_ETS_TAG);
    sheetPattern->GetOverlayManager();

    rootNode->tag_ = V2::PAGE_ETS_TAG;
    rootNode->pattern_ = AceType::MakeRefPtr<PagePattern>(AceType::MakeRefPtr<PageInfo>());
    rootNode->GetPattern<PagePattern>()->overlayManager_ = PipelineContext::GetCurrentContext()->overlayManager_;
    EXPECT_EQ(rootNode->GetTag(), V2::PAGE_ETS_TAG);
    EXPECT_NE(rootNode->GetPattern<PagePattern>(), nullptr);
    EXPECT_NE(rootNode->GetPattern<PagePattern>()->GetOverlayManager(), nullptr);
    sheetPattern->GetOverlayManager();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: OnWindowSizeChanged001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::OnWindowSizeChanged function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, OnWindowSizeChanged001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto pipelineContext = PipelineContext::GetCurrentContext();
    auto windowManager = pipelineContext->windowManager_;
    pipelineContext->windowManager_ = nullptr;
    sheetPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::MOVE);

    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_POPUP);
    EXPECT_EQ(sheetPattern->GetSheetType(), SheetType::SHEET_BOTTOM_FREE_WINDOW);
    pipelineContext->windowManager_ = windowManager;
    pipelineContext->windowManager_->windowGetModeCallback_ = []() { return WindowMode::WINDOW_MODE_UNDEFINED; };
    sheetPattern->OnWindowSizeChanged(0, 0,  WindowSizeChangeReason::ROTATION);

    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_BOTTOMLANDSPACE);
    EXPECT_EQ(sheetPattern->GetSheetType(), SheetType::SHEET_BOTTOM_FREE_WINDOW);
    sheetPattern->isScrolling_ = false;
    pipelineContext->windowManager_->windowGetModeCallback_ = []() { return WindowMode::WINDOW_MODE_FLOATING; };
    sheetPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::ROTATION);

    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_BOTTOM);
    sheetPattern->isScrolling_ = true;
    EXPECT_EQ(sheetPattern->GetSheetType(), SheetType::SHEET_BOTTOM);
    sheetPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::ROTATION);
    sheetPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::UNDEFINED);
    sheetPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::DRAG);
    sheetPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::RESIZE);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: HandleFitContontChange001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::HandleFitContontChange function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, HandleFitContontChange001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->UpdateSheetType();
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->height_ = sheetPattern->sheetFitContentHeight_ + 1.0f;
    float height = sheetPattern->sheetFitContentHeight_;
    EXPECT_FALSE(NearEqual(sheetPattern->height_, sheetPattern->sheetFitContentHeight_));
    sheetPattern->HandleFitContontChange(height);

    sheetPattern->height_ = sheetPattern->sheetFitContentHeight_;
    EXPECT_TRUE(NearEqual(sheetPattern->height_, sheetPattern->sheetFitContentHeight_));
    EXPECT_TRUE(NearEqual(height, sheetPattern->sheetFitContentHeight_));
    sheetPattern->HandleFitContontChange(height);

    height = sheetPattern->sheetFitContentHeight_ + 1.0f;
    EXPECT_FALSE(NearEqual(height, sheetPattern->sheetFitContentHeight_));
    sheetPattern->HandleFitContontChange(height);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: DismissTransition001
 * @tc.desc: Increase the coverage of SheetPresentationPattern::DismissTransition function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, DismissTransition001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto rootNode = FrameNode::CreateFrameNode("Root", 101,
        AceType::MakeRefPtr<PagePattern>(AceType::MakeRefPtr<PageInfo>()));
    auto stageNode = FrameNode::CreateFrameNode("Stage", 201, AceType::MakeRefPtr<StagePattern>());
    stageNode->MountToParent(rootNode);
    sheetNode->MountToParent(stageNode);
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    ASSERT_NE(sheetPattern, nullptr);
    sheetPattern->UpdateSheetType();
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    auto pipelineContext = PipelineContext::GetCurrentContext();
    sheetPattern->overlayManager_ = AceType::WeakClaim(AceType::RawPtr(pipelineContext->overlayManager_));
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_BOTTOM);
    EXPECT_NE(sheetPattern->GetSheetType(), SheetType::SHEET_POPUP);
    sheetPattern->DismissTransition(true, 1);

    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    layoutProperty->propSheetStyle_ = sheetStyle;
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_POPUP);
    EXPECT_FALSE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    EXPECT_EQ(sheetPattern->GetSheetType(), SheetType::SHEET_POPUP);
    sheetPattern->DismissTransition(false, 1);

    sheetStyle.showInPage = true;
    layoutProperty->propSheetStyle_ = sheetStyle;
    EXPECT_TRUE(layoutProperty->GetSheetStyleValue(SheetStyle()).showInPage.value_or(false));
    EXPECT_NE(rootNode->GetTag(), V2::PAGE_ETS_TAG);
    sheetPattern->DismissTransition(false, 1);

    rootNode->tag_ = V2::PAGE_ETS_TAG;
    EXPECT_EQ(rootNode->GetTag(), V2::PAGE_ETS_TAG);
    sheetPattern->DismissTransition(false, 1);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetHeightBySheetStyle001
 * @tc.desc: Increase the coverage of SheetPresentationLayoutAlgorithm::GetHeightBySheetStyle function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetHeightBySheetStyle001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto algorithm = AceType::DynamicCast<SheetPresentationLayoutAlgorithm>(sheetPattern->CreateLayoutAlgorithm());
    EXPECT_NE(algorithm->sheetStyle_.sheetHeight.sheetMode, SheetMode::MEDIUM);
    EXPECT_NE(algorithm->sheetStyle_.sheetHeight.sheetMode, SheetMode::LARGE);
    EXPECT_FALSE(algorithm->sheetStyle_.sheetHeight.height.has_value());
    auto maxHeight = 1000;
    auto maxWidth = 1000;
    algorithm->GetHeightBySheetStyle(maxHeight, maxWidth, AceType::RawPtr(sheetNode));

    algorithm->sheetStyle_.sheetHeight.sheetMode = SheetMode::MEDIUM;
    EXPECT_EQ(algorithm->sheetStyle_.sheetHeight.sheetMode, SheetMode::MEDIUM);
    EXPECT_FALSE(algorithm->sheetStyle_.sheetHeight.height.has_value());
    algorithm->GetHeightBySheetStyle(maxHeight, maxWidth, AceType::RawPtr(sheetNode));

    algorithm->sheetStyle_.sheetHeight.sheetMode = SheetMode::LARGE;
    algorithm->sheetStyle_.sheetHeight.height = 100.0_vp;
    EXPECT_EQ(algorithm->sheetStyle_.sheetHeight.sheetMode, SheetMode::LARGE);
    EXPECT_TRUE(algorithm->sheetStyle_.sheetHeight.height.has_value());
    EXPECT_FALSE(algorithm->SheetInSplitWindow());
    algorithm->GetHeightBySheetStyle(maxHeight, maxWidth, AceType::RawPtr(sheetNode));

    algorithm->sheetType_ = SheetType::SHEET_CENTER;
    auto pipelineContext = PipelineContext::GetCurrentContext();
    auto windowManager = pipelineContext->windowManager_;
    windowManager->SetWindowGetModeCallBack([]() -> WindowMode { return WindowMode::WINDOW_MODE_SPLIT_PRIMARY; });
    EXPECT_TRUE(algorithm->SheetInSplitWindow());
    algorithm->GetHeightBySheetStyle(maxHeight, maxWidth, AceType::RawPtr(sheetNode));
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: CreateSheetChildConstraint001
 * @tc.desc: Increase the coverage of SheetPresentationLayoutAlgorithm::CreateSheetChildConstraint function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, CreateSheetChildConstraint001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 301, AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto algorithm = AceType::DynamicCast<SheetPresentationLayoutAlgorithm>(sheetPattern->CreateLayoutAlgorithm());
    EXPECT_FALSE(algorithm->sheetStyle_.isTitleBuilder.has_value());
    algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));

    algorithm->sheetStyle_.isTitleBuilder = true;
    EXPECT_NE(algorithm->sheetType_, SheetType::SHEET_CENTER);
    EXPECT_NE(algorithm->sheetType_, SheetType::SHEET_POPUP);
    algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));

    algorithm->sheetType_ = SheetType::SHEET_CENTER;
    algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));
    algorithm->sheetType_ = SheetType::SHEET_POPUP;
    algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: CreateSheetChildConstraint002
 * @tc.desc: Branch: if ((sheetStyle_.isTitleBuilder.has_value()) && ((sheetType_ == SheetType::SHEET_CENTER) ||
 *                      (sheetType_ == SheetType::SHEET_POPUP)))
 *           Condition: sheetStyle_.isTitleBuilder.has_value() = true && sheetType_ == SheetType::SHEET_CENTER
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, CreateSheetChildConstraint002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sheet node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    auto builder = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));
    auto callback = [](const std::string&) {};
    SheetStyle style;
    style.isTitleBuilder = true;
    style.sheetType = SheetType::SHEET_CENTER;
    style.sheetTitle = MESSAGE;
    style.showCloseIcon = false;
    auto sheetNode = SheetView::CreateSheetPage(0, "", builder, builder, std::move(callback), style);
    ASSERT_NE(sheetNode, nullptr);

    /**
     * @tc.steps: step2. create sheet algorithm.
     * @tc.expected: sheet title has value.
     */
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto algorithm = AceType::DynamicCast<SheetPresentationLayoutAlgorithm>(sheetPattern->CreateLayoutAlgorithm());
    EXPECT_FALSE(algorithm->sheetStyle_.isTitleBuilder.has_value());
    algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));

    /**
     * @tc.steps: step3. set sheetType is SHEET_CENTER and sheetStyle_.isTitleBuilder is true and sheetHeight_ is 1000.
     */
    algorithm->sheetStyle_.isTitleBuilder = true;
    algorithm->sheetType_ = SheetType::SHEET_CENTER;
    algorithm->sheetStyle_.sheetHeight.sheetMode = SheetMode::AUTO;
    algorithm->sheetHeight_ = 1000.0f;

    /**
     * @tc.steps: step4. set title height is 100 and excute CreateSheetChildConstraint function.
     * @tc.expected: childConstraint.maxSize.Height() is 1000.
     */
    auto operationNode = AceType::DynamicCast<FrameNode>(sheetNode->GetChildAtIndex(0));
    ASSERT_NE(operationNode, nullptr);
    auto titleGeometryNode = operationNode->GetGeometryNode();
    ASSERT_NE(titleGeometryNode, nullptr);
    titleGeometryNode->SetFrameSize(SizeF(100.0f, 100.0f));
    auto childConstraint = algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));
    EXPECT_EQ(childConstraint.maxSize.Height(), 1000);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: CreateSheetChildConstraint003
 * @tc.desc: Branch: if (sheetType_ == SheetType::SHEET_POPUP)
 *           Condition: sheetStyle_.isTitleBuilder.has_value() = true && sheetType_ == SheetType::SHEET_POPUP
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, CreateSheetChildConstraint003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sheet node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    MockContainer::SetUp();
    MockContainer::Current()->pipelineContext_ = MockPipelineContext::GetCurrentContext();
    auto lastPlatformVersion = PipelineBase::GetCurrentContext()->GetMinPlatformVersion();
    MockContainer::Current()->SetApiTargetVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto builder = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));
    auto callback = [](const std::string&) {};
    SheetStyle style;
    style.isTitleBuilder = true;
    style.sheetType = SheetType::SHEET_POPUP;
    style.sheetTitle = MESSAGE;
    style.showCloseIcon = false;
    auto sheetNode = SheetView::CreateSheetPage(0, "", builder, builder, std::move(callback), style);
    ASSERT_NE(sheetNode, nullptr);

    /**
     * @tc.steps: step2. create sheet algorithm.
     * @tc.expected: sheet title has value.
     */
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto algorithm = AceType::DynamicCast<SheetPresentationLayoutAlgorithm>(sheetPattern->CreateLayoutAlgorithm());
    EXPECT_FALSE(algorithm->sheetStyle_.isTitleBuilder.has_value());
    algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));

    /**
     * @tc.steps: step3. set sheetType is SHEET_CENTER and sheetStyle_.isTitleBuilder is true and sheetHeight_ is 1000.
     */
    algorithm->sheetStyle_.isTitleBuilder = true;
    algorithm->sheetType_ = SheetType::SHEET_POPUP;
    algorithm->sheetStyle_.sheetHeight.sheetMode = SheetMode::AUTO;
    algorithm->sheetHeight_ = 1000.0f;

    /**
     * @tc.steps: step4. set title height is 100 and excute CreateSheetChildConstraint function.
     * @tc.expected: childConstraint.maxSize.Height() is (1000 - SHEET_ARROW_HEIGHT.ConvertToPx()).
     */
    auto operationNode = AceType::DynamicCast<FrameNode>(sheetNode->GetChildAtIndex(0));
    ASSERT_NE(operationNode, nullptr);
    auto titleGeometryNode = operationNode->GetGeometryNode();
    ASSERT_NE(titleGeometryNode, nullptr);
    titleGeometryNode->SetFrameSize(SizeF(100.0f, 100.0f));
    auto childConstraint = algorithm->CreateSheetChildConstraint(
        sheetPattern->GetLayoutProperty<SheetPresentationProperty>(), AceType::RawPtr(sheetNode));
    EXPECT_EQ(childConstraint.maxSize.Height(), 1000 - SHEET_ARROW_HEIGHT.ConvertToPx());
    MockContainer::Current()->SetApiTargetVersion(lastPlatformVersion);
    MockContainer::TearDown();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: SheetInSplitWindow001
 * @tc.desc: Increase the coverage of SheetPresentationLayoutAlgorithm::SheetInSplitWindow function.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, SheetInSplitWindow001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode("Sheet", 301,
        AceType::MakeRefPtr<SheetPresentationPattern>(401, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto algorithm = AceType::DynamicCast<SheetPresentationLayoutAlgorithm>(sheetPattern->CreateLayoutAlgorithm());
    algorithm->sheetType_ = SheetType::SHEET_BOTTOM;
    EXPECT_FALSE(algorithm->SheetInSplitWindow());
    algorithm->sheetType_ = SheetType::SHEET_CENTER;
    auto pipelineContext = PipelineContext::GetCurrentContext();
    auto windowManager = pipelineContext->windowManager_;
    pipelineContext->windowManager_ = nullptr;
    EXPECT_FALSE(algorithm->SheetInSplitWindow());
    pipelineContext->windowManager_ = windowManager;
    pipelineContext->displayWindowRectInfo_.height_ = SystemProperties::GetDeviceHeight();
    EXPECT_FALSE(algorithm->SheetInSplitWindow());
    pipelineContext->displayWindowRectInfo_.height_ = SystemProperties::GetDeviceHeight() - 10.0f;
    EXPECT_FALSE(algorithm->SheetInSplitWindow());

    windowManager->SetWindowGetModeCallBack([]() -> WindowMode { return WindowMode::WINDOW_MODE_SPLIT_PRIMARY; });
    EXPECT_TRUE(algorithm->SheetInSplitWindow());
    windowManager->SetWindowGetModeCallBack([]() -> WindowMode { return WindowMode::WINDOW_MODE_SPLIT_SECONDARY; });
    EXPECT_TRUE(algorithm->SheetInSplitWindow());
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetTopAreaInWindow001
 * @tc.desc: Get the TopArea for landscape and portrait.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetTopAreaInWindow001, TestSize.Level1)
{
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    sheetStyle.sheetHeight.sheetMode = SheetMode::LARGE;
    sheetStyle.sheetType = SheetType::SHEET_BOTTOM;
    layoutProperty->propSheetStyle_ = sheetStyle;

    auto deviceOrientation = static_cast<int32_t>(DeviceOrientation::LANDSCAPE);
    SystemProperties::SetDeviceOrientation(deviceOrientation);
    auto pipelineContext = MockPipelineContext::GetCurrentContext();
    auto safeAreaInsets = pipelineContext->GetSafeAreaWithoutProcess();
    EXPECT_EQ(safeAreaInsets.top_.Length(), sheetPattern->GetBottomSafeArea());

    deviceOrientation = static_cast<int32_t>(DeviceOrientation::PORTRAIT);
    SystemProperties::SetDeviceOrientation(deviceOrientation);
    safeAreaInsets = pipelineContext->GetSafeAreaWithoutProcess();
    EXPECT_EQ(safeAreaInsets.top_.Length(), sheetPattern->GetBottomSafeArea());
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: GetBottomSafeArea001
 * @tc.desc: Branch: if (cutoutSafeArea.top_.IsValid())
 *           Condition: cutoutSafeArea.top_.IsValid() == true && GetStatusBarHeight() == 300
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, GetBottomSafeArea001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create sheet node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto layoutProperty = sheetPattern->GetLayoutProperty<SheetPresentationProperty>();
    ASSERT_NE(layoutProperty, nullptr);
    SheetStyle sheetStyle;
    sheetStyle.sheetHeight.sheetMode = SheetMode::LARGE;
    sheetStyle.sheetType = SheetType::SHEET_BOTTOM;
    layoutProperty->propSheetStyle_ = sheetStyle;

    /**
     * @tc.steps: step1. mock pipeline and window, set cutoutSafeArea.top_.IsValid() is true.
     * @tc.expected: GetBottomSafeArea is 300.
     */
    auto pipelineContext = MockPipelineContext::GetCurrentContext();
    SafeAreaInsets::Inset insetleft;
    SafeAreaInsets::Inset insetTop;
    insetTop.start = 0;
    insetTop.end = 1;
    SafeAreaInsets::Inset insetRight;
    SafeAreaInsets::Inset insetBottom;
    pipelineContext->safeAreaManager_->cutoutSafeArea_ =
        SafeAreaInsets(insetleft, insetTop, insetRight, insetBottom);
    auto window = std::make_shared<MockWindow>();
    EXPECT_CALL(*window, GetStatusBarHeight()).WillRepeatedly([]() -> uint32_t { return 300; });
    pipelineContext->window_ = window;
    EXPECT_EQ(sheetPattern->GetBottomSafeArea(), 300);

    /**
     * @tc.steps: step1. set cutoutSafeArea.top_.IsValid() is false.
     * @tc.expected: GetBottomSafeArea is safeAreaInsets.top_.Length().
     */
    insetTop.end = 0;
    pipelineContext->safeAreaManager_->cutoutSafeArea_ =
        SafeAreaInsets(insetleft, insetTop, insetRight, insetBottom);
    auto safeAreaInsets = pipelineContext->GetSafeAreaWithoutProcess();
    EXPECT_EQ(sheetPattern->GetBottomSafeArea(), safeAreaInsets.top_.Length());

    pipelineContext->window_.reset();
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: SetSheetOuterBorderWidth001
 * @tc.desc: Test SetSheetOuterBorderWidth outerBorder updated.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, SetSheetOuterBorderWidth001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));

    auto renderContext = sheetNode->GetRenderContext();

    /**
     * @tc.steps: step2. Change the SheetTheme border.
     * @tc.expected: the border is updated successfully
     */
    SheetStyle sheetStyle;
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->isOuterBorderEnable_ = true;
    sheetTheme->sheetOuterBorderWidth_ = Dimension(20.0);
    sheetTheme->sheetInnerBorderWidth_ = Dimension(10.0);
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_CENTER);
    sheetPattern->sheetType_ = SheetType::SHEET_CENTER;
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->SetSheetOuterBorderWidth(sheetTheme, sheetStyle);
    EXPECT_EQ(renderContext->GetOuterBorderWidth().has_value(), true);
    BorderWidthProperty BorderWidth;
    BorderWidth.SetBorderWidth(Dimension(20.0));
    EXPECT_EQ(renderContext->GetOuterBorderWidth().value(), BorderWidth);
    BorderWidth.SetBorderWidth(Dimension(10.0));
    EXPECT_EQ(renderContext->GetBorderWidth().value(), BorderWidth);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: SetSheetOuterBorderWidth002
 * @tc.desc: Test SetSheetOuterBorderWidth outerBorder.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, SetSheetOuterBorderWidth002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));

    auto renderContext = sheetNode->GetRenderContext();

    /**
     * @tc.steps: step2. Change the SheetTheme border.
     * @tc.expected: the border is not updated
     */
    SheetStyle sheetStyle;
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->isOuterBorderEnable_ = false;
    sheetTheme->sheetOuterBorderWidth_ = Dimension(20.0);
    sheetTheme->sheetInnerBorderWidth_ = Dimension(10.0);
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_CENTER);
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->SetSheetOuterBorderWidth(sheetTheme, sheetStyle);
    EXPECT_EQ(renderContext->GetOuterBorderWidth().has_value(), false);
    EXPECT_EQ(renderContext->GetBorderWidth().has_value(), false);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: SetSheetOuterBorderWidth003
 * @tc.desc: Test SetSheetOuterBorderWidth outerBorder.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, SetSheetOuterBorderWidth003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));

    auto renderContext = sheetNode->GetRenderContext();

    /**
     * @tc.steps: step2. Change the SheetTheme border.
     * @tc.expected: the border is not updated
     */
    SheetStyle sheetStyle;

    sheetStyle.borderWidth = BORDER_WIDTH_TEST;
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->isOuterBorderEnable_ = true;
    sheetTheme->sheetOuterBorderWidth_ = Dimension(20.0);
    sheetTheme->sheetInnerBorderWidth_ = Dimension(10.0);
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_CENTER);
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->SetSheetOuterBorderWidth(sheetTheme, sheetStyle);
    EXPECT_EQ(renderContext->GetOuterBorderWidth().has_value(), false);
    EXPECT_EQ(renderContext->GetBorderWidth().has_value(), false);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: SetSheetOuterBorderWidth004
 * @tc.desc: Test SetSheetOuterBorderWidth outerBorder updated.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, SetSheetOuterBorderWidth004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));

    auto renderContext = sheetNode->GetRenderContext();

    /**
     * @tc.steps: step2. Change the SheetTheme border.
     * @tc.expected: the border is updated successfully
     */
    SheetStyle sheetStyle;
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->isOuterBorderEnable_ = true;
    sheetTheme->sheetOuterBorderWidth_ = Dimension(20.0);
    sheetTheme->sheetInnerBorderWidth_ = Dimension(10.0);
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_BOTTOM);
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->SetSheetOuterBorderWidth(sheetTheme, sheetStyle);
    EXPECT_EQ(renderContext->GetOuterBorderWidth().has_value(), true);
    EXPECT_EQ(renderContext->GetBorderWidth().has_value(), true);
    SheetPresentationTestNg::TearDownTestCase();
}

/**
 * @tc.name: SetSheetOuterBorderWidth005
 * @tc.desc: Test SetSheetOuterBorderWidth outerBorder updated.
 * @tc.type: FUNC
 */
HWTEST_F(SheetPresentationTestNg, SetSheetOuterBorderWidth005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    SheetPresentationTestNg::SetUpTestCase();
    SheetPresentationTestNg::SetApiVersion(static_cast<int32_t>(PlatformVersion::VERSION_TWELVE));
    auto callback = [](const std::string&) {};
    auto sheetNode = FrameNode::CreateFrameNode(
        "Sheet", 101, AceType::MakeRefPtr<SheetPresentationPattern>(201, "SheetPresentation", std::move(callback)));

    auto renderContext = sheetNode->GetRenderContext();

    /**
     * @tc.steps: step2. Change the SheetTheme border.
     * @tc.expected: the border is not updated successfully
     */
    SheetStyle sheetStyle;
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    auto sheetTheme = AceType::MakeRefPtr<SheetTheme>();
    sheetTheme->isOuterBorderEnable_ = true;
    sheetTheme->sheetOuterBorderWidth_ = Dimension(20.0);
    sheetTheme->sheetInnerBorderWidth_ = Dimension(10.0);
    SheetPresentationTestNg::SetSheetTheme(sheetTheme);
    SheetPresentationTestNg::SetSheetType(sheetPattern, SheetType::SHEET_POPUP);
    sheetPattern->InitSheetObject();
    ASSERT_NE(sheetPattern->GetSheetObject(), nullptr);
    sheetPattern->SetSheetOuterBorderWidth(sheetTheme, sheetStyle);
    EXPECT_EQ(renderContext->GetOuterBorderWidth().has_value(), true);
    EXPECT_EQ(renderContext->GetBorderWidth().has_value(), true);
    SheetPresentationTestNg::TearDownTestCase();
}
} // namespace OHOS::Ace::NG
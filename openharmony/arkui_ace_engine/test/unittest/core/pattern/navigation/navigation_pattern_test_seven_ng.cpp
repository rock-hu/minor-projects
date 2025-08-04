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

#include <optional>

#include "gtest/gtest.h"
#include "mock_navigation_route.h"
#include "mock_navigation_stack.h"

#define protected public
#define private public
#include "core/components_ng/pattern/navigation/nav_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_content_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_drag_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_model_ng.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navigation/tool_bar_pattern.h"
#include "core/components_ng/pattern/stage/page_node.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/common/mock_theme_manager.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"
#include "core/components_ng/pattern/navigation/bar_item_node.h"
#include "core/components_ng/pattern/navigation/bar_item_pattern.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string BUNDLE_NAME = "com.example.test";
const std::string MODULE_NAME = "entry";
} // namespace

class NavigationPatternTestSevenNg : public testing::Test {
public:
    static RefPtr<NavigationBarTheme> navigationBarTheme_;
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    void SetForceSplitEnabled(bool enable);
    void MockPipelineContextGetTheme();
};

RefPtr<NavigationBarTheme> NavigationPatternTestSevenNg::navigationBarTheme_ = nullptr;

void NavigationPatternTestSevenNg::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();

    navigationBarTheme_ = AceType::MakeRefPtr<NavigationBarTheme>();
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly([](ThemeType type) -> RefPtr<Theme> {
        if (type == NavigationBarTheme::TypeId()) {
            return navigationBarTheme_;
        } else {
            return nullptr;
        }
    });
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
}

void NavigationPatternTestSevenNg::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
}

void NavigationPatternTestSevenNg::SetForceSplitEnabled(bool enable)
{
    auto context = PipelineContext::GetCurrentContext();
    ASSERT_NE(context, nullptr);
    auto manager = context->GetNavigationManager();
    ASSERT_NE(manager, nullptr);
    manager->isForceSplitSupported_ = enable;
}

void NavigationPatternTestSevenNg::MockPipelineContextGetTheme()
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineContext::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<NavigationBarTheme>()));
    EXPECT_CALL(*themeManager, GetTheme(_, _)).WillRepeatedly(Return(AceType::MakeRefPtr<NavigationBarTheme>()));
}

/**
 * @tc.name: RemoveRedundantPrimaryNavDestination001
 * @tc.desc: Branch: while (primaryContentNode->GetChildren().size() > primaryNodes_.size()) { => false
 *                   if (hasRemoveNode) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, RemoveRedundantPrimaryNavDestination001, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto primaryContentNode = AceType::DynamicCast<FrameNode>(navNode->GetPrimaryContentNode());
    ASSERT_NE(primaryContentNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    ASSERT_TRUE(primaryContentNode->GetChildren().empty());

    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));

    primaryContentNode->needSyncRenderTree_ = false;
    pattern->RemoveRedundantPrimaryNavDestination();
    EXPECT_FALSE(primaryContentNode->needSyncRenderTree_);
}

/**
 * @tc.name: RemoveRedundantPrimaryNavDestination002
 * @tc.desc: Branch: while (primaryContentNode->GetChildren().size() > primaryNodes_.size()) { => true
 *                   if (hasRemoveNode) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, RemoveRedundantPrimaryNavDestination002, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto primaryContentNode = AceType::DynamicCast<FrameNode>(navNode->GetPrimaryContentNode());
    ASSERT_NE(primaryContentNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    primaryContentNode->AddChild(dest);
    ASSERT_FALSE(primaryContentNode->GetChildren().empty());
    pattern->primaryNodes_.clear();
    primaryContentNode->needSyncRenderTree_ = false;
    pattern->RemoveRedundantPrimaryNavDestination();
    EXPECT_TRUE(primaryContentNode->needSyncRenderTree_);
}

/**
 * @tc.name: FirePrimaryNodesOnShowAndActive001
 * @tc.desc: Branch: if (!pattern->GetIsOnShow()) { => true
 *                   if (!pattern->IsActive()) { => true or false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesOnShowAndActive001, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    destPattern->SetIsOnShow(false);
    destPattern->SetIsActive(false);

    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    pattern->FirePrimaryNodesOnShowAndActive();
    EXPECT_TRUE(destPattern->GetIsOnShow());
    EXPECT_TRUE(destPattern->IsActive());

    destPattern->SetIsOnShow(false);
    destPattern->SetIsActive(true);

    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    pattern->FirePrimaryNodesOnShowAndActive();
    EXPECT_TRUE(destPattern->GetIsOnShow());
    EXPECT_TRUE(destPattern->IsActive());
}

/**
 * @tc.name: FirePrimaryNodesOnShowAndActive002
 * @tc.desc: Branch: if (!pattern->GetIsOnShow()) { => false
 *                   if (!pattern->IsActive()) { => true or false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesOnShowAndActive002, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    destPattern->SetIsOnShow(true);
    destPattern->SetIsActive(false);

    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    pattern->FirePrimaryNodesOnShowAndActive();
    EXPECT_TRUE(destPattern->GetIsOnShow());
    EXPECT_TRUE(destPattern->IsActive());

    destPattern->SetIsOnShow(true);
    destPattern->SetIsActive(true);

    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    pattern->FirePrimaryNodesOnShowAndActive();
    EXPECT_TRUE(destPattern->GetIsOnShow());
    EXPECT_TRUE(destPattern->IsActive());
}

/**
 * @tc.name: FirePrePrimaryNodesOnWillDisappear001
 * @tc.desc: Branch: if (filterNodes.find(primaryNode) != filterNodes.end()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrePrimaryNodesOnWillDisappear001, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    pattern->primaryNodesToBeRemoved_.clear();
    pattern->primaryNodesToBeRemoved_.push_back(dest);
    std::set<RefPtr<NavDestinationGroupNode>> filterNodes;
    filterNodes.emplace(dest);
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrePrimaryNodesOnWillDisappear(std::move(filterNodes));
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePrePrimaryNodesOnWillDisappear002
 * @tc.desc: Branch: if (filterNodes.find(primaryNode) != filterNodes.end()) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrePrimaryNodesOnWillDisappear002, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    pattern->primaryNodesToBeRemoved_.clear();
    pattern->primaryNodesToBeRemoved_.push_back(dest);
    std::set<RefPtr<NavDestinationGroupNode>> filterNodes;
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrePrimaryNodesOnWillDisappear(std::move(filterNodes));
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_WILL_DISAPPEAR);
}

/**
 * @tc.name: FirePrePrimaryNodesOnHide001
 * @tc.desc: Branch: if (!node->IsShowInPrimaryPartition() && pattern->GetIsOnShow()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrePrimaryNodesOnHide001, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    dest->SetIsShowInPrimaryPartition(false);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    destPattern->SetIsOnShow(true);
    pattern->primaryNodesToBeRemoved_.clear();
    pattern->prePrimaryNodes_.clear();
    pattern->prePrimaryNodes_.push_back(WeakPtr(dest));
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrePrimaryNodesOnHide();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_HIDDEN);
}

/**
 * @tc.name: FirePrePrimaryNodesOnHide002
 * @tc.desc: Branch: if (!node->IsShowInPrimaryPartition() && pattern->GetIsOnShow()) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrePrimaryNodesOnHide002, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    pattern->primaryNodesToBeRemoved_.clear();
    pattern->prePrimaryNodes_.clear();
    pattern->prePrimaryNodes_.push_back(WeakPtr(dest));

    dest->SetIsShowInPrimaryPartition(true);
    destPattern->SetIsOnShow(true);
    destEventHub->state_ = NavDestinationState::ON_HIDDEN;
    pattern->FirePrePrimaryNodesOnHide();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_HIDDEN);

    dest->SetIsShowInPrimaryPartition(false);
    destPattern->SetIsOnShow(false);
    destEventHub->state_ = NavDestinationState::ON_HIDDEN;
    pattern->FirePrePrimaryNodesOnHide();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_HIDDEN);

    dest->SetIsShowInPrimaryPartition(true);
    destPattern->SetIsOnShow(false);
    destEventHub->state_ = NavDestinationState::ON_HIDDEN;
    pattern->FirePrePrimaryNodesOnHide();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_HIDDEN);
}

/**
 * @tc.name: FirePrePrimaryNodesOnHide003
 * @tc.desc: Branch: if (!pattern->GetIsOnShow()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrePrimaryNodesOnHide003, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    pattern->primaryNodesToBeRemoved_.clear();
    pattern->primaryNodesToBeRemoved_.push_back(dest);

    destPattern->SetIsOnShow(false);
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrePrimaryNodesOnHide();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePrePrimaryNodesOnHide004
 * @tc.desc: Branch: if (!pattern->GetIsOnShow()) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrePrimaryNodesOnHide004, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    pattern->primaryNodesToBeRemoved_.clear();
    pattern->primaryNodesToBeRemoved_.push_back(dest);

    destPattern->SetIsOnShow(true);
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrePrimaryNodesOnHide();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_HIDDEN);
}

/**
 * @tc.name: FirePreTopPrimaryNodeInactiveIfNeeded001
 * @tc.desc: Branch: if (prePrimaryNodes_.empty()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePreTopPrimaryNodeInactiveIfNeeded001, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    pattern->prePrimaryNodes_.clear();
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePreTopPrimaryNodeInactiveIfNeeded();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePreTopPrimaryNodeInactiveIfNeeded002
 * @tc.desc: Branch: if (prePrimaryNodes_.empty()) { => false
 *                   if (!preTopPrimaryNode->IsActive()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePreTopPrimaryNodeInactiveIfNeeded002, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    pattern->prePrimaryNodes_.clear();
    pattern->prePrimaryNodes_.push_back(WeakPtr(dest));
    dest->SetActive(false);
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePreTopPrimaryNodeInactiveIfNeeded();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePreTopPrimaryNodeInactiveIfNeeded003
 * @tc.desc: Branch: if (prePrimaryNodes_.empty()) { => false
 *                   if (!preTopPrimaryNode->IsActive()) { => false
 *                   if (!primaryNodes_.empty()) { => false
 *                   if (navigationStack_) { => false
 *                   if (preTopPrimaryNode != topPrimaryNode && preTopPrimaryNode != topNode) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePreTopPrimaryNodeInactiveIfNeeded003, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);

    dest->SetActive(true);
    destPattern->SetIsActive(true);
    pattern->prePrimaryNodes_.clear();
    pattern->prePrimaryNodes_.push_back(WeakPtr(dest));
    pattern->primaryNodes_.clear();
    auto backupStack = pattern->navigationStack_;
    pattern->navigationStack_ = nullptr;

    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePreTopPrimaryNodeInactiveIfNeeded();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_INACTIVE);
    pattern->navigationStack_ = backupStack;
}

/**
 * @tc.name: FirePreTopPrimaryNodeInactiveIfNeeded004
 * @tc.desc: Branch: if (prePrimaryNodes_.empty()) { => false
 *                   if (!preTopPrimaryNode->IsActive()) { => false
 *                   if (!primaryNodes_.empty()) { => true
 *                   if (navigationStack_) { => false
 *                   if (preTopPrimaryNode != topPrimaryNode && preTopPrimaryNode != topNode) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePreTopPrimaryNodeInactiveIfNeeded004, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);

    dest->SetActive(true);
    pattern->prePrimaryNodes_.clear();
    pattern->prePrimaryNodes_.push_back(WeakPtr(dest));
    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    auto backupStack = pattern->navigationStack_;
    pattern->navigationStack_ = nullptr;
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePreTopPrimaryNodeInactiveIfNeeded();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
    pattern->navigationStack_ = backupStack;
}

/**
 * @tc.name: FirePreTopPrimaryNodeInactiveIfNeeded005
 * @tc.desc: Branch: if (prePrimaryNodes_.empty()) { => false
 *                   if (!preTopPrimaryNode->IsActive()) { => false
 *                   if (!primaryNodes_.empty()) { => false
 *                   if (navigationStack_) { => true
 *                   if (preTopPrimaryNode != topPrimaryNode && preTopPrimaryNode != topNode) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePreTopPrimaryNodeInactiveIfNeeded005, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    std::pair<std::string, RefPtr<UINode>> testPair{"one", dest};
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);

    dest->SetActive(true);
    pattern->prePrimaryNodes_.clear();
    pattern->prePrimaryNodes_.push_back(WeakPtr(dest));
    pattern->primaryNodes_.clear();
    ASSERT_NE(pattern->navigationStack_, nullptr);
    pattern->navigationStack_->navPathList_.clear();
    pattern->navigationStack_->navPathList_.push_back(testPair);
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePreTopPrimaryNodeInactiveIfNeeded();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePreTopPrimaryNodeInactiveIfNeeded006
 * @tc.desc: Branch: if (prePrimaryNodes_.empty()) { => false
 *                   if (!preTopPrimaryNode->IsActive()) { => false
 *                   if (!primaryNodes_.empty()) { => true
 *                   if (navigationStack_) { => true
 *                   if (preTopPrimaryNode != topPrimaryNode && preTopPrimaryNode != topNode) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePreTopPrimaryNodeInactiveIfNeeded006, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    std::pair<std::string, RefPtr<UINode>> testPair{"one", dest};
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);

    dest->SetActive(true);
    pattern->prePrimaryNodes_.clear();
    pattern->prePrimaryNodes_.push_back(WeakPtr(dest));
    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(dest);
    ASSERT_NE(pattern->navigationStack_, nullptr);
    pattern->navigationStack_->navPathList_.clear();
    pattern->navigationStack_->navPathList_.push_back(testPair);
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePreTopPrimaryNodeInactiveIfNeeded();
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePrimaryNodesLifecycle001
 * @tc.desc: Branch: if (lifecycle != NavDestinationLifecycle::ON_SHOW &&
 *                       lifecycle != NavDestinationLifecycle::ON_HIDE) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesLifecycle001, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrimaryNodesLifecycle(NavDestinationLifecycle::ON_ACTIVE);
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePrimaryNodesLifecycle002
 * @tc.desc: Branch: if (lifecycle != NavDestinationLifecycle::ON_SHOW &&
 *                       lifecycle != NavDestinationLifecycle::ON_HIDE) { => false
 *                   if (homeNode->GetIndex() >= navigation->GetLastStandardIndex()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesLifecycle002, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    dest->index_ = 1;
    navNode->lastStandardIndex_ = 0;
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrimaryNodesLifecycle(NavDestinationLifecycle::ON_SHOW);
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: FirePrimaryNodesLifecycle003
 * @tc.desc: Branch: if (lifecycle != NavDestinationLifecycle::ON_SHOW &&
 *                       lifecycle != NavDestinationLifecycle::ON_HIDE) { => false
 *                   if (homeNode->GetIndex() >= navigation->GetLastStandardIndex()) { => false
 *                   if (lifecycle == NavDestinationLifecycle::ON_SHOW) { => true
 *                   if (!pattern->GetIsOnShow()) { => true
 *                   if (idx == primaryNodes.size() - 1 && !pattern->IsActive()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesLifecycle003, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    pattern->homeNode_ = WeakPtr(dest);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    dest->index_ = 0;
    navNode->lastStandardIndex_ = 1;

    destPattern->SetIsOnShow(false);
    destPattern->SetIsActive(false);
    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));

    destEventHub->state_ = NavDestinationState::ON_HIDDEN;
    pattern->FirePrimaryNodesLifecycle(NavDestinationLifecycle::ON_SHOW);
    EXPECT_TRUE(destPattern->GetIsOnShow());
    EXPECT_TRUE(destPattern->IsActive());
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_ACTIVE);
}

/**
 * @tc.name: FirePrimaryNodesLifecycle004
 * @tc.desc: Branch: if (lifecycle != NavDestinationLifecycle::ON_SHOW &&
 *                       lifecycle != NavDestinationLifecycle::ON_HIDE) { => false
 *                   if (homeNode->GetIndex() >= navigation->GetLastStandardIndex()) { => false
 *                   if (lifecycle == NavDestinationLifecycle::ON_SHOW) { => true
 *                   if (!pattern->GetIsOnShow()) { => false
 *                   if (idx == primaryNodes.size() - 1 && !pattern->IsActive()) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesLifecycle004, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    pattern->homeNode_ = WeakPtr(dest);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    dest->index_ = 0;
    navNode->lastStandardIndex_ = 1;

    destPattern->SetIsOnShow(true);
    destPattern->SetIsActive(true);
    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_HIDDEN;
    pattern->FirePrimaryNodesLifecycle(NavDestinationLifecycle::ON_SHOW);
    EXPECT_TRUE(destPattern->GetIsOnShow());
    EXPECT_TRUE(destPattern->IsActive());
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_HIDDEN);
}

/**
 * @tc.name: FirePrimaryNodesLifecycle005
 * @tc.desc: Branch: if (lifecycle != NavDestinationLifecycle::ON_SHOW &&
 *                       lifecycle != NavDestinationLifecycle::ON_HIDE) { => false
 *                   if (homeNode->GetIndex() >= navigation->GetLastStandardIndex()) { => false
 *                   if (lifecycle == NavDestinationLifecycle::ON_SHOW) { => false
 *                   if (idx == 0 && pattern->IsActive()) { => true
 *                   if (pattern->GetIsOnShow()) { => true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesLifecycle005, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    pattern->homeNode_ = WeakPtr(dest);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    dest->index_ = 0;
    navNode->lastStandardIndex_ = 1;
    destPattern->SetIsOnShow(true);
    destPattern->SetIsActive(true);
    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrimaryNodesLifecycle(NavDestinationLifecycle::ON_HIDE);
    EXPECT_FALSE(destPattern->GetIsOnShow());
    EXPECT_FALSE(destPattern->IsActive());
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_HIDDEN);
}

/**
 * @tc.name: FirePrimaryNodesLifecycle006
 * @tc.desc: Branch: if (lifecycle != NavDestinationLifecycle::ON_SHOW &&
 *                       lifecycle != NavDestinationLifecycle::ON_HIDE) { => false
 *                   if (homeNode->GetIndex() >= navigation->GetLastStandardIndex()) { => false
 *                   if (lifecycle == NavDestinationLifecycle::ON_SHOW) { => false
 *                   if (idx == 0 && pattern->IsActive()) { => false
 *                   if (pattern->GetIsOnShow()) { => false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, FirePrimaryNodesLifecycle006, TestSize.Level1)
{
    SetForceSplitEnabled(true);
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto navNode = AceType::DynamicCast<NavigationGroupNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(navNode, nullptr);
    auto pattern = navNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto dest = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(dest, nullptr);
    pattern->homeNode_ = WeakPtr(dest);
    auto destPattern = dest->GetPattern<NavDestinationPattern>();
    ASSERT_NE(destPattern, nullptr);
    auto destEventHub = dest->GetOrCreateEventHub<NavDestinationEventHub>();
    ASSERT_NE(destEventHub, nullptr);
    dest->index_ = 0;
    navNode->lastStandardIndex_ = 1;
    destPattern->SetIsOnShow(false);
    destPattern->SetIsActive(false);
    pattern->primaryNodes_.clear();
    pattern->primaryNodes_.push_back(WeakPtr(dest));
    // unchanged, just for Branch Coverage
    destEventHub->state_ = NavDestinationState::ON_SHOWN;
    pattern->FirePrimaryNodesLifecycle(NavDestinationLifecycle::ON_HIDE);
    EXPECT_FALSE(destPattern->GetIsOnShow());
    EXPECT_FALSE(destPattern->IsActive());
    EXPECT_EQ(destEventHub->state_, NavDestinationState::ON_SHOWN);
}

/**
 * @tc.name: SetBackButtonIcon001
 * @tc.desc: Test SetBackButtonIcon function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetBackButtonIcon001, TestSize.Level1)
{
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

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
    std::string backButtonIconKey = "navigation.backButtonIcon.icon";
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), "");
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.SetBackButtonIcon(iconSymbol, resObj, imageOption, pixMap, nameList);

    titleBarPattern->OnColorModeChange(1);
    std::string result;
    ResourceParseUtils::ParseResMedia(resObj, result);
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), result);
}

/**
 * @tc.name: SetBackButtonIcon002
 * @tc.desc: Test SetBackButtonIconSrcAndTextRes function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetBackButtonIcon002, TestSize.Level1)
{
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

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
    std::string backButtonIconKey = "navigation.backButtonIcon.icon";
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), "");
    std::string backButtonTextKey = "navigation.backButtonIcon.accessibilityText";
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonTextKey), "");
    RefPtr<ResourceObject> backButtonIconResObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    RefPtr<ResourceObject> backButtonTextResObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.SetBackButtonIconSrcAndTextRes(
        iconSymbol, backButtonIconResObj, imageOption, pixMap, nameList, true, backButtonTextResObj);

    titleBarPattern->OnColorModeChange(1);
    std::string icon;
    ResourceParseUtils::ParseResMedia(backButtonIconResObj, icon);
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), icon);
    std::string text;
    ResourceParseUtils::ParseResString(backButtonIconResObj, text);
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), text);
}

/**
 * @tc.name: SetBackButtonIcon003
 * @tc.desc: Test UpdateBackButtonIcon function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetBackButtonIcon003, TestSize.Level1)
{
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

    std::vector<std::string> nameList;
    nameList.push_back("");
    nameList.push_back("");
    std::string backButtonIconKey = "navigation.backButtonIcon.icon";
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), "");
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.UpdateBackButtonIcon(nameList, frameNode, resObj);

    titleBarPattern->OnColorModeChange(1);
    std::string result;
    ResourceParseUtils::ParseResMedia(resObj, result);
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), result);
}

/**
 * @tc.name: SetBackButtonIcon004
 * @tc.desc: Test UpdateBackButtonIconText function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetBackButtonIcon004, TestSize.Level1)
{
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

    std::string backButtonTextKey = "navigation.backButtonIcon.accessibilityText";
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonTextKey), "");
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.UpdateBackButtonIconText(true, titleBarNode, resObj);

    titleBarPattern->OnColorModeChange(1);
    std::string result;
    ResourceParseUtils::ParseResString(resObj, result);
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonTextKey), result);
}

/**
 * @tc.name: SetBackButtonIcon005
 * @tc.desc: Test SetBackButtonIconTextRes function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetBackButtonIcon005, TestSize.Level1)
{
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

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
    std::string backButtonTextKey = "navigation.backButtonIcon.accessibilityText";
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonTextKey), "");
    RefPtr<ResourceObject> backButtonTextResObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.SetBackButtonIconTextRes(
        iconSymbol, imageSource, imageOption, pixMap, nameList, true, backButtonTextResObj);

    titleBarPattern->OnColorModeChange(1);
    std::string result;
    ResourceParseUtils::ParseResString(backButtonTextResObj, result);
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonTextKey), result);
}

/**
 * @tc.name: SetBackButtonIcon006
 * @tc.desc: Test SetBackButtonIcon function with specific node.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetBackButtonIcon006, TestSize.Level1)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    EXPECT_NE(titleBarPattern, nullptr);

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
    std::string backButtonIconKey = "navigation.backButtonIcon.icon";
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), "");
    RefPtr<ResourceObject> backButtonIconResObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    NavigationModelNG::SetBackButtonIcon(frameNode, iconSymbol, imageOption, pixMap, backButtonIconResObj);

    titleBarPattern->OnColorModeChange(1);
    std::string result;
    ResourceParseUtils::ParseResMedia(backButtonIconResObj, result);
    EXPECT_EQ(titleBarPattern->GetResCacheMapByKey(backButtonIconKey), result);
}

/**
 * @tc.name: SetNavBarWidth002
 * @tc.desc: Test SetMinNavBarWidth and SetMaxNavBarWidth function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetNavBarWidth002, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    navigationModel.SetTitle("navigationModel", false);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.SetMinNavBarWidth(resObj);
    std::string minKey = "navigation.navBarWidthRange.minNavBarWidth";
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(minKey), "");
    navigationModel.SetMaxNavBarWidth(resObj);
    std::string maxKey = "navigation.navBarWidthRange.maxNavBarWidth";
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(maxKey), "");
    navigationPattern->OnColorModeChange(1);
    CalcDimension minNavBarWidth;
    ResourceParseUtils::ParseResDimensionVpNG(resObj, minNavBarWidth);
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(minKey), minNavBarWidth.ToString());
}

/**
 * @tc.name: SetNavBarWidth003
 * @tc.desc: Test SetMinNavBarWidth and SetMaxNavBarWidth function with specific node.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetNavBarWidth003, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    navigationModel.SetTitle("navigationModel", false);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.SetMinNavBarWidth(frameNode, resObj);
    std::string minKey = "navigation.navBarWidthRange.minNavBarWidth";
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(minKey), "");
    navigationModel.SetMaxNavBarWidth(frameNode, resObj);
    std::string maxKey = "navigation.navBarWidthRange.maxNavBarWidth";
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(maxKey), "");
    navigationPattern->OnColorModeChange(1);
    CalcDimension minNavBarWidth;
    ResourceParseUtils::ParseResDimensionVpNG(resObj, minNavBarWidth);
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(minKey), minNavBarWidth.ToString());
}

/**
 * @tc.name: SetNavBarWidth004
 * @tc.desc: Test SetNavBarWidth function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetNavBarWidth004, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    navigationModel.SetTitle("navigationModel", false);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.SetNavBarWidth(resObj);
    Dimension value = 250.0_vp;
    navigationPattern->initNavBarWidthValue_ = value;
    EXPECT_EQ(navigationPattern->GetInitNavBarWidth(), value);
    std::string key = "navigation.navBarWidth";
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(key), "");
    navigationPattern->OnColorModeChange(1);
    CalcDimension navBarWidth;
    ResourceParseUtils::ParseResDimensionVpNG(resObj, navBarWidth);
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(key), navBarWidth.ToString());
}

/**
 * @tc.name: SetNavBarWidth005
 * @tc.desc: Test SetNavBarWidth function with specific node.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetNavBarWidth005, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    navigationModel.SetTitle("navigationModel", false);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.SetNavBarWidth(frameNode, resObj);
    Dimension value = 250.0_vp;
    navigationPattern->initNavBarWidthValue_ = value;
    EXPECT_EQ(navigationPattern->GetInitNavBarWidth(), value);
    std::string key = "navigation.navBarWidth";
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(key), "");
    navigationPattern->OnColorModeChange(1);
    CalcDimension navBarWidth;
    ResourceParseUtils::ParseResDimensionVpNG(resObj, navBarWidth);
    EXPECT_EQ(navigationPattern->GetResCacheMapByKey(key), navBarWidth.ToString());
}

/**
 * @tc.name: ParseTitleHeight001
 * @tc.desc: Test ParseTitleHeight
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, ParseTitleHeight001, TestSize.Level1)
{
    /**
     * @tc.steps1: init
     * @tc.expected: All pointer is non-null.
     */
    MockPipelineContextGetTheme();
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    navigationModel.SetTitle("navigationModel", false);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    ASSERT_NE(titleBarPattern, nullptr);

    std::string heighValue;
    std::string key = "navigation.title.customtitle";
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    navigationModel.ParseTitleHeight(titleBarNode, resObj);
    std::string heighString = titleBarPattern->GetResCacheMapByKey(key);
    EXPECT_EQ(heighString, "0.00px");
    ResourceParseUtils::ParseResString(resObj, key);
    CalcDimension height;
    bool isValid = ResourceParseUtils::ParseResDimensionVpNG(resObj, height);
    EXPECT_EQ(isValid, false);
}

/**
 * @tc.name: SetTitleHeight002
 * @tc.desc: Test SetTitleHeight
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, SetTitleHeight002, TestSize.Level1)
{
    /**
     * @tc.steps1: init
     * @tc.expected: All pointer is non-null.
     */
    MockPipelineContextGetTheme();
    NavigationModelNG navigationModel;
    navigationModel.Create();
    navigationModel.SetNavigationStack();
    navigationModel.SetTitle("navigationModel", false);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    ASSERT_NE(titleBarNode, nullptr);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    ASSERT_NE(titleBarLayoutProperty, nullptr);
    auto titleBarPattern = titleBarNode->GetPattern<TitleBarPattern>();
    ASSERT_NE(titleBarPattern, nullptr);

    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>(BUNDLE_NAME, MODULE_NAME, 0);
    CalcDimension height;
    auto heightValue = navigationModel.ParseTitleHeight(titleBarNode, resObj);
    EXPECT_EQ(height, heightValue);
    titleBarPattern->OnColorModeChange(1);
}

/**
 * @tc.name: BarItemPattern_ToJsonValue001
 * @tc.desc: Test ToJsonValue when filter.IsFastFilter() is true.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, BarItemPattern_ToJsonValue001, TestSize.Level1)
{
    auto barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto barItemNode = BarItemNode::GetOrCreateBarItemNode(
        V2::BAR_ITEM_ETS_TAG, barItemNodeId, []() { return AceType::MakeRefPtr<BarItemPattern>(); });
    auto barItemPattern = barItemNode->GetPattern<BarItemPattern>();
    ASSERT_NE(barItemPattern, nullptr);

    std::unique_ptr<JsonValue> json = JsonUtil::Create(true);
    InspectorFilter filter;
    filter.filterFixed = 1; // Make IsFastFilter() true
    barItemPattern->ToJsonValue(json, filter);
    // Only check that function runs, as fast filter returns early
    SUCCEED();
}

/**
 * @tc.name: BarItemPattern_ToJsonValue002
 * @tc.desc: Test ToJsonValue when filter.IsFastFilter() is false.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, BarItemPattern_ToJsonValue002, TestSize.Level1)
{
    auto barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto barItemNode = BarItemNode::GetOrCreateBarItemNode(
        V2::BAR_ITEM_ETS_TAG, barItemNodeId, []() { return AceType::MakeRefPtr<BarItemPattern>(); });
    auto barItemPattern = barItemNode->GetPattern<BarItemPattern>();
    ASSERT_NE(barItemPattern, nullptr);

    // Set up text node and property
    auto textNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });
    auto textLayoutProperty = textNode->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    textLayoutProperty->propContent_ = u"TestLabel";
    barItemNode->SetTextNode(textNode);

    std::unique_ptr<JsonValue> json = JsonUtil::Create(true);
    InspectorFilter filter; // IsFastFilter() false
    barItemPattern->ToJsonValue(json, filter);
    EXPECT_EQ(json->GetString("label"), "TestLabel");
}

/**
 * @tc.name: BarItemPattern_OnThemeScopeUpdate001
 * @tc.desc: Test OnThemeScopeUpdate with ToolbarIconStatus::ACTIVE and iconNode is SYMBOL_ETS_TAG.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, BarItemPattern_OnThemeScopeUpdate001, TestSize.Level1)
{
    auto barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto barItemNode = BarItemNode::GetOrCreateBarItemNode(
        V2::BAR_ITEM_ETS_TAG, barItemNodeId, []() { return AceType::MakeRefPtr<BarItemPattern>(); });
    auto barItemPattern = barItemNode->GetPattern<BarItemPattern>();
    ASSERT_NE(barItemPattern, nullptr);

    // Set icon node as SYMBOL_ETS_TAG
    auto iconNode = FrameNode::GetOrCreateFrameNode(V2::SYMBOL_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });
    barItemNode->SetIconNode(iconNode);

    // Set text node
    auto textNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });
    barItemNode->SetTextNode(textNode);

    barItemPattern->SetCurrentIconStatus(ToolbarIconStatus::ACTIVE);
    bool result = barItemPattern->OnThemeScopeUpdate(0);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: BarItemPattern_OnThemeScopeUpdate002
 * @tc.desc: Test OnThemeScopeUpdate with ToolbarIconStatus::INITIAL and iconNode is not SYMBOL_ETS_TAG.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, BarItemPattern_OnThemeScopeUpdate002, TestSize.Level1)
{
    auto barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto barItemNode = BarItemNode::GetOrCreateBarItemNode(
        V2::BAR_ITEM_ETS_TAG, barItemNodeId, []() { return AceType::MakeRefPtr<BarItemPattern>(); });
    auto barItemPattern = barItemNode->GetPattern<BarItemPattern>();
    ASSERT_NE(barItemPattern, nullptr);

    // Set icon node as IMAGE_ETS_TAG (not SYMBOL_ETS_TAG)
    auto iconNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ImagePattern>(); });
    barItemNode->SetIconNode(iconNode);

    // Set text node
    auto textNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });
    barItemNode->SetTextNode(textNode);

    barItemPattern->SetCurrentIconStatus(ToolbarIconStatus::INITIAL);
    bool result = barItemPattern->OnThemeScopeUpdate(0);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: InnerNavigationController_DeletePIPMode001
 * @tc.desc: Test DeletePIPMode with valid navigation pattern and stack.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, InnerNavigationController_DeletePIPMode001, TestSize.Level1)
{
    // Create navigation pattern
    auto navigationPattern = AceType::MakeRefPtr<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);

    // Create navigation stack
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    ASSERT_NE(navigationStack, nullptr);
    navigationPattern->SetNavigationStack(navigationStack);

    // Create InnerNavigationController using std::make_unique since it's not a Referenced subclass
    auto controller = std::make_unique<InnerNavigationController>(navigationPattern, 1);
    ASSERT_NE(controller, nullptr);

    // Test DeletePIPMode
    int32_t handle = 123;
    controller->DeletePIPMode(handle);
    SUCCEED();
}

/**
 * @tc.name: NavBarNode_InitSoftTransitionPop001
 * @tc.desc: Test InitSoftTransitionPop function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, NavBarNode_InitSoftTransitionPop001, TestSize.Level1)
{
    // Create NavBarNode
    auto navBarNode = NavBarNode::GetOrCreateNavBarNode(V2::NAVBAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    ASSERT_NE(navBarNode, nullptr);

    // Test InitSoftTransitionPop
    navBarNode->InitSoftTransitionPop();

    // Verify the function executed without crashing
    auto renderContext = navBarNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
}

/**
 * @tc.name: NavBarNode_SoftTransitionPushAction001
 * @tc.desc: Test SoftTransitionPushAction with isStart = true.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, NavBarNode_SoftTransitionPushAction001, TestSize.Level1)
{
    // Create NavBarNode
    auto navBarNode = NavBarNode::GetOrCreateNavBarNode(V2::NAVBAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    ASSERT_NE(navBarNode, nullptr);

    // Test SoftTransitionPushAction with isStart = true
    navBarNode->SoftTransitionPushAction(true);

    // Verify the function executed without crashing
    auto renderContext = navBarNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
}

/**
 * @tc.name: NavBarNode_SoftTransitionPushAction002
 * @tc.desc: Test SoftTransitionPushAction with isStart = false.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, NavBarNode_SoftTransitionPushAction002, TestSize.Level1)
{
    // Create NavBarNode
    auto navBarNode = NavBarNode::GetOrCreateNavBarNode(V2::NAVBAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    ASSERT_NE(navBarNode, nullptr);

    // Test SoftTransitionPushAction with isStart = false
    navBarNode->SoftTransitionPushAction(false);

    // Verify the function executed without crashing
    auto renderContext = navBarNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
}

/**
 * @tc.name: NavBarNode_StartSoftTransitionPush001
 * @tc.desc: Test StartSoftTransitionPush function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, NavBarNode_StartSoftTransitionPush001, TestSize.Level1)
{
    // Create NavBarNode
    auto navBarNode = NavBarNode::GetOrCreateNavBarNode(V2::NAVBAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    ASSERT_NE(navBarNode, nullptr);

    // Test StartSoftTransitionPush
    navBarNode->StartSoftTransitionPush();

    // Verify the function executed without crashing
    auto renderContext = navBarNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
}

/**
 * @tc.name: NavBarNode_StartSoftTransitionPop001
 * @tc.desc: Test StartSoftTransitionPop function.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, NavBarNode_StartSoftTransitionPop001, TestSize.Level1)
{
    // Create NavBarNode
    auto navBarNode = NavBarNode::GetOrCreateNavBarNode(V2::NAVBAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    ASSERT_NE(navBarNode, nullptr);

    // Test StartSoftTransitionPop
    navBarNode->StartSoftTransitionPop();

    // Verify the function executed without crashing
    auto renderContext = navBarNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
}

/**
 * @tc.name: TitleBarNode_MarkIsInitialTitle001
 * @tc.desc: Test MarkIsInitialTitle with true and false.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationPatternTestSevenNg, TitleBarNode_MarkIsInitialTitle001, TestSize.Level1)
{
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(V2::TITLE_BAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    ASSERT_NE(titleBarNode, nullptr);
    auto pattern = titleBarNode->GetPattern<TitleBarPattern>();
    ASSERT_NE(pattern, nullptr);

    titleBarNode->MarkIsInitialTitle(true);
    EXPECT_TRUE(pattern->isInitialTitle_);

    titleBarNode->MarkIsInitialTitle(false);
    EXPECT_FALSE(pattern->isInitialTitle_);
}
} // namespace OHOS::Ace::NG

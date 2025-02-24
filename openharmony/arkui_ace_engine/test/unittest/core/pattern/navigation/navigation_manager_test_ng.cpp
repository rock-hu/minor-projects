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
#include "core/components_ng/manager/navigation/navigation_manager.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "test/mock/core/common/mock_container.h"
#include "test/mock/core/pipeline/mock_pipeline_context.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string NAVIGATION_ID1 = "Navigation1";
const std::string PAGE1 = "Page1";
const std::string PARAM1 = "Param1";
} // namespace

class NavigationManagerTestNg : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
};

void NavigationManagerTestNg::SetUpTestSuite()
{
    MockPipelineContext::SetUp();
    MockContainer::SetUp();
}

void NavigationManagerTestNg::TearDownTestSuite()
{
    MockPipelineContext::TearDown();
    MockContainer::TearDown();
}

/**
 * @tc.name: RemoveNavigationDumpCallback001
 * @tc.desc: Branch: if (it != dumpMap_.end()) = true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, RemoveNavigationDumpCallback001, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_NE(navigationManager, nullptr);
    int32_t nodeId = 101;
    int32_t depth = 1;
    navigationManager->AddNavigationDumpCallback(nodeId, depth, [](int depth) {});

    navigationManager->RemoveNavigationDumpCallback(nodeId, depth);
    EXPECT_EQ(navigationManager->dumpMap_.size(), 0);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: RemoveNavigationDumpCallback002
 * @tc.desc: Branch: if (it != dumpMap_.end()) = false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, RemoveNavigationDumpCallback002, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_NE(navigationManager, nullptr);
    int32_t nodeId = 101;
    int32_t depth = 1;
    navigationManager->AddNavigationDumpCallback(nodeId, depth, [](int depth) {});

    navigationManager->RemoveNavigationDumpCallback(nodeId + 1, depth + 1);
    EXPECT_EQ(navigationManager->dumpMap_.size(), 1);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: GetNavigationInfo001
 * @tc.desc: Branch: if (customNode) = true
 *           Branch: if (!current) = false
 *           Branch: if (!current) = false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, GetNavigationInfo001, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto customNode = CustomNode::CreateCustomNode(ElementRegister::GetInstance()->MakeUniqueId(), V2::TEXT_ETS_TAG);
    customNode->SetNavigationNode(AceType::WeakClaim(AceType::RawPtr(navigationGroupNode)));
    navigationGroupNode->propInspectorId_ = NAVIGATION_ID1;

    auto pipelineContext = customNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_NE(navigationManager, nullptr);
    auto navigationInfo = navigationManager->GetNavigationInfo(customNode);
    ASSERT_NE(navigationInfo, nullptr);
    EXPECT_EQ(navigationInfo->navigationId, NAVIGATION_ID1);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: GetNavigationInfo002
 * @tc.desc: Branch: if (customNode) = false
 *           Branch: if (!current) = true
 *           Branch: if (current->GetTag() == V2::NAVIGATION_VIEW_ETS_TAG) = false
 *           Branch: if (current->GetTag() == V2::NAVIGATION_VIEW_ETS_TAG) = true
 *           Branch: if (!current) = false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, GetNavigationInfo002, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    navigationGroupNode->propInspectorId_ = NAVIGATION_ID1;
    auto navBarNode = NavBarNode::GetOrCreateNavBarNode(V2::NAVBAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    navBarNode->MountToParent(navigationGroupNode);

    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_NE(navigationManager, nullptr);
    auto navigationInfo = navigationManager->GetNavigationInfo(navBarNode);
    ASSERT_NE(navigationInfo, nullptr);
    EXPECT_EQ(navigationInfo->navigationId, NAVIGATION_ID1);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: GetNavigationInfo003
 * @tc.desc: Branch: if (customNode) = false
 *           Branch: if (!current) = true
 *           Branch: if (!current) = true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, GetNavigationInfo003, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    navigationGroupNode->propInspectorId_ = NAVIGATION_ID1;
    auto navBarNode = NavBarNode::GetOrCreateNavBarNode(V2::NAVBAR_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavBarPattern>(); });

    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_NE(navigationManager, nullptr);
    auto navigationInfo = navigationManager->GetNavigationInfo(navBarNode);
    EXPECT_EQ(navigationInfo, nullptr);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: AddInteractiveAnimation001
 * @tc.desc: Branch: if (!isInteractive_) = true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, AddInteractiveAnimation001, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    int32_t navigationNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        navigationNodeId, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    navigationGroupNode->propInspectorId_ = NAVIGATION_ID1;
    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_NE(navigationManager, nullptr);
    navigationManager->isInteractive_ = true;
    navigationManager->interactiveAnimationId_ = navigationNodeId;
    navigationPattern->proxyList_.push_back(AceType::MakeRefPtr<NavigationTransitionProxy>());
    auto navigationProxy = navigationPattern->GetTopNavigationProxy();
    ASSERT_NE(navigationProxy, nullptr);
    navigationProxy->interactive_ = true;

    bool ret = navigationManager->AddInteractiveAnimation([]() {});
    EXPECT_EQ(ret, true);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: AddInteractiveAnimation002
 * @tc.desc: Branch: if (!isInteractive_) = false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, AddInteractiveAnimation002, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    int32_t navigationNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        navigationNodeId, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    navigationGroupNode->propInspectorId_ = NAVIGATION_ID1;
    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_NE(navigationManager, nullptr);
    navigationManager->isInteractive_ = false;
    navigationManager->interactiveAnimationId_ = navigationNodeId;
    navigationPattern->proxyList_.push_back(AceType::MakeRefPtr<NavigationTransitionProxy>());
    auto navigationProxy = navigationPattern->GetTopNavigationProxy();
    ASSERT_NE(navigationProxy, nullptr);
    navigationProxy->interactive_ = true;

    bool ret = navigationManager->AddInteractiveAnimation([]() {});
    EXPECT_EQ(ret, false);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: StorageNavigationRecoveryInfo001
 * @tc.desc: Branch: if (!allNavigationInfo || !allNavigationInfo->IsArray()) = false
 *           Condition: !allNavigationInfo = false, !allNavigationInfo->IsArray() = false
 *           Branch: if (!stackInfo->IsArray()) = false
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, StorageNavigationRecoveryInfo001, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto allNavigationInfo = JsonUtil::CreateArray(true);
    auto navigationInfo = JsonUtil::Create(true);
    navigationInfo->Put("id", NAVIGATION_ID1.c_str());
    auto allNavdestinationInfo = JsonUtil::CreateArray(true);
    auto navdestinationInfo = JsonUtil::Create(true);
    navdestinationInfo->Put("name", PAGE1.c_str());
    navdestinationInfo->Put("param", PARAM1.c_str());
    navdestinationInfo->Put("mode", 0);
    allNavdestinationInfo->Put(navdestinationInfo);
    navigationInfo->Put("stack", allNavdestinationInfo);
    allNavigationInfo->Put(navigationInfo);

    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    navigationManager->StorageNavigationRecoveryInfo(std::move(allNavigationInfo));
    auto recoveryInfo = navigationManager->navigationRecoveryInfo_[NAVIGATION_ID1];
    EXPECT_EQ(recoveryInfo.size(), 1);
    EXPECT_EQ(recoveryInfo[0].name, PAGE1);
    EXPECT_EQ(recoveryInfo[0].param, PARAM1);
    EXPECT_EQ(recoveryInfo[0].mode, 0);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: StorageNavigationRecoveryInfo002
 * @tc.desc: Branch: if (!allNavigationInfo || !allNavigationInfo->IsArray()) = false
 *           Condition: !allNavigationInfo = false, !allNavigationInfo->IsArray() = false
 *           Branch: if (!stackInfo->IsArray()) = true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, StorageNavigationRecoveryInfo002, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto allNavigationInfo = JsonUtil::CreateArray(true);
    auto navigationInfo = JsonUtil::Create(true);
    navigationInfo->Put("id", NAVIGATION_ID1.c_str());
    auto allNavdestinationInfo = JsonUtil::Create(true);
    navigationInfo->Put("stack", allNavdestinationInfo);
    allNavigationInfo->Put(navigationInfo);

    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    navigationManager->StorageNavigationRecoveryInfo(std::move(allNavigationInfo));
    auto recoveryInfo = navigationManager->navigationRecoveryInfo_[NAVIGATION_ID1];
    EXPECT_EQ(recoveryInfo.size(), 0);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: StorageNavigationRecoveryInfo003
 * @tc.desc: Branch: if (!allNavigationInfo || !allNavigationInfo->IsArray()) = true
 *           Condition: !allNavigationInfo = false, !allNavigationInfo->IsArray() = true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, StorageNavigationRecoveryInfo003, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto allNavigationInfo = JsonUtil::Create(true);

    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    navigationManager->StorageNavigationRecoveryInfo(std::move(allNavigationInfo));
    EXPECT_EQ(navigationManager->navigationRecoveryInfo_[NAVIGATION_ID1].size(), 0);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: StorageNavigationRecoveryInfo004
 * @tc.desc: Branch: if (!allNavigationInfo || !allNavigationInfo->IsArray()) = true
 *           Condition: !allNavigationInfo = true
 * @tc.type: FUNC
 */
HWTEST_F(NavigationManagerTestNg, StorageNavigationRecoveryInfo004, TestSize.Level1)
{
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));

    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    navigationManager->StorageNavigationRecoveryInfo(std::move(nullptr));
    EXPECT_EQ(navigationManager->navigationRecoveryInfo_[NAVIGATION_ID1].size(), 0);
    NavigationManagerTestNg::TearDownTestSuite();
}

/**
 * @tc.name: NavigationMapsTest001
 * @tc.desc: Test AddNavigation/RemoveNavigation/FindNavigationInTargetParent
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(NavigationManagerTestNg, NavigationMapsTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get navigation manager
     */
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_TRUE(navigationManager->navigationMaps_.empty());

    /**
     * @tc.steps: step2. configure parameters .
     */
    int32_t parentId1 = 1;
    int32_t navigationId1 = 11;
    int32_t parentId2 = 2;
    int32_t navigationId2 = 21;
    int32_t parentId3 = 3;
    int32_t navigationId3 = 31;
    int32_t navigationId4 = 41;

    /**
     * @tc.steps: step3. Register navigation to manager.
     */
    navigationManager->AddNavigation(parentId1, navigationId1);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 1);
    navigationManager->AddNavigation(parentId2, navigationId2);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 2);
    navigationManager->AddNavigation(parentId3, navigationId3);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 3);
    navigationManager->AddNavigation(parentId3, navigationId4);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 3);

    /**
     * @tc.steps: step4. Find navigation in target parent.
     */
    std::vector<int32_t> result = navigationManager->FindNavigationInTargetParent(parentId1);
    std::vector<int32_t> expected = {navigationId1};
    EXPECT_EQ(result, expected);
    result = navigationManager->FindNavigationInTargetParent(parentId2);
    expected = {navigationId2};
    EXPECT_EQ(result, expected);
    result = navigationManager->FindNavigationInTargetParent(parentId3);
    expected = {navigationId3, navigationId4};
    EXPECT_EQ(result, expected);
}

/**
 * @tc.name: NavigationMapsTest002
 * @tc.desc: Test AddNavigation/RemoveNavigation/FindNavigationInTargetParent
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(NavigationManagerTestNg, NavigationMapsTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. get navigation manager
     */
    NavigationManagerTestNg::SetUpTestSuite();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(V2::NAVIGATION_VIEW_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
    auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
    ASSERT_NE(navigationPattern, nullptr);
    navigationPattern->SetNavigationStack(std::move(navigationStack));
    auto pipelineContext = navigationGroupNode->GetContext();
    ASSERT_NE(pipelineContext, nullptr);
    auto navigationManager = pipelineContext->GetNavigationManager();
    ASSERT_TRUE(navigationManager->navigationMaps_.empty());

    /**
     * @tc.steps: step2. configure parameters .
     */
    int32_t parentId1 = 1;
    int32_t navigationId1 = 11;
    int32_t parentId2 = 2;
    int32_t navigationId2 = 21;
    int32_t parentId3 = 3;
    int32_t navigationId3 = 31;
    int32_t navigationId4 = 41;

    /**
     * @tc.steps: step3. Register navigation to manager.
     */
    navigationManager->AddNavigation(parentId1, navigationId1);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 1);
    navigationManager->AddNavigation(parentId2, navigationId2);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 2);
    navigationManager->AddNavigation(parentId3, navigationId3);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 3);
    navigationManager->AddNavigation(parentId3, navigationId4);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 3);

    /**
     * @tc.steps: step4. remove navigation to manager.
     */
    navigationManager->RemoveNavigation(navigationId1);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 2);

    navigationManager->RemoveNavigation(navigationId2);
    ASSERT_EQ(navigationManager->navigationMaps_.size(), 1);
}
} // namespace OHOS::Ace::NG
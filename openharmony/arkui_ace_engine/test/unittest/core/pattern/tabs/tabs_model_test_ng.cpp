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

#include "gtest/gtest.h"
#include "tabs_test_ng.h"
#include "ui/base/ace_type.h"

#include "core/components_ng/pattern/tabs/tabs_model_ng.h"
#include "core/components_ng/pattern/tabs/tabs_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"

namespace OHOS::Ace::NG {

class TabsModelTestNg : public TabsTestNg {
public:
};

/**
 * @tc.name: SetTabBarIndex001
 * @tc.desc: Test TabsModelNG SetTabBarIndex
 * @tc.type: FUNC
 */
HWTEST_F(TabsModelTestNg, SetTabBarIndex001, TestSize.Level1)
{
    TabsModelNG model = CreateTabs();
    CreateTabContents();
    CreateTabsDone(model);

    RefPtr<TabsPattern> tabsPattern = AceType::MakeRefPtr<TabsPattern>();
    RefPtr<FrameNode> tabsNode = AceType::MakeRefPtr<TabsNode>(V2::TABS_ETS_TAG, 2, tabsPattern, true);
    ASSERT_NE(tabsNode, nullptr);
    auto frameNode = AceType::RawPtr(tabsNode);
    RefPtr<TabsLayoutProperty> tabsLayoutProperty = AceType::MakeRefPtr<TabsLayoutProperty>();
    tabsLayoutProperty->Reset();
    int32_t index = 2;
    tabsLayoutProperty->propIndex_ = index;
    tabsNode->layoutProperty_ = tabsLayoutProperty;
    tabsPattern->frameNode_ = tabsNode;
    model.SetTabBarIndex(frameNode, 2);
    EXPECT_FALSE(tabsLayoutProperty->propIndexSetByUser_.has_value());
}

/**
 * @tc.name: SetTabBarIndex002
 * @tc.desc: Test TabsModelNG SetTabBarIndex
 * @tc.type: FUNC
 */
HWTEST_F(TabsModelTestNg, SetTabBarIndex002, TestSize.Level1)
{
    TabsModelNG model = CreateTabs();
    CreateTabContents();
    CreateTabsDone(model);

    RefPtr<TabsPattern> tabsPattern = AceType::MakeRefPtr<TabsPattern>();
    RefPtr<FrameNode> tabsNode = AceType::MakeRefPtr<TabsNode>(V2::TABS_ETS_TAG, 2, tabsPattern, true);
    ASSERT_NE(tabsNode, nullptr);
    auto frameNode = AceType::RawPtr(tabsNode);
    RefPtr<TabsLayoutProperty> tabsLayoutProperty = AceType::MakeRefPtr<TabsLayoutProperty>();
    tabsLayoutProperty->Reset();
    int32_t index = -1;
    tabsLayoutProperty->propIndex_ = index;
    tabsNode->layoutProperty_ = tabsLayoutProperty;
    tabsPattern->frameNode_ = tabsNode;
    model.SetTabBarIndex(frameNode, -1);
    EXPECT_FALSE(tabsLayoutProperty->propIndexSetByUser_.has_value());
}

/**
 * @tc.name: SetTabBarIndex003
 * @tc.desc: Test TabsModelNG SetTabBarIndex
 * @tc.type: FUNC
 */
HWTEST_F(TabsModelTestNg, SetTabBarIndex003, TestSize.Level1)
{
    TabsModelNG model = CreateTabs();
    CreateTabContents();
    CreateTabsDone(model);

    RefPtr<TabsPattern> tabsPattern = AceType::MakeRefPtr<TabsPattern>();
    RefPtr<FrameNode> tabsNode = AceType::MakeRefPtr<TabsNode>(V2::TABS_ETS_TAG, 2, tabsPattern, true);
    ASSERT_NE(tabsNode, nullptr);
    auto frameNode = AceType::RawPtr(tabsNode);
    RefPtr<TabsLayoutProperty> tabsLayoutProperty = AceType::MakeRefPtr<TabsLayoutProperty>();
    tabsLayoutProperty->Reset();
    int32_t index = 2;
    tabsLayoutProperty->propIndex_ = index;
    tabsNode->layoutProperty_ = tabsLayoutProperty;
    tabsPattern->frameNode_ = tabsNode;
    model.SetTabBarIndex(frameNode, -1);
    EXPECT_FALSE(tabsLayoutProperty->propIndexSetByUser_.has_value());
}

/**
 * @tc.name: SetTabBarIndex004
 * @tc.desc: Test TabsModelNG SetTabBarIndex
 * @tc.type: FUNC
 */
HWTEST_F(TabsModelTestNg, SetTabBarIndex004, TestSize.Level1)
{
    TabsModelNG model = CreateTabs();
    CreateTabContents();
    CreateTabsDone(model);

    RefPtr<TabsPattern> tabsPattern = AceType::MakeRefPtr<TabsPattern>();
    RefPtr<FrameNode> tabsNode = AceType::MakeRefPtr<TabsNode>(V2::TABS_ETS_TAG, 2, tabsPattern, true);
    ASSERT_NE(tabsNode, nullptr);
    auto frameNode = AceType::RawPtr(tabsNode);
    RefPtr<TabsLayoutProperty> tabsLayoutProperty = AceType::MakeRefPtr<TabsLayoutProperty>();
    tabsLayoutProperty->Reset();
    int32_t index = 2;
    tabsLayoutProperty->propIndex_ = index;
    tabsNode->layoutProperty_ = tabsLayoutProperty;
    tabsPattern->frameNode_ = tabsNode;
    model.SetTabBarIndex(frameNode, 1);
    EXPECT_EQ(tabsLayoutProperty->propIndexSetByUser_.value(), 1);
}

/**
 * @tc.name: SetTabBarIndex005
 * @tc.desc: Test TabsModelNG SetTabBarIndex
 * @tc.type: FUNC
 */
HWTEST_F(TabsModelTestNg, SetTabBarIndex005, TestSize.Level1)
{
    TabsModelNG model = CreateTabs();
    CreateTabContents();
    CreateTabsDone(model);

    RefPtr<TabsPattern> tabsPattern = AceType::MakeRefPtr<TabsPattern>();
    RefPtr<FrameNode> tabsNode = AceType::MakeRefPtr<TabsNode>(V2::TABS_ETS_TAG, 2, tabsPattern, true);
    ASSERT_NE(tabsNode, nullptr);
    auto frameNode = AceType::RawPtr(tabsNode);
    RefPtr<TabsLayoutProperty> tabsLayoutProperty = AceType::MakeRefPtr<TabsLayoutProperty>();
    tabsLayoutProperty->Reset();
    tabsNode->layoutProperty_ = tabsLayoutProperty;
    tabsPattern->frameNode_ = tabsNode;
    model.SetTabBarIndex(frameNode, 4);
    EXPECT_EQ(tabsLayoutProperty->propIndexSetByUser_.value(), 4);
}

/**
 * @tc.name: SetBarBackgroundEffect001
 * @tc.desc: Test TabsModelNG SetBarBackgroundEffect
 * @tc.type: FUNC
 */
HWTEST_F(TabsModelTestNg, SetBarBackgroundEffect001, TestSize.Level1)
{
    TabsModelNG model = CreateTabs();
    CreateTabContents();
    CreateTabsDone(model);

    RefPtr<TabsPattern> tabsPattern = AceType::MakeRefPtr<TabsPattern>();
    RefPtr<FrameNode> tabsNode = AceType::MakeRefPtr<TabsNode>(V2::TABS_ETS_TAG, 2, tabsPattern, true);
    ASSERT_NE(tabsNode, nullptr);
    auto node = FrameNode::CreateFrameNode(V2::TABS_COMPONENT_TAG, 1, tabsPattern);
    ASSERT_NE(node, nullptr);
    int32_t nodeId = 1;
    node->nodeId_ = nodeId;
    RefPtr<PipelineContext> pipe = AceType::MakeRefPtr<PipelineContext>();
    pipe->onWindowFocusChangedCallbacks_.clear();
    pipe->AddWindowFocusChangedCallback(node->GetId());
    node->context_ = AceType::RawPtr(pipe);
    tabsNode->children_.clear();
    tabsNode->children_.emplace_back(node);
    tabsNode->children_.emplace_back(node);
    tabsNode->children_.emplace_back(node);
    tabsNode->children_.emplace_back(node);
    auto frameNode = AceType::RawPtr(tabsNode);
    RefPtr<TabsLayoutProperty> tabsLayoutProperty = AceType::MakeRefPtr<TabsLayoutProperty>();
    tabsLayoutProperty->Reset();
    tabsNode->layoutProperty_ = tabsLayoutProperty;
    tabsPattern->frameNode_ = tabsNode;
    EffectOption effectOption;
    effectOption.policy = BlurStyleActivePolicy::FOLLOWS_WINDOW_ACTIVE_STATE;
    model.SetBarBackgroundEffect(frameNode, effectOption);
    EXPECT_TRUE(pipe->onWindowFocusChangedCallbacks_.empty());
}

/**
 * @tc.name: SetBarBackgroundEffect002
 * @tc.desc: Test TabsModelNG SetBarBackgroundEffect
 * @tc.type: FUNC
 */
HWTEST_F(TabsModelTestNg, SetBarBackgroundEffect002, TestSize.Level1)
{
    TabsModelNG model = CreateTabs();
    CreateTabContents();
    CreateTabsDone(model);

    RefPtr<TabsPattern> tabsPattern = AceType::MakeRefPtr<TabsPattern>();
    RefPtr<FrameNode> tabsNode = AceType::MakeRefPtr<TabsNode>(V2::TABS_ETS_TAG, 2, tabsPattern, true);
    ASSERT_NE(tabsNode, nullptr);
    auto node = FrameNode::CreateFrameNode(V2::TABS_COMPONENT_TAG, 2, tabsPattern);
    ASSERT_NE(node, nullptr);
    int32_t nodeId = 2;
    node->nodeId_ = nodeId;
    RefPtr<PipelineContext> pipe = AceType::MakeRefPtr<PipelineContext>();
    pipe->onWindowFocusChangedCallbacks_.clear();
    pipe->onWindowFocusChangedCallbacks_.emplace(nodeId);
    pipe->RemoveWindowFocusChangedCallback(node->GetId());
    node->context_ = AceType::RawPtr(pipe);
    tabsNode->children_.clear();
    tabsNode->children_.emplace_back(node);
    tabsNode->children_.emplace_back(node);
    tabsNode->children_.emplace_back(node);
    tabsNode->children_.emplace_back(node);
    auto frameNode = AceType::RawPtr(tabsNode);
    RefPtr<TabsLayoutProperty> tabsLayoutProperty = AceType::MakeRefPtr<TabsLayoutProperty>();
    tabsLayoutProperty->Reset();
    tabsNode->layoutProperty_ = tabsLayoutProperty;
    tabsPattern->frameNode_ = tabsNode;
    EffectOption effectOption;
    effectOption.policy = BlurStyleActivePolicy::ALWAYS_ACTIVE;
    model.SetBarBackgroundEffect(frameNode, effectOption);
    EXPECT_FALSE(pipe->onWindowFocusChangedCallbacks_.empty());
}
} // namespace OHOS::Ace::NG
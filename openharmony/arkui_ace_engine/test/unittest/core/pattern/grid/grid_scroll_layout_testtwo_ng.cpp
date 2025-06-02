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

#include "grid_test_ng.h"

#include "core/components_ng/pattern/grid/grid_scroll/grid_scroll_layout_algorithm.h"
#include "core/components_ng/pattern/grid/grid_scroll/grid_scroll_with_options_layout_algorithm.h"

namespace OHOS::Ace::NG {
class GridScrollLayoutTestNg : public GridTestNg {
};

/**
 * @tc.name: GetStartingItem001
 * @tc.desc: Test GetStartingItem in GridScrollWithOptionsLayoutAlgorithm
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, GetStartingItem001, TestSize.Level1)
{
    GridLayoutOptions option;
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr");
    model.SetLayoutOptions(option);
    model.SetEdgeEffect(EdgeEffect::SPRING, true);
    CreateFixedHeightItems(600, ITEM_MAIN_SIZE);
    CreateDone();

    ScrollToIndex(99, false, ScrollAlign::END);
    FlushUITasks();

    auto layoutAlgorithmWrapper = AceType::DynamicCast<LayoutAlgorithmWrapper>(frameNode_->GetLayoutAlgorithm());
    auto layoutAlgorithm =
        AceType::DynamicCast<GridScrollWithOptionsLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());

    layoutAlgorithm->info_.hasBigItem_ = true;
    EXPECT_EQ(layoutAlgorithm->GetStartingItem(AceType::RawPtr(frameNode_), 99), 99);
}

/**
 * @tc.name: ShowCache001
 * @tc.desc: Test show cache in Grid
 * @tc.type: FUNC
 */
HWTEST_F(GridScrollLayoutTestNg, ShowCache001, TestSize.Level1)
{
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr 1fr");
    model.SetCachedCount(2, true);
    CreateBigItem(0, 0, 0, 2, NULL_VALUE, ITEM_MAIN_SIZE);
    CreateFixedHeightItems(14, ITEM_MAIN_SIZE);
    CreateDone();

    /**
     * @tc.steps: step1. Add item
     * @tc.expected: The added item in the correct position
     */
    for (int32_t i = 0; i < 5; i++) {
        GridItemModelNG itemModel;
        itemModel.Create(GridItemStyle::NONE);
        ViewAbstract::SetHeight(CalcLength(Dimension(ITEM_MAIN_SIZE)));
        RefPtr<UINode> currentNode = ViewStackProcessor::GetInstance()->Finish();
        auto currentFrameNode = AceType::DynamicCast<FrameNode>(currentNode);
        currentFrameNode->MountToParent(frameNode_);
    }
    frameNode_->ChildrenUpdatedFrom(17);
    frameNode_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    FlushUITasks();

    for (int i = 0; i <= 5; i++) {
        UpdateCurrentOffset(-15);
        FlushUITasks();
    }

    std::map<int32_t, std::map<int32_t, int32_t>> gridMatrix = { { 0, { { 0, 0 }, { 1, 0 }, { 2, 0 } } },
        { 1, { { 0, 1 }, { 1, 2 }, { 2, 3 } } }, { 2, { { 0, 4 }, { 1, 5 }, { 2, 6 } } },
        { 3, { { 0, 7 }, { 1, 8 }, { 2, 9 } } }, { 4, { { 0, 10 }, { 1, 11 }, { 2, 12 } } },
        { 5, { { 0, 13 }, { 1, 14 }, { 2, 15 } } }, { 6, { { 0, 16 }, { 1, 17 }, { 2, 18 } } } };
    EXPECT_EQ(pattern_->info_.gridMatrix_, gridMatrix);
}
} // namespace OHOS::Ace::NG
 
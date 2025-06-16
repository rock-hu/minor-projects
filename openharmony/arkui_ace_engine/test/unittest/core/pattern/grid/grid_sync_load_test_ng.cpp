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
#include "test/mock/core/pipeline/mock_pipeline_context.h"

namespace OHOS::Ace::NG {
class GridSyncLoadTestNg : public GridTestNg {
public:
    void TearDown() override;
    GridItemModelNG CreateGridItemForSyncLoad(float width, float height, GridItemStyle gridItemStyle);
    void CreateGridItems(int32_t itemNumber);
};

void GridSyncLoadTestNg::TearDown()
{
    GridTestNg::TearDown();
    MockPipelineContext::GetCurrent()->SetResponseTime(INT32_MAX);
}

GridItemModelNG GridSyncLoadTestNg::CreateGridItemForSyncLoad(float width, float height, GridItemStyle gridItemStyle)
{
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(GetElmtId());
    GridItemModelNG itemModel;
    itemModel.Create(gridItemStyle);
    ViewAbstract::SetWidth(CalcLength(width));
    ViewAbstract::SetHeight(CalcLength(height));
    ViewAbstract::SetFocusable(true);
    RefPtr<UINode> element = ViewStackProcessor::GetInstance()->GetMainElementNode();
    auto frameNode = AceType::DynamicCast<FrameNode>(element);
    frameNode->measureCallback_ = [](RefPtr<Kit::FrameNode>& node) {
        NG::MockPipelineContext::GetCurrent()->DecResponseTime();
    };
    return itemModel;
}

void GridSyncLoadTestNg::CreateGridItems(int32_t itemNumber)
{
    for (int32_t index = 0; index < itemNumber; index++) {
        CreateGridItemForSyncLoad(ITEM_MAIN_SIZE, ITEM_MAIN_SIZE, GridItemStyle::NONE);
        ViewStackProcessor::GetInstance()->Pop();
        ViewStackProcessor::GetInstance()->StopGetAccessRecording();
    }
}

/**
 * @tc.name: SyncLoad001
 * @tc.desc: Test Grid sync load
 * @tc.type: FUNC
 */
HWTEST_F(GridSyncLoadTestNg, SyncLoad001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Grid
     * @tc.expected: Grid load 2 item(just one line) in first frame
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetSyncLoad(false);
    CreateGridItems(10);
    MockPipelineContext::GetCurrent()->SetResponseTime(2);
    CreateDone();
    EXPECT_EQ(pattern_->info_.endIndex_, 1);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 0);
    EXPECT_TRUE(frameNode_->isLayoutDirtyMarked_);

    /**
     * @tc.steps: step1. Flush next frame
     * @tc.expected: Grid load 1 item(less than one line) in the second frame
     */
    MockPipelineContext::GetCurrent()->SetResponseTime(1);
    MockPipelineContext::GetCurrent()->FlushUITaskWithSingleDirtyNode(frameNode_);
    EXPECT_EQ(pattern_->info_.endIndex_, 2);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 1);
}

/**
 * @tc.name: SyncLoad002
 * @tc.desc: Test Grid sync load
 * @tc.type: FUNC
 */
HWTEST_F(GridSyncLoadTestNg, SyncLoad002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Grid
     * @tc.expected: Grid load 1 item(less than one line) in first frame
     */
    GridModelNG model = CreateGrid();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetSyncLoad(false);
    CreateGridItems(10);
    MockPipelineContext::GetCurrent()->SetResponseTime(1);
    CreateDone();
    EXPECT_EQ(pattern_->info_.endIndex_, 0);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 0);
    EXPECT_TRUE(frameNode_->isLayoutDirtyMarked_);

    /**
     * @tc.steps: step1. Flush next frame
     * @tc.expected: Grid load 1 item(fill the first line) in the second frame
     */
    MockPipelineContext::GetCurrent()->SetResponseTime(1);
    MockPipelineContext::GetCurrent()->FlushUITaskWithSingleDirtyNode(frameNode_);
    EXPECT_EQ(pattern_->info_.endIndex_, 1);
    EXPECT_EQ(pattern_->info_.endMainLineIndex_, 0);
}
} // namespace OHOS::Ace::NG

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

#include "test/mock/core/render/mock_render_context.h"
#include "water_flow_test_ng.h"

namespace OHOS::Ace::NG {
/**
 * @tc.name: Cache001
 * @tc.desc: Test cache item preload
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, Cache001, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    CreateItemsInRepeat(50, [](int32_t i) { return i % 2 ? 100.0f : 200.0f; });

    model.SetCachedCount(3);
    model.SetColumnsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    CreateDone();
    auto info = pattern_->layoutInfo_;
    EXPECT_EQ(info->startIndex_, 0);
    EXPECT_EQ(info->endIndex_, 10);

    const std::list<int32_t> preloadList = { 11, 12, 13 };
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 11));
    EXPECT_EQ(pattern_->preloadItems_, preloadList);
    EXPECT_TRUE(pattern_->cacheLayout_);
    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    EXPECT_TRUE(pattern_->preloadItems_.empty());
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 11));
    EXPECT_EQ(GetChildHeight(frameNode_, 12), 200.0f);
    EXPECT_EQ(GetChildWidth(frameNode_, 13), (WATER_FLOW_WIDTH - 10.0f) / 2.0f);
    EXPECT_EQ(layoutProperty_->GetPropertyChangeFlag(), PROPERTY_UPDATE_LAYOUT);

    UpdateCurrentOffset(-500.0f);
    EXPECT_EQ(info->startIndex_, 4);
    EXPECT_EQ(info->endIndex_, 17);
    EXPECT_EQ(GetChildY(frameNode_, 3), -290.0f);
    EXPECT_EQ(GetChildY(frameNode_, 2), -390.0f);
    EXPECT_EQ(GetChildY(frameNode_, 1), -500.0f);
    const std::list<int32_t> preloadList2 = { 18, 19, 20 };
    EXPECT_EQ(pattern_->preloadItems_, preloadList2);
    PipelineContext::GetCurrentContext()->OnIdle(GetSysTimestamp());
    EXPECT_EQ(pattern_->preloadItems_, preloadList2);
}

/**
 * @tc.name: Cache004
 * @tc.desc: Test cache item reaching deadline
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, Cache004, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    CreateItemsInRepeat(50, [](int32_t i) { return i % 2 ? 100.0f : 200.0f; });

    model.SetCachedCount(3);
    model.SetColumnsTemplate("1fr 1fr");
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    CreateDone();
    auto info = pattern_->layoutInfo_;
    EXPECT_EQ(info->startIndex_, 0);
    EXPECT_EQ(info->endIndex_, 10);

    const std::list<int32_t> preloadList = { 11, 12, 13 };
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 11));
    EXPECT_EQ(pattern_->preloadItems_, preloadList);
    EXPECT_TRUE(pattern_->cacheLayout_);
    // later expand to fuzz test
    PipelineContext::GetCurrentContext()->OnIdle(100000);
    // items still in preload list should not be created
    for (auto&& item : pattern_->preloadItems_) {
        EXPECT_FALSE(GetChildFrameNode(frameNode_, item));
    }
    for (auto&& itemIdx : preloadList) {
        // check preloaded items
        if (!pattern_->preloadItems_.empty() && itemIdx == *pattern_->preloadItems_.begin()) {
            break;
        }
        EXPECT_TRUE(GetChildFrameNode(frameNode_, itemIdx));
        EXPECT_EQ(GetChildHeight(frameNode_, itemIdx), itemIdx % 2 ? 100.0f : 200.0f);
        EXPECT_EQ(GetChildWidth(frameNode_, itemIdx), (WATER_FLOW_WIDTH - 10.0f) / 2.0f);
    }
    if (pattern_->preloadItems_.size() != preloadList.size()) {
        EXPECT_EQ(layoutProperty_->GetPropertyChangeFlag(), PROPERTY_UPDATE_LAYOUT);
    }
}

/**
 * @tc.name: Cache005
 * @tc.desc: Test items in preloadList when Waterflow's height is changed to 0.
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, Cache005, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    CreateItemsInRepeat(50, [](int32_t i) { return 100.0f; });
    model.SetCachedCount(3);
    model.SetColumnsTemplate("1fr 1fr");
    CreateDone();

    pattern_->ScrollToIndex(10);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 10);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 25);
    std::list<int32_t> preloadList = { 26, 27, 28 };
    EXPECT_EQ(pattern_->preloadItems_, preloadList);

    // change height to 0.0f.
    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(CalcLength(500.0f), CalcLength(Dimension(0.0f))));
    FlushLayoutTask(frameNode_);
    EXPECT_TRUE(IsEqual(frameNode_->GetGeometryNode()->GetFrameRect(), RectF(0, 0, 500.0f, 0)));
    EXPECT_TRUE(pattern_->PreloadListEmpty());
}

/**
 * @tc.name: Cache006
 * @tc.desc: Test items in preloadList when Waterflow is empty.
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, Cache006, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    model.SetCachedCount(3);
    model.SetColumnsTemplate("1fr 1fr");
    CreateDone();
    EXPECT_TRUE(pattern_->PreloadListEmpty());
}

/**
 * @tc.name: Cache007
 * @tc.desc: Test layout cache items without scrolling.
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, Cache007, TestSize.Level1)
{
    WaterFlowModelNG model = CreateWaterFlow();
    model.SetColumnsTemplate("1fr 1fr");
    model.SetCachedCount(2);
    CreateItemsInLazyForEach(50, [](uint32_t idx) { return 50.0f; });
    CreateDone();

    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 31);
    EXPECT_FALSE(GetItem(32, true));
    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    auto item = GetItem(32, true);
    ASSERT_TRUE(item);
    auto ctx = AceType::DynamicCast<MockRenderContext>(item->GetRenderContext());
    FlushUITasks();
    EXPECT_EQ(item->GetGeometryNode()->GetFrameRect().ToString(), "RectT (0.00, 800.00) - [240.00 x 50.00]");
    EXPECT_EQ(ctx->paintRect_.ToString(), "RectT (0.00, 800.00) - [240.00 x 50.00]"); // should be synced with layout

    layoutProperty_->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, CalcLength(1000.0f)));
    FlushUITasks();
    EXPECT_EQ(item->GetGeometryNode()->GetFrameRect().ToString(), "RectT (0.00, 800.00) - [240.00 x 50.00]");
    EXPECT_EQ(ctx->paintRect_, item->GetGeometryNode()->GetFrameRect()); // should be synced with layout
    EXPECT_EQ(GetChildFrameNode(frameNode_, 32), item);
    EXPECT_EQ(ctx, GetItem(32)->GetRenderContext());
}

/*
 * @tc.name: ShowCache004
 * @tc.desc: Test cache items active state
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, ShowCache004, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    CreateItemsInLazyForEach(50, [](uint32_t idx) { return 100.0f; });
    model.SetCachedCount(1, true);
    model.SetLayoutDirection(FlexDirection::COLUMN_REVERSE);
    model.SetColumnsTemplate("1fr 1fr");
    CreateDone();

    UpdateCurrentOffset(-300.0f);
    UpdateCurrentOffset(110.0f);
    UpdateCurrentOffset(-200.0f);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 6);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 23);
        for (int i = 0; i < 50; ++i) {
        if (i < 5 || i > 24) {
            if (GetItem(i, true)) {
                EXPECT_FALSE(GetItem(i, true)->IsActive());
            }
        } else {
            EXPECT_TRUE(GetItem(i, true));
            EXPECT_TRUE(GetItem(i, true)->IsActive());
        }
    }
}

/**
 * @tc.name: CacheScroll001
 * @tc.desc: Layout WaterFlow cache items
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, CacheScroll001, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    model.SetCachedCount(10);
    model.SetRowsGap(Dimension(10));
    model.SetColumnsGap(Dimension(10));
    CreateItemsInLazyForEach(100, [](int32_t) { return 100.0f; });
    CreateDone();
    frameNode_->AttachToMainTree(true, PipelineContext::GetCurrentContextPtrSafely());

    pattern_->isAnimationStop_ = false;
    UpdateCurrentOffset(-2000.0f);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 18);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 25);
    EXPECT_EQ(GetChildY(frameNode_, 18), -20.0f);
    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    EXPECT_TRUE(GetChildFrameNode(frameNode_, 8));
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 7));

    UpdateCurrentOffset(200.0f);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 16);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 23);
    EXPECT_FALSE(GetChildFrameNode(frameNode_, 7));
    EXPECT_EQ(GetChildY(frameNode_, 18), 180.0f);
    EXPECT_TRUE(GetItem(18)->IsOnMainTree());

    PipelineContext::GetCurrentContext()->OnIdle(INT64_MAX);
    ASSERT_TRUE(GetItem(7, true));
    EXPECT_FALSE(GetItem(7, true)->IsOnMainTree());
    EXPECT_EQ(GetItem(7, true)->GetLayoutProperty()->GetPropertyChangeFlag() & PROPERTY_UPDATE_MEASURE,
        PROPERTY_UPDATE_MEASURE);
    UpdateCurrentOffset(5.0f);
    EXPECT_FALSE(GetItem(7, true)->IsOnMainTree());
    EXPECT_EQ(GetChildLayoutProperty<LayoutProperty>(frameNode_, 7)->GetPropertyChangeFlag() & PROPERTY_UPDATE_MEASURE,
        PROPERTY_UPDATE_MEASURE);
}

/**
 * @tc.name: LazyForEachJump001
 * @tc.desc: Test jump function after changing dataSource in lazyforeach.
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, LazyForEachJump001, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    model.SetCachedCount(10);
    CreateItemsInLazyForEach(100, [](int32_t) { return 100.0f; });
    CreateDone();

    AddItemInLazyForEach(1);
    FlushUITasks();
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 0);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 7);
    EXPECT_EQ(GetChildY(frameNode_, 0), 0.0f);

    AddItemInLazyForEach(6);
    FlushUITasks();
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 0);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 7);
    EXPECT_EQ(GetChildY(frameNode_, 6), 600.0f);
    ScrollToIndex(6, false, ScrollAlign::START);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 6);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 13);
    EXPECT_FALSE(GetItem(5, true)->IsOnMainTree());

    AddItemInLazyForEach(17);
    FlushUITasks();
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 6);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 13);
    ScrollToIndex(17, false, ScrollAlign::START);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 17);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 24);
    EXPECT_EQ(GetChildY(frameNode_, 17), 0.0f);
    EXPECT_FALSE(GetItem(5, true)->IsOnMainTree());

    AddItemInLazyForEach(99);
    FlushUITasks();
    ScrollToIndex(99, false, ScrollAlign::START);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 92);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 99);
    EXPECT_EQ(frameNode_->GetTotalChildCount(), 100);
}

/**
 * @tc.name: LazyForEachJump002
 * @tc.desc: Test jump function after changing dataSource in lazyforeach.
 * @tc.type: FUNC
 */
HWTEST_F(WaterFlowTestNg, LazyForEachJump002, TestSize.Level1)
{
    auto model = CreateWaterFlow();
    model.SetCachedCount(10);
    RefPtr<WaterFlowMockLazy> mockLazy = CreateItemsInLazyForEach(100, [](int32_t) { return 100.0f; });
    CreateDone();
    frameNode_->AttachToMainTree(true, PipelineContext::GetCurrentContextPtrSafely());

    pattern_->ScrollToIndex(2, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 2);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 9);

    DeleteItemInLazyForEach(58);
    mockLazy->SetTotalCount(99);
    FlushUITasks();
    pattern_->ScrollToIndex(58, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 58);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 65);
    EXPECT_EQ(frameNode_->GetTotalChildCount(), 99);

    DeleteItemInLazyForEach(63);
    mockLazy->SetTotalCount(98);
    FlushUITasks();
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 58);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 65);
    EXPECT_EQ(GetChildY(frameNode_, 58), 0.0f);
    pattern_->ScrollToIndex(60, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 60);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 67);
    EXPECT_TRUE(GetItem(58, true)->IsOnMainTree());

    DeleteItemInLazyForEach(52);
    mockLazy->SetTotalCount(97);
    FlushUITasks();
    pattern_->ScrollToIndex(49, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 49);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 56);

    DeleteItemInLazyForEach(0);
    mockLazy->SetTotalCount(96);
    FlushUITasks();
    pattern_->ScrollToIndex(0, false, ScrollAlign::START);
    FlushLayoutTask(frameNode_);
    EXPECT_EQ(pattern_->layoutInfo_->startIndex_, 0);
    EXPECT_EQ(pattern_->layoutInfo_->endIndex_, 7);
    EXPECT_EQ(frameNode_->GetTotalChildCount(), 96);
}
} // namespace OHOS::Ace::NG

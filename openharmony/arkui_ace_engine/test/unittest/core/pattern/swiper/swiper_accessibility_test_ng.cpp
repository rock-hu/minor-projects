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

#include "swiper_test_ng.h"

#include "core/components_ng/pattern/swiper_indicator/indicator_common/swiper_indicator_accessibility_property.h"

namespace OHOS::Ace::NG {
class SwiperAccessibilityTestNg : public SwiperTestNg {
public:
};

/**
 * @tc.name: GetInfo001
 * @tc.desc: Test AccessibilityProperty
 * @tc.type: FUNC
 */
HWTEST_F(SwiperAccessibilityTestNg, GetInfo001, TestSize.Level1)
{
    CreateDefaultSwiper();
    EXPECT_TRUE(accessibilityProperty_->IsScrollable());
    EXPECT_EQ(accessibilityProperty_->GetCollectionItemCounts(), 4);
    EXPECT_EQ(accessibilityProperty_->GetCurrentIndex(), 0);
    EXPECT_EQ(accessibilityProperty_->GetBeginIndex(), 0);
    EXPECT_EQ(accessibilityProperty_->GetEndIndex(), 0);
    AccessibilityValue result = accessibilityProperty_->GetAccessibilityValue();
    EXPECT_EQ(result.min, 0);
    EXPECT_EQ(result.max, 3);
    EXPECT_EQ(result.current, 0);

    auto indicatorAcc = indicatorNode_->GetAccessibilityProperty<SwiperIndicatorAccessibilityProperty>();
    EXPECT_EQ(indicatorAcc->GetCollectionItemCounts(), ITEM_NUMBER);

    ChangeIndex(1);
    EXPECT_EQ(pattern_->GetCurrentIndex(), 1);
    EXPECT_EQ(indicatorAcc->GetCurrentIndex(), 1);

    ChangeIndex(2);
    EXPECT_EQ(indicatorAcc->GetBeginIndex(), 2);
    EXPECT_EQ(indicatorAcc->GetEndIndex(), 2);
}

/**
 * @tc.name: AccessibilityProperty001
 * @tc.desc: Test AccessibilityProperty
 * @tc.type: FUNC
 */
HWTEST_F(SwiperAccessibilityTestNg, AccessibilityProperty001, TestSize.Level1)
{
    SwiperModelNG model = CreateSwiper();
    model.SetLoop(false);
    CreateSwiperItems();
    CreateSwiperDone();

    /**
     * @tc.steps: step1. Current is first page
     * @tc.expected: ACTION_SCROLL_FORWARD
     */
    accessibilityProperty_->ResetSupportAction();
    uint64_t expectActions = 0;
    expectActions |= 1UL << static_cast<uint32_t>(AceAction::ACTION_SCROLL_FORWARD);
    EXPECT_EQ(GetActions(accessibilityProperty_), expectActions);

    /**
     * @tc.steps: step2. Show next page, Current is second(middle) page
     * @tc.expected: ACTION_SCROLL_FORWARD ACTION_SCROLL_BACKWARD
     */
    ShowNext();
    accessibilityProperty_->ResetSupportAction();
    expectActions = 0;
    expectActions |= 1UL << static_cast<uint32_t>(AceAction::ACTION_SCROLL_FORWARD);
    expectActions |= 1UL << static_cast<uint32_t>(AceAction::ACTION_SCROLL_BACKWARD);
    EXPECT_EQ(GetActions(accessibilityProperty_), expectActions);

    /**
     * @tc.steps: step3. Show last page, Current is last page
     * @tc.expected: ACTION_SCROLL_BACKWARD
     */
    ChangeIndex(3);
    accessibilityProperty_->ResetSupportAction();
    expectActions = 0;
    expectActions |= 1UL << static_cast<uint32_t>(AceAction::ACTION_SCROLL_BACKWARD);
    EXPECT_EQ(GetActions(accessibilityProperty_), expectActions);

    /**
     * @tc.cases: Swiper is loop
     * @tc.expected: ACTION_SCROLL_FORWARD ACTION_SCROLL_BACKWARD
     */
    model.SetLoop(AceType::RawPtr(frameNode_), true);
    accessibilityProperty_->ResetSupportAction();
    expectActions = 0;
    expectActions |= 1UL << static_cast<uint32_t>(AceAction::ACTION_SCROLL_FORWARD);
    expectActions |= 1UL << static_cast<uint32_t>(AceAction::ACTION_SCROLL_BACKWARD);
    EXPECT_EQ(GetActions(accessibilityProperty_), expectActions);
}

/**
 * @tc.name: AccessibilityProperty002
 * @tc.desc: Test AccessibilityProperty
 * @tc.type: FUNC
 */
HWTEST_F(SwiperAccessibilityTestNg, AccessibilityProperty002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. UnScrollable swiper
     */
    SwiperModelNG model = CreateSwiper();
    model.SetDisableSwipe(true);
    CreateSwiperItems();
    CreateSwiperDone();
    EXPECT_FALSE(accessibilityProperty_->IsScrollable());
    accessibilityProperty_->ResetSupportAction();
    EXPECT_EQ(GetActions(accessibilityProperty_), 0);
}

/**
 * @tc.name: PerformActionTest001
 * @tc.desc: Swiper AccessibilityProperty PerformAction test ScrollForward and ScrollBackward.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperAccessibilityTestNg, PerformActionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Scrollable swiper
     */
    CreateDefaultSwiper();
    EXPECT_TRUE(accessibilityProperty_->IsScrollable());

    /**
     * @tc.steps: step2. Call ActActionScrollForward
     * @tc.expected: ShowNext is triggered
     */
    accessibilityProperty_->ActActionScrollForward();
    FlushUITasks();
    EXPECT_EQ(pattern_->GetCurrentShownIndex(), 1);

    /**
     * @tc.steps: step3. Call ActActionScrollBackward
     * @tc.expected: ShowPrevious is triggered
     */
    accessibilityProperty_->ActActionScrollBackward();
    FlushUITasks();
    EXPECT_EQ(pattern_->GetCurrentShownIndex(), 0);
}

/**
 * @tc.name: PerformActionTest002
 * @tc.desc: Swiper AccessibilityProperty PerformAction test ScrollForward and ScrollBackward.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperAccessibilityTestNg, PerformActionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. UnScrollable swiper
     */
    SwiperModelNG model = CreateSwiper();
    model.SetLoop(false);
    CreateSwiperItems(1);
    CreateSwiperDone();
    EXPECT_FALSE(accessibilityProperty_->IsScrollable());

    /**
     * @tc.steps: step2. Call ActActionScrollForward
     * @tc.expected: ShowNext is not triggered
     */
    accessibilityProperty_->ActActionScrollForward();
    FlushUITasks();
    EXPECT_EQ(pattern_->GetCurrentShownIndex(), 0);

    /**
     * @tc.steps: step3. Call ActActionScrollBackward
     * @tc.expected: ShowPrevious is not triggered
     */
    accessibilityProperty_->ActActionScrollBackward();
    FlushUITasks();
    EXPECT_EQ(pattern_->GetCurrentShownIndex(), 0);
}

/**
 * @tc.name: UpdateFocusable001
 * @tc.desc: Test UpdateFocusable, indicatorNode is unFocusable when swiper has no items
 * @tc.type: FUNC
 */
HWTEST_F(SwiperAccessibilityTestNg, UpdateFocusable001, TestSize.Level1)
{
    SwiperModelNG model = CreateSwiper();
    CreateSwiperItems(3);
    CreateSwiperDone();
    auto totalCount = pattern_->TotalCount();
    EXPECT_EQ(totalCount, 3);

    auto indicatorFocusHub = indicatorNode_->GetOrCreateFocusHub();
    EXPECT_TRUE(indicatorFocusHub->GetFocusable());
    auto accessibilityProperty = indicatorNode_->GetAccessibilityProperty<AccessibilityProperty>();
    EXPECT_EQ(accessibilityProperty->GetAccessibilityLevel(), "auto");

    frameNode_->RemoveChildAtIndex(0);
    FlushUITasks();
    EXPECT_TRUE(indicatorFocusHub->GetFocusable());

    frameNode_->RemoveChildAtIndex(0);
    frameNode_->RemoveChildAtIndex(0);
    FlushUITasks();
    EXPECT_EQ(pattern_->TotalCount(), 0);

    FlushUITasks();
    EXPECT_FALSE(indicatorFocusHub->GetFocusable());
    EXPECT_EQ(accessibilityProperty->GetAccessibilityLevel(), "no");
}
} // namespace OHOS::Ace::NG

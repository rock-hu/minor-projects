/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SELECT_SELECT_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SELECT_SELECT_LAYOUT_ALGORITHM_H

#include "core/components/select/select_theme.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT SelectLayoutAlgorithm : public BoxLayoutAlgorithm {
    DECLARE_ACE_TYPE(SelectLayoutAlgorithm, BoxLayoutAlgorithm);

public:
    SelectLayoutAlgorithm() = default;

    ~SelectLayoutAlgorithm() override = default;

    void Measure(LayoutWrapper* layoutWrapper) override;

private:
    SizeF MeasureAndGetSize(const RefPtr<LayoutWrapper>& childLayoutWrapper, const LayoutConstraintF& constraint);
    SizeF MeasureSelectText(
        RefPtr<LayoutWrapper> textWrapper, const LayoutConstraintF& childConstraint, std::optional<float> maxWidth);
    void MeasureAndGetTextSize(double fontSize, SizeF& textSize, bool& isTextMin);
    float MeasureAndGetDefaultHeight(RefPtr<LayoutProperty> layoutProps, RefPtr<SelectTheme> theme);
    void NeedAgingUpdateParams(LayoutWrapper* layoutWrapper);
    void UpdateOptionsMaxLines(const std::vector<RefPtr<FrameNode>>& options, int32_t maxLines);
    void UpdateMargin(LayoutWrapper* layoutWrapper, RefPtr<SelectTheme> theme);
    void RemoveParentRestrictionsForFixIdeal(
        const RefPtr<LayoutProperty> layoutProperty, LayoutConstraintF& childConstraint);
    float fontScale_ = 0.0f;

    ACE_DISALLOW_COPY_AND_MOVE(SelectLayoutAlgorithm);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SELECT_SELECT_LAYOUT_ALGORITHM_H

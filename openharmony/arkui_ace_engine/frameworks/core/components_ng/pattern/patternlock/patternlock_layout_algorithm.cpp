/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/patternlock/patternlock_layout_algorithm.h"
#include "core/components_ng/pattern/patternlock/patternlock_layout_property.h"

namespace OHOS::Ace::NG {

std::optional<SizeF> PatternLockLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, std::nullopt);
    auto patternLockTheme = pipeline->GetTheme<V2::PatternLockTheme>();
    CHECK_NULL_RETURN(patternLockTheme, std::nullopt);
    sideLength_ = patternLockTheme->GetSideLength();
    auto patternLockLayoutProperty = DynamicCast<PatternLockLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_RETURN(patternLockLayoutProperty, std::nullopt);
    sideLength_ = patternLockLayoutProperty->GetSideLength().value_or(sideLength_);
    auto size = patternLockLayoutProperty->GetLayoutConstraint()->percentReference;
    auto length = static_cast<float>(sideLength_.ConvertToPxWithSize(std::min(size.Height(), size.Width())));
    float maxWidth = std::max(contentConstraint.maxSize.Width(), contentConstraint.minSize.Width());
    float maxHeight = std::max(contentConstraint.maxSize.Height(), contentConstraint.minSize.Height());
    float selfIdealWidth = contentConstraint.selfIdealSize.Width().value_or(maxWidth);
    float selfIdealHeight = contentConstraint.selfIdealSize.Height().value_or(maxHeight);
    float maxLength = std::min(selfIdealWidth, selfIdealHeight);
    length = std::min(maxLength, length);
    return SizeF(length, length);
}
} // namespace OHOS::Ace::NG

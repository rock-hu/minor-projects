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

#include "core/components_ng/pattern/swiper_indicator/indicator_common/swiper_arrow_layout_algorithm.h"

namespace OHOS::Ace::NG {
void SwiperArrowLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto layoutPropertyConstraint = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutPropertyConstraint);
    auto layoutConstraint = layoutPropertyConstraint->CreateChildConstraint();
    for (auto&& child : layoutWrapper->GetAllChildrenWithBuild()) {
        child->Measure(layoutConstraint);
    }
}

void SwiperArrowLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    for (auto&& child : layoutWrapper->GetAllChildrenWithBuild()) {
        child->Layout();
    }
}
} // namespace OHOS::Ace::NG

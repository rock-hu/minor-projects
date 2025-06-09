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

#include "core/components_ng/pattern/node_container/node_container_layout_algorithm.h"

#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {
void NodeContainerLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto layoutConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
    
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = host->GetPattern();
    CHECK_NULL_VOID(pattern);
    bool isEnabledChildrenMatchParent = pattern->IsEnableChildrenMatchParent();
    
    for (auto&& child : layoutWrapper->GetAllChildrenWithBuild()) {
        auto childLayoutProperty = child->GetLayoutProperty();
        CHECK_NULL_CONTINUE(childLayoutProperty);
        auto layoutPolicy = childLayoutProperty->GetLayoutPolicyProperty();
        if (isEnabledChildrenMatchParent&&layoutPolicy.has_value()) {
            auto widthLayoutPolicy = layoutPolicy.value().widthLayoutPolicy_;
            auto heightLayoutPolicy = layoutPolicy.value().heightLayoutPolicy_;
            if (widthLayoutPolicy.value_or(LayoutCalPolicy::NO_MATCH) != LayoutCalPolicy::NO_MATCH ||
                heightLayoutPolicy.value_or(LayoutCalPolicy::NO_MATCH) != LayoutCalPolicy::NO_MATCH) {
                layoutPolicyChildren_.emplace_back(child);
                continue;
            }
        }
        if (child->GetHostTag() == "RenderNode") {
            child->Measure(std::nullopt);
        } else {
            child->Measure(layoutConstraint);
        }
    }
    PerformMeasureSelf(layoutWrapper);
    if (isEnabledChildrenMatchParent) {
        auto frameSize = layoutWrapper->GetGeometryNode()->GetFrameSize();
        MeasureAdaptiveLayoutChildren(layoutWrapper, frameSize);
    }
}
} // namespace OHOS::Ace::NG
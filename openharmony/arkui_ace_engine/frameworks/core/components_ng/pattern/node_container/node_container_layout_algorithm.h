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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NODE_CONTAINER_NODE_CONTAINER_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NODE_CONTAINER_NODE_CONTAINER_LAYOUT_ALGORITHM_H

#include "core/components/common/properties/alignment.h"
#include "core/components_ng/pattern/stack/stack_layout_algorithm.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT NodeContainerLayoutAlgorithm : public StackLayoutAlgorithm {
    DECLARE_ACE_TYPE(NodeContainerLayoutAlgorithm, StackLayoutAlgorithm);

public:
    NodeContainerLayoutAlgorithm() = default;
    ~NodeContainerLayoutAlgorithm() override = default;

    std::optional<SizeF> MeasureContent(
        const LayoutConstraintF&  /*contentConstraint*/, LayoutWrapper*  /*layoutWrapper*/) override
    {
        return std::nullopt;
    }

    void Measure(LayoutWrapper* layoutWrapper) override;

private:
    ACE_DISALLOW_COPY_AND_MOVE(NodeContainerLayoutAlgorithm);
};
} // namespace OHOS::Ace::NG

#endif

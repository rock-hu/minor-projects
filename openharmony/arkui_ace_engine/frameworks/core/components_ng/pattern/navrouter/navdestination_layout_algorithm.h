/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_NAVROUTER_NAVDESTINATION_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_NAVROUTER_NAVDESTINATION_LAYOUT_ALGORITHM_H

#include <cstdint>

#include "base/memory/referenced.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT NavDestinationLayoutAlgorithm : public LayoutAlgorithm {
    DECLARE_ACE_TYPE(NavDestinationLayoutAlgorithm, LayoutAlgorithm);

public:
    NavDestinationLayoutAlgorithm() = default;
    ~NavDestinationLayoutAlgorithm() override = default;
    void Measure(LayoutWrapper* layoutWrapper) override;
    void Layout(LayoutWrapper* layoutWrapper) override;
    ACE_DISALLOW_COPY_AND_MOVE(NavDestinationLayoutAlgorithm);
    void MeasureAdaptiveLayoutChildren(
        LayoutWrapper* layoutWrapper, SizeF& realSize, float titleBarHeight, float toolBarHeight);
    void ReCalcNavDestinationSize(LayoutWrapper* layoutWrapper, SizeF& size);

    void SetIsShown(bool isShown)
    {
        isShown_ = isShown;
    }

    bool IsOnShown() const
    {
        return isShown_;
    }

private:
    bool isShown_ = false;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_NAVROUTER_NAVDESTINATION_LAYOUT_ALGORITHM_H

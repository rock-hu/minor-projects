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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCROLL_SCROLL_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCROLL_SCROLL_LAYOUT_ALGORITHM_H

#include <map>

#include "base/geometry/axis.h"
#include "base/memory/referenced.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"

namespace OHOS::Ace::NG {

constexpr int32_t SCROLL_FROM_JUMP = 3;

class ACE_EXPORT ScrollLayoutAlgorithm : public LayoutAlgorithm {
    DECLARE_ACE_TYPE(ScrollLayoutAlgorithm, LayoutAlgorithm);

public:
    explicit ScrollLayoutAlgorithm(float currentOffset) : currentOffset_(currentOffset) {}
    ~ScrollLayoutAlgorithm() override = default;

    void OnReset() override {}

    void SetCurrentOffset(float offset)
    {
        currentOffset_ = offset;
    }

    float GetCurrentOffset() const
    {
        return currentOffset_;
    }

    float GetScrollableDistance() const
    {
        return scrollableDistance_;
    }

    float GetViewPortLength() const
    {
        return viewPortLength_;
    }

    const SizeF& GetViewPort() const
    {
        return viewPort_;
    }

    const SizeF& GetViewSize() const
    {
        return viewSize_;
    }

    const SizeF& GetViewPortExtent() const
    {
        return viewPortExtent_;
    }

    void Measure(LayoutWrapper* layoutWrapper) override;

    void Layout(LayoutWrapper* layoutWrapper) override;
    void UpdateOverlay(LayoutWrapper* layoutWrapper);

private:
    void UseInitialOffset(Axis axis, SizeF selfSize, LayoutWrapper* layoutWrapper);
    bool UnableOverScroll(LayoutWrapper* layoutWrapper) const;

    float currentOffset_ = 0.0f;
    float scrollableDistance_ = 0.0f;
    float viewPortLength_ = 0.0f;
    SizeF viewPort_;
    SizeF viewPortExtent_;
    SizeF viewSize_;
    void UpdateScrollAlignment(Alignment& scrollAlignment);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCROLL_SCROLL_LAYOUT_ALGORITHM_H

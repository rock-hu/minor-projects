/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_LAYOUT_ALGORITHM_H

#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/layout/box_layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT ImageLayoutAlgorithm : public BoxLayoutAlgorithm {
    DECLARE_ACE_TYPE(ImageLayoutAlgorithm, BoxLayoutAlgorithm);

public:
    ImageLayoutAlgorithm() = default;
    ~ImageLayoutAlgorithm() override = default;

    std::optional<SizeF> MeasureContent(
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper) override;

    void Measure(LayoutWrapper* layoutWrapper) override;

    void Layout(LayoutWrapper* layoutWrapper) override;
    void UpdateFrameSizeWithLayoutPolicy(
        LayoutWrapper* layoutWrapper, OptionalSizeF& imageFrameSize, const std::optional<SizeF>& rawImageSize);

    static void PerformImageAnimationLayout(LayoutWrapper* layoutWrapper);
    static bool IsImageAnimationLayout(LayoutWrapper* layoutWrapper);

private:
    ACE_DISALLOW_COPY_AND_MOVE(ImageLayoutAlgorithm);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_LAYOUT_ALGORITHM_H

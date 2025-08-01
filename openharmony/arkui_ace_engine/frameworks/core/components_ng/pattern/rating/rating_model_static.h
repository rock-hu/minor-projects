/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RATING_RATING_MODEL_STATIC_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RATING_RATING_MODEL_STATIC_H

#include "core/components_ng/base/common_configuration.h"
#include "core/components_ng/pattern/rating/rating_model.h"
#include "core/components_ng/base/frame_node.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT RatingModelStatic {
public:
    static void SetStars(FrameNode* frameNode, const std::optional<int32_t>& value);
    static void SetStepSize(FrameNode* frameNode, const std::optional<double>& value);
    static void SetRatingOptions(FrameNode* frameNode, const std::optional<double>& rating = 0.0,
                                 const std::optional<bool>&  indicator = false);
    static void SetOnChange(FrameNode* frameNode, RatingChangeEvent&& onChange);
    static void SetOnChangeEvent(FrameNode* frameNode, RatingChangeEvent&& onChange);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RATING_RATING_MODEL_STATIC_H

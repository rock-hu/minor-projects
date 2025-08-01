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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RADIO_RADIO_MODEL_STATIC_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RADIO_RADIO_MODEL_STATIC_H

#include "core/components_ng/base/common_configuration.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/radio/radio_model.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT RadioModelStatic {
public:
    static void SetChecked(FrameNode* frameNode, const std::optional<bool> isChecked);
    static void SetCheckedBackgroundColor(FrameNode* frameNode, const std::optional<Color>& color);
    static void SetUncheckedBorderColor(FrameNode* frameNode, const std::optional<Color>& color);
    static void SetIndicatorColor(FrameNode* frameNode, const std::optional<Color>& color);
    static void SetBuilder(FrameNode* frameNode, std::function<void()>&& builder);
    static void SetOnChangeEvent(FrameNode* frameNode, ChangeEvent&& onChangeEvent);
    static void SetRadioIndicatorType(FrameNode* frameNode, const std::optional<int32_t>& indicator);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RADIO_RADIO_MODEL_STATIC_H

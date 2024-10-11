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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RADIO_RADIO_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RADIO_RADIO_MODEL_NG_H

#include "core/components_ng/base/common_configuration.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/radio/radio_model.h"

namespace OHOS::Ace::NG {
class RadioConfiguration : public CommonConfiguration {
public:
    RadioConfiguration(const std::string& value, bool checked, bool enabled)
        : CommonConfiguration(enabled), value_(value), checked_(checked)
    {}
    std::string value_;
    bool checked_;
};
using RadioMakeCallback = std::function<RefPtr<FrameNode>(RadioConfiguration& radioConfiguration)>;

enum class RadioIndicatorType {
    TICK = 0,
    DOT,
    CUSTOM,
};

class ACE_EXPORT RadioModelNG : public OHOS::Ace::RadioModel {
public:
    void Create(const std::optional<std::string>& value, const std::optional<std::string>& group,
        const std::optional<int32_t>& indicator) override;
    void SetChecked(bool isChecked) override;
    void SetOnChange(ChangeEvent&& onChange) override;
    void SetWidth(const Dimension& width) override;
    void SetHeight(const Dimension& height) override;
    void SetPadding(const NG::PaddingPropertyF& args, const NG::PaddingProperty& newArgs) override;
    void SetCheckedBackgroundColor(const Color& color) override;
    void SetUncheckedBorderColor(const Color& color) override;
    void SetIndicatorColor(const Color& color) override;
    void SetOnChangeEvent(ChangeEvent&& onChangeEvent) override;
    void SetResponseRegion(const std::vector<DimensionRect>& responseRegion) override;
    void SetHoverEffect(HoverEffectType hoverEffect) override;

    static RefPtr<FrameNode> CreateFrameNode(int32_t nodeId);
    void SetBuilder(std::function<void()>&& buildFunc) override;
    static void SetRadioIndicator(int32_t indicator);
    static void SetChecked(FrameNode* frameNode, bool isChecked);
    static void SetCheckedBackgroundColor(FrameNode* frameNode, const Color& color);
    static void SetUncheckedBorderColor(FrameNode* frameNode, const Color& color);
    static void SetIndicatorColor(FrameNode* frameNode, const Color& color);
    static void SetWidth(FrameNode* frameNode, const Dimension& width);
    static void SetHeight(FrameNode* frameNode, const Dimension& height);
    static void SetHoverEffect(FrameNode* frameNode, HoverEffectType hoverEffect);
    static void SetPadding(FrameNode* frameNode, const NG::PaddingProperty& newArgs);
    static void SetResponseRegion(FrameNode* frameNode, const std::vector<DimensionRect>& responseRegion);
    static void SetBuilderFunc(FrameNode* frameNode, NG::RadioMakeCallback&& jsMake);
    static void SetChangeValue(FrameNode* frameNode, bool value);
    static void SetOnChange(FrameNode* frameNode, ChangeEvent&& onChange);
    static bool GetChecked(FrameNode* frameNode);
    static Color GetCheckedBackgroundColor(FrameNode* frameNode);
    static Color GetUncheckedBorderColor(FrameNode* frameNode);
    static Color GetIndicatorColor(FrameNode* frameNode);
    static void SetRadioValue(FrameNode* frameNode, const std::string& value);
    static std::string GetRadioValue(FrameNode* frameNode);
    static void SetRadioGroup(FrameNode* frameNode, const std::string& value);
    static std::string GetRadioGroup(FrameNode* frameNode);
    static void SetRadioOptions(FrameNode* frameNode, const std::string& value,
        const std::string& group, int32_t indicator);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_RADIO_RADIO_MODEL_NG_H

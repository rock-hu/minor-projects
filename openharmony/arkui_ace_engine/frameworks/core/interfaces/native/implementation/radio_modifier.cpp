/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/components_ng/pattern/radio/radio_model_ng.h"
#include "core/components_ng/pattern/radio/radio_model_static.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "core/interfaces/native/utility/ace_engine_types.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "core/common/container.h"
#include "core/components_ng/pattern/radio/radio_pattern.h"

namespace OHOS::Ace::NG::Converter {
template<>
void AssignCast(std::optional<RadioIndicatorType>& dst, const Ark_RadioIndicatorType& src)
{
    switch (src) {
        case ARK_RADIO_INDICATOR_TYPE_TICK: dst = RadioIndicatorType::TICK; break;
        case ARK_RADIO_INDICATOR_TYPE_DOT: dst = RadioIndicatorType::DOT; break;
        case ARK_RADIO_INDICATOR_TYPE_CUSTOM: dst = RadioIndicatorType::CUSTOM; break;
        default: LOGE("Unexpected enum value in Ark_RadioIndicatorType: %{public}d", src);
    }
}
} // namespace OHOS::Ace::NG::Converter

namespace OHOS::Ace::NG::GeneratedModifier {
namespace RadioModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = RadioModelNG::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // RadioModifier
namespace RadioInterfaceModifier {
void SetRadioOptionsImpl(Ark_NativePointer node,
                         const Ark_RadioOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    auto group = Converter::Convert<std::string>(options->group);
    RadioModelNG::SetRadioGroup(frameNode, group);
    auto radioValue = Converter::Convert<std::string>(options->value);
    RadioModelNG::SetRadioValue(frameNode, radioValue);
    if (Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        auto indicatorType = Converter::OptConvert<RadioIndicatorType>(options->indicatorType);
        RadioModelStatic::SetRadioIndicatorType(frameNode, EnumToInt(indicatorType));
    }
    auto arkBuilder = Converter::OptConvert<CustomNodeBuilder>(options->indicatorBuilder);
    if (arkBuilder.has_value()) {
        auto builder = [callback = CallbackHelper(arkBuilder.value()), node]() {
            auto builderNode = callback.BuildSync(node);
            NG::ViewStackProcessor::GetInstance()->Push(builderNode);
        };
        RadioModelStatic::SetBuilder(frameNode, std::move(builder));
    }
}
} // RadioInterfaceModifier
namespace RadioAttributeModifier {
void Checked0Impl(Ark_NativePointer node,
                  const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto isChecked = value ? Converter::OptConvert<bool>(*value) : std::nullopt;
    RadioModelStatic::SetChecked(frameNode, isChecked);
}
void Checked1Impl(Ark_NativePointer node,
                  const Opt_Boolean* value)
{
    Checked0Impl(node, value);
}
void OnChange0Impl(Ark_NativePointer node,
                   const Opt_Callback_Boolean_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    ChangeEvent onEvent{};
    if (optValue) {
        onEvent = [arkCallback = CallbackHelper(*optValue)](const bool param) {
            auto arkValue = Converter::ArkValue<Ark_Boolean>(param);
            arkCallback.Invoke(arkValue);
        };
    }
    RadioModelNG::SetOnChange(frameNode, std::move(onEvent));
}
void OnChange1Impl(Ark_NativePointer node,
                   const Opt_OnRadioChangeCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optCallback = Converter::GetOptPtr(value);
    ChangeEvent onEvent = {};
    if (optCallback.has_value()) {
        onEvent = [arkCallback = CallbackHelper(*optCallback)](const bool param) {
            auto arkValue = Converter::ArkValue<Ark_Boolean>(param);
            arkCallback.Invoke(arkValue);
        };
    }
    RadioModelNG::SetOnChange(frameNode, std::move(onEvent));
}
void RadioStyleImpl(Ark_NativePointer node,
                    const Opt_RadioStyle* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto style = value ? Converter::OptConvert<Converter::RadioStyle>(*value) : std::nullopt;
    if (style) {
        RadioModelStatic::SetCheckedBackgroundColor(frameNode, style->checkedBackgroundColor);
        RadioModelStatic::SetUncheckedBorderColor(frameNode, style->uncheckedBorderColor);
        RadioModelStatic::SetIndicatorColor(frameNode, style->indicatorColor);
    } else {
        RadioModelStatic::SetCheckedBackgroundColor(frameNode, std::nullopt);
        RadioModelStatic::SetUncheckedBorderColor(frameNode, std::nullopt);
        RadioModelStatic::SetIndicatorColor(frameNode, std::nullopt);
    }
}
void ContentModifier0Impl(Ark_NativePointer node,
                          const Opt_ContentModifier* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    LOGE("ARKOALA RadioAttributeModifier::ContentModifierImpl -> Method is not implemented.");
}
void ContentModifier1Impl(Ark_NativePointer node,
                          const Opt_ContentModifier* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
}
void _onChangeEvent_checkedImpl(Ark_NativePointer node,
                                const Callback_Opt_Boolean_Void* callback)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(callback);
    WeakPtr<FrameNode> weakNode = AceType::WeakClaim(frameNode);
    auto onEvent = [arkCallback = CallbackHelper(*callback), weakNode](bool check) {
        PipelineContext::SetCallBackNode(weakNode);
        arkCallback.Invoke(Converter::ArkValue<Opt_Boolean>(check));
    };
    RadioModelStatic::SetOnChangeEvent(frameNode, std::move(onEvent));
}
} // RadioAttributeModifier
const GENERATED_ArkUIRadioModifier* GetRadioModifier()
{
    static const GENERATED_ArkUIRadioModifier ArkUIRadioModifierImpl {
        RadioModifier::ConstructImpl,
        RadioInterfaceModifier::SetRadioOptionsImpl,
        RadioAttributeModifier::Checked0Impl,
        RadioAttributeModifier::Checked1Impl,
        RadioAttributeModifier::OnChange0Impl,
        RadioAttributeModifier::OnChange1Impl,
        RadioAttributeModifier::RadioStyleImpl,
        RadioAttributeModifier::ContentModifier0Impl,
        RadioAttributeModifier::ContentModifier1Impl,
        RadioAttributeModifier::_onChangeEvent_checkedImpl,
    };
    return &ArkUIRadioModifierImpl;
}

}

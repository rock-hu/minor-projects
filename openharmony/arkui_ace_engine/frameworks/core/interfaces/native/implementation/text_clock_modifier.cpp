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

#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "core/components_ng/pattern/text_clock/text_clock_model_ng.h"
#include "core/interfaces/native/utility/validators.h"
#include "core/components/common/properties/text_style_parser.h"
#include "core/interfaces/native/implementation/text_clock_controller_peer_impl.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "arkoala_api_generated.h"
#include "core/interfaces/native/utility/callback_helper.h"

namespace OHOS::Ace::NG {
    struct TextClockOptions {
        std::optional<float> timeZoneOffset = std::nullopt;
        TextClockControllerPeer* peerController = nullptr;
    };
} // namespace OHOS::Ace::NG

namespace OHOS::Ace::NG::Converter {
    template<>
    TextClockOptions Convert(const Ark_TextClockOptions& src)
    {
        TextClockOptions dst;
        auto value = OptConvert<float>(src.timeZoneOffset);
        // valid hoursWest is within [-14, 12]
        Validator::ValidateByRange(value, -14.0f, 12.0f);
        dst.timeZoneOffset = value;
        dst.peerController = OptConvert<Ark_TextClockController>(src.controller).value_or(nullptr);
        return dst;
    }
} // namespace OHOS::Ace::NG::Converter


namespace OHOS::Ace::NG::GeneratedModifier {
namespace TextClockModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = TextClockModelNG::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // TextClockModifier
namespace TextClockInterfaceModifier {
void SetTextClockOptionsImpl(Ark_NativePointer node,
                             const Opt_TextClockOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    if (options->tag != ARK_TAG_UNDEFINED) {
        auto textClockOptionsOpt = Converter::OptConvert<TextClockOptions>(options->value);
        if (textClockOptionsOpt.has_value()) {
            // TextClockModelNG::SetHoursWest(frameNode, textClockOptionsOpt.value().timeZoneOffset);

            auto controller = TextClockModelNG::InitTextController(frameNode);
            CHECK_NULL_VOID(controller);
            auto peerImplPtr = textClockOptionsOpt.value().peerController;
            CHECK_NULL_VOID(peerImplPtr);
            peerImplPtr->controller = std::move(controller);
        }
    }
}
} // TextClockInterfaceModifier
namespace TextClockAttributeModifier {
void FormatImpl(Ark_NativePointer node,
                const Ark_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto convValue = value ? Converter::OptConvert<std::string>(*value) : std::nullopt;
    // TextClockModelNG::SetFormat(frameNode, convValue);
}
void OnDateChangeImpl(Ark_NativePointer node,
                      const Callback_Number_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto onDateChange = [arkCallback = CallbackHelper(*value)](const std::string& data) {
        arkCallback.Invoke(Converter::ArkValue<Ark_Number>(std::stof(data)));
    };
    TextClockModelNG::SetOnDateChange(frameNode, std::move(onDateChange));
}
void FontColorImpl(Ark_NativePointer node,
                   const Ark_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = value ? Converter::OptConvert<Color>(*value) : std::nullopt;
    // TextClockModelNG::SetFontColor(frameNode, convValue);
}
void FontSizeImpl(Ark_NativePointer node,
                  const Ark_Length* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto convValue = value ? Converter::OptConvert<Dimension>(*value) : std::nullopt;
    Validator::ValidateNonNegative(convValue);
    Validator::ValidateNonPercent(convValue);
    // TextClockModelNG::SetFontSize(frameNode, convValue);
}
void FontStyleImpl(Ark_NativePointer node,
                   Ark_FontStyle value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = value ? Converter::OptConvert<Ace::FontStyle>(value) : std::nullopt;
    // TextClockModelNG::SetFontStyle(frameNode, convValue);
}
void FontWeightImpl(Ark_NativePointer node,
                    const Ark_Union_Number_FontWeight_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = value ? Converter::OptConvert<Ace::FontWeight>(*value) : std::nullopt;
    // TextClockModelNG::SetFontWeight(frameNode, convValue);
}
void FontFamilyImpl(Ark_NativePointer node,
                    const Ark_ResourceStr* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    StringArray families;
    if (auto fontfamiliesOpt = Converter::OptConvert<Converter::FontFamilies>(*value); fontfamiliesOpt) {
        families = fontfamiliesOpt->families;
    }
    TextClockModelNG::SetFontFamily(frameNode, families);
}
void TextShadowImpl(Ark_NativePointer node,
                    const Ark_Union_ShadowOptions_Array_ShadowOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    std::vector<Shadow> shadowListResult;
    auto shadowList = Converter::OptConvert<std::vector<Shadow>>(*value);
    if (shadowList.has_value()) {
        shadowListResult = shadowList.value();
    }
    TextClockModelNG::SetTextShadow(frameNode, shadowListResult);
}
void FontFeatureImpl(Ark_NativePointer node,
                     const Ark_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto fontFeatureSettings = Converter::Convert<std::string>(*value);
    TextClockModelNG::SetFontFeature(frameNode, ParseFontFeatureSettings(fontFeatureSettings));
}
void ContentModifierImpl(Ark_NativePointer node,
                         const Ark_CustomObject* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    //auto convValue = Converter::OptConvert<type_name>(*value);
    //TextClockModelNG::SetContentModifier(frameNode, convValue);
}
void DateTimeOptionsImpl(Ark_NativePointer node,
                         const Opt_DateTimeOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    //auto convValue = value ? Converter::OptConvert<type>(*value) : std::nullopt;
    //TextClockModelNG::SetDateTimeOptions(frameNode, convValue);
}
} // TextClockAttributeModifier
const GENERATED_ArkUITextClockModifier* GetTextClockModifier()
{
    static const GENERATED_ArkUITextClockModifier ArkUITextClockModifierImpl {
        TextClockModifier::ConstructImpl,
        TextClockInterfaceModifier::SetTextClockOptionsImpl,
        TextClockAttributeModifier::FormatImpl,
        TextClockAttributeModifier::OnDateChangeImpl,
        TextClockAttributeModifier::FontColorImpl,
        TextClockAttributeModifier::FontSizeImpl,
        TextClockAttributeModifier::FontStyleImpl,
        TextClockAttributeModifier::FontWeightImpl,
        TextClockAttributeModifier::FontFamilyImpl,
        TextClockAttributeModifier::TextShadowImpl,
        TextClockAttributeModifier::FontFeatureImpl,
        TextClockAttributeModifier::ContentModifierImpl,
        TextClockAttributeModifier::DateTimeOptionsImpl,
    };
    return &ArkUITextClockModifierImpl;
}

}

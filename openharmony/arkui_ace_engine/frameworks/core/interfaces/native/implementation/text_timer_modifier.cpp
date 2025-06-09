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
#include "core/interfaces/native/utility/validators.h"
#include "core/components_ng/pattern/texttimer/text_timer_model_ng.h"
#include "core/interfaces/native/implementation/text_timer_controller_peer_impl.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG {
struct TextTimerOptions {
    std::optional<bool> isCountDown;
    std::optional<double> count;
    std::optional<Ark_TextTimerController> controller;
};

namespace Converter {
template<>
TextTimerOptions Convert(const Ark_TextTimerOptions& src)
{
    TextTimerOptions dst;
    dst.isCountDown = Converter::OptConvert<bool>(src.isCountDown);
    dst.count = Converter::OptConvert<float>(src.count);
    dst.controller = Converter::OptConvert<Ark_TextTimerController>(src.controller);
    return dst;
}
} // namespace Converter
} // namespace OHOS::Ace::NG

namespace OHOS::Ace::NG::GeneratedModifier {
namespace TextTimerModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = TextTimerModelNG::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // TextTimerModifier
namespace TextTimerInterfaceModifier {
void SetTextTimerOptionsImpl(Ark_NativePointer node,
                             const Opt_TextTimerOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    auto opts = Converter::OptConvert<TextTimerOptions>(*options);
    // TextTimerModelNG::SetIsCountDown(frameNode, opts->isCountDown);
    if (!opts->isCountDown.value_or(false)) {
        opts->count.reset();
    }
    // TextTimerModelNG::SetInputCount(frameNode, opts->count);

    CHECK_NULL_VOID(opts->controller);
    auto textTimerController = TextTimerModelNG::InitTextController(frameNode);
    CHECK_NULL_VOID(textTimerController);
    TextTimerControllerPeer* peerPtr = opts->controller.value();
    CHECK_NULL_VOID(peerPtr);
    peerPtr->SetController(textTimerController);
}
} // TextTimerInterfaceModifier
namespace TextTimerAttributeModifier {
void FormatImpl(Ark_NativePointer node,
                const Ark_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto format = Converter::OptConvert<std::string>(*value);

    if (format) {
        std::smatch result;
        std::regex pattern("(([YyMdD]+))");
        if (std::regex_search(format.value(), result, pattern)) {
            if (!result.empty()) {
                format.reset();
            }
        }

        std::string target = "HmsS:.";
        for (auto ch : *format) {
            if (target.find(ch) == std::string::npos) {
                format.reset();
            }
        }

        auto pos = format->find("hh");
        if (pos != std::string::npos) {
            format->replace(pos, sizeof("hh") - 1, "HH");
        }
    }
    // TextTimerModelNG::SetFormat(frameNode, format);
}
void FontColorImpl(Ark_NativePointer node,
                   const Ark_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto color = Converter::OptConvert<Color>(*value);
    // TextTimerModelNG::SetFontColor(frameNode, color);
}
void FontSizeImpl(Ark_NativePointer node,
                  const Ark_Length* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto size = Converter::OptConvert<Dimension>(*value);
    Validator::ValidateNonNegative(size);
    Validator::ValidateNonPercent(size);
    // TextTimerModelNG::SetFontSize(frameNode, size);
}
void FontStyleImpl(Ark_NativePointer node,
                   Ark_FontStyle value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto style = Converter::OptConvert<Ace::FontStyle>(value);
    // TextTimerModelNG::SetFontStyle(frameNode, style);
}
void FontWeightImpl(Ark_NativePointer node,
                    const Ark_Union_Number_FontWeight_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto weight = Converter::OptConvert<Ace::FontWeight>(*value);
    // TextTimerModelNG::SetFontWeight(frameNode, weight);
}
void FontFamilyImpl(Ark_NativePointer node,
                    const Ark_ResourceStr* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<StringArray> families;
    if (auto fontfamiliesOpt = Converter::OptConvert<Converter::FontFamilies>(*value); fontfamiliesOpt) {
        families = fontfamiliesOpt->families;
    }
    // TextTimerModelNG::SetFontFamily(frameNode, families);
}
void OnTimerImpl(Ark_NativePointer node,
                 const Callback_Number_Number_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    LOGE("Arkoala method TextTimerAttributeModifier.setOnTimer not implemented");
}
void TextShadowImpl(Ark_NativePointer node,
                    const Ark_Union_ShadowOptions_Array_ShadowOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto shadowList = Converter::OptConvert<std::vector<Shadow>>(*value);
    // TextTimerModelNG::SetTextShadow(frameNode, shadowList);
}
void ContentModifierImpl(Ark_NativePointer node,
                         const Ark_CustomObject* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    //auto convValue = Converter::OptConvert<type_name>(*value);
    //TextTimerModelNG::SetContentModifier(frameNode, convValue);
    LOGE("Arkoala method TextTimerAttributeModifier.setContentModifier not implemented");
}
} // TextTimerAttributeModifier
const GENERATED_ArkUITextTimerModifier* GetTextTimerModifier()
{
    static const GENERATED_ArkUITextTimerModifier ArkUITextTimerModifierImpl {
        TextTimerModifier::ConstructImpl,
        TextTimerInterfaceModifier::SetTextTimerOptionsImpl,
        TextTimerAttributeModifier::FormatImpl,
        TextTimerAttributeModifier::FontColorImpl,
        TextTimerAttributeModifier::FontSizeImpl,
        TextTimerAttributeModifier::FontStyleImpl,
        TextTimerAttributeModifier::FontWeightImpl,
        TextTimerAttributeModifier::FontFamilyImpl,
        TextTimerAttributeModifier::OnTimerImpl,
        TextTimerAttributeModifier::TextShadowImpl,
        TextTimerAttributeModifier::ContentModifierImpl,
    };
    return &ArkUITextTimerModifierImpl;
}
}

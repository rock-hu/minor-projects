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
#include "core/components_ng/pattern/text/symbol_span_model_ng.h"
#include "core/components_ng/pattern/text/symbol_span_model_static.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/validators.h"
#include "arkoala_api_generated.h"
#include "frameworks/core/components_ng/pattern/symbol/constants.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace SymbolSpanModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = SymbolSpanModelStatic::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // SymbolSpanModifier
namespace SymbolSpanInterfaceModifier {
void SetSymbolSpanOptionsImpl(Ark_NativePointer node,
                              const Ark_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto convValue = Converter::OptConvert<Converter::SymbolData>(*value);
    if (convValue.has_value() && convValue->symbol.has_value()) {
        SymbolSpanModelNG::InitialSymbol(frameNode, convValue->symbol.value());
    }
}
} // SymbolSpanInterfaceModifier
namespace SymbolSpanAttributeModifier {
void FontSizeImpl(Ark_NativePointer node,
                  const Opt_Union_Number_String_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<Dimension> optValue = std::nullopt;
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        optValue = Converter::OptConvertFromArkNumStrRes(value->value);
    }
    Validator::ValidateNonNegative(optValue);
    Validator::ValidateNonPercent(optValue);
    SymbolSpanModelStatic::SetFontSize(frameNode, optValue);
}
void FontColorImpl(Ark_NativePointer node,
                   const Opt_Array_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optColorVec = Converter::OptConvert<std::vector<std::optional<Color>>>(*value);
    if (!optColorVec) {
        // TODO: Reset value
        return;
    }
    std::vector<Color> colorVec;
    for (std::optional<Color> color: *optColorVec) {
        if (color.has_value()) {
            colorVec.emplace_back(color.value());
        }
    }
    SymbolSpanModelNG::SetFontColor(frameNode, colorVec);
}
void FontWeightImpl(Ark_NativePointer node,
                    const Opt_Union_Number_FontWeight_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<Ace::FontWeight>(*value);
    SymbolSpanModelStatic::SetFontWeight(frameNode, convValue);
}
void EffectStrategyImpl(Ark_NativePointer node,
                        const Opt_SymbolEffectStrategy* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<SymbolEffectType>(*value);
    SymbolSpanModelStatic::SetSymbolEffect(frameNode, EnumToInt(convValue));
}
void RenderingStrategyImpl(Ark_NativePointer node,
                           const Opt_SymbolRenderingStrategy* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<Converter::RenderingStrategy>(*value);
    SymbolSpanModelStatic::SetSymbolRenderingStrategy(frameNode, EnumToInt(convValue));
}
} // SymbolSpanAttributeModifier
const GENERATED_ArkUISymbolSpanModifier* GetSymbolSpanModifier()
{
    static const GENERATED_ArkUISymbolSpanModifier ArkUISymbolSpanModifierImpl {
        SymbolSpanModifier::ConstructImpl,
        SymbolSpanInterfaceModifier::SetSymbolSpanOptionsImpl,
        SymbolSpanAttributeModifier::FontSizeImpl,
        SymbolSpanAttributeModifier::FontColorImpl,
        SymbolSpanAttributeModifier::FontWeightImpl,
        SymbolSpanAttributeModifier::EffectStrategyImpl,
        SymbolSpanAttributeModifier::RenderingStrategyImpl,
    };
    return &ArkUISymbolSpanModifierImpl;
}

}

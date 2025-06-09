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

#include "core/interfaces/native/utility/ace_engine_types.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/stack/stack_model_ng.h"
#include "core/components/common/properties/alignment.h"
#include "core/components_ng/base/view_abstract_model_ng.h"

namespace OHOS::Ace::NG {
namespace {
struct StackOptions {
    std::optional<Alignment> alignContent;
};
}

namespace Converter {
template<>
StackOptions Convert(const Ark_StackOptions& src)
{
    return {
        .alignContent = OptConvert<Alignment>(src.alignContent),
    };
}
} // namespace Converter
} // namespace OHOS::Ace::NG

namespace OHOS::Ace::NG::GeneratedModifier {
namespace StackModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = StackModelNG::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // StackModifier
namespace StackInterfaceModifier {
void SetStackOptionsImpl(Ark_NativePointer node,
                         const Opt_StackOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto opts = Converter::OptConvert<StackOptions>(*options);
    auto align = opts ? opts->alignContent : std::nullopt;
    StackModelNG::SetAlignment(frameNode, align.value_or(Alignment::CENTER));
}
} // StackInterfaceModifier
namespace StackAttributeModifier {
void AlignContentImpl(Ark_NativePointer node,
                      Ark_Alignment value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    StackModelNG::SetAlignment(frameNode, Converter::ConvertOrDefault(value, Alignment::CENTER));
}
void PointLightImpl(Ark_NativePointer node,
                    const Ark_PointLightStyle* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
#ifdef POINT_LIGHT_ENABLE
    // auto pointLightStyle = Converter::OptConvert<Converter::PointLightStyle>(*value);
    // auto uiNode = reinterpret_cast<Ark_NodeHandle>(node);
    // auto themeConstants = Converter::GetThemeConstants(uiNode, "", "");
    // CHECK_NULL_VOID(themeConstants);
    // if (pointLightStyle) {
    //     if (pointLightStyle->lightSource) {
    //         ViewAbstractModelNG::SetLightPosition(frameNode, pointLightStyle->lightSource->x,
    //             pointLightStyle->lightSource->y,
    //             pointLightStyle->lightSource->z);
    //         ViewAbstractModelNG::SetLightIntensity(frameNode,
    //             pointLightStyle->lightSource->intensity);
    //         ViewAbstractModelNG::SetLightColor(frameNode, pointLightStyle->lightSource->lightColor);
    //     } else {
    //         ViewAbstractModelNG::SetLightPosition(frameNode, std::nullopt, std::nullopt, std::nullopt);
    //         ViewAbstractModelNG::SetLightIntensity(frameNode, std::nullopt);
    //         ViewAbstractModelNG::SetLightColor(frameNode, std::nullopt);
    //     }
    //     // illuminated
    //     ViewAbstractModelNG::SetLightIlluminated(frameNode, pointLightStyle->illuminationType, themeConstants);
    //     // bloom
    //     ViewAbstractModelNG::SetBloom(frameNode, pointLightStyle->bloom, themeConstants);
    // } else {
    //     ViewAbstractModelNG::SetLightPosition(frameNode, std::nullopt, std::nullopt, std::nullopt);
    //     ViewAbstractModelNG::SetLightIntensity(frameNode, std::nullopt);
    //     ViewAbstractModelNG::SetLightColor(frameNode, std::nullopt);
    //     ViewAbstractModelNG::SetLightIlluminated(frameNode, std::nullopt, themeConstants);
    //     ViewAbstractModelNG::SetBloom(frameNode, std::nullopt, themeConstants);
    // }
#endif
}
} // StackAttributeModifier
const GENERATED_ArkUIStackModifier* GetStackModifier()
{
    static const GENERATED_ArkUIStackModifier ArkUIStackModifierImpl {
        StackModifier::ConstructImpl,
        StackInterfaceModifier::SetStackOptionsImpl,
        StackAttributeModifier::AlignContentImpl,
        StackAttributeModifier::PointLightImpl,
    };
    return &ArkUIStackModifierImpl;
}

}

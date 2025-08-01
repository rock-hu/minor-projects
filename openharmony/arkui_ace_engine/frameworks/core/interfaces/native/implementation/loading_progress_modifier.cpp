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

#include "arkoala_api_generated.h"
#include "core/components_ng/pattern/loading_progress/loading_progress_model_ng.h"
#include "core/components_ng/pattern/loading_progress/loading_progress_model_static.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "core/interfaces/native/utility/converter.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace LoadingProgressModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = LoadingProgressModelNG::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // LoadingProgressModifier
namespace LoadingProgressInterfaceModifier {
void SetLoadingProgressOptionsImpl(Ark_NativePointer node)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
}
} // LoadingProgressInterfaceModifier
namespace LoadingProgressAttributeModifier {
void ColorImpl(Ark_NativePointer node,
               const Opt_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto color = Converter::OptConvert<Color>(*value);
    LoadingProgressModelStatic::SetColor(frameNode, color);
}
void EnableLoadingImpl(Ark_NativePointer node,
                       const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    LoadingProgressModelNG::SetEnableLoading(frameNode, *convValue);
}
void ContentModifierImpl(Ark_NativePointer node,
                         const Opt_ContentModifier* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    LOGE("ARKOALA LoadingProgressAttributeModifier::ContentModifierImpl -> Method is not implemented");
}
} // LoadingProgressAttributeModifier
const GENERATED_ArkUILoadingProgressModifier* GetLoadingProgressModifier()
{
    static const GENERATED_ArkUILoadingProgressModifier ArkUILoadingProgressModifierImpl {
        LoadingProgressModifier::ConstructImpl,
        LoadingProgressInterfaceModifier::SetLoadingProgressOptionsImpl,
        LoadingProgressAttributeModifier::ColorImpl,
        LoadingProgressAttributeModifier::EnableLoadingImpl,
        LoadingProgressAttributeModifier::ContentModifierImpl,
    };
    return &ArkUILoadingProgressModifierImpl;
}

}

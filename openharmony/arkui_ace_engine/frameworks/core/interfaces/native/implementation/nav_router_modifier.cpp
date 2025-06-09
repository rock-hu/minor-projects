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
#include "core/interfaces/native/node/nav_router_modifier.h"
#include "core/components_ng/pattern/navrouter/navrouter_model_ng.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "core/interfaces/native/generated/interface/node_api.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace NavRouterModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    // auto frameNode = NavRouterModelNG::CreateFrameNode(id);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}
} // NavRouterModifier
namespace NavRouterInterfaceModifier {
void SetNavRouterOptions0Impl(Ark_NativePointer node)
{
    // still it empty for save default values
}
void SetNavRouterOptions1Impl(Ark_NativePointer node,
                              const Ark_RouteInfo* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    //auto convValue = Converter::OptConvert<type_name>(*value);
    //NavRouterModelNG::SetSetNavRouterOptions1(frameNode, convValue);
    LOGE("ARKOALA SetNavRouterOptions1 -> Method is not implemented.");
}
} // NavRouterInterfaceModifier
namespace NavRouterAttributeModifier {
void OnStateChangeImpl(Ark_NativePointer node,
                       const Callback_Boolean_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto onStateChangeCallback = [arkCallback = CallbackHelper(*value)](const bool isActivated) {
        auto arkIsActivated = Converter::ArkValue<Ark_Boolean>(isActivated);
        arkCallback.Invoke(arkIsActivated);
    };
    // NavRouterModelNG::SetOnStateChange(frameNode, onStateChangeCallback);
}
void ModeImpl(Ark_NativePointer node,
              Ark_NavRouteMode value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto enumMode = Converter::OptConvert<NavRouteMode>(value);
    // NavRouterModelNG::SetNavRouteMode(frameNode, EnumToInt(enumMode));
}
} // NavRouterAttributeModifier
const GENERATED_ArkUINavRouterModifier* GetNavRouterModifier()
{
    static const GENERATED_ArkUINavRouterModifier ArkUINavRouterModifierImpl {
        NavRouterModifier::ConstructImpl,
        NavRouterInterfaceModifier::SetNavRouterOptions0Impl,
        NavRouterInterfaceModifier::SetNavRouterOptions1Impl,
        NavRouterAttributeModifier::OnStateChangeImpl,
        NavRouterAttributeModifier::ModeImpl,
    };
    return &ArkUINavRouterModifierImpl;
}

}

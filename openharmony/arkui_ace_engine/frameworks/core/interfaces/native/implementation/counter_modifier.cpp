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
#include "core/components_ng/pattern/counter/counter_model_ng.h"
#include "core/components_ng/pattern/counter/counter_model_static.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace CounterModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = CounterModelStatic::CreateFrameNode(id);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // CounterModifier
namespace CounterInterfaceModifier {
void SetCounterOptionsImpl(Ark_NativePointer node)
{
    // keep it empty because Counter don`t have any options
}
} // CounterInterfaceModifier
namespace CounterAttributeModifier {
void OnIncImpl(Ark_NativePointer node,
               const Opt_VoidCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onEvent = [arkCallback = CallbackHelper(*optValue)]() {
        arkCallback.Invoke();
    };
    CounterModelStatic::SetOnInc(frameNode, onEvent);
}
void OnDecImpl(Ark_NativePointer node,
               const Opt_VoidCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onEvent = [arkCallback = CallbackHelper(*optValue)]() {
        arkCallback.Invoke();
    };
    CounterModelStatic::SetOnDec(frameNode, onEvent);
}
void EnableDecImpl(Ark_NativePointer node,
                   const Opt_Boolean* value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    CounterModelNG::SetEnableDec(frameNode, *convValue);
}
void EnableIncImpl(Ark_NativePointer node,
                   const Opt_Boolean* value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    CounterModelNG::SetEnableInc(frameNode, *convValue);
}
} // CounterAttributeModifier
const GENERATED_ArkUICounterModifier* GetCounterModifier()
{
    static const GENERATED_ArkUICounterModifier ArkUICounterModifierImpl {
        CounterModifier::ConstructImpl,
        CounterInterfaceModifier::SetCounterOptionsImpl,
        CounterAttributeModifier::OnIncImpl,
        CounterAttributeModifier::OnDecImpl,
        CounterAttributeModifier::EnableDecImpl,
        CounterAttributeModifier::EnableIncImpl,
    };
    return &ArkUICounterModifierImpl;
}

}

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
#include "core/interfaces/native/implementation/text_input_controller_peer.h"
#include "core/interfaces/native/utility/converter.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace TextInputControllerAccessor {
void DestroyPeerImpl(Ark_TextInputController peer)
{
    CHECK_NULL_VOID(peer);
    peer->controller_ = nullptr;
    delete peer;
}
Ark_TextInputController CtorImpl()
{
    return new TextInputControllerPeer();
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
void CaretPositionImpl(Ark_TextInputController peer,
                       const Ark_Number* value)
{
    CHECK_NULL_VOID(peer && value && peer->controller_);
    peer->controller_->CaretPosition(std::max(Converter::Convert<int32_t>(*value), 0));
}
void SetTextSelectionImpl(Ark_TextInputController peer,
                          const Ark_Number* selectionStart,
                          const Ark_Number* selectionEnd,
                          const Opt_SelectionOptions* options)
{
    CHECK_NULL_VOID(peer && selectionStart && selectionEnd && peer->controller_);
    auto selectionOptions = options ? Converter::OptConvert<SelectionOptions>(*options) : std::nullopt;
    peer->controller_->SetTextSelection(
        Converter::Convert<int32_t>(*selectionStart),
        Converter::Convert<int32_t>(*selectionEnd),
        selectionOptions);
}
void StopEditingImpl(Ark_TextInputController peer)
{
    CHECK_NULL_VOID(peer && peer->controller_);
    peer->controller_->StopEditing();
}
} // TextInputControllerAccessor
const GENERATED_ArkUITextInputControllerAccessor* GetTextInputControllerAccessor()
{
    static const GENERATED_ArkUITextInputControllerAccessor TextInputControllerAccessorImpl {
        TextInputControllerAccessor::DestroyPeerImpl,
        TextInputControllerAccessor::CtorImpl,
        TextInputControllerAccessor::GetFinalizerImpl,
        TextInputControllerAccessor::CaretPositionImpl,
        TextInputControllerAccessor::SetTextSelectionImpl,
        TextInputControllerAccessor::StopEditingImpl,
    };
    return &TextInputControllerAccessorImpl;
}

struct TextInputControllerPeer {
    virtual ~TextInputControllerPeer() = default;
};
}

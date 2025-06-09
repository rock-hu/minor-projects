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
#include "core/interfaces/native/implementation/gesture_group_interface_peer.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace GestureGroupInterfaceAccessor {
void DestroyPeerImpl(Ark_GestureGroupInterface peer)
{
    delete peer;
}
Ark_GestureGroupInterface CtorImpl()
{
    auto gestureMode = GestureMode::Sequence;
    auto peer = new GestureGroupInterfacePeer();
    peer->gesture = AceType::MakeRefPtr<GestureGroup>(gestureMode);
    return peer;
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
Ark_GestureGroupInterface OnCancelImpl(Ark_GestureGroupInterface peer,
                                       const Callback_Void* event)
{
    CHECK_NULL_RETURN(peer && peer->gesture && event, peer);
    auto callback = [arkCallback = CallbackHelper(*event)]() {
        arkCallback.Invoke();
    };
    // peer->gesture->SetOnActionCancelId(std::move(callback));
    return peer;
}
} // GestureGroupInterfaceAccessor
const GENERATED_ArkUIGestureGroupInterfaceAccessor* GetGestureGroupInterfaceAccessor()
{
    static const GENERATED_ArkUIGestureGroupInterfaceAccessor GestureGroupInterfaceAccessorImpl {
        GestureGroupInterfaceAccessor::DestroyPeerImpl,
        GestureGroupInterfaceAccessor::CtorImpl,
        GestureGroupInterfaceAccessor::GetFinalizerImpl,
        GestureGroupInterfaceAccessor::OnCancelImpl,
    };
    return &GestureGroupInterfaceAccessorImpl;
}
}

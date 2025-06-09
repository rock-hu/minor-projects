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
#include "core/interfaces/native/implementation/event_result_peer_impl.h"
#include "core/interfaces/native/utility/converter.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace EventResultAccessor {
void DestroyPeerImpl(Ark_EventResult peer)
{
    CHECK_NULL_VOID(peer);
    peer->handler = nullptr;
    delete peer;
}
Ark_EventResult CtorImpl()
{
    return new EventResultPeer();
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
void SetGestureEventResultImpl(Ark_EventResult peer,
                               Ark_Boolean result)
{
    CHECK_NULL_VOID(peer && peer->handler);
    peer->handler->SetGestureEventResult(
        Converter::Convert<bool>(result)
    );
}
} // EventResultAccessor
const GENERATED_ArkUIEventResultAccessor* GetEventResultAccessor()
{
    static const GENERATED_ArkUIEventResultAccessor EventResultAccessorImpl {
        EventResultAccessor::DestroyPeerImpl,
        EventResultAccessor::CtorImpl,
        EventResultAccessor::GetFinalizerImpl,
        EventResultAccessor::SetGestureEventResultImpl,
    };
    return &EventResultAccessorImpl;
}

}

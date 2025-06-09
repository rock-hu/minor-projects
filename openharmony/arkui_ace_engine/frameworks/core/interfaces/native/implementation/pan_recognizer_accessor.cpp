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
#include "pan_recognizer_peer.h"
#include "pan_gesture_options_peer.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
const GENERATED_ArkUIPanGestureOptionsAccessor* GetPanGestureOptionsAccessor();
namespace PanRecognizerAccessor {
void DestroyPeerImpl(Ark_PanRecognizer peer)
{
    if (peer) {
        delete peer;
    }
}
Ark_PanRecognizer CtorImpl()
{
    return nullptr;
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
Ark_PanGestureOptions GetPanGestureOptionsImpl(Ark_PanRecognizer peer)
{
    CHECK_NULL_RETURN(peer, nullptr);
    auto panGestureOptionsPeer = GetPanGestureOptionsAccessor()->ctor(nullptr);
    auto options = peer->GetOptions();
    CHECK_NULL_RETURN(options, panGestureOptionsPeer);
    panGestureOptionsPeer->handler = options;
    return panGestureOptionsPeer;
}
} // PanRecognizerAccessor
const GENERATED_ArkUIPanRecognizerAccessor* GetPanRecognizerAccessor()
{
    static const GENERATED_ArkUIPanRecognizerAccessor PanRecognizerAccessorImpl {
        PanRecognizerAccessor::DestroyPeerImpl,
        PanRecognizerAccessor::CtorImpl,
        PanRecognizerAccessor::GetFinalizerImpl,
        PanRecognizerAccessor::GetPanGestureOptionsImpl,
    };
    return &PanRecognizerAccessorImpl;
}

struct PanRecognizerPeer {
    virtual ~PanRecognizerPeer() = default;
};
}

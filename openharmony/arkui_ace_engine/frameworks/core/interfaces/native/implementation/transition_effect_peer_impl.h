/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_CLIENT_TRANSITION_EFFECT_PEER_IMPL_H
#define FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_CLIENT_TRANSITION_EFFECT_PEER_IMPL_H
#include "core/components_ng/property/transition_property.h"
#include "core/interfaces/native/utility/peer_utils.h"

struct TransitionEffectPeer {
protected:
    TransitionEffectPeer() = default;
    ~TransitionEffectPeer() = default;
    friend OHOS::Ace::NG::PeerUtils;
public:
    OHOS::Ace::RefPtr<OHOS::Ace::NG::ChainedTransitionEffect> handler;
};
#endif // FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_CLIENT_TRANSITION_EFFECT_PEER_IMPL_H


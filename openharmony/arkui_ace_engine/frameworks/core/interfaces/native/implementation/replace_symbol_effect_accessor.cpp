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
#include "core/interfaces/native/utility/reverse_converter.h"
#include "arkoala_api_generated.h"
#include "core/interfaces/native/implementation/symbol_effect_peer.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace ReplaceSymbolEffectAccessor {
void DestroyPeerImpl(Ark_ReplaceSymbolEffect peer)
{
    CHECK_NULL_VOID(peer);
    delete peer;
}
Ark_ReplaceSymbolEffect CtorImpl(const Opt_EffectScope* scope)
{
    std::optional<OHOS::Ace::ScopeType> optScope;
    if (scope) {
        optScope = Converter::OptConvert<OHOS::Ace::ScopeType>(*scope);
    }
    return new ReplaceSymbolEffectPeer(optScope);
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
Ark_EffectScope GetScopeImpl(Ark_ReplaceSymbolEffect peer)
{
    CHECK_NULL_RETURN(peer, ARK_EFFECT_SCOPE_LAYER);
    CHECK_NULL_RETURN(peer->scope, ARK_EFFECT_SCOPE_LAYER);
    switch (peer->scope.value()) {
        case OHOS::Ace::ScopeType::LAYER:
            return ARK_EFFECT_SCOPE_LAYER;
        case OHOS::Ace::ScopeType::WHOLE:
            return ARK_EFFECT_SCOPE_WHOLE;
        default:
            return ARK_EFFECT_SCOPE_LAYER;
    }
}
void SetScopeImpl(Ark_ReplaceSymbolEffect peer,
                  Ark_EffectScope scope)
{
    CHECK_NULL_VOID(peer);
    peer->scope = Converter::OptConvert<OHOS::Ace::ScopeType>(scope);
}
} // ReplaceSymbolEffectAccessor
const GENERATED_ArkUIReplaceSymbolEffectAccessor* GetReplaceSymbolEffectAccessor()
{
    static const GENERATED_ArkUIReplaceSymbolEffectAccessor ReplaceSymbolEffectAccessorImpl {
        ReplaceSymbolEffectAccessor::DestroyPeerImpl,
        ReplaceSymbolEffectAccessor::CtorImpl,
        ReplaceSymbolEffectAccessor::GetFinalizerImpl,
        ReplaceSymbolEffectAccessor::GetScopeImpl,
        ReplaceSymbolEffectAccessor::SetScopeImpl,
    };
    return &ReplaceSymbolEffectAccessorImpl;
}
}

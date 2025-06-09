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
#include "arkoala_api_generated.h"
#include "transition_effect_peer_impl.h"
#include "core/interfaces/native/utility/reverse_converter.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace TransitionEffectAccessor {

const auto TRANSLATE_TOKEN = "translate";
const auto ROTATE_TOKEN = "rotate";
const auto SCALE_TOKEN = "scale";
const auto OPACITY_TOKEN = "opacity";
const auto MOVE_TOKEN = "move";
const auto ASYMMETRIC_TOKEN = "asymmetric";
const auto IDENTITY_TOKEN = "identity";

void DestroyPeerImpl(Ark_TransitionEffect peer)
{
    CHECK_NULL_VOID(peer);
    peer->handler = nullptr;
    delete peer;
}
Ark_TransitionEffect CtorImpl(const Ark_String* type,
                              const Ark_TransitionEffects* effect)
{
    CHECK_NULL_RETURN(type && effect, nullptr);
    auto valueText = Converter::Convert<std::string>(*type);
    TransitionEffectPeer* peer = new TransitionEffectPeer();
    auto emptyDimension = Dimension();
    if (valueText == TRANSLATE_TOKEN) {
        auto x = Converter::OptConvert<CalcDimension>(effect->translate.x.value);
        auto y = Converter::OptConvert<CalcDimension>(effect->translate.y.value);
        auto z = Converter::OptConvert<CalcDimension>(effect->translate.z.value);
        TranslateOptions translate(x.value_or(emptyDimension), y.value_or(emptyDimension), z.value_or(emptyDimension));
        peer->handler = AceType::MakeRefPtr<ChainedTranslateEffect>(translate);
    } else if (valueText == ROTATE_TOKEN) {
        auto x = Converter::Convert<float>(effect->rotate.x.value);
        auto y = Converter::Convert<float>(effect->rotate.y.value);
        auto z = Converter::Convert<float>(effect->rotate.z.value);
        auto centerX = Converter::OptConvert<CalcDimension>(effect->rotate.centerX.value);
        auto centerY = Converter::OptConvert<CalcDimension>(effect->rotate.centerY.value);
        auto centerZ = Converter::OptConvert<CalcDimension>(effect->rotate.centerZ.value);
        auto perspective = Converter::Convert<float>(effect->rotate.perspective.value);
        auto angle = Converter::OptConvert<float>(effect->rotate.angle);
        RotateOptions rotate(x, y, z, angle.value_or(0),
            centerX.value_or(emptyDimension),
            centerY.value_or(emptyDimension),
            centerZ.value_or(emptyDimension), perspective);
        peer->handler = AceType::MakeRefPtr<ChainedRotateEffect>(rotate);
    } else if (valueText == SCALE_TOKEN) {
        auto x = Converter::Convert<float>(effect->scale.x.value);
        auto y = Converter::Convert<float>(effect->scale.y.value);
        auto z = Converter::Convert<float>(effect->scale.z.value);
        auto centerX = Converter::OptConvert<CalcDimension>(effect->scale.centerX.value);
        auto centerY = Converter::OptConvert<CalcDimension>(effect->scale.centerY.value);
        ScaleOptions scale(x, y, z, centerX.value_or(emptyDimension), centerY.value_or(emptyDimension));
        peer->handler = AceType::MakeRefPtr<ChainedScaleEffect>(scale);
    } else if (valueText == OPACITY_TOKEN) {
        auto opacity = Converter::Convert<float>(effect->opacity);
        peer->handler = AceType::MakeRefPtr<ChainedOpacityEffect>(opacity);
    } else if (valueText == MOVE_TOKEN) {
        auto move = Converter::OptConvert<TransitionEdge>(effect->move);
        peer->handler = AceType::MakeRefPtr<ChainedMoveEffect>(move.value_or(TransitionEdge::TOP));
    } else if (valueText == ASYMMETRIC_TOKEN) {
        CHECK_NULL_RETURN(effect, nullptr);
        auto app = effect->asymmetric.appear;
        CHECK_NULL_RETURN(app, nullptr);
        auto disapp = effect->asymmetric.disappear;
        CHECK_NULL_RETURN(disapp, nullptr);
        peer->handler = AceType::MakeRefPtr<ChainedAsymmetricEffect>(app->handler, disapp->handler);
    } else if (valueText == IDENTITY_TOKEN) {
        peer->handler = AceType::MakeRefPtr<ChainedIdentityEffect>();
    }
    return peer;
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
Ark_TransitionEffect TranslateImpl(const Ark_TranslateOptions* options)
{
    CHECK_NULL_RETURN(options, nullptr);
    Ark_String type = Converter::ArkValue<Ark_String>(TRANSLATE_TOKEN);
    Ark_TransitionEffects effects {
        .translate = *options
    };
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect RotateImpl(const Ark_RotateOptions* options)
{
    CHECK_NULL_RETURN(options, nullptr);
    Ark_String type = Converter::ArkValue<Ark_String>(ROTATE_TOKEN);
    Ark_TransitionEffects effects {
        .rotate = *options
    };
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect ScaleImpl(const Ark_ScaleOptions* options)
{
    CHECK_NULL_RETURN(options, nullptr);
    Ark_String type = Converter::ArkValue<Ark_String>(SCALE_TOKEN);
    Ark_TransitionEffects effects {
        .scale = *options
    };
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect OpacityImpl(const Ark_Number* alpha)
{
    CHECK_NULL_RETURN(alpha, nullptr);
    Ark_String type = Converter::ArkValue<Ark_String>(OPACITY_TOKEN);
    Ark_TransitionEffects effects {
        .opacity = *alpha
    };
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect MoveImpl(Ark_TransitionEdge edge)
{
    Ark_String type = Converter::ArkValue<Ark_String>(MOVE_TOKEN);
    Ark_TransitionEffects effects {
        .move = edge
    };
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect AsymmetricImpl(Ark_TransitionEffect appear,
                                    Ark_TransitionEffect disappear)
{
    CHECK_NULL_RETURN(appear, nullptr);
    CHECK_NULL_RETURN(disappear, nullptr);

    Ark_String type = Converter::ArkValue<Ark_String>(ASYMMETRIC_TOKEN);
    Ark_Literal_TransitionEffect_appear_disappear asymm;
    asymm.appear = appear;
    asymm.disappear = disappear;
    Ark_TransitionEffects effects {
        .asymmetric = asymm
    };
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect AnimationImpl(Ark_TransitionEffect peer,
                                   const Ark_AnimateParam* value)
{
    CHECK_NULL_RETURN(peer, nullptr);
    CHECK_NULL_RETURN(value, nullptr);
    AnimationOption option = Converter::Convert<AnimationOption>(*value);
    auto refOpt = std::make_shared<AnimationOption>(option);
    peer->handler->SetAnimationOption(refOpt);
    return peer;
}
Ark_TransitionEffect CombineImpl(Ark_TransitionEffect peer,
                                 Ark_TransitionEffect transitionEffect)
{
    CHECK_NULL_RETURN(peer, nullptr);
    CHECK_NULL_RETURN(transitionEffect, peer);
    auto lastEffect = peer;
    while (lastEffect->handler->GetNext() != nullptr) {
        lastEffect->handler = lastEffect->handler->GetNext();
    }
    const auto nextPeer = transitionEffect;
    lastEffect->handler->SetNext(nextPeer->handler);
    return peer;
}
Ark_TransitionEffect GetIDENTITYImpl()
{
    Ark_String type = Converter::ArkValue<Ark_String>(IDENTITY_TOKEN);
    Ark_TransitionEffects effects {};
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect GetOPACITYImpl()
{
    auto alpha = Converter::ArkValue<Ark_Number>(static_cast<float>(0.0f));
    Ark_String type = Converter::ArkValue<Ark_String>(OPACITY_TOKEN);
    Ark_TransitionEffects effects {
        .opacity = alpha
    };
    return CtorImpl(&type, &effects);
}
Ark_TransitionEffect GetSLIDEImpl()
{
    Ark_String type = Converter::ArkValue<Ark_String>(MOVE_TOKEN);
    Ark_TransitionEdge startEdge = Converter::ArkValue<Ark_TransitionEdge>(TransitionEdge::START);
    Ark_TransitionEffects startEffects {
        .move = startEdge
    };
    Ark_TransitionEffect appear = CtorImpl(&type, &startEffects);
    Ark_TransitionEdge endEdge = Converter::ArkValue<Ark_TransitionEdge>(TransitionEdge::END);
    Ark_String type1 = Converter::ArkValue<Ark_String>(MOVE_TOKEN);
    Ark_TransitionEffects endEffects {
        .move = endEdge
    };
    Ark_TransitionEffect disappear = CtorImpl(&type1, &endEffects);

    Ark_String asyType = Converter::ArkValue<Ark_String>(ASYMMETRIC_TOKEN);
    Ark_Literal_TransitionEffect_appear_disappear asymm;
    asymm.appear = appear;
    asymm.disappear = disappear;
    Ark_TransitionEffects effects {
        .asymmetric = asymm
    };
    return CtorImpl(&asyType, &effects);
}
Ark_TransitionEffect GetSLIDE_SWITCHImpl()
{
    return {};
}
} // TransitionEffectAccessor
const GENERATED_ArkUITransitionEffectAccessor* GetTransitionEffectAccessor()
{
    static const GENERATED_ArkUITransitionEffectAccessor TransitionEffectAccessorImpl {
        TransitionEffectAccessor::DestroyPeerImpl,
        TransitionEffectAccessor::CtorImpl,
        TransitionEffectAccessor::GetFinalizerImpl,
        TransitionEffectAccessor::TranslateImpl,
        TransitionEffectAccessor::RotateImpl,
        TransitionEffectAccessor::ScaleImpl,
        TransitionEffectAccessor::OpacityImpl,
        TransitionEffectAccessor::MoveImpl,
        TransitionEffectAccessor::AsymmetricImpl,
        TransitionEffectAccessor::AnimationImpl,
        TransitionEffectAccessor::CombineImpl,
        TransitionEffectAccessor::GetIDENTITYImpl,
        TransitionEffectAccessor::GetOPACITYImpl,
        TransitionEffectAccessor::GetSLIDEImpl,
        TransitionEffectAccessor::GetSLIDE_SWITCHImpl,
    };
    return &TransitionEffectAccessorImpl;
}
}

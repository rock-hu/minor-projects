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

struct IndicatorComponentControllerPeer {};

#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/utility/converter.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace IndicatorComponentControllerAccessor {
void DestroyPeerImpl(Ark_IndicatorComponentController peer)
{
}
Ark_IndicatorComponentController CtorImpl()
{
    return nullptr;
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
void ShowNextImpl(Ark_IndicatorComponentController peer)
{
}
void ShowPreviousImpl(Ark_IndicatorComponentController peer)
{
}
void ChangeIndexImpl(Ark_IndicatorComponentController peer,
                     const Ark_Number* index,
                     const Opt_Boolean* useAnimation)
{
}
} // IndicatorComponentControllerAccessor
const GENERATED_ArkUIIndicatorComponentControllerAccessor* GetIndicatorComponentControllerAccessor()
{
    static const GENERATED_ArkUIIndicatorComponentControllerAccessor IndicatorComponentControllerAccessorImpl {
        IndicatorComponentControllerAccessor::DestroyPeerImpl,
        IndicatorComponentControllerAccessor::CtorImpl,
        IndicatorComponentControllerAccessor::GetFinalizerImpl,
        IndicatorComponentControllerAccessor::ShowNextImpl,
        IndicatorComponentControllerAccessor::ShowPreviousImpl,
        IndicatorComponentControllerAccessor::ChangeIndexImpl,
    };
    return &IndicatorComponentControllerAccessorImpl;
}

struct IndicatorComponentControllerPeer {
    virtual ~IndicatorComponentControllerPeer() = default;
};
}

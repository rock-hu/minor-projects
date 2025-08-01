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

namespace OHOS::Ace::NG::GeneratedModifier {
namespace VirtualScrollOptionsAccessor {
void DestroyPeerImpl(Ark_VirtualScrollOptions peer)
{
}
Ark_VirtualScrollOptions CtorImpl()
{
    return nullptr;
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
void OnLazyLoadingImpl(Ark_VirtualScrollOptions peer,
                       const Ark_Number* index)
{
}
Ark_Number OnTotalCountImpl(Ark_VirtualScrollOptions peer)
{
    return {};
}
Opt_Number GetTotalCountImpl(Ark_VirtualScrollOptions peer)
{
    return {};
}
void SetTotalCountImpl(Ark_VirtualScrollOptions peer,
                       const Ark_Number* totalCount)
{
}
Opt_Boolean GetReusableImpl(Ark_VirtualScrollOptions peer)
{
    return {};
}
void SetReusableImpl(Ark_VirtualScrollOptions peer,
                     Ark_Boolean reusable)
{
}
} // VirtualScrollOptionsAccessor
const GENERATED_ArkUIVirtualScrollOptionsAccessor* GetVirtualScrollOptionsAccessor()
{
    static const GENERATED_ArkUIVirtualScrollOptionsAccessor VirtualScrollOptionsAccessorImpl {
        VirtualScrollOptionsAccessor::DestroyPeerImpl,
        VirtualScrollOptionsAccessor::CtorImpl,
        VirtualScrollOptionsAccessor::GetFinalizerImpl,
        VirtualScrollOptionsAccessor::OnLazyLoadingImpl,
        VirtualScrollOptionsAccessor::OnTotalCountImpl,
        VirtualScrollOptionsAccessor::GetTotalCountImpl,
        VirtualScrollOptionsAccessor::SetTotalCountImpl,
        VirtualScrollOptionsAccessor::GetReusableImpl,
        VirtualScrollOptionsAccessor::SetReusableImpl,
    };
    return &VirtualScrollOptionsAccessorImpl;
}

}

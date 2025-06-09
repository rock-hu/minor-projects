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
#include "core/interfaces/native/implementation/permission_request_peer_impl.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace PermissionRequestAccessor {
void DestroyPeerImpl(Ark_PermissionRequest peer)
{
    CHECK_NULL_VOID(peer);
    peer->handler = nullptr;
    delete peer;
}
Ark_PermissionRequest CtorImpl()
{
    return new PermissionRequestPeer();
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
void DenyImpl(Ark_PermissionRequest peer)
{
    CHECK_NULL_VOID(peer && peer->handler);
    peer->handler->Deny();
}
Ark_String GetOriginImpl(Ark_PermissionRequest peer)
{
    std::string result = "";
    CHECK_NULL_RETURN(peer && peer->handler, Converter::ArkValue<Ark_String>(result, Converter::FC));
    result = peer->handler->GetOrigin();
    return Converter::ArkValue<Ark_String>(result, Converter::FC);
}
Array_String GetAccessibleResourceImpl(Ark_PermissionRequest peer)
{
    std::vector<std::string> result;
    CHECK_NULL_RETURN(peer && peer->handler, Converter::ArkValue<Array_String>(result, Converter::FC));
    result = peer->handler->GetResources();
    return Converter::ArkValue<Array_String>(result, Converter::FC);
}
void GrantImpl(Ark_PermissionRequest peer,
               const Array_String* resources)
{
    CHECK_NULL_VOID(peer && peer->handler);
    CHECK_NULL_VOID(resources);
    auto res = Converter::Convert<std::vector<std::string>>(*resources);
    peer->handler->Grant(res);
}
} // PermissionRequestAccessor
const GENERATED_ArkUIPermissionRequestAccessor* GetPermissionRequestAccessor()
{
    static const GENERATED_ArkUIPermissionRequestAccessor PermissionRequestAccessorImpl {
        PermissionRequestAccessor::DestroyPeerImpl,
        PermissionRequestAccessor::CtorImpl,
        PermissionRequestAccessor::GetFinalizerImpl,
        PermissionRequestAccessor::DenyImpl,
        PermissionRequestAccessor::GetOriginImpl,
        PermissionRequestAccessor::GetAccessibleResourceImpl,
        PermissionRequestAccessor::GrantImpl,
    };
    return &PermissionRequestAccessorImpl;
}

struct PermissionRequestPeer {
    virtual ~PermissionRequestPeer() = default;
};
}

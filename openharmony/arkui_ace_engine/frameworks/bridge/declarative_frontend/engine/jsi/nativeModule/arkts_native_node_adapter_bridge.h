/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_NATIVEMODULE_ARKTS_NATIVE_NODE_ADAPTER_BRIDGE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_NATIVEMODULE_ARKTS_NATIVE_NODE_ADAPTER_BRIDGE_H

#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_api_bridge.h"

namespace OHOS::Ace::NG {
class NodeAdapterBridge {
public:
    static ArkUINativeModuleValue CreateNodeAdapter(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue SetCallbacks(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue SetTotalNodeCount(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue GetTotalNodeCount(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue NotifyItemReloaded(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue NotifyItemChanged(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue NotifyItemRemoved(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue NotifyItemInserted(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue NotifyItemMoved(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue GetAllItems(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue AttachNodeAdapter(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue DetachNodeAdapter(ArkUIRuntimeCallInfo* runtimeCallInfo);
};

} // namespace OHOS::Ace::NG
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_NATIVEMODULE_ARKTS_NATIVE_NODE_ADAPTER_BRIDGE_H

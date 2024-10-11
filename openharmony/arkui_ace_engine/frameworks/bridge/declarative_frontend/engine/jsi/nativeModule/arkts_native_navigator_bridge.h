/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_NATIVEMODULE_ARKTS_NATIVE_NAVIGATOR_BRIDGE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_NATIVEMODULE_ARKTS_NATIVE_NAVIGATOR_BRIDGE_H

#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_api_bridge.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_common_bridge.h"

namespace OHOS::Ace::NG {
class NavigatorBridge : public CommonBridge {
public:
    static ArkUINativeModuleValue ResetTarget(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue SetTarget(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue ResetType(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue SetType(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue ResetActive(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue SetActive(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue ResetParams(ArkUIRuntimeCallInfo* runtimeCallInfo);
    static ArkUINativeModuleValue SetParams(ArkUIRuntimeCallInfo* runtimeCallInfo);
};
} // namespace OHOS::Ace::NG

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_NATIVEMODULE_ARKTS_NATIVE_STEPPER_ITEM_BRIDGE_H
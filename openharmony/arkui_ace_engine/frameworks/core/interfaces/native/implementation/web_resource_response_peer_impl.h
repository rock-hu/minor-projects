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
#ifndef FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_WEB_RESOURCE_RESPONSE_PEER_IMPL_H
#define FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_WEB_RESOURCE_RESPONSE_PEER_IMPL_H

#include "core/components/web/web_event.h"
#include "arkoala_api_generated.h"

struct WebResourceResponsePeer {
    OHOS::Ace::RefPtr<OHOS::Ace::WebResponse> handler;
    std::optional<int32_t> responseDataType;
    std::optional<Ark_Resource> responseDataResEx;
};
#endif // FOUNDATION_ARKUI_ACE_ENGINE_FRAMEWORKS_CORE_INTERFACES_NATIVE_IMPL_WEB_RESOURCE_RESPONSE_PEER_IMPL_H
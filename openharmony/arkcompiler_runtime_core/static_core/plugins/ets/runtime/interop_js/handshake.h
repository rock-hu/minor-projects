/**
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

#ifndef PLUGINS_ETS_RUNTIME_INTEROP_JS_HYBRID_HANDSHAKE_H
#define PLUGINS_ETS_RUNTIME_INTEROP_JS_HYBRID_HANDSHAKE_H

#include <node_api.h>

namespace ark::ets::interop::js {

class InteropCtx;

class Handshake {
public:
    static void VmHandshake(napi_env env, InteropCtx *ctx);
};

}  // namespace ark::ets::interop::js

#endif  // PLUGINS_ETS_RUNTIME_INTEROP_JS_HYBRID_HANDSHAKE_H

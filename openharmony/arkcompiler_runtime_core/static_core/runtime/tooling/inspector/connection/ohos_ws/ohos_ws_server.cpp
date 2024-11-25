/**
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

#include "connection/ohos_ws/ohos_ws_server.h"

#include <string_view>

#include "os/thread.h"
#include "utils/logger.h"

namespace ark::tooling::inspector {
[[maybe_unused]] static constexpr std::string_view G_ARK_TS_INSPECTOR_NAME = "ArkEtsDebugger";

bool OhosWsServer::RunOne()
{
    if (!endpoint_.IsConnected() && !endpoint_.AcceptNewConnection()) {
        LOG(WARNING, DEBUGGER) << "Inspector server is unable to establish a new connection, exiting";
        return false;
    }

    auto message = endpoint_.Decode();
    if (!message.empty()) {
        if (Endpoint::IsDecodeDisconnectMsg(message)) {
            LOG(WARNING, DEBUGGER) << "Inspector server received `disconnect`, exiting";
            return false;
        }
        HandleMessage(message);
    }
    return true;
}

bool OhosWsServer::Start([[maybe_unused]] uint32_t port)
{
    bool succeeded = false;

#if !defined(PANDA_TARGET_OHOS)
    succeeded = endpoint_.InitTcpWebSocket(port);
    uint32_t name = port;
#else
    auto pid = os::thread::GetPid();
    std::string name = std::to_string(pid) + std::string(G_ARK_TS_INSPECTOR_NAME);
    succeeded = endpoint_.InitUnixWebSocket(name);
#endif
    if (succeeded) {
        LOG(INFO, DEBUGGER) << "Inspector server listening on " << name;
        return true;
    }
    LOG(ERROR, DEBUGGER) << "Failed to bind Inspector server on " << name;
    return false;
}

bool OhosWsServer::Stop()
{
    // Stop event loop before closing endpoint server.
    Kill();
    endpoint_.Close();
    return true;
}
}  // namespace ark::tooling::inspector

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_TOOLING_PROTOCOL_HANDLER_H
#define ECMASCRIPT_TOOLING_PROTOCOL_HANDLER_H

#include <condition_variable>
#include <functional>
#include <queue>
#include <memory>

#include "protocol_channel.h"

namespace panda::ecmascript::tooling {
class ProtocolHandler final : public ProtocolChannel {
public:
    enum DispatchStatus : int32_t {
        UNKNOWN = 0,
        DISPATCHING,
        DISPATCHED
    };

    ProtocolHandler(std::function<void(const void *, const std::string &)> callback, const EcmaVM *vm)
        : callback_(std::move(callback)), dispatcher_(vm, this), vm_(vm) {}
    ~ProtocolHandler() override = default;

    void WaitForDebugger() override;
    void RunIfWaitingForDebugger() override;
    void ProcessCommand();
    void DispatchCommand(std::string &&msg);
    int32_t GetDispatchStatus();

    void SendResponse(const DispatchRequest &request, const DispatchResponse &response,
                      const PtBaseReturns &result) override;
    void SendNotification(const PtBaseEvents &events) override;

    std::unique_ptr<PtJson> CreateErrorReply(const DispatchResponse &response);
    void SendReply(const PtJson &reply);

private:
    NO_MOVE_SEMANTIC(ProtocolHandler);
    NO_COPY_SEMANTIC(ProtocolHandler);

    std::function<void(const void *, const std::string &)> callback_;
    Dispatcher dispatcher_;

    bool waitingForDebugger_ {false};
    const EcmaVM *vm_ {nullptr};

    std::condition_variable requestQueueCond_;
    std::queue<std::string> requestQueue_;
    std::mutex requestLock_;
    std::atomic<bool> isDispatchingMessage_ {false};
};
}  // namespace panda::ecmascript::tooling

#endif

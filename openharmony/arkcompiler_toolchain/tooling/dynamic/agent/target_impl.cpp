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

#include "agent/target_impl.h"

namespace panda::ecmascript::tooling {
void TargetImpl::DispatcherImpl::Dispatch(const DispatchRequest &request)
{
    Method method = GetMethodEnum(request.GetMethod());
    LOG_DEBUGGER(INFO) << "dispatch [" << request.GetMethod() << "] to TargetImpl";
    switch (method) {
        case Method::SET_AUTO_ATTACH:
            SetAutoAttach(request);
            break;
        default:
            SendResponse(request, DispatchResponse::Fail("Unknown method: " + request.GetMethod()));
            break;
    }
}

TargetImpl::DispatcherImpl::Method TargetImpl::DispatcherImpl::GetMethodEnum(const std::string& method)
{
    if (method == "setAutoAttach") {
        return Method::SET_AUTO_ATTACH;
    } else {
        return Method::UNKNOWN;
    }
}

void TargetImpl::DispatcherImpl::SetAutoAttach(const DispatchRequest &request)
{
    DispatchResponse response = DispatchResponse::Ok();
    SendResponse(request, response);
}
}  // namespace panda::ecmascript::tooling
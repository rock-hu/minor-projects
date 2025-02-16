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

#include "core/components/web/resource/web_resource.h"

namespace OHOS::Ace {

const char WEB_PARAM_NONE[] = "";
const char WEB_PARAM_AND[] = "#HWJS-&-#";
const char WEB_PARAM_VALUE[] = "value";
const char WEB_PARAM_EQUALS[] = "#HWJS-=-#";
const char WEB_PARAM_BEGIN[] = "#HWJS-?-#";
const char WEB_METHOD[] = "method";
const char WEB_EVENT[] = "event";
const char WEB_RESULT_FAIL[] = "fail";

void WebResource::Release(const std::function<void(bool)>& onRelease)
{
    if (id_ == WEB_INVALID_ID) {
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    auto releaseTask = [weakWeb = AceType::WeakClaim(this), weakRes, onRelease] {
        auto webResource = weakWeb.Upgrade();
        auto resRegister = weakRes.Upgrade();
        if (webResource == nullptr || resRegister == nullptr) {
            return;
        }
        bool ret = resRegister->ReleaseResource(webResource->hash_);
        if (ret) {
            webResource->id_ = WEB_INVALID_ID;
            webResource->hash_.clear();
        }

        if (onRelease) {
            onRelease(ret);
        }
    };
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        releaseTask();
    } else {
        platformTaskExecutor.PostTask(releaseTask, "ArkUIWebReleaseResource");
    }
}

double WebResource::GetDoubleParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t pos = param.find(name);
    double result = 0.0;

    if (pos != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len);
        ss >> result;
    }

    return result;
}

int32_t WebResource::GetIntParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t pos = param.find(name);
    int32_t result = 0;

    if (pos != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len);
        ss >> result;
    }

    return result;
}

std::string WebResource::MakeResourceHash() const
{
    std::stringstream hashCode;
    hashCode << type_ << "@" << id_;

    return hashCode.str();
}

std::string WebResource::MakeEventHash(const std::string& event) const
{
    std::string eventHash = hash_;

    eventHash += std::string(WEB_EVENT);
    eventHash += std::string(WEB_PARAM_EQUALS);
    eventHash += event;
    eventHash += std::string(WEB_PARAM_BEGIN);

    return eventHash;
}

std::string WebResource::MakeMethodHash(const std::string& method) const
{
    std::string methodHash = hash_;

    methodHash += std::string(WEB_METHOD);
    methodHash += std::string(WEB_PARAM_EQUALS);
    methodHash += method;
    methodHash += std::string(WEB_PARAM_BEGIN);

    return methodHash;
}

void WebResource::OnError(const std::string& errorCode, const std::string& errorMsg)
{
    if (onError_) {
        onError_(errorCode, errorMsg);
    }
}

void WebResource::CallResRegisterMethod(
    const std::string& method, const std::string& param, const std::function<void(std::string&)>& callback)
{
    if (method.empty()) {
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    platformTaskExecutor.PostTask([method, param, weakRes, callback] {
        auto resRegister = weakRes.Upgrade();
        if (resRegister == nullptr) {
            return;
        }
        std::string result;
        resRegister->OnMethodCall(method, param, result);
        if (callback) {
            callback(result);
        }
    }, "ArkUIWebCallResRegisterMethod");
}

std::string WebResource::GetStringParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t pos = param.find(name);
    std::string result;

    if (pos != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len);
        ss >> result;
    }
    return result;
}

} // namespace OHOS::Ace
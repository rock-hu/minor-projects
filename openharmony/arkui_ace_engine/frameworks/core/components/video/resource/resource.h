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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_RESOURCE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_RESOURCE_H

#include <string>

#include "base/memory/ace_type.h"
#include "core/pipeline/pipeline_base.h"

namespace OHOS::Ace {

constexpr int64_t INVALID_ID = -1;

extern const char PARAM_NONE[];
extern const char PARAM_AND[];
extern const char PARAM_VALUE[];
extern const char PARAM_EQUALS[];
extern const char PARAM_BEGIN[];
extern const char METHOD[];
extern const char EVENT[];
extern const char RESULT_FAIL[];

class Resource : public virtual AceType {
    DECLARE_ACE_TYPE(Resource, AceType);

public:
    using ErrorCallback = std::function<void(const std::string&, const std::string&)>;
    using Method = std::string;

    Resource(const std::string& type, const WeakPtr<PipelineBase>& context, ErrorCallback&& onError)
        : type_(type), context_(context), onError_(std::move(onError))
    {}
    virtual ~Resource() = default;

    virtual void Release(const std::function<void(bool)>& onRelease = nullptr);

    void CallResRegisterMethod(
        const Method& method, const std::string& param, const std::function<void(std::string&)>& callback = nullptr);
    
    void CallSyncResRegisterMethod(
        const Method& method, const std::string& param, const std::function<void(std::string&)>& callback = nullptr);

    static std::map<std::string, std::string> ParseMapFromString(const std::string& param);

    int64_t GetId() const
    {
        return id_;
    }

    const std::string& GetHashCode() const
    {
        return hash_;
    }

protected:
    void GetFloatArrayParam(const std::string& param, const std::string& name, std::vector<float>& matrix) const;
    double GetDoubleParam(const std::string& param, const std::string& name) const;
    int32_t GetIntParam(const std::string& param, const std::string& name) const;
    int64_t GetInt64Param(const std::string& param, const std::string& name) const;
    std::string MakeResourceHash() const;
    std::string MakeEventHash(const std::string& event) const;
    std::string MakeMethodHash(const std::string& method) const;
    bool IsResultSuccess(const std::string& result) const;

    void OnError(const std::string& errorCode, const std::string& errorMsg);

    int64_t id_ = INVALID_ID;
    std::string hash_;
    std::string type_;
    WeakPtr<PipelineBase> context_;
    ErrorCallback onError_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_RESOURCE_H

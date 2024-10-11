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

#ifndef OHOS_ARKCOMPILER_AOTCOMPILER_CLIENT_H
#define OHOS_ARKCOMPILER_AOTCOMPILER_CLIENT_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

#include "iaot_compiler_interface.h"
#include "singleton.h"

namespace OHOS::ArkCompiler {
class AotCompilerClient {
public:
    AotCompilerClient();
    virtual ~AotCompilerClient() = default;
    static AotCompilerClient &GetInstance();
    int32_t AotCompiler(const std::unordered_map<std::string, std::string> &argsMap,
                        std::vector<int16_t> &sigData);
    int32_t StopAotCompiler();
    int32_t NeedReCompile(const std::string& oldVersion, bool& sigData);
    int32_t GetAOTVersion(std::string& sigData);
    void OnLoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject);
    void OnLoadSystemAbilityFail();
    void AotCompilerOnRemoteDied(const wptr<IRemoteObject> &remoteObject);

private:
    sptr<IAotCompilerInterface> GetAotCompilerProxy();
    bool LoadAotCompilerService();
    void SetAotCompiler(const sptr<IRemoteObject> &remoteObject);
    sptr<IAotCompilerInterface> GetAotCompiler();
    class AotCompilerDiedRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };

private:
    std::condition_variable loadSaCondition_;
    std::mutex loadSaMutex_;
    bool loadSaFinished_ {false};
    std::mutex mutex_;
    sptr<IAotCompilerInterface> aotCompilerProxy_ = nullptr;
    sptr<AotCompilerDiedRecipient> aotCompilerDiedRecipient_ = nullptr;
    DISALLOW_COPY_AND_MOVE(AotCompilerClient);
};
} // namespace OHOS::ArkCompiler
#endif // OHOS_ARKCOMPILER_AOTCOMPILER_CLIENT_H
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
#ifndef FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_ENTRANCE_APS_MONITOR_IMPL_H
#define FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_ENTRANCE_APS_MONITOR_IMPL_H

#include "base/utils/aps_monitor.h"

namespace OHOS::Ace {
using SetSceneFunc = void (*)(std::string pkgName, std::string sceneName, uint32_t state);
class ApsMonitorImpl final : public ApsMonitor {
public:
    explicit ApsMonitorImpl(int32_t instanceId) : instanceId_(instanceId) {}
    ~ApsMonitorImpl() override;
    void SetApsScene(const std::string& sceneName, bool onOff) override;

private:
    void LoadApsFuncOnce();
    static const std::set<std::string> apsScenes;
    void* loadfilehandle_ = nullptr;
    SetSceneFunc setFunc_ = nullptr;
    bool isloadapsfunc_ = false;
    int32_t instanceId_ = -1;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_ENTRANCE_APS_MONITOR_IMPL_H
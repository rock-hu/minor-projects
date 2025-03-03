/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "adapter/ohos/entrance/ui_session/ui_session_manager_ohos.h"

namespace OHOS::Ace {
std::mutex UiSessionManager::mutex_;
std::shared_mutex UiSessionManager::reportObjectMutex_;
constexpr int32_t ONCE_IPC_SEND_DATA_MAX_SIZE = 131072;
UiSessionManager* UiSessionManager::GetInstance()
{
    static UiSessionManagerOhos instance;
    return &instance;
}

void UiSessionManagerOhos::ReportClickEvent(const std::string& data)
{
    std::shared_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr) {
            reportService->ReportClickEvent(data);
        } else {
            LOGW("report click event failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::ReportSearchEvent(const std::string& data)
{
    std::shared_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr) {
            reportService->ReportSearchEvent(data);
        } else {
            LOGW("report search event failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::ReportRouterChangeEvent(const std::string& data)
{
    std::shared_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr) {
            reportService->ReportRouterChangeEvent(data);
        } else {
            LOGW("report switch event failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::ReportComponentChangeEvent(const std::string& key, const std::string& value)
{
    std::shared_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr && GetComponentChangeEventRegistered()) {
            auto data = InspectorJsonUtil::Create();
            data->Put(key.data(), value.data());
            reportService->ReportComponentChangeEvent(data->ToString());
        } else {
            LOGW("report component change event failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::ReportComponentChangeEvent(
    int32_t nodeId, const std::string& key, const std::shared_ptr<InspectorJsonValue>& value)
{
    std::shared_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr && GetComponentChangeEventRegistered()) {
            auto data = InspectorJsonUtil::Create();
            data->Put("nodeId", nodeId);
            data->Put(key.data(), value->ToString().data());
            reportService->ReportComponentChangeEvent(data->ToString());
        } else {
            LOGW("report component event failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::ReportWebUnfocusEvent(int64_t accessibilityId, const std::string& data)
{
    auto jsonValue = InspectorJsonUtil::Create(true);
    jsonValue->Put("id", accessibilityId);
    jsonValue->Put("$type", "web");
    jsonValue->Put("text", data.c_str());
    std::shared_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr) {
            reportService->ReportWebUnfocusEvent(accessibilityId, jsonValue->ToString());
        } else {
            LOGW("report web unfocus event failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::SaveReportStub(sptr<IRemoteObject> reportStub, int32_t processId)
{
    // add death callback
    auto uiReportProxyRecipient = new UiReportProxyRecipient([processId, this]() {
        std::unique_lock<std::shared_mutex> reportLock(reportObjectMutex_);
        LOGW("agent process dead,processId:%{public}d", processId);
        // reportMap remove this processId
        this->reportObjectMap_.erase(processId);
    });
    reportStub->AddDeathRecipient(uiReportProxyRecipient);
    std::unique_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    reportObjectMap_[processId] = reportStub;
}

void UiSessionManagerOhos::SetClickEventRegistered(bool status)
{
    if (status) {
        clickEventRegisterProcesses_.fetch_add(1);
    } else {
        clickEventRegisterProcesses_.fetch_sub(1);
    }
}

void UiSessionManagerOhos::SetSearchEventRegistered(bool status)
{
    if (status) {
        searchEventRegisterProcesses_.fetch_add(1);
    } else {
        searchEventRegisterProcesses_.fetch_sub(1);
    }
}

void UiSessionManagerOhos::SetRouterChangeEventRegistered(bool status)
{
    if (status) {
        routerChangeEventRegisterProcesses_.fetch_add(1);
    } else {
        routerChangeEventRegisterProcesses_.fetch_sub(1);
    }
}

void UiSessionManagerOhos::SetComponentChangeEventRegistered(bool status)
{
    if (status) {
        componentChangeEventRegisterProcesses_.fetch_add(1);
    } else {
        componentChangeEventRegisterProcesses_.fetch_sub(1);
    }
}

bool UiSessionManagerOhos::GetClickEventRegistered()
{
    return clickEventRegisterProcesses_.load() > 0 ? true : false;
}

bool UiSessionManagerOhos::GetSearchEventRegistered()
{
    return searchEventRegisterProcesses_.load() > 0 ? true : false;
}

bool UiSessionManagerOhos::GetRouterChangeEventRegistered()
{
    return routerChangeEventRegisterProcesses_.load() > 0 ? true : false;
}

bool UiSessionManagerOhos::GetComponentChangeEventRegistered()
{
    return componentChangeEventRegisterProcesses_.load() > 0 ? true : false;
}

void UiSessionManagerOhos::GetInspectorTree()
{
    WebTaskNumsChange(1);
    std::unique_lock<std::mutex> lock(mutex_);
    jsonValue_ = InspectorJsonUtil::Create(true);
    webTaskNums_.store(0);
    inspectorFunction_();
}

void UiSessionManagerOhos::SaveInspectorTreeFunction(InspectorFunction&& function)
{
    std::unique_lock<std::mutex> lock(mutex_);
    inspectorFunction_ = std::move(function);
}

void UiSessionManagerOhos::AddValueForTree(int32_t id, const std::string& value)
{
    std::unique_lock<std::mutex> lock(mutex_);
    std::string key = std::to_string(id);
    if (jsonValue_->Contains(key)) {
        jsonValue_->Replace(key.c_str(), value.c_str());
    } else {
        jsonValue_->Put(key.c_str(), value.c_str());
    }
}

void UiSessionManagerOhos::WebTaskNumsChange(int32_t num)
{
    webTaskNums_.fetch_add(num);
    if (webTaskNums_.load() == 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        std::string data = jsonValue_->ToString();
        lock.unlock();
        ReportInspectorTreeValue(data);
    }
}

void UiSessionManagerOhos::ReportInspectorTreeValue(const std::string& data)
{
    std::shared_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr) {
            size_t partSize = data.size() / ONCE_IPC_SEND_DATA_MAX_SIZE;
            for (size_t i = 0; i <= partSize; i++) {
                if (i != partSize) {
                    reportService->ReportInspectorTreeValue(
                        data.substr(i * ONCE_IPC_SEND_DATA_MAX_SIZE, ONCE_IPC_SEND_DATA_MAX_SIZE), i + 1, false);
                } else {
                    reportService->ReportInspectorTreeValue(data.substr(i * ONCE_IPC_SEND_DATA_MAX_SIZE), i + 1, true);
                }
            }
        } else {
            LOGW("report component event failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::NotifyAllWebPattern(bool isRegister)
{
    std::unique_lock<std::mutex> lock(mutex_);
    webFocusEventRegistered = isRegister;
    notifyWebFunction_(isRegister);
}

void UiSessionManagerOhos::SaveRegisterForWebFunction(NotifyAllWebFunction&& function)
{
    std::unique_lock<std::mutex> lock(mutex_);
    notifyWebFunction_ = std::move(function);
}

bool UiSessionManagerOhos::GetWebFocusRegistered()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return webFocusEventRegistered;
}

void UiSessionManagerOhos::OnRouterChange(const std::string& path, const std::string& event)
{
    if (GetRouterChangeEventRegistered()) {
        auto value = InspectorJsonUtil::Create(true);
        value->Put("path", path.c_str());
        value->Put("event", event.c_str());
        ReportRouterChangeEvent(value->ToString());
    }
}

void UiSessionManagerOhos::SaveBaseInfo(const std::string& info)
{
    std::unique_lock<std::mutex> lock(mutex_);
    baseInfo_ = info;
}

void UiSessionManagerOhos::SendBaseInfo(int32_t processId)
{
    std::unique_lock<std::shared_mutex> reportLock(reportObjectMutex_);
    auto reportService = iface_cast<ReportService>(reportObjectMap_[processId]);
    reportLock.unlock();
    if (reportService != nullptr) {
        std::unique_lock<std::mutex> lock(mutex_);
        reportService->SendBaseInfo(baseInfo_);
    }
}

void UiSessionManagerOhos::SaveGetPixelMapFunction(GetPixelMapFunction&& function)
{
    getPixelMapFunction_ = std::move(function);
}

void UiSessionManagerOhos::SaveTranslateManager(std::shared_ptr<UiTranslateManager> uiTranslateManager)
{
    translateManager_ = uiTranslateManager;
}

void UiSessionManagerOhos::GetWebViewLanguage()
{
    if (translateManager_) {
        translateManager_->GetWebViewCurrentLanguage();
    } else {
        LOGE("translateManager is nullptr ,translate failed");
    }
}

void UiSessionManagerOhos::RegisterPipeLineGetCurrentPageName(const std::function<std::string()>&& callback)
{
    pipelineContextPageNameCallback_ = std::move(callback);
}

void UiSessionManagerOhos::GetCurrentPageName()
{
    if (pipelineContextPageNameCallback_) {
        auto result = pipelineContextPageNameCallback_();
        SendCurrentPageName(result);
    }
}

void UiSessionManagerOhos::SendCurrentPageName(const std::string result)
{
    for (auto pair : reportObjectMap_) {
        auto reportService = iface_cast<ReportService>(pair.second);
        if (reportService != nullptr) {
            reportService->SendCurrentPageName(result);
        } else {
            LOGW("report send current page name failed,process id:%{public}d", pair.first);
        }
    }
}

void UiSessionManagerOhos::SaveProcessId(std::string key, int32_t id)
{
    processMap_[key] = id;
}

void UiSessionManagerOhos::SendCurrentLanguage(std::string result)
{
    auto reportService = iface_cast<ReportService>(reportObjectMap_[processMap_["translate"]]);
    if (reportService) {
        reportService->SendCurrentLanguage(result);
    } else {
    }
}
void UiSessionManagerOhos::GetWebTranslateText(std::string extraData, bool isContinued)
{
    if (translateManager_) {
        translateManager_->GetTranslateText(extraData, isContinued);
    } else {
        LOGE("translateManager is nullptr ,translate failed");
    }
}

void UiSessionManagerOhos::SendWebTextToAI(int32_t nodeId, std::string res)
{
    auto reportService = iface_cast<ReportService>(reportObjectMap_[processMap_["translate"]]);
    if (reportService != nullptr) {
        reportService->SendWebText(nodeId, res);
    } else {
        LOGW("report component event failed,process id:%{public}d", processMap_["translate"]);
    }
}

void UiSessionManagerOhos::SendTranslateResult(
    int32_t nodeId, std::vector<std::string> results, std::vector<int32_t> ids)
{
    if (translateManager_) {
        translateManager_->SendTranslateResult(nodeId, results, ids);
    } else {
        LOGE("translateManager is nullptr ,translate failed");
    }
}

void UiSessionManagerOhos::SendTranslateResult(int32_t nodeId, std::string res)
{
    if (translateManager_) {
        translateManager_->SendTranslateResult(nodeId, res);
    } else {
        LOGE("translateManager is nullptr ,translate failed");
    }
}

void UiSessionManagerOhos::ResetTranslate(int32_t nodeId)
{
    if (translateManager_) {
        translateManager_->ResetTranslate(nodeId);
    } else {
        LOGE("translateManager is nullptr ,translate failed");
    }
}

void UiSessionManagerOhos::GetPixelMap()
{
    if (getPixelMapFunction_) {
        getPixelMapFunction_();
    }
}

void UiSessionManagerOhos::SendPixelMap(std::vector<std::pair<int32_t, std::shared_ptr<Media::PixelMap>>> maps)
{
    auto reportService = iface_cast<ReportService>(reportObjectMap_[processMap_["pixel"]]);
    if (reportService != nullptr && translateManager_) {
        reportService->SendShowingImage(maps);
        translateManager_->ClearMap();
    } else {
        LOGW("report component event failed,process id:%{public}d", processMap_["pixel"]);
    }
}
} // namespace OHOS::Ace

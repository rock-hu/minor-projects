/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <dlfcn.h>

#include "base/ressched/ressched_report.h"

namespace OHOS::Ace {
namespace {
const std::string RES_SCHED_CLIENT_SO = "libressched_client.z.so";
}

ReportDataFunc LoadReportDataFunc()
{
    auto handle = dlopen(RES_SCHED_CLIENT_SO.c_str(), RTLD_NOW);
    CHECK_NULL_RETURN(handle, nullptr);
    auto func = reinterpret_cast<ReportDataFunc>(dlsym(handle, "ReportData"));
    if (func == nullptr) {
        LOGW("dlsym function ReportData failed!");
        dlclose(handle);
        return nullptr;
    }
    LOGI("dlsym function ReportData success.");
    return func;
}

ReportSyncEventFunc LoadReportSyncEventFunc()
{
    auto handle = dlopen(RES_SCHED_CLIENT_SO.c_str(), RTLD_NOW);
    CHECK_NULL_RETURN(handle, nullptr);
    auto func = reinterpret_cast<ReportSyncEventFunc>(dlsym(handle, "ReportSyncEvent"));
    if (func == nullptr) {
        LOGW("dlsym function ReportSyncEvent failed!");
        dlclose(handle);
        return nullptr;
    }
    LOGI("dlsym function ReportSyncEvent success.");
    return func;
}
} // namespace OHOS::Ace

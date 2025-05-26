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

#include <chrono>
#include <thread>
#include <sched.h>
#include <hilog/log.h>
#include "napi/native_api.h"
#include "qos/qos.h"

const unsigned int LOG_PRINT_DOMAIN = 0xFF00;
constexpr int DEPTH = 34;
constexpr int TASKS = 3; // Number of loaded threads
constexpr long long ONE = 1;
constexpr long long TWO = 2;
constexpr int BOUND = 20000;

static bool g_addLoad = false;    // if loaded threads is add
static double g_durationTime = 0; // Calculate task time consumption

static int g_mask = 2; // bind cpu
static int *g_affinity = &g_mask;

// [Start do_fid]
// Perform Fibonacci sequence calculations
long long DoFib(double n) {
    if (n == ONE) {
        return ONE;
    }
    if (n == TWO) {
        return TWO;
    }
    return DoFib(n - ONE) + DoFib(n - TWO);
}

void SetQoS(QoS_Level level) {
    // set QoS level
    int ret = OH_QoS_SetThreadQoS(level);
    if (!ret) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "QoS", "set qos level success.");
        //  query qos level
        QoS_Level queryLevel = QOS_DEFAULT;
        ret = OH_QoS_GetThreadQoS(&queryLevel);
        if (!ret) {
            OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "QoS", "the qos level of current thread : %{public}d",
                         queryLevel);
        } else {
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "get qos level failed.");
            return;
        }
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "get level qos failed!");
        return;
    }

    // bind cpu
    cpu_set_t mask;
    CPU_SET(*g_affinity, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) != 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "bind qos thread failed");
        return;
    }
    auto startTime = std::chrono::system_clock::now();
    // Execute computational tasks
    long long res = DoFib(DEPTH);
    auto endTime = std::chrono::system_clock::now();
    g_durationTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "QoS", "calculate res is: %{public}llu", res);

    // Reset QoS level
    ret = OH_QoS_ResetThreadQoS();
    if (!ret) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "QoS", "reset qos level success.");
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "reset qos level failed!");
        return;
    }

    // after reset QoS, query QoS again will fail
    QoS_Level queryLevelTwo;
    ret = OH_QoS_GetThreadQoS(&queryLevelTwo);
    if (!ret) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "QoS", "the qos level after: %{public}d", queryLevelTwo);
        return;
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "querry qos level failed after reset.");
        return;
    }
}
// [End do_fid]

// [Start add_loads]
// the Load task
void AddLoads(int n) {
    if (!n) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "invalid input.");
        return;
    }

    // set QoS level
    int ret = OH_QoS_SetThreadQoS(QoS_Level::QOS_BACKGROUND);
    if (ret) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "set load thread QoS level failed.");
        return;
    }

    // bind cpu
    cpu_set_t mask;
    CPU_SET(*g_affinity, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) != 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "QoS", "bind load thread failed");
        return;
    }
    // Perform load calculation
    for (int i = 0; i < BOUND; i++) {
        for (int j = 0; j < BOUND; j++) {
            int x = (i + j) - n;
            printf("%d", x);
        }
    }
    // reset load flag
    g_addLoad = false;
}
// [End add_loads]

// [Start high_qos_calculate]
static napi_value highQoSCalculate(napi_env env, napi_callback_info info) {
    g_durationTime = 0;
    // Simulate system load
    if (!g_addLoad) {
        std::vector<std::thread> loadThreads;
        for (int i = 0; i < TASKS; i++) {
            // Activate threads to execute load tasks
            loadThreads.emplace_back(std::thread(AddLoads, TASKS));
            loadThreads[i].detach();
        }
        g_addLoad = true;
    }
    // set QOS_USER_INTERACTIVE level
    QoS_Level level = QoS_Level::QOS_USER_INTERACTIVE;
    std::thread task(SetQoS, level);
    task.join();

    // Return calculation time
    napi_value res;
    napi_create_double(env, g_durationTime, &res);
    return res;
}
// [End high_qos_calculate]

// [Start low_qos_calculate]
static napi_value lowQoSCalculate(napi_env env, napi_callback_info info) {
    g_durationTime = 0;
    // Simulate system load
    if (!g_addLoad) {
        std::vector<std::thread> loadThreads;
        for (int i = 0; i < TASKS; i++) {
            // Activate threads to execute load tasks
            loadThreads.emplace_back(std::thread(AddLoads, TASKS));
            loadThreads[i].detach();
        }
        g_addLoad = true;
    }

    // set QOS_BACKGROUND level
    QoS_Level level = QoS_Level::QOS_BACKGROUND;
    std::thread task(SetQoS, level);
    task.join();

    // Return calculation time
    napi_value res;
    napi_create_double(env, g_durationTime, &res);
    return res;
}
// [End low_qos_calculate]

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"useHighQos", nullptr, highQoSCalculate, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"useLowQos", nullptr, lowQoSCalculate, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) { napi_module_register(&demoModule); }
/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef DPROF_IPC_MESSAGE_PROTOCOL_H
#define DPROF_IPC_MESSAGE_PROTOCOL_H

#include <vector>
#include <string>

namespace ark::dprof::ipc::protocol {
// NOLINTNEXTLINE(modernize-avoid-c-arrays)
const char VERSION[] = "v1";

struct Version {
    std::string version;
};
const size_t VERSION_FCOUNT = 1;

struct AppInfo {
    std::string appName {};
    uint64_t hash {};
    uint32_t pid {};
};
const size_t APP_INFO_FCOUNT = 3;

struct FeatureData {
    std::string name;
    std::vector<uint8_t> data;
};
const size_t FEATURE_DATA_FCOUNT = 2;
}  // namespace ark::dprof::ipc::protocol

#endif  // DPROF_IPC_MESSAGE_PROTOCOL_H

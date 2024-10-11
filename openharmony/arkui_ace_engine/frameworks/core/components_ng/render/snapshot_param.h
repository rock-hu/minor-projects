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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_SNAPSHOT_PARAM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_SNAPSHOT_PARAM_H

#include <string>
#include <utility>

namespace OHOS::Ace::NG {

constexpr float DEFAULT_SNAPSHOT_SCALE = 1.f;
constexpr int32_t DEFAULT_DELAY_TIME = 300;

struct SnapshotOptions {
    float scale;
    bool waitUntilRenderFinished;
    explicit SnapshotOptions(float scale = DEFAULT_SNAPSHOT_SCALE, bool waitUntilRenderFinished = false)
        : scale(scale), waitUntilRenderFinished(waitUntilRenderFinished) {}
    std::string ToString() const
    {
        return "{" + std::to_string(scale) + ", " + (waitUntilRenderFinished ? "true}" : "false}");
    }
};
struct SnapshotParam {
    int32_t delay;
    bool checkImageStatus;
    SnapshotOptions options;
    explicit SnapshotParam(int32_t delay = DEFAULT_DELAY_TIME, bool checkImageStatus = false,
        SnapshotOptions options = SnapshotOptions()) : delay(delay), checkImageStatus(checkImageStatus),
        options(options) {}
    std::string ToString() const
    {
        return "{" + std::to_string(delay) + ", " + (checkImageStatus ? "true" : "false") + ", " +
            std::to_string(options.scale) + ", " + (options.waitUntilRenderFinished ? "true}" : "false}");
    }
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_LINE_PAINTER_H
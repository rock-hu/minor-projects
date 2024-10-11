/**
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

#ifndef PANDA_RUNTIME_TOOLING_TOOLS_H
#define PANDA_RUNTIME_TOOLING_TOOLS_H

#include "libpandabase/macros.h"

namespace ark::tooling {

namespace sampler {
class Sampler;
}  // namespace sampler

class Tools {
public:
    Tools() = default;
    ~Tools() = default;

    void CreateSamplingProfiler();
    sampler::Sampler *GetSamplingProfiler();
    bool StartSamplingProfiler(const std::string &asptFilename, uint32_t interval);
    void StopSamplingProfiler();

private:
    NO_COPY_SEMANTIC(Tools);
    NO_MOVE_SEMANTIC(Tools);

    sampler::Sampler *sampler_ {nullptr};
};

}  // namespace ark::tooling

#endif  // PANDA_RUNTIME_TOOLING_TOOLS_H

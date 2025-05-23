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
#ifndef PANDA_RUNTIME_MEM_MEM_STATS_ADDITIONAL_INFO_H
#define PANDA_RUNTIME_MEM_MEM_STATS_ADDITIONAL_INFO_H

#include <array>

#include "runtime/include/mem/panda_containers.h"
#include "runtime/mem/mem_stats.h"

namespace ark::mem {

/// Implementation of MemStats with additional info about memory.
class MemStatsAdditionalInfo : public MemStats<MemStatsAdditionalInfo> {
    enum StatType { BYTES_ALLOCATED = 0, BYTES_FREED, MAX_FOOTPRINT, OBJECTS_ALLOCATED, STAT_TYPE_NUM };

public:
    NO_COPY_SEMANTIC(MemStatsAdditionalInfo);
    NO_MOVE_SEMANTIC(MemStatsAdditionalInfo);

    MemStatsAdditionalInfo() = default;

    PandaString GetAdditionalStatistics();

    ~MemStatsAdditionalInfo() override = default;

private:
};

extern template class MemStats<MemStatsAdditionalInfo>;
}  // namespace ark::mem
#endif  // PANDA_RUNTIME_MEM_MEM_STATS_ADDITIONAL_INFO_H

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "common_components/common_runtime/src/mutator/satb_buffer.h"
#include "common_components/common_runtime/src/heap/allocator/region_space.h"

#include "common_components/common_runtime/src/base/immortal_wrapper.h"

namespace panda {
static ImmortalWrapper<SatbBuffer> g_instance;

SatbBuffer& SatbBuffer::Instance() noexcept { return *g_instance; }

bool SatbBuffer::ShouldEnqueue(const BaseObject* obj)
{
    if (UNLIKELY_CC(obj == nullptr)) {
        return false;
    }
    if (RegionSpace::IsNewObjectSinceTrace(obj)) {
        return false;
    }
    if (RegionSpace::IsMarkedObject(obj)) {
        return false;
    }
    return !RegionSpace::EnqueueObject(obj);
}
} // namespace panda

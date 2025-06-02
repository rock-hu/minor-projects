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

#include "common_components/base_runtime/base_runtime_param.h"

#include "common_components/common_runtime/src/base/globals.h"
#include "common_components/platform/cpu.h"

namespace panda {
size_t BaseRuntimeParam::InitHeapSize()
{
    size_t systemSize = PhysicalSize();
    size_t initHeapSize = systemSize > 1 * GB ? std::min(systemSize * 0.9 / KB, 3.6 * MB) : 64 * KB;
    return initHeapSize;
}

/**
 * Determine the default stack size and heap size according to system memory.
 * If system memory size is less then 1GB, heap size is 64MB and stack size is 64KB.
 * Otherwise heap size is 256MB and stack size is 1MB.
 */
RuntimeParam BaseRuntimeParam::DefaultRuntimeParam()
{
    RuntimeParam param;

#define INIT_DEFAULT_PARAM(key, subKey, type, minValue, maxValue, defaultValue) \
        param.key.subKey = (defaultValue)

    RUNTIME_PARAM_LIST(INIT_DEFAULT_PARAM);
#undef INIT_DEFAULT_PARAM

    return param;
}
} // namespace panda

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

#include "ecmascript/platform/parameters.h"

namespace panda::ecmascript {
    uint64_t GetImportDuration(uint64_t defaultTime)
    {
        return defaultTime;
    }

    size_t GetPoolSize(size_t defaultSize)
    {
        return defaultSize;
    }

    bool IsEnableCMCGC(bool defaultValue)
    {
        return defaultValue;
    }

    uint64_t GetCMCMaxGarbageCacheSize(uint64_t defaultSize)
    {
        return defaultSize;
    }
}  // namespace panda::ecmascript

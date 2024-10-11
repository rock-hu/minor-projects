/**
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_PROFILING_INL_H
#define PANDA_PROFILING_INL_H

#include "profiling.h"
#include "runtime/profiling/generated/profiling_includes.h"
#include "runtime/include/profiling_gen.h"

namespace ark::profiling {

inline void ClearProfile([[maybe_unused]] const BytecodeInstruction &inst, [[maybe_unused]] Method *method,
                         [[maybe_unused]] PandaVM *vm, [[maybe_unused]] ark::panda_file::SourceLang lang)
{
    // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
    switch (lang) {
#include "runtime/profiling/generated/clear_profile.h"
        default:
            break;
    }
}

}  // namespace ark::profiling

#endif  // PANDA_PROFILING_INL_H
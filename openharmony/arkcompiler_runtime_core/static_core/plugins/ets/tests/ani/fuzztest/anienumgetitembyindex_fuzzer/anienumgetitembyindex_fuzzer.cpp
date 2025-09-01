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

#include "ani.h"
#include "anienumgetitembyindex_fuzzer.h"
#include "test_common.h"
#include <cstddef>
#include <cstdint>
// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays, readability-magic-numbers)

namespace OHOS {
void AniEnumGetItemByIndexFuzzTest(const char *data, size_t size)
{
    if (size <= 0U) {
        return;
    }

    if (size > MAX_INPUT_SIZE) {
        size = MAX_INPUT_SIZE;
    }
    AniFuzzEngine *engine = AniFuzzEngine::GetInstance();
    ani_env *env {};
    engine->GetAniEnv(&env);
    ani_enum e {};
    env->FindEnum("std.core.LogLevel", &e);
    ani_enum_item eItem {};
    auto index = static_cast<ani_size>(static_cast<unsigned int>(data[0]));
    env->Enum_GetEnumItemByIndex(e, index, &eItem);
}
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AniEnumGetItemByIndexFuzzTest(reinterpret_cast<const char *>(data), size);
    return 0;
}

// NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays, readability-magic-numbers)

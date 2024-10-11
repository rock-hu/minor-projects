/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef LIBPANDABASE_GLOBALS_H
#define LIBPANDABASE_GLOBALS_H

#include <cstdint>

namespace panda {

static constexpr unsigned BITS_PER_BYTE = 8;
static constexpr unsigned BITS_PER_UINT16 = sizeof(uint16_t) * BITS_PER_BYTE;
static constexpr unsigned BITS_PER_UINT32 = sizeof(uint32_t) * BITS_PER_BYTE;
static constexpr unsigned BITS_PER_UINT64 = sizeof(uint64_t) * BITS_PER_BYTE;
static constexpr unsigned BITS_PER_INTPTR = sizeof(intptr_t) * BITS_PER_BYTE;

static constexpr unsigned BITS_PER_BYTE_LOG2 = 3;

static constexpr uint32_t PERCENT_100_U32 = 100U;
static constexpr double PERCENT_100_D = 100.0;

#ifndef NDEBUG
static constexpr bool DEBUG_BUILD = true;
#else
static constexpr bool DEBUG_BUILD = false;
#endif

}  // namespace panda

#endif  // LIBPANDABASE_GLOBALS_H

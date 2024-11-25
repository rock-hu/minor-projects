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

#ifndef PANDA_LIBPANDABASE_UTILS_UTILS_H
#define PANDA_LIBPANDABASE_UTILS_UTILS_H

#include "libpandabase/macros.h"

#include <limits>

namespace ark {
// ----------------------------------------------------------------------------
// User-defined suffixes
constexpr int operator""_I(unsigned long long v)  // NOLINT(google-runtime-int)
{
    if (v > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {  // NOLINT(google-runtime-int)
        UNREACHABLE_CONSTEXPR();
    }
    return static_cast<int>(v);
}

constexpr double operator""_D(long double v)
{
    if (v < static_cast<long double>(std::numeric_limits<double>::lowest()) ||
        v > static_cast<long double>(std::numeric_limits<double>::max())) {
        UNREACHABLE_CONSTEXPR();
    }
    return static_cast<double>(v);
}

// General helper functions

// Returns the value (0 .. 15) of a hexadecimal character c.
// If c is not a legal hexadecimal character, returns a value < 0.
// CC-OFFNXT(G.FUD.06) solid logic
inline uint32_t HexValue(uint32_t c)
{
    constexpr uint32_t BASE16 = 16;
    constexpr uint32_t BASE10 = 10;
    constexpr uint32_t MASK = 0x20;

    c -= '0';
    if (static_cast<unsigned>(c) < BASE10) {
        return c;
    }
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    c = (c | MASK) - ('a' - '0');
    if (static_cast<unsigned>(c) < (BASE16 - BASE10)) {
        return c + BASE10;
    }
    return -1;
}

PANDA_PUBLIC_API uint32_t CountDigits(uint64_t v);

}  // namespace ark
#endif  // PANDA_LIBPANDABASE_UTILS_UTILS_H

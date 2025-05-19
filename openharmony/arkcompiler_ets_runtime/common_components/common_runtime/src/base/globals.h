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

#ifndef ARK_COMMON_GLOBALS_H
#define ARK_COMMON_GLOBALS_H

#include <cstddef>

#include "common_interfaces/base/common.h"

namespace panda {
// Time Factors
constexpr uint64_t TIME_FACTOR = 1000LL;
constexpr uint64_t SECOND_TO_NANO_SECOND = TIME_FACTOR * TIME_FACTOR * TIME_FACTOR;
constexpr uint64_t MICRO_SECOND_TO_NANO_SECOND = TIME_FACTOR;
constexpr uint64_t MILLI_SECOND_TO_NANO_SECOND = TIME_FACTOR * TIME_FACTOR;

constexpr size_t KB = 1024;
constexpr size_t MB = KB * KB;
constexpr size_t GB = KB * KB * KB;

template<typename T>
struct Identity {
    using type = T;
};

extern const size_t ARK_COMMON_PAGE_SIZE;

template<typename T>
constexpr bool IsPowerOfTwo(T x)
{
    static_assert(std::is_integral<T>::value, "T must be integral");
    bool ret = false;
    if (x != 0) {
        ret = (x & (x - 1)) == 0;
    }
    return ret;
}

template<typename T>
T RoundDown(T x, typename Identity<T>::type n)
{
    DCHECK_CC(IsPowerOfTwo(n));
    return (x & -n);
}

template<class T>
constexpr T AlignUp(T size, T alignment)
{
    return ((size + alignment - 1) & ~static_cast<T>(alignment - 1));
}

template<typename T>
constexpr T RoundUp(T x, typename std::remove_reference<T>::type n)
{
    return RoundDown(x + n - 1, n);
}

template<typename T>
inline T* AlignUp(T* x, uintptr_t n)
{
    return reinterpret_cast<T*>(RoundUp(reinterpret_cast<uintptr_t>(x), n));
}

template<class T>
constexpr T AlignDown(T size, T alignment)
{
    return (size & ~static_cast<T>(alignment - 1));
}
} // namespace panda

#endif // ARK_COMMON_GLOBALS_H

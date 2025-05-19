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
#ifndef ARK_COMMON_ALLOC_UTILS_H
#define ARK_COMMON_ALLOC_UTILS_H

#include <cstdint>
#include <cstdio>

namespace panda {
constexpr uint32_t ALLOC_UTIL_PAGE_SIZE = 4096;
#define ALLOCUTIL_PAGE_RND_UP(x) \
    (((static_cast<uintptr_t>(x)) + ALLOC_UTIL_PAGE_SIZE - 1) & (~(ALLOC_UTIL_PAGE_SIZE - 1)))

template<typename T>
constexpr T AllocUtilRndDown(T x, size_t n)
{
    return (x & static_cast<size_t>(-n));
}

#ifdef _WIN64
#define ALLOCUTIL_MEM_UNMAP(address, sizeInBytes) \
    if (!VirtualFree(reinterpret_cast<void*>(address), 0, MEM_RELEASE)) { \
        LOG_COMMON(FATAL) << "VirtualFree failed. Process terminating.";      \
        UNREACHABLE_CC();                                                    \
    }
#else
#define ALLOCUTIL_MEM_UNMAP(address, sizeInBytes) \
    if (munmap(reinterpret_cast<void*>(address), sizeInBytes) != EOK) { \
        LOG_COMMON(FATAL) << "munmap failed. Process terminating.";         \
        UNREACHABLE_CC();                                                  \
    }
#endif

template<typename T>
constexpr T AllocUtilRndUp(T x, size_t n)
{
    return AllocUtilRndDown(x + n - 1, n);
}

} // namespace panda

#endif // ARK_COMMON_ALLOC_UTILS_H

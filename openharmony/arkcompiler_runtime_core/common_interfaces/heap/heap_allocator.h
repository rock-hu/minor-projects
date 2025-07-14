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

#ifndef COMMON_INTERFACES_HEAP_ALLOCATOR_H
#define COMMON_INTERFACES_HEAP_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include "objects/base_state_word.h"

namespace common {
using Address32 = uint32_t;
using Address = uint64_t;

class HeapAllocator {
public:
    static Address AllocateInYoungOrHuge(size_t size, LanguageType language);
    static Address AllocateInNonmoveOrHuge(size_t size, LanguageType language);
    static Address32 Allocate32(size_t size, LanguageType language);
    static Address AllocateInOldOrHuge(size_t size, LanguageType language);
    static Address AllocateInHuge(size_t size, LanguageType language);
    static Address AllocateInReadOnly(size_t size, LanguageType language);
    static uintptr_t AllocateLargeJitFortRegion(size_t size, LanguageType language);
    // below are interfaces used for serialize
    static Address AllocateNoGC(size_t size);
    static Address AllocateOldOrLargeNoGC(size_t size);
    static Address AllocatePinNoGC(size_t size);
    static Address AllocateOldRegion();
    static Address AllocatePinnedRegion();
    static Address AllocateLargeRegion(size_t size);
};
}  // namespace common
#endif  // COMMON_INTERFACES_HEAP_ALLOCATOR_H

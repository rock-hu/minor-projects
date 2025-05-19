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
#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <cstdint>

#include "common_components/common_runtime/src/base/c_string.h"

namespace panda {

struct HashString {
    // 211 is a proper prime, which can reduce the conflict rate.
    const uint32_t properPrime = 211;
    
    size_t operator()(const char* key) const
    {
        uint32_t hash = 0;
        while ((*key) != '\0') {
            uint32_t keyChar = *key;
            hash = hash * properPrime + keyChar;
            key += 1;
        }
        return hash;
    }
};
struct EqualString {
    bool operator()(const char* lhs, const char* rhs) const { return strcmp(lhs, rhs) == 0; }
};

} // namespace panda

#endif // ARK_COMMON_BASE_LOG_H

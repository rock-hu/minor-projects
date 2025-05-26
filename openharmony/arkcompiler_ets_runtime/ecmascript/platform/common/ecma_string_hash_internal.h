/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_PLATFORM_ECMA_STRING_HASH_COMMON_H
#define ECMASCRIPT_PLATFORM_ECMA_STRING_HASH_COMMON_H

#include <cstdint>
#include "ecmascript/base/config.h"
#include "ecmascript/platform/ecma_string_hash.h"

namespace panda::ecmascript {
class EcmaStringHashInternal {
friend class EcmaStringHashHelper;
private:
#if ENABLE_NEXT_OPTIMIZATION
    template <typename T>
    static uint32_t ComputeHashForDataOfLongString(const T *data, size_t size,
                                                   uint32_t hashSeed)
    {
        constexpr uint32_t blockSize = EcmaStringHash::BLOCK_SIZE;
        constexpr uint32_t scale = EcmaStringHash::BLOCK_MULTIPLY;
        uint32_t hash[blockSize] = {};
        uint32_t index = 0;
        uint32_t remainder = size & (blockSize - 1);
        switch (remainder) {
#define CASE(N) case (N): \
    hash[blockSize - (N)] = data[index++] * EcmaStringHash::MULTIPLIER[blockSize - (N)]; [[fallthrough]]
            CASE(EcmaStringHash::SIZE_3);
            CASE(EcmaStringHash::SIZE_2);
            CASE(EcmaStringHash::SIZE_1);
#undef CASE
            default:
                break;
        }
        hash[0] += hashSeed * EcmaStringHash::MULTIPLIER[blockSize - 1 - remainder];

        uint32_t dataMul[blockSize] = {};
        for (; index < size; index += blockSize) {
            for (size_t i = 0; i < blockSize; i++) {
                dataMul[i] = data[index + i] * EcmaStringHash::MULTIPLIER[i];
                hash[i] = hash[i] * scale + dataMul[i];
            }
        }
        uint32_t hashTotal = 0;
        for (size_t i = 0; i < blockSize; i++) {
            hashTotal += hash[i];
        }
        return hashTotal;
    }
#else
    template <typename T>
    static uint32_t ComputeHashForDataOfLongString(const T *data, size_t size,
                                                   uint32_t hashSeed)
    {
        constexpr uint32_t hashShift = static_cast<uint32_t>(EcmaStringHash::HASH_SHIFT);
        constexpr uint32_t blockSize = static_cast<size_t>(EcmaStringHash::BLOCK_SIZE);
        uint32_t hash[blockSize] = {0};
        uint32_t index = 0;
        for (; index + blockSize <= size; index += blockSize) {
            hash[0] = (hash[0] << hashShift) - hash[0] + data[index];
            hash[1] = (hash[1] << hashShift) - hash[1] + data[index + 1]; // 1: the second element of the block
            hash[2] = (hash[2] << hashShift) - hash[2] + data[index + 2]; // 2: the third element of the block
            hash[3] = (hash[3] << hashShift) - hash[3] + data[index + 3]; // 3: the fourth element of the block
        }
        for (; index < size; ++index) {
            hash[0] = (hash[0] << hashShift) - hash[0] + data[index];
        }
        uint32_t totalHash = hashSeed;
        for (uint32_t i = 0; i < blockSize; ++i) {
            totalHash = (totalHash << hashShift) - totalHash + hash[i];
        }
        return totalHash;
    }
#endif
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_PLATFORM_ECMA_STRING_HASH_COMMON_H
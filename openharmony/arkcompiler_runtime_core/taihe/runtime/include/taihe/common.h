/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef TAIHE_COMMON_H
#define TAIHE_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// This file is used as a standard library and needs to be easy to use.
// The length limit of functional macros and internal function name does not apply.
// NOLINTBEGIN
#define TH_NONNULL __attribute__((nonnull))

#define TH_ASSERT(condition, message)                                                        \
    do {                                                                                     \
        if (!(condition)) {                                                                  \
            fprintf(stderr, "Assertion failed: (%s), function %s, file %s, line %d.\n"       \
                    "Message: %s\n", #condition, __FUNCTION__, __FILE__, __LINE__, message); \
            abort();                                                                         \
        }                                                                                    \
    } while (0)

#ifdef __cplusplus
#define TH_EXPORT extern "C" __attribute__((visibility("default")))
#else
#define TH_EXPORT extern __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
#define TH_INLINE inline
#else
#define TH_INLINE static inline
#endif

// REFERENCE COUNTING

typedef uint32_t TRefCount;

// Sets the counter to a fixed value.
TH_INLINE void tref_set(TRefCount *c, TRefCount i)
{
    __atomic_store_n(c, i, __ATOMIC_SEQ_CST);
}

// Increments the refcount and returns the *original* value before add.
TH_INLINE TRefCount tref_inc(TRefCount *c)
{
    return __atomic_fetch_add(c, 1, __ATOMIC_SEQ_CST);
}

// Decrements the refcount and returns whether the memory should be freed.
TH_INLINE int tref_dec(TRefCount *c)
{
    return __atomic_sub_fetch(c, 1, __ATOMIC_SEQ_CST) == 0;
}
// NOLINTEND

#endif // TAIHE_COMMON_H

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

#ifndef ARK_COMMON_MUTATOR_INLINE_H
#define ARK_COMMON_MUTATOR_INLINE_H

#include "common_components/common_runtime/src/mutator/mutator_manager.h"
#include "common_interfaces/thread/mutator_base-inl.h"

namespace panda {
inline void Mutator::DoEnterSaferegion()
{
    mutatorBase_.DoEnterSaferegion();
}

inline bool Mutator::EnterSaferegion(bool updateUnwindContext) noexcept
{
    return mutatorBase_.EnterSaferegion(updateUnwindContext);
}

inline bool Mutator::LeaveSaferegion() noexcept
{
    return mutatorBase_.LeaveSaferegion();
}

// Ensure that mutator phase is changed only once by mutator itself or GC
__attribute__((always_inline)) inline bool Mutator::TransitionGCPhase(bool bySelf)
{
    return mutatorBase_.TransitionGCPhase(bySelf);
}

// Ensure that mutator is changed only once by mutator itself or Profile
__attribute__((always_inline)) inline bool Mutator::TransitionToCpuProfile(bool bySelf)
{
    return mutatorBase_.TransitionToCpuProfile(bySelf);
}
} // namespace panda

#endif // ARK_COMMON_MUTATOR_INLINE_H

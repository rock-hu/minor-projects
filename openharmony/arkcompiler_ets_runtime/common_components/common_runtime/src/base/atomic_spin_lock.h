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

#ifndef ARK_COMMON_ATOMIC_SPINLOCK_H
#define ARK_COMMON_ATOMIC_SPINLOCK_H

#include <atomic>

#include "common_interfaces/base/common.h"

namespace panda {
class AtomicSpinLock {
public:
    AtomicSpinLock() {}
    ~AtomicSpinLock() = default;

    void Lock()
    {
        while (state_.test_and_set(std::memory_order_acquire)) {}
    }

    void Unlock() { state_.clear(std::memory_order_release); }

    bool TryLock() { return (state_.test_and_set(std::memory_order_acquire) == false); }

private:
    std::atomic_flag state_ = ATOMIC_FLAG_INIT;

    NO_COPY_SEMANTIC_CC(AtomicSpinLock);
};
} // namespace panda

#endif // ARK_COMMON_ATOMIC_SPINLOCK_H

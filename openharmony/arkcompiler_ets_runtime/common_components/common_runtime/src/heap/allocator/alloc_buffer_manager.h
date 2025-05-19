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
#ifndef ARK_COMMON_ALLOC_BUFFER_MANAGER
#define ARK_COMMON_ALLOC_BUFFER_MANAGER
#include <atomic>
#include <functional>
#include <unordered_set>

#include "common_components/common_runtime/src/heap/allocator/alloc_buffer.h"
#include "common_components/common_runtime/src/base/atomic_spin_lock.h"
#include "common_components/common_runtime/src/common/page_allocator.h"
#include "common_components/common_runtime/src/common/run_type.h"
namespace panda {
using AllocBufferVisitor = std::function<void(AllocationBuffer& buffer)>;
class AllocBufferManager {
public:
    using AllocBuffersSet = std::unordered_set<AllocationBuffer*, std::hash<AllocationBuffer*>,
        std::equal_to<AllocationBuffer*>, StdContainerAllocator<AllocationBuffer*, ALLOCATOR>>;
    using HungryBuffers = AllocBuffersSet;
    AllocBufferManager() {}
    ~AllocBufferManager()
    {
        for (auto* buffer : allocBuffers_) {
            if (buffer != nullptr) {
                delete buffer;
            }
        }
        allocBuffers_.clear();
    };

    void RegisterAllocBuffer(AllocationBuffer& buffer)
    {
        allocBufferLock_.Lock();
        allocBuffers_.insert(&buffer);
        allocBufferLock_.Unlock();
    }

    void VisitAllocBuffers(const AllocBufferVisitor& visitor)
    {
        allocBufferLock_.Lock();
        for (auto* buffer : allocBuffers_) {
            visitor(*buffer);
        }
        allocBufferLock_.Unlock();
    }

    void AddHungryBuffer(AllocationBuffer& buffer)
    {
        std::lock_guard<std::mutex> lg(hungryBuffersLock_);
        hungryBuffers_.insert(&buffer);
    }
    void SwapHungryBuffers(HungryBuffers& getBufferSet)
    {
        std::lock_guard<std::mutex> lg(hungryBuffersLock_);
        hungryBuffers_.swap(getBufferSet);
    }

private:
    AllocBuffersSet allocBuffers_;
    HungryBuffers hungryBuffers_;
    std::mutex hungryBuffersLock_;
    AtomicSpinLock allocBufferLock_;
};
} // namespace panda
#endif // ~ARK_COMMON_ALLOC_BUFFER_MANAGER

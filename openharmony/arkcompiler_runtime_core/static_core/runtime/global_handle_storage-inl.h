/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#ifndef PANDA_RUNTIME_GLOABL_HANDLE_STORAGE_INL_H
#define PANDA_RUNTIME_GLOABL_HANDLE_STORAGE_INL_H

#include "runtime/global_handle_storage.h"
#include "libpandabase/trace/trace.h"
#include "runtime/include/runtime.h"
#include "runtime/mem/object_helpers.h"

namespace ark {
using InternalAllocatorPtr = mem::AllocatorPtr<mem::AllocatorPurpose::ALLOCATOR_PURPOSE_INTERNAL>;

template <typename T>
inline GlobalHandleStorage<T>::~GlobalHandleStorage()
{
    FreeGlobalNodes();
    allocator_->Delete(globalNodes_);
}

template <typename T>
inline void GlobalHandleStorage<T>::FreeHandles()
{
    FreeGlobalNodes();
    globalNodes_->clear();
}

template <typename T>
// CC-OFFNXT(G.FUD.06) solid logic
inline uintptr_t GlobalHandleStorage<T>::NewGlobalHandle(T value)
{
    if (count_ == GLOBAL_BLOCK_SIZE && freeList_ == nullptr) {
        // alloc new block
        auto block = allocator_->New<std::array<Node, GLOBAL_BLOCK_SIZE>>();
        globalNodes_->push_back(block);
        count_ = count_ - GLOBAL_BLOCK_SIZE;
    }

    // use node in block first
    if (count_ != GLOBAL_BLOCK_SIZE) {
        globalNodes_->back()->at(count_).SetNext(nullptr);
        globalNodes_->back()->at(count_).SetObject(value);
        return globalNodes_->back()->at(count_++).GetObjectAddress();
    }

    // use free_list node
    Node *node = freeList_;
    freeList_ = freeList_->GetNext();
    node->SetNext(nullptr);
    node->SetObject(value);
    return node->GetObjectAddress();
}

template <typename T>
inline void GlobalHandleStorage<T>::DisposeGlobalHandle(uintptr_t nodeAddr)
{
    Node *node = reinterpret_cast<Node *>(nodeAddr);
    node->SetNext(freeList_->GetNext());
    freeList_->SetNext(node);
}

template <>
inline void GlobalHandleStorage<coretypes::TaggedType>::DisposeGlobalHandle(uintptr_t nodeAddr)
{
    Node *node = reinterpret_cast<Node *>(nodeAddr);
#ifndef NDEBUG
    node->SetObject(coretypes::TaggedValue::VALUE_UNDEFINED);
#endif
    if (freeList_ != nullptr) {
        node->SetNext(freeList_->GetNext());
        freeList_->SetNext(node);
    } else {
        freeList_ = node;
    }
}

template <>
inline void GlobalHandleStorage<coretypes::TaggedType>::DealUpdateObject(std::array<Node, GLOBAL_BLOCK_SIZE> *block,
                                                                         size_t index,
                                                                         const GCRootUpdater &gcRootUpdater)
{
    coretypes::TaggedValue obj(block->at(index).GetObject());
    if (obj.IsHeapObject()) {
        ObjectHeader *objH = obj.GetHeapObject();
        if (gcRootUpdater(&objH)) {
            coretypes::TaggedValue value(objH);
            block->at(index).SetObject(value.GetRawData());
        };
    }
}

template <>
// CC-OFFNXT(G.FUD.06) solid logic
inline void GlobalHandleStorage<coretypes::TaggedType>::UpdateHeapObject(const GCRootUpdater &gcRootUpdater)
{
    if (globalNodes_->empty()) {
        return;
    }

    for (size_t i = 0; i < globalNodes_->size() - 1; i++) {
        auto block = globalNodes_->at(i);
        for (size_t j = 0; j < GLOBAL_BLOCK_SIZE; j++) {
            DealUpdateObject(block, j, gcRootUpdater);
        }
    }

    auto block = globalNodes_->back();
    for (int32_t i = 0; i < count_; i++) {
        DealUpdateObject(block, i, gcRootUpdater);
    }
}

template <>
inline void GlobalHandleStorage<coretypes::TaggedType>::DealVisitGCRoots(std::array<Node, GLOBAL_BLOCK_SIZE> *block,
                                                                         size_t index, const ObjectVisitor &cb)
{
    coretypes::TaggedValue value(block->at(index).GetObject());
    if (value.IsHeapObject()) {
        cb(value.GetHeapObject());
    }
}

template <>
// CC-OFFNXT(G.FUD.06) solid logic
inline void GlobalHandleStorage<coretypes::TaggedType>::VisitGCRoots([[maybe_unused]] const ObjectVisitor &cb)
{
    if (globalNodes_->empty()) {
        return;
    }

    for (size_t i = 0; i < globalNodes_->size() - 1; i++) {
        auto block = globalNodes_->at(i);
        for (size_t j = 0; j < GLOBAL_BLOCK_SIZE; j++) {
            DealVisitGCRoots(block, j, cb);
        }
    }

    auto block = globalNodes_->back();
    for (int32_t i = 0; i < count_; i++) {
        DealVisitGCRoots(block, i, cb);
    }
}

template class GlobalHandleStorage<coretypes::TaggedType>;
}  // namespace ark
#endif  // PANDA_RUNTIME_GLOABL_HANDLE_STORAGE_INL_H

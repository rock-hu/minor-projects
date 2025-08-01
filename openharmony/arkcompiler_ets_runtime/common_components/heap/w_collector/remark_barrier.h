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
#ifndef ARK_COMMON_REMARK_BARRIER_H
#define ARK_COMMON_REMARK_BARRIER_H

#include "common_components/heap/w_collector/idle_barrier.h"

namespace common {
// TraceBarrier is the barrier for concurrent marking phase.
// rename to TracingBarrier. Marking is confusing in consideration of MarkObject.
class RemarkBarrier : public IdleBarrier {
public:
    explicit RemarkBarrier(Collector& collector) : IdleBarrier(collector) {}

    BaseObject* ReadRefField(BaseObject* obj, RefField<false>& field) const override;
    BaseObject* ReadStaticRef(RefField<false>& field) const override;
    void ReadStruct(HeapAddress dst, BaseObject* obj, HeapAddress src, size_t size) const override;
    BaseObject* ReadStringTableStaticRef(RefField<false> &field) const override;

    void WriteRoot(BaseObject *obj) const override;
    void WriteRefField(BaseObject* obj, RefField<false>& field, BaseObject* ref) const override;
    void WriteBarrier(BaseObject* obj, RefField<false>& field, BaseObject* ref) const override;
    void WriteStruct(BaseObject* obj, HeapAddress dst, size_t dstLen, HeapAddress src, size_t srcLen) const override;
    void WriteStaticRef(RefField<false>& field, BaseObject* ref) const override;

    BaseObject* AtomicReadRefField(BaseObject* obj, RefField<true>& field, MemoryOrder order) const override;
    void AtomicWriteRefField(BaseObject* obj, RefField<true>& field, BaseObject* ref,
                              MemoryOrder order) const override;
    BaseObject* AtomicSwapRefField(BaseObject* obj, RefField<true>& field, BaseObject* ref,
                                    MemoryOrder order) const override;
    bool CompareAndSwapRefField(BaseObject* obj, RefField<true>& field, BaseObject* oldRef, BaseObject* newRef,
                                 MemoryOrder succOrder, MemoryOrder failOrder) const override;

    void CopyStructArray(BaseObject* dstObj, HeapAddress dstField, MIndex dstSize, BaseObject* srcObj,
                         HeapAddress srcField, MIndex srcSize) const override;
};
} // namespace panda
#endif // ~ARK_COMMON_MARK_BARRIER_H
 

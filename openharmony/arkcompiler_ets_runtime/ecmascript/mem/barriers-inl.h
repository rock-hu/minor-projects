/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_MEM_BARRIERS_INL_H
#define ECMASCRIPT_MEM_BARRIERS_INL_H

#include "ecmascript/base/config.h"
#include "ecmascript/daemon/daemon_thread.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/mem/assert_scope.h"
#include "ecmascript/mem/barriers.h"
#include "ecmascript/mem/region-inl.h"
#include "ecmascript/mem/heap.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/tagged_array.h"

namespace panda::ecmascript {
template<WriteBarrierType writeType = WriteBarrierType::NORMAL>
static ARK_INLINE void WriteBarrier(const JSThread *thread, void *obj, size_t offset, JSTaggedType value)
{
    if (UNLIKELY(thread->IsEnableCMCGC())) {
        common::BaseRuntime::WriteBarrier(obj, reinterpret_cast<void*>(ToUintPtr(obj) + offset),
                                        reinterpret_cast<void*>(value));
        // Ignore barrier for cmc gc allocation
        return;
    }

    // NOTE: The logic in WriteBarrier should be synced with CopyObject.
    // if any new feature/bugfix be added in WriteBarrier, it should also be added to CopyObject.
    ASSERT(value != JSTaggedValue::VALUE_UNDEFINED);
    Region *objectRegion = Region::ObjectAddressToRange(static_cast<TaggedObject *>(obj));
    Region *valueRegion = Region::ObjectAddressToRange(reinterpret_cast<TaggedObject *>(value));
#if ECMASCRIPT_ENABLE_BARRIER_CHECK
    // During the AOT deserialization process, the address of hclass is set on the object first, but in reality, the
    // object layout of hclass has not been fully initialized, so this check needs to be skipped.
    if constexpr (writeType != WriteBarrierType::AOT_DESERIALIZE) {
        if (!thread->GetEcmaVM()->GetHeap()->IsAlive(JSTaggedValue(value).GetHeapObject())) {
            LOG_FULL(FATAL) << "WriteBarrier checked value:" << value << " is invalid!";
        }
    }
#endif
    uintptr_t slotAddr = ToUintPtr(obj) + offset;
    if (objectRegion->InGeneralOldSpace() && valueRegion->InYoungSpace()) {
        // Should align with '8' in 64 and 32 bit platform
        ASSERT((slotAddr % static_cast<uint8_t>(MemAlignment::MEM_ALIGN_OBJECT)) == 0);
        objectRegion->InsertOldToNewRSet(slotAddr);
    } else if (!objectRegion->InSharedHeap() && valueRegion->InSharedSweepableSpace()) {
#ifndef NDEBUG
        if (UNLIKELY(JSTaggedValue(value).IsWeakForHeapObject())) {
            CHECK_NO_LOCAL_TO_SHARE_WEAK_REF_HANDLE;
        }
#endif
        objectRegion->InsertLocalToShareRSet(slotAddr);
    }
    ASSERT(!objectRegion->InSharedHeap() || valueRegion->InSharedHeap());
    if (!valueRegion->InSharedHeap() && thread->IsConcurrentMarkingOrFinished()) {
        Barriers::Update(thread, slotAddr, objectRegion, reinterpret_cast<TaggedObject *>(value),
                         valueRegion, writeType);
        // NOTE: ConcurrentMarking and SharedConcurrentMarking can be enabled at the same time, but a specific value
        // can't be "not shared heap" and "in SharedSweepableSpace" at the same time. So using "if - else if" is safe.
    } else if (valueRegion->InSharedSweepableSpace() && thread->IsSharedConcurrentMarkingOrFinished()) {
        if constexpr (writeType != WriteBarrierType::DESERIALIZE) {
            Barriers::UpdateShared(thread, slotAddr, objectRegion, reinterpret_cast<TaggedObject *>(value),
                                   valueRegion);
        } else {
            // In deserialize, will never add references from old object(not allocated by deserialing) to
            // new object(allocated by deserializing), only two kinds of references(new->old, new->new) will
            // be added, the old object is considered as serialize_root, and be marked and pushed in
            // SharedGC::MarkRoots, so just mark all the new object is enough, do not need to push them to
            // workmanager and recursively visit slots of that.
            ASSERT(DaemonThread::GetInstance()->IsConcurrentMarkingOrFinished());
            if (valueRegion->InSCollectSet() && objectRegion->InSharedHeap()) {
                objectRegion->AtomicInsertCrossRegionRSet(slotAddr);
            }
            valueRegion->AtomicMark(JSTaggedValue(value).GetHeapObject());
        }
    }
}

/* template<bool atomic>
static ARK_INLINE JSTaggedType ReadBarrier(const JSThread *thread, const void *obj, size_t offset)
{
} */

template<bool needWriteBarrier>
inline void Barriers::SetObject(const JSThread *thread, void *obj, size_t offset, JSTaggedType value)
{
#ifndef ARK_USE_SATB_BARRIER
    // NOLINTNEXTLINE(clang-analyzer-core.NullDereference)
    *reinterpret_cast<JSTaggedType *>(reinterpret_cast<uintptr_t>(obj) + offset) = value;
    if constexpr (needWriteBarrier) {
        WriteBarrier(thread, obj, offset, value);
    }
#else
    WriteBarrier(thread, obj, offset, value);
    *reinterpret_cast<JSTaggedType *>(reinterpret_cast<uintptr_t>(obj) + offset) = value;
#endif
}

// explicit Instantiation to avoid compile optimization
template void Barriers::SetObject<true>(const JSThread*, void*, size_t, JSTaggedType);
template void Barriers::SetObject<false>(const JSThread*, void*, size_t, JSTaggedType);

inline void Barriers::SynchronizedSetObject(const JSThread *thread, void *obj, size_t offset, JSTaggedType value,
                                            bool isPrimitive)
{
#ifndef ARK_USE_SATB_BARRIER
    reinterpret_cast<volatile std::atomic<JSTaggedType> *>(ToUintPtr(obj) + offset)->store(value,
        std::memory_order_release);
    if (!isPrimitive) {
        WriteBarrier(thread, obj, offset, value);
    }
#else
    WriteBarrier(thread, obj, offset, value);
    reinterpret_cast<volatile std::atomic<JSTaggedType> *>(ToUintPtr(obj) + offset)->store(value,
        std::memory_order_release);
#endif
}

template <bool needReadBarrier>
static inline void CopyBackward(JSTaggedValue* dst, const JSTaggedValue* src, size_t count)
{
    if constexpr (needReadBarrier == false) {
        std::copy_backward(src, src + count, dst + count);
        return;
    }

    for (size_t i = count; i > 0; i--) {
        JSTaggedType valueToRef = Barriers::GetTaggedValue(src, (i - 1) * sizeof(JSTaggedType));
        Barriers::SetObject<false>(nullptr, dst, (i - 1) * sizeof(JSTaggedType), valueToRef);
    }
}

template <bool needReadBarrier>
static inline void CopyForward(JSTaggedValue* dst, const JSTaggedValue* src, size_t count)
{
    if constexpr (needReadBarrier == false) {
        std::copy_n(src, count, dst);
        return;
    }

    for (size_t i = 0; i < count; i++) {
        JSTaggedType valueToRef = Barriers::GetTaggedValue(src, i * sizeof(JSTaggedType));
        Barriers::SetObject<false>(nullptr, dst, i * sizeof(JSTaggedType), valueToRef);
    }
}

template <Region::RegionSpaceKind kind>
ARK_NOINLINE bool BatchBitSet(const JSThread* thread, Region* objectRegion, JSTaggedValue* dst, size_t count);

// to remove parameter dstObj, maybe thread also
template <bool needWriteBarrier, bool maybeOverlap>
void Barriers::CopyObject(const JSThread *thread, const TaggedObject *dstObj, JSTaggedValue *dstAddr,
                          const JSTaggedValue *srcAddr, size_t count)
{
    // NOTE: The logic in CopyObject should be synced with WriteBarrier.
    // if any new feature/bugfix be added in CopyObject, it should also be added to WriteBarrier.

    // step 1. copy from src to dst directly.
    if (thread->NeedReadBarrier()) {
        CopyObjectPrimitive<true, maybeOverlap>(dstAddr, srcAddr, count);
        return;
    } else {
        CopyObjectPrimitive<false, maybeOverlap>(dstAddr, srcAddr, count);
    }

    if constexpr (!needWriteBarrier) {
        return;
    }

    if (UNLIKELY(thread->IsEnableCMCGC())) {
        Barriers::CMCArrayCopyWriteBarrier(thread, dstObj, (void*)srcAddr, (void*)dstAddr, count);
        return;
    }

    // step 2. According to object region, update the corresponding bit set batch.
    Region* objectRegion = Region::ObjectAddressToRange(ToUintPtr(dstObj));
    if (!objectRegion->InSharedHeap()) {
        bool allValueNotHeap = false;
        if (objectRegion->InYoungSpace()) {
            allValueNotHeap = BatchBitSet<Region::InYoung>(thread, objectRegion, dstAddr, count);
        } else if (objectRegion->InGeneralOldSpace()) {
            allValueNotHeap = BatchBitSet<Region::InGeneralOld>(thread, objectRegion, dstAddr, count);
        } else {
            allValueNotHeap = BatchBitSet<Region::Other>(thread, objectRegion, dstAddr, count);
        }
        if (allValueNotHeap) {
            return;
        }
    }
    // step 3. According to marking status, update the barriers.
    const bool marking = thread->IsConcurrentMarkingOrFinished();
    const bool sharedMarking = thread->IsSharedConcurrentMarkingOrFinished();
    if (!marking && !sharedMarking) {
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        JSTaggedValue taggedValue = *(dstAddr + i);
        if (!taggedValue.IsHeapObject()) {
            continue;
        }
        Region* valueRegion = Region::ObjectAddressToRange(taggedValue.GetTaggedObject());
        ASSERT(!objectRegion->InSharedHeap() || valueRegion->InSharedHeap());
        if (marking && !valueRegion->InSharedHeap()) {
            const uintptr_t slotAddr = ToUintPtr(dstAddr) + JSTaggedValue::TaggedTypeSize() * i;
            Barriers::Update(thread, slotAddr, objectRegion, taggedValue.GetTaggedObject(), valueRegion);
            // NOTE: ConcurrentMarking and SharedConcurrentMarking can be enabled at the same time, but a specific
            // value can't be "not shared heap" and "in SharedSweepableSpace" at the same time. So using "if - else if"
            // is safe.
        } else if (sharedMarking && valueRegion->InSharedSweepableSpace()) {
            const uintptr_t slotAddr = ToUintPtr(dstAddr) + JSTaggedValue::TaggedTypeSize() * i;
            Barriers::UpdateShared(thread, slotAddr, objectRegion, taggedValue.GetTaggedObject(), valueRegion);
        }
    }
}

template <bool needReadBarrier, bool maybeOverlap>
inline void Barriers::CopyObjectPrimitive(JSTaggedValue* dst, const JSTaggedValue* src, size_t count)
{
    // Copy Primitive value don't need thread.
    ASSERT((ToUintPtr(dst) % static_cast<uint8_t>(MemAlignment::MEM_ALIGN_OBJECT)) == 0);
    if constexpr (maybeOverlap == false) {
        CopyForward<needReadBarrier>(dst, src, count);
        return;
    }
    if (dst > src && dst < src + count) {
        CopyBackward<needReadBarrier>(dst, src, count);
    } else {
        CopyForward<needReadBarrier>(dst, src, count);
    }
}
} // namespace panda::ecmascript

#endif  // ECMASCRIPT_MEM_BARRIERS_INL_H

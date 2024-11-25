/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ecmascript/mem/idle_gc_trigger.h"

#include <algorithm>

#include "ecmascript/mem/mem_controller.h"
#include "ecmascript/mem/shared_mem_controller.h"
#include "ecmascript/mem/concurrent_marker.h"
#include "ecmascript/mem/heap-inl.h"

namespace panda::ecmascript {
void IdleGCTrigger::NotifyVsyncIdleStart()
{
    TryTriggerIdleLocalOldGC();
}

void IdleGCTrigger::NotifyLooperIdleStart([[maybe_unused]] int64_t timestamp, [[maybe_unused]] int idleTime)
{
    LOG_ECMA_IF(optionalLogEnabled_, DEBUG) << "IdleGCTrigger: recv once looper idle time";
    idleState_.store(true);
    if (!TryTriggerIdleLocalOldGC()) {
        TryTriggerIdleSharedOldGC();
    }
}

void IdleGCTrigger::NotifyLooperIdleEnd([[maybe_unused]] int64_t timestamp)
{
    idleState_.store(false);
}

void IdleGCTrigger::TryTriggerHandleMarkFinished()
{
    // wait sharedGC finish
    if (!thread_->IsReadyToSharedConcurrentMark()) {
        return ;
    }
    if (heap_->GetJSThread()->IsMarkFinished() && heap_->GetConcurrentMarker()->IsTriggeredConcurrentMark()
        && !heap_->GetOnSerializeEvent() && !heap_->InSensitiveStatus()) {
        heap_->SetCanThrowOOMError(false);
        heap_->GetConcurrentMarker()->HandleMarkingFinished();
        heap_->SetCanThrowOOMError(true);
    }
}

void IdleGCTrigger::TryTriggerLocalConcurrentMark()
{
    if (heap_->GetConcurrentMarker()->IsEnabled() && heap_->CheckCanTriggerConcurrentMarking()) {
        heap_->SetFullMarkRequestedState(true);
        heap_->SetMarkType(MarkType::MARK_FULL);
        heap_->TriggerConcurrentMarking();
    }
}

bool IdleGCTrigger::TryTriggerIdleLocalOldGC()
{
    if (heap_->GetJSThread()->IsMarkFinished() &&
        heap_->GetConcurrentMarker()->IsTriggeredConcurrentMark()) {
        PostIdleGCTask(TRIGGER_IDLE_GC_TYPE::LOCAL_REMARK);
        return true;
    }
    if (!IsPossiblePostGCTask(TRIGGER_IDLE_GC_TYPE::LOCAL_CONCURRENT_MARK) ||
        !heap_->CheckCanTriggerConcurrentMarking()) {
        return true;
    }
    if (heap_->GetJSThread()->FullMarkRequest() && !heap_->NeedStopCollection()) {
        heap_->GetJSThread()->ResetFullMarkRequest();
        PostIdleGCTask(TRIGGER_IDLE_GC_TYPE::LOCAL_CONCURRENT_MARK);
        return true;
    }
    if (CheckIdleOrHintOldGC<Heap>(heap_) && ReachIdleLocalOldGCThresholds() && !heap_->NeedStopCollection()) {
        PostIdleGCTask(TRIGGER_IDLE_GC_TYPE::LOCAL_CONCURRENT_MARK);
        return true;
    }
    return false;
}

bool IdleGCTrigger::TryTriggerIdleSharedOldGC()
{
    if (CheckIdleOrHintOldGC<SharedHeap>(sHeap_) &&
        ReachIdleSharedGCThresholds() && !sHeap_->NeedStopCollection() &&
        sHeap_->CheckCanTriggerConcurrentMarking(thread_)) {
        PostIdleGCTask(TRIGGER_IDLE_GC_TYPE::SHARED_CONCURRENT_MARK);
        return true;
    }
    return false;
}

bool IdleGCTrigger::ReachIdleLocalOldGCThresholds()
{
    bool isFullMarking = heap_->IsConcurrentFullMark() && heap_->GetJSThread()->IsMarking();
    bool isNativeSizeLargeTrigger = isFullMarking ? false : heap_->GlobalNativeSizeLargerThanLimitForIdle();
    if (isNativeSizeLargeTrigger) {
        return true;
    }

    OldSpace *oldSpace = heap_->GetOldSpace();
    HugeObjectSpace *hugeObjectSpace = heap_->GetHugeObjectSpace();
    size_t idleSizeLimit = static_cast<size_t>(oldSpace->GetInitialCapacity() *
                                                IDLE_SPACE_SIZE_LIMIT_RATE);
    size_t currentSize = oldSpace->GetHeapObjectSize() + hugeObjectSpace->GetHeapObjectSize();
    if (currentSize >= idleSizeLimit) {
        return true;
    }

    size_t maxCapacity = oldSpace->GetMaximumCapacity() + oldSpace->GetOvershootSize() +
                        oldSpace->GetOutOfMemoryOvershootSize();
    size_t currentCapacity = oldSpace->GetCommittedSize() + hugeObjectSpace->GetCommittedSize();
    size_t idleCapacityLimit = static_cast<size_t>(maxCapacity * IDLE_SPACE_SIZE_LIMIT_RATE);
    if (currentCapacity >= idleCapacityLimit) {
        return true;
    }

    size_t oldSpaceAllocLimit = heap_->GetGlobalSpaceAllocLimit() + oldSpace->GetOvershootSize();
    size_t idleOldSpaceAllocLimit = static_cast<size_t>(oldSpaceAllocLimit * IDLE_SPACE_SIZE_LIMIT_RATE);
    if (heap_->GetHeapObjectSize() > idleOldSpaceAllocLimit) {
        return true;
    }
    return false;
}

bool IdleGCTrigger::ReachIdleSharedGCThresholds()
{
    size_t expectSizeLimit = sHeap_->GetOldSpace()->GetInitialCapacity() * IDLE_SPACE_SIZE_LIMIT_RATE;
    size_t currentOldSize = sHeap_->GetOldSpace()->GetHeapObjectSize();
    size_t expectGlobalSizeLimit = sHeap_->GetGlobalSpaceAllocLimit() * IDLE_SPACE_SIZE_LIMIT_RATE;
    return currentOldSize > expectSizeLimit || sHeap_->GetHeapObjectSize() > expectGlobalSizeLimit;
}

void IdleGCTrigger::TryPostHandleMarkFinished()
{
    if (IsIdleState()) {
        PostIdleGCTask(TRIGGER_IDLE_GC_TYPE::LOCAL_REMARK);
    }
}

void IdleGCTrigger::PostIdleGCTask(TRIGGER_IDLE_GC_TYPE gcType)
{
    if (triggerGCTaskCallback_ != nullptr && IsPossiblePostGCTask(gcType) && !heap_->NeedStopCollection()) {
        std::pair<void*, uint8_t> data(heap_->GetEcmaVM(), static_cast<uint8_t>(gcType));
        triggerGCTaskCallback_(data);
        SetPostGCTask(gcType);
        LOG_GC(INFO) << "IdleGCTrigger: post once " << GetGCTypeName(gcType) << " on idleTime";
    }
    LOG_GC(DEBUG) << "IdleGCTrigger: failed to post once " << GetGCTypeName(gcType);
}

bool IdleGCTrigger::CheckIdleYoungGC() const
{
    auto newSpace = heap_->GetNewSpace();
    LOG_GC(DEBUG) << "IdleGCTrigger: check young GC semi Space size since gc:"
        << newSpace->GetAllocatedSizeSinceGC(newSpace->GetTop());
    return newSpace->GetAllocatedSizeSinceGC(newSpace->GetTop()) > IDLE_MIN_EXPECT_RECLAIM_SIZE;
}

bool IdleGCTrigger::CheckLocalBindingNativeTriggerFullGC() const
{
    size_t nativeBindingSize = heap_->GetNativeBindingSize();
    size_t nativeBindingSizeLastGC = heap_->GetNativeBindingSizeAfterLastGC();
    if (nativeBindingSize <= nativeBindingSizeLastGC) {
        return false;
    }
    size_t expectIncrementalNative = std::max(IDLE_BINDING_NATIVE_MIN_INC_SIZE,
        static_cast<size_t>(nativeBindingSizeLastGC * IDLE_BINDING_NATIVE_MIN_INC_RATIO));
    size_t incrementalNative = nativeBindingSize - nativeBindingSizeLastGC;
    LOG_GC(DEBUG) << "IdleGCTrigger: check full GC expectIncrementalNative:" << expectIncrementalNative
        << ";incrementalNative:" << incrementalNative;
    return incrementalNative > expectIncrementalNative;
}

void IdleGCTrigger::TryTriggerIdleGC(TRIGGER_IDLE_GC_TYPE gcType)
{
    LOG_GC(DEBUG) << "IdleGCTrigger: recv once notify of " << GetGCTypeName(gcType);
    switch (gcType) {
        case TRIGGER_IDLE_GC_TYPE::FULL_GC:
            if (CheckIdleOrHintFullGC<Heap>(heap_) && !heap_->NeedStopCollection()) {
                LOG_GC(INFO) << "IdleGCTrigger: trigger " << GetGCTypeName(gcType);
                heap_->CollectGarbage(TriggerGCType::FULL_GC, GCReason::IDLE);
            } else if (CheckLocalBindingNativeTriggerFullGC() && !heap_->NeedStopCollection()) {
                LOG_GC(INFO) << "IdleGCTrigger: trigger local full GC by native binding size.";
                heap_->CollectGarbage(TriggerGCType::FULL_GC, GCReason::IDLE_NATIVE);
            } else if (CheckIdleYoungGC() && !heap_->NeedStopCollection()) {
                LOG_GC(INFO) << "IdleGCTrigger: trigger young gc";
                heap_->CollectGarbage(TriggerGCType::YOUNG_GC, GCReason::IDLE);
            }
            break;
        case TRIGGER_IDLE_GC_TYPE::SHARED_CONCURRENT_MARK:
            if (CheckIdleOrHintOldGC<SharedHeap>(sHeap_) && sHeap_->CheckCanTriggerConcurrentMarking(thread_)
                && !sHeap_->NeedStopCollection()) {
                LOG_GC(INFO) << "IdleGCTrigger: trigger " << GetGCTypeName(gcType);
                sHeap_->TriggerConcurrentMarking<TriggerGCType::SHARED_GC, GCReason::IDLE>(thread_);
            }
            break;
        case TRIGGER_IDLE_GC_TYPE::SHARED_FULL_GC:
            if (CheckIdleOrHintFullGC<SharedHeap>(sHeap_) && !sHeap_->NeedStopCollection()) {
                LOG_GC(INFO) << "IdleGCTrigger: trigger " << GetGCTypeName(gcType);
                sHeap_->CollectGarbage<TriggerGCType::SHARED_FULL_GC, GCReason::IDLE>(thread_);
            }
            break;
        case TRIGGER_IDLE_GC_TYPE::LOCAL_CONCURRENT_MARK:
            if (CheckIdleOrHintOldGC<Heap>(heap_) && !heap_->NeedStopCollection()) {
                LOG_GC(INFO) << "IdleGCTrigger: trigger " << GetGCTypeName(gcType);
                TryTriggerLocalConcurrentMark();
            }
            break;
        case TRIGGER_IDLE_GC_TYPE::LOCAL_REMARK:
            if (!heap_->NeedStopCollection()) {
                LOG_GC(INFO) << "IdleGCTrigger: trigger " << GetGCTypeName(gcType);
                TryTriggerHandleMarkFinished();
            }
            break;
        default: // LCOV_EXCL_BR_LINE
            LOG_GC(ERROR) << "IdleGCTrigger: this branch is unreachable";
            return;
    }
    ClearPostGCTask(gcType);
}
}
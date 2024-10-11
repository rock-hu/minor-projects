/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "runtime/arch/memory_helpers.h"
#include "runtime/include/managed_thread.h"
#include "runtime/mem/gc/gc_barrier_set.h"
#include "libpandabase/mem/gc_barrier.h"
#include "libpandabase/mem/mem.h"
#include "runtime/include/object_header.h"
#include "runtime/include/panda_vm.h"
#include "runtime/mem/rem_set.h"
#include "runtime/mem/gc/heap-space-misc/crossing_map.h"
#include <atomic>

namespace ark::mem {

GCBarrierSet::~GCBarrierSet() = default;

bool CheckPostBarrier(CardTable *cardTable, const void *objAddr, bool checkCardTable = true)
{
    if constexpr (PANDA_CROSSING_MAP_MANAGE_CROSSED_BORDER) {
        return true;
    }

    // check that obj_addr must be object header
    ASSERT(IsAddressInObjectsHeap(objAddr));
    [[maybe_unused]] auto *object = reinterpret_cast<const ObjectHeader *>(objAddr);
    ASSERT(IsAddressInObjectsHeap(object->ClassAddr<BaseClass>()));

    // we need to check that card related by object header must be young/marked/processed.
    // doesn't for G1, because card_table is processed concurrently, so it can be cleared before we enter here
    bool res = true;
    if (checkCardTable) {
        res = !cardTable->IsClear(ToUintPtr(objAddr));
    }
    return res;
}

static void PreSATBBarrier(ObjectHeader *preVal)
{
    if (preVal != nullptr) {
        LOG(DEBUG, GC) << "GC PreSATBBarrier pre val -> new val:" << preVal;
        auto preBuff = static_cast<PandaVector<ObjectHeader *> *>(ManagedThread::GetCurrent()->GetPreBuff());
        ASSERT(preBuff != nullptr);
        ValidateObject(RootType::SATB_BUFFER, preVal);
        preBuff->push_back(preVal);
    }
}

void PostIntergenerationalBarrier(const void *minAddr, uint8_t *cardTableAddr, uint8_t cardBits, uint8_t dirtyCardValue,
                                  const void *objFieldAddr)
{
    size_t cardIndex = (ToUintPtr(objFieldAddr) - ToUintPtr(minAddr)) >> cardBits;
    auto *cardAddr = static_cast<std::atomic_uint8_t *>(ToVoidPtr(ToUintPtr(cardTableAddr) + cardIndex));
    // Atomic with relaxed order reason: data race with card_addr with no synchronization or ordering constraints
    // imposed on other reads or writes
    cardAddr->store(dirtyCardValue, std::memory_order_relaxed);
}

BarrierOperand GCBarrierSet::GetBarrierOperand(BarrierPosition barrierPosition, std::string_view name)
{
    if (barrierPosition == BarrierPosition::BARRIER_POSITION_PRE) {
        if (UNLIKELY(preOperands_.find(name.data()) == preOperands_.end())) {
            LOG(FATAL, GC) << "Operand " << name << " not found for pre barrier";
        }
        return preOperands_.at(name.data());
    }
    if (UNLIKELY(postOperands_.find(name.data()) == postOperands_.end())) {
        LOG(FATAL, GC) << "Operand " << name << " not found for post barrier";
    }
    return postOperands_.at(name.data());
}

BarrierOperand GCBarrierSet::GetPostBarrierOperand(std::string_view name)
{
    return GetBarrierOperand(BarrierPosition::BARRIER_POSITION_POST, name);
}

void GCGenBarrierSet::PreBarrier([[maybe_unused]] void *preValAddr) {}

void GCGenBarrierSet::PostBarrier(const void *objAddr, [[maybe_unused]] size_t offset,
                                  [[maybe_unused]] void *storedValAddr)
{
    LOG(DEBUG, GC) << "GC PostBarrier: write to " << std::hex << objAddr << " value " << storedValAddr;
    PostIntergenerationalBarrier(minAddr_, cardTableAddr_, cardBits_, dirtyCardValue_, objAddr);
    ASSERT(CheckPostBarrier(cardTable_, objAddr));
}

void GCGenBarrierSet::PostBarrier(const void *objAddr, [[maybe_unused]] size_t offset, [[maybe_unused]] size_t count)
{
    // NOTE: We can improve an implementation here
    // because now we consider every field as an object reference field.
    // Maybe, it will be better to check it, but there can be possible performance degradation.
    PostIntergenerationalBarrier(minAddr_, cardTableAddr_, cardBits_, dirtyCardValue_, objAddr);
    ASSERT(CheckPostBarrier(cardTable_, objAddr));
}

bool GCG1BarrierSet::IsPreBarrierEnabled()
{
    // No data race because G1GC sets this flag on pause
    return Thread::GetCurrent()->GetPreWrbEntrypoint() != nullptr;
}

void GCG1BarrierSet::PreBarrier(void *preValAddr)
{
    LOG_IF(preValAddr != nullptr, DEBUG, GC) << "GC PreBarrier: with pre-value " << preValAddr;
    ASSERT(Thread::GetCurrent()->GetPreWrbEntrypoint() != nullptr);

    PreSATBBarrier(reinterpret_cast<ObjectHeader *>(preValAddr));
}

void GCG1BarrierSet::PostBarrier(const void *objAddr, size_t offset, void *storedValAddr)
{
    if (storedValAddr == nullptr) {
        return;
    }

    LOG(DEBUG, GC) << "GC PostBarrier: write to " << std::hex << objAddr << " value " << storedValAddr;

    if (ark::mem::IsSameRegion(objAddr, storedValAddr, regionSizeBitsCount_)) {
        return;
    }

    auto *card = cardTable_->GetCardPtr(ToUintPtr(objAddr) + offset);
    if (card->IsYoung()) {
        return;
    }

    // StoreLoad barrier is required to guarantee order of previous reference store and card load
    arch::StoreLoadBarrier();

    auto cardValue = card->GetCard();
    auto cardStatus = CardTable::Card::GetStatus(cardValue);
    if (!CardTable::Card::IsMarked(cardStatus)) {
        LOG(DEBUG, GC) << "GC Interregion barrier write to " << objAddr << " value " << storedValAddr;
        card->Mark();
        if (!CardTable::Card::IsHot(cardValue)) {
            Enqueue(card);
        }
    }
    ASSERT(CheckPostBarrier(cardTable_, objAddr, false));
}

void GCG1BarrierSet::PostBarrier(const void *objAddr, size_t offset, size_t count)
{
    // Force post inter-region barrier
    auto firstAddr = ToUintPtr(objAddr) + offset;
    auto *beginCard = cardTable_->GetCardPtr(firstAddr);
    auto *lastCard = cardTable_->GetCardPtr(firstAddr + count - 1);
    if (beginCard->IsYoung()) {
        // Check one card only because cards from beginCard to lastCard belong to the same region
        return;
    }
    // StoreLoad barrier is required to guarantee order of previous reference store and card load
    arch::StoreLoadBarrier();
    Invalidate(beginCard, lastCard);
    ASSERT(CheckPostBarrier(cardTable_, objAddr, false));
    // NOTE: We can improve an implementation here
    // because now we consider every field as an object reference field.
    // Maybe, it will be better to check it, but there can be possible performance degradation.
}

void GCG1BarrierSet::Invalidate(CardTable::CardPtr begin, CardTable::CardPtr last)
{
    LOG(DEBUG, GC) << "GC Interregion barrier write for memory range from  " << cardTable_->GetCardStartAddress(begin)
                   << " to " << cardTable_->GetCardEndAddress(last);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (auto *card = begin; card <= last; ++card) {
        auto cardValue = card->GetCard();
        auto cardStatus = CardTable::Card::GetStatus(cardValue);
        ASSERT(!CardTable::Card::IsYoung(cardStatus));
        if (!CardTable::Card::IsMarked(cardStatus)) {
            card->Mark();
            if (!CardTable::Card::IsHot(cardValue)) {
                Enqueue(card);
            }
        }
    }
}

void GCG1BarrierSet::Enqueue(CardTable::CardPtr card)
{
    auto *thread = ManagedThread::GetCurrent();
    if (thread == nullptr) {  // slow path via shared-queue for VM threads: gc/compiler/etc
        os::memory::LockHolder lock(*queueLock_);
        updatedRefsQueue_->push_back(card);
    } else {
        // general fast-path for mutators
        ASSERT(thread->GetPreBuff() != nullptr);  // write barrier cant be called after Terminate
        auto *buffer = thread->GetG1PostBarrierBuffer();
        ASSERT(buffer != nullptr);
        // try to push it twice
        for (size_t i = 0; i < 2U; i++) {
            bool success = buffer->TryPush(card);
            if (success) {
                return;
            }
        }
        // After 2 unsuccessfull pushing, we see that current buffer still full
        // so, reuse shared buffer
        os::memory::LockHolder lock(*queueLock_);
        updatedRefsQueue_->push_back(card);
    }
}
}  // namespace ark::mem

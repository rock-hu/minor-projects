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

#ifndef PANDA_PROFILING_DATA_H
#define PANDA_PROFILING_DATA_H

#include "macros.h"
#include <array>
#include <atomic>
#include <numeric>

#include <cstdint>

#include "runtime/include/mem/panda_containers.h"

namespace ark {

class Class;

class CallSiteInlineCache {
public:
    static constexpr size_t CLASSES_COUNT = 4;
    static constexpr uintptr_t MEGAMORPHIC_FLAG = static_cast<uintptr_t>(-1);

    static Span<CallSiteInlineCache> From(void *mem, PandaVector<uint32_t> vcalls)
    {
        auto inlineCaches = reinterpret_cast<CallSiteInlineCache *>(mem);
        auto ics = Span<CallSiteInlineCache>(inlineCaches, vcalls.size());
        for (size_t i = 0; i < vcalls.size(); i++) {
            ics[i].Init(vcalls[i]);
        }
        return ics;
    }

    void Init(uintptr_t pc)
    {
        SetBytecodePc(pc);
        std::fill(classes_.begin(), classes_.end(), nullptr);
    }

    void UpdateInlineCaches(Class *cls)
    {
        for (uint32_t i = 0; i < classes_.size();) {
            auto *classAtomic = reinterpret_cast<std::atomic<Class *> *>(&(classes_[i]));
            // Atomic with acquire order reason: data race with classes_ with dependecies on reads after the load which
            // should become visible
            auto storedClass = classAtomic->load(std::memory_order_acquire);
            // Check that the call is already megamorphic
            if (i == 0 && storedClass == reinterpret_cast<Class *>(MEGAMORPHIC_FLAG)) {
                return;
            }
            if (storedClass == cls) {
                return;
            }
            if (storedClass == nullptr) {
                if (!classAtomic->compare_exchange_weak(storedClass, cls, std::memory_order_acq_rel)) {
                    continue;
                }
                return;
            }
            i++;
        }
        // Megamorphic call, disable devirtualization for this call site.
        auto *classAtomic = reinterpret_cast<std::atomic<Class *> *>(&(classes_[0]));
        // Atomic with release order reason: data race with classes_ with dependecies on writes before the store which
        // should become visible acquire
        classAtomic->store(reinterpret_cast<Class *>(MEGAMORPHIC_FLAG), std::memory_order_release);
    }

    auto GetBytecodePc() const
    {
        // Atomic with acquire order reason: data race with bytecode_pc_ with dependecies on reads after the load which
        // should become visible
        return bytecodePc_.load(std::memory_order_acquire);
    }

    void SetBytecodePc(uintptr_t pc)
    {
        // Atomic with release order reason: data race with bytecode_pc_ with dependecies on writes before the store
        // which should become visible acquire
        bytecodePc_.store(pc, std::memory_order_release);
    }

    std::vector<Class *> GetClassesCopy()
    {
        std::vector<Class *> result;
        for (uint32_t i = 0; i < classes_.size();) {
            auto *classAtomic = reinterpret_cast<std::atomic<Class *> const *>(&(classes_[i]));
            // Atomic with acquire order reason: data race with classes_ with dependecies on reads after the load which
            // should become visible
            auto storedClass = classAtomic->load(std::memory_order_acquire);
            if (storedClass != nullptr) {
                result.push_back(storedClass);
            }
            i++;
        }
        return result;
    }

    size_t GetClassesCount() const
    {
        size_t classesCount = 0;
        for (uint32_t i = 0; i < classes_.size();) {
            auto *classAtomic = reinterpret_cast<std::atomic<Class *> const *>(&(classes_[i]));
            // Atomic with acquire order reason: data race with classes_ with dependecies on reads after the load which
            // should become visible
            auto storedClass = classAtomic->load(std::memory_order_acquire);
            if (storedClass != nullptr) {
                classesCount++;
            }
            i++;
        }
        return classesCount;
    }

    static bool IsMegamorphic(Class *cls)
    {
        auto *classAtomic = reinterpret_cast<std::atomic<Class *> *>(&cls);
        // Atomic with acquire order reason: data race with classes_ with dependecies on reads after the load which
        // should become visible
        return classAtomic->load(std::memory_order_acquire) == reinterpret_cast<Class *>(MEGAMORPHIC_FLAG);
    }

private:
    std::atomic_uintptr_t bytecodePc_;
    std::array<Class *, CLASSES_COUNT> classes_ {};
};

class BranchData {
public:
    static Span<BranchData> From(void *mem, PandaVector<uint32_t> branches)
    {
        auto branchData = reinterpret_cast<BranchData *>(mem);
        auto span = Span<BranchData>(branchData, branches.size());
        for (size_t i = 0; i < branches.size(); i++) {
            span[i].Init(branches[i]);
        }
        return span;
    }

    void Init(uintptr_t pc)
    {
        // Atomic with relaxed order reason: data race with pc_
        pc_.store(pc, std::memory_order_relaxed);
        // Atomic with relaxed order reason: data race with taken_counter_
        takenCounter_.store(0, std::memory_order_relaxed);
        // Atomic with relaxed order reason: data race with not_taken_counter_
        notTakenCounter_.store(0, std::memory_order_relaxed);
    }

    uintptr_t GetPc() const
    {
        // Atomic with relaxed order reason: data race with pc_
        return pc_.load(std::memory_order_relaxed);
    }

    int64_t GetTakenCounter() const
    {
        // Atomic with relaxed order reason: data race with taken_counter_
        return takenCounter_.load(std::memory_order_relaxed);
    }

    int64_t GetNotTakenCounter() const
    {
        // Atomic with relaxed order reason: data race with not_taken_counter_
        return notTakenCounter_.load(std::memory_order_relaxed);
    }

    void IncrementTaken()
    {
        // Atomic with relaxed order reason: data race with taken_counter_
        takenCounter_.fetch_add(1, std::memory_order_relaxed);
    }

    void IncrementNotTaken()
    {
        // Atomic with relaxed order reason: data race with not_taken_counter_
        notTakenCounter_.fetch_add(1, std::memory_order_relaxed);
    }

private:
    std::atomic_uintptr_t pc_;
    std::atomic_llong takenCounter_;
    std::atomic_llong notTakenCounter_;
};

class ThrowData {
public:
    static Span<ThrowData> From(void *mem, PandaVector<uint32_t> throws)
    {
        auto throwData = reinterpret_cast<ThrowData *>(mem);
        auto span = Span<ThrowData>(throwData, throws.size());
        for (size_t i = 0; i < throws.size(); i++) {
            span[i].Init(throws[i]);
        }
        return span;
    }

    void Init(uintptr_t pc)
    {
        // Atomic with relaxed order reason: data race with pc_
        pc_.store(pc, std::memory_order_relaxed);
        // Atomic with relaxed order reason: data race with taken_counter_
        takenCounter_.store(0, std::memory_order_relaxed);
    }

    uintptr_t GetPc() const
    {
        // Atomic with relaxed order reason: data race with pc_
        return pc_.load(std::memory_order_relaxed);
    }

    int64_t GetTakenCounter() const
    {
        // Atomic with relaxed order reason: data race with taken_counter_
        return takenCounter_.load(std::memory_order_relaxed);
    }

    void IncrementTaken()
    {
        // Atomic with relaxed order reason: data race with taken_counter_
        takenCounter_.fetch_add(1, std::memory_order_relaxed);
    }

private:
    std::atomic_uintptr_t pc_;
    std::atomic_llong takenCounter_;
};

class ProfilingData {
public:
    explicit ProfilingData(Span<CallSiteInlineCache> inlineCaches, Span<BranchData> branchData,
                           Span<ThrowData> throwData)
        : inlineCaches_(inlineCaches), branchData_(branchData), throwData_(throwData)
    {
    }

    Span<CallSiteInlineCache> GetInlineCaches()
    {
        return inlineCaches_;
    }

    CallSiteInlineCache *FindInlineCache(uintptr_t pc)
    {
        auto ics = GetInlineCaches();
        auto ic = std::lower_bound(ics.begin(), ics.end(), pc,
                                   [](const auto &a, uintptr_t counter) { return a.GetBytecodePc() < counter; });
        return (ic == ics.end() || ic->GetBytecodePc() != pc) ? nullptr : &*ic;
    }

    void UpdateInlineCaches(uintptr_t pc, Class *cls)
    {
        auto ic = FindInlineCache(pc);
        ASSERT(ic != nullptr);
        if (ic != nullptr) {
            ic->UpdateInlineCaches(cls);
        }
    }

    void UpdateBranchTaken(uintptr_t pc)
    {
        auto branch = FindBranchData(pc);
        ASSERT(branch != nullptr);
        branch->IncrementTaken();
    }

    void UpdateBranchNotTaken(uintptr_t pc)
    {
        auto branch = FindBranchData(pc);
        ASSERT(branch != nullptr);
        branch->IncrementNotTaken();
    }

    int64_t GetBranchTakenCounter(uintptr_t pc)
    {
        auto branch = FindBranchData(pc);
        ASSERT(branch != nullptr);
        return branch->GetTakenCounter();
    }

    int64_t GetBranchNotTakenCounter(uintptr_t pc)
    {
        auto branch = FindBranchData(pc);
        ASSERT(branch != nullptr);
        return branch->GetNotTakenCounter();
    }

    void UpdateThrowTaken(uintptr_t pc)
    {
        auto thr0w = FindThrowData(pc);
        ASSERT(thr0w != nullptr);
        thr0w->IncrementTaken();
    }

    int64_t GetThrowTakenCounter(uintptr_t pc)
    {
        auto thr0w = FindThrowData(pc);
        ASSERT(thr0w != nullptr);
        return thr0w->GetTakenCounter();
    }

private:
    BranchData *FindBranchData(uintptr_t fromPc)
    {
        auto it = std::lower_bound(branchData_.begin(), branchData_.end(), fromPc,
                                   [](const auto &a, uintptr_t counter) { return a.GetPc() < counter; });
        if (it == branchData_.end() || it->GetPc() != fromPc) {
            return nullptr;
        }

        return &*it;
    }
    ThrowData *FindThrowData(uintptr_t fromPc)
    {
        if (throwData_.empty()) {
            return nullptr;
        }
        auto it = std::lower_bound(throwData_.begin(), throwData_.end(), fromPc,
                                   [](const auto &a, uintptr_t counter) { return a.GetPc() < counter; });
        if (it == throwData_.end() || it->GetPc() != fromPc) {
            return nullptr;
        }

        return &*it;
    }

    Span<CallSiteInlineCache> inlineCaches_;
    Span<BranchData> branchData_;
    Span<ThrowData> throwData_;
};

}  // namespace ark

#endif  // PANDA_PROFILING_DATA_H

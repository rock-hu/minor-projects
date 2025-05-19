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

#ifndef COMMON_INTERFACES_THREAD_MUTATOR_BASE_H
#define COMMON_INTERFACES_THREAD_MUTATOR_BASE_H

#include <atomic>
#include "base/common.h"

namespace panda {
class ThreadHolder;
}

namespace panda {
class Mutator;

// GCPhase describes phases for stw/concurrent gc.
enum GCPhase : uint8_t {
    GC_PHASE_UNDEF = 0,
    GC_PHASE_IDLE = 1,
    GC_PHASE_START = 8,

    // only gc phase after GC_PHASE_START ( enum value > GC_PHASE_START) needs barrier.
    GC_PHASE_ENUM = 9,
    GC_PHASE_MARK = 10,
    GC_PHASE_REMARK_SATB = 11,
    GC_PHASE_FINAL_MARK = 12,
    GC_PHASE_POST_MARK = 13,
    GC_PHASE_PRECOPY = 14,
    GC_PHASE_COPY = 15,
    GC_PHASE_FIX = 16,
};

class MutatorBase {
public:
    // flag which indicates the reason why mutator should suspend. flag is set by some external thread.
    enum SuspensionType : uint32_t {
        SUSPENSION_FOR_GC_PHASE = 1,
        SUSPENSION_FOR_STW = 2,
        SUSPENSION_FOR_EXIT = 4,
        SUSPENSION_FOR_CPU_PROFILE = 8,
    };

    enum GCPhaseTransitionState : uint32_t {
        NO_TRANSITION,
        NEED_TRANSITION,
        IN_TRANSITION,
        FINISH_TRANSITION,
    };

    enum CpuProfileState : uint32_t {
        NO_CPUPROFILE,
        NEED_CPUPROFILE,
        IN_CPUPROFILING,
        FINISH_CPUPROFILE,
    };

    // Indicate whether mutator is in saferegion
    enum SaferegionState : uint32_t {
        SAFE_REGION_TRUE = 0x17161514,
        SAFE_REGION_FALSE = 0x03020100,
    };

    // // Indicate whether mutator need SafepointPolling
    // enum class SafepointPollingState : uint64_t {
    //     SAFEPOINT_POLLING_FALSE = 0,
    //     SAFEPOINT_POLLING_TRUE = 1,
    // };

    // Called when a mutator starts and finishes, respectively.
    void Init()
    {
        observerCnt = 0;
        mutatorPhase.store(GCPhase::GC_PHASE_IDLE);
    }

    // Sets saferegion state of this mutator.
    __attribute__((always_inline)) inline void SetInSaferegion(SaferegionState state)
    {
        // assure sequential execution of setting insaferegion state and checking suspended state.
        inSaferegion.store(state, std::memory_order_seq_cst);
    }

    // Returns true if this mutator is in saferegion, otherwise false.
    __attribute__((always_inline)) inline bool InSaferegion() const
    {
        return inSaferegion.load(std::memory_order_seq_cst) != SAFE_REGION_FALSE;
    }

    inline void IncObserver() { observerCnt.fetch_add(1); }

    inline void DecObserver() { observerCnt.fetch_sub(1); }

    // Return true indicate there are some observer is visitting this mutator
    inline bool HasObserver() { return observerCnt.load() != 0; }

    inline size_t GetObserverCount() const { return observerCnt.load(); }

    // Force current mutator enter saferegion, internal use only.
    __attribute__((always_inline)) inline void DoEnterSaferegion();
    // Force current mutator leave saferegion, internal use only.
    __attribute__((always_inline)) inline void DoLeaveSaferegion()
    {
        SetInSaferegion(SAFE_REGION_FALSE);
        // go slow path if the mutator should suspend
        if (UNLIKELY_CC(HasAnySuspensionRequest())) {
            HandleSuspensionRequest();
        }
        // temporary impl, and need to refact to call this by flip function
        HandleJSGCCallback();
    }

    // If current mutator is not in saferegion, enter and return true
    // If current mutator has been in saferegion, return false
    __attribute__((always_inline)) inline bool EnterSaferegion(bool updateUnwindContext) noexcept;
    // If current mutator is in saferegion, leave and return true
    // If current mutator has left saferegion, return false
    __attribute__((always_inline)) inline bool LeaveSaferegion() noexcept;

    // Called if current mutator should do corresponding task by suspensionFlag value
    void HandleSuspensionRequest();
    // Called if current mutator should handle stw request
    void SuspendForStw();

    // temporary impl to clean GC callback, and need to refact to flip function
    void HandleJSGCCallback();

    __attribute__((always_inline)) inline bool FinishedTransition() const
    {
        return transitionState == FINISH_TRANSITION;
    }

    __attribute__((always_inline)) inline bool FinishedCpuProfile() const
    {
        return cpuProfileState.load(std::memory_order_acquire) == FINISH_CPUPROFILE;
    }

    __attribute__((always_inline)) inline void SetCpuProfileState(CpuProfileState state)
    {
        cpuProfileState.store(state, std::memory_order_relaxed);
    }

    __attribute__((always_inline)) inline void SetSuspensionFlag(SuspensionType flag)
    {
        if (flag == SUSPENSION_FOR_GC_PHASE) {
            transitionState.store(NEED_TRANSITION, std::memory_order_relaxed);
        } else if (flag == SUSPENSION_FOR_CPU_PROFILE) {
            cpuProfileState.store(NEED_CPUPROFILE, std::memory_order_relaxed);
        }
        suspensionFlag.fetch_or(flag, std::memory_order_seq_cst);
    }

    __attribute__((always_inline)) inline void ClearSuspensionFlag(SuspensionType flag)
    {
        suspensionFlag.fetch_and(~flag, std::memory_order_seq_cst);
    }

    __attribute__((always_inline)) inline uint32_t GetSuspensionFlag() const
    {
        return suspensionFlag.load(std::memory_order_acquire);
    }

    __attribute__((always_inline)) inline bool HasSuspensionRequest(SuspensionType flag) const
    {
        return (suspensionFlag.load(std::memory_order_acquire) & flag) != 0;
    }

    // Check whether current mutator needs to be suspended for GC or other request
    __attribute__((always_inline)) inline bool HasAnySuspensionRequest() const
    {
        return (suspensionFlag.load(std::memory_order_acquire) != 0);
    }

    void SetSafepointStatePtr(uint64_t* slot) { safepointStatePtr = slot; }

    void SetSafepointActive(bool value)
    {
        uint64_t* statePtr = safepointStatePtr;
        if (statePtr == nullptr) {
            return;
        }
        *statePtr = static_cast<uint64_t>(value);
    }

    // Spin wait phase transition finished when GC is tranverting this mutator's phase
    __attribute__((always_inline)) inline void WaitForPhaseTransition() const
    {
        GCPhaseTransitionState state = transitionState.load(std::memory_order_acquire);
        while (state != FINISH_TRANSITION) {
            if (state != IN_TRANSITION) {
                return;
            }
            // Give up CPU to avoid overloading
            (void)sched_yield();
            state = transitionState.load(std::memory_order_acquire);
        }
    }

    __attribute__((always_inline)) inline void WaitForCpuProfiling() const
    {
        while (cpuProfileState.load(std::memory_order_acquire) != FINISH_CPUPROFILE) {
            // Give up CPU to avoid overloading
            (void)sched_yield();
        }
    }

    inline void GcPhaseEnum(GCPhase newPhase);
    inline void GCPhasePreForward(GCPhase newPhase);
    inline void HandleGCPhase(GCPhase newPhase);

    inline void HandleCpuProfile();

    void TransitionToGCPhaseExclusive(GCPhase newPhase);

    void TransitionToCpuProfileExclusive();

    // Ensure that mutator phase is changed only once by mutator itself or GC
    bool TransitionGCPhase(bool bySelf);

    __attribute__((always_inline)) inline bool TransitionToCpuProfile(bool bySelf);

    __attribute__((always_inline)) inline void SetMutatorPhase(const GCPhase newPhase)
    {
        mutatorPhase.store(newPhase, std::memory_order_release);
    }

    __attribute__((always_inline)) inline GCPhase GetMutatorPhase() const
    {
        return mutatorPhase.load(std::memory_order_acquire);
    }

    const void* GetSafepointPage() const
    {
        return safepointStatePtr;
    }

    void MutatorBaseLock() { mutatorBaseLock.lock(); }

    void MutatorBaseUnlock() { mutatorBaseLock.unlock(); }

private:
    // Indicate the current mutator phase and use which barrier in concurrent gc
    // ATTENTION: THE LAYOUT FOR GCPHASE MUST NOT BE CHANGED!
    std::atomic<GCPhase> mutatorPhase = { GCPhase::GC_PHASE_UNDEF };
    // in saferegion, it will not access any managed objects and can be visitted by observer
    std::atomic<uint32_t> inSaferegion = { SAFE_REGION_TRUE };
    // Protect observerCnt
    std::mutex observeCntMutex;
    // Increase when this mutator is observed by some observer
    std::atomic<size_t> observerCnt = { 0 };

    uint64_t* safepointStatePtr = nullptr; // state: active or not

    // If set implies this mutator should process suspension requests
    std::atomic<uint32_t> suspensionFlag = { 0 };
    // Indicate the state of mutator's phase transition
    std::atomic<GCPhaseTransitionState> transitionState = { NO_TRANSITION };

    std::mutex mutatorBaseLock;

    std::atomic<CpuProfileState> cpuProfileState = { NO_CPUPROFILE };

    // This is stored for process `satbNode`, merge Mutator & MutatorBase & SatbNode
    void *mutator {nullptr};

    friend Mutator;
    friend panda::ThreadHolder;
};
}  // namespace panda
#endif  // COMMON_INTERFACES_THREAD_MUTATOR_BASE_H
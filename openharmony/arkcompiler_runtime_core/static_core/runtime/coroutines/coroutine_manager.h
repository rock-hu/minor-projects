/**
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#ifndef PANDA_RUNTIME_COROUTINES_COROUTINE_MANAGER_H
#define PANDA_RUNTIME_COROUTINES_COROUTINE_MANAGER_H

#include "runtime/coroutines/coroutine_context.h"
#include "runtime/thread_manager.h"
#include "runtime/include/runtime.h"
#include "runtime/coroutines/coroutine.h"
#include "runtime/coroutines/coroutine_events.h"

namespace ark {

/// @brief describes the set of adjustable parameters for CoroutineManager and its descendants initialization
struct CoroutineManagerConfig {
    static constexpr uint32_t WORKERS_COUNT_AUTO = 0;

    /// enable the experimental task execution interface
    bool enableDrainQueueIface = false;
    /// enable migration
    bool enableMigration = false;
    /// migrate coroutines that resumed from wait
    bool migrateAwakenedCoros = false;
    /// Number of coroutine workers for the N:M mode
    uint32_t workersCount = WORKERS_COUNT_AUTO;
    /// Limit on the number of exclusive coroutines workers
    uint32_t exclusiveWorkersLimit = 0;
    /// Collection of performance statistics
    bool enablePerfStats = false;
};

/// @brief defines the requested launch mode for a coroutine
enum class CoroutineLaunchMode {
    /// no specific requests
    DEFAULT,
    /// schedule to the parent's worker only
    SAME_WORKER,
    /// schedule to the main worker only
    MAIN_WORKER,
    /// schedule exclusively, moving other coros off the target worker
    EXCLUSIVE
};

/// @brief defines the scheduling policy for a coroutine. Maybe in future we would like to add more types.
enum class CoroutineSchedulingPolicy {
    /// choose the least busy worker
    ANY_WORKER,
    /**
     * same as any_worker but exclude the main worker from available hosts on launch and
     * disallow non_main -> main transitions on migration
     */
    NON_MAIN_WORKER
};

/// @brief defines the selection policy for a worker.
enum class WorkerSelectionPolicy {
    /// choose the least busy worker
    LEAST_LOADED,
    /// choose the busiest worker
    MOST_LOADED
};

/**
 * @brief The interface of all coroutine manager implementations.
 *
 * Manages (registers, unregisters, enumerates) and schedules coroutines for execution using the worker threads.
 * Creates and destroys the main coroutine.
 * Provides interfaces for coroutine synchronization.
 */
class CoroutineManager : public ThreadManager {
public:
    /**
     * @brief The coroutine factory interface.
     *
     * @param name the coroutine name (for debugging and logging purposes)
     *
     * @param ctx the instance of implementation-dependend native coroutine context. Usually is provided by the
     * concrete CoroutineManager descendant.
     *
     * @param ep_info if provided (that means, ep_info != std::nullopt), defines the coroutine entry point to execute.
     * It can be either (the bytecode method, its arguments and the completion event instance to hold the method return
     * value) or (a native function and its parameter). See Coroutine::EntrypointInfo for details. If this parameter is
     * std::nullopt (i.e. no entrypoint present) then the following rules apply:
     * - the factory should only create the coroutine instance and expect that the bytecode for the coroutine will be
     * invoked elsewhere within the context of the newly created coroutine
     * - the coroutine should be destroyed manually by the runtime by calling the Destroy() method
     * - the coroutine does not use the method/arguments passing interface and the completion event interface
     * - no other coroutine will await this one (although it can await others).
     * The "main" coroutine (the EP of the application) is the specific example of such "no entrypoint" coroutine
     * If ep_info is provided then the newly created coroutine will execute the specified method and do all the
     * initialization/finalization steps, including completion_event management and notification of waiters.
     *
     * @param type type of work, which the coroutine performs: whether it is a mutator, a schedule loop or some other
     * thing
     */
    using CoroutineFactory = Coroutine *(*)(Runtime *runtime, PandaVM *vm, PandaString name, CoroutineContext *ctx,
                                            std::optional<Coroutine::EntrypointInfo> &&epInfo, Coroutine::Type type,
                                            CoroutinePriority priority);
    using NativeEntrypointFunc = Coroutine::NativeEntrypointInfo::NativeEntrypointFunc;

    NO_COPY_SEMANTIC(CoroutineManager);
    NO_MOVE_SEMANTIC(CoroutineManager);

    /// Factory is used to create coroutines when needed. See CoroutineFactory for details.
    explicit CoroutineManager(CoroutineFactory factory);
    ~CoroutineManager() override = default;

    /**
     * @brief Should be called after CoroutineManager creation and before any other method calls.
     * Initializes internal structures, creates the main coroutine.
     *
     * @param config describes the CoroutineManager operation mode
     */
    virtual void Initialize(CoroutineManagerConfig config, Runtime *runtime, PandaVM *vm) = 0;
    /// Should be called after all execution is finished. Destroys the main coroutine.
    virtual void Finalize() = 0;
    /// Add coroutine to registry (used for enumeration and tracking) and perform all the required actions
    virtual void RegisterCoroutine(Coroutine *co) = 0;
    /**
     * @brief Remove coroutine from all internal structures, notify waiters about its completion, correctly
     * delete coroutine and free its resources
     * @return returnes true if coroutine has been deleted, false otherwise (e.g. in case of terminating main)
     */
    virtual bool TerminateCoroutine(Coroutine *co) = 0;
    /**
     * @brief The public coroutine creation interface.
     *
     * @param completionEvent the event used for notification when coroutine completes (also used to pass the return
     * value to the language-level entities)
     * @param entrypoint the coroutine entrypoint method
     * @param arguments array of coroutine's entrypoint arguments
     * @param abortFlag if true, finishing with an exception will abort the program
     */
    virtual bool Launch(CompletionEvent *completionEvent, Method *entrypoint, PandaVector<Value> &&arguments,
                        CoroutineLaunchMode mode, CoroutinePriority priority, bool abortFlag) = 0;
    /**
     * @brief The public coroutine creation and execution interface. Switching to the newly created coroutine occurs
     * immediately. Coroutine launch mode should correspond to the use of parent's worker.
     *
     * @param completionEvent the event used for notification when coroutine completes (also used to pass the return
     * value to the language-level entities)
     * @param entrypoint the coroutine entrypoint method
     * @param arguments array of coroutine's entrypoint arguments
     */
    virtual bool LaunchImmediately(CompletionEvent *completionEvent, Method *entrypoint, PandaVector<Value> &&arguments,
                                   CoroutineLaunchMode mode, CoroutinePriority priority, bool abortFlag) = 0;

    /**
     * @brief The public coroutine creation and execution interface with native entrypoint.
     *
     * @param epFunc the native function of coroutine entrypoint
     * @param param the argument of coroutine entrypoint
     * @param coroName the name of launching coroutine
     * NOTE: native function can have Managed scopes
     */
    virtual bool LaunchNative(NativeEntrypointFunc epFunc, void *param, PandaString coroName, CoroutineLaunchMode mode,
                              CoroutinePriority priority, bool abortFlag) = 0;
    /// Suspend the current coroutine and schedule the next ready one for execution
    virtual void Schedule() = 0;
    /**
     *  @brief Move the current coroutine to the waiting state until awaitee happens and schedule the
     * next ready coroutine for execution.
     */
    virtual void Await(CoroutineEvent *awaitee) RELEASE(awaitee) = 0;
    /**
     * @brief Notify the waiting coroutines that an event has happened, so they can stop waiting and
     * become ready for execution
     * @param blocker the blocking event which transitioned from pending to happened state
     *
     * NB! this functions deletes @param blocker (assuming that it was allocated via InternalAllocator)!
     */
    virtual void UnblockWaiters(CoroutineEvent *blocker) = 0;

    /**
     * The designated interface for creating the main coroutine instance.
     * The program EP will be invoked within its context.
     */
    Coroutine *CreateMainCoroutine(Runtime *runtime, PandaVM *vm);
    /// Delete the main coroutine instance and free its resources
    void DestroyMainCoroutine();

    /**
     * Create a coroutine instance (including the context) for internal purposes (e.g. verifier) and
     * set it as the current one.
     * The created coroutine instance will not have any method to execute. All the control flow must be managed
     * by the caller.
     *
     * @return nullptr if resource limit reached or something went wrong; ptr to the coroutine otherwise
     */
    Coroutine *CreateEntrypointlessCoroutine(Runtime *runtime, PandaVM *vm, bool makeCurrent, PandaString name,
                                             Coroutine::Type type, CoroutinePriority priority);
    void DestroyEntrypointlessCoroutine(Coroutine *co);

    /// Destroy a coroutine with an entrypoint
    virtual void DestroyEntrypointfulCoroutine(Coroutine *co);

    /// @return unique coroutine id
    virtual uint32_t AllocateCoroutineId();
    /// mark the specified @param id as free
    virtual void FreeCoroutineId(uint32_t id);

    void SetSchedulingPolicy(CoroutineSchedulingPolicy policy);
    CoroutineSchedulingPolicy GetSchedulingPolicy() const;

    virtual bool IsMainWorker(Coroutine *coro) const = 0;

    virtual Coroutine *CreateExclusiveWorkerForThread([[maybe_unused]] Runtime *runtime, [[maybe_unused]] PandaVM *vm)
    {
        return nullptr;
    }

    virtual bool DestroyExclusiveWorker()
    {
        return false;
    }

    /**
     * @brief This method creates the required number of worker threads
     * @param howMany - number of common workers to be created
     */
    virtual void CreateWorkers([[maybe_unused]] size_t howMany, [[maybe_unused]] Runtime *runtime,
                               [[maybe_unused]] PandaVM *vm)
    {
    }

    /**
     * @brief This method finalizes the required number of common worker threads
     * @param howMany - number of common workers to be finalized
     * NOTE: Make sure that howMany is less than the number of active workers
     */
    virtual void FinalizeWorkers([[maybe_unused]] size_t howMany, [[maybe_unused]] Runtime *runtime,
                                 [[maybe_unused]] PandaVM *vm)
    {
    }

    virtual bool IsExclusiveWorkersLimitReached() const
    {
        return false;
    }

    /* events */
    /// Should be called when a coro makes the non_active->active transition (see the state diagram in coroutine.h)
    virtual void OnCoroBecameActive([[maybe_unused]] Coroutine *co) {};
    /**
     * Should be called when a running coro is being blocked or terminated, i.e. makes
     * the active->non_active transition (see the state diagram in coroutine.h)
     */
    virtual void OnCoroBecameNonActive([[maybe_unused]] Coroutine *co) {};
    /// Should be called at the beginning of the VM native interface call
    virtual void OnNativeCallEnter([[maybe_unused]] Coroutine *co) {};
    /// Should be called at the end of the VM native interface call
    virtual void OnNativeCallExit([[maybe_unused]] Coroutine *co) {};

    /* debugging tools */
    /**
     * Disable coroutine switch for the current worker.
     * If an attempt to switch the active coroutine is performed when coroutine switch is disabled, the exact actions
     * are defined by the concrete CoroutineManager implementation (they could include no-op, program halt or something
     * else).
     *
     * NOTE(konstanting): consider extending this interface to allow for disabling the individual coroutine
     * operations, like CoroutineOperation::LAUNCH, AWAIT, SCHEDULE, ALL
     */
    virtual void DisableCoroutineSwitch() = 0;
    /// Enable coroutine switch for the current worker.
    virtual void EnableCoroutineSwitch() = 0;
    /// @return true if coroutine switch for the current worker is disabled
    virtual bool IsCoroutineSwitchDisabled() = 0;

    /* ZygoteFork operations */
    /// Called before Zygote fork to clean up and stop all worker threads.
    virtual void PreZygoteFork() = 0;
    /// Called after Zygote fork to reinitialize and restart worker threads.
    virtual void PostZygoteFork() = 0;

protected:
    using EntrypointInfo = Coroutine::EntrypointInfo;
    /// Create native coroutine context instance (implementation dependent)
    virtual CoroutineContext *CreateCoroutineContext(bool coroHasEntrypoint) = 0;
    /// Delete native coroutine context instance (implementation dependent)
    virtual void DeleteCoroutineContext(CoroutineContext *) = 0;
    /**
     * Create coroutine instance including the native context and link the context and the coroutine.
     * The coroutine is created using the factory provided in the CoroutineManager constructor and the virtual
     * context creation function, which should be defined in concrete CoroutineManager implementations.
     *
     * @return nullptr if resource limit reached or something went wrong; ptr to the coroutine otherwise
     */
    Coroutine *CreateCoroutineInstance(EntrypointInfo &&epInfo, PandaString name, Coroutine::Type type,
                                       CoroutinePriority priority);
    /// Returns number of existing coroutines
    virtual size_t GetCoroutineCount() = 0;
    /**
     * @brief returns number of coroutines that could be created till the resource limit is reached.
     * The resource limit definition is specific to the exact coroutines/coroutine manager implementation.
     */
    virtual size_t GetCoroutineCountLimit() = 0;

    /// Can be used in descendants to create custom coroutines manually
    CoroutineFactory GetCoroutineFactory();

    /// limit the number of IDs for performance reasons
    static constexpr size_t MAX_COROUTINE_ID = std::min(0xffffU, Coroutine::MAX_COROUTINE_ID);
    static constexpr size_t UNINITIALIZED_COROUTINE_ID = 0x0U;

private:
    CoroutineFactory coFactory_ = nullptr;

    mutable os::memory::Mutex policyLock_;
    CoroutineSchedulingPolicy schedulingPolicy_ GUARDED_BY(policyLock_) = CoroutineSchedulingPolicy::NON_MAIN_WORKER;

    // coroutine id management
    os::memory::Mutex idsLock_;
    std::bitset<MAX_COROUTINE_ID> coroutineIds_ GUARDED_BY(idsLock_);
    uint32_t lastCoroutineId_ GUARDED_BY(idsLock_) = UNINITIALIZED_COROUTINE_ID;
};

/// Disables coroutine switch on the current worker for some scope. Can be used recursively.
class ScopedDisableCoroutineSwitch {
public:
    explicit ScopedDisableCoroutineSwitch(CoroutineManager *coroManager) : coroManager_(coroManager)
    {
        ASSERT(coroManager_ != nullptr);
        coroManager_->DisableCoroutineSwitch();
    }

    ~ScopedDisableCoroutineSwitch()
    {
        coroManager_->EnableCoroutineSwitch();
    }

private:
    CoroutineManager *coroManager_;

    NO_COPY_SEMANTIC(ScopedDisableCoroutineSwitch);
    NO_MOVE_SEMANTIC(ScopedDisableCoroutineSwitch);
};

}  // namespace ark

#endif /* PANDA_RUNTIME_COROUTINES_COROUTINE_MANAGER_H */

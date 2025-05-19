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
#ifndef ARK_COMMON_COLLECTOR_RESOURCES_H
#define ARK_COMMON_COLLECTOR_RESOURCES_H

#include "common_components/common_runtime/src/heap/collector/finalizer_processor.h"
#include "common_components/common_runtime/src/heap/gc_thread_pool.h"
#include "common_components/common_runtime/src/heap/collector/task_queue.h"
#include "common_components/common_runtime/src/heap/collector/task_queue.h"

namespace panda {
class CollectorProxy;
// CollectorResources provides the resources that a functional collector need,
// such as gc thread/threadPool, gc task queue...
class CollectorResources {
public:
    // the collector thread entry routine.
    PUBLIC_API static void* GCMainThreadEntry(void* arg);

    // a collectorResources without a collector entity is functionless
    explicit CollectorResources(CollectorProxy& proxy) : collectorProxy_(proxy) {}
    NO_INLINE_CC virtual ~CollectorResources() = default;

    void Init();
    void Fini();
    void StopGCWork();
    void RequestGC(GCReason reason, bool async);
    void WaitForGCFinish();
    // gc main loop
    void RunTaskLoop();
    // Notify that GC has finished.
    // Must be called by gc thread only
    void NotifyGCFinished(uint64_t gcIndex);
    int32_t GetGCThreadCount(const bool isConcurrent) const;

    GCThreadPool* GetThreadPool() const { return gcThreadPool_; }

    bool IsHeapMarked() const { return isHeapMarked_; }

    void SetHeapMarked(bool value) { isHeapMarked_ = value; }

    bool IsGcStarted() const { return isGcStarted_.load(std::memory_order_relaxed); }

    void SetGcStarted(bool val) { isGcStarted_.store(val, std::memory_order_relaxed); }

    bool IsGCActive() const { return Heap::GetHeap().IsGCEnabled() && isGCActive_.load(std::memory_order_relaxed); }

    FinalizerProcessor& GetFinalizerProcessor() { return finalizerProcessor_; }

    void BroadcastGCFinished();
    GCStats& GetGCStats() { return gcStats_; }
    void RequestHeapDump(GCTask::GCTaskType gcTask);

    void StartRuntimeThreads();
    void StopRuntimeThreads();

private:
    void StartGCThreads();
    void TerminateGCTask();
    void StopGCThreads();
    // Notify the GC thread to start GC, and wait.
    // Called by mutator.
    // reason: The reason for this GC.
    void RequestAsyncGC(GCReason reason);
    void RequestGCAndWait(GCReason reason);
    void PostIgnoredGcRequest(GCReason reason);

    // the thread pool for parallel tracing.
    GCThreadPool* gcThreadPool_ = nullptr;
    int32_t gcThreadCount_ = 1;
    GCTaskQueue<GCRunner>* taskQueue_ = nullptr;

    // the collector thread handle.
    pthread_t gcMainThread_ = 0;
    std::atomic<pid_t> gcTid_{ 0 };
    std::atomic<bool> gcThreadRunning_ = { false };
    // finishedGcIndex records the currently finished gcIndex
    // may be read by mutator but only be written by gc thread sequentially
    std::atomic<uint64_t> finishedGcIndex_ = { 0 };
    // protect condition_variable gcFinishedCondVar's status.
    std::mutex gcFinishedCondMutex_;
    // notified when GC finished, requires gcFinishedCondMutex
    std::condition_variable gcFinishedCondVar_;

    // Indicate whether GC is already started.
    // NOTE: When GC finishes, it clears isGcStarted, must be over-written only by gc thread.
    std::atomic<bool> isGcStarted_ = { false };

    // a switch to disable gc for hotupdate.
    std::atomic<bool> isGCActive_ = { true };

    // only gc thread can access it, so we don't use atomic type
    bool isHeapMarked_ = false;
    // Represent the number of returned raw pointer
    std::atomic<int> criticalNum_{ 0 };
    int gcWorking_ = 0;
#if defined(_WIN64) || defined(__APPLE__)
    std::condition_variable gcWorkingCV;
    std::mutex gcWorkingMtx;
    __attribute__((always_inline)) inline void WaitUntilGCDone()
    {
        std::unique_lock<std::mutex> gcWorkingLck(gcWorkingMtx);
        gcWorkingCV.wait(gcWorkingLck);
    }

    __attribute__((always_inline)) inline void WakeWhenGCDone()
    {
        std::unique_lock<std::mutex> gcWorkingLck(gcWorkingMtx);
        gcWorkingCV.notify_all();
    }
#endif
    CollectorProxy& collectorProxy_;
    FinalizerProcessor finalizerProcessor_;
    GCStats gcStats_;
};
} // namespace panda
#endif

/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_UTIL_WORKERQUEUE_H
#define ES2PANDA_UTIL_WORKERQUEUE_H

#include <es2panda.h>
#include <macros.h>
#include <os/thread.h>

#include <condition_variable>
#include <mutex>

namespace panda::es2panda::util {

class WorkerJob {
public:
    explicit WorkerJob() {};
    NO_COPY_SEMANTIC(WorkerJob);
    NO_MOVE_SEMANTIC(WorkerJob);
    virtual ~WorkerJob() = default;

    virtual void Run() = 0;
    void DependsOn(WorkerJob *job);
    void Signal();

protected:
    std::mutex m_;
    std::condition_variable cond_;
    std::vector<WorkerJob *> dependants_ {};
    size_t dependencies_ {0};
};

class WorkerQueue {
public:
    explicit WorkerQueue(size_t threadCount);
    NO_COPY_SEMANTIC(WorkerQueue);
    NO_MOVE_SEMANTIC(WorkerQueue);
    virtual ~WorkerQueue();

    virtual void Schedule() = 0;
    void Consume();
    void Wait();

protected:
    static void Worker(WorkerQueue *queue);

    std::vector<os::thread::native_handle_type> threads_;
    std::vector<Error> errors_;
    std::mutex m_;
    std::condition_variable jobsAvailable_;
    std::condition_variable jobsFinished_;
    std::vector<WorkerJob *> jobs_ {};
    size_t jobsCount_ {0};
    size_t activeWorkers_ {0};
    bool terminate_ {false};
};
}  // namespace panda::es2panda::util

#endif

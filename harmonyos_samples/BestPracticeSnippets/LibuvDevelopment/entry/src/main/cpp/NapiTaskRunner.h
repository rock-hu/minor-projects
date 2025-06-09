//
// Created on 2025/5/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
/**
 * libuv使用规范及案例
 */
#include "napi/native_api.h"
#include <mutex>
#include <queue>
#include <thread>
#include <uv.h>
#ifndef LIBUVDEVELOPMENT_NAPITASKRUNNER_H
#define LIBUVDEVELOPMENT_NAPITASKRUNNER_H
class Task {};
class ExceptionHandler { };
ExceptionHandler defaultExceptionHandler;

class AbstractTaskRunner{
    public:
    virtual ~AbstractTaskRunner();
    virtual void runAsyncTask(Task&& task) = 0;
    virtual void runSyncTask(Task&& task)  = 0;
    virtual bool isOnCurrentThread() const  = 0;
    virtual void setExceptionHandler (ExceptionHandler handler) = 0;
};

// [Start napi_task_runner]
class NapiTaskRunner : public AbstractTaskRunner {
public:
    NapiTaskRunner (
        napi_env env,
        ExceptionHandler exceptionHandler = defaultExceptionHandler);
    ~NapiTaskRunner() override;
    NapiTaskRunner(const NapiTaskRunner&) = delete;
    NapiTaskRunner& operator=(const NapiTaskRunner&) = delete;
    void runAsyncTask(Task&& task) override;
    void runSyncTask(Task&& task) override;
    bool isOnCurrentThread() const override;
    void setExceptionHandler (ExceptionHandler handler) override;
private:
    napi_env env;
    uv_loop_t* getLoop() const;
    uv_async_t*    asyncHandle;
    std::mutex tasksMutex;
    std::queue<Task> tasksQueue;
    std::thread::id threadId;
    std::condition_variable cv;
    std::shared_ptr<std::atomic_bool> running = std::make_shared<std::atomic_bool>(true);
    ExceptionHandler exceptionHandler;
};


//Constructor code section
NapiTaskRunner::NapiTaskRunner (napi_env env, ExceptionHandler exceptionHandler)
    : env(env), exceptionHandler(std::move (exceptionHandler)) {
    threadId = std::this_thread::get_id();
    auto loop = getLoop() ;
    asyncHandle = new uv_async_t;
    asyncHandle->data = static_cast<void*>(this);
    uv_async_init(loop, asyncHandle, [](auto handle) {});
}


//Destructor section code
NapiTaskRunner::~NapiTaskRunner() {
    running->store(false);
    cv.notify_all();
    uv_close(reinterpret_cast<uv_handle_t*>(asyncHandle), [](uv_handle_t* handle) {
        delete (uv_async_t*)handle;
    });
}
// [End napi_task_runner]


// [Start napi_task_runner_uv_loop]
uv_loop_t* loop;

bool g_done = false;
uv_work_t *work = new uv_work_t;
void ExecuteCB(uv_work_t *work) {
    // Business Logic
}


void CompleteCB(uv_work_t *work, int status) {
    // Business Logic
    if (!g_done) {
        uv_queue_work(work->loop, work, ExecuteCB, CompleteCB);
    } else {
        delete work;
    }
}
// [End napi_task_runner_uv_loop]

void doWork(uv_loop_t* loop, uv_work_t *work) {
    // [Start napi_task_runner_uv_loop]
    uv_queue_work(loop, work, ExecuteCB, CompleteCB);
    // [End napi_task_runner_uv_loop]
} 

#endif //LIBUVDEVELOPMENT_NAPITASKRUNNER_H

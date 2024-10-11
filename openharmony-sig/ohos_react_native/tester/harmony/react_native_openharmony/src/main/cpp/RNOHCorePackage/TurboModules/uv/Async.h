﻿#ifndef UV_ASYNC_H
#define UV_ASYNC_H

#include <memory>
#include <functional>
#include <queue>
#include <mutex>

#include "EventLoop.h"

namespace uv
{

class Async  : public std::enable_shared_from_this<Async>
{
public:
    using OnCloseCompletedCallback = std::function<void(Async*)>;
    Async(EventLoop* loop);
    void  init();
    virtual ~Async();

    void runInThisLoop(DefaultCallback callback);

    void close(OnCloseCompletedCallback callback);
    EventLoop* Loop();
private:
    EventLoop* loop_;
    std::mutex mutex_;
    uv_async_t* handle_;
    std::queue<DefaultCallback> callbacks_;
    OnCloseCompletedCallback onCloseCompletCallback_;
    void process();
    static void Callback(uv_async_t* handle);
    void onCloseCompleted();

};

using AsyncPtr = std::shared_ptr<Async>;

}
#endif

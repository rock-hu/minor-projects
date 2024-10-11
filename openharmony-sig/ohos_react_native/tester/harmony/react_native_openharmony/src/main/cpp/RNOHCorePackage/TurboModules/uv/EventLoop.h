﻿#ifndef   UV_EVENT_LOOP_H
#define   UV_EVENT_LOOP_H

#include <uv.h>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>

namespace uv
{
using DefaultCallback = std::function<void()>;

class Async;
class EventLoop
{
public:
    enum Mode
    {
        Default,
        New
    };
    enum Status
    {
        NotStarted,
        Started,
        Stopped
    };
    EventLoop();
    ~EventLoop();

    static EventLoop* DefaultLoop();

    int run();
    int runNoWait();
    int stop();
    bool isStopped();
    Status getStatus();
    bool isRunInLoopThread();
    void runInThisLoop(const DefaultCallback func);
    void runInThisLoopEn(const DefaultCallback func);
    uv_loop_t* handle();

private:
    EventLoop(Mode mode);

    std::thread::id loopThreadId_;
    uv_loop_t* loop_;
    Async* async_;
    std::atomic<Status> status_;
};

using EventLoopPtr = std::shared_ptr<uv::EventLoop>;
}
#endif


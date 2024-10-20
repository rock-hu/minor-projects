#pragma once

#include <uv.h>
#include <memory>

namespace rnoh {
namespace uv {
class EventLoop final {
 public:
  EventLoop();
  ~EventLoop();

  // rule of five (no copy, no move)
  EventLoop(const EventLoop&) = delete;
  EventLoop& operator=(const EventLoop&) = delete;
  EventLoop(EventLoop&&) = delete;
  EventLoop& operator=(EventLoop&&) = delete;

  uv_loop_t* handle() const;

  void run() const;

 private:
  uv_loop_t* m_loop;
};
} // namespace uv
} // namespace rnoh
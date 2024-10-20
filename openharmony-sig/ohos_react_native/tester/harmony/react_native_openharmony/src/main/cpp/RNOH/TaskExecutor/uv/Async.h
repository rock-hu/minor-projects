#pragma once

#include <uv.h>
#include <functional>

namespace rnoh {
namespace uv {
class Async final {
 public:
  using Callback = std::function<void()>;

  Async(uv_loop_t* loop, Callback callback);
  ~Async() noexcept;

  // rule of five (no copy, no move)
  Async(const Async&) = delete;
  Async& operator=(const Async&) = delete;
  Async(Async&&) = delete;
  Async& operator=(Async&&) = delete;

  void send();

 private:
  uv_async_t* m_handle;
  Callback m_callback;
};
} // namespace uv
} // namespace rnoh
#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include "NativeVsyncHandle.h"

namespace rnoh {
class VSyncListener : public std::enable_shared_from_this<VSyncListener> {
 public:
  using VSyncCallback = std::function<void(long long timestamp)>;

  VSyncListener(char const* name = "VSyncListener");

  void requestFrame(VSyncCallback callback);

  bool isScheduled() const;

 private:
  void scheduleNextVsync();

  NativeVsyncHandle m_vsyncHandle;
  std::mutex m_callbackMutex;
  VSyncCallback m_callback;
  std::atomic_bool m_scheduled{false};
};
} // namespace rnoh
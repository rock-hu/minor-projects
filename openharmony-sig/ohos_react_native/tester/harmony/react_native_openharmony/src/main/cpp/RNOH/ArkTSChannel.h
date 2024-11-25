#pragma once
#include <folly/dynamic.h>
#include <glog/logging.h>
#include <string>
#include "ArkJS.h"
#include "TaskExecutor/TaskExecutor.h"

namespace rnoh {
class ArkTSChannel {
  ArkJS m_arkJs;
  napi_ref m_napi_event_dispatcher_ref;
  TaskExecutor::Weak m_taskExecutor;

 public:
  using Shared = std::shared_ptr<ArkTSChannel>;

  ArkTSChannel(
      TaskExecutor::Shared taskExecutor,
      ArkJS arkJs,
      napi_ref napiEventDispatcherRef)
      : m_arkJs(arkJs),
        m_napi_event_dispatcher_ref(napiEventDispatcherRef),
        m_taskExecutor(taskExecutor) {}

  void postMessage(std::string type, folly::dynamic payload) {
    auto executor = m_taskExecutor.lock();
    if (executor) {
      executor->runTask(
          TaskThread::MAIN,
          [env = m_arkJs.getEnv(),
           eventDispatcherRef = m_napi_event_dispatcher_ref,
           type = std::move(type),
           payload = std::move(payload)]() {
            ArkJS arkJS(env);
            auto napi_event_handler =
                arkJS.getReferenceValue(eventDispatcherRef);
            arkJS.call<2>(
                napi_event_handler,
                {arkJS.createString(type), arkJS.createFromDynamic(payload)});
          });
    }
  }
};
} // namespace rnoh

#pragma once

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <uv.h>
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "DefaultExceptionHandler.h"
#include "EventLoopTaskRunner.h"

namespace rnoh {

class NapiTaskRunner : public EventLoopTaskRunner {
 public:
  NapiTaskRunner(
      napi_env env,
      ExceptionHandler exceptionHandler = defaultExceptionHandler);
  ~NapiTaskRunner() override;

  bool isOnCurrentThread() const override;

 protected:
  void executeTask() override;
  napi_env m_env;
  uv_loop_t* getLoop(napi_env env) const;

  std::thread::id m_threadId;
};

} // namespace rnoh

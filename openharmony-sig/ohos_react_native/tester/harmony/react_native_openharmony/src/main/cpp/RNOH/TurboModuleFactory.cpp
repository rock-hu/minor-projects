#include "RNOH/TurboModuleFactory.h"
#include "RNOH/RNInstance.h"
#include "RNOH/RNOHError.h"
#include "RNOH/StubModule.h"
#include "RNOH/UIManagerModule.h"
#include "TurboModuleFactory.h"

using namespace rnoh;
using namespace facebook;

TurboModuleFactory::TurboModuleFactory(
    napi_env env,
    napi_ref arkTsTurboModuleProviderRef,
    const ComponentJSIBinderByString&& componentBinderByString,
    std::shared_ptr<TaskExecutor> taskExecutor,
    std::vector<std::shared_ptr<TurboModuleFactoryDelegate>> delegates)
    : m_env(env),
      m_arkTsTurboModuleProviderRef(arkTsTurboModuleProviderRef),
      m_componentBinderByString(std::move(componentBinderByString)),
      m_taskExecutor(taskExecutor),
      m_delegates(delegates) {}

TurboModuleFactory::SharedTurboModule TurboModuleFactory::create(
    std::shared_ptr<facebook::react::CallInvoker> jsInvoker,
    const std::string& name,
    std::shared_ptr<EventDispatcher> eventDispatcher,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<facebook::react::Scheduler> scheduler,
    std::weak_ptr<RNInstance> instance) const {
//  LOG(INFO) << "Providing Turbo Module: " << name;
  Context ctx{
      {.jsInvoker = jsInvoker, .instance = instance},
      .env = m_env,
      .arkTSTurboModuleInstanceRef =
          this->maybeGetArkTsTurboModuleInstanceRef(name),
      .taskExecutor = m_taskExecutor,
      .eventDispatcher = eventDispatcher,
      .jsQueue = jsQueue,
      .scheduler = scheduler};
  if (name == "UIManager") {
    return std::make_shared<UIManagerModule>(
        ctx, name, std::move(m_componentBinderByString));
  } else {
    auto result = this->delegateCreatingTurboModule(ctx, name);
    if (result != nullptr) {
      auto arkTSTurboModule =
          std::dynamic_pointer_cast<const ArkTSTurboModule>(result);
      if (arkTSTurboModule != nullptr &&
          ctx.arkTSTurboModuleInstanceRef == nullptr) {
        throw FatalRNOHError(
            std::move(std::string("Couldn't find turbo module '")
                          .append(name)
                          .append("' on the ArkTS side.")),
            {"Have you linked a package that provides this turbo module on the ArkTS side?"});
      }
      return result;
    }
  }

  return this->handleUnregisteredModuleRequest(ctx, name);
}

TurboModuleFactory::SharedTurboModule
TurboModuleFactory::delegateCreatingTurboModule(
    Context ctx,
    const std::string& name) const {
  for (auto delegate : m_delegates) {
    auto result = delegate->createTurboModule(ctx, name);
    if (result != nullptr) {
      return result;
    }
  }
  return nullptr;
}

napi_ref TurboModuleFactory::maybeGetArkTsTurboModuleInstanceRef(
    const std::string& name) const {
  VLOG(3) << "TurboModuleFactory::maybeGetarkTSTurboModuleInstanceRef: start";
  napi_ref result = nullptr;
  m_taskExecutor->runSyncTask(
      TaskThread::MAIN,
      [env = m_env,
       arkTsTurboModuleProviderRef = m_arkTsTurboModuleProviderRef,
       name,
       &result]() {
        VLOG(3)
            << "TurboModuleFactory::maybeGetarkTSTurboModuleInstanceRef: started calling hasModule";
        ArkJS arkJs(env);
        {
          auto result = arkJs.getObject(arkTsTurboModuleProviderRef)
                            .call("hasModule", {arkJs.createString(name)});
          if (!arkJs.getBoolean(result)) {
            return;
          }
        }
        auto n_turboModuleInstance =
            arkJs.getObject(arkTsTurboModuleProviderRef)
                .call("getModule", {arkJs.createString(name)});
        result = arkJs.createReference(n_turboModuleInstance);
      });
  VLOG(3) << "TurboModuleFactory::maybeGetarkTSTurboModuleInstanceRef: stop";
  return result;
}

TurboModuleFactory::SharedTurboModule
TurboModuleFactory::handleUnregisteredModuleRequest(
    Context ctx,
    const std::string& name) const {
  LOG(WARNING) << "Turbo Module '" << name << "' not found.";
  return nullptr;
}

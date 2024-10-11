#pragma once
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include <cxxreact/Instance.h>
#include <cxxreact/ModuleRegistry.h>
#include <cxxreact/NativeModule.h>
#include <folly/dynamic.h>
#include <react/renderer/animations/LayoutAnimationDriver.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/scheduler/Scheduler.h>
#include <react/renderer/uimanager/LayoutAnimationStatusDelegate.h>

#include "RNOH/ArkTSChannel.h"
#include "RNOH/ArkTSMessageHandler.h"
#include "RNOH/EventDispatcher.h"
#include "RNOH/EventEmitRequestHandler.h"
#include "RNOH/GlobalJSIBinder.h"
#include "RNOH/MessageQueueThread.h"
#include "RNOH/RNInstance.h"
#include "RNOH/MountingManager.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOH/TurboModuleFactory.h"
#include "RNOH/TurboModuleProvider.h"
#include "RNOH/UITicker.h"

namespace rnoh {


class RNInstanceArkTS : public RNInstanceInternal,
                        public facebook::react::LayoutAnimationStatusDelegate {
 public:
  RNInstanceArkTS(
      int id,
      std::shared_ptr<facebook::react::ContextContainer> contextContainer,
      TurboModuleFactory&& turboModuleFactory,
      std::shared_ptr<TaskExecutor> taskExecutor,
      std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry>
          componentDescriptorProviderRegistry,
      MutationsToNapiConverter::Shared mutationsToNapiConverter,
      EventEmitRequestHandlers eventEmitRequestHandlers,
      GlobalJSIBinders globalJSIBinders,
      UITicker::Shared uiTicker,
      ShadowViewRegistry::Shared shadowViewRegistry,
      ArkTSChannel::Shared arkTSChannel,
      MountingManager::Shared mountingManager,
      std::vector<ArkTSMessageHandler::Shared> arkTSMessageHandlers,
      bool shouldEnableDebugger,
      bool shouldEnableBackgroundExecutor)
      : RNInstanceInternal(),
        m_id(id),
        instance(std::make_shared<facebook::react::Instance>()),
        m_contextContainer(contextContainer),
        scheduler(nullptr),
        taskExecutor(taskExecutor),
        m_shadowViewRegistry(shadowViewRegistry),
        m_turboModuleFactory(std::move(turboModuleFactory)),
        m_componentDescriptorProviderRegistry(
            componentDescriptorProviderRegistry),
        m_mutationsToNapiConverter(mutationsToNapiConverter),
        m_eventEmitRequestHandlers(eventEmitRequestHandlers),
        m_globalJSIBinders(globalJSIBinders),
        m_shouldRelayUITick(false),
        m_uiTicker(uiTicker),
        m_mountingManager(std::move(mountingManager)),
        m_arkTSMessageHandlers(std::move(arkTSMessageHandlers)),
        m_shouldEnableDebugger(shouldEnableDebugger),
        m_arkTSChannel(std::move(arkTSChannel)),
        m_shouldEnableBackgroundExecutor(shouldEnableBackgroundExecutor) {
    this->unsubscribeUITickListener =
        this->m_uiTicker->subscribe(m_id, [this](long long timestamp) {
          this->taskExecutor->runTask(
              TaskThread::MAIN, [this, timestamp]() { this->onUITick(timestamp); });
        });
  }

  ~RNInstanceArkTS() {
    if (this->unsubscribeUITickListener != nullptr) {
      unsubscribeUITickListener();
    }
    // synchronization primitives used to ensure all tasks currently in queue
    // run before this destructor returns. This ensures the tasks scheduled by
    // this object are not running after the object is destroyed.
    std::mutex surfacesUnregisteredMutex;
    std::condition_variable cv;
    std::unique_lock lock(surfacesUnregisteredMutex);
    taskExecutor->runTask(
        TaskThread::JS, [this, &cv, &surfacesUnregisteredMutex] {
          std::unique_lock lock(surfacesUnregisteredMutex);
          for (auto& [_tag, surfaceHandler] : surfaceHandlers) {
            if (surfaceHandler->getStatus() ==
                facebook::react::SurfaceHandler::Status::Running) {
              surfaceHandler->stop();
            }
            scheduler->unregisterSurface(*surfaceHandler);
          }
          cv.notify_one();
        });
    // block until the task has finished running,
    // to ensure that the instance is not destroyed before all surface are
    // unregistered
    cv.wait(lock);
  }

  TaskExecutor::Shared getTaskExecutor() override;

  void start() override;
  void loadScript(
      std::vector<uint8_t>&& bundle,
      std::string const sourceURL,
      std::function<void(const std::string)>&& onFinish) override;
  void createSurface(
      facebook::react::Tag surfaceId,
      std::string const& moduleName) override;
  void updateSurfaceConstraints(
      facebook::react::Tag surfaceId,
      float width,
      float height,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL) override;
  void startSurface(
      facebook::react::Tag surfaceId,
      float width,
      float height,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL,
      folly::dynamic&& initialProps) override;
  void setSurfaceProps(facebook::react::Tag surfaceId, folly::dynamic&& props)
      override;
  void stopSurface(facebook::react::Tag surfaceId) override;
  void destroySurface(facebook::react::Tag surfaceId) override;
  void setSurfaceDisplayMode(
      facebook::react::Tag surfaceId,
      facebook::react::DisplayMode displayMode) override;
  void callFunction(
      std::string&& module,
      std::string&& method,
      folly::dynamic&& params) override;
  void emitComponentEvent(
      napi_env env,
      facebook::react::Tag tag,
      std::string eventName,
      napi_value payload) override;
  void onMemoryLevel(size_t memoryLevel) override;
  void updateState(
      napi_env env,
      std::string const& componentName,
      facebook::react::Tag tag,
      napi_value newState) override;
  void handleArkTSMessage(
      const std::string& name,
      folly::dynamic const& payload) override;
  void synchronouslyUpdateViewOnUIThread(
      facebook::react::Tag tag,
      folly::dynamic props) override;
  facebook::react::ContextContainer const& getContextContainer() const override;
  TurboModule::Shared getTurboModule(const std::string& name) override;
  void postMessageToArkTS(
      const std::string& name,
      folly::dynamic const& payload) override;
  NativeResourceManager const* getNativeResourceManager() const override;
  void setBundlePath(std::string const& path) override;
  std::string getBundlePath() override;

  std::shared_ptr<TaskExecutor> taskExecutor;

 protected:
  int m_id;
  facebook::react::ContextContainer::Shared m_contextContainer;
  std::map<
      facebook::react::Tag,
      std::shared_ptr<facebook::react::SurfaceHandler>>
      surfaceHandlers;
  std::shared_ptr<facebook::react::Scheduler> scheduler;
  std::unique_ptr<facebook::react::SchedulerDelegate> m_schedulerDelegate = nullptr;
    MountingManager::Shared m_mountingManager;
  std::shared_ptr<facebook::react::Instance> instance;
  TurboModuleProvider::Shared m_turboModuleProvider;
  std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry>
      m_componentDescriptorProviderRegistry;
  ShadowViewRegistry::Shared m_shadowViewRegistry;
  TurboModuleFactory m_turboModuleFactory;
  std::shared_ptr<EventDispatcher> m_eventDispatcher;
  MutationsToNapiConverter::Shared m_mutationsToNapiConverter;
  EventEmitRequestHandlers m_eventEmitRequestHandlers;
  GlobalJSIBinders m_globalJSIBinders;
  std::shared_ptr<facebook::react::LayoutAnimationDriver> m_animationDriver;
  UITicker::Shared m_uiTicker;
  std::function<void()> unsubscribeUITickListener = nullptr;
  std::atomic<bool> m_shouldRelayUITick;
  std::shared_ptr<MessageQueueThread> m_jsQueue;
  bool m_shouldEnableDebugger;
  bool m_shouldEnableBackgroundExecutor;
  std::vector<ArkTSMessageHandler::Shared> m_arkTSMessageHandlers;
  ArkTSChannel::Shared m_arkTSChannel;
  std::string m_bundlePath;

  void initialize();
  void initializeScheduler(
      std::shared_ptr<TurboModuleProvider> turboModuleProvider);
  std::shared_ptr<TurboModuleProvider> createTurboModuleProvider();
  void onUITick(long long timestamp);

  void onAnimationStarted() override; // react::LayoutAnimationStatusDelegate
  void onAllAnimationsComplete()
      override; // react::LayoutAnimationStatusDelegate
};

} // namespace rnoh

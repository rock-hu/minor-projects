/**
 * @architecture: C-API
 */
#pragma once
#include <time.h>
#include <folly/dynamic.h>
#include <react/renderer/core/ComponentDescriptor.h>
#include <react/renderer/scheduler/SchedulerDelegate.h>
#include "RNOH/ComponentInstanceFactory.h"
#include "RNOH/ComponentInstanceRegistry.h"
#include "RNOH/FeatureFlagRegistry.h"
#include "RNOH/MountingManager.h"
#include "RNOH/PreAllocationBuffer.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOH/FeatureFlagRegistry.h"

namespace facebook {
namespace react {
class Scheduler;
}
} // namespace facebook

namespace rnoh {

/**
 * @Thread: MAIN
 */
class MountingManagerCAPI final : public MountingManager {
  using Mutation = facebook::react::ShadowViewMutation;
  using MutationList = facebook::react::ShadowViewMutationList;

 public:
  MountingManagerCAPI(
      ComponentInstanceRegistry::Shared componentInstanceRegistry,
      ComponentInstanceFactory::Shared componentInstanceFactory,
      MountingManager::Shared arkTsMountingManager,
      std::unordered_set<std::string> arkTsComponentNames,
      PreAllocationBuffer::Shared preAllocationBuffer,
      FeatureFlagRegistry::Shared featureFlagRegistry)
      : m_componentInstanceRegistry(std::move(componentInstanceRegistry)),
        m_componentInstanceFactory(std::move(componentInstanceFactory)),
        m_arkTsMountingManager(std::move(arkTsMountingManager)),
        m_arkTsComponentNames(std::move(arkTsComponentNames)) ,
        m_preAllocationBuffer(std::move(preAllocationBuffer)),
        m_featureFlagRegistry(std::move(featureFlagRegistry)) {
        m_preAllocationBuffer->setPreAllocDelegate(
        [this](
            facebook::react::Tag tag,facebook::react::ComponentHandle componentHandle,std::string componentName) {
          auto componentInstance = m_componentInstanceFactory->create(tag, componentHandle, componentName);
          if (componentInstance != nullptr) {
            m_componentInstanceRegistry->insert(componentInstance);
          } else {
//            LOG(ERROR) << "Couldn't create CppComponentInstance for: " << componentName;
          }
        });
  };
    
    ~MountingManagerCAPI() {
    VLOG(1) << "~MountingManagerCAPI";
    m_preAllocationBuffer->setPreAllocDelegate(nullptr);
  }

  PreAllocationBuffer::Shared getPreAllocationBuffer() override{
        return  m_preAllocationBuffer;
  }
    
  void willMount(MutationList const& mutations) override;

  void doMount(MutationList const& mutations) override;

  void didMount(MutationList const& mutations) override;
    
  facebook::react::ShadowViewMutationList getValidMutations(
      facebook::react::ShadowViewMutationList const& mutations);
    
    void schedulerDidViewAllocationByVsync(long long timestamp, long long period) {
    m_preAllocationBuffer->flushByVsync(timestamp, period);
  }
    
  void dispatchCommand(
      const facebook::react::ShadowView& shadowView,
      const std::string& commandName,
      folly::dynamic const& args) override;

  void setIsJsResponder(
      const facebook::react::ShadowView& shadowView,
      bool isJsResponder,
      bool blockNativeResponder) override;

  void updateView(
      facebook::react::Tag tag,
      folly::dynamic props,
      facebook::react::ComponentDescriptor const& componentDescriptor) override;
    
  

 private:
  void updateComponentWithShadowView(
      ComponentInstance::Shared const& componentInstance,
      facebook::react::ShadowView const& shadowView);

  void handleMutation(Mutation const& mutation);

  void finalizeMutationUpdates(MutationList const& mutations);

  ComponentInstanceRegistry::Shared m_componentInstanceRegistry;
  ComponentInstanceFactory::Shared m_componentInstanceFactory;
  facebook::react::ContextContainer::Shared m_contextContainer;
  MountingManager::Shared m_arkTsMountingManager;
  std::unordered_set<std::string> m_arkTsComponentNames;
  PreAllocationBuffer::Shared m_preAllocationBuffer;
  FeatureFlagRegistry::Shared m_featureFlagRegistry;
  std::unordered_set<std::string> m_cApiComponentNames;
};
} // namespace rnoh

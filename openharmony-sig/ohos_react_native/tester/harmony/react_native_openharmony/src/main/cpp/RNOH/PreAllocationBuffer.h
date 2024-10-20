/**
 * Used only in C-API based Architecture.
 */
#pragma once

#include <queue>
#include "ComponentInstanceFactory.h"
#include "ComponentInstanceRegistry.h"
#include "react/renderer/core/ReactPrimitives.h"
#include "react/renderer/core/ShadowNode.h"
namespace rnoh
{
  class PreAllocationBuffer
  {
  private:
    using PreAllocInfo = struct
    {
      facebook::react::Tag tag;
      facebook::react::ComponentHandle componentHandle;
      std::string componentName;
    };
    std::queue<PreAllocInfo> m_preAllocQueue;
    std::mutex m_mtx;
    ComponentInstanceFactory::Shared m_componentInstanceFactory;
    std::unordered_map<facebook::react::Tag, ComponentInstance::Shared>
      m_preallocatedComponentInstanceByTag;
    ComponentInstanceRegistry::Shared m_componentInstanceRegistry;

  public:
    using Shared = std::shared_ptr<PreAllocationBuffer>;
    using Weak = std::weak_ptr<PreAllocationBuffer>;

    PreAllocationBuffer(
      ComponentInstanceFactory::Shared componentInstanceFactory,
      ComponentInstanceRegistry::Shared componentInstanceRegistry
    )
    : m_componentInstanceFactory(std::move(componentInstanceFactory)),
      m_componentInstanceRegistry(std::move(componentInstanceRegistry)) {}

    ~PreAllocationBuffer()
    {
      DLOG(INFO) << "~PreAllocationBuffer";
    }

    void push(const facebook::react::ShadowNode &shadowNode)
    {
      std::lock_guard<std::mutex> lock(m_mtx);
      m_preAllocQueue.push(
          {shadowNode.getTag(),
           shadowNode.getComponentHandle(),
           shadowNode.getComponentName()});
    }

    std::shared_ptr<PreAllocInfo> pop() {
      std::lock_guard<std::mutex> lock(m_mtx);
      if (!m_preAllocQueue.empty()) {
        auto allocInfo = m_preAllocQueue.front();
        m_preAllocQueue.pop();
        return std::make_shared<PreAllocInfo>(allocInfo);
      }
      return nullptr;
    }

    void  clear() {
      std::lock_guard<std::mutex> lock(m_mtx);
      m_preAllocQueue = {};
    }

    
    void clearPreallocatedViews() {
      m_preallocatedComponentInstanceByTag.clear();
    }


    bool isPreAllocBufferEmpty() {
      std::lock_guard<std::mutex> lock(m_mtx);
      return m_preAllocQueue.empty();
    }
    
    bool haveExceededNonBatchedFrameTime(long long timestamp) {
      struct timespec tp;
      // 获取实时时间
      if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
        return false;
      }
      long long nanoseconds = (long long)tp.tv_sec * 1000000000 + tp.tv_nsec;
      long timeLeftInFrame = 8 - ((nanoseconds - timestamp) / 1000000);
      return timeLeftInFrame < 4;
    }

    void flushByVsync(long long timestamp, long long period)
    {
      if (isPreAllocBufferEmpty()) {
        return;
      }
      while (true)
      {
        if (haveExceededNonBatchedFrameTime(timestamp))
        {
          break;
        }
        auto allocInfo = pop();
        if (allocInfo == nullptr)
        {
          break;
        }
        bool isRequestOutdated =
            m_componentInstanceRegistry->findByTag(allocInfo->tag) != nullptr;
        if (isRequestOutdated) {
          continue;
        }
        auto componentInstance = m_componentInstanceFactory->create(allocInfo->tag, allocInfo->componentHandle, allocInfo->componentName);
        if (componentInstance != nullptr) {
          m_preallocatedComponentInstanceByTag.emplace(
              allocInfo->tag, componentInstance);
        } else {
          LOG(ERROR) << "Couldn't preallocate CppComponentInstance for: " << allocInfo->componentName;
        }
      }
    }

    ComponentInstance::Shared getComponentInstance(
    facebook::react::Tag tag,
    facebook::react::ComponentHandle componentHandle,
    std::string componentName) {
      auto componentInstanceIt = m_preallocatedComponentInstanceByTag.find(tag);
      if (componentInstanceIt == m_preallocatedComponentInstanceByTag.end()) {
        return m_componentInstanceFactory->create(
            tag, componentHandle, std::move(componentName));
      } else {
        return m_preallocatedComponentInstanceByTag.extract(componentInstanceIt)
            .mapped();
      }
    }
  };
} // namespace rnoh
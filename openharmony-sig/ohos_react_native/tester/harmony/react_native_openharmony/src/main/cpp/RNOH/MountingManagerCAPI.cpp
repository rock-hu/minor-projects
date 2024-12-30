/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "MountingManagerCAPI.h"
#include <cxxreact/SystraceSection.h>
#include "RNOH/Performance/HarmonyReactMarker.h"

namespace rnoh {

std::string_view getMutationNameFromType(
    facebook::react::ShadowViewMutation::Type mutationType) {
  switch (mutationType) {
    case facebook::react::ShadowViewMutation::Create:
      return "CREATE";
    case facebook::react::ShadowViewMutation::Delete:
      return "DELETE";
    case facebook::react::ShadowViewMutation::Update:
      return "UPDATE";
    case facebook::react::ShadowViewMutation::Insert:
      return "INSERT";
    case facebook::react::ShadowViewMutation::Remove:
      return "REMOVE";
    case facebook::react::ShadowViewMutation::RemoveDeleteTree:
      return "REMOVE_DELETE_TREE";
    default:
      return "UNKNOWN";
  }
}

std::optional<facebook::react::SurfaceId> findSurfaceIdForComponentInstance(
    ComponentInstance::Shared componentInstance) {
  auto parentInstance = componentInstance->getParent().lock();
  while (parentInstance != nullptr) {
    componentInstance = std::move(parentInstance);
    parentInstance = componentInstance->getParent().lock();
  }
  auto rootComponentTag = componentInstance->getTag();

  // Fabric reserves numbers equal 1 mod 10 for SurfaceIds
  if (rootComponentTag % 10 == 1) {
    return static_cast<facebook::react::SurfaceId>(rootComponentTag);
  }

  return std::nullopt;
}

void MountingManagerCAPI::willMount(MutationList const& mutations) {
  // Check the comment in SchedulerDelegate::schedulerDidFinishTransaction before editing here
}

void MountingManagerCAPI::doMount(MutationList const& mutations) {
  m_arkTsMountingManager->doMount(mutations);
}

void MountingManagerCAPI::didMount(MutationList const& mutations) {
    auto validMutations = getValidMutations(mutations);
    m_arkTsMountingManager->didMount(validMutations);
 
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_BATCH_EXECUTION_START); 
  m_componentInstanceProvider->clearPreallocationRequestQueue();
  for (auto const& mutation : mutations) {
    try {
      this->handleMutation(mutation);
    } catch (std::exception const& e) {
      LOG(ERROR) << "Mutation " << getMutationNameFromType(mutation.type)
                 << " failed: " << e.what();
    }
  }
  this->finalizeMutationUpdates(mutations);
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_BATCH_EXECUTION_END);
}

auto MountingManagerCAPI::getValidMutations(MutationList const& mutations)
    -> MutationList {
  MutationList arkTSMutations{};
  for (auto const& mutation : mutations) {
    bool isArkTSMutation = false;
    switch (mutation.type) {
      case facebook::react::ShadowViewMutation::Create:
        isArkTSMutation = !isCAPIComponent(mutation.newChildShadowView);
        break;
      case facebook::react::ShadowViewMutation::Update:
        isArkTSMutation = !isCAPIComponent(mutation.newChildShadowView);
        break;
      case facebook::react::ShadowViewMutation::Delete:
        isArkTSMutation = !isCAPIComponent(mutation.oldChildShadowView);
        break;
      case facebook::react::ShadowViewMutation::Insert:
        isArkTSMutation = !isCAPIComponent(mutation.parentShadowView) ||
            !isCAPIComponent(mutation.newChildShadowView);
        break;
      case facebook::react::ShadowViewMutation::Remove:
        isArkTSMutation = !isCAPIComponent(mutation.parentShadowView) ||
            !isCAPIComponent(mutation.oldChildShadowView);
        break;
      case facebook::react::ShadowViewMutation::RemoveDeleteTree:
        isArkTSMutation = false;
        break;
    }
    if (isArkTSMutation) {
      auto copyMutation = mutation;
      copyMutation.newChildShadowView.layoutMetrics.frame.origin = {0, 0};
      arkTSMutations.push_back(copyMutation);
    }
  }
  return arkTSMutations;
}

bool MountingManagerCAPI::isCAPIComponent(
    facebook::react::ShadowView const& shadowView) {
  std::string componentName = shadowView.componentName;
  if (m_cApiComponentNames.count(componentName) > 0) {
    return true;
  }
  if (m_arkTsComponentNames.count(componentName) > 0) {
    return false;
  }
  auto componentInstance = m_componentInstanceProvider->getComponentInstance(
      shadowView.tag, shadowView.componentHandle, componentName);
  if (componentInstance) {
    m_cApiComponentNames.insert(std::move(componentName));
    return true;
  }
  m_arkTsComponentNames.insert(std::move(componentName));
  return false;
}

void MountingManagerCAPI::dispatchCommand(
    const facebook::react::ShadowView& shadowView,
    const std::string& commandName,
    folly::dynamic const& args) {

  if (!isCAPIComponent(shadowView)) {
    m_arkTsMountingManager->dispatchCommand(shadowView, commandName, args);
  }
  auto componentInstance =
      m_componentInstanceRegistry->findByTag(shadowView.tag);
  if (componentInstance) {
    componentInstance->handleCommand(commandName, args);
  }
}

void MountingManagerCAPI::setIsJsResponder(
    const facebook::react::ShadowView& shadowView,
    bool isJsResponder,
    bool blockNativeResponder) {
  if (!isCAPIComponent(shadowView)) {
    m_arkTsMountingManager->setIsJsResponder(
        shadowView, isJsResponder, blockNativeResponder);
  }

  auto componentInstance =
      m_componentInstanceRegistry->findByTag(shadowView.tag);
  while (componentInstance != nullptr) {
    componentInstance->setNativeResponderBlocked(blockNativeResponder);
    componentInstance = componentInstance->getParent().lock();
  }
}

void MountingManagerCAPI::updateView(
    facebook::react::Tag tag,
    folly::dynamic props,
    facebook::react::ComponentDescriptor const& componentDescriptor) {
  auto componentInstance = m_componentInstanceRegistry->findByTag(tag);
  if (componentInstance == nullptr) {
    return;
  }

  auto surfaceId = findSurfaceIdForComponentInstance(componentInstance);
  if (!surfaceId.has_value()) {
    return;
  }

  std::unordered_set<std::string> propKeys =
      componentInstance->getIgnoredPropKeys();
  for (const auto& key : props.keys()) {
    propKeys.insert(key.getString());
  }

  auto oldProps = componentInstance->getProps();
  auto newProps = componentDescriptor.cloneProps(
      facebook::react::PropsParserContext{
          surfaceId.value(), *m_contextContainer},
      oldProps,
      std::move(props));

  componentInstance->setIgnoredPropKeys({});
  componentInstance->setProps(newProps);
  componentInstance->finalizeUpdates();
  componentInstance->setIgnoredPropKeys(std::move(propKeys));
}

void MountingManagerCAPI::updateComponentWithShadowView(
    ComponentInstance::Shared const& componentInstance,
    facebook::react::ShadowView const& shadowView) {
  // NOTE: updating tag by id must happen before updating props
     m_componentInstanceRegistry->updateTagById(
        shadowView.tag, shadowView.props->nativeId, componentInstance->getId());
    componentInstance->setShadowView(shadowView);
    componentInstance->setLayout(shadowView.layoutMetrics);
    componentInstance->setEventEmitter(shadowView.eventEmitter);
    componentInstance->setState(shadowView.state);
    componentInstance->setProps(shadowView.props);
}

void MountingManagerCAPI::handleMutation(Mutation const& mutation) {
    DVLOG(1) << "Mutation (type:" << getMutationNameFromType(mutation.type)
             << "; componentName: "
             << (mutation.newChildShadowView.componentName != nullptr
                     ? mutation.newChildShadowView.componentName
                     : "null")
             << "; newTag: " << mutation.newChildShadowView.tag
             << "; index: " << mutation.index
             << "; oldTag: " << mutation.oldChildShadowView.tag
             << "; parentTag: " << mutation.parentShadowView.tag << ")";
    switch (mutation.type) {
      case facebook::react::ShadowViewMutation::Create: {
        auto newChild = mutation.newChildShadowView;
        auto componentInstance =
              m_componentInstanceProvider->getComponentInstance(
                  newChild.tag, newChild.componentHandle, newChild.componentName);

      if (componentInstance == nullptr) {
        componentInstance = m_componentInstanceProvider->createArkTSComponent(
            newChild.tag, newChild.componentHandle, newChild.componentName);
      }

        if (componentInstance == nullptr) {
          LOG(ERROR) << "Couldn't create CppComponentInstance for: "
                     << newChild.componentName;
          return;
        }
        m_componentInstanceRegistry->insert(componentInstance);
        updateComponentWithShadowView(componentInstance, newChild);
        break;
      }
      case facebook::react::ShadowViewMutation::Delete: {
        auto oldChild = mutation.oldChildShadowView;
        m_componentInstanceRegistry->deleteByTag(oldChild.tag);
        break;
      }
      case facebook::react::ShadowViewMutation::Insert: {
        auto parentComponentInstance = m_componentInstanceRegistry->findByTag(
            mutation.parentShadowView.tag);
        auto newChildComponentInstance = m_componentInstanceRegistry->findByTag(
            mutation.newChildShadowView.tag);

        if (parentComponentInstance != nullptr &&
            newChildComponentInstance != nullptr) {
          // text need change stackNode
          if (parentComponentInstance->checkUpdateBaseNode()) {
            if (parentComponentInstance->getParent().lock() != nullptr) {
              facebook::react::Tag grandParentTag = parentComponentInstance->getParent().lock()->getTag();
              std::size_t parentIndex = parentComponentInstance->getIndex();
              //  mutation.newChildShadowView is old data
              facebook::react::ShadowView parentShadowView = parentComponentInstance->getShadowView();
              DLOG(INFO) << "need update text node, parent tag=" << parentComponentInstance->getTag()
                << ", grandParentTag tag=" << grandParentTag << ", parent index=" << parentIndex
                << ", child index=" << mutation.index;
              auto grandParentComponentInstance = m_componentInstanceRegistry->findByTag(grandParentTag);
              if (grandParentComponentInstance != nullptr) {
                grandParentComponentInstance->removeChild(parentComponentInstance);
                m_componentInstanceRegistry->deleteByTag(parentComponentInstance->getTag());
                auto newParentComponentInstance = m_componentInstanceFactory->create(
                  parentShadowView.tag, parentShadowView.componentHandle, parentShadowView.componentName);
                if (newParentComponentInstance != nullptr) {
                  updateComponentWithShadowView(newParentComponentInstance, parentShadowView);
                  m_componentInstanceRegistry->insert(newParentComponentInstance);
                  newParentComponentInstance->insertChild(newChildComponentInstance, mutation.index);
                  grandParentComponentInstance->insertChild(newParentComponentInstance, parentIndex);
                }
              } else {
                LOG(FATAL) << "Couldn't find grandParentComponentInstance by tag=" << grandParentTag;
              }
            } else {
              LOG(FATAL) << "Couldn't find grandParentComponentInstance";
            }
          } else {
            parentComponentInstance->insertChild(
                newChildComponentInstance, mutation.index);
          }
          
        }
        break;
      }
      case facebook::react::ShadowViewMutation::Remove: {
        auto parentComponentInstance = m_componentInstanceRegistry->findByTag(
            mutation.parentShadowView.tag);
        if (parentComponentInstance) {
          parentComponentInstance->removeChild(
              m_componentInstanceRegistry->findByTag(
                  mutation.oldChildShadowView.tag));
        }
        break;
      }
      case facebook::react::ShadowViewMutation::Update: {
        auto componentInstance = m_componentInstanceRegistry->findByTag(
            mutation.newChildShadowView.tag);
        if (componentInstance != nullptr) {
          updateComponentWithShadowView(
              componentInstance, mutation.newChildShadowView);
        }
        break;
      }
    }
}

void MountingManagerCAPI::finalizeMutationUpdates(
    MutationList const& mutations) {
  std::unordered_set<ComponentInstance::Shared> componentInstancesToFinalize;

  for (const auto& mutation : mutations) {
    std::shared_ptr<ComponentInstance> componentInstance = nullptr;
    switch (mutation.type) {
      case facebook::react::ShadowViewMutation::Create: {
        componentInstance = m_componentInstanceRegistry->findByTag(
            mutation.newChildShadowView.tag);
        break;
      }
      case facebook::react::ShadowViewMutation::Delete: {
        break;
      }
      case facebook::react::ShadowViewMutation::Insert: {
        componentInstance = m_componentInstanceRegistry->findByTag(
            mutation.parentShadowView.tag);
        break;
      }
      case facebook::react::ShadowViewMutation::Remove: {
        componentInstance = m_componentInstanceRegistry->findByTag(
            mutation.parentShadowView.tag);
        break;
      }
      case facebook::react::ShadowViewMutation::Update: {
        componentInstance = m_componentInstanceRegistry->findByTag(
            mutation.newChildShadowView.tag);
        break;
      }
    }
    if (componentInstance != nullptr) {
      componentInstancesToFinalize.insert(componentInstance);
    }
  }
  for (const auto& componentInstance : componentInstancesToFinalize) {
    componentInstance->finalizeUpdates();
  }
}

void MountingManagerCAPI::schedulerDidSendAccessibilityEvent(
  const facebook::react::ShadowView& shadowView,
  std::string const& eventType) {
    auto componentInstance =
      m_componentInstanceRegistry->findByTag(shadowView.tag);
    folly::dynamic payload = folly::dynamic::object("type", eventType)(
      "targetId", componentInstance->getLocalRootArkUINode().getId());
    m_arkTSChannel->postMessage(
      "RNOH::schedulerDidSendAccessibilityEvent", payload);
};


void MountingManagerCAPI::clearPreallocatedViews() {
  m_componentInstanceProvider->clearPreallocatedViews();
}
} // namespace rnoh

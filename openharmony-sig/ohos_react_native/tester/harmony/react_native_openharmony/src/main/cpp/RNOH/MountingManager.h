#pragma once

#include <react/renderer/mounting/ShadowViewMutation.h>
#include "RNOH/ComponentInstance.h"

namespace rnoh {

/**
 * @Thread: MAIN
 *
 * MountingManager is responsible for creating, updating and destroying
 * ComponentInstances
 *
 */
class MountingManager {
 protected:
  using Mutation = facebook::react::ShadowViewMutation;
  using MutationList = facebook::react::ShadowViewMutationList;

 public:
  using Shared = std::shared_ptr<MountingManager>;
  using Weak = std::weak_ptr<MountingManager>;

  virtual ~MountingManager() noexcept = default;

  virtual void willMount(MutationList const& mutations) = 0;

  virtual void doMount(MutationList const& mutations) = 0;

  virtual void didMount(MutationList const& mutations) = 0;

  virtual void dispatchCommand(
      const facebook::react::ShadowView& shadowView,
      const std::string& commandName,
      folly::dynamic const& args) = 0;

  virtual void setIsJsResponder(
      const facebook::react::ShadowView& shadowView,
      bool isJsResponder,
      bool blockNativeResponder) = 0;

     virtual void schedulerDidSendAccessibilityEvent(
      const facebook::react::ShadowView& shadowView,
      std::string const& eventType) {}

  virtual void updateView(
      facebook::react::Tag tag,
      folly::dynamic props,
      facebook::react::ComponentDescriptor const& componentDescriptor) = 0;

  virtual void clearPreallocatedViews() = 0;
};
} // namespace rnoh

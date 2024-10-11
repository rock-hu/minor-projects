#pragma once

#include <unordered_map>

#include <folly/dynamic.h>
#include <jsi/jsi.h>
#include <react/renderer/core/ReactPrimitives.h>

#include "Drivers/AnimationDriver.h"
#include "Drivers/EventAnimationDriver.h"
#include "Nodes/AnimatedNode.h"

namespace rnoh {
using PropUpdate = std::pair<facebook::react::Tag, folly::dynamic>;
using PropUpdatesList = std::vector<PropUpdate>;

class AnimatedNode;
class ValueAnimatedNode;
class AnimationDriver;
class EventAnimationDriver;

class AnimatedNodesManager {
 public:
  AnimatedNodesManager(std::function<void()>&& scheduleUpdateFn);

  void createNode(facebook::react::Tag tag, folly::dynamic const& config);
  void dropNode(facebook::react::Tag tag);

  void connectNodes(
      facebook::react::Tag parentTag,
      facebook::react::Tag childTag);
  void disconnectNodes(
      facebook::react::Tag parentTag,
      facebook::react::Tag childTag);

  void connectNodeToView(
      facebook::react::Tag nodeTag,
      facebook::react::Tag viewTag);
  void disconnectNodeFromView(
      facebook::react::Tag nodeTag,
      facebook::react::Tag viewTag);

  void addAnimatedEventToView(
      facebook::react::Tag viewTag,
      std::string const& eventName,
      folly::dynamic const& eventMapping);
  void removeAnimatedEventFromView(
      facebook::react::Tag viewTag,
      std::string const& eventName,
      facebook::react::Tag nodeTag);

  void startListeningToAnimatedNodeValue(
      facebook::react::Tag tag,
      ValueAnimatedNode::AnimatedNodeValueListener&& listener);
  void stopListeningToAnimatedNodeValue(facebook::react::Tag tag);

  void setValue(facebook::react::Tag tag, double value);
  void setOffset(facebook::react::Tag tag, double offset);
  void flattenOffset(facebook::react::Tag tag);
  void extractOffset(facebook::react::Tag tag);

  folly::dynamic getNodeOutput(facebook::react::Tag tag);

  void startAnimatingNode(
      facebook::react::Tag animationId,
      facebook::react::Tag nodeTag,
      folly::dynamic const& config,
      std::function<void(bool)>&& endCallback);
  void stopAnimation(facebook::react::Tag animationId);

  PropUpdatesList runUpdates(uint64_t frameTimeNanos);

  void setNeedsUpdate(facebook::react::Tag nodeTag);

  PropUpdatesList handleEvent(
      facebook::react::Tag targetTag,
      std::string const& eventName,
      folly::dynamic const& eventValue);

  AnimatedNode& getNodeByTag(facebook::react::Tag tag);
  ValueAnimatedNode& getValueNodeByTag(facebook::react::Tag tag);

 private:
  PropUpdatesList updateNodes();
  void stopAnimationsForNode(facebook::react::Tag tag);
  void maybeStartAnimations();

  std::function<void()> m_scheduleUpdateFn;
  std::unordered_map<facebook::react::Tag, std::unique_ptr<AnimatedNode>>
      m_nodeByTag;
  std::unordered_map<facebook::react::Tag, std::unique_ptr<AnimationDriver>>
      m_animationById;
  std::vector<std::unique_ptr<EventAnimationDriver>> m_eventDrivers;
  std::unordered_set<facebook::react::Tag> m_nodeTagsToUpdate;
  bool m_isRunningAnimations = false;
};

} // namespace rnoh
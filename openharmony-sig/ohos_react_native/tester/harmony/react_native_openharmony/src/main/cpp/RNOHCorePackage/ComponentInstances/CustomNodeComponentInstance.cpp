#include "CustomNodeComponentInstance.h"
#include "conversions.h"

namespace rnoh {
CustomNodeComponentInstance::CustomNodeComponentInstance(Context context)
    : CppComponentInstance(std::move(context)) {
  getLocalRootArkUINode().setCustomNodeDelegate(this);
}

void CustomNodeComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance, std::size_t index) {
  CppComponentInstance::onChildInserted(childComponentInstance, index);
  if (m_props->removeClippedSubviews && !m_parent.expired()) {
    return;
  }
  m_customNode.insertChild(
        childComponentInstance->getLocalRootArkUINode(), index);
}

void CustomNodeComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance)
{
  CppComponentInstance::onChildRemoved(childComponentInstance);
  m_childrenClippedState.erase(childComponentInstance->getTag());
  getLocalRootArkUINode().removeChild(childComponentInstance->getLocalRootArkUINode());
}

void CustomNodeComponentInstance::onHoverIn() {
    if (m_eventEmitter != nullptr) {
      m_eventEmitter->dispatchEvent(
          "pointerEnter", [=](facebook::jsi::Runtime& runtime) {
            auto payload = facebook::jsi::Object(runtime);
            return payload;
          });
    }
}

void CustomNodeComponentInstance::onHoverOut() {
    if (m_eventEmitter != nullptr) {
      m_eventEmitter->dispatchEvent(
          "pointerLeave", [=](facebook::jsi::Runtime& runtime) {
            auto payload = facebook::jsi::Object(runtime);
            return payload;
          });
    }
}

void CustomNodeComponentInstance::onPropsChanged(SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);
  if (!props->removeClippedSubviews && !m_childrenClippedState.empty()) {
    restoreClippedSubviews();
  }
}

bool CustomNodeComponentInstance::isViewClipped(
    const ComponentInstance::Shared& child,
    facebook::react::Point currentOffset,
    facebook::react::Rect parentBoundingBox) {
  auto scrollRectOrigin =
      facebook::react::Point{currentOffset.x, currentOffset.y};
  auto scrollRect =
      facebook::react::Rect{scrollRectOrigin, parentBoundingBox.size};

  return !rnoh::rectIntersects(scrollRect, child->getLayoutMetrics().frame);
}

void CustomNodeComponentInstance::restoreClippedSubviews() {
  size_t i = 0;
  for (const auto& child : m_children) {
    auto tag = child->getTag();
    if (auto it = m_childrenClippedState.find(tag);
        it != m_childrenClippedState.end() && it->second) {
      m_customNode.insertChild(child->getLocalRootArkUINode(), i);
      it->second = false;
    }
    i++;
  }
}

void CustomNodeComponentInstance::updateClippedSubviews(bool childrenChange) {
  auto parent = m_parent.lock();
  if ((m_props && !m_props->removeClippedSubviews) || !parent) {
    return;
  }

  auto currentOffset = parent->getCurrentOffset();

  auto parentBoundingBox = parent->getBoundingBox();

  if (m_previousOffset == currentOffset && !childrenChange) {
    return;
  }

  m_previousOffset = currentOffset;

  size_t nextChildIndex = 0;
  for (const auto& child : m_children) {
    auto tag = child->getTag();
    bool childClipped = isViewClipped(child, currentOffset, parentBoundingBox);
    auto it = m_childrenClippedState.find(tag);
    bool wasChildClipped =
        it == m_childrenClippedState.end() || it->second != childClipped;

    if (wasChildClipped) {
      m_childrenClippedState.insert_or_assign(tag, childClipped);
      if (childClipped) {
        m_customNode.removeChild(child->getLocalRootArkUINode());
      } else {
        m_customNode.insertChild(child->getLocalRootArkUINode(), nextChildIndex);
      }
    }

    if (!childClipped) {
      nextChildIndex++;
    }
  }
}

void CustomNodeComponentInstance::onFinalizeUpdates() {
  ComponentInstance::onFinalizeUpdates();
  if (m_props->removeClippedSubviews) {
    updateClippedSubviews(true);
  }
}

void CustomNodeComponentInstance::onClick() {
  if (m_eventEmitter != nullptr) {
    m_eventEmitter->dispatchEvent(
        "click", [=](facebook::jsi::Runtime& runtime) {
          auto payload = facebook::jsi::Object(runtime);
          return payload;
        });
  }
}

CustomNode& CustomNodeComponentInstance::getLocalRootArkUINode()
{
  return m_customNode;
}

} // namespace rnoh

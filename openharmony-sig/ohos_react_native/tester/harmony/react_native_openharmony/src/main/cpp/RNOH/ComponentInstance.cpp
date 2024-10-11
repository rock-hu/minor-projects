#include "ComponentInstance.h"
#include <glog/logging.h>

namespace rnoh {
ComponentInstance::ComponentInstance(Context ctx)
    : m_tag(ctx.tag),
      m_componentHandle(ctx.componentHandle),
      m_componentName(std::move(ctx.componentName)),
      m_deps(std::move(ctx.dependencies)) {
        if (m_deps != nullptr && m_deps->rnInstance.lock() != nullptr) {
          m_rnInstanceId = m_deps->rnInstance.lock()->getId();
        }
      }

void ComponentInstance::insertChild(
    ComponentInstance::Shared childComponentInstance,
    std::size_t index) {
  auto it = m_children.begin() + index;
  std::size_t newIndex = index;
  childComponentInstance->setParent(shared_from_this());

  onChildInserted(childComponentInstance, newIndex);
  childComponentInstance->setIndex(index);
  m_children.insert(it, std::move(childComponentInstance));
}

void ComponentInstance::removeChild(
    ComponentInstance::Shared childComponentInstance) {
  auto it =
      std::find(m_children.begin(), m_children.end(), childComponentInstance);
  if (it != m_children.end()) {
    std::size_t index = it - m_children.begin();

    auto childComponentInstance = std::move(*it);
    m_children.erase(it);
    onChildRemoved(childComponentInstance);
  }
}
} // namespace rnoh

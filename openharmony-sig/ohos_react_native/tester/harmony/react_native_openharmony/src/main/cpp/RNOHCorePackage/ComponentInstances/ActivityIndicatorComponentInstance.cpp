#include "ActivityIndicatorComponentInstance.h"

#include <glog/logging.h>
#include <react/renderer/components/rncore/Props.h>
#include <react/renderer/core/ConcreteState.h>
#include <sstream>

namespace rnoh {

ActivityIndicatorComponentInstance::ActivityIndicatorComponentInstance(
    Context context)
    : CppComponentInstance(std::move(context)) {}

void ActivityIndicatorComponentInstance::onPropsChanged(
    SharedConcreteProps const& props) {
  auto old = m_props;
  CppComponentInstance::onPropsChanged(props);
  if (!old || props->animating != old->animating) {
    getLocalRootArkUINode().setAnimating(props->animating);
  }
  if (props->color && (!old || *(props->color) != *(old->color))) {
    getLocalRootArkUINode().setColor(props->color);
  }
}

LoadingProgressNode&
ActivityIndicatorComponentInstance::getLocalRootArkUINode() {
  return m_loadingProgressNode;
}

} // namespace rnoh

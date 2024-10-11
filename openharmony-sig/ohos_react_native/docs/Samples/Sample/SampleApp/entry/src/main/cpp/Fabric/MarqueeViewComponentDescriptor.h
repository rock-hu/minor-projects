
#pragma once

#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>
#include "MarqueeViewEventEmitter.h"
#include "Props.h"

namespace facebook {
namespace react {

extern const char MarqueeViewComponentName[] = "MarqueeView";

  using MarqueeViewShadowNode = ConcreteViewShadowNode<MarqueeViewComponentName, MarqueeViewProps, MarqueeViewEventEmitter>;
  using MarqueeViewComponentDescriptor = ConcreteComponentDescriptor<MarqueeViewShadowNode>;

} // namespace react
} // namespace facebook

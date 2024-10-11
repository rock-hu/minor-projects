#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {
class PullToRefreshViewJSIBinder : public ViewComponentJSIBinder {
  facebook::jsi::Object createNativeProps(facebook::jsi::Runtime& rt) override {
    auto nativeProps = ViewComponentJSIBinder::createNativeProps(rt);
    nativeProps.setProperty(rt, "refreshing", "boolean");
    nativeProps.setProperty(rt, "progressViewOffset", "number");
    nativeProps.setProperty(rt, "progressBackgroundColor", "Color");
    nativeProps.setProperty(rt, "tintColor", "Color");
    nativeProps.setProperty(rt, "title", "string");
    nativeProps.setProperty(rt, "titleColor", "Color");
    nativeProps.setProperty(rt, "enabled", "boolean");
    nativeProps.setProperty(rt, "size", "string");
    return nativeProps;
  }
  facebook::jsi::Object createDirectEventTypes(
      facebook::jsi::Runtime& rt) override {
    facebook::jsi::Object events =
        ViewComponentJSIBinder::createDirectEventTypes(rt);

    events.setProperty(rt, "topRefresh", createDirectEvent(rt, "onRefresh"));

    return events;
  }
};
} // namespace rnoh

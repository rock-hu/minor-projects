
#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>
#include <jsi/jsi.h>

namespace facebook {
namespace react {

class JSI_EXPORT MarqueeViewEventEmitter : public ViewEventEmitter {
  public:
    using ViewEventEmitter::ViewEventEmitter;
    struct OnStop {
      bool isStop;
      std::string type;
    };

    void onStop(OnStop value) const;
};

} // namespace react
} // namespace facebook

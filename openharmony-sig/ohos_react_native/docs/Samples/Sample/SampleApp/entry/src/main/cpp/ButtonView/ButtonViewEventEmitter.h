
#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>
#include <jsi/jsi.h>

namespace facebook {
namespace react {

class JSI_EXPORT ButtonViewEventEmitter : public ViewEventEmitter {
  public:
    using ViewEventEmitter::ViewEventEmitter;
    struct OnButtonClick {
      bool isButtonClick;
      std::string type;
    };

    void onButtonClick(OnButtonClick value) const;
};

} // namespace react
} // namespace facebook

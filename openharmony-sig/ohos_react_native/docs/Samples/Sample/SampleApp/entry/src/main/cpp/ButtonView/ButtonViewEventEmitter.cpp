

#include "ButtonViewEventEmitter.h"

namespace facebook {
namespace react {

void ButtonViewEventEmitter::onButtonClick(OnButtonClick event) const {
    dispatchEvent("topButtonClick", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "isButtonClick", event.isButtonClick);
        payload.setProperty(runtime, "type", event.type);
        return payload;
    });
}

} // namespace react
} // namespace facebook

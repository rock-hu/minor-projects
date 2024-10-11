

#include "MarqueeViewEventEmitter.h"

namespace facebook {
namespace react {

void MarqueeViewEventEmitter::onStop(OnStop event) const {
    dispatchEvent("stop", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "isStop", event.isStop);
        payload.setProperty(runtime, "type", event.type);
        return payload;
    });
}

} // namespace react
} // namespace facebook

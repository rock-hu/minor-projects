

#include <react/renderer/components/rncore/Props.h>
#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>
#include "Props.h"

namespace facebook {
namespace react {
MarqueeViewProps::MarqueeViewProps(
    const PropsParserContext &context, 
    const MarqueeViewProps &sourceProps, 
    const RawProps &rawProps): ViewProps(context, sourceProps, rawProps),

    src(convertRawProp(context, rawProps, "src", sourceProps.src, {""}))
      {}
} // namespace react
} // namespace facebook
#pragma once

#include <jsi/jsi.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/PropsParserContext.h>
#include <react/debug/react_native_assert.h>

namespace facebook {
namespace react {
class JSI_EXPORT MarqueeViewProps final : public ViewProps {
  public:
    MarqueeViewProps() = default;
    MarqueeViewProps(const PropsParserContext &context, const MarqueeViewProps &sourceProps, const RawProps &rawProps);

#pragma mark - Props

    std::string src{""};
};

} // namespace react
} // namespace facebook

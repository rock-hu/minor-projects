/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <jsi/jsi.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/PropsParserContext.h>
#include <react/debug/react_native_assert.h>

namespace facebook {
namespace react {
class JSI_EXPORT ButtonViewProps final : public ViewProps {
  public:
    ButtonViewProps() = default;
    ButtonViewProps(const PropsParserContext &context, const ButtonViewProps &sourceProps, const RawProps &rawProps);

#pragma mark - Props

    std::string buttonText{""};
};

} // namespace react
} // namespace facebook

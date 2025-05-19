/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ECMASCRIPT_TOOLING_AGENT_CSS_IMPL_H
#define ECMASCRIPT_TOOLING_AGENT_CSS_IMPL_H

#include "base/pt_params.h"
#include "base/pt_returns.h"
#include "dispatcher.h"

#include "libpandabase/macros.h"

namespace panda::ecmascript::tooling {
class CssImpl final {
public:
    CssImpl() = default;
    ~CssImpl() = default;

    class DispatcherImpl final : public DispatcherBase {
    public:
        DispatcherImpl(ProtocolChannel *channel, std::unique_ptr<CssImpl> css)
            : DispatcherBase(channel), css_(std::move(css)) {}
        ~DispatcherImpl() override = default;
        void Disable(const DispatchRequest &request);
        void Dispatch(const DispatchRequest &request) override;

        enum class Method {
            DISABLE,
            UNKNOWN
        };
        Method GetMethodEnum(const std::string& method);

    private:
        NO_COPY_SEMANTIC(DispatcherImpl);
        NO_MOVE_SEMANTIC(DispatcherImpl);

        std::unique_ptr<CssImpl> css_ {};
    };

private:
    NO_COPY_SEMANTIC(CssImpl);
    NO_MOVE_SEMANTIC(CssImpl);
};
}  // namespace panda::ecmascript::tooling
#endif
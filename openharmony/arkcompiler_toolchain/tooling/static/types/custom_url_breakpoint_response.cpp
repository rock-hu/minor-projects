/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "types/custom_url_breakpoint_response.h"

#include "utils/json_builder.h"

namespace ark::tooling::inspector {

void CustomUrlBreakpointResponse::Serialize(JsonObjectBuilder &builder) const
{
    std::string id = id_.has_value() ? std::to_string(*id_) : "invalid";
    builder.AddProperty("id", id);
    // NOTE(fangting, #IC98WJ): make 0-based line numbers default for Inspector
    builder.AddProperty("lineNumber", lineNumber_ - 1);
    builder.AddProperty("columnNumber", columnNumber_);
    builder.AddProperty("scriptId", scriptId_);
}

void CustomUrlBreakpointLocations::Serialize(JsonObjectBuilder &builder) const
{
    builder.AddProperty("locations", [this](JsonArrayBuilder &arrayBuilder) {
        for (const auto &loc : locations_) {
            arrayBuilder.Add(loc);
        }
    });
}

}  // namespace ark::tooling::inspector

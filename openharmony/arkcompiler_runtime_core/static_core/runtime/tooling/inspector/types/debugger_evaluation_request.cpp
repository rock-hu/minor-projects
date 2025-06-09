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

#include "types/debugger_evaluation_request.h"

#include "utils/json_parser.h"

#include "types/numeric_id.h"

namespace ark::tooling::inspector {

Expected<DebuggerEvaluationRequest, std::string> DebuggerEvaluationRequest::FromJson(const JsonObject &object)
{
    DebuggerEvaluationRequest parsed;

    auto optFrameId = ParseNumericId<FrameId>(object, "callFrameId");
    if (!optFrameId) {
        return Unexpected(optFrameId.Error());
    }
    parsed.callFrameId_ = *optFrameId;

    const auto *optExpression = object.GetValue<JsonObject::StringT>("expression");
    if (optExpression == nullptr) {
        return Unexpected(std::string("no 'expression' field"));
    }
    parsed.expression_ = optExpression;

    auto optSilent = object.GetValue<JsonObject::BoolT>("silent");
    if (optSilent != nullptr) {
        parsed.silent_ = *optSilent;
    }

    auto optReturnByValue = object.GetValue<JsonObject::BoolT>("returnByValue");
    if (optReturnByValue != nullptr) {
        parsed.returnByValue_ = *optReturnByValue;
    }

    auto optGeneratePreview = object.GetValue<JsonObject::BoolT>("generatePreview");
    if (optGeneratePreview != nullptr) {
        parsed.generatePreview_ = *optGeneratePreview;
    }

    return parsed;
}

}  // namespace ark::tooling::inspector

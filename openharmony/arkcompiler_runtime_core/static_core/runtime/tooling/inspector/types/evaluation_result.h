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

#ifndef PANDA_TOOLING_INSPECTOR_TYPES_EVALUATION_RESULT_H
#define PANDA_TOOLING_INSPECTOR_TYPES_EVALUATION_RESULT_H

#include "tooling/inspector/json_serialization/serializable.h"

#include "macros.h"
#include "utils/json_builder.h"

#include "types/exception_details.h"
#include "types/remote_object.h"

namespace ark::tooling::inspector {

class EvaluationResult : public JsonSerializable {
public:
    explicit EvaluationResult(RemoteObject &&res, std::optional<ExceptionDetails> &&exc)
        : result_(std::move(res)), exceptionDetails_(std::move(exc))  // NOLINT(bugprone-throw-keyword-missing)
    {
    }

    DEFAULT_COPY_SEMANTIC(EvaluationResult);
    DEFAULT_MOVE_SEMANTIC(EvaluationResult);

    ~EvaluationResult() override = default;

    void Serialize(JsonObjectBuilder &builder) const override
    {
        builder.AddProperty("result", result_);
        if (exceptionDetails_) {
            builder.AddProperty("exceptionDetails", *exceptionDetails_);
        }
    }

private:
    RemoteObject result_;
    std::optional<ExceptionDetails> exceptionDetails_;
};

}  // namespace ark::tooling::inspector

#endif  // PANDA_TOOLING_INSPECTOR_TYPES_EVALUATION_RESULT_H

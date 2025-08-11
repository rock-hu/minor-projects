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

#ifndef PANDA_TOOLING_INSPECTOR_JSON_SERIALIZATION_JRPC_ERROR_H
#define PANDA_TOOLING_INSPECTOR_JSON_SERIALIZATION_JRPC_ERROR_H

#include "json_serialization/serializable.h"

namespace ark::tooling::inspector {

enum class ErrorCode {
    PARSE_ERROR = -32700,
    INTERNAL_ERROR = -32603,
    INVALID_PARAMS = -32602,
    METHOD_NOT_FOUND = -32601,
    INVALID_REQUEST = -32600,
    SESSION_NOT_FOUND = -32001,
    SERVER_ERROR = -32000,
};

/// @brief Inspector error object with json-serialization capability.
class JRPCError final : public JsonSerializable {
public:
    explicit JRPCError(std::string_view message, ErrorCode code = ErrorCode::INVALID_REQUEST)
        : code_(code), message_(message)
    {
    }

    explicit JRPCError(std::string &&message, ErrorCode code = ErrorCode::INVALID_REQUEST)
        : code_(code), message_(std::move(message))
    {
    }

    DEFAULT_COPY_SEMANTIC(JRPCError);
    DEFAULT_MOVE_SEMANTIC(JRPCError);

    ~JRPCError() override = default;

    void Serialize(JsonObjectBuilder &builder) const override;

private:
    ErrorCode code_ {0};
    std::string message_;
};

}  // namespace ark::tooling::inspector

#endif  // PANDA_TOOLING_INSPECTOR_JSON_SERIALIZATION_JRPC_ERROR_H

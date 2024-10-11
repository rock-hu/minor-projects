/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/text_field/text_content_type.h"

namespace OHOS::Ace::NG {
TextContentType CastToTextContentType(int32_t value)
{
    if (value < static_cast<int32_t>(TextContentType::BEGIN) || value > static_cast<int32_t>(TextContentType::END)) {
        return TextContentType::BEGIN;
    }
    return static_cast<TextContentType>(value);
}
}  // namespace OHOS::Ace::NG

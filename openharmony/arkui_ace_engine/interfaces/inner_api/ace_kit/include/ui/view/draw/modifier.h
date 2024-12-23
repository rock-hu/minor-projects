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

#ifndef FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_VIEW_DRAW_MODIFIER_H
#define FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_VIEW_DRAW_MODIFIER_H

#include "ui/base/ace_type.h"

namespace OHOS::Rosen::Drawing {
class Canvas;
}

namespace OHOS::Ace::Kit {
using RSCanvas = OHOS::Rosen::Drawing::Canvas;

struct DrawingContext {
    RSCanvas* canvas;
    float width = 0.f;
    float height = 0.f;
};

class Modifier : public Ace::AceType {
    DECLARE_ACE_TYPE(Modifier, Ace::AceType);

public:
    Modifier() = default;
    ~Modifier() = default;
};
} // namespace OHOS::Ace::Kit

#endif

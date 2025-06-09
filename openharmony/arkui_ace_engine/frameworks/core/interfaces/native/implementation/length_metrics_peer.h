/*
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

#pragma once

#include "base/geometry/dimension.h"

struct LengthMetricsPeer final {
    OHOS::Ace::Dimension value;

    static LengthMetricsPeer *Create(const OHOS::Ace::Dimension& src)
    {
        return new LengthMetricsPeer{src};
    }

    static void Destroy(LengthMetricsPeer *peer)
    {
        delete peer;
    }
};

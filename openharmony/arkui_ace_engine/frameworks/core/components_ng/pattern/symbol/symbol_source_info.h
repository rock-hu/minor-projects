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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SYMBOL_SYMBOL_SOURCE_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SYMBOL_SYMBOL_SOURCE_INFO_H

#include <optional>
#include <utility>

#include "base/geometry/dimension.h"
#include "base/geometry/size.h"
#include "base/image/pixel_map.h"
#include "base/resource/internal_resource.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace::NG {
class SymbolSourceInfo {
public:
    explicit SymbolSourceInfo(std::uint32_t unicode);

    SymbolSourceInfo() = default;
    ~SymbolSourceInfo() = default;

    std::uint32_t GetUnicode() const
    {
        return unicode;
    }

    bool operator==(const SymbolSourceInfo& info) const;
    bool operator!=(const SymbolSourceInfo& info) const;

private:
    std::uint32_t unicode;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SYMBOL_SYMBOL_SOURCE_INFO_H

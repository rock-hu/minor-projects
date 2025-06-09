/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_GRID_GRID_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_GRID_GRID_UTILS_H

#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/pattern/grid/grid_layout_property.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {

template<typename Key, typename Value>
Value GetOrDefault(const std::map<Key, Value>& map, const Key& key, const Value& defaultValue)
{
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    } else {
        return defaultValue;
    }
}

class GridUtils {
public:
    GridUtils() = delete;
    ~GridUtils() = delete;

    static std::string ParseArgs(const std::string& args);
    static float GetMainGap(const GridLayoutProperty& props, const SizeF& frameSize, Axis axis);
    static float GetCrossGap(const GridLayoutProperty& props, const SizeF& frameSize, Axis axis);
    static inline float GetMainGap(const RefPtr<GridLayoutProperty>& props, const SizeF& frameSize, Axis axis)
    {
        return GetMainGap(*props, frameSize, axis);
    }
    static inline float GetCrossGap(const RefPtr<GridLayoutProperty>& props, const SizeF& frameSize, Axis axis)
    {
        return GetCrossGap(*props, frameSize, axis);
    }
};

} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_GRID_GRID_UTILS_H

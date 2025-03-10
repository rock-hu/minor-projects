/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCROLL_VIEW_GRID_LAZY_LAYOUT_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCROLL_VIEW_GRID_LAZY_LAYOUT_INFO_H

#include <climits>
#include <cstdint>
#include <map>

#include "base/geometry/axis.h"
#include "base/json/json_util.h"
#include "base/memory/ace_type.h"
#include "base/utils/macros.h"

namespace OHOS::Ace::NG {

class LazyGridLayoutAlgorithm;
class LazyGridLayoutPattern;

struct GridItemMainPos {
    int32_t laneIdx = 0;
    float startPos = 0.0f;
    float endPos = 0.0f;
};

struct AdjustRefPos {
    float start = 0.0f;
    float end = 0.0f;
};

class ACE_EXPORT LazyGridLayoutInfo : public AceType {
    DECLARE_ACE_TYPE(LazyGridLayoutInfo, AceType);
public:
    void EstimateItemSize();
    void UpdatePosMap();
    void SetPosMap(int32_t index, const GridItemMainPos& pos);
    void SetSpace(float space);
    void SetLanes(int32_t lanes);
    void DumpAdvanceInfo();
    void DumpAdvanceInfo(std::unique_ptr<JsonValue>& json);

    int32_t LanesFloor(int32_t index) const;
    int32_t LanesCeil(int32_t index) const;
    int32_t LineCount(int32_t start, int32_t end) const;

    bool NeedPredict() const;
private:
    float UpdatePosMapStart();
    void UpdatePosMapEnd();
    float UpdateSpaceStart();
    float UpdatePosWithIter(std::map<int, GridItemMainPos>::iterator &it, int32_t& prevIndex, float& prevPos) const;

private:
    std::map<int32_t, GridItemMainPos> posMap_;
    int32_t startIndex_ = -1;
    int32_t endIndex_ = -1;
    int32_t totalItemCount_ = 0;
    int32_t lanes_ = 1;
    float totalMainSize_ = 0.0f;
    float spaceWidth_ = 0.0f;

    int32_t updatedStart_ = INT_MAX;
    int32_t updatedEnd_ = -1;
    float estimateItemSize_ = -1.0f;

    // cache
    float layoutedStart_ = 0.0f;
    float layoutedEnd_ = 0.0f;
    int32_t layoutedStartIndex_ = -1;
    int32_t layoutedEndIndex_ = -1;
    float cacheStartPos_ = 0.0f;
    float cacheEndPos_ = 0.0f;
    int32_t cachedStartIndex_ = -1;
    int32_t cachedEndIndex_ = -1;
    std::optional<int64_t> deadline_;

    friend class LazyGridLayoutAlgorithm;
    friend class LazyGridLayoutPattern;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCROLL_VIEW_GRID_LAZY_LAYOUT_PROPERTY_H

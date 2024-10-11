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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_LAYOUT_GRID_CONTAINER_UTIL_CLASS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_LAYOUT_GRID_CONTAINER_UTIL_CLASS_H

#include <utility>

#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components/common/layout/grid_container_info.h"

namespace OHOS::Ace::V2 {

constexpr int32_t DEFAULT_COLUMN_NUMBER = 12;
struct GridContainerSize : public Referenced {
    GridContainerSize() = default;
    explicit GridContainerSize(int32_t column)
    {
        xs = column;
        sm = column;
        md = column;
        lg = column;
        xl = column;
        xxl = column;
    };
    DEFINE_COPY_CONSTRUCTOR_AND_COPY_OPERATOR_AND_COMPARE_OPERATOR_WITH_PROPERTIES(
        GridContainerSize, (xs)(sm)(md)(lg)(xl)(xxl))
    int32_t xs = DEFAULT_COLUMN_NUMBER;
    int32_t sm = DEFAULT_COLUMN_NUMBER;
    int32_t md = DEFAULT_COLUMN_NUMBER;
    int32_t lg = DEFAULT_COLUMN_NUMBER;
    int32_t xl = DEFAULT_COLUMN_NUMBER;
    int32_t xxl = DEFAULT_COLUMN_NUMBER;
};

enum class BreakPointsReference {
    WindowSize,
    ComponentSize,
};

enum class GridRowDirection {
    Row,
    RowReverse,
};

enum class GridSizeType {
    XS = 0,
    SM = 1,
    MD = 2,
    LG = 3,
    XL = 4,
    XXL = 5,
    UNDEFINED = 6,
};

struct GridSizeInfo : public Referenced {
    std::vector<Dimension> sizeInfo {
        Dimension(320, DimensionUnit::VP),
        Dimension(600, DimensionUnit::VP),
        Dimension(840, DimensionUnit::VP),
    };

    void Reset()
    {
        sizeInfo.clear();
    }
};

class Gutter : public AceType {
    DECLARE_ACE_TYPE(Gutter, AceType);

public:
    Gutter() = default;
    DEFINE_COPY_CONSTRUCTOR_AND_COPY_OPERATOR_AND_COMPARE_OPERATOR_WITH_PROPERTIES(
        Gutter, (xXs)(yXs)(xSm)(ySm)(xMd)(yMd)(xLg)(yLg)(xXl)(yXl)(xXXl)(yXXl))
    explicit Gutter(Dimension dimension)
        : xXs(dimension), yXs(dimension), xSm(dimension), ySm(dimension), xMd(dimension), yMd(dimension),
          xLg(dimension), yLg(dimension), xXl(dimension), yXl(dimension), xXXl(dimension), yXXl(dimension) {};
    Gutter(Dimension xDimension, Dimension yDimension)
        : xXs(xDimension), yXs(yDimension), xSm(xDimension), ySm(yDimension), xMd(xDimension), yMd(yDimension),
          xLg(xDimension), yLg(yDimension), xXl(xDimension), yXl(yDimension), xXXl(xDimension), yXXl(yDimension) {};

    void SetYGutter(Dimension yDimension)
    {
        yXs = yDimension;
        ySm = yDimension;
        yMd = yDimension;
        yLg = yDimension;
        yXl = yDimension;
        yXXl = yDimension;
    }

    void SetXGutter(Dimension xDimension)
    {
        xXs = xDimension;
        xSm = xDimension;
        xMd = xDimension;
        xLg = xDimension;
        xXl = xDimension;
        xXXl = xDimension;
    }
    Dimension xXs;
    Dimension yXs;
    Dimension xSm;
    Dimension ySm;
    Dimension xMd;
    Dimension yMd;
    Dimension xLg;
    Dimension yLg;
    Dimension xXl;
    Dimension yXl;
    Dimension xXXl;
    Dimension yXXl;
};

class BreakPoints : public AceType {
    DECLARE_ACE_TYPE(BreakPoints, AceType);

public:
    BreakPoints() = default;
    DEFINE_COPY_CONSTRUCTOR_AND_COPY_OPERATOR_AND_COMPARE_OPERATOR_WITH_PROPERTIES(
        BreakPoints, (reference)(breakpoints))
    BreakPointsReference reference = BreakPointsReference::WindowSize;
    std::vector<std::string> breakpoints { "320vp", "600vp", "840vp" };
};

} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_LAYOUT_GRID_CONTAINER_UTIL_CLASS_H
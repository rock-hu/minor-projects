/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_RECT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_RECT_H

#include <cmath>
#include <limits>
#include <functional>

#include "base/geometry/dimension.h"
#include "base/geometry/dimension_offset.h"
#include "base/geometry/dimension_size.h"
#include "base/json/json_util.h"
#include "core/common/resource/resource_object.h"

namespace OHOS::Ace {

class DimensionRect {
public:
    DimensionRect() = default;
    ~DimensionRect() = default;

    DimensionRect(const Dimension& width, const Dimension& height, const DimensionOffset& offset)
        : width_(width), height_(height), offset_(offset)
    {}

    DimensionRect(const Dimension& width, const Dimension& height) : width_(width), height_(height) {}

    const Dimension& GetWidth() const
    {
        return width_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    const DimensionOffset& GetOffset() const
    {
        return offset_;
    }

    void SetOffset(const DimensionOffset& offset)
    {
        offset_ = offset;
    }

    void SetSize(const DimensionSize& size)
    {
        width_ = size.Width();
        height_ = size.Height();
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    void Reset()
    {
        width_ = 0.0_vp;
        height_ = 0.0_vp;
        offset_ = DimensionOffset();
    }

    std::string ToString() const
    {
        static const int32_t precision = 2;
        std::stringstream ss;
        ss << "Rect (" << std::fixed << std::setprecision(precision) << offset_.GetX().ToString() << ", "
           << offset_.GetY().ToString() << ") - [";
        ss << width_.ToString();
        ss << " x ";
        ss << height_.ToString();
        ss << "]";
        std::string output = ss.str();
        return output;
    }

    std::string ToJsonString() const
    {
        auto jsonValue = JsonUtil::Create(true);
        jsonValue->Put("x", offset_.GetX().ToString().c_str());
        jsonValue->Put("y", offset_.GetY().ToString().c_str());
        jsonValue->Put("width", width_.ToString().c_str());
        jsonValue->Put("height", height_.ToString().c_str());
        return jsonValue->ToString();
    }
    void AddResource(const std::string& key, const RefPtr<ResourceObject>& resObj,
        std::function<void(const RefPtr<ResourceObject>&, DimensionRect&)>&& updateFunc)
    {
        if (resObj == nullptr || !updateFunc) {
            return;
        }
        resMap_[key] = { resObj, std::move(updateFunc) };
    }

    void ReloadResources()
    {
        for (const auto& [key, resourceUpdater] : resMap_) {
            resourceUpdater.updateFunc(resourceUpdater.resObj, *this);
        }
    }

private:
    Dimension width_ = 0.0_vp;
    Dimension height_ = 0.0_vp;
    DimensionOffset offset_;
    struct ResourceUpdater {
        RefPtr<ResourceObject> resObj;
        std::function<void(const RefPtr<ResourceObject>&, DimensionRect&)> updateFunc;
    };
    std::unordered_map<std::string, ResourceUpdater> resMap_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_RECT_H

/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_BASE_GEOMETRY_POINT_H
#define FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_BASE_GEOMETRY_POINT_H

#include "ui/base/geometry/offset.h"
#include "ui/base/macros.h"
#include "ui/base/utils/utils.h"

#include "core/event/ace_events.h"

namespace OHOS::Ace {

class ACE_EXPORT Point {
public:
    Point() = default;
    ~Point() = default;
    Point(double x, double y) : x_(x), y_(y) {}
    Point(double x, double y, SourceType sourceType) : x_(x), y_(y), sourceType_(sourceType) {}
    Point(double x, double y, double screenX, double screenY) : x_(x), y_(y), screenX_(screenX), screenY_(screenY) {}
    Point(double x, double y, double screenX, double screenY, SourceType sourceType)
        : x_(x), y_(y), screenX_(screenX), screenY_(screenY), sourceType_(sourceType)
    {}

    Point(double x, double y, double screenX, double screenY, double globalDisplayX, double globalDisplayY)
        : x_(x), y_(y), screenX_(screenX), screenY_(screenY), globalDisplayX_(globalDisplayX),
          globalDisplayY_(globalDisplayY)
    {}
    Point(double x, double y, double screenX, double screenY, double globalDisplayX, double globalDisplayY,
        SourceType sourceType)
        : x_(x), y_(y), screenX_(screenX), screenY_(screenY), globalDisplayX_(globalDisplayX),
          globalDisplayY_(globalDisplayY), sourceType_(sourceType)
    {}

    double GetX() const
    {
        return x_;
    }

    double GetY() const
    {
        return y_;
    }

    void SetX(double x)
    {
        x_ = x;
    }

    void SetY(double y)
    {
        y_ = y;
    }

    double GetScreenX() const
    {
        return screenX_;
    }

    double GetScreenY() const
    {
        return screenY_;
    }

    void SetScreenX(double x)
    {
        screenX_ = x;
    }

    void SetScreenY(double y)
    {
        screenY_ = y;
    }

    double GetGlobalDisplayX() const
    {
        return globalDisplayX_;
    }

    double GetGlobalDisplayY() const
    {
        return globalDisplayY_;
    }

    void SetGlobalDisplayX(double x)
    {
        globalDisplayX_ = x;
    }

    void SetGlobalDisplayY(double y)
    {
        globalDisplayY_ = y;
    }

    void SetSourceType(SourceType sourceType)
    {
        sourceType_ = sourceType;
    }

    SourceType GetSourceType() const
    {
        return sourceType_;
    }

    void Rotate(const Point& center, double angle)
    {
        double x = (x_ - center.GetX()) * std::cos(angle) - (y_ - center.GetY()) * std::sin(angle) + center.GetX();
        double y = (x_ - center.GetX()) * std::sin(angle) + (y_ - center.GetY()) * std::cos(angle) + center.GetY();
        x_ = x;
        y_ = y;
    }

    Point operator-(const Offset& offset) const
    {
        return Point(x_ - offset.GetX(), y_ - offset.GetY());
    }

    Point operator+(const Offset& offset) const
    {
        return Point(x_ + offset.GetX(), y_ + offset.GetY());
    }

    Offset operator-(const Point& point) const
    {
        return Offset(x_ - point.x_, y_ - point.y_);
    }

    bool operator==(const Point& point) const
    {
        return NearEqual(x_, point.x_) && NearEqual(y_, point.y_);
    }

    bool operator!=(const Point& point) const
    {
        return !operator==(point);
    }

private:
    double x_ = 0.0;
    double y_ = 0.0;
    double screenX_ = 0.0;
    double screenY_ = 0.0;
    double globalDisplayX_ = 0.0;
    double globalDisplayY_ = 0.0;
    SourceType sourceType_ = SourceType::NONE;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_BASE_GEOMETRY_POINT_H

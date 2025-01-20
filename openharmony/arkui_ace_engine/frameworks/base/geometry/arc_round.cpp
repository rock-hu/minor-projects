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
#include "arc_round.h"

#ifndef PI_NUM
#define PI_NUM 3.14f
#endif

namespace OHOS::Ace {
ArcRound::ArcRound(const ArcRound& other)
{
    *this = other;
}

ArcRound::ArcRound(Point centerPoint, float radius, float startAngle, float sweepAngle, float width)
{
    centerPoint_ = centerPoint;
    radius_ = radius;
    startAngle_ = startAngle;
    sweepAngle_ = sweepAngle;
    width_ = width;
    outerRadius_ = radius + width * HALF;
}

ArcRound& ArcRound::operator=(const ArcRound& arc)
{
    centerPoint_ = arc.GetCenterPoint();
    radius_ = arc.GetRadius();
    startAngle_ = arc.GetStartAngle();
    sweepAngle_ = arc.GetSweepAngle();
    width_ = arc.GetWidth();
    outerRadius_ = radius_ + width_ * HALF;
    return *this;
}

void ArcRound::Rotate(const Point& point, float angle)
{
    centerPoint_.Rotate(point, angle);
    startAngle_ += angle;
    endAngle_ += angle;
}

void ArcRound::Move(float xOffset, float yOffset)
{
    centerPoint_.SetX(centerPoint_.GetX() + xOffset);
    centerPoint_.SetY(centerPoint_.GetY() + yOffset);
}

void ArcRound::GetPointByAngle(float angle, Point& out) const
{
    out.SetX(centerPoint_.GetX() + radius_);
    out.SetY(centerPoint_.GetY());
    out.Rotate(centerPoint_, angle);
}

float ArcRound::GetPositionAngle(const Offset& position) const
{
    float angle = atan2(position.GetY() - centerPoint_.GetY(), position.GetX() - centerPoint_.GetX());
    return angle;
}

Point ArcRound::GetStartPoint() const
{
    Point startPoint;
    GetStartPoint(startPoint);
    return startPoint;
}

Point ArcRound::GetEndPoint() const
{
    Point endPoint;
    GetEndPoint(endPoint);
    return endPoint;
}

void ArcRound::SetOuterRadius(float outerRadius)
{
    if (outerRadius < 0.0) {
        LOGE("Invalid outerRadius:%{public}lf", outerRadius);
        return;
    }
    outerRadius_ = outerRadius;
}

void ArcRound::SetWidth(float width)
{
    if (width < 0.0) {
        LOGE("Invalid width:%{public}lf", width);
        return;
    }
    width_ = width;
}

double ArcRound::Get2PIRadians(double radian) const
{
    if (radian < 0) {
        return 2 * PI_NUM + radian; // 2 * PI: 360度
    }
    return radian;
}

bool ArcRound::IsInRegion(const Point& point) const
{
    float distance = pow(pow(point.GetX() - centerPoint_.GetX(), SQUARE) +
        pow(point.GetY() - centerPoint_.GetY(), SQUARE), HALF);
    float angle = GetPositionAngle(Offset(point.GetX(), point.GetY()));
    if (distance > outerRadius_ || distance < outerRadius_ - width_) {
        return false;
    }
    auto startAngle = startAngle_;
    auto endAngle = startAngle_ + sweepAngle_;
    auto startRadian = startAngle * PI_NUM / HALF_CIRCULARITY;
    auto endRadian = endAngle * PI_NUM / HALF_CIRCULARITY;
    if (positionMode_ == PositionMode::LEFT) {
        endAngle = startAngle_ + sweepAngle_ <= -HALF_CIRCULARITY ? startAngle_ + sweepAngle_ + WHOLE_CIRCULARITY :
                                                                    startAngle_ + sweepAngle_;
        startRadian = Get2PIRadians(startRadian);
        endRadian = Get2PIRadians(endAngle * PI_NUM / HALF_CIRCULARITY);
        angle = Get2PIRadians(angle);
        if (angle < endRadian || angle > startRadian) {
            return false;
        }
    } else {
        if (angle < startRadian || angle > endRadian) {
            return false;
        }
    }
    return true;
}
} // namespace OHOS::Ace
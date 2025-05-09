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

#ifndef FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_ANIMATION_CURVES_H
#define FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_ANIMATION_CURVES_H

#include "ui/animation/anticipate_curve.h"
#include "ui/animation/cubic_curve.h"
#include "ui/animation/curve.h"
#include "ui/base/macros.h"

namespace OHOS::Ace {

class ACE_FORCE_EXPORT Curves {
public:
    Curves() = delete;
    ~Curves() = default;
    static const RefPtr<DecelerationCurve> DECELE;
    static const RefPtr<LinearCurve> LINEAR;
    static const RefPtr<SineCurve> SINE;
    static const RefPtr<CubicCurve> EASE;
    static const RefPtr<CubicCurve> EASE_IN;
    static const RefPtr<CubicCurve> EASE_OUT;
    static const RefPtr<CubicCurve> EASE_IN_OUT;
    static const RefPtr<CubicCurve> FAST_OUT_SLOW_IN;
    static const RefPtr<CubicCurve> LINEAR_OUT_SLOW_IN;
    static const RefPtr<CubicCurve> FAST_OUT_LINEAR_IN;
    static const RefPtr<CubicCurve> FRICTION;
    static const RefPtr<CubicCurve> EXTREME_DECELERATION;
    static const RefPtr<CubicCurve> SHARP;
    static const RefPtr<CubicCurve> RHYTHM;
    static const RefPtr<CubicCurve> SMOOTH;
    static const RefPtr<AnticipateCurve> ANTICIPATE;
    static const RefPtr<CubicCurve> MAGNETIC;
    static const RefPtr<ElasticsCurve> ELASTICS;
    static const std::string DEFAULT_CURVE_NAME;
    static std::string ToString(const RefPtr<Curve>& curve);
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_INTERFACES_INNER_API_ACE_KIT_INCLUDE_ANIMATION_CURVES_H

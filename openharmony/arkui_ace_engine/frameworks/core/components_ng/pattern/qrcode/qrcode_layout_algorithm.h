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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_QRCODE_QRCODE_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_QRCODE_QRCODE_LAYOUT_ALGORITHM_H

#include <cstdint>
#include <map>
#include <optional>

#include "base/geometry/axis.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT QRCodeLayoutAlgorithm : public BoxLayoutAlgorithm {
    DECLARE_ACE_TYPE(QRCodeLayoutAlgorithm, BoxLayoutAlgorithm);

public:
    QRCodeLayoutAlgorithm() = default;
    ~QRCodeLayoutAlgorithm() override = default;

    std::optional<SizeF> MeasureContent(
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper) override;

    float GetQRCodeSize() const
    {
        return qrCodeSize_;
    }

private:
    Color color_ = Color::BLACK;
    Color backgroundColor_ = Color::WHITE;
    float qrCodeSize_ = 0.0f;
};

} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_QRCODE_QRCODE_LAYOUT_ALGORITHM_H

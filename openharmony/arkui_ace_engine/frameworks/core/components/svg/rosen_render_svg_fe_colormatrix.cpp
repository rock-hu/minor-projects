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

#include "frameworks/core/components/svg/rosen_render_svg_fe_colormatrix.h"

namespace OHOS::Ace {

#ifndef USE_ROSEN_DRAWING
void RosenRenderSvgFeColorMatrix::OnAsImageFilter(sk_sp<SkImageFilter>& imageFilter) const
{
    auto colorFilter = SkColorFilters::Matrix(matrix_);

    imageFilter = SkImageFilters::ColorFilter(colorFilter, imageFilter);
}
#else
void RosenRenderSvgFeColorMatrix::OnAsImageFilter(std::shared_ptr<RSImageFilter>& imageFilter) const
{
    RSColorMatrix colorMatrix;
    colorMatrix.SetArray(matrix_);
    auto colorFilter = RSRecordingColorFilter::CreateMatrixColorFilter(colorMatrix);
    if (colorFilter) {
        imageFilter = RSRecordingImageFilter::CreateColorFilterImageFilter(*colorFilter, imageFilter);
    }
}
#endif

} // namespace OHOS::Ace

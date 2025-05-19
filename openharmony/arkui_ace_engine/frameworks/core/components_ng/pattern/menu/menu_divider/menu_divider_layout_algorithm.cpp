/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/menu/menu_divider/menu_divider_layout_algorithm.h"

#include "core/components_ng/pattern/menu/menu_divider/menu_divider_pattern.h"

namespace OHOS::Ace::NG {
std::optional<SizeF> MenuDividerLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_RETURN(layoutWrapper, std::nullopt);
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(host, std::nullopt);
    auto pattern = host->GetPattern<MenuDividerPattern>();
    CHECK_NULL_RETURN(pattern, std::nullopt);
    auto menuItemNode = pattern->GetMenuItem();
    CHECK_NULL_RETURN(menuItemNode, SizeF());
    auto geometryNode = menuItemNode->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, SizeF());
    auto property = host->GetPaintProperty<MenuDividerPaintProperty>();
    CHECK_NULL_RETURN(property, SizeF());
    auto strokeWidth = property->GetStrokeWidthValue(Dimension(0, DimensionUnit::INVALID));
    auto itemSize = geometryNode->GetFrameSize();
    if (strokeWidth.Unit() != DimensionUnit::INVALID) {
        return SizeF(itemSize.Width(), strokeWidth.ConvertToPxWithSize(itemSize.Height()));
    } else {
        return SizeF(itemSize.Width(), themeStrokeWidth_.ConvertToPx());
    }
}
} // namespace OHOS::Ace::NG
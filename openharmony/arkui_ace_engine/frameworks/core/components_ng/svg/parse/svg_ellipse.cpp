/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "frameworks/core/components_ng/svg/parse/svg_ellipse.h"

#include "frameworks/core/components_ng/svg/parse/svg_constants.h"

namespace OHOS::Ace::NG {

SvgEllipse::SvgEllipse() : SvgGraphic() {}

RefPtr<SvgNode> SvgEllipse::Create()
{
    return AceType::MakeRefPtr<SvgEllipse>();
}

#ifndef USE_ROSEN_DRAWING
SkPath SvgEllipse::AsPath(const Size& viewPort) const
{
    SkPath path;
#else
RSRecordingPath SvgEllipse::AsPath(const Size& viewPort) const
{
    RSRecordingPath path;
#endif
    double rx = 0.0;
    if (GreatOrEqual(ellipseAttr_.rx.Value(), 0.0)) {
        rx = ConvertDimensionToPx(ellipseAttr_.rx, viewPort, SvgLengthType::HORIZONTAL);
    } else {
        if (GreatNotEqual(ellipseAttr_.ry.Value(), 0.0)) {
            rx = ConvertDimensionToPx(ellipseAttr_.ry, viewPort, SvgLengthType::VERTICAL);
        }
    }
    double ry = 0.0;
    if (GreatOrEqual(ellipseAttr_.ry.Value(), 0.0)) {
        ry = ConvertDimensionToPx(ellipseAttr_.ry, viewPort, SvgLengthType::VERTICAL);
    } else {
        if (GreatNotEqual(ellipseAttr_.rx.Value(), 0.0)) {
            ry = ConvertDimensionToPx(ellipseAttr_.rx, viewPort, SvgLengthType::HORIZONTAL);
        }
    }
#ifndef USE_ROSEN_DRAWING
    SkRect rect = SkRect::MakeXYWH(ConvertDimensionToPx(ellipseAttr_.cx, viewPort, SvgLengthType::HORIZONTAL) - rx,
        ConvertDimensionToPx(ellipseAttr_.cy, viewPort, SvgLengthType::VERTICAL) - ry, rx + rx, ry + ry);
    path.addOval(rect);
#else
    RSScalar left = ConvertDimensionToPx(ellipseAttr_.cx, viewPort, SvgLengthType::HORIZONTAL) - rx;
    RSScalar top = ConvertDimensionToPx(ellipseAttr_.cy, viewPort, SvgLengthType::VERTICAL) - ry;
    RSRect rect = RSRect(left, top, rx + rx + left, ry + ry + top);
    path.AddOval(rect);
#endif
    return path;
}

bool SvgEllipse::ParseAndSetSpecializedAttr(const std::string& name, const std::string& value)
{
    static const LinearMapNode<void (*)(const std::string&, SvgEllipseAttribute&)> attrs[] = {
        { SVG_CX,
            [](const std::string& val, SvgEllipseAttribute& attr) {
                attr.cx = SvgAttributesParser::ParseDimension(val);
            } },
        { SVG_CY,
            [](const std::string& val, SvgEllipseAttribute& attr) {
                attr.cy = SvgAttributesParser::ParseDimension(val);
            } },
        { SVG_RX,
            [](const std::string& val, SvgEllipseAttribute& attr) {
                attr.rx = SvgAttributesParser::ParseDimension(val);
            } },
        { SVG_RY,
            [](const std::string& val, SvgEllipseAttribute& attr) {
                attr.ry = SvgAttributesParser::ParseDimension(val);
            } },
    };
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), name.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(value, ellipseAttr_);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace::NG

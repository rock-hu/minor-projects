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

#include "core/components/declaration/svg/svg_circle_declaration.h"

#include "core/components/declaration/common/declaration_constants.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgCircleDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_CIRCLE_ATTR);
}

void SvgCircleDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgCircleAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgCircleDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgCircleDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgCircleDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgCircleDeclaration&)> attrs[] = {
        { DOM_SVG_CX,
            [](const std::string& val, SvgCircleDeclaration& declaration) {
                declaration.SetCx(declaration.ParseDimension(val));
            } },
        { DOM_SVG_CY,
            [](const std::string& val, SvgCircleDeclaration& declaration) {
                declaration.SetCy(declaration.ParseDimension(val));
            } },
        { DOM_SVG_R,
            [](const std::string& val, SvgCircleDeclaration& declaration) {
                declaration.SetR(declaration.ParseDimension(val));
            } },
    };
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), attr.first.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return SetPresentationAttr(attr);
}

} // namespace OHOS::Ace
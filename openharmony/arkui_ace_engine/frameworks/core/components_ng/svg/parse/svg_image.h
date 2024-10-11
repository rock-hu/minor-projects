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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_SVG_PARSE_SVG_IMAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_SVG_PARSE_SVG_IMAGE_H

#include "frameworks/core/components_ng/svg/parse/svg_attributes_parser.h"
#include "frameworks/core/components_ng/svg/parse/svg_node.h"

namespace OHOS::Ace::NG {

class SvgImage : public SvgNode {
    DECLARE_ACE_TYPE(SvgImage, SvgNode);

public:
    SvgImage();
    ~SvgImage() override = default;
    static RefPtr<SvgNode> Create();

    void OnDraw(RSCanvas& canvas, const Size& layout, const std::optional<Color>& color) override;
    std::shared_ptr<RSData> LoadLocalImage(const std::string& uri);
    std::shared_ptr<RSData> LoadBase64Image(const std::string& uri);
    SrcType ParseHrefAttr(const std::string& uri);
    RSRect CalcDstRect(const Size& realSize, const Rect& viewBox);
    bool ParseAndSetSpecializedAttr(const std::string& name, const std::string& value) override;

private:
    SvgImageAttribute imageAttr_;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_SVG_PARSE_SVG_IMAGE_H

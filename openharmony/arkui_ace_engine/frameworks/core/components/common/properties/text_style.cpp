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

#include "core/components/common/properties/text_style.h"

namespace OHOS::Ace {
const std::vector<WordBreak> WORD_BREAK_TYPES = {
    WordBreak::NORMAL, WordBreak::BREAK_ALL, WordBreak::BREAK_WORD, WordBreak::HYPHENATION};
const std::vector<LineBreakStrategy> LINE_BREAK_STRATEGY_TYPES = {
    LineBreakStrategy::GREEDY, LineBreakStrategy::HIGH_QUALITY, LineBreakStrategy::BALANCED};
TextStyle::TextStyle(const std::vector<std::string> &fontFamilies, double fontSize, FontWeight fontWeight,
    FontStyle fontStyle, const Color &textColor)
    : propFontFamilies_(fontFamilies), propFontStyle_(fontStyle), propTextColor_(textColor), fontSize_(fontSize),
      fontWeight_(fontWeight)
{}

bool TextStyle::operator==(const TextStyle &rhs) const
{
    return propFontFamilies_ == rhs.propFontFamilies_ && fontFeatures_ == rhs.fontFeatures_ &&
           propTextDecorationStyle_ == rhs.propTextDecorationStyle_ && preferFontSizes_ == rhs.preferFontSizes_ &&
           fontSize_ == rhs.fontSize_ && adaptMinFontSize_ == rhs.adaptMinFontSize_ &&
           adaptMaxFontSize_ == rhs.adaptMaxFontSize_ && propAdaptFontSizeStep_ == rhs.propAdaptFontSizeStep_ &&
           lineHeight_ == rhs.lineHeight_ && fontWeight_ == rhs.fontWeight_ && propFontStyle_ == rhs.propFontStyle_ &&
           propTextBaseline_ == rhs.propTextBaseline_ && propTextOverflow_ == rhs.propTextOverflow_ &&
           propTextAlign_ == rhs.propTextAlign_ && propTextColor_ == rhs.propTextColor_ &&
           propTextDecoration_ == rhs.propTextDecoration_ && propTextShadows_ == rhs.propTextShadows_ &&
           propLetterSpacing_ == rhs.propLetterSpacing_ && propMaxLines_ == rhs.propMaxLines_ &&
           adaptTextSize_ == rhs.adaptTextSize_ && propAllowScale_ == rhs.propAllowScale_ &&
           propWordBreak_ == rhs.propWordBreak_ && propTextDecorationColor_ == rhs.propTextDecorationColor_ &&
           propTextCase_ == rhs.propTextCase_ && propBaselineOffset_ == rhs.propBaselineOffset_ &&
           adaptHeight_ == rhs.adaptHeight_ && propTextIndent_ == rhs.propTextIndent_ &&
           propTextVerticalAlign_ == rhs.propTextVerticalAlign_ && propWordSpacing_ == rhs.propWordSpacing_ &&
           propEllipsisMode_ == rhs.propEllipsisMode_ && propLineBreakStrategy_ == rhs.propLineBreakStrategy_ &&
           propTextBackgroundStyle_ == rhs.propTextBackgroundStyle_;
}

bool TextStyle::operator!=(const TextStyle &rhs) const
{
    return !(rhs == *this);
}

void TextStyle::SetAdaptTextSize(
    const Dimension &maxFontSize, const Dimension &minFontSize, const Dimension &fontSizeStep)
{
    adaptMaxFontSize_ = maxFontSize;
    adaptMinFontSize_ = minFontSize;
    propAdaptFontSizeStep_ = fontSizeStep;
    adaptTextSize_ = true;
}

void TextBackgroundStyle::ToJsonValue(std::unique_ptr<JsonValue> &json, const std::optional<TextBackgroundStyle> &style,
    const NG::InspectorFilter &filter)
{
    NG::BorderRadiusProperty defaultRadius;
    TextBackgroundStyle exportStyle = {.backgroundColor = Color::TRANSPARENT, .backgroundRadius = defaultRadius};
    if (style.has_value()) {
        exportStyle.backgroundColor = style.value().backgroundColor.value_or(Color::TRANSPARENT);
        exportStyle.backgroundRadius = style.value().backgroundRadius.value_or(defaultRadius);
    }

    auto styleJson = JsonUtil::Create(true);
    styleJson->Put("color", exportStyle.backgroundColor->ColorToString().c_str());
    auto radiusJson = JsonUtil::Create(true);
    exportStyle.backgroundRadius->ToJsonValue(radiusJson, styleJson, filter);

    json->PutExtAttr("textBackgroundStyle", styleJson, filter);
}

void TextStyle::UpdateColorByResourceId()
{
    propTextColor_.UpdateColorByResourceId();
    propTextDecorationColor_.UpdateColorByResourceId();
    if (propTextBackgroundStyle_.has_value()) {
        propTextBackgroundStyle_->UpdateColorByResourceId();
    }
    std::for_each(propRenderColors_.begin(), propRenderColors_.end(), [](Color &cl) { cl.UpdateColorByResourceId(); });
    std::for_each(propTextShadows_.begin(), propTextShadows_.end(), [](Shadow &sd) { sd.UpdateColorByResourceId(); });
}

std::string TextStyle::ToString() const
{
    auto jsonValue = JsonUtil::Create(true);
    JSON_STRING_PUT_STRINGABLE(jsonValue, fontSize_);
    JSON_STRING_PUT_STRINGABLE(jsonValue, propTextColor_);
    JSON_STRING_PUT_STRINGABLE(jsonValue, lineHeight_);
    JSON_STRING_PUT_STRINGABLE(jsonValue, propBaselineOffset_);
    JSON_STRING_PUT_STRINGABLE(jsonValue, propWordSpacing_);
    JSON_STRING_PUT_STRINGABLE(jsonValue, propTextIndent_);
    JSON_STRING_PUT_STRINGABLE(jsonValue, propLetterSpacing_);
    JSON_STRING_PUT_STRINGABLE(jsonValue, lineSpacing_);

    JSON_STRING_PUT_INT(jsonValue, fontWeight_);
    JSON_STRING_PUT_INT(jsonValue, propFontStyle_);
    JSON_STRING_PUT_INT(jsonValue, propTextBaseline_);
    JSON_STRING_PUT_INT(jsonValue, propTextOverflow_);
    JSON_STRING_PUT_INT(jsonValue, propTextVerticalAlign_);
    JSON_STRING_PUT_INT(jsonValue, propTextAlign_);
    JSON_STRING_PUT_INT(jsonValue, propTextDecorationStyle_);
    JSON_STRING_PUT_INT(jsonValue, propTextDecoration_);
    JSON_STRING_PUT_INT(jsonValue, propWhiteSpace_);
    JSON_STRING_PUT_INT(jsonValue, propWordBreak_);
    JSON_STRING_PUT_INT(jsonValue, propTextCase_);
    JSON_STRING_PUT_INT(jsonValue, propEllipsisMode_);
    JSON_STRING_PUT_INT(jsonValue, propLineBreakStrategy_);

    std::stringstream ss;
    std::for_each(
        propRenderColors_.begin(), propRenderColors_.end(), [&ss](const Color &c) { ss << c.ToString() << ","; });
    jsonValue->Put("renderColors", ss.str().c_str());
    JSON_STRING_PUT_INT(jsonValue, propRenderStrategy_);
    JSON_STRING_PUT_INT(jsonValue, propEffectStrategy_);
    JSON_STRING_PUT_OPTIONAL_STRINGABLE(jsonValue, propSymbolEffectOptions_);

    return jsonValue->ToString();
}

}  // namespace OHOS::Ace

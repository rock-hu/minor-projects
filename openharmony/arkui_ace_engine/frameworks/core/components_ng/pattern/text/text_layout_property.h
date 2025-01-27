/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_LAYOUT_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_LAYOUT_PROPERTY_H

#include <string>

#include "base/utils/utf_helper.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/pattern/symbol/symbol_effect_options.h"
#include "core/components_ng/pattern/symbol/symbol_source_info.h"
#include "core/components_ng/pattern/text/text_styles.h"
#include "core/components_ng/property/property.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::NG {
#define ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(group, name, type, changeFlag) \
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP_GET(group, name, type)                  \
    void Update##name(const type& value)                                        \
    {                                                                           \
        auto& groupProperty = GetOrCreate##group();                             \
        if (groupProperty->Check##name(value)) {                                \
            return;                                                             \
        }                                                                       \
        groupProperty->Update##name(value);                                     \
        UpdatePropertyChangeFlag(changeFlag);                                   \
        propNeedReCreateParagraph_ = true;                                      \
    }

#define ACE_DEFINE_TEXT_PROPERTY_ITEM_WITHOUT_GROUP(name, type, changeFlag) \
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP_GET(name, type)                  \
public:                                                                     \
    void Update##name(const type& value)                                    \
    {                                                                       \
        if (prop##name##_.has_value()) {                                    \
            if (NearEqual(prop##name##_.value(), value)) {                  \
                return;                                                     \
            }                                                               \
        }                                                                   \
        prop##name##_ = value;                                              \
        UpdatePropertyChangeFlag(changeFlag);                               \
        propNeedReCreateParagraph_ = true;                                  \
    }

#define ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP_ITEM(group, groupItem, name, type, changeFlag) \
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP_ITEM_GET(group, groupItem, name, type)                  \
    void Update##name(const type& value)                                                        \
    {                                                                                           \
        auto& groupProperty = GetOrCreate##group();                                             \
        if (groupProperty->Check##groupItem(value)) {                                           \
            return;                                                                             \
        }                                                                                       \
        groupProperty->Update##groupItem(value);                                                \
        UpdatePropertyChangeFlag(changeFlag);                                                   \
        propNeedReCreateParagraph_ = true;                                                      \
    }

class InspectorFilter;

struct TextMarqueeOptions {
    ACE_DEFINE_PROPERTY_GROUP_ITEM(TextMarqueeStart, bool);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(TextMarqueeStep, double);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(TextMarqueeLoop, int32_t);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(TextMarqueeDirection, MarqueeDirection);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(TextMarqueeDelay, int32_t);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(TextMarqueeFadeout, bool);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(TextMarqueeStartPolicy, MarqueeStartPolicy);
};

class ACE_EXPORT TextLayoutProperty : public LayoutProperty {
    DECLARE_ACE_TYPE(TextLayoutProperty, LayoutProperty);

public:
    TextLayoutProperty() = default;

    ~TextLayoutProperty() override = default;

    RefPtr<LayoutProperty> Clone() const override
    {
        auto value = MakeRefPtr<TextLayoutProperty>();
        Clone(value);
        value->LayoutProperty::UpdateLayoutProperty(DynamicCast<LayoutProperty>(this));
        value->propSymbolSourceInfo_ = CloneSymbolSourceInfo();
        return value;
    }

    void Reset() override
    {
        LayoutProperty::Reset();
        ResetFontStyle();
        ResetTextLineStyle();
        ResetContent();
        ResetSymbolSourceInfo();
        ResetAdaptFontSizeStep();
        ResetTextMarqueeOptions();
        ResetCursorColor();
        ResetSelectedBackgroundColor();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;

    void ToJsonValueForOption(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;

    void FromJson(const std::unique_ptr<JsonValue>& json) override;

    ACE_DEFINE_PROPERTY_GROUP(FontStyle, FontStyle);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, FontSize, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, TextColor, Color, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, TextShadow, std::vector<Shadow>, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, ItalicFontStyle, Ace::FontStyle, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, FontWeight, FontWeight, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, VariableFontWeight, int32_t, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, FontFamily, std::vector<std::string>, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, FontFeature, FONT_FEATURES_LIST, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, TextDecoration, TextDecoration, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, TextDecorationColor, Color, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(
        FontStyle, TextDecorationStyle, TextDecorationStyle, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, TextCase, TextCase, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, AdaptMinFontSize, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, AdaptMaxFontSize, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, LetterSpacing, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, EnableVariableFontWeight, bool, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_GROUP(TextLineStyle, TextLineStyle);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, LineHeight, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, LineSpacing, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, TextBaseline, TextBaseline, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, BaselineOffset, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, TextAlign, TextAlign, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, TextOverflow, TextOverflow, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, EllipsisMode, EllipsisMode, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, MaxLines, uint32_t, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, AllowScale, bool, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(
        TextLineStyle, HeightAdaptivePolicy, TextHeightAdaptivePolicy, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, TextIndent, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, WordBreak, WordBreak, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(
        TextLineStyle, LineBreakStrategy, LineBreakStrategy, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(TextLineStyle, HalfLeading, bool, PROPERTY_UPDATE_MEASURE_SELF);

    ACE_DEFINE_PROPERTY_GROUP(TextMarqueeOptions, TextMarqueeOptions);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(TextMarqueeOptions, TextMarqueeStart, bool, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(TextMarqueeOptions, TextMarqueeStep, double, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(TextMarqueeOptions, TextMarqueeLoop, int32_t, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(
        TextMarqueeOptions, TextMarqueeDirection, MarqueeDirection, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(TextMarqueeOptions, TextMarqueeDelay, int32_t, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(TextMarqueeOptions, TextMarqueeFadeout, bool, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITH_GROUP(
        TextMarqueeOptions, TextMarqueeStartPolicy, MarqueeStartPolicy, PROPERTY_UPDATE_RENDER);

    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITHOUT_GROUP(Content, std::u16string, PROPERTY_UPDATE_MEASURE);
public:
    void UpdateContent(const std::string& value)
    {
        UpdateContent(UtfUtils::Str8DebugToStr16(value));
    }
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(CopyOption, CopyOptions, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITHOUT_GROUP(AdaptFontSizeStep, Dimension, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(CursorColor, Color, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(SelectedBackgroundColor, Color, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(TextSelectableMode, TextSelectableMode, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(IsAnimationNeeded, bool, PROPERTY_UPDATE_NORMAL);

    // placeholder
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITHOUT_GROUP(Placeholder, std::u16string, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_GROUP(PlaceholderFontStyle, FontStyle);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP_ITEM(
        PlaceholderFontStyle, FontSize, PlaceholderFontSize, Dimension, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP_ITEM(
        PlaceholderFontStyle, TextColor, PlaceholderTextColor, Color, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP_ITEM(
        PlaceholderFontStyle, ItalicFontStyle, PlaceholderItalicFontStyle, Ace::FontStyle, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP_ITEM(
        PlaceholderFontStyle, FontWeight, PlaceholderFontWeight, FontWeight, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP_ITEM(
        PlaceholderFontStyle, FontFamily, PlaceholderFontFamily, std::vector<std::string>, PROPERTY_UPDATE_MEASURE);

    // symbol
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITHOUT_GROUP(SymbolSourceInfo, SymbolSourceInfo, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(
        FontStyle, SymbolColorList, std::vector<Color>, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(
        FontStyle, SymbolRenderingStrategy, uint32_t, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, SymbolEffectStrategy, uint32_t, PROPERTY_UPDATE_MEASURE_SELF);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(
        FontStyle, SymbolEffectOptions, SymbolEffectOptions, PROPERTY_UPDATE_MEASURE_SELF);
    // fontscale
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, MinFontScale, float, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_TEXT_PROPERTY_ITEM_WITH_GROUP(FontStyle, MaxFontScale, float, PROPERTY_UPDATE_MEASURE);

    // for XTS inspector
    std::string InspectorGetTextFont() const
    {
        TextStyle font;
        if (GetFontFamily().has_value()) {
            font.SetFontFamilies(GetFontFamily().value());
        }
        if (GetFontSize().has_value()) {
            font.SetFontSize(GetFontSize().value());
        }
        if (GetItalicFontStyle().has_value()) {
            font.SetFontStyle(GetItalicFontStyle().value());
        }
        if (GetFontWeight().has_value()) {
            font.SetFontWeight(GetFontWeight().value());
        }
        return V2::GetTextStyleInJson(font);
    }
    std::string GetCopyOptionString() const;

    void UpdateTextColorByRender(const Color &value)
    {
        auto& groupProperty = GetOrCreateFontStyle();
        if (groupProperty->CheckTextColor(value)) {
            return;
        }
        groupProperty->UpdateTextColor(value);
        UpdatePropertyChangeFlag(PROPERTY_UPDATE_RENDER);
        propNeedReCreateParagraph_ = true;
    }

    void OnPropertyChangeMeasure() override
    {
        propNeedReCreateParagraph_ = true;
    }

    // Used to mark whether a paragraph needs to be recreated for Measure.
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP_GET(NeedReCreateParagraph, bool);

    std::string GetTextMarqueeOptionsString() const;
    void UpdateMarqueeOptionsFromJson(const std::unique_ptr<JsonValue>& json);

protected:
    void Clone(RefPtr<LayoutProperty> property) const override
    {
        auto value = DynamicCast<TextLayoutProperty>(property);
        value->LayoutProperty::UpdateLayoutProperty(DynamicCast<LayoutProperty>(this));
        value->propFontStyle_ = CloneFontStyle();
        value->propTextLineStyle_ = CloneTextLineStyle();
        value->propContent_ = CloneContent();
        value->propAdaptFontSizeStep_ = CloneAdaptFontSizeStep();
        value->propTextMarqueeOptions_ = CloneTextMarqueeOptions();
        value->propCursorColor_ = CloneCursorColor();
        value->propSelectedBackgroundColor_ = CloneSelectedBackgroundColor();
    }

private:
    ACE_DISALLOW_COPY_AND_MOVE(TextLayoutProperty);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_LAYOUT_PROPERTY_H

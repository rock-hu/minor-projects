/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_ACELITE_TEXT_COMPONENT_H
#define OHOS_ACELITE_TEXT_COMPONENT_H

#include "component.h"

#include "js_app_context.h"
#include "js_fwk_common.h"
#include "non_copyable.h"
#include "ui_label.h"
#ifdef FEATURE_EXTRA_TEXT_X_SUPPORT
#include "ui_label_x.h"
#define UI_LABEL_TYPE_WRAPPER UILabelX
#else
#define UI_LABEL_TYPE_WRAPPER UILabel
#endif // FEATURE_EXTRA_TEXT_X_SUPPORT

namespace OHOS {
namespace ACELite {
#if (defined(FEATURE_COMPONENT_TEXT_SPANNABLE) && (FEATURE_COMPONENT_TEXT_SPANNABLE == 1))
struct AbsoluteSizeSpan {
    int16_t start;
    int16_t end;
    uint8_t size;
    AbsoluteSizeSpan() : start(-1), end(-1), size(0) {}
};

struct RelativeSizeSpan {
    int16_t start;
    int16_t end;
    float size;
    RelativeSizeSpan() : start(-1), end(-1), size(0) {}
};

struct StringStyleSpan {
    int16_t start;
    int16_t end;
    TextStyle style;
    StringStyleSpan() : start(-1), end(-1), style(TextStyle::TEXT_STYLE_NORMAL) {}
};
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE

class TextComponent : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(TextComponent);
    TextComponent() = delete;
    TextComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~TextComponent() override {}

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool ApplyPrivateStyle(const AppStyleItem* styleItem) override;
    UI_LABEL_TYPE_WRAPPER *GetUILabelView() const;
    void OnViewAttached() override;
    void PostUpdate(uint16_t attrKeyId) override;

private:
    // parse js text align style to ui_label
    void SetTextAlign(UI_LABEL_TYPE_WRAPPER& label, const AppStyleItem* styleItem);
    void UpdateTextAlignToLabel(UI_LABEL_TYPE_WRAPPER& label);
    // parse js text size style to fontSize_
    void SetTextSize(const AppStyleItem* styleItem);
    // parse js text color style to ui_label
    void SetTextColor(UI_LABEL_TYPE_WRAPPER& label, const AppStyleItem* styleItem) const;
    // parse js text overflow style to ui_label
    void SetTextOverflow(UI_LABEL_TYPE_WRAPPER& label, const AppStyleItem* styleItem);
    // parse js text letter space style to ui_label
    void SetTextLetterSpace(UI_LABEL_TYPE_WRAPPER& label, const AppStyleItem* styleItem) const;
    // parse js text line height style to ui_label
    void SetTextLineHeight(UI_LABEL_TYPE_WRAPPER& label, const AppStyleItem* styleItem) const;
#if FEATURE_COMPONENT_TEXT_SPANNABLE
    void SetRichTextSpan();
#endif
    UI_LABEL_TYPE_WRAPPER uiLabel_;
    uint8_t fontSize_;
    char* fontFamily_;
    char* textValue_;
    uint8_t overflowMode_;
    UITextLanguageAlignment horizontalAlign_;
#if FEATURE_COMPONENT_TEXT_SPANNABLE
    BackgroundColor backgroundColorSpan_;
    ForegroundColor foregroundColorSpan_;
    LineBackgroundColor lineBackgroundColorSpan_;
    AbsoluteSizeSpan absoluteSizeSpan_;
    RelativeSizeSpan relativeSizeSpan_;
    StringStyleSpan stringStyleSpan_;
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TEXT_COMPONENT_H

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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_FIELD_TEXT_FIELD_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_FIELD_TEXT_FIELD_MODEL_NG_H

#include "core/components_ng/pattern/text_field/text_content_type.h"
#include "core/components_ng/pattern/text_field/text_keyboard_common_type.h"
#include "core/components_ng/pattern/text_field/text_field_model.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT TextFieldModelNG : public TextFieldModel {
public:
    TextFieldModelNG() = default;
    ~TextFieldModelNG() override = default;
    void CreateNode(
        const std::optional<std::u16string>& placeholder, const std::optional<std::u16string>& value, bool isTextArea);
    RefPtr<TextFieldControllerBase> CreateTextInput(
        const std::optional<std::u16string>& placeholder, const std::optional<std::u16string>& value) override;

    RefPtr<TextFieldControllerBase> CreateTextArea(
        const std::optional<std::u16string>& placeholder, const std::optional<std::u16string>& value) override;

    void RequestKeyboardOnFocus(bool needToRequest) override;
    void SetWidthAuto(bool isAuto) override;
    void SetType(TextInputType value) override;
    void SetContentType(const TextContentType& value) override;
    void SetPlaceholderColor(const Color& value) override;
    void ResetPlaceholderColor() override;
    void SetPlaceholderFont(const Font& value) override;
    void SetEnterKeyType(TextInputAction value) override;
    void SetCapitalizationMode(AutoCapitalizationMode value) override;
    void SetTextAlign(TextAlign value) override;
    void SetLineBreakStrategy(LineBreakStrategy value) override;
    void SetCaretColor(const Color& value) override;
    void ResetCaretColor() override;
    void SetCaretStyle(const CaretStyle& value) override;
    void SetCaretPosition(const int32_t& value) override;
    void SetSelectedBackgroundColor(const Color& value) override;
    void SetMaxLength(uint32_t value) override;
    void SetMaxLines(uint32_t value) override;
    void SetFontSize(const Dimension& value) override;
    void SetFontWeight(FontWeight value) override;
    void SetTextColor(const Color& value) override;
    void ResetTextColor() override;
    void SetWordBreak(Ace::WordBreak value) override;
    void SetFontStyle(Ace::FontStyle value) override;
    void SetFontFamily(const std::vector<std::string>& value) override;
    void SetMinFontScale(const float value) override;
    void SetMaxFontScale(const float value) override;
    void SetInputFilter(const std::string& value, const std::function<void(const std::u16string&)>&& func) override;
    void SetInputStyle(InputStyle value) override;
    void SetShowPasswordIcon(bool value) override;
    void SetShowPasswordText(bool value) override;
    void SetEllipsisMode(EllipsisMode modal) override;
    void SetOnEditChanged(std::function<void(bool)>&& func) override;
    void SetOnSubmit(std::function<void(int32_t)>&& func) override {};
    void SetOnSubmit(std::function<void(int32_t, NG::TextFieldCommonEvent&)>&& func) override;
    void SetOnWillChangeEvent(std::function<bool(const ChangeValueInfo&)>&& func) override;
    void SetOnChange(std::function<void(const ChangeValueInfo&)>&& func) override;
    void SetOnTextSelectionChange(std::function<void(int32_t, int32_t)>&& func) override;
    void SetOnSecurityStateChange(std::function<void(bool)>&& func) override;
    void SetOnContentScroll(std::function<void(float, float)>&& func) override;
    void SetOnCopy(std::function<void(const std::u16string&)>&& func) override;
    void SetOnCut(std::function<void(const std::u16string&)>&& func) override;
    void SetOnPaste(std::function<void(const std::u16string&)>&& func) override;
    void SetOnPasteWithEvent(std::function<void(const std::u16string&, NG::TextCommonEvent&)>&& func) override;
    void SetCopyOption(CopyOptions copyOption) override;
    static void ProcessDefaultStyleAndBehaviors(const RefPtr<FrameNode>& frameNode);
    void ResetMaxLength() override;
    void SetForegroundColor(const Color& value) override;
    void SetPasswordIcon(const PasswordIcon& passwordIcon) override;
    void SetShowUnit(std::function<void()>&& unitFunction) override;
    void SetShowError(const std::u16string& errorText, bool visible) override;
    void SetBarState(OHOS::Ace::DisplayMode value) override;
    void SetMaxViewLines(uint32_t value) override;
    void SetNormalMaxViewLines(uint32_t value) override;
    void SetMinLines(uint32_t value) override;
    void SetOverflowMode(OverflowMode value) override;

    void SetShowUnderline(bool showUnderLine) override;
    void SetNormalUnderlineColor(const Color& normalColor) override;
    void SetUserUnderlineColor(UserUnderlineColor userColor) override;
    void SetShowCounter(bool value) override;
    void SetCounterType(int32_t value) override;
    void SetShowCounterBorder(bool value) override;
    void SetOnChangeEvent(std::function<void(const std::u16string&)>&& func) override;
    void SetBackgroundColor(const Color& color, bool tmp) override;
    void ResetBackgroundColor() override;
    void SetHeight(const Dimension& value) override;
    void SetPadding(const NG::PaddingProperty& newPadding, Edge oldPadding, bool tmp) override;
    void SetMargin() override;
    void SetHoverEffect(HoverEffectType hoverEffect) override;
    void SetSelectionMenuHidden(bool contextMenuHidden) override;
    void SetCustomKeyboard(const std::function<void()>&& buildFunc, bool supportAvoidance = false) override;
    void SetPasswordRules(const std::string& passwordRules) override;
    void SetEnableAutoFill(bool enableAutoFill) override;
    void SetEnableAutoFillAnimation(bool enableAutoFillAnimation) override;
    void SetCleanNodeStyle(CleanNodeStyle cleanNodeStyle) override;
    void SetCancelIconSize(const CalcDimension& iconSize) override;
    void SetCanacelIconSrc(
        const std::string& iconSrc, const std::string& bundleName, const std::string& moduleName) override;
    void SetCancelIconColor(const Color& iconColor) override;
    void SetCancelSymbolIcon(const std::function<void(WeakPtr<NG::FrameNode>)>& iconSymbol) override;
    void SetCancelButtonSymbol(bool isShowSymbol) override;
    void SetIsShowCancelButton(bool isShowCancelButton) override;
    void SetSelectAllValue(bool isSetSelectAllValue) override;
    void SetFontFeature(const FONT_FEATURES_LIST& value) override;
    void SetLetterSpacing(const Dimension& value) override;
    void SetLineHeight(const Dimension& value) override;
    void SetHalfLeading(bool value) override;
    void SetLineSpacing(const Dimension& value) override;
    void SetIsOnlyBetweenLines(bool isOnlyBetweenLines) override;
    void SetAdaptMinFontSize(const Dimension& value) override;
    void SetAdaptMaxFontSize(const Dimension& value) override;
    void SetHeightAdaptivePolicy(TextHeightAdaptivePolicy value) override;
    void SetTextDecoration(Ace::TextDecoration value) override;
    void SetTextDecorationColor(const Color& value) override;
    void SetTextDecorationStyle(Ace::TextDecorationStyle value) override;
    void SetBackBorder() override;
    void SetOnWillInsertValueEvent(std::function<bool(const InsertValueInfo&)>&& func) override;
    void SetOnDidInsertValueEvent(std::function<void(const InsertValueInfo&)>&& func) override;
    void SetOnWillDeleteEvent(std::function<bool(const DeleteValueInfo&)>&& func) override;
    void SetOnDidDeleteEvent(std::function<void(const DeleteValueInfo&)>&& func) override;
    void SetSelectionMenuOptions(const NG::OnCreateMenuCallback&& onCreateMenuCallback,
        const NG::OnMenuItemClickCallback&& onMenuItemClick,
        const NG::OnPrepareMenuCallback&& onPrepareMenuCallback) override;
    void SetEnablePreviewText(bool enablePreviewText) override;
    void SetEnableHapticFeedback(bool state) override;
    void SetStopBackPress(bool isStopBackPress) override;
    void SetKeyboardAppearance(KeyboardAppearance value) override;
    void SetStrokeWidth(const Dimension& value) override;
    void SetStrokeColor(const Color& value) override;
    void ResetStrokeColor() override;
    void SetEnableAutoSpacing(bool enabled) override;
    void SetOnWillAttachIME(std::function<void(const IMEClient&)>&& func) override;

    static void SetTextDecoration(FrameNode* frameNode, const std::optional<TextDecoration>& valueOpt);
    static void SetTextDecorationColor(FrameNode* frameNode, const std::optional<Color>& valueOpt);
    static void SetTextDecorationStyle(FrameNode* frameNode, const std::optional<TextDecorationStyle>& valueOpt);
    static void SetLetterSpacing(FrameNode* frameNode, const std::optional<Dimension>& valueOpt);
    static void SetLineHeight(FrameNode* frameNode, const std::optional<Dimension>& valueOpt);
    static void SetLineSpacing(FrameNode* frameNode, const std::optional<Dimension>& valueOpt);
    static void SetHalfLeading(FrameNode* frameNode, const bool& value);
    static void SetLineSpacing(FrameNode* frameNode, const Dimension& value, bool isOnlyBetweenLines);
    static float GetLineSpacing(FrameNode* frameNode);
    void SetTextOverflow(Ace::TextOverflow value) override;
    void SetTextIndent(const Dimension& value) override;
    static void SetTextOverflow(FrameNode* frameNode, const std::optional<TextOverflow>& valueOpt);
    static void SetTextIndent(FrameNode* frameNode, const std::optional<Dimension>& valueOpt);
    static RefPtr<FrameNode> CreateFrameNode(int32_t nodeId, const std::optional<std::u16string>& placeholder,
        const std::optional<std::u16string>& value, bool isTextArea);
    static void SetAdaptMinFontSize(FrameNode* frameNode, const std::optional<Dimension>& valueOpt);
    static void SetAdaptMaxFontSize(FrameNode* frameNode, const std::optional<Dimension>& valueOpt);
    static void SetHeightAdaptivePolicy(FrameNode* frameNode, const std::optional<TextHeightAdaptivePolicy>& valueOpt);
    static void SetInputStyle(FrameNode* frameNode, const std::optional<InputStyle>& valueOpt);
    static void SetMinFontScale(FrameNode* frameNode, const float value);
    static void SetMaxFontScale(FrameNode* frameNode, const float value);
    static void SetSelectionMenuHidden(FrameNode* frameNode, bool contextMenuHidden);
    static bool GetSelectionMenuHidden(FrameNode* frameNode);
    static void SetPasswordRules(FrameNode* frameNode, const std::optional<std::string>& passwordRules);
    static void SetEnableAutoFill(FrameNode* frameNode, bool enableAutoFill);
    static void SetEnableAutoFillAnimation(FrameNode* frameNode, bool enableAutoFillAnimation);
    static void RequestKeyboardOnFocus(FrameNode* frameNode, bool needToRequest);
    static void SetBarState(FrameNode* frameNode, const std::optional<OHOS::Ace::DisplayMode>& valueOpt);
    static void SetPasswordIcon(FrameNode* frameNode, const std::optional<PasswordIcon>& passwordIcon);
    static void SetSelectedBackgroundColor(FrameNode* frameNode, const std::optional<Color>& colorOpt);
    static void SetMaxViewLines(FrameNode* frameNode, const std::optional<uint32_t>& valueOpt);
    static void SetNormalMaxViewLines(FrameNode* frameNode, uint32_t value);
    static void SetMinLines(FrameNode* frameNode, uint32_t value);
    static void SetOverflowMode(FrameNode* frameNode, OverflowMode value);
    // static void SetType(FrameNode* frameNode, TextInputType value);
    // static void SetContentType(const FrameNode* frameNode, const TextContentType& value);
    // static void SetCopyOption(FrameNode* frameNode, CopyOptions copyOption);
    static void SetType(FrameNode* frameNode, const std::optional<TextInputType>& valueOpt);
    static void SetContentType(const FrameNode* frameNode, const std::optional<TextContentType>& valueOpt);
    static void SetCopyOption(FrameNode* frameNode, const std::optional<CopyOptions>& copyOptionOpt);
    static void SetShowPasswordIcon(FrameNode* frameNode, bool value);
    static void SetShowPassword(FrameNode* frameNode, const std::optional<bool>& valueOpt);
    static void SetTextAlign(FrameNode* frameNode, const std::optional<TextAlign>& valueOpt);
    static void SetTextColor(FrameNode* frameNode, const std::optional<Color>& colorOpt);
    static void SetCaretPosition(FrameNode* frameNode, const std::optional<int32_t>& optValue);
    static void SetFontStyle(FrameNode* frameNode, const std::optional<Ace::FontStyle>& valueOpt);
    static void ResetTextColor(FrameNode* frameNode);
    static void SetMaxLength(FrameNode* frameNode, uint32_t value);
    static void ResetMaxLength(FrameNode* frameNode);
    static void SetCaretStyle(FrameNode* frameNode, const std::optional<CaretStyle>& value);
    static void SetPlaceholderColor(FrameNode* frameNode, const std::optional<Color>& colorOpt);
    static void SetFontWeight(FrameNode* frameNode, const std::optional<FontWeight>& value);
    static void SetEnterKeyType(FrameNode* frameNode, const std::optional<TextInputAction>& valueOpt);
    static void ResetPlaceholderColor(FrameNode* frameNode);
    static void SetShowUnderline(FrameNode* frameNode, bool showUnderLine);
    static void SetNormalUnderlineColor(FrameNode* frameNode, const std::optional<Color>& normalColor);
    static void SetUserUnderlineColor(FrameNode* frameNode, const std::optional<UserUnderlineColor>& userColor);
    static void SetFontFamily(FrameNode* frameNode, const std::optional<std::vector<std::string>>& fontFamilies);
    static void SetMaxLines(FrameNode* frameNode, const std::optional<uint32_t>& value);
    static void SetPlaceholderFont(FrameNode* frameNode, const std::optional<Font>& valueOpt);
    static void SetFontSize(FrameNode* frameNode, const std::optional<Dimension>& value);
    static void SetCaretColor(FrameNode* frameNode, const std::optional<Color>& colorOpt);
    static void ResetCaretColor(FrameNode* frameNode);
    static void SetShowCounter(FrameNode* frameNode, bool value);
    static void SetCounterType(FrameNode* frameNode, const std::optional<int32_t>& value);
    static void SetShowError(FrameNode* frameNode, const std::optional<std::u16string>& errorText, bool visible);
    static void SetOnWillChangeEvent(FrameNode* frameNode, std::function<bool(const ChangeValueInfo&)>&& func);
    static void SetOnChange(FrameNode* frameNode, std::function<void(const ChangeValueInfo&)>&& func);
    static void SetOnContentSizeChange(FrameNode* frameNode, std::function<void(float, float)>&& func);
    static void SetOnTextSelectionChange(FrameNode* frameNode, std::function<void(int32_t, int32_t)>&& func);
    static void SetTextFieldText(FrameNode* frameNode, const std::optional<std::u16string>& optValue);
    static void SetTextFieldPlaceHolder(FrameNode* frameNode, const std::optional<std::u16string>& placeholder);
    static void StopTextFieldEditing(FrameNode* frameNode);
    static void SetOnSubmit(FrameNode* frameNode, std::function<void(int32_t, NG::TextFieldCommonEvent&)>&& func);
    static void SetOnCut(FrameNode* frameNode, std::function<void(const std::u16string&)>&& func);
    static void SetOnPaste(FrameNode* frameNode, std::function<void(const std::u16string&)>&& func);
    static void SetOnPasteWithEvent(FrameNode* frameNode,
        std::function<void(const std::u16string&, NG::TextCommonEvent&)>&& func);
    static void SetShowUnit(FrameNode* frameNode, std::function<RefPtr<UINode>()>&& builder);
    static void SetOnChangeEvent(FrameNode* frameNode, std::function<void(const std::u16string&)>&& func);
    static void SetCleanNodeStyle(FrameNode* frameNode, const std::optional<CleanNodeStyle>& cleanNodeStyle);
    static void SetIsShowCancelButton(FrameNode* frameNode, bool isShowCancelButton);
    static void SetCancelIconSize(FrameNode* frameNode, const std::optional<CalcDimension>& iconSize);
    static void SetCanacelIconSrc(FrameNode* frameNode, const std::optional<std::string>& iconSrc);
    static void SetCancelIconColor(FrameNode* frameNode, const std::optional<Color>& iconColor);
    static void SetCancelButtonSymbol(FrameNode* frameNode, bool isShowSymbol);
    static void SetCancelSymbolIcon(FrameNode* frameNode,
        const std::function<void(WeakPtr<NG::FrameNode>)>& iconSymbol);
    static void SetBackgroundColor(FrameNode* frameNode, const std::optional<Color>& color);
    static void SetAutoCapitalizationMode(FrameNode* frameNode, AutoCapitalizationMode value);
    static void ResetBackgroundColor(FrameNode* frameNode);
    static std::u16string GetPlaceholderText(FrameNode* frameNode);
    static std::u16string GetTextFieldText(FrameNode* frameNode);
    static Color GetCaretColor(FrameNode* frameNode);
    static Dimension GetCaretStyle(FrameNode* frameNode);
    static bool GetShowUnderline(FrameNode* frameNode);
    static uint32_t GetMaxLength(FrameNode* frameNode);
    static TextInputAction GetEnterKeyType(FrameNode* frameNode);
    static Color GetPlaceholderColor(FrameNode* frameNode);
    static Font GetPlaceholderFont(FrameNode* frameNode);
    static bool GetRequestKeyboardOnFocus(FrameNode* frameNode);
    static TextInputType GetType(FrameNode* frameNode);
    static Color GetSelectedBackgroundColor(FrameNode* frameNode);
    static bool GetShowPasswordIcon(FrameNode* frameNode);
    static bool GetShowPassword(FrameNode* frameNode);
    static bool GetTextFieldEditing(FrameNode* frameNode);
    static bool GetShowCancelButton(FrameNode* frameNode);
    static CalcDimension GetCancelIconSize(FrameNode* frameNode);
    static std::string GetCanacelIconSrc(FrameNode* frameNode);
    static Color GetCancelIconColor(FrameNode* frameNode);
    static TextAlign GetTextAlign(FrameNode* frameNode);
    static Color GetTextColor(FrameNode* frameNode);
    static Ace::FontStyle GetFontStyle(FrameNode* frameNode);
    static FontWeight GetFontWeight(FrameNode* frameNode);
    static Dimension GetFontSize(FrameNode* frameNode);
    static CleanNodeStyle GetCleanNodeStyle(FrameNode* frameNode);
    static void SetShowCounterBorder(FrameNode* frameNode, const std::optional<bool>& value);
    static bool GetShowCounter(FrameNode* frameNode);
    static int GetCounterType(FrameNode* frameNode);
    static uint32_t GetMinLines(FrameNode* frameNode);
    static bool GetShowCounterBorder(FrameNode* frameNode);
    static void SetTextSelection(FrameNode* frameNode, int32_t start, int32_t end);
    static int32_t GetTextSelectionIndex(FrameNode* frameNode, bool isEnd);
    static void ResetTextInputPadding(FrameNode* frameNode);
    static void SetFontFeature(FrameNode* frameNode, const FONT_FEATURES_LIST& value);
    static void SetAutoWidth(FrameNode* frameNode);
    static void SetWidth(FrameNode* frameNode, const std::string& value);
    static void ClearWidth(FrameNode* frameNode);
    static void SetWordBreak(FrameNode* frameNode, const std::optional<Ace::WordBreak>& valueOpt);
    static void SetLineBreakStrategy(FrameNode* frameNode, const std::optional<LineBreakStrategy>& valueOpt);
    static void SetSelectAllValue(FrameNode* frameNode, bool isSelectAllValue);
    static void SetBlurOnSubmit(FrameNode* frameNode, bool blurOnSubmit);
    static bool GetBlurOnSubmit(FrameNode* frameNode);
    static void SetOnEditChange(FrameNode* frameNode, std::function<void(bool)>&& func);
    static void SetInputFilter(FrameNode* frameNode, const std::string& value,
        const std::function<void(const std::u16string&)>& onError);
    static void SetOnContentScroll(FrameNode* frameNode, std::function<void(float, float)>&& func);
    static void SetOnCopy(FrameNode* frameNode, std::function<void(const std::u16string&)>&& func);
    static void SetOnEditChanged(FrameNode* frameNode, std::function<void(bool)>&& func);
    static void SetCustomKeyboard(FrameNode* frameNode, FrameNode* customKeyboard, bool supportAvoidance = false);
    static void SetInputFilter(FrameNode* frameNode, const std::string& value);
    static void SetInputFilterError(FrameNode* frameNode, const std::function<void(const std::u16string&)>& onError);
    static Ace::WordBreak GetWordBreak(FrameNode* frameNode);
    static bool GetEnableAutoFill(FrameNode* frameNode);
    static bool GetEnableAutoFillAnimation(FrameNode* frameNode);
    static TextContentType GetContentType(FrameNode* frameNode);
    static UserUnderlineColor GetUnderLineColor(FrameNode* frameNode);
    static std::string GetPasswordRules(FrameNode* frameNode);
    static bool GetSelectAllValue(FrameNode* frameNode);
    static std::string GetInputFilter(FrameNode* frameNode);
    static InputStyle GetInputStyle(FrameNode* frameNode);
    static RefPtr<TextFieldControllerBase> GetOrCreateController(FrameNode* frameNode);
    static FONT_FEATURES_LIST GetFontFeature(FrameNode* frameNode);
    static Dimension GetAdaptMinFontSize(FrameNode* frameNode);
    static Dimension GetAdaptMaxFontSize(FrameNode* frameNode);
    static Dimension GetLineHeight(FrameNode* frameNode);
    static uint32_t GetMaxLines(FrameNode* frameNode);
    static void SetPadding(FrameNode* frameNode, NG::PaddingProperty& newPadding);
    static RefPtr<UINode> GetCustomKeyboard(FrameNode* frameNode);
    static bool GetCustomKeyboardOption(FrameNode* frameNode);
    static void SetShowKeyBoardOnFocus(FrameNode* frameNode, bool value);
    static bool GetShowKeyBoardOnFocus(FrameNode* frameNode);
    static void SetNumberOfLines(FrameNode* frameNode, int32_t value);
    static int32_t GetNumberOfLines(FrameNode* frameNode);
    static void ResetNumberOfLines(FrameNode* frameNode);
    static void SetBorderWidth(FrameNode* frameNode, NG::BorderWidthProperty borderWidth);
    static void SetBorderRadius(FrameNode* frameNode, NG::BorderRadiusProperty borderRadius);
    static void SetBorderColor(FrameNode* frameNode, NG::BorderColorProperty borderColors);
    static void SetBorderStyle(FrameNode* frameNode, NG::BorderStyleProperty borderStyles);
    static void SetMargin(FrameNode* frameNode, NG::PaddingProperty& margin);
    static PaddingProperty GetMargin(FrameNode* frameNode);
    static void SetOnWillInsertValueEvent(FrameNode* frameNode, std::function<bool(const InsertValueInfo&)>&& func);
    static void SetOnDidInsertValueEvent(FrameNode* frameNode, std::function<void(const InsertValueInfo&)>&& func);
    static void SetOnWillDeleteEvent(FrameNode* frameNode, std::function<bool(const DeleteValueInfo&)>&& func);
    static void SetOnDidDeleteEvent(FrameNode* frameNode, std::function<void(const DeleteValueInfo&)>&& func);
    static void SetEnablePreviewText(FrameNode* frameNode, bool enablePreviewText);
    static PaddingProperty GetPadding(FrameNode* frameNode);
    static void OnCreateMenuCallbackUpdate(FrameNode* frameNode, const NG::OnCreateMenuCallback&& onCreateMenuCallback);
    static void OnMenuItemClickCallbackUpdate(
        FrameNode* frameNode, const NG::OnMenuItemClickCallback&& onMenuItemClick);
    static void OnPrepareMenuCallbackUpdate(
        FrameNode* frameNode, const NG::OnPrepareMenuCallback&& onPrepareMenuCallback);
    static RefPtr<TextFieldControllerBase> GetController(FrameNode* frameNode,
        const std::optional<std::string>& placeholder, const std::optional<std::string>& value);
    static void SetJSTextEditableController(FrameNode* frameNode, const RefPtr<Referenced>& controller);
    static RefPtr<Referenced> GetJSTextEditableController(FrameNode* frameNode);
    static void SetEnableHapticFeedback(FrameNode* frameNode, bool state);
    static Dimension GetLetterSpacing(FrameNode* frameNode);
    static bool GetEnablePreviewText(FrameNode* frameNode);
    static void SetEllipsisMode(FrameNode* frameNode, EllipsisMode modal);
    static void SetStopBackPress(FrameNode* frameNode, bool isStopBackPress);
    static void SetKeyboardAppearance(FrameNode* frameNode, KeyboardAppearance value);
    static int32_t GetKeyboardAppearance(FrameNode* frameNode);
    static Dimension GetStrokeWidth(FrameNode* frameNode);
    static Color GetStrokeColor(FrameNode* frameNode);
    static void SetStrokeWidth(FrameNode* frameNode, const Dimension& value);
    static void SetStrokeColor(FrameNode* frameNode, const Color& value);
    static void ResetStrokeColor(FrameNode* frameNode);
    static void SetEnableAutoSpacing(FrameNode* frameNode, bool enabled);
    static bool GetEnableAutoSpacing(FrameNode* frameNode);
    static void SetOnSecurityStateChange(FrameNode* frameNode, std::function<void(bool)>&& func);
    static void SetKeyboardAppearanceConfig(FrameNode* frameNode, KeyboardAppearanceConfig config);

private:
    void AddDragFrameNodeToManager() const;
    void SetDraggable(bool draggable);
    void SetTextRectWillChange();
    void SetDefaultPadding();
    void SetBackBorderRadius();
};

} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_FIELD_TEXT_FIELD_MODEL_NG_H

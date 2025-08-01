/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/implementation/text_input_controller_peer.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/converter_union.h"
#include "core/interfaces/native/utility/validators.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "arkoala_api_generated.h"
#include "core/components_ng/pattern/text_field/text_field_model_ng.h"
#include "core/components_ng/pattern/text_field/text_field_model_static.h"
#include "base/utils/utils.h"
#include "core/common/container.h"
#include "core/components/common/properties/text_style_parser.h"
#include "core/image/image_source_info.h"
#include "core/interfaces/native/utility/callback_helper.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t MIN_THRESHOLD_PERCENTAGE = 1;
constexpr int32_t MAX_THRESHOLD_PERCENTAGE = 100;
constexpr float SCALE_LIMIT = 1.f;

struct InputCounterOptions {
    std::optional<int> thresholdPercentage;
    std::optional<bool> highlightBorder;
};
} // namespace

namespace Converter {
template<>
InputCounterOptions Convert(const Ark_InputCounterOptions& src)
{
    InputCounterOptions options;
    options.thresholdPercentage = Converter::OptConvert<int>(src.thresholdPercentage);
    options.highlightBorder = Converter::OptConvert<bool>(src.highlightBorder);
    return options;
}

template<>
PasswordIcon Convert(const Ark_PasswordIcon& src)
{
    PasswordIcon result {};
    Converter::VisitUnion(
        src.onIconSrc, [&result](const Ark_String& src) { result.showResult = Converter::Convert<std::string>(src); },
        [&result](const Ark_Resource& src) {
            result.showBundleName = Converter::Convert<std::string>(src.bundleName);
            result.showModuleName = Converter::Convert<std::string>(src.moduleName);
            auto resStr = Converter::OptConvert<std::string>(src);
            result.showResult = (resStr.has_value()) ? resStr.value() : "";
        },
        []() {});
    Converter::VisitUnion(
        src.offIconSrc, [&result](const Ark_String& src) { result.hideResult = Converter::Convert<std::string>(src); },
        [&result](const Ark_Resource& src) {
            result.hideBundleName = Converter::Convert<std::string>(src.bundleName);
            result.hideModuleName = Converter::Convert<std::string>(src.moduleName);
            auto resStr = Converter::OptConvert<std::string>(src);
            result.hideResult = (resStr.has_value()) ? resStr.value() : "";
        },
        []() {});
    return result;
}
} // namespace Converter
} // namespace OHOS::Ace::NG

namespace OHOS::Ace::NG::GeneratedModifier {
namespace TextInputModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = TextFieldModelNG::CreateTextInputNode(id, u"", u"");
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // TextInputModifier
namespace TextInputInterfaceModifier {
void SetTextInputOptionsImpl(Ark_NativePointer node,
                             const Opt_TextInputOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    std::optional<std::u16string> placeholder;
    std::optional<std::u16string> text;
    TextInputControllerPeer* peerPtr = nullptr;
    auto textInputOptions = Converter::OptConvert<Ark_TextInputOptions>(*value);
    if (textInputOptions) {
        placeholder = Converter::OptConvert<std::u16string>(textInputOptions.value().placeholder);
        text = Converter::OptConvert<std::u16string>(textInputOptions.value().text);
        auto controller = Converter::OptConvert<Ark_TextInputController>(textInputOptions.value().controller);
        if (controller.has_value()) {
            peerPtr = controller.value();
        }
    }
    auto controller = TextFieldModelStatic::GetController(frameNode, placeholder, text);
    if (peerPtr) {
        peerPtr->SetController(controller);
    }
}
} // TextInputInterfaceModifier
namespace TextInputAttributeModifier {
void TypeImpl(Ark_NativePointer node,
              const Opt_InputType* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<TextInputType>(*value);
    TextFieldModelStatic::SetType(frameNode, convValue);
}
void ContentTypeImpl(Ark_NativePointer node,
                     const Opt_ContentType* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<TextContentType>(*value);
    TextFieldModelStatic::SetContentType(frameNode, convValue);
}
void PlaceholderColorImpl(Ark_NativePointer node,
                          const Opt_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<Color>(*value);
    TextFieldModelStatic::SetPlaceholderColor(frameNode, convValue);
}
void TextOverflowImpl(Ark_NativePointer node,
                      const Opt_TextOverflow* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<TextOverflow>(*value);
    TextFieldModelStatic::SetTextOverflow(frameNode, convValue);
}
void TextIndentImpl(Ark_NativePointer node,
                    const Opt_Length* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<Dimension>(*value);
    TextFieldModelStatic::SetTextIndent(frameNode, convValue);
}
void PlaceholderFontImpl(Ark_NativePointer node,
                         const Opt_Font* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = value ? Converter::OptConvert<Font>(*value) : std::nullopt;
    TextFieldModelStatic::SetPlaceholderFont(frameNode, convValue);
}
void EnterKeyTypeImpl(Ark_NativePointer node,
                      const Opt_EnterKeyType* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<TextInputAction>(*value);
    TextFieldModelStatic::SetEnterKeyType(frameNode, convValue);
}
void CaretColorImpl(Ark_NativePointer node,
                    const Opt_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<Color>(*value);
    TextFieldModelStatic::SetCaretColor(frameNode, convValue);
}
void OnEditChangedImpl(Ark_NativePointer node,
                       const Opt_Callback_Boolean_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onEditChanged = [arkCallback = CallbackHelper(*optValue)](const bool& boolValue) {
        arkCallback.Invoke(Converter::ArkValue<Ark_Boolean>(boolValue));
    };
    TextFieldModelNG::SetOnEditChanged(frameNode, onEditChanged);
}
void OnEditChangeImpl(Ark_NativePointer node,
                      const Opt_Callback_Boolean_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onEditChange = [arkCallback = CallbackHelper(*optValue)](const bool& boolValue) {
        arkCallback.Invoke(Converter::ArkValue<Ark_Boolean>(boolValue));
    };
    TextFieldModelNG::SetOnEditChange(frameNode, onEditChange);
}
void OnSubmitImpl(Ark_NativePointer node,
                  const Opt_OnSubmitCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onSubmit = [arkCallback = CallbackHelper(*optValue)](
                        const int32_t& value, NG::TextFieldCommonEvent& info) {
        auto enterKeyType = Converter::ArkValue<Ark_EnterKeyType>(static_cast<TextInputAction>(value));
        const auto event = Converter::ArkSubmitEventSync(info);
        arkCallback.InvokeSync(enterKeyType, event.ArkValue());
    };
    TextFieldModelNG::SetOnSubmit(frameNode, onSubmit);
}
void OnChangeImpl(Ark_NativePointer node,
                  const Opt_EditableTextOnChangeCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        TextFieldModelNG::SetOnChange(frameNode, nullptr);
        return;
    }
    auto onChange = [arkCallback = CallbackHelper(*optValue)](const ChangeValueInfo& info) {
        Converter::ConvContext ctx;
        auto textArkString = Converter::ArkValue<Ark_String>(info.value, &ctx);
        auto textArkPrevText = Converter::ArkValue<Opt_PreviewText>(info.previewText, &ctx);
        auto options = Converter::ArkValue<Opt_TextChangeOptions>();
        options.tag = INTEROP_TAG_OBJECT;
        options.value.rangeBefore = Converter::ArkValue<Ark_TextRange>(info.rangeBefore);
        options.value.rangeAfter = Converter::ArkValue<Ark_TextRange>(info.rangeAfter);
        options.value.oldContent = Converter::ArkValue<Ark_String>(info.oldContent, &ctx);
        options.value.oldPreviewText = Converter::ArkValue<Ark_PreviewText>(info.oldPreviewText, &ctx);
        arkCallback.InvokeSync(textArkString, textArkPrevText, options);
    };
    TextFieldModelNG::SetOnChange(frameNode, onChange);
}
void OnTextSelectionChangeImpl(Ark_NativePointer node,
                               const Opt_OnTextSelectionChangeCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onTextSelectionChange = [arkCallback = CallbackHelper(*optValue)](const int32_t& start, const int32_t& end) {
        arkCallback.Invoke(Converter::ArkValue<Ark_Number>(start), Converter::ArkValue<Ark_Number>(end));
    };
    TextFieldModelNG::SetOnTextSelectionChange(frameNode, onTextSelectionChange);
}
void OnContentScrollImpl(Ark_NativePointer node,
                         const Opt_OnContentScrollCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onContentScroll = [arkCallback = CallbackHelper(*optValue)](const float& offsetX, const float& offsetY) {
        arkCallback.Invoke(Converter::ArkValue<Ark_Number>(offsetX), Converter::ArkValue<Ark_Number>(offsetY));
    };
    TextFieldModelNG::SetOnContentScroll(frameNode, onContentScroll);
}
void MaxLengthImpl(Ark_NativePointer node,
                   const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto maxLength = Converter::OptConvert<int>(*value);
    Validator::ValidateNonNegative(maxLength);
    if (maxLength) {
        TextFieldModelNG::SetMaxLength(frameNode, *maxLength);
    } else {
        TextFieldModelNG::ResetMaxLength(frameNode);
    }
}
void FontColorImpl(Ark_NativePointer node,
                   const Opt_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<Color>(*value);
    TextFieldModelStatic::SetTextColor(frameNode, convValue);
}
void FontSizeImpl(Ark_NativePointer node,
                  const Opt_Length* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto fontSize = Converter::OptConvert<Dimension>(*value);
    Validator::ValidateNonNegative(fontSize);
    Validator::ValidateNonPercent(fontSize);
    TextFieldModelStatic::SetFontSize(frameNode, fontSize);
}
void FontStyleImpl(Ark_NativePointer node,
                   const Opt_FontStyle* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<Ace::FontStyle>(*value);
    TextFieldModelStatic::SetFontStyle(frameNode, convValue);
}
void FontWeightImpl(Ark_NativePointer node,
                    const Opt_Union_Number_FontWeight_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<FontWeight>(*value);
    TextFieldModelStatic::SetFontWeight(frameNode, convValue);
}
void FontFamilyImpl(Ark_NativePointer node,
                    const Opt_ResourceStr* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<StringArray> families;
    if (auto fontfamiliesOpt = Converter::OptConvert<Converter::FontFamilies>(*value); fontfamiliesOpt) {
        families = fontfamiliesOpt->families;
    }
    TextFieldModelStatic::SetFontFamily(frameNode, families);
}
void OnCopyImpl(Ark_NativePointer node,
                const Opt_Callback_String_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onCopy = [arkCallback = CallbackHelper(*optValue)](const std::u16string& copyStr) {
        Converter::ConvContext ctx;
        arkCallback.Invoke(Converter::ArkValue<Ark_String>(copyStr, &ctx));
    };
    TextFieldModelNG::SetOnCopy(frameNode, onCopy);
}
void OnCutImpl(Ark_NativePointer node,
               const Opt_Callback_String_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onCut = [arkCallback = CallbackHelper(*optValue)](const std::u16string& cutStr) {
        Converter::ConvContext ctx;
        arkCallback.Invoke(Converter::ArkValue<Ark_String>(cutStr, &ctx));
    };
    TextFieldModelNG::SetOnCut(frameNode, onCut);
}
void OnPasteImpl(Ark_NativePointer node,
                 const Opt_OnPasteCallback* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onPaste = [arkCallback = CallbackHelper(*optValue)](const std::u16string& content,
        NG::TextCommonEvent& event) -> void {
        Converter::ConvContext ctx;
        auto arkContent = Converter::ArkValue<Ark_String>(content, &ctx);
        auto keeper = CallbackKeeper::Claim([&event]() {
            event.SetPreventDefault(true);
        });
        Ark_PasteEvent arkEvent = {
            .preventDefault = Converter::ArkValue<Opt_Callback_Void>(keeper.ArkValue())
        };
        arkCallback.Invoke(arkContent, arkEvent);
    };
    TextFieldModelNG::SetOnPasteWithEvent(frameNode, std::move(onPaste));
}
void CopyOptionImpl(Ark_NativePointer node,
                    const Opt_CopyOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<CopyOptions>(*value);
    TextFieldModelStatic::SetCopyOption(frameNode, convValue);
}
void ShowPasswordIconImpl(Ark_NativePointer node,
                          const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetShowPasswordIcon(frameNode, convValue);
}
void TextAlignImpl(Ark_NativePointer node,
                   const Opt_TextAlign* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<TextAlign>(*value);
    TextFieldModelStatic::SetTextAlign(frameNode, convValue);
}
void StyleImpl(Ark_NativePointer node,
               const Opt_Union_TextInputStyle_TextContentStyle* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<InputStyle>(*value);
    TextFieldModelStatic::SetInputStyle(frameNode, convValue);
}
void CaretStyleImpl(Ark_NativePointer node,
                    const Opt_CaretStyle* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto caretStyle = Converter::OptConvert<Converter::CaretStyle>(*value).value_or(Converter::CaretStyle{});
    TextFieldModelStatic::SetCaretColor(frameNode, caretStyle.color);
    Validator::ValidateNonNegative(caretStyle.width);
    Validator::ValidateNonPercent(caretStyle.width);
    CaretStyle caretStyleNative;
    caretStyleNative.caretWidth = caretStyle.width;
    TextFieldModelNG::SetCaretStyle(frameNode, caretStyleNative);
}
void SelectedBackgroundColorImpl(Ark_NativePointer node,
                                 const Opt_ResourceColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    TextFieldModelStatic::SetSelectedBackgroundColor(frameNode, Converter::OptConvert<Color>(*value));
}
void CaretPositionImpl(Ark_NativePointer node,
                       const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<int>(*value);
    Validator::ValidateNonNegative(convValue);
    TextFieldModelStatic::SetCaretPosition(frameNode, convValue);
}
void EnableKeyboardOnFocusImpl(Ark_NativePointer node,
                               const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::RequestKeyboardOnFocus(frameNode, convValue);
}
void PasswordIconImpl(Ark_NativePointer node,
                      const Opt_PasswordIcon* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<PasswordIcon>(*value);
    TextFieldModelStatic::SetPasswordIcon(frameNode, convValue);
}
void ShowErrorImpl(Ark_NativePointer node,
                   const Opt_ResourceStr* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convTextValue = Converter::OptConvert<std::u16string>(*value);
    auto convBoolValue = convTextValue.has_value() && !convTextValue->empty();
    TextFieldModelStatic::SetShowError(frameNode, convTextValue, convBoolValue);
}
void ShowUnitImpl(Ark_NativePointer node,
                  const Opt_CustomNodeBuilder* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        TextFieldModelStatic::SetShowUnit(frameNode, nullptr);
        return;
    }
    CallbackHelper(*optValue).BuildAsync([frameNode](const RefPtr<UINode>& uiNode) {
        auto builder = [uiNode]() -> RefPtr<UINode> {
            return uiNode;
        };
        TextFieldModelStatic::SetShowUnit(frameNode, std::move(builder));
        }, node);
}
void ShowUnderlineImpl(Ark_NativePointer node,
                       const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetShowUnderline(frameNode, convValue);
}
void UnderlineColorImpl(Ark_NativePointer node,
                        const Opt_Union_ResourceColor_UnderlineColor* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<Color> resColor;
    std::optional<UserUnderlineColor> userColor;
    Converter::VisitUnion(
        *value,
        [&resColor](const Ark_ResourceColor& src) { resColor = Converter::OptConvert<Color>(src); },
        [&userColor](const Ark_UnderlineColor& src) { userColor = Converter::OptConvert<UserUnderlineColor>(src); },
        [](const Ark_Undefined& src) {},
        []() {});

    if (resColor.has_value()) {
        auto colorValue = resColor.value();
        userColor = UserUnderlineColor();
        userColor->disable = colorValue;
        userColor->error = colorValue;
        userColor->normal = colorValue;
        userColor->typing = colorValue;
    }
    TextFieldModelStatic::SetUserUnderlineColor(frameNode, userColor);
}
void SelectionMenuHiddenImpl(Ark_NativePointer node,
                             const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetSelectionMenuHidden(frameNode, convValue);
}
void BarStateImpl(Ark_NativePointer node,
                  const Opt_BarState* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<DisplayMode>(*value);
    TextFieldModelStatic::SetBarState(frameNode, convValue);
}
void MaxLinesImpl(Ark_NativePointer node,
                  const Opt_Number* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<int>(*value);
    TextFieldModelStatic::SetMaxViewLines(frameNode, convValue);
}
void WordBreakImpl(Ark_NativePointer node,
                   const Opt_WordBreak* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<WordBreak>(*value);
    TextFieldModelStatic::SetWordBreak(frameNode, convValue);
}
void LineBreakStrategyImpl(Ark_NativePointer node,
                           const Opt_LineBreakStrategy* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<LineBreakStrategy>(*value);
    TextFieldModelStatic::SetLineBreakStrategy(frameNode, convValue);
}
void CancelButton0Impl(Ark_NativePointer node,
                       const Opt_CancelButtonOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    auto cleanButtonStyle = optValue ? Converter::OptConvert<CleanNodeStyle>(optValue->style) : std::nullopt;
    auto optIconOptions = optValue ? Converter::OptConvert<Ark_IconOptions>(optValue->icon) : std::nullopt;
    TextFieldModelStatic::SetCleanNodeStyle(frameNode, cleanButtonStyle);
    TextFieldModelNG::SetIsShowCancelButton(frameNode, true);
    TextFieldModelNG::SetCancelButtonSymbol(frameNode, false);
    if (!optIconOptions) {
        TextFieldModelStatic::SetDefaultCancelIcon(frameNode);
        return;
    }
    // set icon size
    auto iconSize = Converter::OptConvert<CalcDimension>(optIconOptions->size);
    Validator::ValidateNonNegative(iconSize);
    Validator::ValidateNonPercent(iconSize);
    TextFieldModelStatic::SetCancelIconSize(frameNode, iconSize);
    // set icon src
    auto iconSrcOpt = Converter::OptConvert<Converter::Ark_Resource_Simple>(optIconOptions->src);
    if (!iconSrcOpt) {
        iconSrcOpt = Converter::Ark_Resource_Simple();
    }
    TextFieldModelStatic::SetCanacelIconSrc(frameNode, iconSrcOpt->content, iconSrcOpt->bundleName,
        iconSrcOpt->moduleName);
    // set icon color
    auto info = ImageSourceInfo(iconSrcOpt->content, iconSrcOpt->bundleName, iconSrcOpt->moduleName);
    if (info.IsSvg() && iconSrcOpt->content != "") {
        // svg need not default color, otherwise multi color svg will render fault
        return;
    }
    std::optional<Color> iconColor;
    if (Container::CurrentColorMode() != ColorMode::DARK) {
        iconColor = Converter::OptConvert<Color>(optIconOptions->color);
    }
    TextFieldModelStatic::SetCancelIconColor(frameNode, iconColor);
}
void CancelButton1Impl(Ark_NativePointer node,
                       const Opt_CancelButtonSymbolOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    auto cleanButtonStyle = optValue ? Converter::OptConvert<CleanNodeStyle>(optValue->style) : std::nullopt;
    auto symbol = optValue ? Converter::OptConvert<Ark_SymbolGlyphModifier>(optValue->icon) : std::nullopt;
    TextFieldModelStatic::SetCleanNodeStyle(frameNode, cleanButtonStyle);
    TextFieldModelNG::SetIsShowCancelButton(frameNode, true);
    TextFieldModelNG::SetCancelButtonSymbol(frameNode, true);
    if (symbol) {
        // TODO: Reset value
        TextFieldModelNG::SetCancelSymbolIcon(frameNode, nullptr);
        LOGE("TextInputModifier::CancelButton1Impl need to know what data is in value->icon");
    }
}
void SelectAllImpl(Ark_NativePointer node,
                   const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetSelectAllValue(frameNode, convValue);
}
void MinFontSizeImpl(Ark_NativePointer node,
                     const Opt_Union_Number_String_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<Dimension> convValue = std::nullopt;
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        convValue = Converter::OptConvertFromArkNumStrRes(value->value);
    }
    Validator::ValidateNonNegative(convValue);
    Validator::ValidateNonPercent(convValue);
    TextFieldModelStatic::SetAdaptMinFontSize(frameNode, convValue);
}
void MaxFontSizeImpl(Ark_NativePointer node,
                     const Opt_Union_Number_String_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<Dimension> convValue = std::nullopt;
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        convValue = Converter::OptConvertFromArkNumStrRes(value->value);
    }
    Validator::ValidateNonNegative(convValue);
    Validator::ValidateNonPercent(convValue);
    TextFieldModelStatic::SetAdaptMaxFontSize(frameNode, convValue);
}
void MinFontScaleImpl(Ark_NativePointer node,
                      const Opt_Union_Number_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<float>(*value);
    Validator::ValidatePositive(convValue);
    Validator::ValidateLessOrEqual(convValue, SCALE_LIMIT);
    TextFieldModelStatic::SetMinFontScale(frameNode, convValue);
}
void MaxFontScaleImpl(Ark_NativePointer node,
                      const Opt_Union_Number_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<float>(*value);
    Validator::ValidatePositive(convValue);
    Validator::ValidateGreatOrEqual(convValue, SCALE_LIMIT);
    TextFieldModelStatic::SetMaxFontScale(frameNode, convValue);
}
void HeightAdaptivePolicyImpl(Ark_NativePointer node,
                              const Opt_TextHeightAdaptivePolicy* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<TextHeightAdaptivePolicy>(*value);
    TextFieldModelStatic::SetHeightAdaptivePolicy(frameNode, convValue);
}
void EnableAutoFillImpl(Ark_NativePointer node,
                        const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetEnableAutoFill(frameNode, convValue);
}
void DecorationImpl(Ark_NativePointer node,
                    const Opt_TextDecorationOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto options = Converter::OptConvert<Converter::TextDecorationOptions>(*value)
        .value_or(Converter::TextDecorationOptions{});
    TextFieldModelStatic::SetTextDecoration(frameNode, options.textDecoration);
    TextFieldModelStatic::SetTextDecorationColor(frameNode, options.color);
    TextFieldModelStatic::SetTextDecorationStyle(frameNode, options.textDecorationStyle);
}
void LetterSpacingImpl(Ark_NativePointer node,
                       const Opt_Union_Number_String_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<Dimension> spacing = std::nullopt;
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        spacing = Converter::OptConvertFromArkNumStrRes(value->value);
    }
    Validator::ValidateNonPercent(spacing);
    TextFieldModelStatic::SetLetterSpacing(frameNode, spacing);
}
void LineHeightImpl(Ark_NativePointer node,
                    const Opt_Union_Number_String_Resource* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<Dimension> optValue = std::nullopt;
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        optValue = Converter::OptConvertFromArkNumStrRes(value->value);
    }
    Validator::ValidateNonNegative(optValue);
    TextFieldModelStatic::SetLineHeight(frameNode, optValue);
}
void PasswordRulesImpl(Ark_NativePointer node,
                       const Opt_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<std::string>(*value);
    TextFieldModelStatic::SetPasswordRules(frameNode, convValue);
}
void FontFeatureImpl(Ark_NativePointer node,
                     const Opt_String* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<std::string>(*value);
    if (!convValue) {
        return;
    }
    TextFieldModelNG::SetFontFeature(frameNode, ParseFontFeatureSettings(*convValue));
}
void ShowPasswordImpl(Ark_NativePointer node,
                      const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetShowPassword(frameNode, convValue);
}
void OnSecurityStateChangeImpl(Ark_NativePointer node,
                               const Opt_Callback_Boolean_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onSecurityStateChange = [arkCallback = CallbackHelper(*optValue)](const bool& boolValue) {
        arkCallback.Invoke(Converter::ArkValue<Ark_Boolean>(boolValue));
    };
    TextFieldModelStatic::SetOnSecurityStateChange(frameNode, onSecurityStateChange);
}
void OnWillInsertImpl(Ark_NativePointer node,
                      const Opt_Callback_InsertValue_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onWillInsert = [callback = CallbackHelper(*optValue)](const InsertValueInfo& value) -> bool {
        Converter::ConvContext ctx;
        Ark_InsertValue insertValue = {
            .insertOffset = Converter::ArkValue<Ark_Number>(value.insertOffset),
            .insertValue = Converter::ArkValue<Ark_String>(value.insertValue, &ctx)
        };
        return callback.InvokeWithOptConvertResult<bool, Ark_Boolean, Callback_Boolean_Void>(insertValue)
            .value_or(true);
    };
    TextFieldModelNG::SetOnWillInsertValueEvent(frameNode, std::move(onWillInsert));
}
void OnDidInsertImpl(Ark_NativePointer node,
                     const Opt_Callback_InsertValue_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onDidInsert = [arkCallback = CallbackHelper(*optValue)](const InsertValueInfo& insertValueInfo) {
        Converter::ConvContext ctx;
        arkCallback.Invoke(Ark_InsertValue {
                .insertOffset = Converter::ArkValue<Ark_Number>(insertValueInfo.insertOffset),
                .insertValue = Converter::ArkValue<Ark_String>(insertValueInfo.insertValue, &ctx)
        });
    };
    TextFieldModelNG::SetOnDidInsertValueEvent(frameNode, onDidInsert);
}
void OnWillDeleteImpl(Ark_NativePointer node,
                      const Opt_Callback_DeleteValue_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onWillDelete = [callback = CallbackHelper(*optValue)](const DeleteValueInfo& value) -> bool {
        Converter::ConvContext ctx;
        Ark_DeleteValue deleteValue = {
            .deleteOffset = Converter::ArkValue<Ark_Number>(value.deleteOffset),
            .direction = Converter::ArkValue<Ark_TextDeleteDirection>(value.direction),
            .deleteValue = Converter::ArkValue<Ark_String>(value.deleteValue, &ctx)
        };
        return callback.InvokeWithOptConvertResult<bool, Ark_Boolean, Callback_Boolean_Void>(deleteValue)
            .value_or(true);
    };
    TextFieldModelNG::SetOnWillDeleteEvent(frameNode, std::move(onWillDelete));
}
void OnDidDeleteImpl(Ark_NativePointer node,
                     const Opt_Callback_DeleteValue_Void* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onDidDelete = [arkCallback = CallbackHelper(*optValue)](const DeleteValueInfo& deleteValueInfo) {
        Converter::ConvContext ctx;
        arkCallback.Invoke(Ark_DeleteValue {
                .deleteOffset = Converter::ArkValue<Ark_Number>(deleteValueInfo.deleteOffset),
                .direction = Converter::ArkValue<Ark_TextDeleteDirection>(deleteValueInfo.direction),
                .deleteValue = Converter::ArkValue<Ark_String>(deleteValueInfo.deleteValue, &ctx)
        });
    };
    TextFieldModelNG::SetOnDidDeleteEvent(frameNode, onDidDelete);
}
void EditMenuOptionsImpl(Ark_NativePointer node,
                         const Opt_EditMenuOptions* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    auto onCreateMenuCallback = [arkCreateMenu = CallbackHelper(optValue->onCreateMenu)](
        const std::vector<NG::MenuItemParam>& systemMenuItems) -> std::vector<NG::MenuOptionsParam> {
            auto menuItems = Converter::ArkValue<Array_TextMenuItem>(systemMenuItems, Converter::FC);
            auto arkResult = arkCreateMenu.InvokeWithObtainResult<
                Array_TextMenuItem, Callback_Array_TextMenuItem_Void>(menuItems);
            return Converter::Convert<std::vector<NG::MenuOptionsParam>>(arkResult);
        };
    auto onMenuItemClick = [arkMenuItemClick = CallbackHelper(optValue->onMenuItemClick)](
        NG::MenuItemParam menuOptionsParam) -> bool {
            TextRange range {.start = menuOptionsParam.start, .end = menuOptionsParam.end};
            auto menuItem = Converter::ArkValue<Ark_TextMenuItem>(menuOptionsParam);
            auto arkRange = Converter::ArkValue<Ark_TextRange>(range);
            auto arkResult = arkMenuItemClick.InvokeWithObtainResult<
                Ark_Boolean, Callback_Boolean_Void>(menuItem, arkRange);
            return Converter::Convert<bool>(arkResult);
        };
    TextFieldModelStatic::SetSelectionMenuOptions(frameNode, std::move(onCreateMenuCallback),
        std::move(onMenuItemClick));
}
void EnablePreviewTextImpl(Ark_NativePointer node,
                           const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetEnablePreviewText(frameNode, convValue);
}
void EnableHapticFeedbackImpl(Ark_NativePointer node,
                              const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    TextFieldModelStatic::SetEnableHapticFeedback(frameNode, convValue);
}
void AutoCapitalizationModeImpl(Ark_NativePointer node,
                                const Opt_AutoCapitalizationMode* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
}
void HalfLeadingImpl(Ark_NativePointer node,
                     const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    TextFieldModelStatic::SetHalfLeading(frameNode, value ? Converter::OptConvert<bool>(*value) : std::nullopt);
}
void EllipsisModeImpl(Ark_NativePointer node,
                      const Opt_EllipsisMode* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = value ? Converter::OptConvert<EllipsisMode>(*value) : std::nullopt;
    TextFieldModelStatic::SetEllipsisMode(frameNode, convValue);
}
void StopBackPressImpl(Ark_NativePointer node,
                       const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    TextFieldModelStatic::SetStopBackPress(frameNode, value ? Converter::OptConvert<bool>(*value) : std::nullopt);
}
void OnWillChangeImpl(Ark_NativePointer node,
                      const Opt_Callback_EditableTextChangeValue_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        return;
    }
    auto onWillChange = [callback = CallbackHelper(*optValue)](const ChangeValueInfo& value) -> bool {
        Converter::ConvContext ctx;
        Ark_EditableTextChangeValue changeValue = {
            .content = Converter::ArkValue<Ark_String>(value.value, &ctx),
            .previewText = {
                .tag = INTEROP_TAG_OBJECT,
                .value = Converter::ArkValue<Ark_PreviewText>(value.previewText),
            },
            .options = {
                .tag = INTEROP_TAG_OBJECT,
                .value.rangeBefore = Converter::ArkValue<Ark_TextRange>(value.rangeBefore),
                .value.rangeAfter = Converter::ArkValue<Ark_TextRange>(value.rangeAfter),
                .value.oldContent = Converter::ArkValue<Ark_String>(value.oldContent, &ctx),
                .value.oldPreviewText = Converter::ArkValue<Ark_PreviewText>(value.oldPreviewText, &ctx),
            }
        };
        return callback.InvokeWithOptConvertResult<bool, Ark_Boolean, Callback_Boolean_Void>(changeValue)
            .value_or(true);
    };
    TextFieldModelNG::SetOnWillChangeEvent(frameNode, std::move(onWillChange));
}
void KeyboardAppearanceImpl(Ark_NativePointer node,
                            const Opt_KeyboardAppearance* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = value ? Converter::OptConvert<KeyboardAppearance>(*value) : std::nullopt;
    TextFieldModelStatic::SetKeyboardAppearance(frameNode, convValue);
}
void InputFilterImpl(Ark_NativePointer node,
                     const Opt_ResourceStr* value,
                     const Opt_Callback_String_Void* error)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    auto valueString = Converter::OptConvert<std::string>(*value);
    std::function<void(const std::u16string&)> onErrorEvent = nullptr;
    if (error) {
        auto arkOnError = Converter::OptConvert<Callback_String_Void>(*error);
        if (arkOnError) {
            onErrorEvent = [arkCallback = CallbackHelper(arkOnError.value())](const std::u16string& val) {
                Converter::ConvContext ctx;
                arkCallback.Invoke(Converter::ArkValue<Ark_String>(val, &ctx));
            };
        }
    }
    TextFieldModelNG::SetInputFilter(frameNode, valueString.value_or(""), std::move(onErrorEvent));
}
void CustomKeyboardImpl(Ark_NativePointer node,
                        const Opt_CustomNodeBuilder* value,
                        const Opt_KeyboardOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    KeyboardOptions keyboardOptions = {.supportAvoidance = false};
    auto convOptions = options ? Converter::OptConvert<KeyboardOptions>(*options) : keyboardOptions;
    bool supportAvoidance = convOptions.has_value() ? convOptions->supportAvoidance : false;
    auto optValue = Converter::GetOptPtr(value);
    if (!optValue) {
        // TODO: Reset value
        return;
    }
    CallbackHelper(*optValue).BuildAsync([frameNode, supportAvoidance](const RefPtr<UINode>& uiNode) {
        auto customNode = AceType::DynamicCast<FrameNode>(uiNode);
        auto customFrameNode = Referenced::RawPtr(customNode);
        TextFieldModelNG::SetCustomKeyboard(frameNode, customFrameNode, supportAvoidance);
        }, node);
}
void ShowCounterImpl(Ark_NativePointer node,
                     const Opt_Boolean* value,
                     const Opt_InputCounterOptions* options)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto counterOptions = Converter::OptConvert<InputCounterOptions>(*options);
    auto isShowCounter = Converter::OptConvert<bool>(*value);
    if (!isShowCounter) {
        // TODO: Reset value
        return;
    }
    if (counterOptions && counterOptions->thresholdPercentage.has_value()) {
        int32_t thresholdValue = counterOptions->thresholdPercentage.value();
        if (thresholdValue < MIN_THRESHOLD_PERCENTAGE || thresholdValue > MAX_THRESHOLD_PERCENTAGE) {
            counterOptions->thresholdPercentage = std::nullopt;
            isShowCounter = false;
        }
    }
    TextFieldModelNG::SetShowCounter(frameNode, *isShowCounter);
    TextFieldModelStatic::SetCounterType(frameNode, counterOptions->thresholdPercentage);
    TextFieldModelStatic::SetShowCounterBorder(frameNode, counterOptions->highlightBorder);
}
void _onChangeEvent_textImpl(Ark_NativePointer node,
                             const Callback_ResourceStr_Void* callback)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(callback);
    auto onEvent = [arkCallback = CallbackHelper(*callback)](const std::u16string& content) {
        Converter::ConvContext ctx;
        auto arkContent = Converter::ArkUnion<Ark_ResourceStr, Ark_String>(content, &ctx);
        arkCallback.Invoke(arkContent);
    };
    TextFieldModelStatic::SetOnChangeEvent(frameNode, std::move(onEvent));
}
} // TextInputAttributeModifier
const GENERATED_ArkUITextInputModifier* GetTextInputModifier()
{
    static const GENERATED_ArkUITextInputModifier ArkUITextInputModifierImpl {
        TextInputModifier::ConstructImpl,
        TextInputInterfaceModifier::SetTextInputOptionsImpl,
        TextInputAttributeModifier::TypeImpl,
        TextInputAttributeModifier::ContentTypeImpl,
        TextInputAttributeModifier::PlaceholderColorImpl,
        TextInputAttributeModifier::TextOverflowImpl,
        TextInputAttributeModifier::TextIndentImpl,
        TextInputAttributeModifier::PlaceholderFontImpl,
        TextInputAttributeModifier::EnterKeyTypeImpl,
        TextInputAttributeModifier::CaretColorImpl,
        TextInputAttributeModifier::OnEditChangedImpl,
        TextInputAttributeModifier::OnEditChangeImpl,
        TextInputAttributeModifier::OnSubmitImpl,
        TextInputAttributeModifier::OnChangeImpl,
        TextInputAttributeModifier::OnTextSelectionChangeImpl,
        TextInputAttributeModifier::OnContentScrollImpl,
        TextInputAttributeModifier::MaxLengthImpl,
        TextInputAttributeModifier::FontColorImpl,
        TextInputAttributeModifier::FontSizeImpl,
        TextInputAttributeModifier::FontStyleImpl,
        TextInputAttributeModifier::FontWeightImpl,
        TextInputAttributeModifier::FontFamilyImpl,
        TextInputAttributeModifier::OnCopyImpl,
        TextInputAttributeModifier::OnCutImpl,
        TextInputAttributeModifier::OnPasteImpl,
        TextInputAttributeModifier::CopyOptionImpl,
        TextInputAttributeModifier::ShowPasswordIconImpl,
        TextInputAttributeModifier::TextAlignImpl,
        TextInputAttributeModifier::StyleImpl,
        TextInputAttributeModifier::CaretStyleImpl,
        TextInputAttributeModifier::SelectedBackgroundColorImpl,
        TextInputAttributeModifier::CaretPositionImpl,
        TextInputAttributeModifier::EnableKeyboardOnFocusImpl,
        TextInputAttributeModifier::PasswordIconImpl,
        TextInputAttributeModifier::ShowErrorImpl,
        TextInputAttributeModifier::ShowUnitImpl,
        TextInputAttributeModifier::ShowUnderlineImpl,
        TextInputAttributeModifier::UnderlineColorImpl,
        TextInputAttributeModifier::SelectionMenuHiddenImpl,
        TextInputAttributeModifier::BarStateImpl,
        TextInputAttributeModifier::MaxLinesImpl,
        TextInputAttributeModifier::WordBreakImpl,
        TextInputAttributeModifier::LineBreakStrategyImpl,
        TextInputAttributeModifier::CancelButton0Impl,
        TextInputAttributeModifier::CancelButton1Impl,
        TextInputAttributeModifier::SelectAllImpl,
        TextInputAttributeModifier::MinFontSizeImpl,
        TextInputAttributeModifier::MaxFontSizeImpl,
        TextInputAttributeModifier::MinFontScaleImpl,
        TextInputAttributeModifier::MaxFontScaleImpl,
        TextInputAttributeModifier::HeightAdaptivePolicyImpl,
        TextInputAttributeModifier::EnableAutoFillImpl,
        TextInputAttributeModifier::DecorationImpl,
        TextInputAttributeModifier::LetterSpacingImpl,
        TextInputAttributeModifier::LineHeightImpl,
        TextInputAttributeModifier::PasswordRulesImpl,
        TextInputAttributeModifier::FontFeatureImpl,
        TextInputAttributeModifier::ShowPasswordImpl,
        TextInputAttributeModifier::OnSecurityStateChangeImpl,
        TextInputAttributeModifier::OnWillInsertImpl,
        TextInputAttributeModifier::OnDidInsertImpl,
        TextInputAttributeModifier::OnWillDeleteImpl,
        TextInputAttributeModifier::OnDidDeleteImpl,
        TextInputAttributeModifier::EditMenuOptionsImpl,
        TextInputAttributeModifier::EnablePreviewTextImpl,
        TextInputAttributeModifier::EnableHapticFeedbackImpl,
        TextInputAttributeModifier::AutoCapitalizationModeImpl,
        TextInputAttributeModifier::HalfLeadingImpl,
        TextInputAttributeModifier::EllipsisModeImpl,
        TextInputAttributeModifier::StopBackPressImpl,
        TextInputAttributeModifier::OnWillChangeImpl,
        TextInputAttributeModifier::KeyboardAppearanceImpl,
        TextInputAttributeModifier::InputFilterImpl,
        TextInputAttributeModifier::CustomKeyboardImpl,
        TextInputAttributeModifier::ShowCounterImpl,
        TextInputAttributeModifier::_onChangeEvent_textImpl,
    };
    return &ArkUITextInputModifierImpl;
}

}

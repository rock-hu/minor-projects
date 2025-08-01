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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_FIELD_TEXT_FIELD_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_FIELD_TEXT_FIELD_PATTERN_H

#include <cstdint>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/geometry/rect.h"
#include "base/memory/referenced.h"
#include "base/mousestyle/mouse_style.h"
#include "base/utils/multi_thread.h"
#include "base/utils/utf_helper.h"
#include "base/view_data/view_data_wrap.h"
#include "core/common/ace_application_info.h"
#include "core/common/ai/ai_write_adapter.h"
#include "base/view_data/hint_to_type_wrap.h"
#include "core/common/clipboard/clipboard.h"
#include "core/common/ime/text_edit_controller.h"
#include "core/common/ime/text_input_action.h"
#include "core/common/ime/text_input_client.h"
#include "core/common/ime/text_input_configuration.h"
#include "core/common/ime/text_input_connection.h"
#include "core/common/ime/text_input_formatter.h"
#include "core/common/ime/text_input_proxy.h"
#include "core/common/ime/text_input_type.h"
#include "core/common/ime/text_selection.h"
#include "core/components/text_field/textfield_theme.h"
#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/pattern/overlay/keyboard_base_pattern.h"
#include "core/components_ng/pattern/scrollable/scrollable_pattern.h"
#include "core/components_ng/pattern/text/layout_info_interface.h"
#include "core/components_ng/pattern/select_overlay/magnifier.h"
#include "core/components_ng/pattern/text/multiple_click_recognizer.h"
#include "core/components_ng/pattern/text/text_base.h"
#include "core/components_ng/pattern/text/text_menu_extension.h"
#include "core/components_ng/pattern/text_area/text_area_layout_algorithm.h"
#include "core/components_ng/pattern/text_drag/text_drag_base.h"
#include "core/components_ng/pattern/text_field/content_controller.h"
#include "core/components_ng/pattern/text_field/text_component_decorator.h"
#include "core/components_ng/pattern/text_field/text_editing_value_ng.h"
#include "core/components_ng/pattern/text_field/text_content_type.h"
#include "core/components_ng/pattern/text_field/auto_fill_controller.h"
#include "core/components_ng/pattern/text_field/text_field_accessibility_property.h"
#include "core/components_ng/pattern/text_field/text_field_controller.h"
#include "core/components_ng/pattern/text_field/text_field_event_hub.h"
#include "core/components_ng/pattern/text_field/text_field_layout_property.h"
#include "core/components_ng/pattern/text_field/text_field_manager.h"
#include "core/components_ng/pattern/text_field/text_field_paint_method.h"
#include "core/components_ng/pattern/text_field/text_field_paint_property.h"
#include "core/components_ng/pattern/text_field/text_field_select_overlay.h"
#include "core/components_ng/pattern/text_field/text_input_response_area.h"
#include "core/components_ng/pattern/text_field/text_select_controller.h"
#include "core/components_ng/pattern/text_field/text_selector.h"
#include "core/components_ng/pattern/text_input/text_input_layout_algorithm.h"
#include "core/components_ng/pattern/text_field/text_keyboard_common_type.h"

#ifndef ACE_UNITTEST
#ifdef ENABLE_STANDARD_INPUT
#include "refbase.h"

namespace OHOS::MiscServices {
class InspectorFilter;
class OnTextChangedListener;

struct TextConfig;
} // namespace OHOS::MiscServices
#endif
#endif

namespace OHOS::Ace::NG {

enum class FocuseIndex { TEXT = 0, CANCEL, UNIT };

enum class SelectionMode { SELECT, SELECT_ALL, NONE };

enum class DragStatus { DRAGGING, ON_DROP, NONE };

enum class CaretStatus { SHOW, HIDE, NONE };

enum class InputOperation {
    INSERT,
    DELETE_BACKWARD,
    DELETE_FORWARD,
    CURSOR_UP,
    CURSOR_DOWN,
    CURSOR_LEFT,
    CURSOR_RIGHT,
    SET_PREVIEW_TEXT,
    SET_PREVIEW_FINISH,
    INPUT,
};

struct PasswordModeStyle {
    Color bgColor;
    Color textColor;
    BorderWidthProperty borderwidth;
    BorderColorProperty borderColor;
    BorderRadiusProperty radius;
    PaddingProperty padding;
    MarginProperty margin;
};

struct PreState {
    Color textColor;
    Color bgColor;
    BorderRadiusProperty radius;
    BorderWidthProperty borderWidth;
    BorderColorProperty borderColor;
    PaddingProperty padding;
    MarginProperty margin;
    RectF frameRect;
    bool setHeight = false;
    bool saveState = false;
    bool hasBorderColor = false;
};

enum class RequestKeyboardReason {
    UNKNOWN = 0,
    ON_KEY_EVENT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS,
    RESET_KEYBOARD,
    MOUSE_RELEASE,
    SET_SELECTION,
    SEARCH_REQUEST,
    AUTO_FILL_REQUEST_FAIL,
    SHOW_KEYBOARD_ON_FOCUS,
    STYLUS_DETECTOR,
    CUSTOM_KEYBOARD
};

enum class RequestFocusReason {
    UNKNOWN = 0,
    DRAG_END,
    DRAG_MOVE,
    CLICK,
    LONG_PRESS,
    AUTO_FILL,
    CLEAN_NODE,
    MOUSE,
    SYSTEM,
    DRAG_ENTER,
    DRAG_SELECT
};


// reason for needToRequestKeyboardInner_ change
enum class RequestKeyboardInnerChangeReason {
    UNKNOWN = 0,
    BLUR,
    FOCUS,
    AUTOFILL_PROCESS,
    REQUEST_KEYBOARD_SUCCESS,
    SEARCH_FOCUS
};

enum class InputReason {
    NONE = 0,
    IME,
    PASTE,
    CUT,
    DRAG,
    AUTO_FILL,
    AI_WRITE,
    CANCEL_BUTTON
};

struct PreviewTextInfo {
    std::u16string text;
    PreviewRange range;
    bool isIme;
};

struct InsertCommandInfo {
    std::u16string insertValue;
    InputReason reason;
};

struct InputCommandInfo {
    PreviewRange deleteRange;
    int32_t insertOffset;
    std::u16string insertValue;
    InputReason reason;
};

struct TouchAndMoveCaretState {
    bool isTouchCaret = false;
    bool isMoveCaret = false;
    Offset touchDownOffset;
    Dimension minDinstance = 5.0_vp;
    int32_t touchFingerId = -1;
};

struct FloatingCaretState {
    bool FloatingCursorVisible = false;
    bool ShowOriginCursor = false;
    Color OriginCursorColor = Color(0x4D000000);
    std::optional<float> lastFloatingCursorY = std::nullopt;

    void Reset()
    {
        FloatingCursorVisible = false;
        ShowOriginCursor = false;
        lastFloatingCursorY = std::nullopt;
    }
};

struct ContentScroller {
    CancelableCallback<void()> autoScrollTask;
    std::function<void(const Offset&)> scrollingCallback;
    std::function<void(const Offset&)> beforeScrollingCallback;
    bool isScrolling = false;
    float scrollInterval = 15;
    float stepOffset = 0.0f;
    Offset localOffset;
    std::optional<Offset> hotAreaOffset;
    float updateMagniferEpsilon = 0.5f;

    void OnBeforeScrollingCallback(const Offset& localOffset)
    {
        if (beforeScrollingCallback && !isScrolling) {
            beforeScrollingCallback(localOffset);
        }
    }
};

class TextFieldPattern : public ScrollablePattern,
                         public TextDragBase,
                         public ValueChangeObserver,
                         public TextInputClient,
                         public TextBase,
                         public Magnifier,
                         public TextGestureSelector,
                         public LayoutInfoInterface {
    DECLARE_ACE_TYPE(TextFieldPattern, ScrollablePattern, TextDragBase, ValueChangeObserver, TextInputClient, TextBase,
        Magnifier, TextGestureSelector);

public:
    TextFieldPattern();
    ~TextFieldPattern() override;

    int32_t GetInstanceId() const override
    {
        return GetHostInstanceId();
    }

    // TextField needs softkeyboard, override function.
    bool NeedSoftKeyboard() const override
    {
        return true;
    }

    void SetBlurOnSubmit(bool blurOnSubmit)
    {
        textInputBlurOnSubmit_ = blurOnSubmit;
        textAreaBlurOnSubmit_ = blurOnSubmit;
    }

    bool GetBlurOnSubmit()
    {
        return IsTextArea() ? textAreaBlurOnSubmit_ : textInputBlurOnSubmit_;
    }

    void SetKeyboardAppearance(KeyboardAppearance value)
    {
        keyboardAppearance_ = value;
    }

    KeyboardAppearance GetKeyboardAppearance() const
    {
        return keyboardAppearance_;
    }

    bool NeedToRequestKeyboardOnFocus() const override
    {
        return needToRequestKeyboardOnFocus_;
    }

    bool CheckBlurReason();

    bool NeedSetScrollRect();

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override;

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<TextFieldLayoutProperty>();
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<TextFieldEventHub>();
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<TextFieldPaintProperty>();
    }

    RefPtr<AccessibilityProperty> CreateAccessibilityProperty() override
    {
        return MakeRefPtr<TextFieldAccessibilityProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        if (IsTextArea()) {
            return MakeRefPtr<TextAreaLayoutAlgorithm>();
        }
        return MakeRefPtr<TextInputLayoutAlgorithm>();
    }

    void OnModifyDone() override;
    void OnModifyDoneMultiThread();
    void OnModifyDoneMultiThreadPart();
    void OnModifyDoneMultiThreadAddition();
    void ProcessUnderlineColorOnModifierDone();
    void UpdateSelectionOffset();
    void CalcCaretMetricsByPosition(
        int32_t extent, CaretMetricsF& caretCaretMetric, TextAffinity textAffinity = TextAffinity::DOWNSTREAM);
    int32_t ConvertTouchOffsetToCaretPosition(const Offset& localOffset);
    int32_t ConvertTouchOffsetToCaretPositionNG(const Offset& localOffset);

    // Obtain the systemWindowsId when switching between windows
    uint32_t GetSCBSystemWindowId();

    void InsertValue(const std::u16string& insertValue, bool isIME = false) override;
    void InsertValue(const std::string& insertValue, bool isIME = false) override;
    void NotifyImfFinishTextPreview();
    int32_t InsertValueByController(const std::u16string& insertValue, int32_t offset);
    void ExecuteInsertValueCommand(const InsertCommandInfo& info);
    void CalcCounterAfterFilterInsertValue(int32_t curLength, const std::u16string insertValue, int32_t maxLength);
    void UpdateObscure(const std::u16string& insertValue, bool hasInsertValue);
    void CleanCounterNode();
    void CleanErrorNode();
    float CalcDecoratorWidth(const RefPtr<FrameNode>& decoratorNode);
    float CalcDecoratorHeight(const RefPtr<FrameNode>& decoratorNode);
    void UltralimitShake();
    void UpdateAreaBorderStyle(BorderWidthProperty& currentBorderWidth, BorderWidthProperty& overCountBorderWidth,
        BorderColorProperty& overCountBorderColor, BorderColorProperty& currentBorderColor);
    void DeleteBackward(int32_t length) override;
    void DeleteBackwardOperation(int32_t length);
    void DeleteForward(int32_t length) override;
    void DeleteForwardOperation(int32_t length);
    void HandleOnDelete(bool backward) override;
    bool HandleOnDeleteComb(bool backward) override;
    void DeleteBackwardWord();
    void DeleteForwardWord();
    void HandleOnPageUp() override;
    void HandleOnPageDown() override;
    void CreateHandles() override;
    void GetEmojiSubStringRange(int32_t& start, int32_t& end);

    int32_t SetPreviewText(const std::u16string& previewValue, const PreviewRange range) override;
    int32_t SetPreviewText(const std::string& previewValue, const PreviewRange range) override;
    void FinishTextPreview() override;
    void SetPreviewTextOperation(PreviewTextInfo info);
    void SetPreviewTextOperationMultiThread(PreviewTextInfo info);
    void SetPreviewTextOperationMultiThreadPart(PreviewTextInfo info, int32_t start, int32_t end);
    void FinishTextPreviewOperation(bool triggerOnWillChange = true);
    void FinishTextPreviewOperationMultiThread(bool triggerOnWillChange = true);
    void FinishTextPreviewOperationMultiThreadPart(bool triggerOnWillChange = true);
    TextDragInfo CreateTextDragInfo() const;

    RefPtr<TextComponentDecorator> GetCounterDecorator() const
    {
        return counterDecorator_;
    }

    RefPtr<TextComponentDecorator> GetErrorDecorator() const
    {
        return errorDecorator_;
    }

    bool GetShowCounterStyleValue() const
    {
        return showCountBorderStyle_;
    }

    void SetCounterState(bool counterChange)
    {
        counterChange_ = counterChange;
    }

    float GetTextOrPlaceHolderFontSize();

    void SetTextFieldController(const RefPtr<TextFieldController>& controller)
    {
        textFieldController_ = controller;
    }

    const RefPtr<TextFieldController>& GetTextFieldController()
    {
        return textFieldController_;
    }

    void SetJSTextEditableController(const RefPtr<Referenced>& jsController)
    {
        jsTextEditableController_ = jsController;
    }

    RefPtr<Referenced> GetJSTextEditableController()
    {
        return jsTextEditableController_.Upgrade();
    }

    std::string GetTextValue() const
    {
        return contentController_->GetTextValue();
    }

    const std::u16string& GetTextUtf16Value() const
    {
        return contentController_->GetTextUtf16Value();
    }

    const RefPtr<AutoFillController>& GetOrCreateAutoFillController()
    {
        if (!autoFillController_) {
            autoFillController_ = MakeRefPtr<AutoFillController>(WeakClaim(this));
        }
        return autoFillController_;
    }

#if defined(IOS_PLATFORM)
    const TextEditingValue& GetInputEditingValue() const override
    {
        static TextEditingValue value;
        value.text = contentController_->GetTextValue();
        value.hint = UtfUtils::Str16DebugToStr8(GetPlaceHolder());
        value.selection.Update(selectController_->GetStartIndex(), selectController_->GetEndIndex());
        return value;
    };
    Offset GetGlobalOffset() const;
    double GetEditingBoxY() const override;
    double GetEditingBoxTopY() const override;
    bool GetEditingBoxModel() const override;
#endif

    bool ShouldDelayChildPressedState() const override
    {
        return false;
    }

    void UpdateEditingValue(const std::string& value, int32_t caretPosition)
    {
        contentController_->SetTextValue(UtfUtils::Str8DebugToStr16(value));
        selectController_->UpdateCaretIndex(caretPosition);
    }
    void UpdateCaretPositionByTouch(const Offset& offset);
    bool IsReachedBoundary(float offset);

    virtual int32_t GetRequestKeyboardId();

    virtual TextInputAction GetDefaultTextInputAction() const;
    bool RequestKeyboardCrossPlatForm(bool isFocusViewChanged);
    bool RequestKeyboard(bool isFocusViewChanged, bool needStartTwinkling, bool needShowSoftKeyboard,
        SourceType sourceType = SourceType::NONE);
    bool CloseKeyboard(bool forceClose) override;
    bool CloseKeyboard(bool forceClose, bool isStopTwinkling);

    FocusPattern GetFocusPattern() const override;
    void PerformAction(TextInputAction action, bool forceCloseKeyboard = false) override;
    void UpdateEditingValue(const std::shared_ptr<TextEditingValue>& value, bool needFireChangeEvent = true) override;
    void UpdateInputFilterErrorText(const std::u16string& errorText) override;
    void UpdateInputFilterErrorText(const std::string& errorText) override;

    void OnValueChanged(bool needFireChangeEvent = true, bool needFireSelectChangeEvent = true) override;

    void OnHandleAreaChanged() override;

    void HandleCounterBorder();

    int32_t GetCaretIndex() const override
    {
        return selectController_->GetCaretIndex();
    }

    OffsetF GetFirstHandleOffset() const override
    {
        return selectController_->GetFirstHandleOffset();
    }

    OffsetF GetSecondHandleOffset() const override
    {
        return selectController_->GetSecondHandleOffset();
    }

    ACE_DEFINE_PROPERTY_ITEM_FUNC_WITHOUT_GROUP(TextInputAction, TextInputAction)

    ACE_DEFINE_PROPERTY_ITEM_FUNC_WITHOUT_GROUP(AutoCapitalizationMode, AutoCapitalizationMode)

    const RefPtr<Paragraph>& GetParagraph() const
    {
        return paragraph_;
    }

    bool GetCursorVisible() const
    {
        return cursorVisible_;
    }

    bool GetFloatingCursorVisible() const
    {
        return floatCaretState_.FloatingCursorVisible;
    }

    void SetFloatingCursorVisible(bool floatingCursorVisible)
    {
        floatCaretState_.FloatingCursorVisible = floatingCursorVisible;
    }

    void ResetFloatingCursorState()
    {
        floatCaretState_.Reset();
    }

    void SetMagnifierLocalOffsetToFloatingCaretPos();

    bool GetShowOriginCursor() const
    {
        return floatCaretState_.ShowOriginCursor;
    }

    void SetShowOriginCursor(bool showOriginCursor)
    {
        floatCaretState_.ShowOriginCursor = showOriginCursor;
    }

    Color GetOriginCursorColor() const
    {
        return floatCaretState_.OriginCursorColor;
    }

    void SetOriginCursorColor(Color originCursorColor)
    {
        floatCaretState_.OriginCursorColor = originCursorColor;
    }

    virtual void AdjustFloatingCaretInfo(const Offset& localOffset,
        const HandleInfoNG& caretInfo, HandleInfoNG& floatingCaretInfo);

    void FloatingCaretLand();

#if defined(OHOS_STANDARD_SYSTEM) && !defined(PREVIEW)
    bool GetImeAttached() const
    {
        return imeAttached_;
    }
#endif

    const OffsetF& GetLastTouchOffset()
    {
        return lastTouchOffset_;
    }

    OffsetF GetCaretOffset() const override
    {
        return movingCaretOffset_;
    }

    void SetMovingCaretOffset(const OffsetF& offset)
    {
        movingCaretOffset_ = offset;
    }

    CaretUpdateType GetCaretUpdateType() const
    {
        return caretUpdateType_;
    }

    void SetCaretUpdateType(CaretUpdateType type)
    {
        caretUpdateType_ = type;
    }

    float GetPaddingTop() const;
    float GetPaddingBottom() const;
    float GetPaddingLeft() const;
    float GetPaddingRight() const;

    float GetHorizontalPaddingAndBorderSum() const;

    float GetVerticalPaddingAndBorderSum() const;

    double GetPercentReferenceWidth() const;

    BorderWidthProperty GetBorderWidthProperty() const;
    float GetBorderLeft(BorderWidthProperty border) const;
    float GetBorderTop(BorderWidthProperty border) const;
    float GetBorderBottom(BorderWidthProperty border) const;
    float GetBorderRight(BorderWidthProperty border) const;

    void OnDragNodeDetachFromMainTree() override;
    const RectF& GetTextRect() const override
    {
        return textRect_;
    }

    void SetTextRect(const RectF& textRect)
    {
        textRect_ = textRect;
    }

    float GetTextParagraphIndent() const
    {
        return textParagraphIndent_;
    }

    const RectF& GetFrameRect() const
    {
        return frameRect_;
    }

    float GetCountHeight() const
    {
        return countHeight_;
    }

    const RefPtr<TextSelectController>& GetTextSelectController()
    {
        return selectController_;
    }

    const RefPtr<ContentController>& GetTextContentController()
    {
        return contentController_;
    }

    void SetInSelectMode(SelectionMode selectionMode)
    {
        selectionMode_ = selectionMode;
    }

    SelectionMode GetSelectMode() const
    {
        return selectionMode_;
    }

    bool IsSelected() const override
    {
        return selectController_->IsSelected();
    }

    bool IsUsingMouse() const
    {
        return selectOverlay_->IsUsingMouse();
    }
    int32_t GetWordLength(int32_t originCaretPosition, int32_t directionalMove, bool skipNewLineChar = true);
    int32_t GetLineBeginPosition(int32_t originCaretPosition, bool needToCheckLineChanged = true);
    int32_t GetLineEndPosition(int32_t originCaretPosition, bool needToCheckLineChanged = true);
    bool HasText() const
    {
        return !contentController_->IsEmpty();
    }

    void CursorMove(CaretMoveIntent direction) override;
    bool CursorMoveLeft();
    bool CursorMoveLeftOperation();
    bool CursorMoveLeftWord();
    bool CursorMoveLineBegin();
    bool CursorMoveToParagraphBegin();
    bool CursorMoveHome();
    bool CursorMoveRight();
    bool CursorMoveRightOperation();
    bool CursorMoveRightWord();
    bool CursorMoveLineEnd();
    bool CursorMoveToParagraphEnd();
    bool CursorMoveEnd();
    bool CursorMoveUp();
    bool CursorMoveDown();
    bool CursorMoveUpOperation();
    bool CursorMoveDownOperation();
    void SetCaretPosition(int32_t position, bool moveContent = true);
    void SetCaretPositionMultiThread(int32_t position, bool moveContent = true);
    void HandleSetSelection(int32_t start, int32_t end, bool showHandle = true) override;
    void HandleSetSelectionMultiThread(int32_t start, int32_t end, bool showHandle = true);
    void HandleExtendAction(int32_t action) override;
    void HandleSelect(CaretMoveIntent direction) override;

    void HandleSelectExtend(CaretMoveIntent direction) override
    {
        HandleSelect(direction);
    }

    OffsetF GetDragUpperLeftCoordinates() override;

    std::vector<RectF> GetTextBoxes() override
    {
        return selectController_->GetSelectedRects();
    }
    std::vector<RectF> GetTextBoxesForSelect();
    void AdjustSelectedBlankLineWidth(RectF& rect);
    void ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const override;
    void ToTreeJson(std::unique_ptr<JsonValue>& json, const InspectorConfig& config) const override;
    void ToJsonValueForFontFeature(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;
    void ToJsonValueForOption(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;
    void ToJsonValueSelectOverlay(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;
    void ToJsonValueForStroke(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const;
    void FromJson(const std::unique_ptr<JsonValue>& json) override;
    void InitEditingValueText(std::u16string content);
    bool InitValueText(std::u16string content);
    void HandleButtonMouseEvent(const RefPtr<TextInputResponseArea>& responseArea, bool isHover);

    void CloseSelectOverlay() override;
    void CloseSelectOverlay(bool animation);
    void SetInputMethodStatus(bool keyboardShown) override
    {
#if defined(OHOS_STANDARD_SYSTEM) && !defined(PREVIEW)
        imeShown_ = keyboardShown;
#endif
    }
    void NotifyKeyboardClosedByUser() override;

    void NotifyKeyboardClosed() override;

    std::u16string GetLeftTextOfCursor(int32_t number) override;
    std::u16string GetRightTextOfCursor(int32_t number) override;
    int32_t GetTextIndexAtCursor() override;

    bool HasConnection() const
    {
#if defined(OHOS_STANDARD_SYSTEM) && !defined(PREVIEW)
        return imeShown_;
#else
        return connection_;
#endif
    }
    float PreferredLineHeight(bool isAlgorithmMeasure = false);

    void SearchRequestKeyboard();

    bool RequestKeyboardNotByFocusSwitch(
        RequestKeyboardReason reason = RequestKeyboardReason::UNKNOWN, SourceType sourceType = SourceType::NONE);

    bool TextFieldRequestFocus(RequestFocusReason reason = RequestFocusReason::UNKNOWN);

    static std::string RequestFocusReasonToString(RequestFocusReason reason);

    static std::string RequestKeyboardReasonToString(RequestKeyboardReason reason);

    bool GetTextObscured() const
    {
        return textObscured_;
    }

    static std::u16string CreateObscuredText(int32_t len);
    static std::u16string CreateDisplayText(
        const std::u16string& content, int32_t nakedCharPosition, bool needObscureText, bool showPasswordDirectly);
    bool IsTextArea() const override;

    const RefPtr<TouchEventImpl>& GetTouchListener()
    {
        return touchListener_;
    }

    bool NeedShowPasswordIcon()
    {
        auto layoutProperty = GetLayoutProperty<TextFieldLayoutProperty>();
        CHECK_NULL_RETURN(layoutProperty, false);
        return IsInPasswordMode() && layoutProperty->GetShowPasswordIconValue(true);
    }

    void SetEnableTouchAndHoverEffect(bool enable)
    {
        enableTouchAndHoverEffect_ = enable;
    }

    RectF GetCaretRect() const override
    {
        return selectController_->GetCaretRect();
    }

    RectF GetFloatingCaretRect() const
    {
        return selectController_->GetFloatingCaretRect();
    }

    void HandleSurfaceChanged(int32_t newWidth, int32_t newHeight, int32_t prevWidth, int32_t prevHeight);
    void HandleSurfacePositionChanged(int32_t posX, int32_t posY);

    void InitSurfaceChangedCallback();
    void InitSurfaceChangedCallbackMultiThread();
    void InitSurfaceChangedCallbackMultiThreadAction();
    void InitSurfacePositionChangedCallback();
    void InitSurfacePositionChangedCallbackMultiThread();
    void InitSurfacePositionChangedCallbackMultiThreadAction();

    bool HasSurfaceChangedCallback()
    {
        return surfaceChangedCallbackId_.has_value();
    }
    void UpdateSurfaceChangedCallbackId(int32_t id)
    {
        surfaceChangedCallbackId_ = id;
    }

    bool HasSurfacePositionChangedCallback()
    {
        return surfacePositionChangedCallbackId_.has_value();
    }
    void UpdateSurfacePositionChangedCallbackId(int32_t id)
    {
        surfacePositionChangedCallbackId_ = id;
    }

    void ProcessInnerPadding();
    void ProcessNumberOfLines();
    void OnCursorMoveDone(
        TextAffinity textAffinity = TextAffinity::UPSTREAM, std::optional<Offset> offset = std::nullopt);
    bool IsDisabled();
    bool AllowCopy();

    bool GetIsMousePressed() const
    {
        return isMousePressed_;
    }

    MouseStatus GetMouseStatus() const
    {
        return mouseStatus_;
    }

    void UpdateEditingValueToRecord(int32_t beforeCaretPosition = -1);

    void UpdateScrollBarOffset() override;

    bool UpdateCurrentOffset(float offset, int32_t source) override
    {
        OnScrollCallback(offset, source);
        return true;
    }

    void PlayScrollBarAppearAnimation();

    void ScheduleDisappearDelayTask();

    bool IsAtTop() const override
    {
        return contentRect_.GetY() == textRect_.GetY();
    }

    bool IsAtBottom(bool considerRepeat = false) const override
    {
        return contentRect_.GetY() + contentRect_.Height() == textRect_.GetY() + textRect_.Height();
    }

    bool IsScrollable() const override
    {
        return scrollable_;
    }

    bool IsAtomicNode() const override
    {
        return true;
    }

    float GetCurrentOffset() const
    {
        return currentOffset_;
    }

    RefPtr<TextFieldContentModifier> GetContentModifier()
    {
        return textFieldContentModifier_;
    }

    double GetScrollBarWidth();

    float GetLineHeight() const override
    {
        return selectController_->GetCaretRect().Height();
    }

    OffsetF GetParentGlobalOffset() const override
    {
        return parentGlobalOffset_;
    }

    RectF GetTextContentRect(bool isActualText = false) const override
    {
        return contentRect_;
    }

    const RefPtr<OverlayManager>& GetKeyboardOverLay()
    {
        return keyboardOverlay_;
    }

    bool GetIsCustomKeyboardAttached()
    {
        return isCustomKeyboardAttached_;
    }

    const RefPtr<Paragraph>& GetDragParagraph() const override
    {
        return paragraph_;
    }

    const RefPtr<FrameNode>& MoveDragNode() override
    {
        return dragNode_;
    }

    const std::vector<std::u16string>& GetDragContents() const
    {
        return dragContents_;
    }

    bool IsDragging() const
    {
        return dragStatus_ == DragStatus::DRAGGING;
    }

    bool BetweenSelectedPosition(GestureEvent& info);

    bool BetweenSelectedPosition(const Offset& globalOffset) override;

    bool RequestCustomKeyboard();
    bool CloseCustomKeyboard();

    // xts
    std::string GetStrokeWidth() const;
    std::string GetStrokeColor() const;
    std::string TextInputTypeToString() const;
    std::string TextInputActionToString() const;
    std::string AutoCapTypeToString() const;
    std::string TextContentTypeToString() const;
    virtual std::string GetPlaceholderFont() const;
    RefPtr<TextFieldTheme> GetTheme() const;
    void InitTheme();
    std::string GetTextColor() const;
    std::string GetCaretColor() const;
    std::string GetPlaceholderColor() const;
    std::string GetFontSize() const;
    std::string GetMinFontSize() const;
    std::string GetMaxFontSize() const;
    std::string GetMinFontScale() const;
    std::string GetMaxFontScale() const;
    std::string GetEllipsisMode() const;
    std::string GetTextIndent() const;
    Ace::FontStyle GetItalicFontStyle() const;
    FontWeight GetFontWeight() const;
    std::string GetFontFamily() const;
    TextAlign GetTextAlign() const;
    std::u16string GetPlaceHolder() const;
    uint32_t GetMaxLength() const;
    uint32_t GetMaxLines() const;
    uint32_t GetMinLines() const;
    std::string GetInputFilter() const;
    std::string GetCopyOptionString() const;
    std::string GetInputStyleString() const;
    std::u16string GetErrorTextString() const;
    std::string GetBarStateString() const;
    bool GetErrorTextState() const;
    std::string GetShowPasswordIconString() const;
    int32_t GetNakedCharPosition() const;
    void SetSelectionFlag(int32_t selectionStart, int32_t selectionEnd,
        const std::optional<SelectionOptions>& options = std::nullopt, bool isForward = false);
    void SetSelectionFlagMultiThread(int32_t selectionStart, int32_t selectionEnd,
        const std::optional<SelectionOptions>& options = std::nullopt, bool isForward = false);
    void SetSelection(int32_t start, int32_t end,
        const std::optional<SelectionOptions>& options = std::nullopt, bool isForward = false) override;
    void HandleBlurEvent();
    void HandleFocusEvent();
    void SetFocusStyle();
    void ClearFocusStyle();
    void ProcessFocusStyle();
    bool OnBackPressed() override;
    bool IsStopBackPress() const override;
    void CheckScrollable();
    void HandleClickEvent(GestureEvent& info);
    bool CheckMousePressedOverScrollBar(GestureEvent& info);
    int32_t CheckClickLocation(GestureEvent& info);
    void HandleDoubleClickEvent(GestureEvent& info);
    void HandleTripleClickEvent(GestureEvent& info);
    void HandleSingleClickEvent(GestureEvent& info, bool firstGetFocus = false);
    bool HandleBetweenSelectedPosition(const GestureEvent& info);

    void HandleSelectionUp();
    void HandleSelectionDown();
    void HandleSelectionLeft();
    void HandleSelectionLeftWord();
    void HandleSelectionLineBegin();
    void HandleSelectionHome();
    void HandleSelectionParagraghBegin();
    void HandleSelectionRight();
    void HandleSelectionRightWord();
    void HandleSelectionLineEnd();
    void HandleSelectionEnd();
    void HandleSelectionParagraghEnd();
    bool HandleOnEscape() override;
    bool HandleOnTab(bool backward) override;
    void HandleOnEnter() override
    {
        PerformAction(GetTextInputActionValue(GetDefaultTextInputAction()), false);
    }
    void HandleOnUndoAction() override;

    void HandleOnExtendUndoAction() override
    {
        HandleOnUndoAction();
    }
    
    void HandleOnRedoAction() override;
    bool CanUndo();
    bool HasOperationRecords();
    bool CanRedo();
    void HandleOnSelectAll(bool isKeyEvent, bool inlineStyle = false, bool showMenu = false);
    void HandleOnSelectAll() override
    {
        HandleOnSelectAll(true);
    }
    void HandleOnCopy(bool isUsingExternalKeyboard = false) override;
    void HandleOnPaste() override;
    void HandleOnCut() override;
    bool IsShowTranslate();
    bool IsShowSearch();
    void HandleOnCameraInput();
    void HandleOnAIWrite();
    void GetAIWriteInfo(AIWriteInfo& info);
    bool IsShowAIWrite();
    void HandleAIWriteResult(int32_t start, int32_t end, std::vector<uint8_t>& buffer);
    void UpdateShowCountBorderStyle();
    void StripNextLine(std::wstring& data);
    bool IsShowHandle();
    std::string GetCancelButton();
    std::string GetCancelImageText();
    std::string GetPasswordIconPromptInformation(bool show);
    bool OnKeyEvent(const KeyEvent& event);
    size_t GetLineCount() const override;
    TextInputType GetKeyboard()
    {
        return keyboard_;
    }
    TextInputAction GetAction()
    {
        return action_;
    }

    void SetNeedToRequestKeyboardOnFocus(bool needToRequest)
    {
        needToRequestKeyboardOnFocus_ = needToRequest;
    }
    void SetUnitNode(const RefPtr<NG::UINode>& unitNode);
    void AddCounterNode();
    void SetShowError();

    float GetUnderlineWidth() const
    {
        return static_cast<float>(underlineWidth_.Value());
    }

    const Color& GetUnderlineColor() const
    {
        return underlineColor_;
    }

    float GetMarginBottom() const;

    void SetUnderlineColor(Color underlineColor)
    {
        underlineColor_ = underlineColor;
    }

    void SetTypingUnderlineColor(const Color& normalColor)
    {
        userUnderlineColor_.typing = normalColor;
    }

    void ResetTypingUnderlineColor()
    {
        userUnderlineColor_.typing = std::nullopt;
    }

    void SetNormalUnderlineColor(const Color& normalColor)
    {
        userUnderlineColor_.normal = normalColor;
    }

    void ResetNormalUnderlineColor()
    {
        userUnderlineColor_.normal = std::nullopt;
    }

    void SetErrorUnderlineColor(const Color& normalColor)
    {
        userUnderlineColor_.error = normalColor;
    }

    void ResetErrorUnderlineColor()
    {
        userUnderlineColor_.error = std::nullopt;
    }

    void SetDisableUnderlineColor(const Color& normalColor)
    {
        userUnderlineColor_.disable = normalColor;
    }

    void ResetDisableUnderlineColor()
    {
        userUnderlineColor_.disable = std::nullopt;
    }

    void SetUserUnderlineColor(UserUnderlineColor userUnderlineColor)
    {
        userUnderlineColor_ = userUnderlineColor;
    }

    UserUnderlineColor GetUserUnderlineColor()
    {
        return userUnderlineColor_;
    }

    void SetUnderlineWidth(Dimension underlineWidth)
    {
        underlineWidth_ = underlineWidth;
    }

    bool IsSelectAll()
    {
        return abs(selectController_->GetStartIndex() - selectController_->GetEndIndex()) >=
               static_cast<int32_t>(contentController_->GetTextUtf16Value().length());
    }

    void StopEditing();
    void StopEditingMultiThread();
    void StopEditingMultiThreadAction();

    void MarkContentChange()
    {
        contChange_ = true;
    }

    void ResetContChange()
    {
        contChange_ = false;
    }

    bool GetContChange() const
    {
        return contChange_;
    }
    std::string GetShowResultImageSrc() const;
    std::string GetHideResultImageSrc() const;
    std::string GetNormalUnderlineColorStr() const;
    std::string GetTypingUnderlineColorStr() const;
    std::string GetDisableUnderlineColorStr() const;
    std::string GetErrorUnderlineColorStr() const;
    void OnAttachToFrameNode() override;
    void OnAttachToFrameNodeMultiThread();

    bool GetTextInputFlag() const
    {
        return isTextInput_;
    }

    void SetTextInputFlag(bool isTextInput)
    {
        isTextInput_ = isTextInput;
        SetTextFadeoutCapacity(isTextInput_);
    }

    void SetSingleLineHeight(float height)
    {
        inlineSingleLineHeight_ = height;
    }

    float GetSingleLineHeight() const
    {
        return inlineSingleLineHeight_;
    }

    float GetInlinePadding() const
    {
        return inlinePadding_;
    }

    bool GetScrollBarVisible() const
    {
        return scrollBarVisible_;
    }

    void SetFillRequestFinish(bool success)
    {
        isFillRequestFinish_ = success;
    }

    bool IsFillRequestFinish()
    {
        return isFillRequestFinish_;
    }

    bool IsNormalInlineState() const;
    bool IsUnspecifiedOrTextType() const;
    void TextIsEmptyRect(RectF& rect);
    void TextAreaInputRectUpdate(RectF& rect);
    void UpdateRectByTextAlign(RectF& rect);

    void EditingValueFilterChange();

    void SetCustomKeyboard(const std::function<void()>&& keyboardBuilder);

    void SetCustomKeyboardWithNode(const RefPtr<UINode>& keyboardBuilder);
    void SetCustomKeyboardWithNodeMultiThread(const RefPtr<UINode>& keyboardBuilder);
    void SetCustomKeyboardWithNodeMultiThreadAction(const RefPtr<UINode>& keyboardBuilder);

    bool HasCustomKeyboard() const
    {
        return customKeyboard_ != nullptr || customKeyboardBuilder_ != nullptr;
    }

    void DumpInfo() override;
    void DumpSimplifyInfo(std::shared_ptr<JsonValue>& json) override {}
    void DumpAdvanceInfo() override;
    void DumpPlaceHolderInfo();
    void DumpTextEngineInfo();
    void DumpScaleInfo();
    std::string GetDumpTextValue() const;
    void DumpViewDataPageNode(RefPtr<ViewDataWrap> viewDataWrap, bool needsRecordData = false) override;
    void NotifyFillRequestSuccess(RefPtr<ViewDataWrap> viewDataWrap,
        RefPtr<PageNodeInfoWrap> nodeWrap, AceAutoFillType autoFillType) override;
    void NotifyFillRequestFailed(int32_t errCode, const std::string& fillContent = "", bool isPopup = false) override;
    bool CheckAutoSave() override;
    void OnColorConfigurationUpdate() override;
    bool NeedPaintSelect();
    void SetCustomKeyboardOption(bool supportAvoidance);

    void SetIsCustomFont(bool isCustomFont)
    {
        isCustomFont_ = isCustomFont;
    }

    bool GetIsCustomFont() const
    {
        return isCustomFont_;
    }

    void SetIsCounterIdealHeight(bool isIdealHeight)
    {
        isCounterIdealheight_ = isIdealHeight;
    }

    bool GetIsCounterIdealHeight() const
    {
        return isCounterIdealheight_;
    }

    virtual RefPtr<FocusHub> GetFocusHub() const;
    void UpdateCaretInfoToController(bool forceUpdate = false);
    void OnObscuredChanged(bool isObscured);
    const RefPtr<TextInputResponseArea>& GetResponseArea()
    {
        return responseArea_;
    }

    const RefPtr<TextInputResponseArea>& GetCleanNodeResponseArea()
    {
        return cleanNodeResponseArea_;
    }

    bool IsShowUnit() const;
    bool IsShowPasswordIcon() const;
    std::optional<bool> IsShowPasswordText() const;
    bool IsInPasswordMode() const;
    bool IsShowCancelButtonMode() const;
    void CheckPasswordAreaState();

    bool GetShowSelect() const
    {
        return showSelect_;
    }

    void ShowSelect()
    {
        showSelect_ = true;
    }

    void FocusForwardStopTwinkling();
    bool UpdateFocusForward();

    bool UpdateFocusBackward();

    bool HandleSpaceEvent();

    virtual void ApplyNormalTheme();
    void ApplyUnderlineTheme();
    void ApplyInlineTheme();

    int32_t GetContentWideTextLength() override
    {
        return static_cast<int32_t>(contentController_->GetTextUtf16Value().length());
    }

    void HandleOnShowMenu() override
    {
        selectOverlay_->HandleOnShowMenu();
    }
    bool HasFocus() const;
    void StopTwinkling();
    void StartTwinkling();

    bool IsModifyDone()
    {
        return isModifyDone_;
    }
    void SetModifyDoneStatus(bool value)
    {
        isModifyDone_ = value;
    }

    const TimeStamp& GetLastClickTime()
    {
        return lastClickTimeStamp_;
    }

    void CheckTextAlignByDirection(TextAlign& textAlign, TextDirection direction);

    void HandleOnDragStatusCallback(
        const DragEventType& dragEventType, const RefPtr<NotifyDragEvent>& notifyDragEvent) override;

    void GetCaretMetrics(CaretMetricsF& caretCaretMetric) override;

    OffsetF GetTextPaintOffset() const override;

    OffsetF GetPaintRectGlobalOffset() const;

    void NeedRequestKeyboard()
    {
        SetNeedToRequestKeyboardInner(true, RequestKeyboardInnerChangeReason::SEARCH_FOCUS);
    }

    void SetNeedToRequestKeyboardInner(bool needToRequestKeyboardInner,
        RequestKeyboardInnerChangeReason reason = RequestKeyboardInnerChangeReason::UNKNOWN);

    void CleanNodeResponseKeyEvent();

    void ScrollPage(bool reverse, bool smooth = false,
        AccessibilityScrollType scrollType = AccessibilityScrollType::SCROLL_FULL) override;
    void InitScrollBarClickEvent() override {}
    void ClearTextContent();
    bool IsUnderlineMode() const;
    bool IsInlineMode() const;
    bool IsShowError();
    bool IsShowCount();
    void ResetContextAttr();
    void RestoreDefaultMouseState();

    inline void RegisterWindowSizeCallback()
    {
        auto host = GetHost();
        // call RegisterWindowSizeCallbackMultiThread() by multi thread
        FREE_NODE_CHECK(host, RegisterWindowSizeCallback);
        if (isOritationListenerRegisted_) {
            return;
        }
        isOritationListenerRegisted_ = true;
        CHECK_NULL_VOID(host);
        auto pipeline = host->GetContext();
        CHECK_NULL_VOID(pipeline);
        pipeline->AddWindowSizeChangeCallback(host->GetId());
    }

    void RegisterWindowSizeCallbackMultiThread();
    void RegisterWindowSizeCallbackMultiThreadAction();

    void OnWindowSizeChanged(int32_t width, int32_t height, WindowSizeChangeReason type) override;

    bool IsTransparent()
    {
        return isTransparent_;
    }

    RefPtr<Clipboard> GetClipboard() override
    {
        if (!clipboard_) {
            auto host = GetHost();
            CHECK_NULL_RETURN(host, clipboard_);
            auto context = host->GetContext();
            CHECK_NULL_RETURN(context, clipboard_);
            clipboard_ = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
        }
        return clipboard_;
    }

    const Dimension& GetAvoidSoftKeyboardOffset() const override;

    RectF GetPaintContentRect() override
    {
        auto transformContentRect = contentRect_;
        selectOverlay_->GetLocalRectWithTransform(transformContentRect);
        return transformContentRect;
    }

    bool ProcessAutoFill(bool& isPopup, bool isFromKeyBoard = false, bool isNewPassWord = false);
    void SetAutoFillUserName(const std::string& userName)
    {
        autoFillUserName_ = userName;
    }

    std::string GetAutoFillUserName()
    {
        return autoFillUserName_;
    }

    std::string GetAutoFillNewPassword()
    {
        return autoFillNewPassword_;
    }

    void SetAutoFillNewPassword(const std::string& newPassword)
    {
        autoFillNewPassword_ = newPassword;
    }
    void SetAutoFillOtherAccount(bool otherAccount)
    {
        autoFillOtherAccount_ = otherAccount;
    }

    std::vector<RectF> GetPreviewTextRects() const;

    bool GetIsPreviewText() const
    {
        return hasPreviewText_;
    }

    const Color& GetPreviewDecorationColor() const
    {
        auto theme = GetTheme();
        CHECK_NULL_RETURN(theme, Color::TRANSPARENT);
        return theme->GetPreviewUnderlineColor();
    }

    bool NeedDrawPreviewText();

    float GetPreviewUnderlineWidth() const
    {
        return static_cast<float>(previewUnderlineWidth_.ConvertToPx());
    }

    void ReceivePreviewTextStyle(const std::string& style) override;

    PreviewTextStyle GetPreviewTextStyle() const;

    RefPtr<UINode> GetCustomKeyboard()
    {
        return customKeyboard_;
    }

    bool GetCustomKeyboardOption()
    {
        return keyboardAvoidance_;
    }

    void SetShowKeyBoardOnFocus(bool value);
    void SetShowKeyBoardOnFocusMultiThread(bool value);
    bool GetShowKeyBoardOnFocus()
    {
        return showKeyBoardOnFocus_;
    }

    void OnSelectionMenuOptionsUpdate(const NG::OnCreateMenuCallback&& onCreateMenuCallback,
        const NG::OnMenuItemClickCallback&& onMenuItemClick, const NG::OnPrepareMenuCallback&& onPrepareMenuCallback);

    void OnCreateMenuCallbackUpdate(const NG::OnCreateMenuCallback&& onCreateMenuCallback)
    {
        selectOverlay_->OnCreateMenuCallbackUpdate(std::move(onCreateMenuCallback));
    }

    void OnMenuItemClickCallbackUpdate(const NG::OnMenuItemClickCallback&& onMenuItemClick)
    {
        selectOverlay_->OnMenuItemClickCallbackUpdate(std::move(onMenuItemClick));
    }

    void OnPrepareMenuCallbackUpdate(const NG::OnPrepareMenuCallback&& onPrepareMenuCallback)
    {
        selectOverlay_->OnPrepareMenuCallbackUpdate(std::move(onPrepareMenuCallback));
    }

    void SetSupportPreviewText(bool isSupported)
    {
        hasSupportedPreviewText_ = isSupported;
    }

    bool GetSupportPreviewText() const
    {
        return hasSupportedPreviewText_;
    }

    int32_t GetPreviewTextStart() const
    {
        return hasPreviewText_ ? previewTextStart_ : selectController_->GetCaretIndex();
    }

    int32_t GetPreviewTextEnd() const
    {
        return hasPreviewText_ ? previewTextEnd_ : selectController_->GetCaretIndex();
    }

    std::u16string GetPreviewTextValue() const
    {
        return contentController_->GetSelectedValue(GetPreviewTextStart(), GetPreviewTextEnd());
    }

    std::u16string GetBodyTextValue() const
    {
        return hasPreviewText_ ? bodyTextInPreivewing_ : GetTextUtf16Value();
    }

    bool IsPressSelectedBox()
    {
        return isPressSelectedBox_;
    }

    int32_t CheckPreviewTextValidate(const std::string& previewValue, const PreviewRange range) override;
    int32_t CheckPreviewTextValidate(const std::u16string& previewValue, const PreviewRange range) override;
    void HiddenMenu();

    void OnFrameNodeChanged(FrameNodeChangeInfoFlag flag) override
    {
        selectOverlay_->OnAncestorNodeChanged(flag);
    }

    void GetSelectIndex(int32_t& start, int32_t& end) const override
    {
        start = selectController_->GetStartIndex();
        end = selectController_->GetEndIndex();
    }

    void SetTextChangedAtCreation(bool changed)
    {
        isTextChangedAtCreation_ = changed;
    }

    void SetIsPasswordSymbol(bool isPasswordSymbol)
    {
        isPasswordSymbol_ = isPasswordSymbol;
    }

    bool IsShowPasswordSymbol() const
    {
        return isPasswordSymbol_ && Container::GreatOrEqualAPITargetVersion(PlatformVersion::VERSION_THIRTEEN);
    }

    bool IsResponseRegionExpandingNeededForStylus(const TouchEvent& touchEvent) const override;

    RectF ExpandDefaultResponseRegion(RectF& rect) override;

    void UpdateParentGlobalOffset()
    {
        parentGlobalOffset_ = GetPaintRectGlobalOffset();
    }

    PositionWithAffinity GetGlyphPositionAtCoordinate(int32_t x, int32_t y) override;

    bool InsertOrDeleteSpace(int32_t index) override;

    void DeleteRange(int32_t start, int32_t end, bool isIME = true) override;

    void DeleteTextRange(int32_t start, int32_t end, TextDeleteDirection direction);

    bool SetCaretOffset(int32_t caretPostion) override;

    const RefPtr<MultipleClickRecognizer>& GetOrCreateMultipleClickRecognizer()
    {
        if (!multipleClickRecognizer_) {
            multipleClickRecognizer_ = MakeRefPtr<MultipleClickRecognizer>();
        }
        return multipleClickRecognizer_;
    }

    void SetAdaptFontSize(const std::optional<Dimension>& adaptFontSize)
    {
        adaptFontSize_ = adaptFontSize;
    }

    void ShowCaretAndStopTwinkling();

    void TriggerAvoidOnCaretChange();

    void TriggerAvoidWhenCaretGoesDown();

    bool CheckIfNeedAvoidOnCaretChange(float caretPos);

    bool IsTextEditableForStylus() const override;
    bool IsHandleDragging();
    bool IsLTRLayout()
    {
        auto host = GetHost();
        CHECK_NULL_RETURN(host, true);
        return host->GetLayoutProperty()->GetNonAutoLayoutDirection() == TextDirection::LTR;
    }

    std::optional<float> GetLastCaretPos() const
    {
        return lastCaretPos_;
    }

    void SetLastCaretPos(float lastCaretPos)
    {
        lastCaretPos_ = lastCaretPos;
    }

    void SetEnableHapticFeedback(bool isEnabled)
    {
        isEnableHapticFeedback_ = isEnabled;
    }

    bool GetEnableHapticFeedback() const
    {
        return isEnableHapticFeedback_;
    }

    void SetIsFocusedBeforeClick(bool isFocusedBeforeClick)
    {
        isFocusedBeforeClick_ = isFocusedBeforeClick;
    }

    void StartVibratorByIndexChange(int32_t currentIndex, int32_t preIndex);
    virtual void ProcessSelection();
    void AfterLayoutProcessCleanResponse(
        const RefPtr<CleanNodeResponseArea>& cleanNodeResponseArea);
    void StopContentScroll();
    void UpdateContentScroller(const Offset& localOffset, float delay = 0.0f);
    Offset AdjustAutoScrollOffset(const Offset& offset);
    void SetIsInitTextRect(bool isInitTextRect)
    {
        initTextRect_ = isInitTextRect;
    }

    virtual float FontSizeConvertToPx(const Dimension& fontSize);

    SelectionInfo GetSelection();

    bool GetContentScrollerIsScrolling() const
    {
        return contentScroller_.isScrolling;
    }

    void SetTextFadeoutCapacity(bool enabled)
    {
        haveTextFadeoutCapacity_ = enabled;
    }
    bool GetTextFadeoutCapacity()
    {
        return haveTextFadeoutCapacity_;
    }

    void SetHoverPressBgColorEnabled(bool enabled)
    {
        hoverAndPressBgColorEnabled_ = enabled;
    }

    bool GetOriginCaretPosition(OffsetF& offset) const;
    void ResetOriginCaretPosition() override;
    bool RecordOriginCaretPosition() override;
    void AddInsertCommand(const std::u16string& insertValue, InputReason reason);
    void AddInputCommand(const InputCommandInfo& inputCommandInfo);
    void ExecuteInputCommand(const InputCommandInfo& inputCommandInfo);
    void FilterInitializeText();
    void SetIsFilterChanged(bool isFilterChanged)
    {
        isFilterChanged_ = isFilterChanged;
    }
    bool GetCancelButtonTouchInfo()
    {
        return cancelButtonTouched_;
    }

    bool IsUnderlineAndButtonMode() const
    {
        auto layoutProperty = GetLayoutProperty<TextFieldLayoutProperty>();
        CHECK_NULL_RETURN(layoutProperty, false);
        auto cleanNodeStyle = layoutProperty->GetCleanNodeStyle().value_or(CleanNodeStyle::INPUT);
        auto isCancelMode = IsShowCancelButtonMode() && !(cleanNodeStyle == CleanNodeStyle::INVISIBLE);
        return IsUnderlineMode() && (isCancelMode || IsInPasswordMode());
    }

    void SetKeyboardAppearanceConfig(const KeyboardAppearanceConfig& config)
    {
        imeGradientMode_ = config.gradientMode;
        imeFluidLightMode_ = config.fluidLightMode;
    }

    void UpdatePropertyImpl(const std::string& key, RefPtr<PropertyValueBase> value) override;
    void UpdateBorderResource() override;
    void UpdateMarginResource() override;
    void SetBackBorderRadius();
    void OnColorModeChange(uint32_t colorMode) override;

    void ProcessDefaultStyleAndBehaviors();
    void ProcessDefaultStyleAndBehaviorsMultiThread();

    void ProcessResponseArea();
protected:
    virtual void InitDragEvent();
    void OnAttachToMainTree() override;
    void OnAttachToMainTreeMultiThread();
    void OnAttachToMainTreeMultiThreadAddition();

    void OnDetachFromMainTree() override;
    void OnDetachFromMainTreeMultiThread();
    
    bool IsReverse() const override
    {
        return false;
    }

    bool SelectOverlayIsOn()
    {
        return selectOverlay_->SelectOverlayIsOn();
    }

    void SetIsSingleHandle(bool isSingleHandle)
    {
        selectOverlay_->SetIsSingleHandle(isSingleHandle);
    }

    int32_t GetTouchIndex(const OffsetF& offset) override;
    void OnTextGestureSelectionUpdate(int32_t start, int32_t end, const TouchEventInfo& info) override;
    void OnTextGestureSelectionEnd(const TouchLocationInfo& locationInfo) override;
    void DoTextSelectionTouchCancel() override;
    void StartGestureSelection(int32_t start, int32_t end, const Offset& startOffset) override;
    void UpdateSelection(int32_t both);
    void UpdateSelection(int32_t start, int32_t end);
    virtual bool IsNeedProcessAutoFill();
    bool OnThemeScopeUpdate(int32_t themeScopeId) override;

    RefPtr<ContentController> contentController_;
    RefPtr<TextSelectController> selectController_;
    bool needToRefreshSelectOverlay_ = false;
    bool isTextChangedAtCreation_ = false;

    void HandleCloseKeyboard(bool forceClose);

    // 是否独立控制键盘
    bool independentControlKeyboard_ = false;
    RefPtr<AutoFillController> autoFillController_;
    virtual IMEClient GetIMEClientInfo();
    RefPtr<TextFieldSelectOverlay> selectOverlay_;

private:
    Offset ConvertTouchOffsetToTextOffset(const Offset& touchOffset);
    void GetTextSelectRectsInRangeAndWillChange();
    bool BeforeIMEInsertValue(const std::u16string& insertValue, int32_t offset);
    void AfterIMEInsertValue(const std::u16string& insertValue);
    bool BeforeIMEDeleteValue(const std::u16string& deleteValue, TextDeleteDirection direction, int32_t offset);
    void AfterIMEDeleteValue(const std::u16string& deleteValue, TextDeleteDirection direction);

    bool FireOnWillChange(const ChangeValueInfo& changeValueInfo);
    bool OnWillChangePreSetValue(const std::u16string& newValue);
    bool OnWillChangePreDelete(const std::u16string& oldContent, uint32_t start, uint32_t end);
    bool OnWillChangePreInsert(const std::u16string& insertValue, const std::u16string& oldContent,
        uint32_t start, uint32_t end);
    void RecoverTextValueAndCaret(const std::u16string& oldValue, TextRange caretIndex);
    void OnAfterModifyDone() override;
    void HandleTouchEvent(const TouchEventInfo& info);
    void HandleTouchDown(const Offset& offset);
    void HandleTouchUp();
    void HandleCancelButtonTouchDown(const RefPtr<TextInputResponseArea>& responseArea);
    void HandleCancelButtonTouchUp();
    void HandleTouchMove(const TouchLocationInfo& info);
    void UpdateCaretByTouchMove(const TouchLocationInfo& info);
    void InitDisableColor();
    void InitFocusEvent();
    void InitTouchEvent();
    void InitLongPressEvent();
    void InitClickEvent();
    void InitDragDropEvent();
    bool ProcessFocusIndexAction();
    std::function<DragDropInfo(const RefPtr<OHOS::Ace::DragEvent>&, const std::string&)> OnDragStart();
    std::function<void(const RefPtr<OHOS::Ace::DragEvent>&, const std::string&)> OnDragDrop();
    std::string GetDragStyledText();
    void ShowSelectAfterDragEvent();
    void ClearDragDropEvent();
    std::function<void(Offset)> GetThumbnailCallback();
    bool HasStateStyle(UIState state) const;

    void OnTextInputScroll(float offset);
    void OnTextAreaScroll(float offset);
    bool OnScrollCallback(float offset, int32_t source) override;
    void OnScrollEndCallback() override;
    bool CheckSelectAreaVisible();
    void InitMouseEvent();
    void InitCancelButtonMouseEvent();
    void InitPasswordButtonMouseEvent();
    void HandleHoverEffect(MouseInfo& info, bool isHover);
    void OnHover(bool isHover, const HoverInfo& info);
    void UpdateHoverStyle(bool isHover);
    void UpdatePressStyle(bool isPressed);
    void PlayAnimationHoverAndPress(const Color& color);
    void UpdateTextFieldBgColor(const Color& color);
    void ChangeMouseState(const Offset location, int32_t frameId);
    void FreeMouseStyleHoldNode(const Offset location);
    void HandleMouseEvent(MouseInfo& info);
    void FocusAndUpdateCaretByMouse(MouseInfo& info);
    void UpdateShiftFlag(const KeyEvent& keyEvent) override;
    void UpdateCaretByClick(const Offset& localOffset);
    void HandleRightMouseEvent(MouseInfo& info);
    void HandleRightMousePressEvent(MouseInfo& info);
    void HandleRightMouseReleaseEvent(MouseInfo& info);
    void HandleLeftMouseEvent(MouseInfo& info);
    void HandleLeftMousePressEvent(MouseInfo& info);
    void HandleLeftMouseMoveEvent(MouseInfo& info);
    void HandleLeftMouseReleaseEvent(MouseInfo& info);
    void StartVibratorByLongPress();
    bool IsInResponseArea(const Offset& location);
    void HandleLongPress(GestureEvent& info);
    bool CanChangeSelectState();
    void UpdateCaretPositionWithClamp(const int32_t& pos);
    void CursorMoveOnClick(const Offset& offset);

    void DelayProcessOverlay(const OverlayRequest& request = OverlayRequest());
    void CancelDelayProcessOverlay();
    void ProcessOverlayAfterLayout(const OffsetF& prevOffset);
    void ProcessOverlay(const OverlayRequest& request = OverlayRequest());

    // when moving one handle causes shift of textRect, update x position of the other handle
    void SetHandlerOnMoveDone();
    void OnDetachFromFrameNode(FrameNode* node) override;
    void OnDetachFromFrameNodeMultiThread(FrameNode* node);
    void OnAttachContext(PipelineContext* context) override;
    void OnDetachContext(PipelineContext* context) override;
    void UpdateSelectionByMouseDoubleClick();

    void AfterSelection();

    void AutoFillValueChanged();
    void FireEventHubOnChange(const std::u16string& text);
    // The return value represents whether the editor content has change.
    bool FireOnTextChangeEvent();
    void AddTextFireOnChange();
    void RecordTextInputEvent();

    void UpdateCaretPositionByLastTouchOffset();
    bool UpdateCaretPosition();
    void UpdateCaretRect(bool isEditorValueChanged);
    void AdjustTextInReasonableArea();
    bool CharLineChanged(int32_t caretPosition);

    void ScheduleCursorTwinkling();
    void OnCursorTwinkling();
    void CheckIfNeedToResetKeyboard();

    float PreferredTextHeight(bool isPlaceholder, bool isAlgorithmMeasure = false);

    void SetCaretOffsetForEmptyTextOrPositionZero();
    void UpdateTextFieldManager(const Offset& offset, float height);
    void OnTextInputActionUpdate(TextInputAction value);

    void OnAutoCapitalizationModeUpdate(AutoCapitalizationMode value);
    void Delete(int32_t start, int32_t end);
    void CheckAndUpdateRecordBeforeOperation();
    void BeforeCreateLayoutWrapper() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;
    bool CursorInContentRegion();
    bool OffsetInContentRegion(const Offset& offset);
    void SetDisabledStyle();

    void CalculateDefaultCursor();
    void RequestKeyboardByFocusSwitch();
    void TextFieldLostFocusToViewRoot();
    bool IsModalCovered();
    void SetNeedToRequestKeyboardOnFocus();
    void SetAccessibilityAction() override;
    void SetAccessibilityActionGetAndSetCaretPosition();
    void SetAccessibilityActionOverlayAndSelection();
    void SetAccessibilityEditAction();
    void SetAccessibilityMoveTextAction();
    void SetAccessibilityErrorText();
    void SetAccessibilityClearAction();
    void SetAccessibilityPasswordIconAction();
    void SetAccessibilityUnitAction();

    void UpdateCopyAllStatus();
    void RestorePreInlineStates();
    void ProcessRectPadding();
    void CalcScrollRect(Rect& inlineScrollRect);

    bool ResetObscureTickCountDown();
    bool IsAccessibilityClick();
    bool IsOnUnitByPosition(const Offset& globalOffset);
    bool IsOnPasswordByPosition(const Offset& globalOffset);
    bool IsOnCleanNodeByPosition(const Offset& globalOffset);
    bool IsTouchAtLeftOffset(float currentOffsetX);
    void FilterExistText();
    void UpdateErrorTextMargin();
    void UpdateSelectController();
    void UpdateHandlesOffsetOnScroll(float offset);
    void CloseHandleAndSelect() override;
    bool RepeatClickCaret(const Offset& offset, int32_t lastCaretIndex);
    bool RepeatClickCaret(const Offset& offset, const RectF& lastCaretRect);
    void PaintTextRect();
    void GetIconPaintRect(const RefPtr<TextInputResponseArea>& responseArea, RoundRect& paintRect);
    void GetInnerFocusPaintRect(RoundRect& paintRect);
    void GetTextInputFocusPaintRect(RoundRect& paintRect);
    void PaintResponseAreaRect();
    void PaintCancelRect();
    void PaintUnitRect();
    void PaintPasswordRect();
    bool CancelNodeIsShow()
    {
        auto cleanNodeArea = AceType::DynamicCast<CleanNodeResponseArea>(cleanNodeResponseArea_);
        CHECK_NULL_RETURN(cleanNodeArea, false);
        return cleanNodeArea->IsShow();
    }

    void InitPanEvent();

    void PasswordResponseKeyEvent();
    void UnitResponseKeyEvent();
    void ProcBorderAndUnderlineInBlurEvent();
    void ProcNormalInlineStateInBlurEvent();
    bool IsMouseOverScrollBar(const BaseEventInfo* info);

#if defined(ENABLE_STANDARD_INPUT)
    std::optional<MiscServices::TextConfig> GetMiscTextConfig() const;
    void GetInlinePositionYAndHeight(double& positionY, double& height) const;
#endif
    void NotifyOnEditChanged(bool isChanged);
    void ProcessCancelButton();
    bool HasInputOperation();
    AceAutoFillType ConvertToAceAutoFillType(TextInputType type);
    bool CheckAutoFill(bool isFromKeyBoard = false);
    void ScrollToSafeArea() const override;
    void RecordSubmitEvent() const;
    void UpdateCancelNode();
    void AdjustTextRectByCleanNode(RectF& textRect);
    void RequestKeyboardAfterLongPress();
    void UpdatePasswordModeState();
    void InitDragDropCallBack();
    void InitDragDropEventWithOutDragStart();
    void UpdateBlurReason();
    AceAutoFillType TextContentTypeToAceAutoFillType(const TextContentType& type);
    bool CheckAutoFillType(const AceAutoFillType& aceAutoFillAllType, bool isFromKeyBoard = false);
    bool GetAutoFillTriggeredStateByType(const AceAutoFillType& autoFillType);
    void SetAutoFillTriggeredStateByType(const AceAutoFillType& autoFillType);
    AceAutoFillType GetAutoFillType(bool isNeedToHitType = true);
    bool IsAutoFillPasswordType(const AceAutoFillType& autoFillType);
    void DoProcessAutoFill(SourceType sourceType = SourceType::NONE);
    void KeyboardContentTypeToInputType();
    void ProcessScroll();
    void ProcessCounter();
    void HandleParentGlobalOffsetChange();
    HintToTypeWrap GetHintType();
    HintToTypeWrap GetAutoFillTypeAndMetaData(bool isNeedToHitType = true);
    PaddingProperty GetPaddingByUserValue();
    void SetThemeAttr();
    void SetThemeBorderAttr();
    void ProcessInlinePaddingAndMargin();
    Edge GetUnderlinePadding(const RefPtr<TextFieldTheme>& theme, bool processLeftPadding,
        bool processRightPadding) const;
    Offset ConvertGlobalToLocalOffset(const Offset& globalOffset);
    void HandleCountStyle();
    void HandleDeleteOnCounterScene();
    void HandleContentSizeChange(const RectF& textRect);
    void UpdatePreviewIndex(int32_t start, int32_t end)
    {
        previewTextStart_ = start;
        previewTextEnd_ = end;
    }

    void CalculatePreviewingTextMovingLimit(const Offset& touchOffset, double& limitL, double& limitR);
    void UpdateParam(GestureEvent& info, bool shouldProcessOverlayAfterLayout);
    void OnCaretMoveDone(const TouchEventInfo& info);
    void HandleCrossPlatformInBlurEvent();
    void ModifyInnerStateInBlurEvent();

    void TwinklingByFocus();

    bool FinishTextPreviewByPreview(const std::u16string& insertValue);
    bool GetIndependentControlKeyboard();
    bool IsMoveFocusOutFromLeft(const KeyEvent& event);
    bool IsMoveFocusOutFromRight(const KeyEvent& event);

    bool GetTouchInnerPreviewText(const Offset& offset) const;
    bool IsShowMenu(const std::optional<SelectionOptions>& options, bool defaultValue);
    bool IsContentRectNonPositive();
    void ReportEvent();
    void ResetPreviewTextState();
    void CalculateBoundsRect();
    TextFieldInfo GenerateTextFieldInfo();
    void AddTextFieldInfo();
    void RemoveTextFieldInfo();
    void UpdateTextFieldInfo();
    bool IsAutoFillUserName(const AceAutoFillType& autoFillType);
    bool HasAutoFillPasswordNode();
    bool IsTriggerAutoFillPassword();

    void PauseContentScroll();
    void ScheduleContentScroll(float delay);
    void UpdateSelectionByLongPress(int32_t start, int32_t end, const Offset& localOffset);
    std::optional<float> CalcAutoScrollStepOffset(const Offset& localOffset);
    void SetDragMovingScrollback();
    float CalcScrollSpeed(float hotAreaStart, float hotAreaEnd, float point);
    std::optional<TouchLocationInfo> GetAcceptedTouchLocationInfo(const TouchEventInfo& info);
    void ResetTouchAndMoveCaretState();
    void UpdateSelectionAndHandleVisibility();
    void ResetFirstClickAfterGetFocus();
    void ProcessAutoFillOnFocus();
    bool IsStopEditWhenCloseKeyboard();
    void SetIsEnableSubWindowMenu();
    void OnReportPasteEvent(const RefPtr<FrameNode>& frameNode);
    void OnReportSubmitEvent(const RefPtr<FrameNode>& frameNode);
    void BeforeAutoFillAnimation(const std::u16string& content, const AceAutoFillType& type);
    void RemoveFillContentMap();
    bool NeedsSendFillContent();
    void UpdateSelectOverlay(const RefPtr<OHOS::Ace::TextFieldTheme>& textFieldTheme);
    void OnAccessibilityEventTextChange(const std::string& changeType, const std::string& changeString);
    void FireOnWillAttachIME();
    Offset GetCaretClickLocalOffset(const Offset& offset);
    bool ShouldSkipUpdateParagraph();
    void UpdateParagraphForDragNode(bool skipUpdate);

    RectF frameRect_;
    RectF textRect_;
    float textParagraphIndent_ = 0.0;
    RefPtr<Paragraph> paragraph_;
    InlineMeasureItem inlineMeasureItem_;

    RefPtr<ClickEvent> clickListener_;
    RefPtr<TouchEventImpl> touchListener_;
    RefPtr<TouchEventImpl> imageTouchEvent_;
    RefPtr<ScrollableEvent> scrollableEvent_;
    RefPtr<InputEvent> mouseEvent_;
    RefPtr<InputEvent> hoverEvent_;
    RefPtr<InputEvent> imageHoverEvent_;
    RefPtr<LongPressEvent> longPressEvent_;
    CursorPositionType cursorPositionType_ = CursorPositionType::NORMAL;

    // What the keyboard should appears.
    TextInputType keyboard_ = TextInputType::UNSPECIFIED;
    // Action when "enter" pressed.
    TextInputAction action_ = TextInputAction::UNSPECIFIED;
    TextDirection textDirection_ = TextDirection::LTR;
    // Used to record original caret position for "shift + up/down"
    // Less than 0 is invalid, initialized as invalid in constructor
    OffsetF originCaretPosition_;

    OffsetF parentGlobalOffset_;
    OffsetF lastTouchOffset_;
    std::optional<PaddingPropertyF> utilPadding_;

    bool setBorderFlag_ = true;
    BorderWidthProperty lastDiffBorderWidth_;
    BorderColorProperty lastDiffBorderColor_;

    HandleMoveStatus handleMoveStatus_;
    bool cursorVisible_ = false;
    bool focusEventInitialized_ = false;
    bool isMousePressed_ = false;
    bool textObscured_ = true;
    bool enableTouchAndHoverEffect_ = true;
    bool isOnHover_ = false;
    bool needToRequestKeyboardInner_ = false;
    bool needToRequestKeyboardOnFocus_ = false;
    bool isTransparent_ = false;
    bool contChange_ = false;
    bool counterChange_ = false;
    std::optional<int32_t> surfaceChangedCallbackId_;
    std::optional<int32_t> surfacePositionChangedCallbackId_;
    RefPtr<TextComponentDecorator> counterDecorator_;
    RefPtr<TextComponentDecorator> errorDecorator_;

    SelectionMode selectionMode_ = SelectionMode::NONE;
    CaretUpdateType caretUpdateType_ = CaretUpdateType::NONE;
    bool scrollable_ = true;
    bool blockPress_ = false;
    bool isPressSelectedBox_ = false;
    float previewWidth_ = 0.0f;
    float lastTextRectY_ = 0.0f;
    std::optional<DisplayMode> barState_;

    uint32_t twinklingInterval_ = 0;
    int32_t obscureTickCountDown_ = 0;
    int32_t nakedCharPosition_ = -1;
    bool obscuredChange_ = false;
    float currentOffset_ = 0.0f;
    float countHeight_ = 0.0f;
    Dimension underlineWidth_ = 1.0_px;
    Color underlineColor_;
    UserUnderlineColor userUnderlineColor_ = UserUnderlineColor();
    bool scrollBarVisible_ = false;
    bool isCounterIdealheight_ = false;
    float maxFrameOffsetY_ = 0.0f;
    float maxFrameHeight_ = 0.0f;

    CancelableCallback<void()> cursorTwinklingTask_;

    std::list<std::unique_ptr<TextInputFormatter>> textInputFormatters_;

    RefPtr<TextFieldController> textFieldController_;
    WeakPtr<Referenced> jsTextEditableController_;
    TextEditingValueNG textEditingValue_;
    // controls redraw of overlay modifier, update when need to redraw
    bool changeSelectedRects_ = false;
    RefPtr<TextFieldOverlayModifier> textFieldOverlayModifier_;
    RefPtr<TextFieldContentModifier> textFieldContentModifier_;
    RefPtr<TextFieldForegroundModifier> textFieldForegroundModifier_;
    WeakPtr<TextFieldTheme> textFieldTheme_;
    ACE_DISALLOW_COPY_AND_MOVE(TextFieldPattern);

    int32_t dragTextStart_ = 0;
    int32_t dragTextEnd_ = 0;
    std::u16string dragValue_;
    RefPtr<FrameNode> dragNode_;
    DragStatus dragStatus_ = DragStatus::NONE;          // The status of the dragged initiator
    DragStatus dragRecipientStatus_ = DragStatus::NONE; // Drag the recipient's state
    RefPtr<Clipboard> clipboard_;
    std::vector<TextEditingValueNG> operationRecords_;
    std::vector<TextEditingValueNG> redoOperationRecords_;
    std::vector<NG::MenuOptionsParam> menuOptionItems_;
    BorderRadiusProperty borderRadius_;
    PasswordModeStyle passwordModeStyle_;
    SelectMenuInfo selectMenuInfo_;

    RefPtr<PanEvent> boxSelectPanEvent_;

    // inline
    bool isTextInput_ = false;
    bool inlineSelectAllFlag_ = false;
    bool inlineFocusState_ = false;
    float inlineSingleLineHeight_ = 0.0f;
    float inlinePadding_ = 0.0f;

    bool isOritationListenerRegisted_ = false;

#if defined(ENABLE_STANDARD_INPUT)
    sptr<OHOS::MiscServices::OnTextChangedListener> textChangeListener_;
#else
    RefPtr<TextInputConnection> connection_;
#endif
#if defined(OHOS_STANDARD_SYSTEM) && !defined(PREVIEW)
    bool imeAttached_ = false;
    bool imeShown_ = false;
#endif
    bool isFocusedBeforeClick_ = false;
    bool isCustomKeyboardAttached_ = false;
    bool isCustomFont_ = false;
    BlurReason blurReason_ = BlurReason::FOCUS_SWITCH;
    std::function<void()> customKeyboardBuilder_;
    RefPtr<UINode> customKeyboard_;
    RefPtr<OverlayManager> keyboardOverlay_;
    TimeStamp lastClickTimeStamp_;
    float paragraphWidth_ = 0.0f;

    std::queue<int32_t> deleteBackwardOperations_;
    std::queue<int32_t> deleteForwardOperations_;
    std::queue<InsertCommandInfo> insertCommands_;
    std::queue<InputCommandInfo> inputCommands_;
    std::queue<InputOperation> inputOperations_;
    bool leftMouseCanMove_ = false;
    bool isLongPress_ = false;
    bool isEdit_ = false;
    bool isSupportCameraInput_ = false;
    RefPtr<NG::UINode> unitNode_;
    RefPtr<TextInputResponseArea> responseArea_;
    RefPtr<TextInputResponseArea> cleanNodeResponseArea_;
    std::string lastAutoFillTextValue_;
    std::function<void()> processOverlayDelayTask_;
    FocuseIndex focusIndex_ = FocuseIndex::TEXT;
    TouchAndMoveCaretState moveCaretState_;
    FloatingCaretState floatCaretState_;
    bool needSelectAll_ = false;
    bool isModifyDone_ = false;
    bool initTextRect_ = false;
    bool isKeyboardClosedByUser_ = false;
    bool isFillRequestFinish_ = true;
    bool keyboardAvoidance_ = false;
    bool hasMousePressed_ = false;
    bool showCountBorderStyle_ = false;
    OffsetF movingCaretOffset_;
    std::string autoFillUserName_;
    std::string autoFillNewPassword_;
    uint32_t autoFillSessionId_ = 0;
    bool autoFillOtherAccount_ = false;

    bool textInputBlurOnSubmit_ = true;
    bool textAreaBlurOnSubmit_ = false;
    bool isDetachFromMainTree_ = false;

    // 节点是否需要渐隐能力
    bool haveTextFadeoutCapacity_ = false;
    bool isFocusTextColorSet_ = false;
    bool isFocusBGColorSet_ = false;
    bool isFocusPlaceholderColorSet_ = false;
    bool hoverAndPressBgColorEnabled_ = false;
    std::function<void(bool)> isFocusActiveUpdateEvent_;

    Dimension previewUnderlineWidth_ = 2.0_vp;
    bool hasSupportedPreviewText_ = true;
    bool hasPreviewText_ = false;
    std::queue<PreviewTextInfo> previewTextOperation_;
    int32_t previewTextStart_ = -1;
    int32_t previewTextEnd_ = -1;
    std::u16string bodyTextInPreivewing_;
    PreviewRange lastCursorRange_ = {};
    std::u16string lastTextValue_ = u"";
    float lastCursorLeft_ = 0.0f;
    float lastCursorTop_ = 0.0f;
    bool showKeyBoardOnFocus_ = true;
    bool isTextSelectionMenuShow_ = true;
    bool isMoveCaretAnywhere_ = false;
    bool isTouchPreviewText_ = false;
    bool isCaretTwinkling_ = false;
    bool isPasswordSymbol_ = true;
    bool isEnableHapticFeedback_ = true;
    RefPtr<MultipleClickRecognizer> multipleClickRecognizer_ = nullptr;
    WeakPtr<AIWriteAdapter> aiWriteAdapter_;
    std::optional<Dimension> adaptFontSize_;
    uint32_t longPressFingerNum_ = 0;
    ContentScroller contentScroller_;
    WeakPtr<FrameNode> firstAutoFillContainerNode_;
    std::optional<float> lastCaretPos_ = std::nullopt;
    bool firstClickAfterLosingFocus_ = true;
    CancelableCallback<void()> firstClickResetTask_;
    RequestFocusReason requestFocusReason_ = RequestFocusReason::UNKNOWN;
    bool directionKeysMoveFocusOut_ = false;
    KeyboardAppearance keyboardAppearance_ = KeyboardAppearance::NONE_IMMERSIVE;
    TextRange callbackRangeBefore_;
    TextRange callbackRangeAfter_;
    std::u16string callbackOldContent_;
    PreviewText callbackOldPreviewText_;
    bool isFilterChanged_ = false;
    std::optional<bool> showPasswordState_;
    bool cancelButtonTouched_ = false;
    KeyboardGradientMode imeGradientMode_ = KeyboardGradientMode::NONE;
    KeyboardFluidLightMode imeFluidLightMode_ = KeyboardFluidLightMode::NONE;
    OverflowMode lastOverflowMode_ = OverflowMode::SCROLL;
    TextOverflow lastTextOverflow_ = TextOverflow::ELLIPSIS;

    // ----- multi thread state variables -----
    bool initSurfacePositionChangedCallbackMultiThread_ = false;
    bool initSurfaceChangedCallbackMultiThread_ = false;
    bool handleCountStyleMultiThread_ = false;
    bool startTwinklingMultiThread_ = false;
    bool registerWindowSizeCallbackMultiThread_ = false;
    bool processDefaultStyleAndBehaviorsMultiThread_ = false;
    bool stopEditingMultiThread_ = false;
    bool triggerAvoidOnCaretChangeMultiThread_ = false;
    bool updateCaretInfoToControllerMultiThread_ = false;
    bool setShowKeyBoardOnFocusMultiThread_ = false;
    bool setShowKeyBoardOnFocusMultiThreadValue_ = false;
    bool setSelectionFlagMultiThread_ = false;
    bool setCustomKeyboardWithNodeMultiThread_ = false;
    RefPtr<UINode> setCustomKeyboardWithNodeMultiThreadValue_;
    // ----- multi thread state variables end -----
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_FIELD_TEXT_FIELD_PATTERN_H

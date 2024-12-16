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

#include "core/components_ng/pattern/text/text_pattern.h"

#include <cstdint>
#include <iterator>
#include <stack>
#include <string>

#include "adapter/ohos/capability/clipboard/clipboard_impl.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/point_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/geometry/offset.h"
#include "base/log/dump_log.h"
#include "base/log/log_wrapper.h"
#include "base/utils/utf_helper.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "base/window/drag_window.h"
#include "core/common/ace_engine_ext.h"
#include "core/common/ai/data_detector_mgr.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/common/font_manager.h"
#include "core/common/recorder/node_data_cache.h"
#include "core/common/udmf/udmf_client.h"
#include "core/common/vibrator/vibrator_utils.h"
#include "core/components/common/properties/text_style_parser.h"
#include "core/components_ng/gestures/recognizers/gesture_recognizer.h"
#include "core/components_ng/pattern/rich_editor_drag/rich_editor_drag_pattern.h"
#include "core/text/text_emoji_processor.h"
#ifdef ENABLE_ROSEN_BACKEND
#include "core/components/custom_paint/rosen_render_custom_paint.h"
#endif

namespace OHOS::Ace::NG {
namespace {
constexpr double DIMENSION_VALUE = 16.0;
constexpr char COPY[] = "copy";
constexpr char SELECT_TEXT[] = "selectText";
constexpr const char SYMBOL_COLOR[] = "BLACK";
constexpr int32_t API_PROTEXTION_GREATER_NINE = 9;
const std::u16string SYMBOL_TRANS = u"\uF0001";
const std::u16string WIDE_NEWLINE = u"\n";
constexpr float RICH_DEFAULT_SHADOW_COLOR = 0x33000000;
constexpr float RICH_DEFAULT_ELEVATION = 120.0f;
}; // namespace

TextPattern::~TextPattern()
{
    // node destruct, need to stop text race animation
    CHECK_NULL_VOID(contentMod_);
    contentMod_->StopTextRace();
}

void TextPattern::OnWindowHide()
{
    if (magnifierController_) {
        magnifierController_->RemoveMagnifierFrameNode();
    }
    CHECK_NULL_VOID(contentMod_);
    contentMod_->PauseAnimation();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    TAG_LOGD(AceLogTag::ACE_TEXT, "OnWindowHide [%{public}d]", host->GetId());
}

void TextPattern::OnWindowShow()
{
    CHECK_NULL_VOID(contentMod_);
    contentMod_->ResumeAnimation();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    TAG_LOGD(AceLogTag::ACE_TEXT, "OnWindowShow [%{public}d]", host->GetId());
}

void TextPattern::OnAttachToFrameNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    pipeline_ = pipeline;
    auto fontManager = pipeline->GetFontManager();
    if (fontManager) {
        fontManager->AddFontNodeNG(host);
    }
    if (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        if (pipeline->GetMinPlatformVersion() > API_PROTEXTION_GREATER_NINE) {
            host->GetRenderContext()->UpdateClipEdge(true);
            host->GetRenderContext()->SetClipToFrame(true);
        }
    }
    InitSurfaceChangedCallback();
    InitSurfacePositionChangedCallback();
    pipeline->AddWindowStateChangedCallback(host->GetId());
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    textLayoutProperty->UpdateTextAlign(TextAlign::START);
    textLayoutProperty->UpdateAlignment(Alignment::CENTER_LEFT);
}

void TextPattern::OnDetachFromFrameNode(FrameNode* node)
{
    dataDetectorAdapter_->aiDetectDelayTask_.Cancel();
    CloseSelectOverlay();
    auto pipeline = pipeline_.Upgrade();
    CHECK_NULL_VOID(pipeline);
    if (HasSurfaceChangedCallback()) {
        pipeline->UnregisterSurfaceChangedCallback(surfaceChangedCallbackId_.value_or(-1));
    }
    if (HasSurfacePositionChangedCallback()) {
        pipeline->UnregisterSurfacePositionChangedCallback(surfacePositionChangedCallbackId_.value_or(-1));
    }
    auto frameNode = WeakClaim(node);
    pipeline->RemoveFontNodeNG(frameNode);
    auto fontManager = pipeline->GetFontManager();
    if (fontManager) {
        fontManager->UnRegisterCallbackNG(frameNode);
        fontManager->RemoveVariationNodeNG(frameNode);
    }
    pipeline->RemoveOnAreaChangeNode(node->GetId());
    pipeline->RemoveWindowStateChangedCallback(node->GetId());
    pipeline->RemoveVisibleAreaChangeNode(node->GetId());
}

void TextPattern::CloseSelectOverlay()
{
    CloseSelectOverlay(false);
}

void TextPattern::CloseSelectOverlay(bool animation)
{
    // Deprecated use selectOverlay_ instead.
    if (selectOverlayProxy_ && !selectOverlayProxy_->IsClosed()) {
        selectOverlayProxy_->Close(animation);
        RemoveAreaChangeInner();
    }
    selectOverlay_->CloseOverlay(animation, CloseReason::CLOSE_REASON_NORMAL);
}

void TextPattern::ResetSelection()
{
    if (textSelector_.IsValid() && !shiftFlag_) {
        HandleSelectionChange(-1, -1);
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    }
}

void TextPattern::InitSelection(const Offset& pos)
{
    auto selectionOffset = pos;
    if (GreatNotEqual(selectionOffset.GetY(), pManager_->GetHeight())) {
        selectionOffset.SetX(contentRect_.Width());
        selectionOffset.SetY(pManager_->GetHeight());
    }
    int32_t extend = pManager_->GetGlyphIndexByCoordinate(selectionOffset, true);
    if (IsLineBreakOrEndOfParagraph(extend)) {
        extend--;
    }
    int32_t start = 0;
    int32_t end = 0;
    if (!pManager_->GetWordBoundary(extend, start, end)) {
        start = extend;
        end = std::min(static_cast<int32_t>(textForDisplay_.length()) + placeholderCount_,
            extend + GetGraphemeClusterLength(textForDisplay_, extend));
    }
    HandleSelectionChange(start, end);
}

bool TextPattern::IsLineBreakOrEndOfParagraph(int32_t pos) const
{
    CHECK_NULL_RETURN(pos < static_cast<int32_t>(textForDisplay_.length() + placeholderCount_), true);
    CHECK_NULL_RETURN(textForDisplay_[pos] == WIDE_NEWLINE[0], false);
    return true;
}

void TextPattern::CalcCaretMetricsByPosition(int32_t extent, CaretMetricsF& caretCaretMetric, TextAffinity textAffinity)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto rect = host->GetGeometryNode()->GetFrameRect();
    CHECK_NULL_VOID(pManager_);
    auto computeSuccess = pManager_->CalcCaretMetricsByPosition(extent, caretCaretMetric, textAffinity);
    if (!computeSuccess) {
        caretCaretMetric = CaretMetricsF(OffsetF(0.0f, rect.Height()), 0.0f);
    }
}

void TextPattern::CalculateHandleOffsetAndShowOverlay(bool isUsingMouse)
{
    parentGlobalOffset_ = GetParentGlobalOffset();
    auto textContentGlobalOffset = selectOverlay_->GetHandleGlobalOffset() + contentRect_.GetOffset();
    auto paragraphPaintOffset = textContentGlobalOffset - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));

    // calculate firstHandleOffset, secondHandleOffset and handlePaintSize
    CaretMetricsF firstHandleMetrics;
    CaretMetricsF secondHandleMetrics;
    CalcCaretMetricsByPosition(textSelector_.baseOffset, firstHandleMetrics, TextAffinity::DOWNSTREAM);
    CalcCaretMetricsByPosition(textSelector_.destinationOffset, secondHandleMetrics, TextAffinity::UPSTREAM);
    OffsetF firstHandleOffset = firstHandleMetrics.offset + paragraphPaintOffset;
    OffsetF secondHandleOffset = secondHandleMetrics.offset + paragraphPaintOffset;

    textSelector_.selectionBaseOffset = firstHandleOffset;
    textSelector_.selectionDestinationOffset = secondHandleOffset;

    RectF firstHandle;
    firstHandle.SetOffset(firstHandleOffset);
    firstHandle.SetSize({ SelectHandleInfo::GetDefaultLineWidth().ConvertToPx(), firstHandleMetrics.height });
    firstHandle.SetOffset(OffsetF(firstHandle.GetX() - firstHandle.Width() / 2.0f, firstHandle.GetY()));
    textSelector_.firstHandle = firstHandle;

    RectF secondHandle;
    secondHandle.SetOffset(secondHandleOffset);
    secondHandle.SetSize({ SelectHandleInfo::GetDefaultLineWidth().ConvertToPx(), secondHandleMetrics.height });
    secondHandle.SetHeight(secondHandleMetrics.height);
    secondHandle.SetOffset(OffsetF(secondHandle.GetX() - secondHandle.Width() / 2.0f, secondHandle.GetY()));
    textSelector_.secondHandle = secondHandle;
}

std::list<ResultObject> TextPattern::GetSpansInfoInStyledString(int32_t start, int32_t end)
{
    std::list<ResultObject> resultObjects;
    int32_t imageIndex = 0;
    for (const auto& item : spans_) {
        auto obj = item->GetSpanResultObject(start, end);
        if (obj.type == SelectSpanType::TYPEIMAGE) {
            obj.spanPosition.spanIndex = imageIndex;
            ++imageIndex;
        }
        if (obj.isInit) {
            resultObjects.emplace_back(obj);
        }
    }
    return resultObjects;
}

SelectionInfo TextPattern::GetSpansInfo(int32_t start, int32_t end, GetSpansMethod method)
{
    int32_t index = 0;
    std::int32_t realEnd = 0;
    std::int32_t realStart = 0;
    SelectionInfo selection;
    std::list<ResultObject> resultObjects;
    auto length = GetTextContentLength();
    if (method == GetSpansMethod::GETSPANS) {
        realStart = (start == -1) ? 0 : start;
        realEnd = (end == -1) ? length : end;
        if (realStart > realEnd) {
            std::swap(realStart, realEnd);
        }
        realStart = std::max(0, realStart);
        realEnd = std::min(length, realEnd);
    } else if (method == GetSpansMethod::ONSELECT) {
        realEnd = std::min(length, end);
        realStart = std::min(length, start);
    }
    selection.SetSelectionEnd(realEnd);
    selection.SetSelectionStart(realStart);
    // Verify that realStart, realEnd, and spans_ are valid
    if (realStart > length || realEnd < 0 || spans_.empty() || (start > length && end > length) ||
        (method == GetSpansMethod::ONSELECT && realStart == realEnd)) {
        selection.SetResultObjectList(resultObjects);
        return selection;
    }
    if (isSpanStringMode_) {
        auto result = GetSpansInfoInStyledString(realStart, realEnd);
        selection.SetResultObjectList(result);
        return selection;
    }
    const auto& children = GetAllChildren();
    for (const auto& uinode : children) {
        if (uinode->GetTag() == V2::IMAGE_ETS_TAG) {
            ResultObject resultObject = GetImageResultObject(uinode, index, realStart, realEnd);
            if (!resultObject.valueString.empty() || resultObject.valuePixelMap) {
                resultObjects.emplace_back(resultObject);
            }
        } else if (uinode->GetTag() == V2::SPAN_ETS_TAG) {
            ResultObject resultObject = GetTextResultObject(uinode, index, realStart, realEnd);
            if (!resultObject.valueString.empty()) {
                resultObjects.emplace_back(resultObject);
            }
        } else if (uinode->GetTag() == V2::SYMBOL_SPAN_ETS_TAG) {
            ResultObject resultObject = GetSymbolSpanResultObject(uinode, index, realStart, realEnd);
            if (!resultObject.valueString.empty()) {
                resultObjects.emplace_back(resultObject);
            }
        } else if (uinode->GetTag() == V2::PLACEHOLDER_SPAN_ETS_TAG ||
            uinode->GetTag() == V2::CUSTOM_SPAN_NODE_ETS_TAG) {
            ResultObject resultObject = GetBuilderResultObject(uinode, index, realStart, realEnd);
            if (!resultObject.valueString.empty()) {
                resultObjects.emplace_back(resultObject);
            }
        }
        index++;
    }
    selection.SetResultObjectList(resultObjects);
    return selection;
}

int32_t TextPattern::GetTextContentLength()
{
    if (!spans_.empty()) {
        return static_cast<int32_t>(textForDisplay_.length()) + placeholderCount_;
    }
    return 0;
}

void TextPattern::StartVibratorByLongPress()
{
    CHECK_NULL_VOID(isEnableHapticFeedback_);
    VibratorUtils::StartVibraFeedback("longPress.light");
}

void TextPattern::HandleLongPress(GestureEvent& info)
{
    HandleSpanLongPressEvent(info);
    if (!IsSelectableAndCopy() || isMousePressed_ || selectOverlay_->GetIsHandleDragging()) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto hub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    auto localOffset = info.GetLocalLocation();
    if (selectOverlay_->HasRenderTransform()) {
        localOffset = ConvertGlobalToLocalOffset(info.GetGlobalLocation());
    }

    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    if ((textLayoutProperty && textLayoutProperty->GetMaxLines() != 0) && textForDisplay_.length() != 0) {
        StartVibratorByLongPress();
    }

    if (IsDraggable(localOffset)) {
        dragBoxes_ = GetTextBoxes();
        // prevent long press event from being triggered when dragging
        gestureHub->SetIsTextDraggable(true);
        return;
    }
    gestureHub->SetIsTextDraggable(false);
    auto textPaintOffset = contentRect_.GetOffset() - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));
    Offset textOffset = { localOffset.GetX() - textPaintOffset.GetX(), localOffset.GetY() - textPaintOffset.GetY() };
    InitSelection(textOffset);
    textResponseType_ = TextResponseType::LONG_PRESS;
    UpdateSelectionSpanType(std::min(textSelector_.baseOffset, textSelector_.destinationOffset),
        std::max(textSelector_.baseOffset, textSelector_.destinationOffset));
    oldSelectedType_ = selectedType_.value_or(TextSpanType::NONE);
    parentGlobalOffset_ = GetParentGlobalOffset();
    CalculateHandleOffsetAndShowOverlay();
    CloseSelectOverlay(true);
    if (GetOrCreateMagnifier() && HasContent()) {
        magnifierController_->SetLocalOffset({ localOffset.GetX(), localOffset.GetY() });
    }
    StartGestureSelection(textSelector_.GetStart(), textSelector_.GetEnd(), localOffset);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

bool TextPattern::ShowShadow(const PointF& textOffset, const Color& color)
{
    CHECK_NULL_RETURN(overlayMod_, false);
    CHECK_NULL_RETURN(hasUrlSpan_, false);
    CHECK_NULL_RETURN(!spans_.empty() && pManager_, false);
    int32_t start = 0;
    for (const auto& item : spans_) {
        if (!item) {
            continue;
        }
        auto selectedRects = GetSelectedRects(start, item->position);
        for (auto&& rect : selectedRects) {
            if (!rect.IsInRegion(textOffset)) {
                continue;
            }
            if (!item->urlOnRelease) {
                overlayMod_->ClearSelectedForegroundColorAndRects();
                MarkDirtySelf();
                return false;
            }
            auto inter = GetStartAndEnd(start);
            auto rects = GetSelectedRects(inter.first, inter.second);
            overlayMod_->SetSelectedForegroundColorAndRects(rects, color.GetValue());
            MarkDirtySelf();
            return true;
        }
        start = item->position;
    }
    overlayMod_->ClearSelectedForegroundColorAndRects();
    MarkDirtySelf();
    return false;
}

std::pair<int32_t, int32_t> TextPattern::GetStartAndEnd(int32_t start)
{
    auto spanBases = styledString_->GetSpans(0, styledString_->GetLength(), SpanType::Url);
    for (const auto& spanBase : spanBases) {
        if (start >= spanBase->GetStartIndex() && start < spanBase->GetEndIndex()) {
            return {spanBase->GetStartIndex(), spanBase->GetEndIndex()};
        }
    }
    return {0, 0};
}

void TextPattern::HandleSpanLongPressEvent(GestureEvent& info)
{
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    textContentRect.SetHeight(contentRect_.Height() - std::max(baselineOffset_, 0.0f));

    auto localLocation = info.GetLocalLocation();
    if (selectOverlay_->HasRenderTransform()) {
        localLocation = ConvertGlobalToLocalOffset(info.GetGlobalLocation());
    }

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    PointF textOffset = { static_cast<float>(localLocation.GetX()) - textContentRect.GetX(),
        static_cast<float>(localLocation.GetY()) - textContentRect.GetY() };
    if (renderContext->GetClipEdge().has_value() && !renderContext->GetClipEdge().value() && overlayMod_) {
        textContentRect = overlayMod_->GetBoundsRect();
        textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    }
    auto longPressFunc = [](RefPtr<SpanItem> item, GestureEvent& info, const RectF& rect,
                             const PointF& textOffset) -> bool {
        if (rect.IsInRegion(textOffset)) {
            if (item && item->onLongPress) {
                item->onLongPress(info);
            }
            return true;
        }
        return false;
    };

    if (textContentRect.IsInRegion(
        PointF(static_cast<float>(localLocation.GetX()), static_cast<float>(localLocation.GetY()))) &&
        !spans_.empty() && pManager_) {
        int32_t start = 0;
        for (const auto& item : spans_) {
            if (!item) {
                continue;
            }
            auto selectedRects = GetSelectedRects(start, item->position);
            for (auto&& rect : selectedRects) {
                CHECK_NULL_VOID(!longPressFunc(item, info, rect, textOffset));
            }
            start = item->position;
        }
    }
}

// Deprecated: Use the TextSelectOverlay::OnHandleMove() instead.
// It is currently used by RichEditorPattern.
void TextPattern::OnHandleMove(const RectF& handleRect, bool isFirstHandle)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto textContentGlobalOffset = parentGlobalOffset_ + contentRect_.GetOffset();
    auto textPaintOffset = textContentGlobalOffset - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));

    auto localOffset = handleRect.GetOffset();

    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto clip = false;
    if (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        clip = true;
    }
    if (renderContext->GetClipEdge().value_or(clip)) {
        if (localOffset.GetX() < textContentGlobalOffset.GetX()) {
            localOffset.SetX(textContentGlobalOffset.GetX());
        } else if (GreatOrEqual(localOffset.GetX(), textContentGlobalOffset.GetX() + contentRect_.Width())) {
            localOffset.SetX(textContentGlobalOffset.GetX() + contentRect_.Width());
        }

        if (localOffset.GetY() < textContentGlobalOffset.GetY()) {
            localOffset.SetY(textContentGlobalOffset.GetY());
        } else if (GreatNotEqual(localOffset.GetY(), textContentGlobalOffset.GetY() + contentRect_.Height())) {
            localOffset.SetY(textContentGlobalOffset.GetY() + contentRect_.Height());
        }
    }

    localOffset -= textPaintOffset;

    CHECK_NULL_VOID(pManager_);
    // the handle position is calculated based on the middle of the handle height.
    UpdateSelectorOnHandleMove(localOffset, handleRect.Height(), isFirstHandle);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);

    CHECK_NULL_VOID(selectOverlayProxy_);
    auto start = textSelector_.GetTextStart();
    auto end = textSelector_.GetTextEnd();
    selectOverlayProxy_->SetSelectInfo(UtfUtils::Str16ToStr8(GetSelectedText(start, end)));
}

void TextPattern::UpdateSelectorOnHandleMove(const OffsetF& localOffset, float handleHeight, bool isFirstHandle)
{
    if (isFirstHandle) {
        auto start = GetHandleIndex(Offset(
            localOffset.GetX(), localOffset.GetY() + (selectOverlayProxy_->IsHandleReverse() ? handleHeight : 0)));
        HandleSelectionChange(start, textSelector_.destinationOffset);
    } else {
        auto end = GetHandleIndex(Offset(localOffset.GetX(),
            localOffset.GetY() +
                (selectOverlayProxy_->IsHandleReverse() || NearEqual(localOffset.GetY(), 0) ? 0 : handleHeight)));
        HandleSelectionChange(textSelector_.baseOffset, end);
    }
}

bool TextPattern::IsSelectAll()
{
    return textSelector_.GetTextStart() == 0 &&
           textSelector_.GetTextEnd() == static_cast<int32_t>(textForDisplay_.length()) + placeholderCount_;
}

std::u16string TextPattern::GetSelectedText(int32_t start, int32_t end) const
{
    if (spans_.empty()) {
        auto min = std::clamp(std::max(std::min(start, end), 0), 0, static_cast<int32_t>(textForDisplay_.length()));
        auto max = std::clamp(std::min(std::max(start, end), static_cast<int32_t>(textForDisplay_.length())), 0,
            static_cast<int32_t>(textForDisplay_.length()));
        return TextEmojiProcessor::SubU16string(min, max - min, textForDisplay_);
    }
    std::u16string value;
    int32_t tag = 0;
    for (const auto& span : spans_) {
        if (span->GetSymbolUnicode() != 0) {
            tag = span->position == -1 ? tag + 1 : span->position;
            continue;
        }
        if (span->position - 1 >= start && span->placeholderIndex == -1 && span->position != -1) {
            auto wideString = span->GetSpanContent();
            auto max = std::min(span->position, end);
            auto min = std::max(start, tag);
            value += wideString.substr(std::clamp((min - tag), 0, static_cast<int32_t>(wideString.length())),
                std::clamp((max - min), 0, static_cast<int32_t>(wideString.length())));
        } else if (span->position - 1 >= start && span->position != -1) {
            // image span or custom span (span->placeholderIndex != -1)
            value += u" ";
        }
        tag = span->position == -1 ? tag + 1 : span->position;
        if (span->position >= end) {
            break;
        }
    }
    return value;
}

void TextPattern::HandleOnCopy()
{
    CHECK_NULL_VOID(clipboard_);
    if (textSelector_.IsValid() && textSelector_.GetTextStart() == textSelector_.GetTextEnd()) {
        HandleSelectionChange(-1, -1);
        return;
    }
    auto value = GetSelectedText(textSelector_.GetTextStart(), textSelector_.GetTextEnd());
    if (IsSelectableAndCopy() || dataDetectorAdapter_->hasClickedMenuOption_) {
        if (isSpanStringMode_ && !externalParagraph_) {
            HandleOnCopySpanString();
        } else if (!value.empty()) {
            HandleOnCopyWithoutSpanString(UtfUtils::Str16ToStr8(value));
        }
    }
    HiddenMenu();
    CHECK_NULL_VOID(!value.empty());
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<TextEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireOnCopy(value);
}

void TextPattern::HandleOnCopySpanString()
{
    auto subSpanString = styledString_->GetSubSpanString(textSelector_.GetTextStart(),
        textSelector_.GetTextEnd() - textSelector_.GetTextStart());
#if defined(PREVIEW)
    clipboard_->SetData(subSpanString->GetString(), copyOption_);
    return;
#endif
    RefPtr<PasteDataMix> pasteData = clipboard_->CreatePasteDataMix();
    std::vector<uint8_t> tlvData;
    subSpanString->EncodeTlv(tlvData);
    clipboard_->AddSpanStringRecord(pasteData, tlvData);
    clipboard_->AddTextRecord(pasteData, subSpanString->GetString());
    clipboard_->SetData(pasteData, copyOption_);
}

void TextPattern::HandleOnCopyWithoutSpanString(const std::string& pasteData)
{
#if defined(PREVIEW)
    clipboard_->SetData(pasteData, copyOption_);
    return;
#endif
    RefPtr<PasteDataMix> pasteDataMix = clipboard_->CreatePasteDataMix();
    auto multiTypeRecordImpl = AceType::MakeRefPtr<MultiTypeRecordImpl>();
    if (spans_.empty()) {
        EncodeTlvNoChild(pasteData, multiTypeRecordImpl->GetSpanStringBuffer());
    } else {
        EncodeTlvSpanItems(pasteData, multiTypeRecordImpl->GetSpanStringBuffer());
    }
    multiTypeRecordImpl->SetPlainText(pasteData);
    clipboard_->AddMultiTypeRecord(pasteDataMix, multiTypeRecordImpl);
    clipboard_->SetData(pasteDataMix, copyOption_);
}

#define WRITE_TLV_INHERIT(group, name, tag, type, inheritName)   \
    if ((group)->Has##name()) {                                  \
        TLVUtil::WriteUint8(buff, (tag));                        \
        TLVUtil::Write##type(buff, (group)->prop##name.value()); \
    } else if (textStyle_.has_value()) {                         \
        auto temp##name = textStyle_->Get##inheritName();        \
        TLVUtil::WriteUint8(buff, (tag));                        \
        TLVUtil::Write##type(buff, temp##name);                  \
    }

#define WRITE_TEXT_STYLE_TLV(group, name, tag, type)                 \
    do {                                                             \
        if ((group)->Has##name()) {                                  \
            TLVUtil::WriteUint8(buff, (tag));                        \
            TLVUtil::Write##type(buff, (group)->prop##name.value()); \
        }                                                            \
    } while (false)

void TextPattern::EncodeTlvNoChild(const std::string& pasteData, std::vector<uint8_t>& buff)
{
    TLVUtil::WriteUint8(buff, TLV_SPAN_STRING_SPANS);
    TLVUtil::WriteInt32(buff, 1);

    TLVUtil::WriteInt32(buff, static_cast<int32_t>(NG::SpanItemType::NORMAL));
    TLVUtil::WriteUint8(buff, TLV_SPANITEM_TAG);
    TLVUtil::WriteInt32(buff, 0);
    TLVUtil::WriteInt32(buff, pasteData.length());
    TLVUtil::WriteString(buff, pasteData);
    EncodeTlvFontStyleNoChild(buff);
    EncodeTlvTextLineStyleNoChild(buff);
    TLVUtil::WriteUint8(buff, TLV_SPANITEM_END_TAG);

    TLVUtil::WriteUint8(buff, TLV_SPAN_STRING_CONTENT);
    TLVUtil::WriteString(buff, pasteData);
    TLVUtil::WriteUint8(buff, TLV_END);
}

void TextPattern::EncodeTlvFontStyleNoChild(std::vector<uint8_t>& buff)
{
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    auto& fontStyle = textLayoutProperty->GetFontStyle();
    CHECK_NULL_VOID(fontStyle);
    WRITE_TLV_INHERIT(fontStyle, FontSize, TLV_SPAN_FONT_STYLE_FONTSIZE, Dimension, FontSize);
    WRITE_TLV_INHERIT(fontStyle, TextColor, TLV_SPAN_FONT_STYLE_TEXTCOLOR, Color, TextColor);
    WRITE_TLV_INHERIT(fontStyle, TextShadow, TLV_SPAN_FONT_STYLE_TEXTSHADOW, TextShadows, TextShadows);
    WRITE_TLV_INHERIT(fontStyle, ItalicFontStyle, TLV_SPAN_FONT_STYLE_ITALICFONTSTYLE, FontStyle, FontStyle);
    WRITE_TLV_INHERIT(fontStyle, FontWeight, TLV_SPAN_FONT_STYLE_FONTWEIGHT, FontWeight, FontWeight);
    WRITE_TLV_INHERIT(fontStyle, FontFamily, TLV_SPAN_FONT_STYLE_FONTFAMILY, FontFamily, FontFamilies);
    WRITE_TLV_INHERIT(fontStyle, FontFeature, TLV_SPAN_FONT_STYLE_FONTFEATURE, FontFeature, FontFeatures);
    WRITE_TLV_INHERIT(fontStyle, TextDecoration, TLV_SPAN_FONT_STYLE_TEXTDECORATION, TextDecoration, TextDecoration);
    WRITE_TLV_INHERIT(
        fontStyle, TextDecorationColor, TLV_SPAN_FONT_STYLE_TEXTDECORATIONCOLOR, Color, TextDecorationColor);
    WRITE_TLV_INHERIT(fontStyle, TextDecorationStyle, TLV_SPAN_FONT_STYLE_TEXTDECORATIONSTYLE, TextDecorationStyle,
        TextDecorationStyle);
    WRITE_TLV_INHERIT(fontStyle, TextCase, TLV_SPAN_FONT_STYLE_TEXTCASE, TextCase, TextCase);
    WRITE_TLV_INHERIT(fontStyle, AdaptMinFontSize, TLV_SPAN_FONT_STYLE_ADPATMINFONTSIZE, Dimension, AdaptMinFontSize);
    WRITE_TLV_INHERIT(fontStyle, AdaptMaxFontSize, TLV_SPAN_FONT_STYLE_ADPATMAXFONTSIZE, Dimension, AdaptMaxFontSize);
    WRITE_TLV_INHERIT(fontStyle, LetterSpacing, TLV_SPAN_FONT_STYLE_LETTERSPACING, Dimension, LetterSpacing);
}

void TextPattern::EncodeTlvTextLineStyleNoChild(std::vector<uint8_t>& buff)
{
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    auto& textLineStyle = textLayoutProperty->GetTextLineStyle();
    CHECK_NULL_VOID(textLineStyle);
    WRITE_TLV_INHERIT(textLineStyle, LineHeight, TLV_SPAN_TEXT_LINE_STYLE_LINEHEIGHT, Dimension, LineHeight);
    WRITE_TLV_INHERIT(textLineStyle, LineSpacing, TLV_SPAN_TEXT_LINE_STYLE_LINESPACING, Dimension, LineSpacing);
    WRITE_TLV_INHERIT(textLineStyle, TextBaseline, TLV_SPAN_TEXT_LINE_STYLE_TEXTBASELINE, TextBaseline, TextBaseline);
    WRITE_TLV_INHERIT(textLineStyle, TextOverflow, TLV_SPAN_TEXT_LINE_STYLE_TEXTOVERFLOW, TextOverflow, TextOverflow);
    WRITE_TLV_INHERIT(textLineStyle, TextAlign, TLV_SPAN_TEXT_LINE_STYLE_TEXTALIGN, TextAlign, TextAlign);
    WRITE_TEXT_STYLE_TLV(textLineStyle, MaxLength, TLV_SPAN_TEXT_LINE_STYLE_MAXLENGTH, Int32);
    WRITE_TLV_INHERIT(textLineStyle, MaxLines, TLV_SPAN_TEXT_LINE_STYLE_MAXLINES, Int32, MaxLines);
    WRITE_TEXT_STYLE_TLV(
        textLineStyle, HeightAdaptivePolicy, TLV_SPAN_TEXT_LINE_STYLE_HEIGHTADAPTIVEPOLICY, TextHeightAdaptivePolicy);
    WRITE_TLV_INHERIT(textLineStyle, TextIndent, TLV_SPAN_TEXT_LINE_STYLE_TEXTINDENT, Dimension, TextIndent);
    WRITE_TEXT_STYLE_TLV(textLineStyle, LeadingMargin, TLV_SPAN_TEXT_LINE_STYLE_LEADINGMARGIN, LeadingMargin);
    WRITE_TLV_INHERIT(textLineStyle, WordBreak, TLV_SPAN_TEXT_LINE_STYLE_WORDBREAK, WordBreak, WordBreak);
    WRITE_TLV_INHERIT(textLineStyle, LineBreakStrategy, TLV_SPAN_TEXT_LINE_STYLE_LINEBREAKSTRATEGY, LineBreakStrategy,
        LineBreakStrategy);
    WRITE_TLV_INHERIT(textLineStyle, EllipsisMode, TLV_SPAN_TEXT_LINE_STYLE_ELLIPSISMODE, EllipsisMode, EllipsisMode);
}

void TextPattern::EncodeTlvSpanItems(const std::string& pasteData, std::vector<uint8_t>& buff)
{
    auto start = textSelector_.GetTextStart();
    auto end = textSelector_.GetTextEnd();
    std::list<RefPtr<NG::SpanItem>> selectSpanItems;
    int32_t ignoreLength = 0;
    for (const auto& spanItem : spans_) {
        int32_t oldStart = spanItem->position - static_cast<int32_t>(spanItem->length);
        int32_t oldEnd = spanItem->position;
        if (oldEnd <= start || end <= oldStart) {
            continue;
        }
        if (spanItem->spanItemType == NG::SpanItemType::SYMBOL) {
            ignoreLength += static_cast<int32_t>(spanItem->length);
            continue;
        }
        auto spanStart = oldStart <= start ? 0 : oldStart - start;
        auto spanEnd = oldEnd < end ? oldEnd - start : end - start;
        auto newSpanItem = spanItem->GetSameStyleSpanItem();
        newSpanItem->interval = { spanStart - ignoreLength, spanEnd - ignoreLength };
        newSpanItem->content = spanItem->content
                .substr(std::max(start - oldStart, 0), std::min(end, oldEnd) - std::max(start, oldStart));
        selectSpanItems.emplace_back(newSpanItem);
    }

    TLVUtil::WriteUint8(buff, TLV_SPAN_STRING_SPANS);
    TLVUtil::WriteInt32(buff, selectSpanItems.size());
    for (auto it = selectSpanItems.begin(); it != selectSpanItems.end(); ++it) {
        auto spanItem = (*it);
        if (spanItem->spanItemType == NG::SpanItemType::CustomSpan) {
            TLVUtil::WriteInt32(buff, static_cast<int32_t>(NG::SpanItemType::NORMAL));
            auto placeHolderSpan = AceType::MakeRefPtr<NG::SpanItem>();
            placeHolderSpan->content = u" ";
            placeHolderSpan->interval = spanItem->interval;
            placeHolderSpan->EncodeTlv(buff);
            continue;
        }
        TLVUtil::WriteInt32(buff, static_cast<int32_t>(spanItem->spanItemType));
        spanItem->EncodeTlv(buff);
    }
    TLVUtil::WriteUint8(buff, TLV_SPAN_STRING_CONTENT);
    TLVUtil::WriteString(buff, pasteData);
    TLVUtil::WriteUint8(buff, TLV_END);
}

void TextPattern::HiddenMenu()
{
    if (IsUsingMouse()) {
        CloseSelectOverlay();
    } else {
        selectOverlay_->HideMenu();
    }
}

void TextPattern::SetTextSelection(int32_t selectionStart, int32_t selectionEnd)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto context = host->GetContext();
    if (context) {
        context->AddAfterLayoutTask([weak = WeakClaim(this), selectionStart, selectionEnd, eventHub]() {
            auto textPattern = weak.Upgrade();
            CHECK_NULL_VOID(textPattern);
            auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(textLayoutProperty);
            if (textLayoutProperty->GetCalcLayoutConstraint() &&
                textLayoutProperty->GetCalcLayoutConstraint()->selfIdealSize.has_value()) {
                auto selfIdealSizeWidth = textLayoutProperty->GetCalcLayoutConstraint()->selfIdealSize->Width();
                auto selfIdealSizeHeight = textLayoutProperty->GetCalcLayoutConstraint()->selfIdealSize->Height();
                auto constraint = textLayoutProperty->GetLayoutConstraint();
                if ((selfIdealSizeWidth.has_value() && NearZero(selfIdealSizeWidth->GetDimension().ConvertToPxWithSize(
                            constraint->percentReference.Width()))) ||
                    (selfIdealSizeHeight.has_value() &&
                        NearZero(selfIdealSizeHeight->GetDimension().ConvertToPxWithSize(
                            constraint->percentReference.Height())))) {
                    return;
                }
            }

            auto mode = textLayoutProperty->GetTextSelectableModeValue(TextSelectableMode::SELECTABLE_UNFOCUSABLE);
            if (mode == TextSelectableMode::UNSELECTABLE ||
                textLayoutProperty->GetCopyOptionValue(CopyOptions::None) == CopyOptions::None ||
                textLayoutProperty->GetTextOverflowValue(TextOverflow::CLIP) == TextOverflow::MARQUEE) {
                return;
            }
            if (!textPattern->IsSetObscured() && eventHub->IsEnabled()) {
                textPattern->ActSetSelection(selectionStart, selectionEnd);
            }
        });
    }
    host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE_SELF);
}

RefPtr<RenderContext> TextPattern::GetRenderContext()
{
    auto frameNode = GetHost();
    CHECK_NULL_RETURN(frameNode, nullptr);
    return frameNode->GetRenderContext();
}

void TextPattern::ShowSelectOverlay(const OverlayRequest& request)
{
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    if (textLayoutProperty->GetMaxLines() == 0) {
        CloseSelectOverlay();
        ResetSelection();
        return;
    }
    selectOverlay_->ProcessOverlay(request);
}

void TextPattern::HandleOnSelectAll()
{
    auto textSize = static_cast<int32_t>(textForDisplay_.length()) + placeholderCount_;
    HandleSelectionChange(0, textSize);
    CalculateHandleOffsetAndShowOverlay();
    CloseSelectOverlay(true);
    if (IsUsingMouse()) {
        if (IsSelected()) {
            selectOverlay_->SetSelectionHoldCallback();
        }
    } else {
        ShowSelectOverlay({ .animation = true });
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void TextPattern::InitLongPressEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    constexpr int32_t longPressDelay = 600;
    if (longPressEvent_) {
        gestureHub->SetLongPressEvent(longPressEvent_, false, false, longPressDelay);
        return;
    }
    auto longPressCallback = [weak = WeakClaim(this)](GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->sourceType_ = info.GetSourceDevice();
        pattern->HandleLongPress(info);
    };
    longPressEvent_ = MakeRefPtr<LongPressEvent>(std::move(longPressCallback));

    // Default time is 500, used by drag event. Drag event would trigger if text is selected, but we want
    // it to only trigger on the second long press, after selection. Therefore, long press delay of Selection needs to
    // be slightly longer to ensure that order.
    gestureHub->SetLongPressEvent(longPressEvent_, false, false, longPressDelay);

    auto onTextSelectorChange = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto frameNode = pattern->GetHost();
        CHECK_NULL_VOID(frameNode);
        frameNode->OnAccessibilityEvent(AccessibilityEventType::TEXT_SELECTION_UPDATE);
    };
    textSelector_.SetOnAccessibility(std::move(onTextSelectorChange));
}

void TextPattern::OnHandleTouchUp()
{
    CloseSelectOverlay();
    ResetSelection();
}

void TextPattern::HandleClickEvent(GestureEvent& info)
{
    if ((selectOverlay_->IsClickAtHandle(info) && !multipleClickRecognizer_->IsRunning()) ||
        selectOverlay_->GetIsHandleDragging()) {
        return;
    }
    if (dataDetectorAdapter_->hasClickedAISpan_) {
        dataDetectorAdapter_->hasClickedAISpan_ = false;
    }
    multipleClickRecognizer_->Start(info);
    if (multipleClickRecognizer_->IsDoubleClick()) {
        HandleDoubleClickEvent(info);
    } else {
        HandleSingleClickEvent(info);
    }
}

bool TextPattern::HandleUrlClick()
{
    if (LessNotEqual(clickedSpanPosition_, 0)) {
        return false;
    }
    auto iter = spans_.begin();
    std::advance(iter, clickedSpanPosition_);
    RefPtr<SpanItem> span;
    if (iter == spans_.end()) {
        span = spans_.back();
    } else {
        span = *iter;
    }
    if (span && span->urlOnRelease) {
        span->urlOnRelease();
        return true;
    }
    return false;
}

void TextPattern::HandleSingleClickEvent(GestureEvent& info)
{
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    textContentRect.SetHeight(contentRect_.Height() - std::max(baselineOffset_, 0.0f));
    PointF textOffset = { info.GetLocalLocation().GetX() - textContentRect.GetX(),
        info.GetLocalLocation().GetY() - textContentRect.GetY() };
    if (IsSelectableAndCopy() || NeedShowAIDetect()) {
        CheckClickedOnSpanOrText(textContentRect, info.GetLocalLocation());
    }
    if (HandleUrlClick()) {
        return;
    }
    if (selectOverlay_->SelectOverlayIsOn() && !selectOverlay_->IsUsingMouse() &&
        GlobalOffsetInSelectedArea(info.GetGlobalLocation())) {
        selectOverlay_->ToggleMenu();
        selectOverlay_->SwitchToOverlayMode();
        return;
    }
    if (!isMousePressed_) {
        HandleClickAISpanEvent(textOffset);
    }
    if (dataDetectorAdapter_->hasClickedAISpan_) {
        selectOverlay_->HideMenu();
        return;
    }
    HandleClickOnTextAndSpan(info);
}

void TextPattern::HandleClickOnTextAndSpan(GestureEvent& info)
{
    if (textSelector_.IsValid() && mouseStatus_ != MouseStatus::MOVE && !isMousePressed_) {
        CloseSelectOverlay(true);
        ResetSelection();
    }
    if (clickedSpanPosition_ == -1) {
        ActTextOnClick(info);
        return;
    }
    auto iter = spans_.begin();
    std::advance(iter, clickedSpanPosition_);
    RefPtr<SpanItem> span;
    if (iter == spans_.end()) {
        span = spans_.back();
    } else {
        span = *iter;
    }
    if (span && span->onClick) {
        GestureEvent spanClickinfo = info;
        EventTarget target = info.GetTarget();
        target.area.SetWidth(Dimension(0.0f));
        target.area.SetHeight(Dimension(0.0f));
        spanClickinfo.SetTarget(target);
        span->onClick(spanClickinfo);
    } else {
        ActTextOnClick(info);
    }
}

void TextPattern::ActTextOnClick(GestureEvent& info)
{
    if (onClick_) {
        auto onClick = onClick_;
        onClick(info);
    }
}

bool TextPattern::GlobalOffsetInSelectedArea(const Offset& globalOffset)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto offset = host->GetPaintRectOffset();
    auto localOffset = globalOffset - Offset(offset.GetX(), offset.GetY());
    if (selectOverlay_->HasRenderTransform()) {
        localOffset = ConvertGlobalToLocalOffset(globalOffset);
    }
    return LocalOffsetInSelectedArea(localOffset);
}

bool TextPattern::LocalOffsetInSelectedArea(const Offset& localOffset)
{
    if (IsSelectableAndCopy() && GreatNotEqual(textSelector_.GetTextEnd(), textSelector_.GetTextStart())) {
        // Determine if the pan location is in the selected area
        auto selectedRects = pManager_->GetRects(textSelector_.GetTextStart(), textSelector_.GetTextEnd());
        TextBase::CalculateSelectedRect(selectedRects, contentRect_.Width());
        auto panOffset = OffsetF(localOffset.GetX(), localOffset.GetY()) - contentRect_.GetOffset() +
                         OffsetF(0.0f, std::min(baselineOffset_, 0.0f));
        for (const auto& selectedRect : selectedRects) {
            if (selectedRect.IsInRegion(PointF(panOffset.GetX(), panOffset.GetY()))) {
                return true;
            }
        }
    }
    return false;
}

void TextPattern::HandleClickAISpanEvent(const PointF& textOffset)
{
    dataDetectorAdapter_->hasClickedAISpan_ = false;
    if (!NeedShowAIDetect() || mouseStatus_ == MouseStatus::MOVE || IsDragging()) {
        return;
    }

    for (const auto& kv : dataDetectorAdapter_->aiSpanMap_) {
        auto& aiSpan = kv.second;
        ClickAISpan(textOffset, aiSpan);
        if (dataDetectorAdapter_->hasClickedAISpan_) {
            return;
        }
    }
}

bool TextPattern::CheckClickedOnSpanOrText(RectF textContentRect, const Offset& localLocation)
{
    clickedSpanPosition_ = -1;
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_RETURN(host, false);
    PointF textOffset = GetTextOffset(localLocation, textContentRect);
    auto clip = false;
    if (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        clip = true;
    }
    if (renderContext->GetClipEdge().has_value() && !renderContext->GetClipEdge().value_or(clip) && overlayMod_) {
        textContentRect = overlayMod_->GetBoundsRect();
        textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    }
    if (textContentRect.IsInRegion(
        PointF(static_cast<float>(localLocation.GetX()), static_cast<float>(localLocation.GetY()))) &&
        !spans_.empty() && pManager_) {
        if (CalculateClickedSpanPosition(textOffset)) {
            return true;
        }
    }
    if (onClick_) {
        return true;
    }
    return false;
}

PointF TextPattern::GetTextOffset(const Offset &localLocation, const RectF &contentRect)
{
    PointF textOffset = {static_cast<float>(localLocation.GetX()) - contentRect.GetX(),
                         static_cast<float>(localLocation.GetY()) - contentRect.GetY()};
    return textOffset;
}

bool TextPattern::CalculateClickedSpanPosition(const PointF& textOffset)
{
    int32_t start = 0;
    for (const auto& item : spans_) {
        clickedSpanPosition_++;
        if (!item) {
            continue;
        }
        auto selectedRects = GetSelectedRects(start, item->position);
        start = item->position;
        for (auto&& rect : selectedRects) {
            if (!rect.IsInRegion(textOffset)) {
                continue;
            }
            return CheckAndClick(item);
        }
    }
    clickedSpanPosition_ = -1;
    return false;
}

bool TextPattern::CheckAndClick(const RefPtr<SpanItem>& item)
{
    if (item->onClick || item->urlOnRelease) {
        return true;
    }
    clickedSpanPosition_ = -1;
    return false;
}

std::vector<RectF> TextPattern::GetSelectedRects(int32_t start, int32_t end)
{
    return pManager_->GetRects(start, end);
}

bool TextPattern::ClickAISpan(const PointF& textOffset, const AISpan& aiSpan)
{
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, false);
    int32_t start = aiSpan.start;
    int32_t end = aiSpan.end;
    if (textLayoutProperty->GetTextOverflowValue(TextOverflow::CLIP) == TextOverflow::ELLIPSIS) {
        auto range = pManager_->GetEllipsisTextRange();
        int32_t ellipsisStart = static_cast<int32_t>(range.first);
        int32_t ellipsisEnd = static_cast<int32_t>(range.second);
        if (ellipsisStart != -1 && ellipsisEnd > 0 && ellipsisStart < ellipsisEnd) {
            if (ellipsisStart <= aiSpan.start && ellipsisEnd >= aiSpan.end) {
                // ellipsisTextRange contains [aispan.start, aispan.end)
                return false;
            } else if (ellipsisStart <= aiSpan.start && ellipsisEnd >= aiSpan.start) {
                // ellipsisTextRange covers [aispan.start, ellipsisEnd)
                start = ellipsisEnd;
            } else if (ellipsisStart <= aiSpan.end && ellipsisEnd >= aiSpan.end) {
                // ellipsisTextRange covers [ellipsisStart, aiSpan.end);
                end = ellipsisStart;
            }
        }
    }

    auto aiRects = pManager_->GetRects(start, end);
    for (auto&& rect : aiRects) {
        if (rect.IsInRegion(textOffset)) {
            dataDetectorAdapter_->hasClickedAISpan_ = true;
            if (leftMousePressed_) {
                dataDetectorAdapter_->ResponseBestMatchItem(aiSpan);
                return true;
            }
            return ShowAIEntityMenu(aiSpan);
        }
    }
    return false;
}

void TextPattern::InitUrlMouseEvent()
{
    CHECK_NULL_VOID(!urlMouseEventInitialized_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto inputHub = eventHub->GetOrCreateInputEventHub();
    CHECK_NULL_VOID(inputHub);
    auto mouseTask = [weak = WeakClaim(this)](MouseInfo& info) {
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->HandleUrlMouseEvent(info);
        }
    };
    auto mouseEvent = MakeRefPtr<InputEvent>(std::move(mouseTask));
    inputHub->AddOnMouseEvent(mouseEvent);
    auto mouseHoverTask = [weak = WeakClaim(this)](bool isHover) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->URLOnHover(isHover);
    };
    auto mouseHoverEvent = MakeRefPtr<InputEvent>(std::move(mouseHoverTask));
    inputHub->AddOnHoverEvent(mouseHoverEvent);
    urlMouseEventInitialized_ = true;
}

void TextPattern::URLOnHover(bool isHover)
{
    CHECK_NULL_VOID(!isHover);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto nodeId = host->GetId();
    auto pipelineContext = host->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    pipelineContext->ChangeMouseStyle(nodeId, MouseFormat::DEFAULT);
    pipelineContext->FreeMouseStyleHoldNode(nodeId);
    CHECK_NULL_VOID(overlayMod_);
    overlayMod_->ClearSelectedForegroundColorAndRects();
    MarkDirtySelf();
}

void TextPattern::HandleUrlMouseEvent(const MouseInfo& info)
{
    if (isMousePressed_) {
        return;
    }
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    textContentRect.SetHeight(contentRect_.Height() - std::max(baselineOffset_, 0.0f));
    auto localLocation = info.GetLocalLocation();
    if (selectOverlay_->HasRenderTransform()) {
        localLocation = ConvertGlobalToLocalOffset(info.GetGlobalLocation());
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto hostId = host->GetId();
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    PointF textOffset = { static_cast<float>(localLocation.GetX()) - textContentRect.GetX(),
        static_cast<float>(localLocation.GetY()) - textContentRect.GetY() };
    auto show = ShowShadow(textOffset, GetUrlHoverColor());
    auto pipelineContext = host->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    if (show) {
        pipelineContext->SetMouseStyleHoldNode(hostId);
        pipelineContext->ChangeMouseStyle(hostId, MouseFormat::HAND_POINTING);
    } else {
        pipelineContext->ChangeMouseStyle(hostId, MouseFormat::DEFAULT);
        pipelineContext->FreeMouseStyleHoldNode(hostId);
    }
}

void TextPattern::HandleUrlTouchEvent(const TouchEventInfo& info)
{
    CHECK_NULL_VOID(overlayMod_);
    CHECK_NULL_VOID(!IsDragging());
    if (selectOverlay_->IsTouchAtHandle(info)) {
        return;
    }
    auto touchType = info.GetTouches().front().GetTouchType();
    if (touchType != TouchType::DOWN && touchType != TouchType::UP) {
        return;
    }
    if (touchType == TouchType::DOWN) {
        RectF textContentRect = contentRect_;
        auto touchOffset = info.GetTouches().front().GetLocalLocation();
        PointF textOffset = { static_cast<float>(touchOffset.GetX()) - textContentRect.GetX(),
            static_cast<float>(touchOffset.GetY()) - textContentRect.GetY() };
        ShowShadow(textOffset, GetUrlPressColor());
    } else {
        overlayMod_->ClearSelectedForegroundColorAndRects();
        MarkDirtySelf();
    }
}
void TextPattern::SetOnClickMenu(const AISpan& aiSpan, const CalculateHandleFunc& calculateHandleFunc,
    const ShowSelectOverlayFunc& showSelectOverlayFunc)

{
    dataDetectorAdapter_->onClickMenu_ = [aiSpan, weak = WeakClaim(this), calculateHandleFunc, showSelectOverlayFunc](
                                             const std::string& action) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->CloseSelectOverlay();
        pattern->HandleSelectionChange(aiSpan.start, aiSpan.end);
        if (action == COPY) {
            pattern->HandleOnCopy();
            pattern->ResetSelection();
        } else if (action == SELECT_TEXT) {
            if (calculateHandleFunc == nullptr) {
                pattern->CalculateHandleOffsetAndShowOverlay();
            } else {
                calculateHandleFunc();
            }
            if (showSelectOverlayFunc == nullptr) {
                pattern->ShowSelectOverlay({ .animation = true });
            } else {
                showSelectOverlayFunc(pattern->textSelector_.firstHandle, pattern->textSelector_.secondHandle);
            }
            auto frameNode = pattern->GetHost();
            CHECK_NULL_VOID(frameNode);
            frameNode->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
        }
    };
}

RectF TextPattern::CalcAIMenuPosition(const AISpan& aiSpan, const CalculateHandleFunc& calculateHandleFunc)
{
    // save information
    auto baseOffset = textSelector_.baseOffset;
    auto destinationOffset = textSelector_.destinationOffset;
    // calculate result
    textSelector_.Update(aiSpan.start, aiSpan.end);
    parentGlobalOffset_ = GetParentGlobalOffset();
    if (calculateHandleFunc == nullptr) {
        CalculateHandleOffsetAndShowOverlay();
    } else {
        calculateHandleFunc();
    }
    RectF aiRect;
    if (textSelector_.firstHandle.Top() != textSelector_.secondHandle.Top()) {
        auto top = std::min(textSelector_.firstHandle.Top(), textSelector_.secondHandle.Top());
        auto bottom = std::max(textSelector_.firstHandle.Bottom(), textSelector_.secondHandle.Bottom());
        auto textContentGlobalOffset = parentGlobalOffset_ + contentRect_.GetOffset();
        auto left = textContentGlobalOffset.GetX();
        auto right = textContentGlobalOffset.GetY() + contentRect_.Width();
        aiRect = RectT(left, top, right - left, bottom - top);
    } else {
        aiRect = textSelector_.firstHandle.CombineRectT(textSelector_.secondHandle);
    }
    // restore textSelector_
    textSelector_.Update(baseOffset, destinationOffset);
    if (calculateHandleFunc == nullptr) {
        CalculateHandleOffsetAndShowOverlay();
    }
    return aiRect;
}

bool TextPattern::ShowAIEntityMenu(const AISpan& aiSpan, const CalculateHandleFunc& calculateHandleFunc,
    const ShowSelectOverlayFunc& showSelectOverlayFunc)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto context = host->GetContext();
    CHECK_NULL_RETURN(context, false);
    auto safeAreaManager = context->GetSafeAreaManager();
    CHECK_NULL_RETURN(safeAreaManager, false);
    SetOnClickMenu(aiSpan, calculateHandleFunc, showSelectOverlayFunc);
    RectF aiRect = CalcAIMenuPosition(aiSpan, calculateHandleFunc);

    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, false);
    auto mode = textLayoutProperty->GetTextSelectableModeValue(TextSelectableMode::SELECTABLE_UNFOCUSABLE);
    if (!NearEqual(safeAreaManager->GetKeyboardInset().Length(), 0)
        && mode == TextSelectableMode::SELECTABLE_FOCUSABLE) {
        aiRect.SetTop(aiRect.GetY() - safeAreaManager->GetKeyboardOffset());
    }

    bool isShowCopy = true;
    bool isShowSelectText = true;
    if (copyOption_ == CopyOptions::None) {
        isShowCopy = false;
        isShowSelectText = false;
    } else if (mode == TextSelectableMode::UNSELECTABLE) {
        isShowSelectText = false;
    }
    return dataDetectorAdapter_->ShowAIEntityMenu(aiSpan, aiRect, host, isShowCopy, isShowSelectText);
}

void TextPattern::HandleDoubleClickEvent(GestureEvent& info)
{
    CheckOnClickEvent(info);
    if (!IsSelectableAndCopy() || textForDisplay_.empty()) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto hub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    isDoubleClick_ = true;
    auto textPaintOffset = contentRect_.GetOffset() - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));
    Offset textOffset = { info.GetLocalLocation().GetX() - textPaintOffset.GetX(),
        info.GetLocalLocation().GetY() - textPaintOffset.GetY() };
    InitSelection(textOffset);
    textResponseType_ = TextResponseType::NONE;
    UpdateSelectionSpanType(std::min(textSelector_.baseOffset, textSelector_.destinationOffset),
        std::max(textSelector_.baseOffset, textSelector_.destinationOffset));
    parentGlobalOffset_ = GetParentGlobalOffset();
    CalculateHandleOffsetAndShowOverlay();
    if (!isMousePressed_) {
        ShowSelectOverlay({ .animation = true });
    }
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void TextPattern::CheckOnClickEvent(GestureEvent& info)
{
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    textContentRect.SetHeight(contentRect_.Height() - std::max(baselineOffset_, 0.0f));
    PointF textOffset = { info.GetLocalLocation().GetX() - textContentRect.GetX(),
        info.GetLocalLocation().GetY() - textContentRect.GetY() };
    if (IsSelectableAndCopy() || NeedShowAIDetect()) {
        CheckClickedOnSpanOrText(textContentRect, info.GetLocalLocation());
    }
    HandleClickOnTextAndSpan(info);
}

void TextPattern::InitClickEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    gestureHub->SetNodeClickDistance(distanceThreshold_);
    CHECK_NULL_VOID(!clickEventInitialized_);
    auto clickCallback = [weak = WeakClaim(this)](GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->sourceType_ = info.GetSourceDevice();
        pattern->HandleClickEvent(info);
    };
    auto clickListener = MakeRefPtr<ClickEvent>(std::move(clickCallback));
    clickListener->SetSysJudge([weak = WeakClaim(this)](const RefPtr<GestureInfo>& gestureInfo,
                                   const std::shared_ptr<BaseGestureEvent>& info) -> GestureJudgeResult {
        auto textPattern = weak.Upgrade();
        CHECK_NULL_RETURN(textPattern, GestureJudgeResult::CONTINUE);
        if (info->GetFingerList().empty()) {
            return GestureJudgeResult::CONTINUE;
        }
        auto localLocation = info->GetFingerList().begin()->localLocation_;
        auto contentRect = textPattern->GetTextContentRect();
        auto baselineOffset = textPattern->GetBaselineOffset();

        RectF textContentRect = contentRect;
        textContentRect.SetTop(contentRect.GetY() - std::min(baselineOffset, 0.0f));
        textContentRect.SetHeight(contentRect.Height() - std::max(baselineOffset, 0.0f));
        if (textPattern->GetCopyOptions() == CopyOptions::None && !textPattern->NeedShowAIDetect() &&
            !textPattern->CheckClickedOnSpanOrText(textContentRect, localLocation)) {
            return GestureJudgeResult::REJECT;
        }
        return GestureJudgeResult::CONTINUE;
    });
    gestureHub->AddClickEvent(clickListener);
    clickEventInitialized_ = true;
}

void TextPattern::InitAISpanHoverEvent()
{
    CHECK_NULL_VOID(!aiSpanHoverEventInitialized_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto inputHub = eventHub->GetOrCreateInputEventHub();
    CHECK_NULL_VOID(inputHub);

    auto aiSpanHoverTask = [weak = WeakClaim(this)](MouseInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleAISpanHoverEvent(info);
    };
    auto aiSpanHoverEvent = MakeRefPtr<InputEvent>(std::move(aiSpanHoverTask));
    inputHub->AddOnMouseEvent(aiSpanHoverEvent);
    aiSpanHoverEventInitialized_ = true;
}

void TextPattern::HandleAISpanHoverEvent(const MouseInfo& info)
{
    if (info.GetAction() != MouseAction::MOVE || !NeedShowAIDetect() || !isHover_) {
        return;
    }
    if (dataDetectorAdapter_->aiSpanRects_.empty()) {
        for (const auto& kv : dataDetectorAdapter_->aiSpanMap_) {
            auto& aiSpan = kv.second;
            const auto& aiRects = pManager_->GetRects(aiSpan.start, aiSpan.end);
            dataDetectorAdapter_->aiSpanRects_.insert(
                dataDetectorAdapter_->aiSpanRects_.end(), aiRects.begin(), aiRects.end());
        }
    }

    auto textPaintOffset = contentRect_.GetOffset() - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));
    PointF textOffset = { info.GetLocalLocation().GetX() - textPaintOffset.GetX(),
        info.GetLocalLocation().GetY() - textPaintOffset.GetY() };
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    auto nodeId = host->GetId();
    pipeline->SetMouseStyleHoldNode(nodeId);
    for (auto&& rect : dataDetectorAdapter_->aiSpanRects_) {
        if (!rect.IsInRegion(textOffset)) {
            continue;
        }
        if (currentMouseStyle_ != MouseFormat::HAND_POINTING) {
            bool changeSuccess = pipeline->ChangeMouseStyle(nodeId, MouseFormat::HAND_POINTING);
            CHECK_NULL_VOID(changeSuccess);
            currentMouseStyle_ = MouseFormat::HAND_POINTING;
        }
        return;
    }
    if (currentMouseStyle_ != MouseFormat::DEFAULT) {
        bool changeSuccess = pipeline->ChangeMouseStyle(nodeId, MouseFormat::DEFAULT);
        CHECK_NULL_VOID(changeSuccess);
        currentMouseStyle_ = MouseFormat::DEFAULT;
    }
}

void TextPattern::OnHover(bool isHover)
{
    isHover_ = isHover;
    TAG_LOGI(AceLogTag::ACE_TEXT, "isHover=%{public}d", isHover);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    auto nodeId = host->GetId();
    if (isHover) {
        pipeline->SetMouseStyleHoldNode(nodeId);
        pipeline->ChangeMouseStyle(nodeId, MouseFormat::DEFAULT);
        currentMouseStyle_ = MouseFormat::DEFAULT;
    } else {
        pipeline->ChangeMouseStyle(nodeId, MouseFormat::DEFAULT);
        currentMouseStyle_ = MouseFormat::DEFAULT;
        pipeline->FreeMouseStyleHoldNode(nodeId);
    }
}

void TextPattern::InitMouseEvent()
{
    CHECK_NULL_VOID(!mouseEventInitialized_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto inputHub = eventHub->GetOrCreateInputEventHub();
    CHECK_NULL_VOID(inputHub);

    auto mouseTask = [weak = WeakClaim(this)](MouseInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleMouseEvent(info);
    };
    auto mouseEvent = MakeRefPtr<InputEvent>(std::move(mouseTask));
    inputHub->AddOnMouseEvent(mouseEvent);

    auto hoverTask = [weak = WeakClaim(this)](bool isHover) {
        TAG_LOGI(AceLogTag::ACE_TEXT, "on hover event isHover=%{public}d", isHover);
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->OnHover(isHover);
        }
    };
    auto hoverEvent = MakeRefPtr<InputEvent>(std::move(hoverTask));
    inputHub->AddOnHoverEvent(hoverEvent);
    mouseEventInitialized_ = true;
}

void TextPattern::HandleMouseEvent(const MouseInfo& info)
{
    auto localLocation = info.GetLocalLocation();
    if (isAutoScrollByMouse_ && GetHost()) {
        NG::PointF localPoint(info.GetGlobalLocation().GetX(), info.GetGlobalLocation().GetY());
        NG::NGGestureRecognizer::Transform(localPoint, WeakClaim(GetHost().GetRawPtr()), true);
        localLocation.SetX(localPoint.GetX());
        localLocation.SetY(localPoint.GetY());
    }
    auto textPaintOffset = contentRect_.GetOffset() - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));
    Offset textOffset = { localLocation.GetX() - textPaintOffset.GetX(),
        localLocation.GetY() - textPaintOffset.GetY() };
    if (info.GetButton() == MouseButton::LEFT_BUTTON) {
        lastLeftMouseMoveLocation_ = info.GetGlobalLocation();
        HandleMouseLeftButton(info, textOffset);
        if (IsSelected()) {
            selectOverlay_->SetSelectionHoldCallback();
        }
        sourceType_ = info.GetSourceDevice();
    } else if (info.GetButton() == MouseButton::RIGHT_BUTTON) {
        HandleMouseRightButton(info, textOffset);
        sourceType_ = info.GetSourceDevice();
    }
}

void TextPattern::HandleMouseLeftButton(const MouseInfo& info, const Offset& textOffset)
{
    if (info.GetAction() == MouseAction::PRESS) {
        HandleMouseLeftPressAction(info, textOffset);
    } else if (info.GetAction() == MouseAction::MOVE) {
        HandleMouseLeftMoveAction(info, textOffset);
    } else if (info.GetAction() == MouseAction::RELEASE) {
        HandleMouseLeftReleaseAction(info, textOffset);
    }

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void TextPattern::HandleMouseLeftPressAction(const MouseInfo& info, const Offset& textOffset)
{
    isMousePressed_ = true;
    leftMousePressed_ = true;
    ShowShadow({ textOffset.GetX(), textOffset.GetY() }, GetUrlPressColor());
    if (BetweenSelectedPosition(info.GetGlobalLocation())) {
        blockPress_ = true;
        return;
    }
    mouseStatus_ = MouseStatus::PRESSED;
    CHECK_NULL_VOID(pManager_);
    if (shiftFlag_) {
        auto end = pManager_->GetGlyphIndexByCoordinate(textOffset);
        HandleSelectionChange(textSelector_.baseOffset, end);
    } else {
        auto start = pManager_->GetGlyphIndexByCoordinate(textOffset);
        textSelector_.Update(start, start);
    }
    // auto scroll.
    scrollableParent_ = selectOverlay_->FindScrollableParent();
    auto host = GetHost();
    if (scrollableParent_.Upgrade() && host) {
        host->RegisterNodeChangeListener();
    }
}

void TextPattern::HandleMouseLeftReleaseAction(const MouseInfo& info, const Offset& textOffset)
{
    if (blockPress_) {
        blockPress_ = false;
    }
    auto oldMouseStatus = mouseStatus_;
    mouseStatus_ = MouseStatus::RELEASED;
    ShowShadow({ textOffset.GetX(), textOffset.GetY() }, GetUrlHoverColor());
    if (isDoubleClick_) {
        isDoubleClick_ = false;
        isMousePressed_ = false;
        leftMousePressed_ = false;
        return;
    }
    if (oldMouseStatus != MouseStatus::MOVE && oldMouseStatus == MouseStatus::PRESSED && !IsDragging()) {
        HandleClickAISpanEvent(PointF(textOffset.GetX(), textOffset.GetY()));
        if (dataDetectorAdapter_->hasClickedAISpan_) {
            selectOverlay_->DisableMenu();
            isMousePressed_ = false;
            leftMousePressed_ = false;
            return;
        }
    }

    CHECK_NULL_VOID(pManager_);
    auto start = textSelector_.baseOffset;
    auto end = textSelector_.destinationOffset;
    if (!IsSelected() && !textSelector_.IsValid()) {
        start = -1;
        end = -1;
    }
    if (isMousePressed_ || oldMouseStatus == MouseStatus::MOVE || shiftFlag_) {
        HandleSelectionChange(start, end);
    }

    if (IsSelected() && oldMouseStatus == MouseStatus::MOVE && IsSelectedBindSelectionMenu()) {
        selectOverlay_->SetMouseMenuOffset(OffsetF(
            static_cast<float>(info.GetGlobalLocation().GetX()), static_cast<float>(info.GetGlobalLocation().GetY())));
        textResponseType_ = TextResponseType::SELECTED_BY_MOUSE;
        ShowSelectOverlay({ .animation = true });
    }
    isMousePressed_ = false;
    leftMousePressed_ = false;
    // stop auto scroll.
    auto host = GetHost();
    if (host && scrollableParent_.Upgrade() && !selectOverlay_->SelectOverlayIsOn()) {
        host->UnregisterNodeChangeListener();
    }
    selectOverlay_->TriggerScrollableParentToScroll(scrollableParent_.Upgrade(), info.GetGlobalLocation(), true);
    isAutoScrollByMouse_ = false;
}

void TextPattern::HandleMouseLeftMoveAction(const MouseInfo& info, const Offset& textOffset)
{
    if (!IsSelectableAndCopy()) {
        isMousePressed_ = false;
        leftMousePressed_ = false;
        return;
    }
    if (blockPress_ && !shiftFlag_) {
        dragBoxes_ = GetTextBoxes();
        return;
    }
    if (isMousePressed_) {
        mouseStatus_ = MouseStatus::MOVE;
        CHECK_NULL_VOID(pManager_);
        auto end = pManager_->GetGlyphIndexByCoordinate(textOffset);
        HandleSelectionChange(textSelector_.baseOffset, end);
        selectOverlay_->TriggerScrollableParentToScroll(scrollableParent_.Upgrade(), info.GetGlobalLocation(), false);
    }
}

void TextPattern::HandleMouseRightButton(const MouseInfo& info, const Offset& textOffset)
{
    if (info.GetAction() == MouseAction::RELEASE) {
        selectOverlay_->SetMouseMenuOffset(OffsetF(
            static_cast<float>(info.GetGlobalLocation().GetX()), static_cast<float>(info.GetGlobalLocation().GetY())));
        if (!BetweenSelectedPosition(info.GetGlobalLocation())) {
            HandleClickAISpanEvent(PointF(textOffset.GetX(), textOffset.GetY()));
            if (dataDetectorAdapter_->hasClickedAISpan_) {
                isMousePressed_ = false;
                return;
            }
        }
        if (!IsSelectableAndCopy()) {
            return;
        }

        CalculateHandleOffsetAndShowOverlay(true);
        if (selectOverlay_->SelectOverlayIsOn()) {
            CloseSelectOverlay(true);
        }
        textResponseType_ = TextResponseType::RIGHT_CLICK;
        if (!IsSelected()) {
            auto spanNode = DynamicCast<FrameNode>(GetChildByIndex(GetSelectionSpanItemIndex(info)));
            if (spanNode && spanNode->GetTag() == V2::IMAGE_ETS_TAG) {
                selectedType_ = TextSpanType::IMAGE;
            } else {
                selectedType_ = TextSpanType::TEXT;
            }
        }
        ShowSelectOverlay({ .animation = true });
        isMousePressed_ = false;
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    } else if (info.GetAction() == MouseAction::PRESS) {
        isMousePressed_ = true;
        CloseSelectOverlay(true);
    }
}

void TextPattern::InitTouchEvent()
{
    CHECK_NULL_VOID(!touchEventInitialized_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto gesture = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gesture);

    auto touchTask = [weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->sourceType_ = info.GetSourceDevice();
        pattern->HandleTouchEvent(info);
    };
    auto touchListener = MakeRefPtr<TouchEventImpl>(std::move(touchTask));
    gesture->AddTouchEvent(touchListener);
    touchEventInitialized_ = true;
}

void TextPattern::InitUrlTouchEvent()
{
    CHECK_NULL_VOID(!urlTouchEventInitialized_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto gesture = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gesture);

    auto touchTask = [weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleUrlTouchEvent(info);
    };
    auto touchListener = MakeRefPtr<TouchEventImpl>(std::move(touchTask));
    gesture->AddTouchEvent(touchListener);
    urlTouchEventInitialized_ = true;
}

void TextPattern::MarkDirtySelf()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void TextPattern::HandleTouchEvent(const TouchEventInfo& info)
{
    DoGestureSelection(info);
}

void TextPattern::InitKeyEvent()
{
    CHECK_NULL_VOID(!keyEventInitialized_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto focusHub = host->GetOrCreateFocusHub();
    CHECK_NULL_VOID(focusHub);

    auto keyTask = [weak = WeakClaim(this)](const KeyEvent& event) -> bool {
        auto pattern = weak.Upgrade();
        CHECK_NULL_RETURN(pattern, false);
        return pattern->HandleKeyEvent(event);
    };
    focusHub->SetOnKeyEventInternal(std::move(keyTask));
    keyEventInitialized_ = true;
}

void TextPattern::UpdateShiftFlag(const KeyEvent& keyEvent)
{
    bool flag = false;
    if (keyEvent.action == KeyAction::DOWN) {
        if (keyEvent.HasKey(KeyCode::KEY_SHIFT_LEFT) || keyEvent.HasKey(KeyCode::KEY_SHIFT_RIGHT)) {
            flag = true;
        }
    }
    if (flag != shiftFlag_) {
        shiftFlag_ = flag;
        if (!shiftFlag_) {
            // open drag
            InitDragEvent();
        } else  {
            // close drag
            ClearDragEvent();
        }
    }
}

bool TextPattern::HandleKeyEvent(const KeyEvent& keyEvent)
{
    UpdateShiftFlag(keyEvent);
    if (keyEvent.action != KeyAction::DOWN) {
        return false;
    }

    if (keyEvent.IsCtrlWith(KeyCode::KEY_C)) {
        HandleOnCopy();
        return true;
    }

    if (keyEvent.IsShiftWith(keyEvent.code)) {
        HandleOnSelect(keyEvent.code);
        return true;
    }
    return false;
}

void TextPattern::HandleOnSelect(KeyCode code)
{
    auto end = textSelector_.GetEnd();
    switch (code) {
        case KeyCode::KEY_DPAD_LEFT: {
            HandleSelection(true, end - 1);
            break;
        }
        case KeyCode::KEY_DPAD_RIGHT: {
            HandleSelection(false, end + 1);
            break;
        }
        case KeyCode::KEY_DPAD_UP: {
            HandleSelectionUp();
            break;
        }
        case KeyCode::KEY_DPAD_DOWN: {
            HandleSelectionDown();
            break;
        }
        default:
            break;
    }
}

void TextPattern::HandleSelectionUp()
{
    auto end = textSelector_.GetEnd();
    auto line = pManager_->GetLineCount();
    if (line == 1) {
        HandleSelection(true, 0);
        return;
    }
    CaretMetricsF secondHandleMetrics;
    CalcCaretMetricsByPosition(textSelector_.destinationOffset, secondHandleMetrics, TextAffinity::UPSTREAM);
    auto secondOffsetX = secondHandleMetrics.offset.GetX();
    auto secondOffsetY = secondHandleMetrics.offset.GetY();
    double height = GetTextHeight(end, false);
    Offset offset = { secondOffsetX, secondOffsetY - height * 0.5 };
    auto caculateIndex = GetHandleIndex(offset);
    if (end == caculateIndex) {
        caculateIndex = 0;
    }
    HandleSelection(true, caculateIndex);
}

void TextPattern::HandleSelectionDown()
{
    auto end = textSelector_.GetEnd();
    auto line = pManager_->GetLineCount();
    auto lastIndex = GetActualTextLength();
    if (line == 1) {
        HandleSelection(true, lastIndex);
        return;
    }
    CaretMetricsF secondHandleMetrics;
    CalcCaretMetricsByPosition(textSelector_.destinationOffset, secondHandleMetrics, TextAffinity::UPSTREAM);
    auto secondOffsetX = secondHandleMetrics.offset.GetX();
    double height = GetTextHeight(end, true);
    auto caculateIndex = GetHandleIndex({ secondOffsetX, height });
    if (NearZero(height) || caculateIndex == end || caculateIndex > lastIndex) {
        caculateIndex = lastIndex;
    }
    HandleSelection(true, caculateIndex);
}

void TextPattern::HandleSelection(bool isEmojiStart, int32_t end)
{
    auto start = textSelector_.GetStart();
    auto lastIndex = GetActualTextLength();
    if (start < 0 || start > lastIndex || end < 0 || end > lastIndex) {
        return;
    }
    int32_t emojiStartIndex;
    int32_t emojiEndIndex;
    bool isIndexInEmoji = TextEmojiProcessor::IsIndexInEmoji(end, GetSelectedText(0, lastIndex),
        emojiStartIndex, emojiEndIndex);
    if (isIndexInEmoji) {
        end = isEmojiStart ? emojiStartIndex : emojiEndIndex;
    }
    HandleSelectionChange(start, end);
    CalculateHandleOffsetAndShowOverlay();
    CloseSelectOverlay(true);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

double TextPattern::GetTextHeight(int32_t index, bool isNextLine)
{
    double lineHeight = 0.0;
    auto lineCount = static_cast<int32_t>(pManager_->GetLineCount());
    for (auto lineNumber = 0; lineNumber < lineCount; lineNumber++) {
        auto lineMetrics = GetLineMetrics(lineNumber);
        auto startIndex = static_cast<int32_t>(lineMetrics.startIndex);
        auto endIndex = static_cast<int32_t>(lineMetrics.endIndex);
        lineHeight += lineMetrics.height;
        if (isNextLine) {
            if (index <= endIndex && endIndex != GetActualTextLength()) {
                return lineHeight;
            }
        } else {
            auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_RETURN(textLayoutProperty, 0);
            auto maxLines = textLayoutProperty->GetMaxLinesValue(Infinity<uint32_t>());
            if ((index <= endIndex && startIndex != 0) ||
                ((lineNumber + 1) == static_cast<int32_t>(maxLines) && lineNumber != 0)) {
                return GetLineMetrics(lineNumber - 1).height;
            }
        }
    }
    return 0.0;
}

int32_t TextPattern::GetActualTextLength()
{
    auto lineCount = static_cast<int32_t>(pManager_->GetLineCount());
    return GetLineMetrics(lineCount - 1).endIndex;
}

void TextPattern::SetTextSelectableMode(TextSelectableMode value)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto focusHub = host->GetOrCreateFocusHub();
    CHECK_NULL_VOID(focusHub);
    if (value == TextSelectableMode::SELECTABLE_FOCUSABLE) {
        focusHub->SetFocusable(true);
        focusHub->SetIsFocusOnTouch(true);
    } else {
        focusHub->SetFocusable(false);
        focusHub->SetIsFocusOnTouch(false);
    }
}

bool TextPattern::IsSelectableAndCopy()
{
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, false);
    auto mode = textLayoutProperty->GetTextSelectableModeValue(TextSelectableMode::SELECTABLE_UNFOCUSABLE);
    return mode != TextSelectableMode::UNSELECTABLE && copyOption_ != CopyOptions::None;
}

bool TextPattern::IsDraggable(const Offset& offset)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto eventHub = host->GetEventHub<EventHub>();
    bool draggable = eventHub->HasOnDragStart();
    return draggable && LocalOffsetInSelectedArea(offset);
}

NG::DragDropInfo TextPattern::OnDragStart(const RefPtr<Ace::DragEvent>& event, const std::string& extraParams)
{
    DragDropInfo itemInfo;
    auto host = GetHost();
    CHECK_NULL_RETURN(host, itemInfo);
    if (overlayMod_) {
        overlayMod_->ClearSelectedForegroundColorAndRects();
    }
    auto hub = host->GetEventHub<EventHub>();
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    auto selectStart = textSelector_.GetTextStart();
    auto selectEnd = textSelector_.GetTextEnd();
    recoverStart_ = selectStart;
    recoverEnd_ = selectEnd;
    auto textSelectInfo = GetSpansInfo(selectStart, selectEnd, GetSpansMethod::ONSELECT);
    dragResultObjects_ = textSelectInfo.GetSelection().resultObjects;
    ResetDragRecordSize(dragResultObjects_.empty() ? -1 : 1);
    status_ = Status::DRAGGING;
    if (dragResultObjects_.empty() || !gestureHub->GetIsTextDraggable()) {
        return itemInfo;
    }
    auto data = event->GetData();
    if (!data) {
        AddUdmfData(event);
    }
    CloseOperate();
    host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE_SELF);
    return itemInfo;
}

void TextPattern::AddUdmfTxtPreProcessor(const ResultObject src, ResultObject& result, bool isAppend)
{
    auto valueString = GetSelectedSpanText(src.valueString,
        src.offsetInSpan[RichEditorSpanRange::RANGESTART], src.offsetInSpan[RichEditorSpanRange::RANGEEND]);
    if (isAppend) {
        result.valueString = result.valueString + valueString;
    } else {
        result.valueString = valueString;
    }
}

void TextPattern::AddUdmfData(const RefPtr<Ace::DragEvent>& event)
{
    RefPtr<UnifiedData> unifiedData = UdmfClient::GetInstance()->CreateUnifiedData();
    if (isSpanStringMode_) {
        std::vector<uint8_t> arr;
        auto dragSpanString = styledString_->GetSubSpanString(recoverStart_, recoverEnd_ - recoverStart_);
        dragSpanString->EncodeTlv(arr);
        UdmfClient::GetInstance()->AddSpanStringRecord(unifiedData, arr);
    } else {
        ProcessNormalUdmfData(unifiedData);
    }
    event->SetData(unifiedData);
}

void TextPattern::ProcessNormalUdmfData(const RefPtr<UnifiedData>& unifiedData)
{
    std::list<ResultObject> finalResult;
    auto type = SelectSpanType::TYPESPAN;
    for (const auto& resultObj : dragResultObjects_) {
        if (finalResult.empty() || resultObj.type != SelectSpanType::TYPESPAN || type != SelectSpanType::TYPESPAN) {
            type = resultObj.type;
            finalResult.emplace_back(resultObj);
            if (resultObj.type == SelectSpanType::TYPESPAN) {
                AddUdmfTxtPreProcessor(resultObj, finalResult.back(), false);
            }
        } else {
            AddUdmfTxtPreProcessor(resultObj, finalResult.back(), true);
        }
    }
    auto resultProcessor = [unifiedData, weak = WeakClaim(this)](const ResultObject& result) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        std::string u8ValueString = UtfUtils::Str16ToStr8(result.valueString);
        if (result.type == SelectSpanType::TYPESPAN) {
            UdmfClient::GetInstance()->AddPlainTextRecord(unifiedData, u8ValueString);
            return;
        }
        if (result.type == SelectSpanType::TYPEIMAGE) {
            if (result.valuePixelMap) {
                pattern->AddPixelMapToUdmfData(result.valuePixelMap, unifiedData);
            } else if (u8ValueString.size() > 1) {
                UdmfClient::GetInstance()->AddImageRecord(unifiedData, u8ValueString);
            } else {
                // builder span, fill pixelmap data
                auto builderNode = DynamicCast<FrameNode>(pattern->GetChildByIndex(result.spanPosition.spanIndex));
                CHECK_NULL_VOID(builderNode);
                pattern->AddPixelMapToUdmfData(builderNode->GetPixelMap(), unifiedData);
            }
        }
    };
    for (const auto& resultObj : finalResult) {
        resultProcessor(resultObj);
    }
}

void TextPattern::AddPixelMapToUdmfData(const RefPtr<PixelMap>& pixelMap, const RefPtr<UnifiedData>& unifiedData)
{
    CHECK_NULL_VOID(pixelMap && unifiedData);
    const uint8_t* pixels = pixelMap->GetPixels();
    CHECK_NULL_VOID(pixels);
    int32_t length = pixelMap->GetByteCount();
    std::vector<uint8_t> data(pixels, pixels + length);
    PixelMapRecordDetails details = { pixelMap->GetWidth(), pixelMap->GetHeight(),
        pixelMap->GetPixelFormat(), pixelMap->GetAlphaType() };
    UdmfClient::GetInstance()->AddPixelMapRecord(unifiedData, data, details);
}

void TextPattern::CloseOperate()
{
    UpdateSpanItemDragStatus(dragResultObjects_, true);
    recoverDragResultObjects_ = dragResultObjects_;
    AceEngineExt::GetInstance().DragStartExt();
    CloseKeyboard(true);
    CloseSelectOverlay();
    ResetSelection();
}

DragDropInfo TextPattern::OnDragStartNoChild(const RefPtr<Ace::DragEvent>& event, const std::string& extraParams)
{
    auto weakPtr = WeakClaim(this);
    DragDropInfo itemInfo;
    auto pattern = weakPtr.Upgrade();
    auto host = pattern->GetHost();
    auto hub = host->GetEventHub<EventHub>();
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(gestureHub, itemInfo);
    if (!gestureHub->GetIsTextDraggable()) {
        return itemInfo;
    }
    auto layoutProperty = host->GetLayoutProperty<TextLayoutProperty>();
    pattern->status_ = Status::DRAGGING;
    pattern->contentMod_->ChangeDragStatus();
    pattern->showSelect_ = false;
    auto start = textSelector_.GetTextStart();
    pattern->recoverStart_ = start;
    auto end = textSelector_.GetTextEnd();
    pattern->recoverEnd_ = end;
    auto beforeStr = GetSelectedText(0, start);
    auto selectedStr = GetSelectedText(textSelector_.GetTextStart(), textSelector_.GetTextEnd());
    auto afterStr = GetSelectedText(end, textForDisplay_.length());
    pattern->dragContents_ = { beforeStr, selectedStr, afterStr };
    auto selectedUtf8Str = UtfUtils::Str16ToStr8(selectedStr);
    itemInfo.extraInfo = selectedUtf8Str;
    RefPtr<UnifiedData> unifiedData = UdmfClient::GetInstance()->CreateUnifiedData();
    UdmfClient::GetInstance()->AddPlainTextRecord(unifiedData, selectedUtf8Str);
    event->SetData(unifiedData);
    host->MarkDirtyWithOnProChange(layoutProperty->GetMaxLinesValue(Infinity<float>()) <= 1
                                       ? PROPERTY_UPDATE_MEASURE_SELF
                                       : PROPERTY_UPDATE_MEASURE);

    CloseSelectOverlay();
    ResetSelection();
    return itemInfo;
}

void TextPattern::UpdateSpanItemDragStatus(const std::list<ResultObject>& resultObjects, bool isDragging)
{
    if (resultObjects.empty()) {
        return;
    }
    auto dragStatusUpdateAction = [weakPtr = WeakClaim(this), isDragging](const ResultObject& resultObj) {
        auto pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (pattern->spans_.empty()) {
            return;
        }
        auto it = pattern->spans_.begin();
        if (resultObj.spanPosition.spanIndex >= static_cast<int32_t>(pattern->spans_.size())) {
            std::advance(it, !pattern->spans_.empty() ? static_cast<int32_t>(pattern->spans_.size()) - 1 : 0);
            TAG_LOGW(AceLogTag::ACE_RICH_TEXT, "resultObj.spanPosition.spanIndex is larger than spans size.");
        } else {
            std::advance(it, resultObj.spanPosition.spanIndex);
        }
        auto spanItem = *it;
        CHECK_NULL_VOID(spanItem);
        if (resultObj.type == SelectSpanType::TYPESPAN) {
            if (pattern->isSpanStringMode_) {
                spanItem = resultObj.span.Upgrade();
                CHECK_NULL_VOID(spanItem);
            }
            if (isDragging) {
                spanItem->StartDrag(resultObj.offsetInSpan[RichEditorSpanRange::RANGESTART],
                    resultObj.offsetInSpan[RichEditorSpanRange::RANGEEND]);
                pattern->dragSpanItems_.emplace_back(spanItem);
            } else {
                spanItem->EndDrag();
            }
            return;
        }

        if (resultObj.type == SelectSpanType::TYPEIMAGE) {
            if (isDragging) {
                pattern->dragSpanItems_.emplace_back(spanItem);
            }
            auto imageNode = DynamicCast<FrameNode>(pattern->GetChildByIndex(resultObj.spanPosition.spanIndex));
            CHECK_NULL_VOID(imageNode);
            auto renderContext = imageNode->GetRenderContext();
            CHECK_NULL_VOID(renderContext);
            renderContext->UpdateOpacity(isDragging ? (double)DRAGGED_TEXT_OPACITY / 255 : 1);
            imageNode->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
        }
    };
    for (const auto& resultObj : resultObjects) {
        dragStatusUpdateAction(resultObj);
    }
}

void TextPattern::OnDragEnd(const RefPtr<Ace::DragEvent>& event)
{
    ResetDragRecordSize(-1);
    auto wk = WeakClaim(this);
    auto pattern = wk.Upgrade();
    CHECK_NULL_VOID(pattern);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    isMousePressed_ = false;
    if (status_ == Status::DRAGGING) {
        status_ = Status::NONE;
    }
    dragSpanItems_.clear();
    if (dragResultObjects_.empty()) {
        return;
    }
    UpdateSpanItemDragStatus(dragResultObjects_, false);
    dragResultObjects_.clear();
    if (event && event->GetResult() != DragRet::DRAG_SUCCESS && IsSelectableAndCopy()) {
        HandleSelectionChange(recoverStart_, recoverEnd_);
        isShowMenu_ = false;
        if (GetCurrentDragTool() == SourceTool::FINGER) {
            CalculateHandleOffsetAndShowOverlay();
            ShowSelectOverlay({ .menuIsShow = false });
        }
    }
    host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE_SELF);
}

void TextPattern::OnDragEndNoChild(const RefPtr<Ace::DragEvent>& event)
{
    auto wk = WeakClaim(this);
    auto pattern = wk.Upgrade();
    CHECK_NULL_VOID(pattern);
    auto host = pattern->GetHost();
    CHECK_NULL_VOID(host);
    isMousePressed_ = false;
    if (pattern->status_ == Status::DRAGGING) {
        pattern->status_ = Status::NONE;
        pattern->MarkContentChange();
        pattern->contentMod_->ChangeDragStatus();
        if (event && event->GetResult() != DragRet::DRAG_SUCCESS && IsSelectableAndCopy()) {
            HandleSelectionChange(recoverStart_, recoverEnd_);
            isShowMenu_ = false;
            if (GetCurrentDragTool() == SourceTool::FINGER) {
                CalculateHandleOffsetAndShowOverlay();
                ShowSelectOverlay({ .menuIsShow = false });
            }
        }
        auto layoutProperty = host->GetLayoutProperty<TextLayoutProperty>();
        host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE_SELF);
    }
}

void TextPattern::OnDragMove(const RefPtr<Ace::DragEvent>& event)
{
    auto weakPtr = WeakClaim(this);
    auto pattern = weakPtr.Upgrade();
    if (pattern->status_ == Status::DRAGGING) {
        CloseSelectOverlay();
        pattern->showSelect_ = false;
    }
}

void TextPattern::InitDragEvent()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto gestureHub = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    gestureHub->InitDragDropEvent();
    gestureHub->SetTextDraggable(true);
    gestureHub->SetThumbnailCallback(GetThumbnailCallback());
    auto onDragStart = [weakPtr = WeakClaim(this)](
                           const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& extraParams) -> DragDropInfo {
        NG::DragDropInfo itemInfo;
        auto pattern = weakPtr.Upgrade();
        CHECK_NULL_RETURN(pattern, itemInfo);
        auto eventHub = pattern->GetEventHub<EventHub>();
        CHECK_NULL_RETURN(eventHub, itemInfo);
        pattern->SetCurrentDragTool(event->GetSourceTool());
        if (pattern->spans_.empty() && !pattern->isSpanStringMode_) {
            return pattern->OnDragStartNoChild(event, extraParams);
        }
        return pattern->OnDragStart(event, extraParams);
    };
    eventHub->SetDefaultOnDragStart(std::move(onDragStart));
    auto onDragMove = [weakPtr = WeakClaim(this)](
                          const RefPtr<OHOS::Ace::DragEvent>& event, const std::string& extraParams) {
        auto pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->OnDragMove(event);
    };
    eventHub->SetOnDragMove(std::move(onDragMove));
    auto onDragEnd = [weakPtr = WeakClaim(this)](const RefPtr<OHOS::Ace::DragEvent>& event) {
        auto pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        // 拖拽框架强引用导致退出页面后还能够运行到这里
        if (pattern->isDetachFromMainTree_) {
            return;
        }
        ContainerScope scope(pattern->GetHostInstanceId());
        pattern->showSelect_ = true;
        if (pattern->spans_.empty()) {
            pattern->OnDragEndNoChild(event);
        } else {
            pattern->OnDragEnd(event);
        }
    };
    eventHub->SetOnDragEnd(std::move(onDragEnd));
}

void TextPattern::ClearDragEvent()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto gestureHub = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    gestureHub->SetTextDraggable(false);
    gestureHub->SetIsTextDraggable(false);
    gestureHub->SetThumbnailCallback(nullptr);
    eventHub->SetDefaultOnDragStart(nullptr);
    eventHub->SetOnDragMove(nullptr);
    eventHub->SetOnDragEnd(nullptr);
}

std::function<void(Offset)> TextPattern::GetThumbnailCallback()
{
    return [wk = WeakClaim(this)](const Offset& point) {
        auto pattern = wk.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (pattern->BetweenSelectedPosition(point)) {
            const auto& children = pattern->GetChildNodes();
            std::list<RefPtr<FrameNode>> imageChildren;
            for (const auto& child : children) {
                auto node = DynamicCast<FrameNode>(child);
                if (!node) {
                    continue;
                }
                auto image = node->GetPattern<ImagePattern>();
                if (image) {
                    imageChildren.emplace_back(node);
                }
            }
            RichEditorDragInfo info;
            if (pattern->selectOverlay_->IsHandleVisible(true)) {
                info.firstHandle = pattern->textSelector_.firstHandle;
            }
            if (pattern->selectOverlay_->IsHandleVisible(false)) {
                info.secondHandle = pattern->textSelector_.secondHandle;
            }
            pattern->dragNode_ = RichEditorDragPattern::CreateDragNode(pattern->GetHost(), imageChildren, info);
            auto textDragPattern = pattern->dragNode_->GetPattern<TextDragPattern>();
            if (textDragPattern) {
                auto option = pattern->GetHost()->GetDragPreviewOption();
                option.options.shadowPath = textDragPattern->GetBackgroundPath()->ConvertToSVGString();
                option.options.shadow = Shadow(RICH_DEFAULT_ELEVATION, {0.0, 0.0}, Color(RICH_DEFAULT_SHADOW_COLOR),
                ShadowStyle::OuterFloatingSM);
                pattern->GetHost()->SetDragPreviewOptions(option);
            }
            FrameNode::ProcessOffscreenNode(pattern->dragNode_);
        }
    };
}

const std::list<RefPtr<UINode>>& TextPattern::GetAllChildren() const
{
    return childNodes_;
}

std::u16string TextPattern::GetSelectedSpanText(std::u16string value, int32_t start, int32_t end) const
{
    if (start < 0 || end > static_cast<int32_t>(value.length()) || start >= end) {
        return u"";
    }
    auto min = std::min(start, end);
    auto max = std::max(start, end);

    return value.substr(min, max - min);
}

TextStyleResult TextPattern::GetTextStyleObject(const RefPtr<SpanNode>& node)
{
    TextStyleResult textStyle;
    textStyle.fontColor = node->GetTextColorValue(Color::BLACK).ColorToString();
    textStyle.fontStyle = static_cast<int32_t>(node->GetItalicFontStyleValue(OHOS::Ace::FontStyle::NORMAL));
    textStyle.fontWeight = static_cast<int32_t>(node->GetFontWeightValue(FontWeight::NORMAL));
    std::string fontFamilyValue;
    const std::vector<std::string> defaultFontFamily = { "HarmonyOS Sans" };
    auto fontFamily = node->GetFontFamilyValue(defaultFontFamily);
    for (const auto& str : fontFamily) {
        fontFamilyValue += str;
        fontFamilyValue += ",";
    }
    fontFamilyValue =
        fontFamilyValue.substr(0, !fontFamilyValue.empty() ? static_cast<int32_t>(fontFamilyValue.size()) - 1 : 0);
    textStyle.fontFamily = !fontFamilyValue.empty() ? fontFamilyValue : defaultFontFamily.front();
    textStyle.decorationType = static_cast<int32_t>(node->GetTextDecorationValue(TextDecoration::NONE));
    textStyle.decorationColor = node->GetTextDecorationColorValue(Color::BLACK).ColorToString();
    textStyle.decorationStyle = static_cast<int32_t>(node->GetTextDecorationStyleValue(TextDecorationStyle::SOLID));
    textStyle.textAlign = static_cast<int32_t>(node->GetTextAlignValue(TextAlign::START));
    auto lm = node->GetLeadingMarginValue({});
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        textStyle.fontSize = node->GetFontSizeValue(Dimension(16.0f, DimensionUnit::VP)).ConvertToFp();
        textStyle.lineHeight = node->GetLineHeightValue(Dimension()).ConvertToFp();
        textStyle.letterSpacing = node->GetLetterSpacingValue(Dimension()).ConvertToFp();
        textStyle.lineSpacing = node->GetLineSpacingValue(Dimension()).ConvertToFp();
    } else {
        textStyle.fontSize = node->GetFontSizeValue(Dimension(16.0f, DimensionUnit::VP)).ConvertToVp();
        textStyle.lineHeight = node->GetLineHeightValue(Dimension()).ConvertToVp();
        textStyle.letterSpacing = node->GetLetterSpacingValue(Dimension()).ConvertToVp();
        textStyle.lineSpacing = node->GetLineSpacingValue(Dimension()).ConvertToVp();
    }
    textStyle.fontFeature = node->GetFontFeatureValue(ParseFontFeatureSettings("\"pnum\" 1"));
    textStyle.leadingMarginSize[RichEditorLeadingRange::LEADING_START] = lm.size.Width().ToString();
    textStyle.leadingMarginSize[RichEditorLeadingRange::LEADING_END] = lm.size.Height().ToString();
    textStyle.wordBreak = static_cast<int32_t>(node->GetWordBreakValue(WordBreak::BREAK_WORD));
    textStyle.lineBreakStrategy = static_cast<int32_t>(node->GetLineBreakStrategyValue(LineBreakStrategy::GREEDY));
    textStyle.textShadows = node->GetTextShadowValue({});
    textStyle.textBackgroundStyle = node->GetTextBackgroundStyle();
    return textStyle;
}

RefPtr<UINode> TextPattern::GetChildByIndex(int32_t index) const
{
    const auto& children = childNodes_;
    int32_t size = static_cast<int32_t>(children.size());
    if (index < 0 || index >= size) {
        return nullptr;
    }
    auto pos = children.begin();
    std::advance(pos, index);
    return *pos;
}

RefPtr<SpanItem> TextPattern::GetSpanItemByIndex(int32_t index) const
{
    int32_t size = static_cast<int32_t>(spans_.size());
    if (index < 0 || index >= size) {
        return nullptr;
    }
    auto pos = spans_.begin();
    std::advance(pos, index);
    return *pos;
}

ResultObject TextPattern::GetTextResultObject(RefPtr<UINode> uinode, int32_t index, int32_t start, int32_t end)
{
    bool selectFlag = false;
    ResultObject resultObject;
    if (!DynamicCast<SpanNode>(uinode)) {
        return resultObject;
    }
    auto spanItem = DynamicCast<SpanNode>(uinode)->GetSpanItem();
    int32_t itemLength = static_cast<int32_t>(spanItem->content.length());
    int32_t endPosition = std::min(GetTextContentLength(), spanItem->position);
    int32_t startPosition = endPosition - itemLength;

    if (startPosition >= start && endPosition <= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
    } else if (startPosition < start && endPosition <= end && endPosition > start) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = start - startPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
    } else if (startPosition >= start && startPosition < end && endPosition >= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = end - startPosition;
    } else if (startPosition <= start && endPosition >= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = start - startPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = end - startPosition;
    }
    if (selectFlag) {
        resultObject.spanPosition.spanIndex = index;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGESTART] = startPosition;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGEEND] = endPosition;
        resultObject.type = SelectSpanType::TYPESPAN;
        SetResultObjectText(resultObject, spanItem);
        auto spanNode = DynamicCast<SpanNode>(uinode);
        resultObject.textStyle = GetTextStyleObject(spanNode);
    }
    return resultObject;
}

void TextPattern::SetResultObjectText(ResultObject& resultObject, const RefPtr<SpanItem>& spanItem)
{
    CHECK_NULL_VOID(spanItem);
    resultObject.valueString = spanItem->content;
}

ResultObject TextPattern::GetSymbolSpanResultObject(RefPtr<UINode> uinode, int32_t index, int32_t start, int32_t end)
{
    bool selectFlag = false;
    ResultObject resultObject;
    resultObject.isDraggable = false;
    if (!DynamicCast<SpanNode>(uinode)) {
        return resultObject;
    }
    auto spanItem = DynamicCast<SpanNode>(uinode)->GetSpanItem();
    int32_t itemLength = static_cast<int32_t>(spanItem->content.length());
    int32_t endPosition = std::min(GetTextContentLength(), spanItem->position);
    int32_t startPosition = endPosition - itemLength;

    if (startPosition >= start && endPosition <= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
    } else if (startPosition < start && endPosition <= end && endPosition > start) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = start - startPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
    } else if (startPosition >= start && startPosition < end && endPosition >= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = end - startPosition;
    } else if (startPosition <= start && endPosition >= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = start - startPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = end - startPosition;
    }
    if (selectFlag) {
        resultObject.valueResource = spanItem->GetResourceObject();
        resultObject.spanPosition.spanIndex = index;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGESTART] = startPosition;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGEEND] = endPosition;
        resultObject.type = SelectSpanType::TYPESYMBOLSPAN;
        resultObject.valueString = UtfUtils::Str8ToStr16(std::to_string(spanItem->unicode));
        auto spanNode = DynamicCast<SpanNode>(uinode);
        resultObject.symbolSpanStyle = GetSymbolSpanStyleObject(spanNode);
    }
    return resultObject;
}

SymbolSpanStyle TextPattern::GetSymbolSpanStyleObject(const RefPtr<SpanNode>& node)
{
    SymbolSpanStyle symbolSpanStyle;
    std::string symbolColorValue;
    auto symbolColors = node->GetSymbolColorList();
    for (const auto& color : *symbolColors) {
        symbolColorValue += color.ColorToString() + ",";
    }
    symbolColorValue =
        symbolColorValue.substr(0, !symbolColorValue.empty() ? static_cast<int32_t>(symbolColorValue.size()) - 1 : 0);
    symbolSpanStyle.symbolColor = !symbolColorValue.empty() ? symbolColorValue : SYMBOL_COLOR;
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        symbolSpanStyle.fontSize = node->GetFontSizeValue(Dimension(DIMENSION_VALUE, DimensionUnit::VP)).ConvertToFp();
    } else {
        symbolSpanStyle.fontSize = node->GetFontSizeValue(Dimension(DIMENSION_VALUE, DimensionUnit::VP)).ConvertToVp();
    }
    symbolSpanStyle.fontWeight = static_cast<int32_t>(node->GetFontWeightValue(FontWeight::NORMAL));
    symbolSpanStyle.renderingStrategy = static_cast<int32_t>(node->GetSymbolRenderingStrategyValue(0));
    symbolSpanStyle.effectStrategy = static_cast<int32_t>(node->GetSymbolEffectStrategyValue(0));
    return symbolSpanStyle;
}

ResultObject TextPattern::GetImageResultObject(RefPtr<UINode> uinode, int32_t index, int32_t start, int32_t end)
{
    int32_t itemLength = 1;
    ResultObject resultObject;
    if (!DynamicCast<FrameNode>(uinode) || !GetSpanItemByIndex(index)) {
        return resultObject;
    }
    int32_t endPosition = std::min(GetTextContentLength(), GetSpanItemByIndex(index)->position);
    int32_t startPosition = endPosition - itemLength;
    if ((start <= startPosition) && (end >= endPosition)) {
        auto imageNode = DynamicCast<FrameNode>(uinode);
        auto imageLayoutProperty = DynamicCast<ImageLayoutProperty>(imageNode->GetLayoutProperty());
        resultObject.spanPosition.spanIndex = index;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGESTART] = startPosition;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGEEND] = endPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
        resultObject.type = SelectSpanType::TYPEIMAGE;
        if (!imageLayoutProperty->GetImageSourceInfo()->GetPixmap()) {
            resultObject.valueString = UtfUtils::Str8ToStr16(imageLayoutProperty->GetImageSourceInfo()->GetSrc());
        } else {
            resultObject.valuePixelMap = imageLayoutProperty->GetImageSourceInfo()->GetPixmap();
        }
        auto geometryNode = imageNode->GetGeometryNode();
        resultObject.imageStyle.size[RichEditorImageSize::SIZEWIDTH] = geometryNode->GetMarginFrameSize().Width();
        resultObject.imageStyle.size[RichEditorImageSize::SIZEHEIGHT] = geometryNode->GetMarginFrameSize().Height();
        if (imageLayoutProperty->HasImageFit()) {
            resultObject.imageStyle.objectFit = static_cast<int32_t>(imageLayoutProperty->GetImageFitValue());
        }
        if (imageLayoutProperty->HasVerticalAlign()) {
            resultObject.imageStyle.verticalAlign = static_cast<int32_t>(imageLayoutProperty->GetVerticalAlignValue());
        }
        if (imageLayoutProperty->GetMarginProperty()) {
            resultObject.imageStyle.margin = imageLayoutProperty->GetMarginProperty()->ToString();
        }
        auto imageRenderCtx = imageNode->GetRenderContext();
        if (imageRenderCtx->GetBorderRadius()) {
            BorderRadiusProperty brp;
            auto jsonObject = JsonUtil::Create(true);
            auto jsonBorder = JsonUtil::Create(true);
            InspectorFilter emptyFilter;
            imageRenderCtx->GetBorderRadiusValue(brp).ToJsonValue(jsonObject, jsonBorder, emptyFilter);
            resultObject.imageStyle.borderRadius = jsonObject->GetValue("borderRadius")->IsObject()
                                                       ? jsonObject->GetValue("borderRadius")->ToString()
                                                       : jsonObject->GetString("borderRadius");
        }
    }
    return resultObject;
}

// ===========================================================
// TextDragBase implementations
float TextPattern::GetLineHeight() const
{
    auto selectedRects = pManager_->GetRects(textSelector_.GetTextStart(), textSelector_.GetTextEnd());
    CHECK_NULL_RETURN(selectedRects.size(), {});
    return selectedRects.front().Height();
}

std::vector<RectF> TextPattern::GetTextBoxes()
{
    return pManager_->GetRects(textSelector_.GetTextStart(), textSelector_.GetTextEnd());
}

OffsetF TextPattern::GetParentGlobalOffset() const
{
    selectOverlay_->UpdateHandleGlobalOffset();
    auto host = GetHost();
    CHECK_NULL_RETURN(host, {});
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, {});
    auto rootOffset = pipeline->GetRootRect().GetOffset();
    return host->GetPaintRectOffset() - rootOffset;
}

void TextPattern::CreateHandles()
{
    if (IsDragging()) {
        TAG_LOGI(AceLogTag::ACE_TEXT, "do not show handles when dragging");
        return;
    }
    ShowSelectOverlay({ .menuIsShow = false });
}

bool TextPattern::BetweenSelectedPosition(const Offset& globalOffset)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto offset = host->GetPaintRectOffset();
    auto localOffset = globalOffset - Offset(offset.GetX(), offset.GetY());
    if (selectOverlay_->HasRenderTransform()) {
        localOffset = ConvertGlobalToLocalOffset(globalOffset);
    }
    return IsDraggable(localOffset);
}

// end of TextDragBase implementations
// ===========================================================

void TextPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto nowTime = static_cast<unsigned long long>(GetSystemTimestamp());
    ACE_TEXT_SCOPED_TRACE("OnModifyDone[Text][id:%d][time:%llu]", host->GetId(), nowTime);
    DumpRecord("OnModifyDone:" + std::to_string(nowTime));
    auto pipeline = host->GetContext();
    if (!(pipeline && pipeline->GetMinPlatformVersion() > API_PROTEXTION_GREATER_NINE)) {
        bool shouldClipToContent =
            textLayoutProperty->GetTextOverflow().value_or(TextOverflow::CLIP) == TextOverflow::CLIP;
        host->GetRenderContext()->SetClipToFrame(shouldClipToContent);
    }
    if (textLayoutProperty->GetTextOverflowValue(TextOverflow::CLIP) == TextOverflow::MARQUEE) {
        if (!renderContext->GetClipEdge().has_value()) {
            renderContext->UpdateClipEdge(true);
            renderContext->SetClipToFrame(true);
        }
        copyOption_ = CopyOptions::None;
    } else {
        copyOption_ = textLayoutProperty->GetCopyOption().value_or(CopyOptions::None);
    }

    const auto& children = host->GetChildren();
    if (children.empty()) {
        if (IsSetObscured() && !isSpanStringMode_) {
            copyOption_ = CopyOptions::None;
        }

        std::u16string textCache = textForDisplay_;
        if (!isSpanStringMode_) {
            textForDisplay_ = textLayoutProperty->GetContent().value_or(u"");
        }
        if (textCache != textForDisplay_) {
            host->OnAccessibilityEvent(AccessibilityEventType::TEXT_CHANGE, UtfUtils::Str16ToStr8(textCache),
                UtfUtils::Str16ToStr8(textForDisplay_));
            dataDetectorAdapter_->aiDetectInitialized_ = false;
            CloseSelectOverlay();
            ResetSelection();
        }

        if (CanStartAITask() && !dataDetectorAdapter_->aiDetectInitialized_) {
            ParseOriText(textForDisplay_);
        }
    }
    if (copyOption_ == CopyOptions::None) {
        CloseSelectOverlay();
        ResetSelection();
    }
    if (children.empty() && CanStartAITask() && !dataDetectorAdapter_->aiDetectInitialized_) {
        dataDetectorAdapter_->textForAI_ = textForDisplay_;
        dataDetectorAdapter_->StartAITask();
    }
    ProcessMarqueeVisibleAreaCallback();
    auto gestureEventHub = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureEventHub);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    if (IsSelectableAndCopy()) {
        auto context = host->GetContext();
        CHECK_NULL_VOID(context);
        if (!clipboard_ && context) {
            clipboard_ = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
        }
        InitLongPressEvent(gestureEventHub);
        if (host->IsDraggable() && !shiftFlag_) {
            InitDragEvent();
        }
        InitKeyEvent();
        InitMouseEvent();
        InitTouchEvent();
        SetAccessibilityAction();
    } else {
        if (host->IsDraggable() || gestureEventHub->GetTextDraggable()) {
            gestureEventHub->SetTextDraggable(false);
            eventHub->SetDefaultOnDragStart(nullptr);
            if (!eventHub->HasOnDragStart() && IsTextNode()) {
                gestureEventHub->RemoveDragEvent();
            }
        }
        if (longPressEvent_ && !hasSpanStringLongPressEvent_) {
            gestureEventHub->SetLongPressEvent(nullptr);
            longPressEvent_ = nullptr;
        }
    }
    if (onClick_ || IsSelectableAndCopy() || CanStartAITask()) {
        InitClickEvent(gestureEventHub);
        if (CanStartAITask()) {
            auto context = host->GetContext();
            CHECK_NULL_VOID(context);
            if (!clipboard_ && context) {
                clipboard_ = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
            }
            InitMouseEvent();
            InitAISpanHoverEvent();
        }
    }
    bool enabledCache = eventHub->IsEnabled();
    selectOverlay_->UpdateHandleColor();
    if (textDetectEnable_ && enabledCache != enabled_) {
        enabled_ = enabledCache;
        host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE);
    }
}

bool TextPattern::SetActionExecSubComponent()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto accessibilityProperty = host->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, false);
    accessibilityProperty->SetActionExecSubComponent([weakPtr = WeakClaim(this)](int32_t spanId) -> bool {
            const auto& pattern = weakPtr.Upgrade();
            CHECK_NULL_RETURN(pattern, false);
            return pattern->ExecSubComponent(spanId);
        });
    return true;
}

size_t TextPattern::GetSubComponentInfos(std::vector<SubComponentInfo>& subComponentInfos)
{
    subComponentInfos.clear();
    subComponentInfos_.clear();
    if (spans_.empty()) {
        GetSubComponentInfosForAISpans(subComponentInfos);
    } else {
        GetSubComponentInfosForSpans(subComponentInfos);
    }
    SetActionExecSubComponent();
    return subComponentInfos.size();
}

void TextPattern::GetSubComponentInfosForAISpans(std::vector<SubComponentInfo>& subComponentInfos)
{
    CHECK_NULL_VOID(dataDetectorAdapter_);
    for (const auto& kv : dataDetectorAdapter_->aiSpanMap_) {
        auto& aiSpan = kv.second;
        AddSubComponentInfoForAISpan(subComponentInfos, aiSpan.content, aiSpan);
    }
}

void TextPattern::GetSubComponentInfosForSpans(std::vector<SubComponentInfo>& subComponentInfos)
{
    for (const auto& span : spans_) {
        if (span == nullptr) {
            continue; // skip null
        }
        if ((span->imageNodeId >= 0) || (span->unicode > 0)) {
            continue; // skip ImageSpan and SymbolSpan
        }
        if (span->spanItemType == SpanItemType::CustomSpan) {
            continue; // skip CustomSpan
        }
        auto placeholderSpan = DynamicCast<PlaceholderSpanItem>(span);
        if ((placeholderSpan != nullptr) && (placeholderSpan->placeholderSpanNodeId >=0)) {
            continue; // skip PlaceholderSpan
        }
        if (span->content.empty()) {
            continue; // skip empty text
        }
        AddSubComponentInfoForSpan(subComponentInfos, UtfUtils::Str16ToStr8(span->content), span);
        AddSubComponentInfosByDataDetectorForSpan(subComponentInfos, span);
    }
}

void TextPattern::AddSubComponentInfosByDataDetectorForSpan(std::vector<SubComponentInfo>& subComponentInfos,
    const RefPtr<SpanItem>& span)
{
    CHECK_NULL_VOID(span);
    CHECK_NULL_VOID(dataDetectorAdapter_);
    int32_t wSpanContentLength = static_cast<int32_t>(span->content.length());
    int32_t spanStart = span->position - wSpanContentLength;
    if (span->needRemoveNewLine) {
        spanStart -= 1;
    }
    int32_t preEnd = spanStart;
    auto aiSpanMap = dataDetectorAdapter_->aiSpanMap_;
    while (!aiSpanMap.empty()) {
        auto aiSpan = aiSpanMap.begin()->second;
        if (aiSpan.start >= span->position || preEnd >= span->position) {
            break;
        }
        int32_t aiSpanStartInSpan = std::max(spanStart, aiSpan.start);
        int32_t aiSpanEndInSpan = std::min(span->position, aiSpan.end);
        if (aiSpan.end <= spanStart || aiSpanStartInSpan < preEnd) {
            TAG_LOGI(AceLogTag::ACE_TEXT, "Error prediction");
            aiSpanMap.erase(aiSpanMap.begin());
            continue;
        }
        AddSubComponentInfoForAISpan(subComponentInfos, aiSpan.content, aiSpan);
        preEnd = aiSpanEndInSpan;
        if (aiSpan.end > span->position) {
            return;
        } else {
            aiSpanMap.erase(aiSpanMap.begin());
        }
    }
}

bool TextPattern::ExecSubComponent(int32_t spanId)
{
    if ((spanId < 0) || (spanId >= static_cast<int32_t>(subComponentInfos_.size()))) {
        return false;
    }
    auto subComponentInfo = subComponentInfos_[spanId];
    if (subComponentInfo.aiSpan.has_value()) {
        CHECK_NULL_RETURN(dataDetectorAdapter_, false);
        dataDetectorAdapter_->ResponseBestMatchItem(subComponentInfo.aiSpan.value());
        return true;
    }
    const auto& span = subComponentInfo.span.Upgrade();
    CHECK_NULL_RETURN(span, false);
    CHECK_NULL_RETURN(span->onClick, false);
    GestureEvent info;
    std::chrono::microseconds microseconds(GetMicroTickCount());
    TimeStamp time(microseconds);
    info.SetTimeStamp(time);
    span->onClick(info);
    return true;
}

void TextPattern::AddSubComponentInfoForSpan(std::vector<SubComponentInfo>& subComponentInfos,
    const std::string& content, const RefPtr<SpanItem>& span)
{
    CHECK_NULL_VOID(span);
    CHECK_NULL_VOID(span->onClick); // skip null onClick
    SubComponentInfo subComponentInfo;
    subComponentInfo.spanId = static_cast<int32_t>(subComponentInfos.size());
    subComponentInfo.spanText = content;
    if (span->accessibilityProperty == nullptr) {
        subComponentInfo.accessibilityLevel = AccessibilityProperty::Level::AUTO;
    } else {
        subComponentInfo.accessibilityText = span->accessibilityProperty->GetAccessibilityText();
        subComponentInfo.accessibilityDescription =
            span->accessibilityProperty->GetAccessibilityDescription();
        subComponentInfo.accessibilityLevel = span->accessibilityProperty->GetAccessibilityLevel();
    }
    subComponentInfos.emplace_back(subComponentInfo);

    SubComponentInfoEx subComponentInfoEx;
    subComponentInfoEx.span = span;
    subComponentInfos_.emplace_back(subComponentInfoEx);
}

void TextPattern::AddSubComponentInfoForAISpan(std::vector<SubComponentInfo>& subComponentInfos,
    const std::string& content, const AISpan& aiSpan)
{
    SubComponentInfo subComponentInfo;
    subComponentInfo.spanId = static_cast<int32_t>(subComponentInfos.size());
    subComponentInfo.spanText = content;
    subComponentInfo.accessibilityLevel = AccessibilityProperty::Level::AUTO;
    subComponentInfos.emplace_back(subComponentInfo);

    SubComponentInfoEx subComponentInfoEx;
    subComponentInfoEx.aiSpan = aiSpan;
    subComponentInfos_.emplace_back(subComponentInfoEx);
}

void TextPattern::ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const
{
    /* no fixed attr below, just return */
    if (filter.IsFastFilter()) {
        return;
    }
    json->PutExtAttr("enableDataDetector", textDetectEnable_ ? "true" : "false", filter);
    json->PutExtAttr("dataDetectorConfig", dataDetectorAdapter_->textDetectConfigStr_.c_str(), filter);
    const auto& selector = GetTextSelector();
    auto result = "[" + std::to_string(selector.GetTextStart()) + "," + std::to_string(selector.GetTextEnd()) + "]";
    json->PutExtAttr("selection", result.c_str(), filter);
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProp);
    json->PutExtAttr("fontSize", GetFontSizeInJson(textLayoutProp->GetFontSize()).c_str(), filter);
    if (textStyle_.has_value() && textStyle_->GetAdaptTextSize()) {
        auto adaptedFontSize = textStyle_->GetFontSize();
        json->PutExtAttr("actualFontSize", adaptedFontSize.ToString().c_str(), filter);
    } else {
        json->PutExtAttr("actualFontSize", GetFontSizeInJson(textLayoutProp->GetFontSize()).c_str(), filter);
    }
    json->PutExtAttr("font", GetFontInJson().c_str(), filter);
    json->PutExtAttr("bindSelectionMenu", GetBindSelectionMenuInJson().c_str(), filter);
}

std::string TextPattern::GetBindSelectionMenuInJson() const
{
    auto jsonArray = JsonUtil::CreateArray(true);
    for (auto& [spanResponsePair, params] : selectionMenuMap_) {
        auto& [spanType, responseType] = spanResponsePair;
        auto jsonItem = JsonUtil::Create(true);
        jsonItem->Put("spanType", static_cast<int32_t>(spanType));
        jsonItem->Put("responseType", static_cast<int32_t>(responseType));
        jsonItem->Put("menuType", static_cast<int32_t>(SelectionMenuType::SELECTION_MENU));
        jsonArray->Put(jsonItem);
    }
    FillPreviewMenuInJson(jsonArray);
    return StringUtils::RestoreBackslash(jsonArray->ToString());
}

std::string TextPattern::GetFontInJson() const
{
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProp, "");
    auto jsonValue = JsonUtil::Create(true);
    jsonValue->Put("style", GetFontStyleInJson(textLayoutProp->GetItalicFontStyle()).c_str());
    jsonValue->Put("size", GetFontSizeInJson(textLayoutProp->GetFontSize()).c_str());
    jsonValue->Put("weight", GetFontWeightInJson(textLayoutProp->GetFontWeight()).c_str());
    jsonValue->Put("variableFontWeight", std::to_string(textLayoutProp->GetVariableFontWeight().value_or(0)).c_str());
    jsonValue->Put("enableVariableFontWeight",
                   textLayoutProp->GetEnableVariableFontWeight().value_or(false) ? "true" : "false");
    jsonValue->Put("family", GetFontFamilyInJson(textLayoutProp->GetFontFamily()).c_str());
    return jsonValue->ToString();
}

void TextPattern::ToTreeJson(std::unique_ptr<JsonValue>& json, const InspectorConfig& config) const
{
    Pattern::ToTreeJson(json, config);
    if (!json->Contains(TreeKey::CONTENT) && !textForDisplay_.empty()) {
        json->Put(TreeKey::CONTENT, textForDisplay_.c_str());
    }
}

void TextPattern::OnAfterModifyDone()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto inspectorId = host->GetInspectorId().value_or("");
    if (!inspectorId.empty()) {
        auto prop = host->GetAccessibilityProperty<NG::AccessibilityProperty>();
        Recorder::NodeDataCache::Get().PutString(host, inspectorId, prop->GetText());
    }
}

void TextPattern::ActSetSelection(int32_t start, int32_t end)
{
    int32_t min = 0;
    int32_t textSize = static_cast<int32_t>(textForDisplay_.length()) + placeholderCount_;
    start = start < min ? min : start;
    end = end < min ? min : end;
    start = start > textSize ? textSize : start;
    end = end > textSize ? textSize : end;
    if (start >= end) {
        ResetSelection();
        CloseSelectOverlay();
        return;
    }
    HandleSelectionChange(start, end);
    parentGlobalOffset_ = GetParentGlobalOffset();
    CalculateHandleOffsetAndShowOverlay();
    showSelected_ = true;
    if (textSelector_.firstHandle == textSelector_.secondHandle && pManager_) {
        ResetSelection();
        CloseSelectOverlay();
        return;
    }
    showSelected_ = false;
    if (IsShowHandle()) {
        ShowSelectOverlay();
    } else {
        CloseSelectOverlay();
        if (IsSelected()) {
            selectOverlay_->SetSelectionHoldCallback();
        }
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

bool TextPattern::IsShowHandle()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto theme = pipeline->GetTheme<TextTheme>();
    CHECK_NULL_RETURN(theme, false);
    return !theme->IsShowHandle();
}

Color TextPattern::GetUrlHoverColor()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, Color());
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, Color());
    auto theme = pipeline->GetTheme<TextTheme>();
    CHECK_NULL_RETURN(theme, Color());
    return theme->GetUrlHoverColor();
}

Color TextPattern::GetUrlPressColor()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, Color());
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, Color());
    auto theme = pipeline->GetTheme<TextTheme>();
    CHECK_NULL_RETURN(theme, Color());
    return theme->GetUrlPressColor();
}

Color TextPattern::GetUrlSpanColor()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, Color());
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, Color());
    auto theme = pipeline->GetTheme<TextTheme>();
    CHECK_NULL_RETURN(theme, Color());

    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_RETURN(eventHub, Color());

    if (eventHub && !eventHub->IsEnabled()) {
        return theme->GetUrlDisabledColor();
    } else {
        return theme->GetUrlDefaultColor();
    }
}

// Deprecated: Use the TextSelectOverlay::ProcessOverlay() instead.
// It is currently used by RichEditorPattern.
void TextPattern::UpdateSelectOverlayOrCreate(SelectOverlayInfo& selectInfo, bool animation)
{
    if (selectOverlayProxy_ && !selectOverlayProxy_->IsClosed()) {
        SelectHandleInfo firstHandleInfo;
        firstHandleInfo.paintRect = textSelector_.firstHandle;
        CheckHandles(firstHandleInfo);

        SelectHandleInfo secondHandleInfo;
        secondHandleInfo.paintRect = textSelector_.secondHandle;
        CheckHandles(secondHandleInfo);

        auto start = textSelector_.GetTextStart();
        auto end = textSelector_.GetTextEnd();
        selectOverlayProxy_->SetSelectInfo(UtfUtils::Str16ToStr8(GetSelectedText(start, end)));
        if (selectInfo.isNewAvoid) {
            selectOverlayProxy_->UpdateSelectArea(selectInfo.selectArea);
        }
        selectOverlayProxy_->UpdateFirstAndSecondHandleInfo(firstHandleInfo, secondHandleInfo);
        selectOverlayProxy_->ShowOrHiddenMenu(!firstHandleInfo.isShow && !secondHandleInfo.isShow);
    } else {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto pipeline = host->GetContext();
        CHECK_NULL_VOID(pipeline);
        pipeline->AddOnAreaChangeNode(host->GetId());
        selectInfo.callerFrameNode = GetHost();
        selectInfo.hitTestMode = HitTestMode::HTMDEFAULT;
        if (!selectInfo.isUsingMouse) {
            CheckHandles(selectInfo.firstHandle);
            CheckHandles(selectInfo.secondHandle);
        }
        selectOverlayProxy_ =
            pipeline->GetSelectOverlayManager()->CreateAndShowSelectOverlay(selectInfo, WeakClaim(this), animation);
        CHECK_NULL_VOID(selectOverlayProxy_);
        auto start = textSelector_.GetTextStart();
        auto end = textSelector_.GetTextEnd();
        selectOverlayProxy_->SetSelectInfo(UtfUtils::Str16ToStr8(GetSelectedText(start, end)));
    }
}

bool TextPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    if (config.skipMeasure || dirty->SkipMeasureContent()) {
        return false;
    }

    contentRect_ = dirty->GetGeometryNode()->GetContentRect();
    dataDetectorAdapter_->aiSpanRects_.clear();

    auto layoutAlgorithmWrapper = DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithmWrapper, false);
    auto textLayoutAlgorithm = DynamicCast<TextLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(textLayoutAlgorithm, false);
    baselineOffset_ = textLayoutAlgorithm->GetBaselineOffset();
    contentOffset_ = dirty->GetGeometryNode()->GetContentOffset();
    textStyle_ = textLayoutAlgorithm->GetTextStyle();
    ProcessOverlayAfterLayout();
    return true;
}

void TextPattern::ProcessOverlayAfterLayout()
{
    if (selectOverlay_->SelectOverlayIsOn() || showSelected_) {
        showSelected_ = false;
        CalculateHandleOffsetAndShowOverlay();
        selectOverlay_->UpdateAllHandlesOffset();
        selectOverlay_->UpdateViewPort();
    }
}

void TextPattern::PreCreateLayoutWrapper()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);

    auto paintProperty = GetPaintProperty<PaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto flag = paintProperty->GetPropertyChangeFlag();
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    auto layoutFlag = textLayoutProperty->GetPropertyChangeFlag();
    if (!CheckNeedMeasure(flag) && !CheckNeedMeasure(layoutFlag)) {
        return;
    }

    spans_.clear();
    childNodes_.clear();

    // When dirty areas are marked because of child node changes, the text rendering node tree is reset.
    const auto& children = host->GetChildren();
    if (children.empty()) {
        placeholderCount_ = 0;
        return;
    }

    // Depth-first iterates through all host's child nodes to collect the SpanNode object, building a text rendering
    // tree.
    std::stack<SpanNodeInfo> nodes;
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
        nodes.push({ .node = *iter });
    }

    InitSpanItem(nodes);
}

void TextPattern::InitSpanItem(std::stack<SpanNodeInfo> nodes)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    std::u16string textCache;
    std::u16string textForAICache;
    int32_t oldPlaceholderCount = placeholderCount_;
    placeholderCount_ = 0;
    if (!nodes.empty()) {
        textCache = textForDisplay_;
        textForAICache = dataDetectorAdapter_->textForAI_;
        textForDisplay_.clear();
        dataDetectorAdapter_->textForAI_.clear();
    }

    bool isSpanHasClick = false;
    CollectSpanNodes(nodes, isSpanHasClick);
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    if (childNodes_.empty()) {
        textForDisplay_ = textLayoutProperty->GetContent().value_or(u"");
    }
    if (oldPlaceholderCount != placeholderCount_) {
        CloseSelectOverlay();
        ResetSelection();
    }

    if (textCache != textForDisplay_) {
        host->OnAccessibilityEvent(AccessibilityEventType::TEXT_CHANGE, UtfUtils::Str16ToStr8(textCache),
            UtfUtils::Str16ToStr8(textForDisplay_));
        OnAfterModifyDone();
        for (const auto& item : spans_) {
            if (item->inspectId.empty()) {
                continue;
            }
            Recorder::NodeDataCache::Get().PutString(host, item->inspectId, UtfUtils::Str16ToStr8(item->content));
        }
        ResetAfterTextChange();
    }
    if (isSpanHasClick) {
        auto gestureEventHub = host->GetOrCreateGestureEventHub();
        InitClickEvent(gestureEventHub);
    }
    if (textForAICache != dataDetectorAdapter_->textForAI_) {
        dataDetectorAdapter_->aiDetectInitialized_ = false;
    }
    if (CanStartAITask() && !dataDetectorAdapter_->aiDetectInitialized_) {
        ParseOriText(textLayoutProperty->GetContent().value_or(u""));
        if (!dataDetectorAdapter_->aiDetectInitialized_) {
            dataDetectorAdapter_->StartAITask();
        }
    }
}

void TextPattern::ResetAfterTextChange()
{
    CloseSelectOverlay();
    ResetSelection();
}

void TextPattern::ParseOriText(const std::u16string& currentText)
{
    auto entityJson = JsonUtil::ParseJsonString(UtfUtils::Str16ToStr8(currentText));
    bool entityIsJson = !entityJson->IsNull();
    TAG_LOGI(AceLogTag::ACE_TEXT, "text content is the json format: %{public}d", entityIsJson);
    if (entityIsJson && !entityJson->GetValue("bundleName")->IsNull() &&
        dataDetectorAdapter_->ParseOriText(entityJson, textForDisplay_)) {
        if (childNodes_.empty()) {
            auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(textLayoutProperty);
            textLayoutProperty->UpdateContent(textForDisplay_);
        }
    }
}

void TextPattern::BeforeCreateLayoutWrapper()
{
    if (!isSpanStringMode_) {
        PreCreateLayoutWrapper();
    }
    selectOverlay_->MarkOverlayDirty();
}

void TextPattern::CollectSpanNodes(std::stack<SpanNodeInfo> nodes, bool& isSpanHasClick)
{
    while (!nodes.empty()) {
        auto current = nodes.top();
        nodes.pop();
        if (!current.node) {
            continue;
        }
        UpdateContainerChildren(current.containerSpanNode, current.node);
        auto spanNode = DynamicCast<SpanNode>(current.node);
        auto tag = current.node->GetTag();
        if (spanNode && tag == V2::SYMBOL_SPAN_ETS_TAG && spanNode->GetSpanItem()->GetSymbolUnicode() != 0) {
            spanNode->CleanSpanItemChildren();
            spanNode->MountToParagraph();
            textForDisplay_.append(u"    ");
            dataDetectorAdapter_->textForAI_.append(SYMBOL_TRANS);
            childNodes_.push_back(current.node);
        } else if (spanNode && tag != V2::PLACEHOLDER_SPAN_ETS_TAG) {
            CollectTextSpanNodes(spanNode, isSpanHasClick);
            childNodes_.push_back(current.node);
        } else if (tag == V2::IMAGE_ETS_TAG || tag == V2::PLACEHOLDER_SPAN_ETS_TAG) {
            placeholderCount_++;
            AddChildSpanItem(current.node);
            dataDetectorAdapter_->textForAI_.append(u"\n");
            auto imageNode = DynamicCast<FrameNode>(current.node);
            if (!imageNode) {
                continue;
            }
            auto focus_hub = imageNode->GetOrCreateFocusHub();
            if (focus_hub && focus_hub->GetOnClickCallback()) {
                isSpanHasClick = true;
            }
            childNodes_.push_back(current.node);
        } else if (tag == V2::CUSTOM_SPAN_NODE_ETS_TAG) {
            placeholderCount_++;
            AddChildSpanItem(current.node);
            dataDetectorAdapter_->textForAI_.append(u"\n");
            childNodes_.emplace_back(current.node);
        }
        if (tag == V2::PLACEHOLDER_SPAN_ETS_TAG) {
            continue;
        }
        const auto& nextChildren = current.node->GetChildren();
        if (nextChildren.empty()) {
            continue;
        }
        auto containerSpanNode = tag == V2::CONTAINER_SPAN_ETS_TAG ? current.node : current.containerSpanNode;
        for (auto iter = nextChildren.rbegin(); iter != nextChildren.rend(); ++iter) {
            nodes.push({ .node = *iter, .containerSpanNode = containerSpanNode });
        }
    }
}

void TextPattern::CollectTextSpanNodes(const RefPtr<SpanNode>& spanNode, bool& isSpanHasClick)
{
    spanNode->CleanSpanItemChildren();
    spanNode->MountToParagraph();
    textForDisplay_.append(spanNode->GetSpanItem()->content);
    dataDetectorAdapter_->textForAI_.append(spanNode->GetSpanItem()->content);
    if (spanNode->GetSpanItem()->onClick) {
        isSpanHasClick = true;
    }
}

void TextPattern::UpdateContainerChildren(const RefPtr<UINode>& parentNode, const RefPtr<UINode>& child)
{
    CHECK_NULL_VOID(child);
    auto parent = DynamicCast<ContainerSpanNode>(parentNode);
    CHECK_NULL_VOID(parent);
    auto baseSpan = DynamicCast<BaseSpan>(child);
    if (baseSpan) {
        if (baseSpan->HasTextBackgroundStyle()) {
            return;
        }
        baseSpan->UpdateTextBackgroundFromParent(parent->GetTextBackgroundStyle());
        return;
    }
    if (child->GetTag() == V2::IMAGE_ETS_TAG) {
        auto imageNode = DynamicCast<FrameNode>(child);
        CHECK_NULL_VOID(imageNode);
        auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
        CHECK_NULL_VOID(imageLayoutProperty);
        if (imageLayoutProperty->GetHasPlaceHolderStyleValue(false)) {
            return;
        }
        if (parent->GetTextBackgroundStyle().has_value()) {
            imageLayoutProperty->UpdatePlaceHolderStyle(parent->GetTextBackgroundStyle().value());
        }
    }
}

void TextPattern::GetGlobalOffset(Offset& offset)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto rootOffset = pipeline->GetRootRect().GetOffset();
    auto globalOffset = host->GetPaintRectOffset() - rootOffset;
    offset = Offset(globalOffset.GetX(), globalOffset.GetY());
}

void TextPattern::OnVisibleChange(bool isVisible)
{
    if (!isVisible) {
        if (textSelector_.IsValid()) {
            CloseSelectOverlay();
            ResetSelection();
        }
        if (textDetectEnable_) {
            dataDetectorAdapter_->aiDetectDelayTask_.Cancel();
        }
    } else {
        if (CanStartAITask()) {
            dataDetectorAdapter_->StartAITask();
        }
    }
}

void TextPattern::InitSurfaceChangedCallback()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    if (!HasSurfaceChangedCallback()) {
        auto callbackId = pipeline->RegisterSurfaceChangedCallback(
            [weak = WeakClaim(this)](int32_t newWidth, int32_t newHeight, int32_t prevWidth, int32_t prevHeight,
                WindowSizeChangeReason type) {
                auto pattern = weak.Upgrade();
                if (pattern) {
                    pattern->HandleSurfaceChanged(newWidth, newHeight, prevWidth, prevHeight);
                }
            });
        UpdateSurfaceChangedCallbackId(callbackId);
    }
}

void TextPattern::HandleSurfaceChanged(int32_t newWidth, int32_t newHeight, int32_t prevWidth, int32_t prevHeight)
{
    if (newWidth == prevWidth && newHeight == prevHeight) {
        return;
    }
    CHECK_NULL_VOID(selectOverlay_->SelectOverlayIsOn());
    if (selectOverlay_->IsShowMouseMenu()) {
        CloseSelectOverlay();
    } else {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto context = host->GetContext();
        if (context) {
            context->AddAfterLayoutTask([weak = WeakClaim(this)]() {
                auto pattern = weak.Upgrade();
                CHECK_NULL_VOID(pattern);
                pattern->CalculateHandleOffsetAndShowOverlay();
                pattern->ShowSelectOverlay({ .menuIsShow = false });
            });
        }
    }
}

void TextPattern::InitSurfacePositionChangedCallback()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    if (!HasSurfacePositionChangedCallback()) {
        auto callbackId =
            pipeline->RegisterSurfacePositionChangedCallback([weak = WeakClaim(this)](int32_t posX, int32_t posY) {
                auto pattern = weak.Upgrade();
                if (pattern) {
                    pattern->HandleSurfacePositionChanged(posX, posY);
                }
            });
        UpdateSurfacePositionChangedCallbackId(callbackId);
    }
}

void TextPattern::AddChildSpanItem(const RefPtr<UINode>& child)
{
    CHECK_NULL_VOID(child);
    auto chidNode = DynamicCast<FrameNode>(child);
    if (chidNode && chidNode->GetLayoutProperty() && chidNode->GetLayoutProperty()->IsOverlayNode()) {
        return;
    }

    if (child->GetTag() == V2::SPAN_ETS_TAG || child->GetTag() == V2::SYMBOL_SPAN_ETS_TAG) {
        auto spanNode = DynamicCast<SpanNode>(child);
        if (spanNode) {
            spans_.emplace_back(spanNode->GetSpanItem());
        }
    } else if (child->GetTag() == V2::IMAGE_ETS_TAG) {
        AddImageToSpanItem(child);
    } else if (child->GetTag() == V2::PLACEHOLDER_SPAN_ETS_TAG) {
        auto placeholderSpanNode = DynamicCast<PlaceholderSpanNode>(child);
        if (placeholderSpanNode) {
            auto placeholderSpan = placeholderSpanNode->GetSpanItem();
            placeholderSpan->placeholderSpanNodeId = placeholderSpanNode->GetId();
            spans_.emplace_back(placeholderSpan);
        }
    } else if (child->GetTag() == V2::CUSTOM_SPAN_NODE_ETS_TAG) {
        auto customSpanNode = DynamicCast<CustomSpanNode>(child);
        if (customSpanNode) {
            auto customSpan = customSpanNode->GetSpanItem();
            customSpan->placeholderSpanNodeId = customSpanNode->GetId();
            spans_.emplace_back(customSpan);
        }
    }
}

void TextPattern::AddImageToSpanItem(const RefPtr<UINode>& child)
{
    auto imageSpanNode = DynamicCast<ImageSpanNode>(child);
    if (imageSpanNode) {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto imageSpanItem = imageSpanNode->GetSpanItem();
        if (host->GetTag() != V2::RICH_EDITOR_ETS_TAG) {
            auto focus_hub = imageSpanNode->GetOrCreateFocusHub();
            CHECK_NULL_VOID(focus_hub);
            auto clickCall = focus_hub->GetOnClickCallback();
            if (clickCall) {
                imageSpanItem->SetOnClickEvent(std::move(clickCall));
            }
            auto gesture = imageSpanNode->GetOrCreateGestureEventHub();
            CHECK_NULL_VOID(gesture);
            gesture->SetHitTestMode(HitTestMode::HTMNONE);
        }
        imageSpanItem->UpdatePlaceholderBackgroundStyle(imageSpanNode);
        spans_.emplace_back(imageSpanItem);
        spans_.back()->imageNodeId = imageSpanNode->GetId();
        return;
    }
    auto imageNode = DynamicCast<FrameNode>(child);
    if (imageNode) {
        auto imageSpanItem = MakeRefPtr<ImageSpanItem>();
        imageSpanItem->imageNodeId = imageNode->GetId();
        imageSpanItem->UpdatePlaceholderBackgroundStyle(imageNode);
        auto focus_hub = imageNode->GetOrCreateFocusHub();
        CHECK_NULL_VOID(focus_hub);
        auto clickCall = focus_hub->GetOnClickCallback();
        if (clickCall) {
            imageSpanItem->SetOnClickEvent(std::move(clickCall));
        }
        spans_.emplace_back(imageSpanItem);
        auto gesture = imageNode->GetOrCreateGestureEventHub();
        CHECK_NULL_VOID(gesture);
        gesture->SetHitTestMode(HitTestMode::HTMNONE);
        return;
    }
}

void TextPattern::DumpAdvanceInfo()
{
    DumpLog::GetInstance().AddDesc(std::string("-----DumpAdvanceInfo-----"));
    DumpLog::GetInstance().AddDesc(
        std::string("BindSelectionMenu: ").append(std::to_string(selectionMenuMap_.empty())));
    DumpLog::GetInstance().AddDesc(std::string("Selection: ").append("(").append(textSelector_.ToString()).append(")"));
}

void TextPattern::DumpInfo()
{
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProp);
    auto& dumpLog = DumpLog::GetInstance();
    auto nowTime = GetSystemTimestamp();
    dumpLog.AddDesc(std::string("frameRecord: ").append(frameRecord_));
    dumpLog.AddDesc(std::string("time: ").append(std::to_string(nowTime)));
    if (!IsSetObscured()) {
        dumpLog.AddDesc(std::string("Content: ").append(
            UtfUtils::Str16ToStr8(textLayoutProp->GetContent().value_or(u" "))));
    }
    dumpLog.AddDesc(std::string("isSpanStringMode: ").append(std::to_string(isSpanStringMode_)));
    dumpLog.AddDesc(std::string("externalParagraph: ").append(std::to_string(externalParagraph_.has_value())));
    DumpTextStyleInfo();
    if (contentMod_) {
        contentMod_->ContentModifierDump();
    }
    dumpLog.AddDesc(
        std::string("HeightAdaptivePolicy: ")
            .append(V2::ConvertWrapTextHeightAdaptivePolicyToString(
                textLayoutProp->GetHeightAdaptivePolicy().value_or(TextHeightAdaptivePolicy::MAX_LINES_FIRST))));
    if (pManager_) {
        auto num = static_cast<int32_t>(pManager_->GetParagraphs().size());
        dumpLog.AddDesc(std::string("Paragraphs num: ").append(std::to_string(num)));
        dumpLog.AddDesc(std::string("PaintInfo: ").append(paintInfo_));
    }
    DumpScaleInfo();
    DumpTextEngineInfo();
    if (SystemProperties::GetDebugEnabled()) {
        DumpAdvanceInfo();
    }
    DumpSpanItem();
}

void TextPattern::DumpSpanItem()
{
    CHECK_NULL_VOID(isSpanStringMode_);
    auto& dumpLog = DumpLog::GetInstance();
    dumpLog.AddDesc(std::string("-----SpanDumpInfo-----"));
    for (const auto& item : spans_) {
        if (!item) {
            continue;
        }
        item->SpanDumpInfo();
    }
}

void TextPattern::DumpTextStyleInfo()
{
    auto& dumpLog = DumpLog::GetInstance();
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProp);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    dumpLog.AddDesc(
        std::string("FontColor: ")
            .append((textStyle_.has_value() ? textStyle_->GetTextColor() : Color::BLACK).ColorToString())
            .append(" pro: ")
            .append(
            textLayoutProp->HasTextColor() ? textLayoutProp->GetTextColorValue(Color::BLACK).ColorToString() : "Na")
            .append(" ForegroundColor: ")
            .append(
            renderContext->HasForegroundColor() ? renderContext->GetForegroundColorValue().ColorToString() : "Na"));
    if (renderContext->HasForegroundColorStrategy()) {
        auto strategy = static_cast<int32_t>(renderContext->GetForegroundColorStrategyValue());
        DumpLog::GetInstance().AddDesc(std::string("ForegroundColorStrategy: ").append(std::to_string(strategy)));
    }
    dumpLog.AddDesc(
        std::string("FontSize: ")
            .append((textStyle_.has_value() ? textStyle_->GetFontSize() : Dimension(DIMENSION_VALUE, DimensionUnit::FP))
                        .ToString())
            .append(" pro: ")
            .append(textLayoutProp->HasFontSize()
                        ? textLayoutProp->GetFontSizeValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                        : "Na"));
    if (textStyle_.has_value()) {
        dumpLog.AddDesc(
            std::string("MaxFontSize: ")
                .append(textStyle_->GetAdaptMaxFontSize().ToString())
                .append(" pro: ")
                .append(textLayoutProp->HasAdaptMaxFontSize()
                            ? textLayoutProp->GetAdaptMaxFontSizeValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                            : "Na"));
        dumpLog.AddDesc(
            std::string("MinFontSize: ")
                .append(textStyle_->GetAdaptMinFontSize().ToString())
                .append(" pro: ")
                .append(textLayoutProp->HasAdaptMinFontSize()
                            ? textLayoutProp->GetAdaptMinFontSizeValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                            : "Na"));
    }
    DumpTextStyleInfo2();
    DumpTextStyleInfo3();
}

void TextPattern::DumpTextStyleInfo2()
{
    auto& dumpLog = DumpLog::GetInstance();
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProp);
    if (textStyle_.has_value()) {
        dumpLog.AddDesc(std::string("FontWeight: ")
                .append(StringUtils::ToString(textStyle_->GetFontWeight()))
                .append(" pro: ")
                .append(textLayoutProp->HasFontWeight()
                            ? StringUtils::ToString(textLayoutProp->GetFontWeightValue(FontWeight::NORMAL))
                            : "Na"));
        dumpLog.AddDesc(std::string("FontStyle: ").append(StringUtils::ToString(textStyle_->GetFontStyle())));
        dumpLog.AddDesc(
            std::string("LineHeight: ")
                .append(textStyle_->GetLineHeight().ToString())
                .append(" pro: ")
                .append(textLayoutProp->HasLineHeight()
                            ? textLayoutProp->GetLineHeightValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                            : "Na"));
        dumpLog.AddDesc(
            std::string("LineSpacing: ")
                .append(textStyle_->GetLineSpacing().ToString())
                .append(" pro: ").append(textLayoutProp->HasLineSpacing()
                            ? textLayoutProp->GetLineSpacingValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                            : "Na"));
        dumpLog.AddDesc(
            std::string("maxLines: ")
                .append(std::to_string(textStyle_->GetMaxLines()))
                .append(" pro: ")
                .append(textLayoutProp->HasMaxLines() ? std::to_string(textLayoutProp->GetMaxLinesValue(UINT32_MAX))
                                                      : "Na"));
        dumpLog.AddDesc(
            std::string("BaselineOffset: ")
                .append(textStyle_->GetBaselineOffset().ToString())
                .append(" pro: ")
                .append(textLayoutProp->HasBaselineOffset()
                            ? textLayoutProp->GetBaselineOffsetValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                            : "Na"));
        dumpLog.AddDesc(
            std::string("TextIndent: ")
                .append(textStyle_->GetTextIndent().ToString())
                .append(" pro: ")
                .append(textLayoutProp->HasTextIndent()
                            ? textLayoutProp->GetTextIndentValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                            : "Na"));
    }
}

void TextPattern::DumpTextStyleInfo3()
{
    auto& dumpLog = DumpLog::GetInstance();
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProp);
    if (textStyle_.has_value()) {
        dumpLog.AddDesc(
            std::string("LetterSpacing: ")
                .append(textStyle_->GetLetterSpacing().ToString())
                .append(" pro: ")
                .append(textLayoutProp->HasLetterSpacing()
                            ? textLayoutProp->GetLetterSpacingValue(Dimension(0.0, DimensionUnit::FP)).ToString()
                            : "Na"));
        dumpLog.AddDesc(std::string("TextOverflow: ").append(StringUtils::ToString(textStyle_->GetTextOverflow())));
        dumpLog.AddDesc(std::string("TextAlign: ")
                            .append(StringUtils::ToString(textStyle_->GetTextAlign()))
                            .append(" pro: ")
                            .append(textLayoutProp->HasTextAlign()
                                        ? StringUtils::ToString(textLayoutProp->GetTextAlignValue(TextAlign::START))
                                        : "Na"));
        dumpLog.AddDesc(std::string("WordBreak: ").append(StringUtils::ToString(textStyle_->GetWordBreak())));
        dumpLog.AddDesc(std::string("TextCase: ").append(StringUtils::ToString(textStyle_->GetTextCase())));
        dumpLog.AddDesc(std::string("EllipsisMode: ").append(StringUtils::ToString(textStyle_->GetEllipsisMode())));
        dumpLog.AddDesc(
            std::string("LineBreakStrategy: ").append(GetLineBreakStrategyInJson(textStyle_->GetLineBreakStrategy())));
        dumpLog.AddDesc(std::string("SymbolColorList: ")
                            .append(StringUtils::SymbolColorListToString(textStyle_->GetSymbolColorList())));
    }
}

void TextPattern::DumpScaleInfo()
{
    auto& dumpLog = DumpLog::GetInstance();
    dumpLog.AddDesc(std::string("-----DumpScaleInfo-----"));
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto fontScale = pipeline->GetFontScale();
    auto fontWeightScale = pipeline->GetFontWeightScale();
    auto followSystem = pipeline->IsFollowSystem();
    float maxFontScale = pipeline->GetMaxAppFontScale();
    auto halfLeading = pipeline->GetHalfLeading();
    dumpLog.AddDesc(std::string("fontScale: ").append(std::to_string(fontScale)));
    dumpLog.AddDesc(std::string("fontWeightScale: ").append(std::to_string(fontWeightScale)));
    dumpLog.AddDesc(std::string("IsFollowSystem: ").append(std::to_string(followSystem)));
    dumpLog.AddDesc(std::string("maxFontScale: ").append(std::to_string(maxFontScale)));
    dumpLog.AddDesc(std::string("ConfigHalfLeading: ").append(std::to_string(halfLeading)));
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProp);
    auto minFontScale = textLayoutProp->GetMinFontScale().value_or(0.0f);
    dumpLog.AddDesc(std::string("minFontScale: ").append(std::to_string(minFontScale)));
    auto maxfontScale = textLayoutProp->GetMaxFontScale().value_or(static_cast<float>(INT32_MAX));
    dumpLog.AddDesc(std::string("maxFontScale1: ").append(std::to_string(maxfontScale)));
    auto flag = textLayoutProp->HasHalfLeading();
    dumpLog.AddDesc(
        std::string("HalfLeading: ").append(flag ? std::to_string(textLayoutProp->GetHalfLeadingValue(false)) : "NA"));
}

void TextPattern::DumpTextEngineInfo()
{
    auto& dumpLog = DumpLog::GetInstance();
    dumpLog.AddDesc(std::string("-----TextEngine paragraphs_ info-----"));
    dumpLog.AddDesc(std::string("contentRect :").append(contentRect_.ToString()));
    if (pManager_) {
        dumpLog.AddDesc(std::string("from TextEngine paragraphs_ info :"));
        auto paragraphs = pManager_->GetParagraphs();
        if (paragraphs.empty()) {
            dumpLog.AddDesc(std::string("paragraphs is empty!"));
            return;
        }
        dumpLog.AddDesc(std::string("DidExceedMaxLines:").append(std::to_string(pManager_->DidExceedMaxLines())));
        dumpLog.AddDesc(std::string("GetTextWidth:")
                            .append(std::to_string(pManager_->GetTextWidth()))
                            .append(" GetHeight:")
                            .append(std::to_string(pManager_->GetHeight()))
                            .append(" GetMaxWidth:")
                            .append(std::to_string(pManager_->GetMaxWidth()))
                            .append(" GetMaxIntrinsicWidth:")
                            .append(std::to_string(pManager_->GetMaxIntrinsicWidth())));
        dumpLog.AddDesc(std::string("GetLineCount:")
                            .append(std::to_string(pManager_->GetLineCount()))
                            .append(" GetLongestLine:")
                            .append(std::to_string(pManager_->GetLongestLine()))
                            .append(std::to_string(pManager_->GetLongestLineWithIndent())));
    }
    dumpLog.AddDesc(std::string("spans size :").append(std::to_string(spans_.size())));
    if (!IsSetObscured()) {
        DumpParagraphsInfo();
    }
}

void TextPattern::DumpParagraphsInfo()
{
    CHECK_NULL_VOID(pManager_);
    auto& dumpLog = DumpLog::GetInstance();
    auto paragraphs = pManager_->GetParagraphs();
    if (paragraphs.empty()) {
        dumpLog.AddDesc(std::string("paragraphs is empty!"));
        return;
    }
    dumpLog.AddDesc(std::string("paragraphs size:").append(std::to_string(paragraphs.size())));
    for (auto&& info : paragraphs) {
        auto paragraph = info.paragraph;
        if (paragraph) {
            auto text = StringUtils::Str16ToStr8(paragraph->GetParagraphText());
            auto textStyle = paragraph->GetParagraphStyle();
            auto direction = V2::ConvertTextDirectionToString(textStyle.direction);
            dumpLog.AddDesc(std::string("paragraph: ").append(text).append("; direction:").append(direction));
        }
    }
}

void TextPattern::SetAccessibilityAction()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto textAccessibilityProperty = host->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(textAccessibilityProperty);
    textAccessibilityProperty->SetActionSetSelection(
        [weakPtr = WeakClaim(this)](int32_t start, int32_t end, bool isForward) {
            const auto& pattern = weakPtr.Upgrade();
            CHECK_NULL_VOID(pattern);
            auto textLayoutProperty = pattern->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(textLayoutProperty);
            auto mode = textLayoutProperty->GetTextSelectableModeValue(TextSelectableMode::SELECTABLE_UNFOCUSABLE);
            if (textLayoutProperty->GetCopyOptionValue(CopyOptions::None) != CopyOptions::None &&
                mode != TextSelectableMode::UNSELECTABLE) {
                pattern->ActSetSelection(start, end);
            }
        });

    textAccessibilityProperty->SetActionClearSelection([weakPtr = WeakClaim(this)]() {
        const auto& pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto textLayoutProperty = pattern->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        auto mode = textLayoutProperty->GetTextSelectableModeValue(TextSelectableMode::SELECTABLE_UNFOCUSABLE);
        if (textLayoutProperty->GetCopyOptionValue(CopyOptions::None) != CopyOptions::None &&
            mode != TextSelectableMode::UNSELECTABLE) {
            pattern->CloseSelectOverlay(true);
            pattern->ResetSelection();
        }
    });

    textAccessibilityProperty->SetActionCopy([weakPtr = WeakClaim(this)]() {
        const auto& pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto textLayoutProperty = pattern->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        auto mode = textLayoutProperty->GetTextSelectableModeValue(TextSelectableMode::SELECTABLE_UNFOCUSABLE);
        if (textLayoutProperty->GetCopyOptionValue(CopyOptions::None) != CopyOptions::None &&
            mode != TextSelectableMode::UNSELECTABLE) {
            pattern->HandleOnCopy();
            pattern->CloseSelectOverlay(true);
            pattern->ResetSelection();
        }
    });
}

void TextPattern::OnColorConfigurationUpdate()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    CHECK_NULL_VOID(!textLayoutProperty->GetTextColorFlagByUserValue(false));
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto theme = context->GetTheme<TextTheme>();
    CHECK_NULL_VOID(theme);
    textLayoutProperty->UpdateTextColor(theme->GetTextStyle().GetTextColor());
    if (GetOrCreateMagnifier()) {
        magnifierController_->SetColorModeChange(true);
    }
    ACE_TEXT_SCOPED_TRACE("OnColorConfigurationUpdate[Text][self:%d]", host->GetId());
}

OffsetF TextPattern::GetDragUpperLeftCoordinates()
{
    if (dragBoxes_.empty()) {
        return { 0.0f, 0.0f };
    }
    auto startY = dragBoxes_.front().Top();
    auto startX = dragBoxes_.front().Left();

    auto endY = dragBoxes_.back().Top();
    OffsetF offset;
    if (NearEqual(startY, endY)) {
        offset = { contentRect_.GetX() + startX, startY + contentRect_.GetY() };
    } else {
        offset = { contentRect_.GetX(), startY + contentRect_.GetY() };
    }

    return GetParentGlobalOffset() + offset;
}

void TextPattern::ProcessBoundRectByTextShadow(RectF& rect)
{
    auto property = GetHost()->GetLayoutProperty<TextLayoutProperty>();
    auto shadows = property->GetTextShadow();
    if (!shadows.has_value()) {
        return;
    }
    float leftOffsetX = 0.0f;
    float rightOffsetX = 0.0f;
    float upOffsetY = 0.0f;
    float downOffsetY = 0.0f;
    for (const auto& shadow : shadows.value()) {
        auto shadowBlurRadius = shadow.GetBlurRadius() * 2.0f;
        if (LessNotEqual(shadow.GetOffset().GetX() - shadowBlurRadius, leftOffsetX)) {
            leftOffsetX = shadow.GetOffset().GetX() - shadowBlurRadius;
        }

        if (GreatNotEqual(shadow.GetOffset().GetX() + shadowBlurRadius, rightOffsetX)) {
            rightOffsetX = shadow.GetOffset().GetX() + shadowBlurRadius;
        }

        if (LessNotEqual(shadow.GetOffset().GetY() - shadowBlurRadius, upOffsetY)) {
            upOffsetY = shadow.GetOffset().GetY() - shadowBlurRadius;
        }

        if (GreatNotEqual(shadow.GetOffset().GetY() + shadowBlurRadius, downOffsetY)) {
            downOffsetY = shadow.GetOffset().GetY() + shadowBlurRadius;
        }
    }
    rect.SetRect(
        leftOffsetX, upOffsetY, rect.Width() + rightOffsetX - leftOffsetX, rect.Height() + downOffsetY - upOffsetY);
}

void TextPattern::ProcessBoundRectByTextMarquee(RectF& rect)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto textLayoutProperty = host->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    if (textLayoutProperty->GetTextOverflowValue(TextOverflow::CLIP) != TextOverflow::MARQUEE) {
        return;
    }
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto contentSize = geometryNode->GetContentSize();
    CHECK_NULL_VOID(pManager_);
    if (pManager_->GetTextWidth() < contentSize.Width()) {
        return;
    }
    auto frameSize = geometryNode->GetFrameSize();
    auto relativeSelfLeftOffsetX =
        std::max(-1 * host->GetOffsetRelativeToWindow().GetX(), rect.GetOffset().GetX() - pManager_->GetTextWidth());
    rect.SetLeft(relativeSelfLeftOffsetX);
    rect.SetWidth(frameSize.Width() + pManager_->GetTextWidth() - relativeSelfLeftOffsetX);
}

RefPtr<NodePaintMethod> TextPattern::CreateNodePaintMethod()
{
    CreateModifier();
    auto paintMethod = MakeRefPtr<TextPaintMethod>(WeakClaim(this), baselineOffset_, contentMod_, overlayMod_);
    auto host = GetHost();
    CHECK_NULL_RETURN(host, paintMethod);
    auto context = host->GetRenderContext();
    CHECK_NULL_RETURN(context, paintMethod);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, paintMethod);
    auto frameSize = geometryNode->GetFrameSize();
    if (context->GetClipEdge().value_or(Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE))) {
        SetResponseRegion(frameSize, frameSize);
        return paintMethod;
    }
    CHECK_NULL_RETURN(pManager_, paintMethod);
    RectF boundsRect = overlayMod_->GetBoundsRect();
    auto boundsWidth = contentRect_.GetX() + std::ceil(pManager_->GetLongestLineWithIndent());
    auto boundsHeight = contentRect_.GetY() + static_cast<float>(pManager_->GetHeight() + std::fabs(baselineOffset_));
    boundsRect.SetWidth(boundsWidth);
    boundsRect.SetHeight(boundsHeight);
    SetResponseRegion(frameSize, boundsRect.GetSize());
    ProcessBoundRectByTextShadow(boundsRect);
    ProcessBoundRectByTextMarquee(boundsRect);
    boundsRect.SetWidth(std::max(frameSize.Width(), boundsRect.Width()));
    boundsRect.SetHeight(std::max(frameSize.Height(), boundsRect.Height()));
    auto baselineOffset = LessOrEqual(baselineOffset_, 0) ? std::fabs(baselineOffset_) : 0;
    pManager_->GetPaintRegion(boundsRect, contentRect_.GetX(), contentRect_.GetY() + baselineOffset);
    overlayMod_->SetBoundsRect(boundsRect);
    return paintMethod;
}

void TextPattern::SetResponseRegion(const SizeF& frameSize, const SizeF& boundsSize)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto gestureHub = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    if (isUserSetResponseRegion_) {
        return;
    }
    std::vector<DimensionRect> hotZoneRegions;
    DimensionRect hotZoneRegion;
    hotZoneRegion.SetSize(DimensionSize(Dimension(std::max(boundsSize.Width(), frameSize.Width())),
        Dimension(std::max(frameSize.Height(), boundsSize.Height()))));
    hotZoneRegions.emplace_back(hotZoneRegion);
    gestureHub->SetResponseRegion(hotZoneRegions);
}

void TextPattern::CreateModifier()
{
    if (!contentMod_) {
        contentMod_ = MakeRefPtr<TextContentModifier>(textStyle_, WeakClaim(this));
    }
    if (!overlayMod_) {
        overlayMod_ = MakeRefPtr<TextOverlayModifier>();
    }
    if (isCustomFont_) {
        contentMod_->SetIsCustomFont(true);
    }
}

int32_t TextPattern::GetHandleIndex(const Offset& offset) const
{
    return pManager_->GetGlyphIndexByCoordinate(offset);
}

void TextPattern::OnHandleAreaChanged()
{
    if (selectOverlay_->SelectOverlayIsOn()) {
        auto parentGlobalOffset = GetParentGlobalOffset();
        if (parentGlobalOffset != parentGlobalOffset_) {
            parentGlobalOffset_ = parentGlobalOffset;
            CalculateHandleOffsetAndShowOverlay();
            ShowSelectOverlay({ .menuIsShow = false, .animation = true });
        }
    }
}

void TextPattern::RemoveAreaChangeInner()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto eventHub = host->GetEventHub<TextEventHub>();
    CHECK_NULL_VOID(eventHub);
    if (eventHub->HasOnAreaChanged()) {
        return;
    }
    pipeline->RemoveOnAreaChangeNode(host->GetId());
}

void TextPattern::SetTextDetectEnable(bool enable)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    dataDetectorAdapter_->frameNode_ = host;
    if (enable == textDetectEnable_) {
        return;
    }
    textDetectEnable_ = enable;
    if (textDetectEnable_) {
        auto pipeline = host->GetContext();
        CHECK_NULL_VOID(pipeline);
        auto callback = [weak = WeakClaim(this)]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            pattern->dataDetectorAdapter_->GetAIEntityMenu();
        };
        pipeline->SetConfigChangedCallback(host->GetId(), callback);
    } else {
        dataDetectorAdapter_->CancelAITask();
    }
    host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE);
}

bool TextPattern::CanStartAITask()
{
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    if (textLayoutProperty) {
        return textDetectEnable_ && enabled_ && !IsSetObscured() &&
               textLayoutProperty->GetTextOverflowValue(TextOverflow::CLIP) != TextOverflow::MARQUEE;
    } else {
        return textDetectEnable_ && enabled_;
    }
}

bool TextPattern::NeedShowAIDetect()
{
    return CanStartAITask() && !dataDetectorAdapter_->aiSpanMap_.empty();
}

void TextPattern::BindSelectionMenu(TextSpanType spanType, TextResponseType responseType,
    std::function<void()>& menuBuilder, std::function<void(int32_t, int32_t)>& onAppear,
    std::function<void()>& onDisappear)
{
    auto key = std::make_pair(spanType, responseType);
    auto it = selectionMenuMap_.find(key);
    if (it != selectionMenuMap_.end()) {
        if (menuBuilder == nullptr) {
            selectionMenuMap_.erase(it);
            return;
        }
        it->second->buildFunc = menuBuilder;
        it->second->onAppear = onAppear;
        it->second->onDisappear = onDisappear;
        return;
    }

    auto selectionMenuParams =
        std::make_shared<SelectionMenuParams>(spanType, menuBuilder, onAppear, onDisappear, responseType);
    selectionMenuMap_[key] = selectionMenuParams;
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE_SELF);
}

void TextPattern::CloseSelectionMenu()
{
    textResponseType_ = TextResponseType::NONE;
    CloseSelectOverlay(true);
}

std::shared_ptr<SelectionMenuParams> TextPattern::GetMenuParams(TextSpanType spanType, TextResponseType responseType)
{
    auto key = std::make_pair(spanType, responseType);
    auto it = selectionMenuMap_.find(key);
    if (it != selectionMenuMap_.end()) {
        return it->second;
    }

    TAG_LOGD(AceLogTag::ACE_TEXT, "The key not in selectionMenuMap_");
    return nullptr;
}

void TextPattern::CopySelectionMenuParams(SelectOverlayInfo& selectInfo, TextResponseType responseType)
{
    auto currentSpanType = selectedType_.value_or(TextSpanType::NONE);
    std::shared_ptr<SelectionMenuParams> menuParams = nullptr;
    menuParams = GetMenuParams(currentSpanType, responseType);
    if (menuParams == nullptr) {
        return;
    }
    CopyBindSelectionMenuParams(selectInfo, menuParams);
}

void TextPattern::CopyBindSelectionMenuParams(
    SelectOverlayInfo& selectInfo, std::shared_ptr<SelectionMenuParams> menuParams)
{
    selectInfo.menuInfo.menuBuilder = menuParams->buildFunc;
    if (menuParams->onAppear) {
        auto weak = AceType::WeakClaim(this);
        auto callback = [weak, menuParams]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            CHECK_NULL_VOID(menuParams->onAppear);

            auto& textSelector = pattern->textSelector_;
            auto selectStart = std::min(textSelector.baseOffset, textSelector.destinationOffset);
            auto selectEnd = std::max(textSelector.baseOffset, textSelector.destinationOffset);
            menuParams->onAppear(selectStart, selectEnd);
        };
        selectInfo.menuCallback.onAppear = std::move(callback);
    }
    selectInfo.menuCallback.onDisappear = menuParams->onDisappear;
}

void TextPattern::FireOnSelectionChange(int32_t start, int32_t end)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<TextEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireOnSelectionChange(start, end);
}

void TextPattern::OnSelectionMenuOptionsUpdate(
    const NG::OnCreateMenuCallback&& onCreateMenuCallback, const NG::OnMenuItemClickCallback&& onMenuItemClick)
{
    selectOverlay_->OnSelectionMenuOptionsUpdate(std::move(onCreateMenuCallback), std::move(onMenuItemClick));
}

void TextPattern::StartVibratorByIndexChange(int32_t currentIndex, int32_t preIndex)
{
    CHECK_NULL_VOID(isEnableHapticFeedback_ && (currentIndex != preIndex));
    VibratorUtils::StartVibraFeedback("slide");
}

void TextPattern::HandleSelectionChange(int32_t start, int32_t end)
{
    if (textSelector_.GetStart() == start && textSelector_.GetEnd() == end) {
        return;
    }

    bool changeSymbolEffect = false;
    for (auto& span: spans_) {
        if (span->GetSymbolUnicode() == 0) {
            continue;
        }
        bool nextEffectSwitch = start != -1 && end != -1 ? false : true;
        if (span->GetSymbolEffectSwitch() != nextEffectSwitch) {
            span->SetSymbolEffectSwitch(nextEffectSwitch);
            changeSymbolEffect = true;
        }
    }
    textSelector_.Update(start, end);
    UpdateSelectionSpanType(std::min(start, end), std::max(start, end));
    FireOnSelectionChange(std::min(start, end), std::max(start, end));
    if (changeSymbolEffect) {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE_SELF);
    }
}

bool TextPattern::IsSelectedBindSelectionMenu()
{
    auto currentSpanType = selectedType_.value_or(TextSpanType::TEXT);
    return GetMenuParams(currentSpanType, TextResponseType::SELECTED_BY_MOUSE) != nullptr;
}

void TextPattern::UpdateSelectionSpanType(int32_t selectStart, int32_t selectEnd)
{
    UpdateSelectionType(GetSpansInfo(selectStart, selectEnd, GetSpansMethod::ONSELECT));
    if ((selectedType_ == TextSpanType::NONE && !textSelector_.StartEqualToDest()) ||
        textSelector_.StartEqualToDest()) {
        selectedType_ = TextSpanType::TEXT;
    }
}

void TextPattern::UpdateSelectionType(const SelectionInfo& selection)
{
    selectedType_ = TextSpanType::NONE;
    auto list = selection.GetSelection().resultObjects;
    bool imageSelected = false;
    bool textSelected = false;
    bool builderSelected = false;
    for (const auto& obj : list) {
        if (obj.type == SelectSpanType::TYPEIMAGE) {
            imageSelected = true;
        } else if (obj.type == SelectSpanType::TYPESPAN) {
            textSelected = true;
        } else if (obj.type == SelectSpanType::TYPEBUILDERSPAN) {
            builderSelected = true;
        }
        if ((imageSelected && textSelected) || (builderSelected && textSelected) ||
            (imageSelected && builderSelected)) {
            selectedType_ = TextSpanType::MIXED;
            return;
        }
    }
    if (imageSelected) {
        selectedType_ = TextSpanType::IMAGE;
    } else if (textSelected) {
        selectedType_ = TextSpanType::TEXT;
    } else if (builderSelected) {
        selectedType_ = TextSpanType::BUILDER;
    }

    TAG_LOGD(AceLogTag::ACE_TEXT, "UpdateSelectionSpanType, selectedType_: %{public}d", selectedType_.value());
}

int32_t TextPattern::GetSelectionSpanItemIndex(const MouseInfo& info)
{
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    textContentRect.SetHeight(contentRect_.Height() - std::max(baselineOffset_, 0.0f));
    PointF textOffset = { info.GetLocalLocation().GetX() - textContentRect.GetX(),
        info.GetLocalLocation().GetY() - textContentRect.GetY() };
    if (!textContentRect.IsInRegion(PointF(info.GetLocalLocation().GetX(), info.GetLocalLocation().GetY())) ||
        spans_.empty() || pManager_->GetParagraphs().empty()) {
        return -1;
    }
    int32_t start = 0;
    bool isFind = false;
    int32_t index = -1;
    for (const auto& item : spans_) {
        index++;
        if (!item) {
            continue;
        }
        auto selectedRects = pManager_->GetRects(start, item->position);
        start = item->position;
        for (auto&& rect : selectedRects) {
            if (rect.IsInRegion(textOffset)) {
                isFind = true;
                break;
            }
        }
        if (isFind) {
            TAG_LOGD(AceLogTag::ACE_TEXT, "GetSelectionSpanItemIndex index: %{public}d", index);
            return index;
        }
    }
    return -1;
}

ResultObject TextPattern::GetBuilderResultObject(RefPtr<UINode> uiNode, int32_t index, int32_t start, int32_t end)
{
    int32_t itemLength = 1;
    ResultObject resultObject;
    resultObject.isDraggable = true;
    if (!DynamicCast<FrameNode>(uiNode) || !GetSpanItemByIndex(index)) {
        return resultObject;
    }
    int32_t endPosition = std::min(GetTextContentLength(), GetSpanItemByIndex(index)->position);
    int32_t startPosition = endPosition - itemLength;
    if ((start <= startPosition) && (end >= endPosition)) {
        auto builderNode = DynamicCast<FrameNode>(uiNode);
        CHECK_NULL_RETURN(builderNode, resultObject);
        resultObject.spanPosition.spanIndex = index;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGESTART] = startPosition;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGEEND] = endPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
        resultObject.type = SelectSpanType::TYPEIMAGE;
        auto geometryNode = builderNode->GetGeometryNode();
        CHECK_NULL_RETURN(geometryNode, resultObject);
        resultObject.imageStyle.size[RichEditorImageSize::SIZEWIDTH] = geometryNode->GetMarginFrameSize().Width();
        resultObject.imageStyle.size[RichEditorImageSize::SIZEHEIGHT] = geometryNode->GetMarginFrameSize().Height();
        resultObject.valueString = u" ";
    }
    return resultObject;
}

void TextPattern::SetStyledString(const RefPtr<SpanString>& value)
{
    isSpanStringMode_ = true;
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    CloseSelectOverlay();
    auto length = styledString_->GetLength();
    styledString_->RemoveCustomSpan();
    styledString_->ReplaceSpanString(0, length, value);
    spans_ = styledString_->GetSpanItems();
    ProcessSpanString();
    styledString_->AddCustomSpan();
    styledString_->SetFramNode(WeakClaim(host.GetRawPtr()));
    host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE);
}

void TextPattern::MountImageNode(const RefPtr<ImageSpanItem>& imageItem)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto imageNode = ImageSpanNode::GetOrCreateSpanNode(V2::IMAGE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ImagePattern>(); });
    auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    auto options = imageItem->options;
    auto imageInfo = CreateImageSourceInfo(options);
    imageLayoutProperty->UpdateImageSourceInfo(imageInfo);
    auto index = host->GetChildren().size();
    imageNode->MountToParent(host, index);
    auto gesture = imageNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gesture);
    gesture->SetHitTestMode(HitTestMode::HTMNONE);
    if (options.imageAttribute.has_value()) {
        auto imgAttr = options.imageAttribute.value();
        if (imgAttr.size.has_value()) {
            imageLayoutProperty->UpdateUserDefinedIdealSize(imgAttr.size->GetSize());
        }
        if (imgAttr.verticalAlign.has_value()) {
            imageLayoutProperty->UpdateVerticalAlign(imgAttr.verticalAlign.value());
        }
        if (imgAttr.objectFit.has_value()) {
            imageLayoutProperty->UpdateImageFit(imgAttr.objectFit.value());
        }
        if (imgAttr.marginProp.has_value()) {
            imageLayoutProperty->UpdateMargin(imgAttr.marginProp.value());
        }
        if (imgAttr.paddingProp.has_value()) {
            imageLayoutProperty->UpdatePadding(imgAttr.paddingProp.value());
        }
        if (imgAttr.borderRadius.has_value()) {
            auto imageRenderCtx = imageNode->GetRenderContext();
            imageRenderCtx->UpdateBorderRadius(imgAttr.borderRadius.value());
            imageRenderCtx->SetClipToBounds(true);
        }
    }
    imageNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    imageNode->MarkModifyDone();
    imageItem->imageNodeId = imageNode->GetId();
    imageNode->SetImageItem(imageItem);
    childNodes_.emplace_back(imageNode);
}

ImageSourceInfo TextPattern::CreateImageSourceInfo(const ImageSpanOptions& options)
{
    std::string src;
    RefPtr<PixelMap> pixMap = nullptr;
    std::string bundleName;
    std::string moduleName;
    if (options.image.has_value()) {
        src = options.image.value();
    }
    if (options.imagePixelMap.has_value()) {
        pixMap = options.imagePixelMap.value();
    }
    if (options.bundleName.has_value()) {
        bundleName = options.bundleName.value();
    }
    if (options.moduleName.has_value()) {
        moduleName = options.moduleName.value();
    }
#if defined(PIXEL_MAP_SUPPORTED)
    if (!options.imagePixelMap.has_value()) {
        return { src, bundleName, moduleName };
    }
    return ImageSourceInfo(pixMap);
#else
    return { src, bundleName, moduleName };
#endif
}

void TextPattern::ProcessSpanString()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    textForDisplay_.clear();
    childNodes_.clear();
    dataDetectorAdapter_->textForAI_.clear();
    host->Clean();
    hasSpanStringLongPressEvent_ = false;
    hasUrlSpan_ = false;

    // 适配AI&&挂载image节点
    for (const auto& span : spans_) {
        auto imageSpan = DynamicCast<ImageSpanItem>(span);
        if (imageSpan) {
            dataDetectorAdapter_->textForAI_ += u'\n';
            MountImageNode(imageSpan);
        } else {
            dataDetectorAdapter_->textForAI_ += span->content;
        }
        if (span->onClick || span->urlOnRelease) {
            auto gestureEventHub = host->GetOrCreateGestureEventHub();
            InitClickEvent(gestureEventHub);
        }
        if (span->onLongPress) {
            auto gestureEventHub = host->GetOrCreateGestureEventHub();
            InitLongPressEvent(gestureEventHub);
            hasSpanStringLongPressEvent_ = true;
        }
        if (span->urlOnRelease) {
            hasUrlSpan_ = true;
            InitUrlMouseEvent();
            InitUrlTouchEvent();
        }
        textForDisplay_ += span->content;
    }
    if (dataDetectorAdapter_->textForAI_ != textForDisplay_) {
        dataDetectorAdapter_->aiDetectInitialized_ = false;
    }
    if (CanStartAITask() && !dataDetectorAdapter_->aiDetectInitialized_) {
        dataDetectorAdapter_->StartAITask();
    }

    auto layoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    layoutProperty->UpdateContent(textForDisplay_);
}

void TextPattern::OnSensitiveStyleChange(bool isSensitive)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    isSensitive_ = isSensitive;
    host->MarkDirtyWithOnProChange(PROPERTY_UPDATE_MEASURE);
}

bool TextPattern::IsSensitiveEnalbe()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    return isSensitive_ && host->IsPrivacySensitive();
}

Offset TextPattern::ConvertGlobalToLocalOffset(const Offset& globalOffset)
{
    auto localPoint = OffsetF(globalOffset.GetX(), globalOffset.GetY());
    selectOverlay_->RevertLocalPointWithTransform(localPoint);
    return Offset(localPoint.GetX(), localPoint.GetY());
}

void TextPattern::SetExternalSpanItem(const std::list<RefPtr<SpanItem>>& spans)
{
    isSpanStringMode_ = !spans.empty();
    spans_ = spans;
    ProcessSpanString();
    auto layoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    layoutProperty->UpdateContent(textForDisplay_);
}

RectF TextPattern::GetTextContentRect(bool isActualText) const
{
    auto textRect = contentRect_;
    auto host = GetHost();
    CHECK_NULL_RETURN(host, textRect);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, textRect);
    CHECK_NULL_RETURN(pManager_, textRect);
    if (!renderContext->GetClipEdge().value_or(false) &&
        LessNotEqual(textRect.Width(), pManager_->GetLongestLine())) {
        textRect.SetWidth(pManager_->GetLongestLine());
    }
    if (isActualText && !renderContext->GetClipEdge().value_or(false) &&
        LessNotEqual(textRect.Height(), pManager_->GetHeight())) {
        textRect.SetHeight(pManager_->GetHeight());
    }
    return textRect;
}

size_t TextPattern::GetLineCount() const
{
    CHECK_NULL_RETURN(pManager_, 0);
    return pManager_->GetLineCount();
}

bool TextPattern::DidExceedMaxLines() const
{
    CHECK_NULL_RETURN(pManager_, false);
    return pManager_->DidExceedMaxLines();
}

bool TextPattern::IsSetObscured()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, false);
    auto obscuredReasons = renderContext->GetObscured().value_or(std::vector<ObscuredReasons>());
    bool ifHaveObscured = spans_.empty() && std::any_of(obscuredReasons.begin(), obscuredReasons.end(),
        [](const auto& reason) { return reason == ObscuredReasons::PLACEHOLDER; });
    return ifHaveObscured;
}

TextLineMetrics TextPattern::GetLineMetrics(int32_t lineNumber)
{
    CHECK_NULL_RETURN(pManager_, TextLineMetrics());
    if (lineNumber < 0 || GetLineCount() == 0 || lineNumber > static_cast<int32_t>(GetLineCount()) - 1) {
        TAG_LOGI(AceLogTag::ACE_TEXT, "GetLineMetrics failed, lineNumber not between 0 and max lines:%{public}d",
            lineNumber);
        return TextLineMetrics();
    }
    auto lineMetrics = pManager_->GetLineMetrics(lineNumber);
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    lineMetrics.x += textContentRect.GetX();
    lineMetrics.y += textContentRect.GetY();
    lineMetrics.baseline += textContentRect.GetY();
    return lineMetrics;
}

std::vector<ParagraphManager::TextBox> TextPattern::GetRectsForRange(
    int32_t start, int32_t end, RectHeightStyle heightStyle, RectWidthStyle widthStyle)
{
    if (start < 0 || end < 0 || start > end) {
        return {};
    }
    std::vector<ParagraphManager::TextBox> textBoxes = pManager_->GetRectsForRange(start, end, heightStyle, widthStyle);
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    std::vector<ParagraphManager::TextBox> adjustedTextBoxes;
    for (auto& textBox : textBoxes) {
        ParagraphManager::TextBox adjustedTextBox = textBox;
        adjustedTextBox.rect_.SetLeft(textBox.rect_.Left() + textContentRect.Left());
        adjustedTextBox.rect_.SetTop(textBox.rect_.Top() + textContentRect.Top());
        adjustedTextBoxes.push_back(adjustedTextBox);
    }
    return adjustedTextBoxes;
}

Offset TextPattern::ConvertLocalOffsetToParagraphOffset(const Offset& offset)
{
    RectF textContentRect = contentRect_;
    textContentRect.SetTop(contentRect_.GetY() - std::min(baselineOffset_, 0.0f));
    Offset paragraphOffset = { offset.GetX() - textContentRect.GetX(), offset.GetY() - textContentRect.GetY() };
    return paragraphOffset;
}

PositionWithAffinity TextPattern::GetGlyphPositionAtCoordinate(int32_t x, int32_t y)
{
    Offset offset(x, y);
    return pManager_->GetGlyphPositionAtCoordinate(ConvertLocalOffsetToParagraphOffset(offset));
}

void TextPattern::ProcessMarqueeVisibleAreaCallback()
{
    OnTextOverflowChanged();
    if (!IsMarqueeOverflow()) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    auto callback = [weak = WeakClaim(this)](bool visible, double ratio) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        CHECK_NULL_VOID(pattern->contentMod_);
        if (!pattern->IsMarqueeOverflow()) {
            return;
        }
        if (visible && Positive(ratio)) {
            pattern->contentMod_->ResumeAnimation();
        }
        if (!visible && NonPositive(ratio)) {
            pattern->contentMod_->PauseAnimation();
        }
    };
    std::vector<double> ratioList = { 0.0 };
    pipeline->AddVisibleAreaChangeNode(host, ratioList, callback, false, true);
}

void TextPattern::OnTextOverflowChanged()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    auto eventHub = host->GetEventHub<TextEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto hasInnerCallabck = eventHub->HasVisibleAreaCallback(false);
    if (!hasInnerCallabck) {
        return;
    }
    auto hasUserCallback = eventHub->HasVisibleAreaCallback(true);
    if (!hasUserCallback) {
        pipeline->RemoveVisibleAreaChangeNode(host->GetId());
    }
    eventHub->CleanVisibleAreaCallback(false);
}

void TextPattern::OnFrameNodeChanged(FrameNodeChangeInfoFlag flag)
{
    if (selectOverlay_->SelectOverlayIsOn()) {
        selectOverlay_->OnAncestorNodeChanged(flag);
    }
    if (leftMousePressed_ && mouseStatus_ == MouseStatus::MOVE && scrollableParent_.Upgrade()) {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto textPaintOffset = contentRect_.GetOffset() - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));
        NG::PointF localPoint(lastLeftMouseMoveLocation_.GetX(), lastLeftMouseMoveLocation_.GetY());
        NG::NGGestureRecognizer::Transform(localPoint, WeakClaim(host.GetRawPtr()), true);
        Offset textOffset = { localPoint.GetX() - textPaintOffset.GetX(), localPoint.GetY() - textPaintOffset.GetY() };
        CHECK_NULL_VOID(pManager_);
        auto end = pManager_->GetGlyphIndexByCoordinate(textOffset);
        HandleSelectionChange(textSelector_.baseOffset, end);
        isAutoScrollByMouse_ = true;
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    }
}

bool TextPattern::IsMarqueeOverflow() const
{
    auto textLayoutProperty = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, false);
    return textLayoutProperty->GetTextOverflowValue(TextOverflow::CLIP) == TextOverflow::MARQUEE;
}

void TextPattern::UpdateFontColor(const Color& value)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    const auto& children = host->GetChildren();
    if (children.empty() && spans_.empty() && contentMod_ && !NeedShowAIDetect()) {
        contentMod_->TextColorModifier(value);
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    } else {
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    }
}

void TextPattern::MarkDirtyNodeRender()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void TextPattern::BeforeCreatePaintWrapper()
{
    // mark content dirty
    if (contentMod_) {
        contentMod_->ContentChange();
    }
}

void TextPattern::StartGestureSelection(int32_t start, int32_t end, const Offset& startOffset)
{
    scrollableParent_ = selectOverlay_->FindScrollableParent();
    SetupMagnifier();
    TextGestureSelector::StartGestureSelection(start, end, startOffset);
}

int32_t TextPattern::GetTouchIndex(const OffsetF& offset)
{
    OffsetF deltaOffset;
    if (scrollableParent_.Upgrade()) {
        auto parentGlobalOffset = GetParentGlobalOffset();
        deltaOffset = parentGlobalOffset - parentGlobalOffset_;
    }
    auto paragraphOffset =
        offset - deltaOffset - GetTextContentRect().GetOffset() + OffsetF(0.0f, std::min(GetBaselineOffset(), 0.0f));
    return GetHandleIndex({ paragraphOffset.GetX(), paragraphOffset.GetY() });
}

void TextPattern::OnTextGestureSelectionUpdate(int32_t start, int32_t end, const TouchEventInfo& info)
{
    if (!HasContent()) {
        return;
    }
    selectOverlay_->TriggerScrollableParentToScroll(
        scrollableParent_.Upgrade(), info.GetTouches().front().GetGlobalLocation(), false);
    auto localOffset = info.GetTouches().front().GetLocalLocation();
    if (GetOrCreateMagnifier()) {
        magnifierController_->SetLocalOffset({ localOffset.GetX(), localOffset.GetY() });
    }
    if (start != textSelector_.GetStart()) {
        StartVibratorByIndexChange(start, textSelector_.GetStart());
    } else if (end != textSelector_.GetEnd()) {
        StartVibratorByIndexChange(end, textSelector_.GetEnd());
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    HandleSelectionChange(start, end);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

void TextPattern::OnTextGenstureSelectionEnd()
{
    selectOverlay_->TriggerScrollableParentToScroll(scrollableParent_.Upgrade(), Offset(), true);
    if (magnifierController_) {
        magnifierController_->RemoveMagnifierFrameNode();
    }
    if (HasContent()) {
        CalculateHandleOffsetAndShowOverlay();
        ShowSelectOverlay({ .animation = true });
    }
}

void TextPattern::ChangeHandleHeight(const GestureEvent& event, bool isFirst, bool isOverlayMode)
{
    auto touchOffset = event.GetGlobalLocation();
    auto& currentHandle = isFirst ? textSelector_.firstHandle : textSelector_.secondHandle;
    bool isChangeFirstHandle = isFirst ? (!textSelector_.StartGreaterDest()) : textSelector_.StartGreaterDest();
    if (isChangeFirstHandle) {
        ChangeFirstHandleHeight(touchOffset, currentHandle);
    } else {
        ChangeSecondHandleHeight(touchOffset, currentHandle);
    }
}

void TextPattern::ChangeFirstHandleHeight(const Offset& touchOffset, RectF& handleRect)
{
    auto height = handleRect.Height();
    CalculateDefaultHandleHeight(height);
    bool isTouchHandleCircle = LessNotEqual(touchOffset.GetY(), handleRect.Top());
    if (!isTouchHandleCircle) {
        handleRect.SetTop(static_cast<float>(touchOffset.GetY()) - height / 2.0f);
    }
    handleRect.SetHeight(height);
}

void TextPattern::ChangeSecondHandleHeight(const Offset& touchOffset, RectF& handleRect)
{
    auto height = handleRect.Height();
    CalculateDefaultHandleHeight(height);
    bool isTouchHandleCircle = GreatNotEqual(touchOffset.GetY(), handleRect.Bottom());
    auto handleOffsetY = isTouchHandleCircle
                            ? handleRect.Bottom() - height
                            : static_cast<float>(touchOffset.GetY()) - height / 2.0f;
    handleRect.SetTop(handleOffsetY);
    handleRect.SetHeight(height);
}

void TextPattern::CalculateDefaultHandleHeight(float& height)
{
    CHECK_NULL_VOID(textStyle_.has_value());
#ifdef ENABLE_ROSEN_BACKEND
    MeasureContext content;
    content.textContent = "a";
    content.fontSize = textStyle_.value().GetFontSize();
    auto fontweight = StringUtils::FontWeightToString(textStyle_.value().GetFontWeight());
    content.fontWeight = fontweight;
    height = std::max(static_cast<float>(RosenRenderCustomPaint::MeasureTextSizeInner(content).Height()), 0.0f);
#endif
}

void TextPattern::DumpAdvanceInfo(std::unique_ptr<JsonValue>& json)
{
    json->Put("contentRect", contentRect_.ToString().c_str());
    if (SystemProperties::GetDebugEnabled() && pManager_) {
        std::unique_ptr<JsonValue> children = JsonUtil::Create(true);
        children->Put("DidExceedMaxLines", std::to_string(pManager_->DidExceedMaxLines()).c_str());
        children->Put("GetTextWidth", std::to_string(pManager_->GetTextWidth()).c_str());
        children->Put("GetHeight", std::to_string(pManager_->GetHeight()).c_str());
        children->Put("GetMaxWidth", std::to_string(pManager_->GetMaxWidth()).c_str());
        children->Put("GetMaxIntrinsicWidth", std::to_string(pManager_->GetMaxIntrinsicWidth()).c_str());
        children->Put("GetLineCount", std::to_string(pManager_->GetLineCount()).c_str());
        children->Put("GetLongestLine", std::to_string(pManager_->GetLongestLine()).c_str());
        children->Put("GetLongestLineWithIndent", std::to_string(pManager_->GetLongestLineWithIndent()).c_str());
        json->Put("from TextEngine paragraphs_ info", children);
    }
    json->Put("BindSelectionMenu", std::to_string(selectionMenuMap_.empty()).c_str());
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto fontScale = pipeline->GetFontScale();
    auto fontWeightScale = pipeline->GetFontWeightScale();
    json->Put("fontScale", std::to_string(fontScale).c_str());
    json->Put("fontWeightScale", std::to_string(fontWeightScale).c_str());
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    if (renderContext->HasForegroundColor()) {
        json->Put("ForegroundColor", renderContext->GetForegroundColorValue().ColorToString().c_str());
    }
    if (renderContext->GetForegroundColorStrategy().has_value()) {
        auto strategy = static_cast<int32_t>(renderContext->GetForegroundColorStrategyValue());
        json->Put("ForegroundColorStrategy", strategy);
    }
}

void TextPattern::SetTextStyleDumpInfo(std::unique_ptr<JsonValue>& json)
{
    if (textStyle_.has_value()) {
        json->Put("MaxFontSize", textStyle_->GetAdaptMaxFontSize().ToString().c_str());
        json->Put("MinFontSize", textStyle_->GetAdaptMinFontSize().ToString().c_str());
        json->Put("FontWeight", StringUtils::ToString(textStyle_->GetFontWeight()).c_str());
        json->Put("FontStyle", StringUtils::ToString(textStyle_->GetFontStyle()).c_str());
        json->Put("LineHeight", textStyle_->GetLineHeight().ToString().c_str());
        json->Put("LineSpacing", textStyle_->GetLineSpacing().ToString().c_str());
        json->Put("BaselineOffset", textStyle_->GetBaselineOffset().ToString().c_str());
        json->Put("TextIndent", textStyle_->GetTextIndent().ToString().c_str());
        json->Put("LetterSpacing", textStyle_->GetLetterSpacing().ToString().c_str());
        json->Put("TextOverflow", StringUtils::ToString(textStyle_->GetTextOverflow()).c_str());
        json->Put("TextAlign", StringUtils::ToString(textStyle_->GetTextAlign()).c_str());
        json->Put("WordBreak", StringUtils::ToString(textStyle_->GetWordBreak()).c_str());
        json->Put("TextCase", StringUtils::ToString(textStyle_->GetTextCase()).c_str());
        json->Put("EllipsisMode", StringUtils::ToString(textStyle_->GetEllipsisMode()).c_str());
        json->Put("LineBreakStrategy", GetLineBreakStrategyInJson(textStyle_->GetLineBreakStrategy()).c_str());
    }
}

void TextPattern::DumpInfo(std::unique_ptr<JsonValue>& json)
{
    auto textLayoutProp = GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProp);
    auto nowTime = GetSystemTimestamp();
    json->Put("time", std::to_string(nowTime).c_str());
    if (!IsSetObscured()) {
        json->Put("Content", UtfUtils::Str16ToStr8(textLayoutProp->GetContent().value_or(u" ")).c_str());
    }
    json->Put("ConteFontColornt",
        (textStyle_.has_value() ? textStyle_->GetTextColor() : Color::BLACK).ColorToString().c_str());
    json->Put(
        "FontSize", (textStyle_.has_value() ? textStyle_->GetFontSize() : Dimension(DIMENSION_VALUE, DimensionUnit::FP))
                        .ToString()
                        .c_str());
    SetTextStyleDumpInfo(json);
    json->Put("HeightAdaptivePolicy",
        V2::ConvertWrapTextHeightAdaptivePolicyToString(
            textLayoutProp->GetHeightAdaptivePolicy().value_or(TextHeightAdaptivePolicy::MAX_LINES_FIRST))
            .c_str());

    json->Put("Selection", textSelector_.ToString().c_str());

    if (pManager_ && !pManager_->GetParagraphs().empty()) {
        auto num = static_cast<int32_t>(pManager_->GetParagraphs().size());
        json->Put("Paragraphs num", std::to_string(num).c_str());
        json->Put("PaintInfo", paintInfo_.c_str());
    }
    if (SystemProperties::GetDebugEnabled()) {
        DumpAdvanceInfo(json);
    }
}

bool TextPattern::HasContent()
{
    if (GetTextForDisplay().empty()) {
        for (const auto& span : spans_) {
            if (span->spanItemType != SpanItemType::NORMAL) {
                return true;
            }
        }
        return false;
    }
    return true;
}

void TextPattern::SetupMagnifier()
{
    GetOrCreateMagnifier();
    CHECK_NULL_VOID(magnifierController_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    if (renderContext->GetClipEdge().value_or(false)) {
        return;
    }
    RectF viewPort;
    if (selectOverlay_->GetClipHandleViewPort(viewPort)) {
        viewPort.SetHeight(std::min(pManager_->GetHeight(), viewPort.Height()));
        magnifierController_->SetHostViewPort(viewPort);
    }
}

void TextPattern::DoTextSelectionTouchCancel()
{
    CHECK_NULL_VOID(magnifierController_);
    magnifierController_->RemoveMagnifierFrameNode();
    ResetSelection();
}

void TextPattern::BeforeSyncGeometryProperties(const DirtySwapConfig& config)
{
    if (afterLayoutCallback_.has_value()) {
        (*afterLayoutCallback_)();
    }
}
} // namespace OHOS::Ace::NG

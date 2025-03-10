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

#include "core/components_ng/pattern/rich_editor/rich_editor_select_overlay.h"

#include "core/components_ng/pattern/rich_editor/rich_editor_pattern.h"
#include "core/components/text_overlay/text_overlay_theme.h"

namespace OHOS::Ace::NG {
namespace {
constexpr float BOX_EPSILON = 0.5f;
constexpr float DOUBLE = 2.0f;
}

bool RichEditorSelectOverlay::PreProcessOverlay(const OverlayRequest& request)
{
    auto pipeline = PipelineContext::GetCurrentContextSafely();
    CHECK_NULL_RETURN(pipeline, false);
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_RETURN(pattern, false);
    SetUsingMouse(pattern->IsUsingMouse());
    auto host = pattern->GetHost();
    CHECK_NULL_RETURN(host, false);
    pipeline->AddOnAreaChangeNode(host->GetId());
    SetEnableHandleLevel(true);
    SetEnableSubWindowMenu(true);
    CheckEnableContainerModal();
    return true;
}

std::optional<SelectHandleInfo> RichEditorSelectOverlay::GetFirstHandleInfo()
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_RETURN(pattern, std::nullopt);
    SelectHandleInfo handleInfo;
    handleInfo.paintRect = pattern->textSelector_.firstHandle;
    handleInfo.isShow = dragHandleIndex_ == DragHandleIndex::FIRST || CheckHandleVisible(handleInfo.paintRect);

    auto localPaintRect = handleInfo.paintRect;
    localPaintRect.SetOffset(localPaintRect.GetOffset() - GetPaintOffsetWithoutTransform());
    handleInfo.localPaintRect = localPaintRect;
    SetTransformPaintInfo(handleInfo, localPaintRect);
    return handleInfo;
}

std::optional<SelectHandleInfo> RichEditorSelectOverlay::GetSecondHandleInfo()
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_RETURN(pattern, std::nullopt);
    SelectHandleInfo handleInfo;
    handleInfo.paintRect = pattern->textSelector_.secondHandle;
    handleInfo.isShow = (dragHandleIndex_ == DragHandleIndex::SECOND && !IsSingleHandle()) ||
        CheckHandleVisible(handleInfo.paintRect);

    auto localPaintRect = handleInfo.paintRect;
    localPaintRect.SetOffset(localPaintRect.GetOffset() - GetPaintOffsetWithoutTransform());
    handleInfo.localPaintRect = localPaintRect;
    SetTransformPaintInfo(handleInfo, localPaintRect);
    return handleInfo;
}

bool RichEditorSelectOverlay::CheckHandleVisible(const RectF& paintRect)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_RETURN(pattern, false);
    auto host = pattern->GetHost();
    CHECK_NULL_RETURN(host, false);
    if (IsUsingMouse()) {
        TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "No need to show handle when using mouse");
        return false;
    }

    auto visibleRect = GetVisibleRect();
    if (visibleRect.IsEmpty()) {
        return false;
    }
    auto paintLeft = paintRect.Left() + paintRect.Width() / 2.0f;
    PointF bottomPoint = { paintLeft, paintRect.Bottom() - BOX_EPSILON };
    PointF topPoint = { paintLeft, paintRect.Top() + BOX_EPSILON };
    visibleRect.SetLeft(visibleRect.GetX() - BOX_EPSILON);
    visibleRect.SetWidth(visibleRect.Width() + DOUBLE * BOX_EPSILON);
    visibleRect.SetTop(visibleRect.GetY() - BOX_EPSILON);
    visibleRect.SetHeight(visibleRect.Height() + DOUBLE * BOX_EPSILON);
    return visibleRect.IsInRegion(bottomPoint) && visibleRect.IsInRegion(topPoint);
}

RectF RichEditorSelectOverlay::GetVisibleRect()
{
    RectF visibleRect;
    auto pattern = GetPattern<Pattern>();
    CHECK_NULL_RETURN(pattern, visibleRect);
    auto host = pattern->GetHost();
    CHECK_NULL_RETURN(host, visibleRect);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, visibleRect);
    OffsetF paddingOffset = geometryNode->GetPaddingOffset() - geometryNode->GetFrameOffset();
    auto paintOffset = host->GetPaintRectWithTransform().GetOffset();
    visibleRect = RectF(paddingOffset + paintOffset, geometryNode->GetPaddingSize());
    CalculateClippedRect(visibleRect);
    return visibleRect;
}

void RichEditorSelectOverlay::OnResetTextSelection()
{
    auto textPattern = GetPattern<TextPattern>();
    CHECK_NULL_VOID(textPattern);
    textPattern->ResetSelection();
}

void RichEditorSelectOverlay::AfterCloseOverlay()
{
    RemoveAreaChangeInner();
    CloseMagnifier();
}

void RichEditorSelectOverlay::RemoveAreaChangeInner()
{
    auto textPattern = GetPattern<TextPattern>();
    CHECK_NULL_VOID(textPattern);
    textPattern->RemoveAreaChangeInner();
}

void RichEditorSelectOverlay::CloseMagnifier()
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->magnifierController_->RemoveMagnifierFrameNode();
}

void RichEditorSelectOverlay::OnHandleMove(const RectF& handleRect, bool isFirst)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    CHECK_NULL_VOID(pattern->HasFocus());
    CHECK_NULL_VOID(SelectOverlayIsOn());
    CHECK_NULL_VOID(!pattern->spans_.empty());
    auto host = pattern->GetHost();
    CHECK_NULL_VOID(host);
    // the handle position is calculated based on the middle of the handle height.
    auto handleOffset = GetHandleReferenceOffset(handleRect);
    UpdateSelectorOnHandleMove(handleOffset, isFirst);
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    auto overlayManager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(overlayManager);
    overlayManager->MarkInfoChange(DIRTY_SELECT_TEXT);
    auto localOffset = handleRect.GetOffset();
    if (IsOverlayMode()) {
        auto parentGlobalOffset = hasTransform_ ? GetPaintOffsetWithoutTransform() : pattern->GetParentGlobalOffset();
        localOffset = localOffset - parentGlobalOffset; // original offset
    }
    SetMagnifierOffset(localOffset, handleRect);

    bool isChangeSecondHandle = isFirst ? pattern->textSelector_.StartGreaterDest() :
        (!pattern->textSelector_.StartGreaterDest());
    IF_TRUE(isChangeSecondHandle, pattern->TriggerAvoidOnCaretChange());
    AutoScrollParam param = { .autoScrollEvent = AutoScrollEvent::HANDLE,
        .handleRect = handleRect,
        .isFirstHandle = isFirst,
        .showScrollbar = true };
    pattern->AutoScrollByEdgeDetection(param, localOffset, EdgeDetectionStrategy::OUT_BOUNDARY);
}

void RichEditorSelectOverlay::SetMagnifierOffset(const OffsetF& localOffset, const RectF& handleRect)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    if (IsSingleHandle()) {
        auto [caretOffset, caretHeight] = pattern->CalculateCaretOffsetAndHeight();
        auto floatingCaretCenter = Offset(localOffset.GetX(), caretOffset.GetY() + caretHeight / 2);
        pattern->SetMagnifierOffsetWithAnimation(floatingCaretCenter);
    } else {
        auto handleCenter = Offset(localOffset.GetX(), localOffset.GetY() + handleRect.Height() / 2);
        pattern->SetMagnifierLocalOffset(handleCenter);
    }
}

void RichEditorSelectOverlay::UpdateSelectorOnHandleMove(const OffsetF& handleOffset, bool isFirst)
{
    auto pattern = GetPattern<RichEditorPattern>();
    auto& textSelector = pattern->textSelector_;
    auto currentHandleIndex = pattern->GetHandleIndex(Offset(handleOffset.GetX(), handleOffset.GetY()));
    auto preHandleIndex = isFirst ? textSelector.baseOffset : textSelector.destinationOffset;
    pattern->SetCaretPosition(currentHandleIndex);
    if (isFirst) {
        pattern->HandleSelectionChange(currentHandleIndex, initSelector_.second);
    } else {
        if (IsSingleHandle()) {
            auto localOffset = handleOffset + pattern->contentRect_.GetOffset();
            auto textOffset = localOffset - pattern->richTextRect_.GetOffset();
            pattern->CalcAndRecordLastClickCaretInfo(Offset(textOffset.GetX(), textOffset.GetY()));
            textSelector.Update(currentHandleIndex);
            IF_TRUE(isHandleMoving_, pattern->SetCaretTouchMoveOffset(Offset(localOffset.GetX(), localOffset.GetY())));
        } else {
            pattern->HandleSelectionChange(initSelector_.first, currentHandleIndex);
        }
    }
    auto finalHandleIndex = isFirst ? textSelector.baseOffset : textSelector.destinationOffset;
    IF_TRUE(!IsSingleHandle(), pattern->StartVibratorByIndexChange(finalHandleIndex, preHandleIndex));
}

void RichEditorSelectOverlay::OnHandleMoveDone(const RectF& handleRect, bool isFirstHandle)
{
    BaseTextSelectOverlay::OnHandleMoveDone(handleRect, isFirstHandle);
    isHandleMoving_ = false;
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "handleRect=%{public}s, isFirstHandle=%{public}d, isSingle=%{public}d",
        handleRect.ToString().c_str(), isFirstHandle, IsSingleHandle());
    auto host = pattern->GetHost();
    CHECK_NULL_VOID(host);
    auto& textSelector = pattern->textSelector_;
    auto selectStart = std::min(textSelector.baseOffset, textSelector.destinationOffset);
    auto selectEnd = std::max(textSelector.baseOffset, textSelector.destinationOffset);
    pattern->FireOnSelect(selectStart, selectEnd);
    if (!IsSingleHandle()) {
        pattern->SetCaretPositionWithAffinity({ selectEnd, TextAffinity::UPSTREAM });
    } else {
        pattern->StartFloatingCaretLand();
    }
    pattern->StopAutoScroll();
    pattern->magnifierController_->RemoveMagnifierFrameNode();
    if (!IsSingleHandle() && textSelector.StartEqualToDest()) {
        HideMenu();
        CloseOverlay(true, CloseReason::CLOSE_REASON_NORMAL);
        IF_TRUE(pattern->IsEditing(), pattern->StartTwinkling());
        return;
    }
    auto overlayManager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(overlayManager);
    overlayManager->SetHandleCircleIsShow(isFirstHandle, true);
    if (!isFirstHandle && IsSingleHandle()) {
        overlayManager->SetIsHandleLineShow(true);
    }
    pattern->CalculateHandleOffsetAndShowOverlay();
    overlayManager->MarkInfoChange((isFirstHandle ? DIRTY_FIRST_HANDLE : DIRTY_SECOND_HANDLE) | DIRTY_SELECT_AREA |
                            DIRTY_SELECT_TEXT | DIRTY_COPY_ALL_ITEM);
    ProcessOverlay({ .animation = true });
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

std::string RichEditorSelectOverlay::GetSelectedText()
{
    return TextSelectOverlay::GetSelectedText();
}

RectF RichEditorSelectOverlay::GetSelectAreaFromRects(SelectRectsType pos)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_RETURN(pattern, {});
    auto intersectRect = pattern->GetSelectArea(pos);

    if (hasTransform_) {
        auto textPaintOffset = GetPaintOffsetWithoutTransform();
        intersectRect.SetOffset(intersectRect.GetOffset() - textPaintOffset);
        GetGlobalRectWithTransform(intersectRect);
    }
    ApplySelectAreaWithKeyboard(intersectRect);
    return intersectRect;
}

bool RichEditorSelectOverlay::IsStopBackPress() const
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_RETURN(pattern, true);
    return pattern->IsStopBackPress();
}

void RichEditorSelectOverlay::OnUpdateMenuInfo(SelectMenuInfo& menuInfo, SelectOverlayDirtyFlag dirtyFlag)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    auto hasValue = pattern->GetTextContentLength() > 0;
    menuInfo.showCopyAll = !pattern->IsSelectAll() && hasValue;
    if (dirtyFlag == DIRTY_COPY_ALL_ITEM) {
        return;
    }
    bool isShowItem = pattern->copyOption_ != CopyOptions::None;
    menuInfo.showCopy = isShowItem && hasValue && !pattern->textSelector_.SelectNothing();
    menuInfo.showCut = isShowItem && hasValue && !pattern->textSelector_.SelectNothing();
    menuInfo.showPaste = IsShowPaste();
    menuInfo.menuIsShow = IsShowMenu();
    menuInfo.showTranslate = menuInfo.showCopy && pattern->IsShowTranslate() && IsNeedMenuTranslate();
    menuInfo.showShare = menuInfo.showCopy && IsSupportMenuShare() && IsNeedMenuShare();
    menuInfo.showSearch = menuInfo.showCopy && pattern->IsShowSearch() && IsNeedMenuSearch();
    menuInfo.showAIWrite = pattern->IsShowAIWrite() && hasValue;
    menuInfo.menuType = IsUsingMouse() ? OptionMenuType::MOUSE_MENU : OptionMenuType::TOUCH_MENU;
    pattern->UpdateSelectMenuInfo(menuInfo);
}

// param filling except callback
void RichEditorSelectOverlay::OnUpdateSelectOverlayInfo(SelectOverlayInfo& selectInfo, int32_t requestCode)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    BaseTextSelectOverlay::OnUpdateSelectOverlayInfo(selectInfo, requestCode);
    selectInfo.pattern = AceType::WeakClaim(AceType::RawPtr(pattern));
    selectInfo.handlerColor = pattern->GetCaretColor();
    selectInfo.handleReverse = IsHandleReverse();
    OnUpdateOnCreateMenuCallback(selectInfo);
    bool usingMouse = pattern->IsUsingMouse();
    auto responseType = pattern->textResponseType_.value_or(TextResponseType::NONE);
    auto& firstHandle = pattern->textSelector_.firstHandle;
    auto& secondHandle = pattern->textSelector_.secondHandle;
    if (usingMouse && pattern->sourceType_ == SourceType::MOUSE) {
        if (responseType == TextResponseType::LONG_PRESS) {
            pattern->SetTextResponseType(TextResponseType::RIGHT_CLICK);
            responseType = TextResponseType::RIGHT_CLICK;
        }
        selectInfo.isUsingMouse = true;
        selectInfo.rightClickOffset = pattern->GetSelectionMenuOffset();
        pattern->ResetIsMousePressed();
    } else {
        selectInfo.firstHandle.paintRect = firstHandle;
        selectInfo.secondHandle.paintRect = secondHandle;
    }
    selectInfo.menuInfo.responseType = static_cast<int32_t>(responseType);
    selectInfo.menuInfo.editorType = static_cast<int32_t>(pattern->GetEditorType());
    selectInfo.callerFrameNode = pattern->GetHost();
    selectInfo.isNewAvoid = true;
    selectInfo.selectArea = GetSelectArea();
    selectInfo.checkIsTouchInHostArea =
    [weak = AceType::WeakClaim(AceType::RawPtr(pattern))](const PointF& touchPoint) -> bool {
        auto pattern = weak.Upgrade();
        CHECK_NULL_RETURN(pattern, false);
        return pattern->IsTouchInFrameArea(touchPoint);
    };
    selectInfo.isSingleHandle = IsSingleHandle();
    selectInfo.recreateOverlay = requestCode == REQUEST_RECREATE;
    CheckMenuParamChange(selectInfo, pattern->GetEditorType(), responseType);
    pattern->CopySelectionMenuParams(selectInfo, responseType);
    if (hasTransform_) {
        selectInfo.callerNodeInfo = {
            .paintFrameRect = GetPaintRectWithTransform(),
            .paintOffset = GetPaintRectOffsetWithTransform()
        };
    }
}

void RichEditorSelectOverlay::OnUpdateOnCreateMenuCallback(SelectOverlayInfo& selectInfo)
{
    BaseTextSelectOverlay::OnUpdateOnCreateMenuCallback(selectInfo);
    selectInfo.menuCallback.showMenuOnMoveDone = [weak = WeakClaim(this)]() {
        auto overlay = weak.Upgrade();
        CHECK_NULL_RETURN(overlay, true);
        auto pattern = overlay->GetPattern<RichEditorPattern>();
        CHECK_NULL_RETURN(pattern, true);
        return !pattern->IsSelectedTypeChange();
    };
}

void RichEditorSelectOverlay::CheckMenuParamChange(SelectOverlayInfo& selectInfo,
    TextSpanType selectType, TextResponseType responseType)
{
    auto pattern = GetPattern<RichEditorPattern>();
    auto menuParams = pattern ? pattern->GetMenuParams(selectType, responseType) : nullptr;
    std::pair<TextSpanType, TextResponseType> selectResponseComb = { selectType, responseType };
    selectInfo.recreateOverlay |= (lastMenuParams_ || menuParams) && selectResponseComb != lastSelectResponseComb_;
    lastMenuParams_ = menuParams;
    lastSelectResponseComb_ = selectResponseComb;
}

// set menu callback
void RichEditorSelectOverlay::OnMenuItemAction(OptionMenuActionId id, OptionMenuType type)
{
    TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "MenuActionId=%{public}d, MenuType=%{public}d", id, type);
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    switch (id) {
        case OptionMenuActionId::COPY:
            needRefreshMenu_ = !IsShowPaste() && pattern->copyOption_ != CopyOptions::None;
            pattern->HandleOnCopy();
            break;
        case OptionMenuActionId::CUT:
            pattern->HandleOnCut();
            break;
        case OptionMenuActionId::PASTE:
            pattern->HandleOnPaste();
            CloseOverlay(true, CloseReason::CLOSE_REASON_NORMAL);
            break;
        case OptionMenuActionId::SELECT_ALL:
            pattern->HandleMenuCallbackOnSelectAll();
            break;
        case OptionMenuActionId::TRANSLATE:
            HandleOnTranslate();
            return;
        case OptionMenuActionId::SHARE:
            pattern->HandleOnShare();
            break;
        case OptionMenuActionId::SEARCH:
            HandleOnSearch();
            break;
        case OptionMenuActionId::CAMERA_INPUT:
            pattern->HandleOnCameraInput();
            break;
        case OptionMenuActionId::AI_WRITE:
            pattern->HandleOnAIWrite();
            break;
        case OptionMenuActionId::DISAPPEAR:
            if (pattern->GetTextDetectEnable() && !pattern->HasFocus()) {
                pattern->ResetSelection();
            }
            break;
        default:
            TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "Unsupported menu option id %{public}d", id);
            break;
    }
}

bool RichEditorSelectOverlay::IsMenuShow()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    return manager && manager->IsMenuShow();
}

void RichEditorSelectOverlay::ToggleMenu()
{
    if (IsMenuShow()) {
        HideMenu();
        return;
    }
    if (needRefreshMenu_) {
        needRefreshMenu_ = false;
        ProcessOverlay({ .menuIsShow = true, .animation = true, .requestCode = REQUEST_RECREATE });
    } else {
        UpdateMenuOffset();
        ShowMenu();
    }
}

void RichEditorSelectOverlay::OnCloseOverlay(OptionMenuType menuType, CloseReason reason, RefPtr<OverlayInfo> info)
{
    bool isSingleHandle = info && info->isSingleHandle;
    TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "menuType=%{public}d, closeReason=%{public}d, isSingleHandle=%{public}d",
        menuType, reason, isSingleHandle);
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    BaseTextSelectOverlay::OnCloseOverlay(menuType, reason, info);
    isHandleMoving_ = false;
    if (isSingleHandle) {
        pattern->floatingCaretState_.Reset();
        pattern->isCursorAlwaysDisplayed_ = false;
    }
    auto needResetSelection = pattern->GetTextDetectEnable() && !pattern->HasFocus() &&
        reason != CloseReason::CLOSE_REASON_DRAG_FLOATING;
    auto isBackPressed = reason == CloseReason::CLOSE_REASON_BACK_PRESSED;
    auto isHoldByOther = reason == CloseReason::CLOSE_REASON_HOLD_BY_OTHER;
    needResetSelection = needResetSelection || isBackPressed || isHoldByOther;
    IF_TRUE(needResetSelection, pattern->ResetSelection());
    IF_TRUE(isHoldByOther, pattern->CloseSelectOverlay());
    if (isBackPressed) {
        IF_TRUE(isSingleHandle, pattern->OnBackPressed());
        if (!pattern->IsEditing() && pattern->HasFocus()) {
            FocusHub::LostFocusToViewRoot();
        }
        ResumeTwinkling();
    }
}

void RichEditorSelectOverlay::OnHandleGlobalTouchEvent(SourceType sourceType, TouchType touchType, bool touchInside)
{
    BaseTextSelectOverlay::OnHandleGlobalTouchEvent(sourceType, touchType);
    CHECK_NULL_VOID(IsMouseClickDown(sourceType, touchType) || IsTouchUp(sourceType, touchType));
    if (IsSingleHandle()) {
        CloseOverlay(false, CloseReason::CLOSE_REASON_CLICK_OUTSIDE);
        ResumeTwinkling();
    } else {
        HideMenu();
    }
}

void RichEditorSelectOverlay::OnHandleLevelModeChanged(HandleLevelMode mode)
{
    if (handleLevelMode_ != mode && mode == HandleLevelMode::OVERLAY) {
        auto pattern = GetPattern<RichEditorPattern>();
        CHECK_NULL_VOID(pattern);
        pattern->CalculateHandleOffsetAndShowOverlay();
    }
    BaseTextSelectOverlay::OnHandleLevelModeChanged(mode);
}

std::optional<SelectOverlayInfo> RichEditorSelectOverlay::GetSelectOverlayInfo()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_RETURN(manager, std::optional<SelectOverlayInfo>());
    return manager->GetSelectOverlayInfo();
}

bool RichEditorSelectOverlay::IsSingleHandleShow()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_RETURN(manager && manager->IsSingleHandle(), false);
    auto overlayInfo = manager->GetSelectOverlayInfo();
    CHECK_NULL_RETURN(overlayInfo, false);
    return overlayInfo->secondHandle.isShow;
}

void RichEditorSelectOverlay::UpdateMenuOffset()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(manager);
    manager->MarkInfoChange(DIRTY_SELECT_AREA | DIRTY_ALL_MENU_ITEM);
}

bool RichEditorSelectOverlay::IsBothHandlesShow()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_RETURN(manager && manager->IsHandlesShow(), false);
    auto overlayInfo = manager->GetSelectOverlayInfo();
    CHECK_NULL_RETURN(overlayInfo, false);
    return overlayInfo->firstHandle.isShow && overlayInfo->secondHandle.isShow;
}

bool RichEditorSelectOverlay::IsHandleShow()
{
    return IsBothHandlesShow() || IsSingleHandleShow();
}

void RichEditorSelectOverlay::OnAncestorNodeChanged(FrameNodeChangeInfoFlag flag)
{
    if (IsAncestorNodeGeometryChange(flag)) {
        UpdateAllHandlesOffset();
    }
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    auto host = pattern->GetHost();
    CHECK_NULL_VOID(host);
    FrameNodeChangeInfoFlag changeFlag = flag;
    if (IsAncestorNodeStartScroll(host->GetChangeInfoFlag()) || IsAncestorNodeEndScroll(host->GetChangeInfoFlag())) {
        auto parent = host->GetAncestorNodeOfFrame(true);
        changeFlag = FRAME_NODE_CHANGE_INFO_NONE;
        while (parent) {
            if (parent->GetChangeInfoFlag() != FRAME_NODE_CHANGE_INFO_NONE) {
                changeFlag = changeFlag | parent->GetChangeInfoFlag();
            }
            parent = parent->GetAncestorNodeOfFrame(true);
        }
    }
    BaseTextSelectOverlay::OnAncestorNodeChanged(changeFlag);
}

void RichEditorSelectOverlay::OnHandleMoveStart(const GestureEvent& event, bool isFirst)
{
    BaseTextSelectOverlay::OnHandleMoveStart(event, isFirst);
    isHandleMoving_ = true;
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    initSelector_ = { pattern->textSelector_.GetTextStart(), pattern->textSelector_.GetTextEnd() };
    IF_TRUE(!IsSingleHandle(), pattern->ChangeHandleHeight(event, isFirst, IsOverlayMode()));
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(manager);
    manager->MarkInfoChange(isFirst ? DIRTY_FIRST_HANDLE : DIRTY_SECOND_HANDLE);
    manager->SetHandleCircleIsShow(isFirst, false);
    if (IsSingleHandle()) {
        pattern->ShowCaretWithoutTwinkling();
        manager->SetIsHandleLineShow(false);
    }
}

void RichEditorSelectOverlay::OnOverlayTouchDown(const TouchEventInfo& event)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    if (event.GetSourceTool() == SourceTool::MOUSE && IsHandleShow()) {
        pattern->CloseSelectOverlay();
    }
    pattern->RequestFocusWhenSelected();
}

void RichEditorSelectOverlay::UpdateHandleOffset()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(manager);
    manager->MarkInfoChange(DIRTY_FIRST_HANDLE | DIRTY_SECOND_HANDLE);
}

void RichEditorSelectOverlay::UpdateFirstHandleOffset()
{
    CHECK_NULL_VOID(dragHandleIndex_ != DragHandleIndex::FIRST);
    BaseTextSelectOverlay::UpdateFirstHandleOffset();
}

void RichEditorSelectOverlay::UpdateSecondHandleOffset()
{
    CHECK_NULL_VOID(dragHandleIndex_ != DragHandleIndex::SECOND);
    BaseTextSelectOverlay::UpdateSecondHandleOffset();
}

void RichEditorSelectOverlay::UpdateSelectOverlayOnAreaChanged()
{
    CHECK_NULL_VOID(SelectOverlayIsOn() || SelectOverlayIsCreating());
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->CalculateHandleOffsetAndShowOverlay();
    auto menuParams = pattern->GetMenuParams(pattern->GetEditorType(),
        pattern->textResponseType_.value_or(TextResponseType::NONE));
    bool needRecreate = (lastMenuParams_ == nullptr) ^ (menuParams == nullptr);
    if (needRecreate) {
        TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "menu type changed, recreate menu");
        ProcessOverlay({ .menuIsShow = IsCurrentMenuVisibile(), .requestCode = REQUEST_RECREATE });
    } else {
        UpdateHandleOffset();
        if (IsShowMouseMenu()) {
            CloseOverlay(true, CloseReason::CLOSE_REASON_NORMAL);
            return;
        }
        IF_TRUE(IsMenuShow(), UpdateMenuOffset());
    }
}

void RichEditorSelectOverlay::SwitchCaretState(std::shared_ptr<SelectOverlayInfo> info)
{
    CHECK_NULL_VOID(info && IsSingleHandle() && !isHandleMoving_);
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    bool isSingleHandleShow = !handleIsHidden_ && info->secondHandle.isShow;
    bool isCaretTwinkling = pattern->caretTwinkling_ && !pattern->isCursorAlwaysDisplayed_;
    CHECK_NULL_VOID(isSingleHandleShow == isCaretTwinkling);
    TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "Switch caret state singleHandleShow=%{public}d", isSingleHandleShow);
    if (isSingleHandleShow) {
        pattern->StopTwinkling();
    } else {
        ResumeTwinkling();
    }
}

void RichEditorSelectOverlay::ResumeTwinkling()
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern && pattern->IsEditing());
    TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "only show caret for edit state");
    pattern->isCursorAlwaysDisplayed_ = false;
    pattern->StartTwinkling();
}

void RichEditorSelectOverlay::OnHandleIsHidden()
{
    TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "Start twinking when singleHandle hide");
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->isCursorAlwaysDisplayed_ = false;
    pattern->StartTwinkling();
    handleIsHidden_ = true;
}

void RichEditorSelectOverlay::OnOverlayClick(const GestureEvent& event, bool isFirst)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    if (!pattern->IsEditing() && !IsSingleHandle()) {
        ToggleMenu();
    }
    auto globalOffset = pattern->GetGlobalOffset();
    auto overlayEvent = event;
    auto localLocation = Offset(overlayEvent.GetGlobalLocation().GetX() - globalOffset.GetX(),
        overlayEvent.GetGlobalLocation().GetY() - globalOffset.GetY());
    overlayEvent.SetLocalLocation(localLocation);
    pattern->HandleClickEvent(overlayEvent);
}

void RichEditorSelectOverlay::OnHandleMouseEvent(const MouseInfo& event)
{
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    if (event.GetAction() == MouseAction::PRESS && IsHandleShow()) {
        pattern->CloseSelectOverlay();
    }
}

void RichEditorSelectOverlay::OnAfterSelectOverlayShow(bool isCreate)
{
    handleIsHidden_ = false;
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(manager);
    manager->MarkInfoChange(DIRTY_SELECT_AREA);
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    IF_TRUE(IsSingleHandleShow(), pattern->StopTwinkling());
    if (IsRightButtonCustomMenuShow()) {
        TAG_LOGI(AceLogTag::ACE_RICH_TEXT, "Change mouse style to default after right-button custom menu show");
        pattern->ChangeMouseStyle(MouseFormat::DEFAULT);
    }
}

bool RichEditorSelectOverlay::IsRightButtonCustomMenuShow()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_RETURN(manager && manager->IsMenuShow(), false);
    auto overlayInfo = manager->GetSelectOverlayInfo();
    CHECK_NULL_RETURN(overlayInfo, false);
    auto menuInfo = overlayInfo->menuInfo;
    auto responseType = menuInfo.responseType.value_or(static_cast<int>(TextResponseType::NONE));
    return responseType == static_cast<int>(TextResponseType::RIGHT_CLICK) && menuInfo.menuBuilder != nullptr;
}

float RichEditorSelectOverlay::GetHandleHotZoneRadius()
{
    auto hotZoneRadius = 0.0f;
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_RETURN(pattern, hotZoneRadius);
    auto host = pattern->GetHost();
    CHECK_NULL_RETURN(host, hotZoneRadius);
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, hotZoneRadius);
    auto theme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_RETURN(theme, hotZoneRadius);
    hotZoneRadius = theme->GetHandleHotZoneRadius().ConvertToPx();
    return hotZoneRadius;
}

void RichEditorSelectOverlay::OnHandleMarkInfoChange(
    std::shared_ptr<SelectOverlayInfo> info, SelectOverlayDirtyFlag flag)
{
    IF_TRUE((flag & DIRTY_SECOND_HANDLE) == DIRTY_SECOND_HANDLE, SwitchCaretState(info));
    CHECK_NULL_VOID((flag & DIRTY_HANDLE_COLOR_FLAG) == DIRTY_HANDLE_COLOR_FLAG);
    CHECK_NULL_VOID(info);

    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(manager);
    auto pattern = GetPattern<RichEditorPattern>();
    CHECK_NULL_VOID(pattern);
    info->handlerColor = pattern->caretColor_;
    manager->MarkHandleDirtyNode(PROPERTY_UPDATE_RENDER);
}

void RichEditorSelectOverlay::UpdateHandleColor()
{
    auto manager = GetManager<SelectContentOverlayManager>();
    CHECK_NULL_VOID(manager);
    manager->MarkInfoChange(DIRTY_HANDLE_COLOR_FLAG);
}

} // namespace OHOS::Ace::NG

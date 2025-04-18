/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "event_listener.h"
#include "ace_log.h"
#include "jerryscript.h"
#include "js_fwk_common.h"
#include "root_view.h"
#include "js_app_context.h"

namespace OHOS {
namespace ACELite {
#ifdef JS_EXTRA_EVENT_SUPPORT
KeyBoardEventListener::KeyBoardEventListener(jerry_value_t fn, const uint16_t id)
    : fn_ (jerry_acquire_value(fn)), id_ (id) {}

KeyBoardEventListener::~KeyBoardEventListener()
{
    jerry_release_value(fn_);
}

bool KeyBoardEventListener::OnKeyAct(UIView &view, const KeyEvent &event)
{
    if (jerry_value_is_undefined(fn_)) {
        return false;
    }

    jerry_value_t *args = ConvertKeyEventInfo(event);
    jerry_release_value(CallJSFunctionOnRoot(fn_, args, 1));
    ClearEventListener(args, 1);
    return true;
}

ViewOnTouchCancelListener::ViewOnTouchCancelListener(jerry_value_t fn, uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

ViewOnTouchCancelListener::~ViewOnTouchCancelListener()
{
    jerry_release_value(fn_);
}

bool ViewOnTouchCancelListener::OnCancel(UIView &view, const CancelEvent &event)
{
    return CallBaseEvent(fn_, event, id_);
}
#endif // JS_EXTRA_EVENT_SUPPORT

void ViewOnTouchListener::SetBindTouchStartFuncName(jerry_value_t bindTouchStartFunc)
{
    if (!jerry_value_is_undefined(bindTouchStartFunc)) {
        bindTouchStartFunc_ = jerry_acquire_value(bindTouchStartFunc);
    }
}

void ViewOnTouchListener::SetBindTouchMoveFuncName(jerry_value_t bindTouchMoveFunc)
{
    if (!jerry_value_is_undefined(bindTouchMoveFunc)) {
        bindTouchMoveFunc_ = jerry_acquire_value(bindTouchMoveFunc);
    }
}


void ViewOnTouchListener::SetBindTouchEndFuncName(jerry_value_t bindTouchEndFunc)
{
    if (!jerry_value_is_undefined(bindTouchEndFunc)) {
        bindTouchEndFunc_ = jerry_acquire_value(bindTouchEndFunc);
    }
}

void ViewOnTouchListener::SetBindSwipeFuncName(jerry_value_t bindSwipeFunc)
{
    if (!jerry_value_is_undefined(bindSwipeFunc)) {
        bindSwipeFunc_ = jerry_acquire_value(bindSwipeFunc);
    }
}

void ViewOnTouchListener::SetStopPropagation(bool isStopPropogation)
{
    isStopPropagation_ = isStopPropogation;
}

bool ViewOnTouchListener::OnDragStart(UIView& view, const DragEvent &event)
{
    if (!AsyncTaskManager::GetInstance().IsFront()) {
        return isStopPropagation_;
    }

    if (JSUndefined::Is(bindTouchStartFunc_)) {
        return isStopPropagation_;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDragStart received");

    JSValue arg = EventUtil::CreateTouchEvent(view, event);
    EventUtil::InvokeCallback(vm_, bindTouchStartFunc_, arg, this);
    return isStopPropagation_;
}

bool ViewOnTouchListener::OnDrag(UIView& view, const DragEvent& event)
{
    if (!AsyncTaskManager::GetInstance().IsFront()) {
        return isStopPropagation_;
    }

    if (JSUndefined::Is(bindTouchMoveFunc_)) {
        return isStopPropagation_;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDrag received");

    JSValue arg = EventUtil::CreateTouchEvent(view, event);
    EventUtil::InvokeCallback(vm_, bindTouchMoveFunc_, arg, this);
    return isStopPropagation_;
}

bool ViewOnTouchListener::OnDragEnd(UIView& view, const DragEvent &event)
{
    if (!AsyncTaskManager::GetInstance().IsFront()) {
        return isStopPropagation_;
    }

    if (!JSUndefined::Is(bindSwipeFunc_)) {
        JSValue argSwipe = EventUtil::CreateSwipeEvent(view, event);
        EventUtil::InvokeCallback(vm_, bindSwipeFunc_, argSwipe, this);
    }

    if (!JSUndefined::Is(bindTouchEndFunc_)) {
        JSValue argDragEnd = EventUtil::CreateTouchEvent(view, event);
        EventUtil::InvokeCallback(vm_, bindTouchEndFunc_, argDragEnd, this);
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDragEnd received");
    HILOG_DEBUG(HILOG_MODULE_ACE, "Swipe received");
    return isStopPropagation_;
}

void ValueChangeListener::OnChange(UIView &view, const char *value)
{
    if (IS_UNDEFINED(fn_)) {
        return;
    }

    jerry_value_t textValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_value_t args[1] = {textValue};
    if (jerry_value_is_function(fn_)) {
        CallJSFunctionAutoRelease(fn_, UNDEFINED, args, 1);
    }
    ReleaseJerryValue(textValue, VA_ARG_END_FLAG);
    return;
}

void ListEventListener::EventExcute(const int16_t index, jerry_value_t bindScrollFunc) const
{
        TopAbilityState abilityState = JsAppContext::GetInstance()->GetAbilityState();
        if (abilityState != TopAbilityState::ABILITY_SHOWN) {
            return;
        }

        if (IS_UNDEFINED(bindScrollFunc)) {
            return;
        }

        int8_t currentState = this->GetScrollState();
        jerry_value_t currentStateValue = jerry_create_number(currentState);
        jerry_value_t componentIndex = jerry_create_number(index);
        jerry_value_t args[ARGS_LEN] = {currentStateValue, componentIndex};
        CallJSFunctionAutoRelease(bindScrollFunc, UNDEFINED, args, ARGS_LEN);
        ReleaseJerryValue(currentStateValue, componentIndex, VA_ARG_END_FLAG);
}
} // namespace ACELite
} // namespace OHOS

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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_key_function.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"

namespace OHOS::Ace::Framework {

JSRef<JSObject> JsKeyFunction::createKeyEvent(KeyEventInfo& event)
{
    JSRef<JSObjTemplate> objectTemplate = JSRef<JSObjTemplate>::New();
    objectTemplate->SetInternalFieldCount(1);
    JSRef<JSObject> keyEventObj = objectTemplate->NewInstance();
    keyEventObj->SetProperty<int32_t>("type", static_cast<int32_t>(event.GetKeyType()));
    keyEventObj->SetProperty<int32_t>("keyCode", static_cast<int32_t>(event.GetKeyCode()));
    keyEventObj->SetProperty<const char*>("keyText", event.GetKeyText().c_str());
    keyEventObj->SetProperty<int32_t>("keySource", static_cast<int32_t>(event.GetKeySource()));
    keyEventObj->SetProperty<int64_t>("deviceId", event.GetDeviceId());
    keyEventObj->SetProperty<int32_t>("metaKey", event.GetMetaKey());
    keyEventObj->SetProperty<uint32_t>("unicode", event.GetUnicode());
    keyEventObj->SetProperty<double>("timestamp", static_cast<double>(event.GetTimeStamp().time_since_epoch().count()));
    keyEventObj->SetPropertyObject("stopPropagation", JSRef<JSFunc>::New<FunctionCallback>(JsStopPropagation));
    keyEventObj->SetPropertyObject("getModifierKeyState",
        JSRef<JSFunc>::New<FunctionCallback>(NG::ArkTSUtils::JsGetModifierKeyState));
    keyEventObj->SetProperty<int32_t>("intentionCode", static_cast<int32_t>(event.GetKeyIntention()));
    keyEventObj->SetProperty<bool>("isNumLockOn", event.GetNumLock());
    keyEventObj->SetProperty<bool>("isCapsLockOn", event.GetCapsLock());
    keyEventObj->SetProperty<bool>("isScrollLockOn", event.GetScrollLock());
    keyEventObj->Wrap<KeyEventInfo>(&event);
    return keyEventObj;
}

void JsKeyFunction::Execute(OHOS::Ace::KeyEventInfo& event)
{
    JSRef<JSVal> param = JSRef<JSObject>::Cast(createKeyEvent(event));
    JsFunction::ExecuteJS(1, &param);
}

JSRef<JSVal> JsKeyFunction::ExecuteWithValue(OHOS::Ace::KeyEventInfo& event)
{
    JSRef<JSVal> param = JSRef<JSObject>::Cast(createKeyEvent(event));
    return JsFunction::ExecuteJS(1, &param);
}

} // namespace OHOS::Ace::Framework

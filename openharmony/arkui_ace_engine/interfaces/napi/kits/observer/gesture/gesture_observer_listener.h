/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_INTERFACES_GESTURE_GESTURE_OBSERVER_LISTENER_H
#define FOUNDATION_ACE_INTERFACES_GESTURE_GESTURE_OBSERVER_LISTENER_H

#include "napi/native_api.h"

#include "base/memory/ace_type.h"
#include "core/components_ng/base/observer_handler.h"
#include "core/components_ng/gestures/recognizers/gesture_recognizer.h"

namespace OHOS::Ace::Napi {
enum class GestureRecognizerState {
    UNKNOWN = -1,
    READY = 0,
    DETECTING = 1,
    PENDING = 2,
    BLOCKED = 3,
    SUCCEED = 4,
    FAIL = 5,
};
class GestureObserverListener final {
public:
    GestureObserverListener() = default;
    ~GestureObserverListener() = default;
    static void AddGestureRecognizerInfo(napi_env env, napi_value objValueGestureRecognizer,
        const RefPtr<NG::NGGestureRecognizer>& current, NG::GestureListenerType gestureListenerType);
    static void CreateRecognizerObject(napi_env env, napi_value objValueGestureRecognizer,
        const RefPtr<NG::NGGestureRecognizer>& current, NG::GestureListenerType gestureListenerType);
    static bool MatchValueType(napi_env env, napi_value value, napi_valuetype targetType);
    static GestureRecognizerState ConvertRefereeState(NG::RefereeState state);
};

} // namespace OHOS::Ace::Napi
#endif // FOUNDATION_ACE_INTERFACES_GESTURE_GESTURE_OBSERVER_LISTENER_H

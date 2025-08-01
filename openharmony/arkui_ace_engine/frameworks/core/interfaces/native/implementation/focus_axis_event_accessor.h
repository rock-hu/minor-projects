/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#pragma once

#include "arkoala_api_generated.h"

#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/implementation/focus_axis_event_peer.h"
#include "core/interfaces/native/utility/callback_helper.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/peer_utils.h"
#include "core/interfaces/native/utility/reverse_converter.h"

namespace OHOS::Ace::NG::GeneratedModifier::FocusAxisEventAccessor {
inline std::map<AxisModel, float> getAxisMapFromInfo(const FocusAxisEventInfo& info)
{
    return {
        { AxisModel::ABS_X, info.GetAbsXValue() },
        { AxisModel::ABS_Y, info.GetAbsYValue() },
        { AxisModel::ABS_Z, info.GetAbsZValue() },
        { AxisModel::ABS_RZ, info.GetAbsRzValue() },
        { AxisModel::ABS_GAS, info.GetAbsGasValue() },
        { AxisModel::ABS_BRAKE, info.GetAbsBrakeValue() },
        { AxisModel::ABS_HAT0X, info.GetAbsHat0XValue() },
        { AxisModel::ABS_HAT0Y, info.GetAbsHat0YValue() },
    };
}
} // namespace OHOS::Ace::NG::FocusAxisEventAccessor
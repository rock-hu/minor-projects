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

#include "adapter/ohos/entrance/picker/picker_haptic_impl.h"

namespace OHOS::Ace::NG {

PickerAudioHapticImpl::PickerAudioHapticImpl(const std::string& uri, const std::string& effectId)
{
    handler_ = std::make_unique<PickerHapticController>(uri, effectId);
}

void PickerAudioHapticImpl::Play(size_t speed)
{
    if (handler_) {
        handler_->Play(speed);
    }
}

void PickerAudioHapticImpl::PlayOnce()
{
    if (handler_) {
        handler_->PlayOnce();
    }
}

void PickerAudioHapticImpl::Stop()
{
    if (handler_) {
        handler_->Stop();
    }
}

void PickerAudioHapticImpl::HandleDelta(double dy)
{
    if (handler_) {
        handler_->HandleDelta(dy);
    }
}

} // namespace OHOS::Ace::NG

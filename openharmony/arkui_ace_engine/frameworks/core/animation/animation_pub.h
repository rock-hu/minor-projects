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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_PUB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_PUB_H

#include "ui/animation/animation_constants.h"

namespace OHOS::Ace {

const char* GetAnimationInterfaceName(AnimationInterface interfaceName);

using ShareId = std::string;

int32_t GetAnimationFinishCount();

constexpr uint32_t ROTATION_PARAM_SIZE = 3;
constexpr uint32_t TRANSLATION_PARAM_SIZE = 2;
constexpr uint32_t SCALE_PARAM_SIZE = 2;
constexpr uint32_t OPACITY_PARAM_SIZE = 1;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_PUB_H

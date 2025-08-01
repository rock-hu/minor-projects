/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ARKUI_ACE_ENGINE_TEST_MOCK_CORE_RENDER_MOCK_ANIMATION_UTILS_H
#define FOUNDATION_ARKUI_ACE_ENGINE_TEST_MOCK_CORE_RENDER_MOCK_ANIMATION_UTILS_H

#ifdef ENHANCED_ANIMATION
#include "test/mock/core/animation/mock_implicit_animation.h"
#include "test/mock/core/animation/mock_animation_manager.h"
#endif

#include "core/components_ng/base/modifier.h"
#include "core/components_ng/render/animation_utils.h"

namespace OHOS::Ace {

class AnimationUtils::InteractiveAnimation {
public:
    FinishCallback finishCallback_;
    float progress_ = 0.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ARKUI_ACE_ENGINE_TEST_MOCK_CORE_RENDER_MOCK_ANIMATION_UTILS_H

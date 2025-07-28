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
#include "sceneNodeParameters.proj.hpp"
#include "sceneNodeParameters.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "sceneNodeParameters.h"

namespace {
// To be implemented.

::sceneNodeParameters::SceneNodeParameters GetSceneNodeParameters()
{
    return taihe::make_holder<SceneNodeParametersImpl, ::sceneNodeParameters::SceneNodeParameters>();
}
}  // namespace

TH_EXPORT_CPP_API_GetSceneNodeParameters(GetSceneNodeParameters);
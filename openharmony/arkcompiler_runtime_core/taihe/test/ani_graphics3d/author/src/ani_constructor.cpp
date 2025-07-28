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
#include "scene.ani.hpp"
#include "taihe.platform.ani.ani.hpp"
#include "sceneNodeParameters.ani.hpp"
#if __has_include(<ani.h>)
#include <ani.h>
#elif __has_include(<ani/ani.h>)
#include <ani/ani.h>
#else
#error "ani.h not found. Please ensure the Ani SDK is correctly installed."
#endif
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result) {
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        return ANI_ERROR;
    }
    if (ANI_OK != scene::ANIRegister(env)) {
        std::cerr << "Error from scene::ANIRegister" << std::endl;
        return ANI_ERROR;
    }
    if (ANI_OK != taihe::platform::ani::ANIRegister(env)) {
        std::cerr << "Error from taihe::platform::ani::ANIRegister" << std::endl;
        return ANI_ERROR;
    }
    if (ANI_OK != sceneNodeParameters::ANIRegister(env)) {
        std::cerr << "Error from sceneNodeParameters::ANIRegister" << std::endl;
        return ANI_ERROR;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}

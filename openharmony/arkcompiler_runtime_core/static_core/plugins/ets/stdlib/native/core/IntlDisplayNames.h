/**
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

#ifndef PANDA_PLUGINS_ETS_STDLIB_NATIVE_CORE_INTLDISPLAYNAMES_H
#define PANDA_PLUGINS_ETS_STDLIB_NATIVE_CORE_INTLDISPLAYNAMES_H

#include "ani/ani.h"

namespace ark::ets::stdlib::intl {
ani_status RegisterIntlDisplayNames(ani_env *env);
}  // namespace ark::ets::stdlib::intl

#endif

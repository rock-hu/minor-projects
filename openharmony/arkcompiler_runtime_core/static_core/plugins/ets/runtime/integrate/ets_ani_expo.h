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
#ifndef PLUGINS_ETS_RUNTIME_INTEGRATE_ETS_ANI_EXPO
#define PLUGINS_ETS_RUNTIME_INTEGRATE_ETS_ANI_EXPO
// NOLINTBEGIN

#ifdef __cplusplus
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#else
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#endif
#include <string>
#include <vector>
#include <ani.h>

#include "libpandabase/macros.h"

namespace ark::ets {
class PANDA_PUBLIC_API ETSAni {
public:
    static constexpr std::string_view AOT_FILE_OPTION_PREFIX = "--ext:--aot-file=";
    static constexpr std::string_view INTEROP_OPTION_PREFIX = "--ext:interop";
    static constexpr std::string_view ENABLE_AN_OPTION = "--ext:--enable-an";
    static void Prefork(ani_env *env);
    static void Postfork(ani_env *env, const std::vector<ani_option> &options, bool postZygoteFork = true);

private:
    static void TryLoadAotFileForBoot();
    static void LoadAotFileForApp(std::string const &aotFileName);
};
}  // namespace ark::ets
#endif  // !PLUGINS_ETS_RUNTIME_INTEGRATE_ETS_ANI_EXPO

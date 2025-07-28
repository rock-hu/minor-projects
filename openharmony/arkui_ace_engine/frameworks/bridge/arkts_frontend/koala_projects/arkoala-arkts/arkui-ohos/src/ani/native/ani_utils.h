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

#ifndef KOALA_PROJECTS_ARKOALA_ARKTS_ARKUI_OHOS_ANI_NATIVE_ANI_UTILS
#define KOALA_PROJECTS_ARKOALA_ARKTS_ARKUI_OHOS_ANI_NATIVE_ANI_UTILS

#include "ani.h"
#include <string>

namespace OHOS::Ace::Ani {
class AniUtils {
public:
    static ani_object CreateDouble(ani_env *env, double value);
    static bool CheckType(ani_env *env, ani_object obj, const std::string& type);
    static bool GetIntByName(ani_env *env, ani_object param, const char *name, int &value);
    static bool GetBoolOrUndefined(ani_env *env, ani_object param, const char *name);
};
} // namespace OHOS::Ace::Ani

#endif // KOALA_PROJECTS_ARKOALA_ARKTS_ARKUI_OHOS_ANI_NATIVE_ANI_UTILS
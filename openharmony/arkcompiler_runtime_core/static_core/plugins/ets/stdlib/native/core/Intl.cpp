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

#include "plugins/ets/stdlib/native/core/Intl.h"
#include "plugins/ets/stdlib/native/core/IntlState.h"
#include "plugins/ets/stdlib/native/core/IntlNumberFormat.h"
#include "plugins/ets/stdlib/native/core/IntlLocaleMatch.h"
#include "plugins/ets/stdlib/native/core/IntlCollator.h"
#include "plugins/ets/stdlib/native/core/IntlSegmenter.h"
#include "plugins/ets/stdlib/native/core/IntlCommon.h"
#include "plugins/ets/stdlib/native/core/IntlLocale.h"
#include "plugins/ets/stdlib/native/core/IntlPluralRules.h"
#include "plugins/ets/stdlib/native/core/IntlDateTimeFormat.h"

#include "plugins/ets/runtime/ets_napi_env.h"
#include "ani/ani.h"

namespace ark::ets::stdlib::intl {

ani_status InitCoreIntl(ani_env *env)
{
    // Create internal data
    CreateIntlState();

    // Register Native methods. Stop if an error occurred
    ani_status err = RegisterIntlNumberFormatNativeMethods(env);
    err = err == ANI_OK ? RegisterIntlLocaleMatch(env) : err;
    err = err == ANI_OK ? RegisterIntlCollator(env) : err;
    err = err == ANI_OK ? RegisterIntlLocaleNativeMethods(env) : err;
    err = err == ANI_OK ? RegisterIntlPluralRules(env) : err;
    err = err == ANI_OK ? RegisterIntlDateTimeFormatMethods(env) : err;
    err = err == ANI_OK ? RegisterIntlSegmenter(env) : err;
    return err;
}

}  // namespace ark::ets::stdlib::intl

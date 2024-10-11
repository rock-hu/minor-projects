/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_RUNTIME_LANGUAGE_CONFIG_H_
#define PANDA_RUNTIME_LANGUAGE_CONFIG_H_

#include "runtime/mem/lock_config_helper.h"

namespace ark {

class PandaAssemblyLanguageConfig {
public:
    static constexpr panda_file::SourceLang LANG = panda_file::SourceLang::PANDA_ASSEMBLY;
    static constexpr LangTypeT LANG_TYPE = LANG_TYPE_STATIC;
    static constexpr MTModeT MT_MODE = MT_MODE_MULTI;
    static constexpr bool HAS_VALUE_OBJECT_TYPES = false;
};

#include "language_config_gen.inc"

}  // namespace ark

#endif  // PANDA_RUNTIME_LANGUAGE_CONFIG_H_

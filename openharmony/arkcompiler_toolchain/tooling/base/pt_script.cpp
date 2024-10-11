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

#include "tooling/base/pt_script.h"

#include "ecmascript/debugger/debugger_api.h"

namespace panda::ecmascript::tooling {
PtScript::PtScript(ScriptId scriptId, const std::string &fileName, const std::string &url, const std::string &source)
    : scriptId_(scriptId),
      fileName_(fileName),
      url_(url),
      scriptSource_(source)
{
    endLine_ = std::count(scriptSource_.begin(), scriptSource_.end(), '\n');
}
}  // namespace panda::ecmascript::tooling
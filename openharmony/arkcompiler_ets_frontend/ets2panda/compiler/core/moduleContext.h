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

#ifndef ES2PANDA_COMPILER_CORE_MODULE_CONTEXT_H
#define ES2PANDA_COMPILER_CORE_MODULE_CONTEXT_H

#include "util/ustring.h"

#include <vector>

namespace ark::es2panda::varbinder {
class Variable;
class ModuleScope;
}  // namespace ark::es2panda::varbinder

namespace ark::es2panda::compiler {
class PandaGen;

class ModuleContext {
public:
    ModuleContext() = delete;

    static void Compile(PandaGen *pg, varbinder::ModuleScope *scope);
};
}  // namespace ark::es2panda::compiler

#endif

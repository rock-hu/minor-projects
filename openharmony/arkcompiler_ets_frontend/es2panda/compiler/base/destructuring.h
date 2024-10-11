/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_COMPILER_BASE_DESTRUCTURING_H
#define ES2PANDA_COMPILER_BASE_DESTRUCTURING_H

#include <ir/expression.h>

namespace panda::es2panda::compiler {

class PandaGen;

class Destructuring {
public:
    Destructuring() = delete;

    static void Compile(PandaGen *pg, const ir::Expression *pattern);
};

}  // namespace panda::es2panda::compiler

#endif

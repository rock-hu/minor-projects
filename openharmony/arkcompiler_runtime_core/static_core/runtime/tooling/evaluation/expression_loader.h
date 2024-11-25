/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_RUNTIME_TOOLING_EVALUATION_HELPERS_H
#define PANDA_RUNTIME_TOOLING_EVALUATION_HELPERS_H

#include <string>

#include "runtime/class_linker_context.h"
#include "runtime/include/tooling/debug_interface.h"

namespace ark::tooling {

Expected<Method *, Error> LoadExpressionBytecode(ClassLinkerContext *ctx, const std::string &bytecode);

}  // namespace ark::tooling

#endif  // PANDA_RUNTIME_TOOLING_EVALUATION_HELPERS_H

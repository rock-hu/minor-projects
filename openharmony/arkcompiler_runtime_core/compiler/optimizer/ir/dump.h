/**
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

#ifndef COMPILER_OPTIMIZER_IR_DUMP_H
#define COMPILER_OPTIMIZER_IR_DUMP_H

#include "utils/arena_containers.h"

namespace panda::compiler {

ArenaString IdToString(uint32_t id, ArenaAllocator *allocator, bool v_reg = false, bool is_phi = false);

ArenaString InstId(const Inst *inst, ArenaAllocator *allocator);

ArenaString BBId(const BasicBlock *block, ArenaAllocator *allocator);

void DumpUsers(const Inst *inst, std::ostream *out);

}  // namespace panda::compiler

#endif  // COMPILER_OPTIMIZER_IR_DUMP_H

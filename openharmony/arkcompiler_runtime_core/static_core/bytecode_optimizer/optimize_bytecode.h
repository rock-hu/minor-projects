/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_BYTECODE_OPTIMIZER_OPTIMIZE_BYTECODE_H
#define PANDA_BYTECODE_OPTIMIZER_OPTIMIZE_BYTECODE_H

#include "assembler/assembly-emitter.h"
#include "assembler/assembly-program.h"
#include "compiler/optimizer/ir/graph.h"
#include "ir_interface.h"
#include "libpandabase/macros.h"

namespace ark::bytecodeopt {
PANDA_PUBLIC_API bool RunOptimizations(compiler::Graph *graph, BytecodeOptIrInterface *iface = nullptr);
PANDA_PUBLIC_API bool OptimizeBytecode(pandasm::Program *prog, const pandasm::AsmEmitter::PandaFileToPandaAsmMaps *maps,
                                       const std::string &pandafileName, bool isDynamic = false,
                                       bool hasMemoryPool = false);
}  // namespace ark::bytecodeopt

#endif  // PANDA_BYTECODE_OPTIMIZER_OPTIMIZE_BYTECODE_H

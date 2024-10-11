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

#ifndef COMPILER_OPTIMIZER_OPTIMIZATIONS_MOVE_CONSTANTS_H
#define COMPILER_OPTIMIZER_OPTIMIZATIONS_MOVE_CONSTANTS_H

#include "optimizer/pass.h"
#include "optimizer/ir/basicblock.h"
#include "compiler_options.h"

namespace panda::compiler {

class MoveConstants : public Optimization {
public:
    explicit MoveConstants(Graph *graph);

    NO_MOVE_SEMANTIC(MoveConstants);
    NO_COPY_SEMANTIC(MoveConstants);
    ~MoveConstants() override = default;

    bool RunImpl() override;
    bool IsEnable() const override
    {
        return options.IsCompilerMoveConstants();
    }

    const char *GetPassName() const override
    {
        return "MoveConstants";
    }

private:
    ArenaUnorderedMap<uint32_t, ArenaVector<BasicBlock *>> user_dominators_cache_;
    ArenaVector<const ArenaVector<BasicBlock *> *> user_dominating_blocks_;
    int moved_constants_counter_;

    void MoveFromStartBlock(Inst *inst);
    void GetUsersDominatingBlocks(const Inst *inst);
    BasicBlock *FindCommonDominator();
    const ArenaVector<BasicBlock *> *GetDominators(const User &user);
};

}  // namespace panda::compiler

#endif  // COMPILER_OPTIMIZER_OPTIMIZATIONS_MOVE_CONSTANTS_H

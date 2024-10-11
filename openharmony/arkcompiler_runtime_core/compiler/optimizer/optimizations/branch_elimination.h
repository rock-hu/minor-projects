/*
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

#ifndef COMPILER_OPTIMIZER_OPTIMIZATIONS_BRANCH_ELIMINATION_H
#define COMPILER_OPTIMIZER_OPTIMIZATIONS_BRANCH_ELIMINATION_H

#include "optimizer/ir/graph.h"
#include "optimizer/pass.h"
#include "utils/arena_containers.h"

namespace panda::compiler {
class BranchElimination : public Optimization {
public:
    explicit BranchElimination(Graph *graph);

    NO_MOVE_SEMANTIC(BranchElimination);
    NO_COPY_SEMANTIC(BranchElimination);
    ~BranchElimination() override = default;

    bool RunImpl() override;

    bool IsEnable() const override
    {
        return options.IsCompilerBranchElimination();
    }

    const char *GetPassName() const override
    {
        return "BranchElimination";
    }
    void InvalidateAnalyses() override;

private:
    void VisitBlock(BasicBlock *ifBlock);
    void EliminateBranch(BasicBlock *ifBlock, BasicBlock *eliminatedBlock);
    void MarkUnreachableBlocks(BasicBlock *block);
    void DisconnectBlocks();
    void BranchEliminationConst(BasicBlock *ifBlock);
    void BranchEliminationIntrinsic(BasicBlock *ifBlock);

private:
    bool isApplied_ {false};
    Marker rmBlockMarker_ {UNDEF_MARKER};
};

}  // namespace panda::compiler

#endif  // COMPILER_OPTIMIZER_OPTIMIZATIONS_BRANCH_ELIMINATION_H

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

namespace ark::compiler {
class BranchElimination : public Optimization {
    struct ConditionOps {
        Inst *lhs;
        Inst *rhs;
    };

    struct CcEqual {
        bool operator()(const ConditionOps &obj1, const ConditionOps &obj2) const
        {
            return std::tie(obj1.lhs, obj1.rhs) == std::tie(obj2.lhs, obj2.rhs) ||
                   std::tie(obj1.lhs, obj1.rhs) == std::tie(obj2.rhs, obj2.lhs);
        }
    };

    struct CcHash {
        uint32_t operator()(const ConditionOps &obj) const
        {
            uint32_t hash = std::hash<Inst *> {}(obj.lhs);
            hash += std::hash<Inst *> {}(obj.rhs);
            return hash;
        }
    };

public:
    explicit BranchElimination(Graph *graph);

    NO_MOVE_SEMANTIC(BranchElimination);
    NO_COPY_SEMANTIC(BranchElimination);
    ~BranchElimination() override = default;

    bool RunImpl() override;

    bool IsEnable() const override
    {
        return g_options.IsCompilerBranchElimination();
    }

    const char *GetPassName() const override
    {
        return "BranchElimination";
    }
    void InvalidateAnalyses() override;

private:
    bool SkipForOsr(const BasicBlock *block);
    void VisitBlock(BasicBlock *ifBlock);
    void EliminateBranch(BasicBlock *ifBlock, BasicBlock *eliminatedBlock);
    void MarkUnreachableBlocks(BasicBlock *block);
    void DisconnectBlocks();
    std::optional<bool> GetConditionResult(Inst *condition);
    std::optional<bool> TryResolveResult(Inst *condition, Inst *dominantCondition, IfImmInst *ifImmBlock);
    void BranchEliminationConst(BasicBlock *ifBlock);
    void BranchEliminationCompare(BasicBlock *ifBlock);
    void BranchEliminationCompareAnyType(BasicBlock *ifBlock);
    std::optional<bool> GetCompareAnyTypeResult(IfImmInst *ifImm);
    std::optional<bool> TryResolveCompareAnyTypeResult(CompareAnyTypeInst *compareAny,
                                                       CompareAnyTypeInst *domCompareAny, IfImmInst *ifImmDomBlock);

private:
    bool isApplied_ {false};
    Marker rmBlockMarker_ {UNDEF_MARKER};
    ArenaUnorderedMap<ConditionOps, InstVector, CcHash, CcEqual> sameInputCompares_;
    ArenaUnorderedMap<Inst *, ArenaVector<CompareAnyTypeInst *>> sameInputCompareAnyType_;
};

}  // namespace ark::compiler

#endif  // COMPILER_OPTIMIZER_OPTIMIZATIONS_BRANCH_ELIMINATION_H

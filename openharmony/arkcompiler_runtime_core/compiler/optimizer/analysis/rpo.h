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

#ifndef COMPILER_OPTIMIZER_ANALYSIS_RPO_H
#define COMPILER_OPTIMIZER_ANALYSIS_RPO_H

#include <algorithm>
#include "utils/arena_containers.h"
#include "optimizer/ir/marker.h"
#include "optimizer/pass.h"

namespace panda::compiler {
class BasicBlock;
class Graph;

/**
 * This class builds blocks list for reverse postorder traversal.
 * There is an option to invalidate blocks list. In this case the list
 * will be build from scratch after the next request for it.
 * Also it provides methods for updating an existing tree.
 */
class Rpo : public Analysis {
public:
    explicit Rpo(Graph *graph);

    NO_MOVE_SEMANTIC(Rpo);
    NO_COPY_SEMANTIC(Rpo);
    ~Rpo() override = default;

    const char *GetPassName() const override
    {
        return "RPO";
    }

public:
    void RemoveBasicBlock(BasicBlock *rm_block)
    {
        ASSERT_PRINT(IsValid(), "RPO is invalid");
        auto it = std::find(rpo_vector_.begin(), rpo_vector_.end(), rm_block);
        if (it != rpo_vector_.end()) {
            rpo_vector_.erase(it);
        }
    }

    void AddBasicBlockAfter(BasicBlock *cur_block, BasicBlock *new_block)
    {
        ASSERT_PRINT(IsValid(), "RPO is invalid");
        auto it = std::find(rpo_vector_.begin(), rpo_vector_.end(), cur_block);
        rpo_vector_.insert(it + 1, new_block);
    }

    void AddBasicBlockBefore(BasicBlock *cur_block, BasicBlock *new_block)
    {
        ASSERT_PRINT(IsValid(), "RPO is invalid");
        auto it = std::find(rpo_vector_.begin(), rpo_vector_.end(), cur_block);
        rpo_vector_.insert(it, new_block);
    }

    void AddVectorAfter(BasicBlock *cur_block, const ArenaVector<BasicBlock *> &new_vector)
    {
        ASSERT_PRINT(IsValid(), "RPO is invalid");
        auto it = std::find(rpo_vector_.begin(), rpo_vector_.end(), cur_block);
        rpo_vector_.insert(it + 1, new_vector.begin(), new_vector.end());
    }

    const ArenaVector<BasicBlock *> &GetBlocks() const
    {
        return rpo_vector_;
    }

private:
    bool RunImpl() override;

    void DFS(BasicBlock *block, size_t *blocks_count);

private:
    Marker marker_ {UNDEF_MARKER};
    ArenaVector<BasicBlock *> rpo_vector_;
};
}  // namespace panda::compiler

#endif  // COMPILER_OPTIMIZER_ANALYSIS_RPO_H

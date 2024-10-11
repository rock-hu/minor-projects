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

#ifndef COMPILER_OPTIMIZER_IR_BASICBLOCK_H
#define COMPILER_OPTIMIZER_IR_BASICBLOCK_H

#include <iterator>
#include "constants.h"
#include "inst.h"
#include "marker.h"
#include "macros.h"

namespace panda::compiler {
class Graph;
class Loop;

enum class IterationType : uint8_t {
    PHI,
    INST,
    ALL,
};

enum class IterationDirection : uint8_t {
    FORWARD,
    BACKWARD,
};

template <const IterationType T>
class InstForwardIterator;
template <const IterationType T>
class InstBackwardIterator;
template <const IterationType T, const IterationDirection D>
class InstSafeIterator;

using PhiInstIter = InstForwardIterator<IterationType::PHI>;
using InstIter = InstForwardIterator<IterationType::INST>;
using AllInstIter = InstForwardIterator<IterationType::ALL>;

using InstReverseIter = InstBackwardIterator<IterationType::INST>;

using PhiInstSafeIter = InstSafeIterator<IterationType::PHI, IterationDirection::FORWARD>;
using InstSafeIter = InstSafeIterator<IterationType::INST, IterationDirection::FORWARD>;
using AllInstSafeIter = InstSafeIterator<IterationType::ALL, IterationDirection::FORWARD>;

using PhiInstSafeReverseIter = InstSafeIterator<IterationType::PHI, IterationDirection::BACKWARD>;
using InstSafeReverseIter = InstSafeIterator<IterationType::INST, IterationDirection::BACKWARD>;
using AllInstSafeReverseIter = InstSafeIterator<IterationType::ALL, IterationDirection::BACKWARD>;

class BasicBlock : public MarkerSet {  // , public ArenaObject<ARENA_ALLOC_BASIC_BLOCK>
public:
    explicit BasicBlock(Graph *graph, uint32_t guest_pc = INVALID_PC);

public:
    void SetId(uint32_t id)
    {
        bb_id_ = id;
    }
    uint32_t GetId() const
    {
        return bb_id_;
    }

    ArenaVector<BasicBlock *> &GetPredsBlocks()
    {
        return preds_;
    }
    const ArenaVector<BasicBlock *> &GetPredsBlocks() const
    {
        return preds_;
    }

    ArenaVector<BasicBlock *> &GetSuccsBlocks()
    {
        return succs_;
    }
    const ArenaVector<BasicBlock *> &GetSuccsBlocks() const
    {
        return succs_;
    }

    BasicBlock *GetSuccessor(size_t index) const
    {
        ASSERT(index < succs_.size());
        return succs_[index];
    }

    BasicBlock *GetPredecessor(size_t index) const
    {
        ASSERT(index < preds_.size());
        return preds_[index];
    }

    template <bool invert = false>
    void SwapTrueFalseSuccessors()
    {
        ASSERT(succs_.size() > 1);
        std::swap(succs_[0], succs_[1]);
        if constexpr (invert) {  // NOLINT(readability-braces-around-statements,bugprone-suspicious-semicolon)
            inverted_ = !inverted_;
        }
        // If both succs are irruducible loop entrypoints, swapping can invalidate that loop header, since it's not
        // determined
        succs_[0]->InvalidateLoopIfIrreducible();
        succs_[1]->InvalidateLoopIfIrreducible();
    }

    bool IsInverted() const
    {
        return inverted_;
    }

    BasicBlock *GetTrueSuccessor() const
    {
        ASSERT(!succs_.empty());
        return succs_[0];
    }

    BasicBlock *GetFalseSuccessor() const
    {
        ASSERT(succs_.size() > 1);
        return succs_[1];
    }

    // Get index of the given block in predecessor container
    size_t GetPredBlockIndex(const BasicBlock *block) const
    {
        auto it = std::find(preds_.begin(), preds_.end(), block);
        ASSERT(it != preds_.end());
        ASSERT(std::find(it + 1, preds_.end(), block) == preds_.end());
        return std::distance(preds_.begin(), it);
    }

    // Get index of the given block in successor container
    size_t GetSuccBlockIndex(const BasicBlock *block) const
    {
        auto it = std::find(succs_.begin(), succs_.end(), block);
        ASSERT(it != succs_.end());
        ASSERT(std::find(it + 1, succs_.end(), block) == succs_.end());
        return std::distance(succs_.begin(), it);
    }

    // Get basic block by its index in predecessors container
    BasicBlock *GetPredBlockByIndex(size_t index) const
    {
        ASSERT(index < preds_.size());
        return preds_[index];
    }

    bool IsStartBlock() const;
    bool IsEndBlock() const;
    bool IsPseudoControlFlowBlock() const;

    Graph *GetGraph()
    {
        return graph_;
    }
    const Graph *GetGraph() const
    {
        return graph_;
    }
    void SetGraph(Graph *graph)
    {
        graph_ = graph;
    }

    void SetGuestPc(uint32_t guest_pc)
    {
        guest_pc_ = guest_pc;
    }
    uint32_t GetGuestPc() const
    {
        return guest_pc_;
    }

    /**
     * Split block after the given instruction. Current block will contain instructions before the splitting line and
     * the created block - after.
     * @param inst instruction after which block will be split
     * @param make_edge whether to make control flow edge from first block to the second one
     * @return return created basic block
     */
    BasicBlock *SplitBlockAfterInstruction(Inst *inst, bool make_edge);

    // Add successor in the list
    void AddSucc(BasicBlock *succ, bool can_add_empty_block = false);

    bool HasSucc(BasicBlock *succ)
    {
        return std::find(succs_.begin(), succs_.end(), succ) != succs_.end();
    }

    void ReplacePred(BasicBlock *prev_pred, BasicBlock *new_pred)
    {
        preds_[GetPredBlockIndex(prev_pred)] = new_pred;
        new_pred->succs_.push_back(this);
    }

    void ReplaceSucc(const BasicBlock *prev_succ, BasicBlock *new_succ, bool can_add_empty_block = false);

    void ReplaceTrueSuccessor(BasicBlock *new_succ)
    {
        ASSERT(!succs_.empty());
        succs_[0] = new_succ;
        new_succ->preds_.push_back(this);
    }

    void ReplaceFalseSuccessor(BasicBlock *new_succ)
    {
        ASSERT(succs_.size() > 1);
        succs_[1] = new_succ;
        new_succ->preds_.push_back(this);
    }

    BasicBlock *InsertNewBlockToSuccEdge(BasicBlock *succ);
    BasicBlock *InsertEmptyBlockBefore();

    void InsertBlockBeforeSucc(BasicBlock *block, BasicBlock *succ);

    // Remove empty block with one successor
    void RemoveEmptyBlock(bool irr_loop = false);
    void RemoveFixLoopInfo();

    // Join single successor into single predecessor
    void JoinSuccessorBlock();

    // Add instruction to the end or begin of the BasicBlock
    template <bool to_end>
    void AddInst(Inst *inst);
    // Insert new instruction(NOT PHI) in BasicBlock at the start of instructions
    void PrependInst(Inst *inst)
    {
        AddInst<false>(inst);
    }
    // Insert new instruction(NOT PHI) in BasicBlock at the end of instructions
    void AppendInst(Inst *inst)
    {
        AddInst<true>(inst);
    }
    // Append range of instructions(NOT PHI) in BasicBlock at the end of instructions
    // It is implied that for instructions within range was already called correctly SetBasicBlock() and
    // range_last should dominate range_first.
    void AppendRangeInst(Inst *range_first, Inst *range_last);
    // Insert new PHI instruction in BasicBlock at the end of PHI instructions
    void AppendPhi(Inst *inst);
    // Insert instruction after given instruction
    void InsertAfter(Inst *inst, Inst *after);
    // Insert instruction before given instruction
    void InsertBefore(Inst *inst, Inst *before);
    // Insert range of instructions before given instruction.
    // It is implied that for instructions within range was already called correctly SetBasicBlock() and
    // range_last should dominate range_first.
    void InsertRangeBefore(Inst *range_first, Inst *range_last, Inst *before);
    // Remove instruction from instructions chain. All other data keep unmodified.
    void EraseInst(Inst *inst, [[maybe_unused]] bool will_be_moved = false);
    // Remove instructions from instructions chain, remove its inputs and check that it hasn't users.
    void RemoveInst(Inst *inst);
    // Replace old_inst in BasicBlock to new_inst
    void ReplaceInst(Inst *old_inst, Inst *new_inst);
    // Remove all instructions from bb
    void Clear();

    Inst *GetLastInst() const
    {
        return last_inst_;
    }

    Inst *GetFirstInst() const
    {
        return first_inst_;
    }

    Inst *GetFirstPhi() const
    {
        return first_phi_;
    }

    bool IsEmpty() const
    {
        return first_inst_ == nullptr;
    }

    bool HasPhi() const
    {
        return first_phi_ != nullptr;
    }

    void SetDominator(BasicBlock *dominator)
    {
        dominator_ = dominator;
    }
    void ClearDominator()
    {
        dominator_ = nullptr;
    }

    BasicBlock *CreateImmediateDominator();

    void AddDominatedBlock(BasicBlock *block)
    {
        dom_blocks_.push_back(block);
    }
    void RemoveDominatedBlock(BasicBlock *block)
    {
        dom_blocks_.erase(std::find(dom_blocks_.begin(), dom_blocks_.end(), block));
    }
    void ClearDominatedBlocks()
    {
        dom_blocks_.clear();
    }

    BasicBlock *GetDominator() const;
    const ArenaVector<BasicBlock *> &GetDominatedBlocks() const;
    bool IsDominate(const BasicBlock *other) const;

    void RemovePred(const BasicBlock *pred)
    {
        ASSERT(GetPredBlockIndex(pred) < preds_.size());
        preds_[GetPredBlockIndex(pred)] = preds_.back();
        preds_.pop_back();
    }

    void RemovePred(size_t index)
    {
        ASSERT(index < preds_.size());
        preds_[index] = preds_.back();
        preds_.pop_back();
    }

    void RemoveSucc(BasicBlock *succ)
    {
        auto it = std::find(succs_.begin(), succs_.end(), succ);
        ASSERT(it != succs_.end());
        succs_.erase(it);
    }

    void Dump(std::ostream *out) const;

    Loop *GetLoop() const
    {
        return loop_;
    }
    void SetLoop(Loop *loop)
    {
        loop_ = loop;
    }
    bool IsLoopValid() const
    {
        return GetLoop() != nullptr;
    }
    bool IsLoopHeader() const;

    void SetNextLoop(Loop *loop)
    {
        next_loop_ = loop;
    }
    Loop *GetNextLoop()
    {
        return next_loop_;
    }
    bool IsLoopPreHeader() const
    {
        return (next_loop_ != nullptr);
    }

    void InsertBlockBefore(BasicBlock *block);

    template <typename Accessor>
    typename Accessor::ValueType GetField() const
    {
        return Accessor::Get(bit_fields_);
    }

    template <typename Accessor>
    void SetField(typename Accessor::ValueType value)
    {
        Accessor::Set(value, &bit_fields_);
    }

    void SetAllFields(uint32_t bit_fields)
    {
        bit_fields_ = bit_fields;
    }

    uint32_t GetAllFields() const
    {
        return bit_fields_;
    }

    void SetMonitorEntryBlock(bool v)
    {
        SetField<MonitorEntryBlock>(v);
    }

    bool GetMonitorEntryBlock()
    {
        return GetField<MonitorEntryBlock>();
    }

    void SetMonitorExitBlock(bool v)
    {
        SetField<MonitorExitBlock>(v);
    }

    bool GetMonitorExitBlock() const
    {
        return GetField<MonitorExitBlock>();
    }

    void SetMonitorBlock(bool v)
    {
        SetField<MonitorBlock>(v);
    }

    bool GetMonitorBlock() const
    {
        return GetField<MonitorBlock>();
    }

    void SetCatch(bool v)
    {
        SetField<CatchBlock>(v);
    }

    bool IsCatch() const
    {
        return GetField<CatchBlock>();
    }

    void SetCatchBegin(bool v)
    {
        SetField<CatchBeginBlock>(v);
    }

    bool IsCatchBegin() const
    {
        return GetField<CatchBeginBlock>();
    }

    void SetCatchEnd(bool v)
    {
        SetField<CatchEndBlock>(v);
    }

    bool IsCatchEnd() const
    {
        return GetField<CatchEndBlock>();
    }

    void SetTry(bool v)
    {
        SetField<TryBlock>(v);
    }

    bool IsTry() const
    {
        return GetField<TryBlock>();
    }

    void SetTryBegin(bool v)
    {
        SetField<TryBeginBlock>(v);
    }

    bool IsTryBegin() const
    {
        return GetField<TryBeginBlock>();
    }

    void SetTryEnd(bool v)
    {
        SetField<TryEndBlock>(v);
    }

    bool IsTryEnd() const
    {
        return GetField<TryEndBlock>();
    }

    void SetOsrEntry(bool v)
    {
        SetField<OsrEntry>(v);
    }

    bool IsOsrEntry() const
    {
        return GetField<OsrEntry>();
    }

    void CopyTryCatchProps(BasicBlock *block)
    {
        if (block->IsTry()) {
            SetTry(true);
            SetTryId(block->GetTryId());
        }
        if (block->IsCatch()) {
            SetCatch(true);
        }
    }

    uint32_t GetTryId() const
    {
        return try_id_;
    }

    void SetTryId(uint32_t try_id)
    {
        try_id_ = try_id;
    }

    template <class Callback>
    void EnumerateCatchHandlers(const Callback &callback) const
    {
        ASSERT(this->IsTryBegin());
        auto try_inst = GetTryBeginInst(this);
        auto type_ids = try_inst->GetCatchTypeIds();
        auto catch_indexes = try_inst->GetCatchEdgeIndexes();
        ASSERT(type_ids != nullptr && catch_indexes != nullptr);
        for (size_t idx = 0; idx < type_ids->size(); idx++) {
            auto succ = GetSuccessor(catch_indexes->at(idx));
            while (!succ->IsCatchBegin()) {
                ASSERT(succ->GetSuccsBlocks().size() == 1);
                succ = succ->GetSuccessor(0);
            }
            ASSERT(succ != nullptr);
            if (!callback(succ, type_ids->at(idx))) {
                break;
            }
        }
    }

    BasicBlock *Clone(Graph *target_graph) const;

    void SetNeedsJump(bool v)
    {
        SetField<JumpFlag>(v);
    }

    bool NeedsJump() const
    {
        return GetField<JumpFlag>();
    }

    bool IsIfBlock() const
    {
        if (last_inst_ != nullptr) {
            if (last_inst_->GetOpcode() == Opcode::If || last_inst_->GetOpcode() == Opcode::IfImm) {
                return true;
            }
        }
        return false;
    }

    Inst *GetFistThrowableInst() const;

    void InvalidateLoopIfIrreducible();

    // Member functions to iterate instructions. `Safe` means that you can delete instructions when iterating
    PhiInstIter PhiInsts() const;
    InstIter Insts() const;
    AllInstIter AllInsts() const;

    InstReverseIter InstsReverse() const;

    PhiInstSafeIter PhiInstsSafe() const;
    InstSafeIter InstsSafe() const;
    AllInstSafeIter AllInstsSafe() const;

    PhiInstSafeReverseIter PhiInstsSafeReverse() const;
    InstSafeReverseIter InstsSafeReverse() const;
    AllInstSafeReverseIter AllInstsSafeReverse() const;

private:
    using MonitorEntryBlock = BitField<bool, 0>;           //  block with MonitorEntry
    using MonitorExitBlock = MonitorEntryBlock::NextFlag;  //  block with MonitorExit
    using MonitorBlock = MonitorExitBlock::NextFlag;       //  block between MonitorEntryBlock and MonitorExitBlock.
    using CatchBeginBlock = MonitorBlock::NextFlag;
    using CatchEndBlock = CatchBeginBlock::NextFlag;
    using CatchBlock = CatchEndBlock::NextFlag;
    using TryBeginBlock = CatchBlock::NextFlag;
    using TryEndBlock = TryBeginBlock::NextFlag;
    using TryBlock = TryEndBlock::NextFlag;
    using OsrEntry = TryBlock::NextFlag;
    using JumpFlag = OsrEntry::NextFlag;  // signals that Codegen should insert a jump to the successor
    using LastField = JumpFlag;

    struct saved_if_info {
        BasicBlock *succ;
        bool swapped;
        uint64_t if_imm;
        ConditionCode if_cc;
        DataType::Type if_type;
        uint32_t if_pc;
        Opcode if_opcode;
        Inst *if_input0;
        Inst *if_input1;
    };

    void SelectsFixLoopInfo(BasicBlock *select_bb, BasicBlock *other);

    Graph *graph_;

    // List of predessesor blocks.
    ArenaVector<BasicBlock *> preds_;

    // List of successors blocks.
    ArenaVector<BasicBlock *> succs_;

    // List of dominated blocks.
    ArenaVector<BasicBlock *> dom_blocks_;

    // Dominator block
    BasicBlock *dominator_ {nullptr};

    /// This value hold properties of the basic block. It accessed via BitField types.
    uint32_t bit_fields_ {0};

    // pc address from bytecode file
    uint32_t guest_pc_;
    uint32_t bb_id_ {INVALID_ID};

    Inst *first_phi_ {nullptr};
    Inst *first_inst_ {nullptr};
    Inst *last_inst_ {nullptr};

    Loop *loop_ {nullptr};
    // Not nullptr if block is pre-header of next_loop_
    Loop *next_loop_ {nullptr};

    uint32_t try_id_ {INVALID_ID};
    bool inverted_ {false};

    template <const IterationType T, const IterationDirection D>
    friend class InstIterator;
    template <const IterationType T>
    friend class InstForwardIterator;
    template <const IterationType T>
    friend class InstBackwardIterator;
    template <const IterationType T, const IterationDirection D>
    friend class InstSafeIterator;
};

/*
 * Iterators inheritance-tree
 *
 *  ValueObject
 *   |
 *   |
 *   |----> InstIterator<Type, Direction>  [ add field Inst* current_ ]
 *           |
 *           |
 *           |----> InstForwardIterator<Type, Direction::FORWARD>
 *           |       |
 *           |       |---->   InstForwardIterator<Type::PHI, Direction::FORWARD>
 *           |                [ add field Inst* final_ ]
 *           |
 *           |
 *           |----> InstSafeIterator<Type, Direction> [ add field Inst* follower_ ]
 *                   |
 *                   |---->   InstSafeIterator<Type::PHI, Direction::FORWARD>
 *                   |        [ add field Inst* final_ ]
 *                   |---->   InstSafeIterator<Type::INST, Direction::BACKWARD>
 *                            [ add field Inst* final_ ]
 */

template <const IterationType T, const IterationDirection D>
class InstIterator : public std::iterator<std::forward_iterator_tag, IterationType> {
public:
    const Inst *operator*() const
    {
        return current_;
    }
    Inst *operator*()
    {
        return current_;
    }

    Inst *GetCurrent() const
    {
        return current_;
    }

    void SetCurrent(Inst *current)
    {
        current_ = current;
    }

protected:
    InstIterator(const BasicBlock &block, Inst *start)
    {
#ifndef __clang_analyzer__
        if constexpr (IterationType::INST == T && IterationDirection::FORWARD == D) {
            current_ = (start != nullptr) ? start : block.first_inst_;
            return;
        }
        if constexpr (IterationType::ALL == T && IterationDirection::FORWARD == D) {
            current_ = (block.first_phi_ != nullptr) ? block.first_phi_ : block.first_inst_;
            current_ = (start != nullptr) ? start : current_;
            return;
        }
        if constexpr (IterationType::PHI == T && IterationDirection::BACKWARD == D) {
            current_ = (block.first_inst_ != nullptr) ? block.first_inst_->GetPrev() : block.last_inst_;
            current_ = (start != nullptr) ? start : current_;
            return;
        }
        if constexpr (IterationType::ALL == T && IterationDirection::BACKWARD == D) {
            current_ = (start != nullptr) ? start : block.last_inst_;
            return;
        }
#else
        [[maybe_unused]] auto const &tb = block;
        [[maybe_unused]] auto ts = start;
#endif
        UNREACHABLE();
    }
    explicit InstIterator(Inst *current) : current_(current) {};

    DEFAULT_COPY_SEMANTIC(InstIterator);
    DEFAULT_MOVE_SEMANTIC(InstIterator);
    virtual ~InstIterator() = default;

private:
    Inst *current_ {nullptr};
};

/*
 * Iterates over the instructions in direct order.
 * Starts with the 'start' instruction if it's initialized
 * or with the first phi/non-phi instruction in the basic block
 */
template <const IterationType T>
class InstForwardIterator : public InstIterator<T, IterationDirection::FORWARD> {
public:
    explicit InstForwardIterator(const BasicBlock &block, Inst *start = nullptr)
        : InstIterator<T, IterationDirection::FORWARD>(block, start)
    {
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstForwardIterator begin() const
    {
        return InstForwardIterator(this->GetCurrent());
    }
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstForwardIterator end() const
    {
        return InstForwardIterator(nullptr);
    }
    InstForwardIterator &operator++()
    {
        this->SetCurrent(this->GetCurrent()->GetNext());
        return *this;
    }
    bool operator!=(const InstForwardIterator &other) const
    {
        return this->GetCurrent() != other.GetCurrent();
    }
    bool operator==(const InstForwardIterator &other) const
    {
        return this->GetCurrent() == other.GetCurrent();
    }

protected:
    explicit InstForwardIterator(Inst *current) : InstIterator<T, IterationDirection::FORWARD>(current) {};
};

/*
 * Specialization for PHI instructions
 */
template <>
class InstForwardIterator<IterationType::PHI> : public InstForwardIterator<IterationType::INST> {
public:
    explicit InstForwardIterator(const BasicBlock &block, Inst *start = nullptr)
        : InstForwardIterator<IterationType::INST>(start != nullptr ? start : block.first_phi_)
    {
        final_ = ((block.first_phi_ != nullptr) && (block.first_inst_ != nullptr)) ? block.first_inst_ : nullptr;
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstForwardIterator end() const
    {
        return InstForwardIterator(final_);
    }
    bool operator!=(const InstForwardIterator &other) const
    {
        return this->GetCurrent() != other.GetCurrent();
    }
    bool operator==(const InstForwardIterator &other) const
    {
        return this->GetCurrent() == other.GetCurrent();
    }

private:
    explicit InstForwardIterator(Inst *current) : InstForwardIterator<IterationType::INST>(current) {};

private:
    Inst *final_ {nullptr};
};

/*
 * Iterates over the instructions in reverse order.
 * Starts with the 'start' instruction if it's initialized
 * or with the first phi/non-phi instruction in the basic block
 */
template <const IterationType T>
class InstBackwardIterator : public InstIterator<T, IterationDirection::BACKWARD> {
public:
    explicit InstBackwardIterator(const BasicBlock &block, Inst *start = nullptr)
        : InstIterator<T, IterationDirection::BACKWARD>(block, start)
    {
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstBackwardIterator begin() const
    {
        return InstBackwardIterator(this->GetCurrent());
    }
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstBackwardIterator end() const
    {
        return InstBackwardIterator(nullptr);
    }
    InstBackwardIterator &operator++()
    {
        this->SetCurrent(this->GetCurrent()->GetPrev());
        return *this;
    }
    bool operator!=(const InstBackwardIterator &other) const
    {
        return this->GetCurrent() != other.GetCurrent();
    }
    bool operator==(const InstBackwardIterator &other) const
    {
        return this->GetCurrent() == other.GetCurrent();
    }

protected:
    explicit InstBackwardIterator(Inst *current) : InstIterator<T, IterationDirection::BACKWARD>(current) {}
};

/*
 * Specialization for not-PHI instructions
 */
template <>
class InstBackwardIterator<IterationType::INST> : public InstBackwardIterator<IterationType::ALL> {
public:
    explicit InstBackwardIterator(const BasicBlock &block, Inst *start = nullptr)
        : InstBackwardIterator<IterationType::ALL>(nullptr)
    {
        auto last_inst = (block.first_inst_ != nullptr) ? block.last_inst_ : nullptr;
        this->SetCurrent(start != nullptr ? start : last_inst);
        final_ = (block.first_inst_ != nullptr) ? block.first_inst_->GetPrev() : nullptr;
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstBackwardIterator end() const
    {
        return InstBackwardIterator(final_);
    }

private:
    explicit InstBackwardIterator(Inst *current) : InstBackwardIterator<IterationType::ALL>(current) {}

private:
    Inst *final_ {nullptr};
};

/*
 * Iterates over the instructions in `IterationDirection` order, keeping next instruction in case of removing current
 * instruction
 */
template <const IterationType T, const IterationDirection D>
class InstSafeIterator : public InstIterator<T, D> {
public:
    explicit InstSafeIterator(const BasicBlock &block, Inst *start = nullptr) : InstIterator<T, D>(block, start)
    {
        follower_ = GetFollower();
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstSafeIterator begin() const
    {
        return InstSafeIterator(this->GetCurrent(), follower_);
    }
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstSafeIterator end() const
    {
        return InstSafeIterator(nullptr);
    }
    InstSafeIterator &operator++()
    {
        this->SetCurrent(follower_);
        follower_ = GetFollower();
        return *this;
    }
    bool operator!=(const InstSafeIterator &other) const
    {
        return this->GetCurrent() != other.GetCurrent();
    }
    bool operator==(const InstSafeIterator &other) const
    {
        return this->GetCurrent() == other.GetCurrent();
    }

protected:
    explicit InstSafeIterator(Inst *current) : InstIterator<T, D>(current) {};
    explicit InstSafeIterator(Inst *current, Inst *follower) : InstIterator<T, D>(current), follower_(follower) {};

protected:
    Inst *GetFollower()
    {
#ifndef __clang_analyzer__
        if constexpr (IterationDirection::FORWARD == D) {
            return this->GetCurrent() == nullptr ? nullptr : this->GetCurrent()->GetNext();
        };
        if constexpr (IterationDirection::BACKWARD == D) {
            return this->GetCurrent() == nullptr ? nullptr : this->GetCurrent()->GetPrev();
        };
#endif
        UNREACHABLE();
        return nullptr;
    }

    void SetFollower(Inst *follower)
    {
        follower_ = follower;
    }

private:
    Inst *follower_ {nullptr};
};

/*
 * Specialization for PHI instructions
 */
template <>
class InstSafeIterator<IterationType::PHI, IterationDirection::FORWARD>
    : public InstSafeIterator<IterationType::INST, IterationDirection::FORWARD> {
public:
    explicit InstSafeIterator(const BasicBlock &block, Inst *start = nullptr)
        : InstSafeIterator<IterationType::INST, IterationDirection::FORWARD>(start != nullptr ? start
                                                                                              : block.first_phi_)
    {
        final_ = ((block.first_phi_ != nullptr) && (block.first_inst_ != nullptr)) ? block.first_inst_ : nullptr;
        this->SetFollower(GetFollower());
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstSafeIterator end() const
    {
        return InstSafeIterator(final_);
    }
    bool operator!=(const InstSafeIterator &other) const
    {
        return this->GetCurrent() != other.GetCurrent();
    }
    bool operator==(const InstSafeIterator &other) const
    {
        return this->GetCurrent() == other.GetCurrent();
    }

private:
    explicit InstSafeIterator(Inst *current)
        : InstSafeIterator<IterationType::INST, IterationDirection::FORWARD>(current) {};

private:
    Inst *GetFollower()
    {
        return this->GetCurrent() == final_ ? nullptr : this->GetCurrent()->GetNext();
    }

private:
    Inst *final_ {nullptr};
};

/*
 * Specialization for not-PHI instructions
 */
template <>
class InstSafeIterator<IterationType::INST, IterationDirection::BACKWARD>
    : public InstSafeIterator<IterationType::ALL, IterationDirection::BACKWARD> {
public:
    explicit InstSafeIterator(const BasicBlock &block, Inst *start = nullptr)
        : InstSafeIterator<IterationType::ALL, IterationDirection::BACKWARD>(nullptr)
    {
        auto last_inst = (block.first_inst_ != nullptr) ? block.last_inst_ : nullptr;
        this->SetCurrent(start != nullptr ? start : last_inst);
        final_ = (block.first_inst_ != nullptr) ? block.first_inst_->GetPrev() : nullptr;
        this->SetFollower(GetFollower());
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    InstSafeIterator end() const
    {
        return InstSafeIterator(final_);
    }
    bool operator!=(const InstSafeIterator &other) const
    {
        return this->GetCurrent() != other.GetCurrent();
    }
    bool operator==(const InstSafeIterator &other) const
    {
        return this->GetCurrent() == other.GetCurrent();
    }

private:
    explicit InstSafeIterator(Inst *current)
        : InstSafeIterator<IterationType::ALL, IterationDirection::BACKWARD>(current) {};

private:
    Inst *GetFollower()
    {
        return this->GetCurrent() == final_ ? nullptr : this->GetCurrent()->GetPrev();
    }

private:
    Inst *final_ {nullptr};
};

/**
 * DFS-search to find path between `block` and `target_block`,
 * `exclude_block` can be set to search path without this block
 */
bool BlocksPathDfsSearch(Marker marker, BasicBlock *block, const BasicBlock *target_block,
                         const BasicBlock *exclude_block = nullptr);
}  // namespace panda::compiler

#endif  // COMPILER_OPTIMIZER_IR_BASICBLOCK_H

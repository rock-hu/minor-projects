/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef PANDA_IR_BUILDER_H
#define PANDA_IR_BUILDER_H

#include "bytecode_instruction.h"
#include "optimizer/ir/graph.h"
#include "optimizer/ir/basicblock.h"
#include "optimizer/pass.h"
#include "utils/logger.h"
#include "pbc_iterator.h"
#include "inst_builder.h"

namespace ark {
class Method;
}  // namespace ark

namespace ark::compiler {

struct BlocksConnectorInfo {
    bool fallthrough {};
    bool deadInstructions {};
    BytecodeInstruction prevInst {nullptr};
};

/// Build IR from panda bytecode
class PANDA_PUBLIC_API IrBuilder : public Optimization {
    struct Boundaries {
        uint32_t beginPc;
        uint32_t endPc;
    };

    struct CatchCodeBlock {
        uint32_t pc {};
        uint32_t typeId {};
    };

    struct TryCodeBlock {
        Boundaries boundaries {};                          // NOLINT(misc-non-private-member-variables-in-classes)
        BasicBlock *beginBb {nullptr};                     // NOLINT(misc-non-private-member-variables-in-classes)
        BasicBlock *endBb {nullptr};                       // NOLINT(misc-non-private-member-variables-in-classes)
        ArenaVector<CatchCodeBlock> *catches {nullptr};    // NOLINT(misc-non-private-member-variables-in-classes)
        ArenaVector<BasicBlock *> *basicBlocks {nullptr};  // NOLINT(misc-non-private-member-variables-in-classes)
        uint32_t id {INVALID_ID};                          // NOLINT(misc-non-private-member-variables-in-classes)
        bool containsThrowableInst {false};                // NOLINT(misc-non-private-member-variables-in-classes)
        ArenaSet<BasicBlock *> *throwBlocks {nullptr};     // NOLINT(misc-non-private-member-variables-in-classes)

        void Init(Graph *graph, uint32_t tryId)
        {
            id = tryId;
            auto allocator = graph->GetLocalAllocator();
            catches = allocator->New<ArenaVector<CatchCodeBlock>>(allocator->Adapter());
            beginBb = graph->CreateEmptyBlock(boundaries.beginPc);
            ASSERT(beginBb != nullptr);
            beginBb->SetTryBegin(true);
            endBb = graph->CreateEmptyBlock(boundaries.endPc);
            ASSERT(endBb != nullptr);
            endBb->SetTryEnd(true);
            // Order of try-blocks should be saved in the graph to restore it in the generated bytecode
            graph->AppendTryBeginBlock(beginBb);
        }
    };

public:
    explicit IrBuilder(Graph *graph) : IrBuilder(graph, graph->GetMethod(), nullptr, 0) {}

    IrBuilder(Graph *graph, RuntimeInterface::MethodPtr method, CallInst *callerInst, uint32_t inliningDepth)
        : Optimization(graph),
          blocks_(graph->GetLocalAllocator()->Adapter()),
          catchesPc_(graph->GetLocalAllocator()->Adapter()),
          tryBlocks_(graph->GetLocalAllocator()->Adapter()),
          openedTryBlocks_(graph->GetLocalAllocator()->Adapter()),
          catchHandlers_(graph->GetLocalAllocator()->Adapter()),
          instDefs_(graph->GetLocalAllocator()->Adapter()),
          method_(method),
          isInlinedGraph_(callerInst != nullptr),
          callerInst_(callerInst),
          inliningDepth_(inliningDepth)
    {
    }

    NO_COPY_SEMANTIC(IrBuilder);
    NO_MOVE_SEMANTIC(IrBuilder);
    ~IrBuilder() override = default;

    bool RunImpl() override;

    const char *GetPassName() const override
    {
        return "IrBuilder";
    }

    auto GetMethod() const
    {
        return method_;
    }

private:
    void CreateBlock(size_t pc)
    {
        if (blocks_[pc] == nullptr) {
            blocks_[pc] = GetGraph()->CreateEmptyBlock();
            blocks_[pc]->SetGuestPc(pc);
        }
    }

    BasicBlock *GetBlockForPc(size_t pc)
    {
        return blocks_[pc];
    }

    BasicBlock *GetPrevBlockForPc(size_t pc)
    {
        do {
            ASSERT(pc > 0);
            pc--;
        } while (blocks_[pc] == nullptr || blocks_[pc]->GetGraph() == nullptr);
        return blocks_[pc];
    }

    bool CheckMethodLimitations(const BytecodeInstructions &instructions, size_t vregsCount);
    void BuildBasicBlocks(const BytecodeInstructions &instructions);
    bool CreateSaveStateForLoopBlocks(BasicBlock *bb);
    bool BuildBasicBlock(BasicBlock *bb, const uint8_t *instructionsBuf);
    template <bool NEED_SS_DEOPT>
    bool AddInstructionToBB(BasicBlock *bb, BytecodeInstruction &inst, size_t pc, bool *ssDeoptWasBuilded);
    template <bool NEED_SS_DEOPT>
    bool BuildInstructionsForBB(BasicBlock *bb, const uint8_t *instructionsBuf);
    void SplitConstant(ConstantInst *constInst);
    void ConnectBasicBlocks(const BytecodeInstructions &instructions);
    void CreateTryCatchBoundariesBlocks();
    void ResolveTryCatchBlocks();
    void ConnectTryCatchBlocks();
    IrBuilder::TryCodeBlock *InsertTryBlockInfo(const Boundaries &tryBoundaries);
    void TrackTryBoundaries(size_t pc, const BytecodeInstruction &inst, BasicBlock *targetBb,
                            BlocksConnectorInfo &info);
    BasicBlock *GetBlockToJump(BytecodeInstruction *inst, size_t pc);
    BasicBlock *GetBlockForSaveStateDeoptimize(BasicBlock *block);
    void MarkTryCatchBlocks(Marker marker);
    template <class Callback>
    void EnumerateTryBlocksCoveredPc(uint32_t pc, const Callback &callback);
    void SetMemoryBarrierFlag();
    void ConnectTryCodeBlock(const TryCodeBlock &tryBlock, const ArenaMap<uint32_t, BasicBlock *> &catchBlocks);
    void ProcessThrowableInstructions(Inst *throwableInst);
    void RestoreTryEnd(const TryCodeBlock &tryBlock);
    uint32_t FindCatchBlockInPandaFile(Class *cls, uint32_t pc) const;
    void ConnectThrowBlock(BasicBlock *throwBlock, const TryCodeBlock &tryBlock);
    void ConnectThrowBlocks();
    bool BuildIrImpl(size_t vregsCount);
    bool BuildIr(size_t vregsCount);
    RuntimeInterface::ClassPtr FindExceptionClass(BasicBlock *throwBlock, int32_t *throwPc);
    bool FindAppropriateCatchBlock(const TryCodeBlock &tryBlock, BasicBlock *throwBlock, uint32_t catchPc);
    BasicBlock *FindCatchBegin(BasicBlock *bb);

    void SetInstBuilder(InstBuilder *instBuilder)
    {
        instBuilder_ = instBuilder;
        instBuilder_->Prepare(isInlinedGraph_);
    }
    InstBuilder *GetInstBuilder() const
    {
        return instBuilder_;
    }

private:
    ArenaVector<BasicBlock *> blocks_;
    ArenaSet<uint32_t> catchesPc_;
    ArenaMultiMap<uint32_t, TryCodeBlock> tryBlocks_;
    ArenaList<TryCodeBlock *> openedTryBlocks_;
    ArenaUnorderedSet<BasicBlock *> catchHandlers_;
    InstVector instDefs_;
    RuntimeInterface::MethodPtr method_ = nullptr;
    bool isInlinedGraph_ {false};
    CallInst *callerInst_ {nullptr};
    uint32_t inliningDepth_ {0};
    InstBuilder *instBuilder_ {nullptr};
};

class IrBuilderInliningAnalysis : public Analysis {
public:
    IrBuilderInliningAnalysis(Graph *graph, RuntimeInterface::MethodPtr method) : Analysis(graph), method_(method) {}
    ~IrBuilderInliningAnalysis() override = default;
    NO_COPY_SEMANTIC(IrBuilderInliningAnalysis);
    NO_MOVE_SEMANTIC(IrBuilderInliningAnalysis);

    bool RunImpl() override;

    const char *GetPassName() const override
    {
        return "IrBuilderInlineAnalysis";
    }

    auto GetMethod() const
    {
        return method_;
    }

    auto HasRuntimeCalls() const
    {
        return hasRuntimeCalls_;
    }

private:
    virtual bool IsSuitableForInline(const BytecodeInstruction *inst);

private:
    RuntimeInterface::MethodPtr method_;
    bool hasRuntimeCalls_ {false};
};

class IrBuilderExternalInliningAnalysis : public IrBuilderInliningAnalysis {
public:
    IrBuilderExternalInliningAnalysis(Graph *graph, RuntimeInterface::MethodPtr method)
        : IrBuilderInliningAnalysis(graph, method)
    {
    }

    NO_COPY_SEMANTIC(IrBuilderExternalInliningAnalysis);
    NO_MOVE_SEMANTIC(IrBuilderExternalInliningAnalysis);
    ~IrBuilderExternalInliningAnalysis() override = default;

    const char *GetPassName() const override
    {
        return "IrBuilderExternalInliningAnalysis";
    }

private:
    bool IsSuitableForInline(const BytecodeInstruction *inst) override;
};

}  // namespace ark::compiler

#endif  // PANDA_IR_BUILDER_H

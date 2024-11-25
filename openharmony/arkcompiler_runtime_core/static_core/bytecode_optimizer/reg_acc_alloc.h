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

#ifndef PANDA_REG_ACC_ALLOC_H
#define PANDA_REG_ACC_ALLOC_H

#include "optimizer/ir/graph.h"
#include "optimizer/pass.h"
#include "compiler_options.h"

namespace ark::bytecodeopt {

class PANDA_PUBLIC_API RegAccAlloc : public compiler::Optimization {
    using Optimization::Optimization;

public:
    explicit RegAccAlloc(compiler::Graph *graph) : compiler::Optimization(graph), accMarker_(graph->NewMarker()) {};

    ~RegAccAlloc() override = default;
    NO_COPY_SEMANTIC(RegAccAlloc);
    NO_MOVE_SEMANTIC(RegAccAlloc);

    bool IsEnable() const override
    {
        return compiler::g_options.IsCompilerRegAccAlloc();
    }

    const char *GetPassName() const override
    {
        return "RegAccAlloc";
    }

    bool RunImpl() override;

private:
    bool IsPhiOptimizable(compiler::Inst *phi) const;
    bool IsAccRead(compiler::Inst *inst) const;
    bool IsAccWrite(compiler::Inst *inst) const;

    bool CanUserReadAcc(compiler::Inst *inst, compiler::Inst *user) const;
    bool IsPhiAccReady(compiler::Inst *phi) const;
    void MarkPhiInstructions() const;
    void MarkInstructions();
    void MarkInstruction(compiler::Inst *inst);
    void ClearAccForInstAndUsers(compiler::Inst *inst);
    void SetNeedLda(compiler::Inst *inst, bool need);
    bool CanIntrinsicReadAcc(compiler::IntrinsicInst *inst) const;

    compiler::Marker accMarker_ {0};
};

}  // namespace ark::bytecodeopt

#endif  //  PANDA_REG_ACC_ALLOC_H

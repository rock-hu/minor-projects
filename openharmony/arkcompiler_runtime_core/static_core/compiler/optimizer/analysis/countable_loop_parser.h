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
#ifndef COMPILER_OPTIMIZER_ANALYSIS_COUNTABLE_LOOP_PARSER_H
#define COMPILER_OPTIMIZER_ANALYSIS_COUNTABLE_LOOP_PARSER_H

#include "optimizer/ir/inst.h"

namespace ark::compiler {
class Loop;

/**
 * Example of code
 *  ---------------
 * for (init(a); if_imm(compare(a,test)); update(a)) {...}
 */
struct CountableLoopInfo {
    Inst *ifImm;
    Inst *init;
    Inst *test;
    Inst *update;
    Inst *index;
    uint64_t constStep;
    ConditionCode normalizedCc;  // cc between `update` and `test`
    bool isInc;
};

/// Helper class to check if loop is countable and to get its parameters
class CountableLoopParser {
public:
    explicit CountableLoopParser(const Loop &loop) : loop_(loop) {}

    NO_MOVE_SEMANTIC(CountableLoopParser);
    NO_COPY_SEMANTIC(CountableLoopParser);
    ~CountableLoopParser() = default;

    std::optional<CountableLoopInfo> Parse();
    bool ParseLoopExit();
    static bool HasPreHeaderCompare(Loop *loop, const CountableLoopInfo &loopInfo);
    static std::optional<uint64_t> GetLoopIterations(const CountableLoopInfo &loopInfo);

private:
    bool IsInstIncOrDec(Inst *inst);
    bool SetUpdateAndTestInputs();
    void SetIndexAndConstStep();
    void SetNormalizedConditionCode();
    bool IsConditionCodeAcceptable();
    BasicBlock *FindLoopExitBlock();
    bool CheckParsingLoopCorrectness();
    bool TryProcessBackEdge();

private:
    const Loop &loop_;
    CountableLoopInfo loopInfo_ {};
    bool isHeadLoopExit_ = false;
};
}  // namespace ark::compiler

#endif  // COMPILER_OPTIMIZER_ANALYSIS_COUNTABLE_LOOP_PARSER_H

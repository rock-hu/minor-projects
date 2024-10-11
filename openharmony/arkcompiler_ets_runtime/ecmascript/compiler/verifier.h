/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ECMASCRIPT_COMPILER_VERIFIER_H
#define ECMASCRIPT_COMPILER_VERIFIER_H

#include <algorithm>
#include <deque>
#include <functional>
#include <numeric>
#include <unordered_map>

#include "ecmascript/compiler/circuit.h"

namespace panda::ecmascript::kungfu {
class Verifier {
public:
    static bool RunDataIntegrityCheck(const Circuit *circuit);

    static bool RunStateGatesCheck(const Circuit *circuit, const std::vector<GateRef> &bbGatesList,
                                   const std::string& methodName);

    static bool RunCFGSoundnessCheck(const Circuit *circuit, const std::vector<GateRef> &bbGatesList,
                                     const std::unordered_map<GateRef, size_t> &bbGatesAddrToIdx);

    static bool RunCFGIsDAGCheck(const Circuit *circuit);

    static bool RunCFGReducibilityCheck(const Circuit *circuit, const std::vector<GateRef> &bbGatesList,
                                        const std::unordered_map<GateRef, size_t> &bbGatesAddrToIdx,
                                        const std::function<bool(size_t, size_t)> &isAncestor);

    static bool RunFixedGatesCheck(const Circuit *circuit, const std::vector<GateRef> &fixedGatesList);

    static bool RunFixedGatesRelationsCheck(const Circuit *circuit, const std::vector<GateRef> &fixedGatesList,
                                            const std::unordered_map<GateRef, size_t> &bbGatesAddrToIdx,
                                            const std::function<bool(size_t, size_t)> &isAncestor);

    static bool RunFlowCyclesFind(const Circuit *circuit, std::vector<GateRef> *schedulableGatesListPtr,
                                  const std::vector<GateRef> &bbGatesList,
                                  const std::vector<GateRef> &fixedGatesList);

    static bool RunSchedulableGatesCheck(const Circuit *circuit, const std::vector<GateRef> &schedulableGatesList);

    static bool RunPrologGatesCheck(const Circuit *circuit, const std::vector<GateRef> &schedulableGatesList);

    static bool RunSchedulingBoundsCheck(const Circuit *circuit, const std::vector<GateRef> &schedulableGatesList,
                                         const std::unordered_map<GateRef, size_t> &bbGatesAddrToIdx,
                                         const std::function<bool(size_t, size_t)> &isAncestor,
                                         const std::function<size_t(size_t, size_t)> &lowestCommonAncestor);

    static void FindFixedGates(const Circuit *circuit, const std::vector<GateRef> &bbGatesList,
                               std::vector<GateRef> &fixedGatesList);
    
    static bool RunFlowCyclesFind(const Circuit* circuit);

    static bool Run(const Circuit *circuit, const std::string& methodName = "", bool enableLog = false);
};
}  // namespace panda::ecmascript::kungfu

#endif  // ECMASCRIPT_COMPILER_VERIFIER_H

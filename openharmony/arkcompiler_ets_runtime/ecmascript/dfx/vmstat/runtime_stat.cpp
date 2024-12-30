/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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


#include "ecmascript/runtime_call_id.h"

namespace panda::ecmascript {
// NOLINTNEXTLINE(modernize-avoid-c-arrays)
EcmaRuntimeStat::EcmaRuntimeStat(const char *const runtimeCallerNames[], int count)
{
    for (int i = 0; i < count; i++) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        callerStat_.emplace_back(PandaRuntimeCallerStat(CString(runtimeCallerNames[i])));
    }
}

void EcmaRuntimeStat::StartCount(PandaRuntimeTimer *timer, int callerId)
{
    if (currentTimer_ != nullptr) {
        timer->SetParent(currentTimer_);
    }
    PandaRuntimeTimer *parent = currentTimer_;
    currentTimer_ = timer;
    PandaRuntimeCallerStat *callerStat = &callerStat_[callerId];
    timer->Start(callerStat, parent);
}

void EcmaRuntimeStat::StopCount(const PandaRuntimeTimer *nowTimer)
{
    if (nowTimer != currentTimer_) {
        return;
    }
    PandaRuntimeTimer *parentTimer = currentTimer_->Stop();
    currentTimer_ = parentTimer;
}

void EcmaRuntimeStat::Print() const
{
    if (currentTimer_ != nullptr) {
        currentTimer_->Snapshot();
    }
    PrintAllStats();
}

void EcmaRuntimeStat::ResetAllCount()
{
    while (currentTimer_ != nullptr) {
        StopCount(currentTimer_);
    }
    for (auto &runCallerStat : callerStat_) {
        runCallerStat.Reset();
    }
}

void EcmaRuntimeStat::PrintAllStats() const
{
    LOG_ECMA(INFO) << "panda runtime stat:";
    static constexpr int nameRightAdjustment = 45;
    static constexpr int numberRightAdjustment = 12;
    LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << "InterPreter && GC && C++ Builtin Function"
                       << std::setw(numberRightAdjustment) << "Time(ns)" << std::setw(numberRightAdjustment) << "Count"
                       << std::setw(numberRightAdjustment) << "MaxTime(ns)"
                       << std::setw(numberRightAdjustment) << "AvgTime(ns)";
    LOG_ECMA(INFO) << "============================================================"
                       << "=========================================================";

    CVector<PandaRuntimeCallerStat> callerStat;
    for (auto &runCallerStat : callerStat_) {
        callerStat.push_back(runCallerStat);
    }
    // Sort by TotalTime
    std::sort(callerStat.begin(), callerStat.end(),
        [](const PandaRuntimeCallerStat &a, const PandaRuntimeCallerStat &b) -> bool {
            return a.TotalTime() > b.TotalTime();
    });

    uint64_t totalTime = 0;
    uint64_t interpreterTotalTime = 0;
    uint64_t builtinTotalTime = 0;
    uint64_t abstractOperationTotalTime = 0;
    uint64_t memoryTotalTime = 0;
    uint64_t runtimeTotalTime = 0;
    for (auto &runCallerStat : callerStat) {
        if (runCallerStat.TotalCount() != 0) {
            totalTime += runCallerStat.TotalTime();
            CString header = runCallerStat.GetHeaderOfName();
            if (header == CString("Interpreter")) {
                interpreterTotalTime += runCallerStat.TotalTime();
            } else if (header == CString("BuiltinsApi")) {
                builtinTotalTime += runCallerStat.TotalTime();
            } else if (header == CString("AbstractOperation")) {
                abstractOperationTotalTime += runCallerStat.TotalTime();
            } else if (header == CString("Memory")) {
                memoryTotalTime += runCallerStat.TotalTime();
            } else if (header == CString("Runtime")) {
                runtimeTotalTime += runCallerStat.TotalTime();
            }
            LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << runCallerStat.Name()
                               << std::setw(numberRightAdjustment) << runCallerStat.TotalTime()
                               << std::setw(numberRightAdjustment) << runCallerStat.TotalCount()
                               << std::setw(numberRightAdjustment) << runCallerStat.MaxTime()
                               << std::setw(numberRightAdjustment)
                               << runCallerStat.TotalTime() / runCallerStat.TotalCount();
        }
    }
    LOG_ECMA(INFO) << "------------------------------------------------------------"
                       << "---------------------------------------------------------";
    LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << "Interpreter Total Time(ns)"
                       << std::setw(numberRightAdjustment) << interpreterTotalTime;
    LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << "BuiltinsApi Total Time(ns)"
                       << std::setw(numberRightAdjustment) << builtinTotalTime;
    LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << "AbstractOperation Total Time(ns)"
                       << std::setw(numberRightAdjustment) << abstractOperationTotalTime;
    LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << "Memory Total Time(ns)"
                       << std::setw(numberRightAdjustment) << memoryTotalTime;
    LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << "Runtime Total Time(ns)"
                       << std::setw(numberRightAdjustment) << runtimeTotalTime;
    LOG_ECMA(INFO) << "------------------------------------------------------------"
                       << "---------------------------------------------------------";
    LOG_ECMA(INFO) << std::right << std::setw(nameRightAdjustment) << "Total Time(ns)"
                       << std::setw(numberRightAdjustment) << totalTime;
}

EcmaRuntimeStatScope::EcmaRuntimeStatScope(EcmaVM *vm) : vm_(vm)
{
    if (vm_->GetJSOptions().IsEnableRuntimeStat()) {
        vm_->GetJSThread()->GetCurrentEcmaContext()->SetRuntimeStatEnable(true);
    }
}

EcmaRuntimeStatScope::~EcmaRuntimeStatScope()
{
    if (vm_->GetJSOptions().IsEnableRuntimeStat()) {
        vm_->GetJSThread()->GetCurrentEcmaContext()->SetRuntimeStatEnable(false);
    }
    vm_ = nullptr;
}
}  // namespace panda::ecmascript

/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ecmascript/jobs/hitrace_scope.h"

#include "ecmascript/jobs/pending_job.h"
#include "hitrace/trace.h"
#include "ecmascript/dfx/stackinfo/js_stackinfo.h"

namespace panda::ecmascript::job {
EnqueueJobScope::EnqueueJobScope(const JSHandle<PendingJob> &pendingJob, QueueType queueType)
{
    HiTraceId id = HiTraceChain::GetId();
    if (id.IsValid() && id.IsFlagEnabled(HITRACE_FLAG_INCLUDE_ASYNC)) {
        HiTraceId childId = HiTraceChain::CreateSpan();

        pendingJob->SetChainId(childId.GetChainId());
        pendingJob->SetSpanId(childId.GetSpanId());
        pendingJob->SetParentSpanId(childId.GetParentSpanId());
        pendingJob->SetFlags(childId.GetFlags());

        if (id.IsFlagEnabled(HITRACE_FLAG_TP_INFO)) {
            if (queueType == QueueType::QUEUE_PROMISE) {
                HiTraceChain::Tracepoint(HITRACE_CM_THREAD, HITRACE_TP_CS,
                    childId, "Queue type:%s", "Promise queue");
            } else if (queueType == QueueType::QUEUE_SCRIPT) {
                HiTraceChain::Tracepoint(HITRACE_CM_THREAD, HITRACE_TP_CS,
                    childId, "Queue type:%s", "Script queue");
            } else {
                HiTraceChain::Tracepoint(HITRACE_CM_THREAD, HITRACE_TP_CS,
                    childId, "Queue type:%s", "Other queue");
            }
        }
    }
}

EnqueueJobScope::~EnqueueJobScope()
{
}

ExecuteJobScope::ExecuteJobScope(const JSHandle<PendingJob> &pendingJob)
{
    saveId_ = HiTraceChain::GetId();
    if (saveId_.IsValid()) {
        HiTraceChain::ClearId();
    }
    if (pendingJob->GetChainId() != 0) {
        hitraceId_.SetChainId(pendingJob->GetChainId());
        hitraceId_.SetSpanId(pendingJob->GetSpanId());
        hitraceId_.SetParentSpanId(pendingJob->GetParentSpanId());
        hitraceId_.SetFlags(pendingJob->GetFlags());

        if (hitraceId_.IsValid()) {
            HiTraceChain::SetId(hitraceId_);
            if (hitraceId_.IsFlagEnabled(HITRACE_FLAG_TP_INFO)) {
                HiTraceChain::Tracepoint(HITRACE_CM_THREAD, HITRACE_TP_SR,
                    hitraceId_, "Before %s pending job execute", "Promise");
            }
        }
    }
}

ExecuteJobScope::~ExecuteJobScope()
{
    if (hitraceId_.IsValid()) {
        if (hitraceId_.IsFlagEnabled(HITRACE_FLAG_TP_INFO)) {
            HiTraceChain::Tracepoint(HITRACE_CM_THREAD, HITRACE_TP_SS,
                hitraceId_, "After %s pending job execute", "Promise");
        }
        HiTraceChain::ClearId();
    }
    if (saveId_.IsValid()) {
        HiTraceChain::SetId(saveId_);
    }
}

EnqueueJobTrace::EnqueueJobTrace(JSThread *thread, const JSHandle<PendingJob> &pendingJob)
{
    isMicroJobTraceEnable_ = thread->GetEcmaVM()->GetJSOptions().EnableMicroJobTrace();
    if (!isMicroJobTraceEnable_) {
        return;
    }

    uint64_t jobId = thread->GetJobId();
    pendingJob->SetJobId(jobId);
    std::string strTrace = "MicroJobQueue::EnqueueJob: jobId: " + std::to_string(jobId);
    strTrace += ", threadId: " + std::to_string(thread->GetThreadId());

    std::vector<JsFrameInfo> jsStackInfo = JsStackInfo::BuildJsStackInfo(thread, true);
    if (jsStackInfo.empty()) {
        ECMA_BYTRACE_START_TRACE(HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_ARK, strTrace.c_str());
        return;
    }

    JsFrameInfo jsFrameInfo = jsStackInfo.front();
    size_t pos = jsFrameInfo.pos.find(':', 0);
    if (pos != CString::npos) {
        int64_t tmp = std::strtoll(jsFrameInfo.pos.substr(0, pos).c_str(), nullptr, 10);
        if (tmp < INT_MIN || tmp > INT_MAX) {
            LOG_ECMA(ERROR) << "MicroJobTrace: Invalid line number: " << jsFrameInfo.pos.substr(0, pos);
            return;
        }

        int64_t val = std::strtoll(jsFrameInfo.pos.substr(pos + 1).c_str(), nullptr, 10);
        if (val < INT_MIN || val > INT_MAX) {
            LOG_ECMA(ERROR) << "MicroJobTrace: Invalid column number: " << jsFrameInfo.pos.substr(pos + 1);
            return;
        }
        int lineNumber = static_cast<int>(tmp);
        int columnNumber = static_cast<int>(val);
        auto sourceMapcb = thread->GetEcmaVM()->GetSourceMapTranslateCallback();
        if (sourceMapcb != nullptr && !jsFrameInfo.fileName.empty()) {
            sourceMapcb(jsFrameInfo.fileName, lineNumber, columnNumber, jsFrameInfo.packageName);
        }
    }

    strTrace += ", funcName: " + jsFrameInfo.functionName + ", url: " + jsFrameInfo.fileName + ":" + jsFrameInfo.pos;
    ECMA_BYTRACE_START_TRACE(HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_ARK, strTrace.c_str());
}

EnqueueJobTrace::~EnqueueJobTrace()
{
    if (isMicroJobTraceEnable_) {
        ECMA_BYTRACE_FINISH_TRACE(HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_ARK);
    }
}

ExecuteJobTrace::ExecuteJobTrace(JSThread *thread, const JSHandle<PendingJob> &pendingJob)
{
    isMicroJobTraceEnable_ = thread->GetEcmaVM()->GetJSOptions().EnableMicroJobTrace();
    if (isMicroJobTraceEnable_) {
        uint64_t jobId = pendingJob->GetJobId();
        std::string strTrace = "PendingJob::ExecutePendingJob: jobId: " + std::to_string(jobId);
        strTrace += ", threadId: " + std::to_string(thread->GetThreadId());
        ECMA_BYTRACE_START_TRACE(HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_ARK, strTrace.c_str());
    }
}

ExecuteJobTrace::~ExecuteJobTrace()
{
    if (isMicroJobTraceEnable_) {
        ECMA_BYTRACE_FINISH_TRACE(HITRACE_LEVEL_COMMERCIAL, HITRACE_TAG_ARK);
    }
}
}  // namespace panda::ecmascript::job
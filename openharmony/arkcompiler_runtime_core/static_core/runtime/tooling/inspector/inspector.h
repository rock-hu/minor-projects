/**
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_TOOLING_INSPECTOR_INSPECTOR_H
#define PANDA_TOOLING_INSPECTOR_INSPECTOR_H

#include "debug_info_cache.h"
#include "inspector_server.h"
#include "debuggable_thread.h"
#include "types/numeric_id.h"

#include "tooling/debug_interface.h"
#include "tooling/inspector/object_repository.h"
#include "tooling/inspector/types/pause_on_exceptions_state.h"
#include "tooling/inspector/types/property_descriptor.h"
#include "tooling/inspector/types/remote_object.h"
#include "tooling/pt_thread.h"

#include <array>
#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <string_view>
#include <thread>
#include <vector>

namespace ark::tooling {
class DebugInterface;

namespace inspector {
// NOLINTNEXTLINE(fuchsia-virtual-inheritance)
class Server;

class Inspector final : public PtHooks {
public:
    Inspector(Server &server, DebugInterface &debugger, bool breakOnStart);
    ~Inspector() override;

    NO_COPY_SEMANTIC(Inspector);
    NO_MOVE_SEMANTIC(Inspector);

    void ConsoleCall(PtThread thread, ConsoleCallType type, uint64_t timestamp,
                     const PandaVector<TypedValue> &arguments) override;
    // CC-OFFNXT(G.FUN.01-CPP) Decreasing the number of arguments will decrease the clarity of the code.
    void Exception(PtThread thread, Method *method, const PtLocation &location, ObjectHeader *exception,
                   Method *catchMethod, const PtLocation &catchLocation) override;
    void FramePop(PtThread thread, Method *method, bool wasPoppedByException) override;
    void MethodEntry(PtThread thread, Method *method) override;
    void LoadModule(std::string_view fileName) override;
    void SingleStep(PtThread thread, Method *method, const PtLocation &location) override;
    void ThreadStart(PtThread thread) override;
    void ThreadEnd(PtThread thread) override;
    void VmDeath() override;

private:
    void RuntimeEnable(PtThread thread);

    void RunIfWaitingForDebugger(PtThread thread);

    void Pause(PtThread thread);
    void Continue(PtThread thread);

    void SetBreakpointsActive(PtThread thread, bool active);
    std::set<size_t> GetPossibleBreakpoints(std::string_view sourceFile, size_t startLine, size_t endLine,
                                            bool restrictToFunction);
    std::optional<BreakpointId> SetBreakpoint(PtThread thread,
                                              const std::function<bool(std::string_view)> &sourceFilesFilter,
                                              size_t lineNumber, std::set<std::string_view> &sourceFiles);
    void RemoveBreakpoint(PtThread thread, BreakpointId id);

    void SetPauseOnExceptions(PtThread thread, PauseOnExceptionsState state);

    void StepInto(PtThread thread);
    void StepOver(PtThread thread);
    void StepOut(PtThread thread);
    void ContinueToLocation(PtThread thread, std::string_view sourceFile, size_t lineNumber);

    void RestartFrame(PtThread thread, FrameId frameId);

    std::vector<PropertyDescriptor> GetProperties(PtThread thread, RemoteObjectId objectId, bool generatePreview);
    std::string GetSourceCode(std::string_view sourceFile);

    void DebuggableThreadPostSuspend(PtThread thread, ObjectRepository &objectRepository,
                                     const std::vector<BreakpointId> &hitBreakpoints, ObjectHeader *exception);

    void NotifyExecutionEnded();

    EvaluationResult Evaluate(PtThread thread, const std::string &bcFragment);

    ALWAYS_INLINE bool CheckVmDead() REQUIRES_SHARED(vmDeathLock_)
    {
        if (UNLIKELY(isVmDead_)) {
            LOG(WARNING, DEBUGGER) << "Killing inspector server after VM death";
            inspectorServer_.Kill();
            return true;
        }
        return false;
    }

    /// @brief Get verbose information about the raised exception.
    std::optional<ExceptionDetails> CreateExceptionDetails(PtThread thread, RemoteObject &&exception);

    size_t GetNewExceptionId();

private:
    bool breakOnStart_;

    os::memory::RWLock debuggerEventsLock_;
    bool connecting_ {false};  // Should be accessed only from the server thread

    InspectorServer inspectorServer_;  // NOLINT(misc-non-private-member-variables-in-classes)
    DebugInterface &debugger_;
    DebugInfoCache debugInfoCache_;
    std::map<PtThread, DebuggableThread> threads_;

    std::atomic_size_t currentExceptionId_ {0};

    os::memory::RWLock vmDeathLock_;
    bool isVmDead_ GUARDED_BY(vmDeathLock_) {false};

    std::thread serverThread_;
};
}  // namespace inspector
}  // namespace ark::tooling

#endif  // PANDA_TOOLING_INSPECTOR_INSPECTOR_H

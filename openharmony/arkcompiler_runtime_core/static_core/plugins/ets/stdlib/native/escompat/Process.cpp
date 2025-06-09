/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <cmath>
#include <thread>
#include <set>

#include "libpandabase/macros.h"
#include "libpandabase/os/exec.h"
#include "libpandabase/os/filesystem.h"
#include "libpandabase/os/kill.h"
#include "libpandabase/os/pipe.h"
#include "libpandabase/os/system_environment.h"
#include "libpandabase/os/time.h"
#include "plugins/ets/stdlib/native/escompat/Process.h"
#include "plugins/ets/stdlib/native/core/stdlib_ani_helpers.h"

namespace ark::ets::stdlib {

enum Signals : uint32_t { SIG_INT = 2, SIG_QUIT = 3, SIG_KILL = 9, SIG_TERM = 15 };

static auto GetPipeHandler(int stdOutFdIn, int stdOutFdOut, int stdErrFdIn, int stdErrFdOut)
{
    auto handlePipes = [stdOutFdIn, stdOutFdOut, stdErrFdIn, stdErrFdOut] {
        std::pair<ark::os::unique_fd::UniqueFd, ark::os::unique_fd::UniqueFd> out {stdOutFdIn, stdOutFdOut};
        std::pair<ark::os::unique_fd::UniqueFd, ark::os::unique_fd::UniqueFd> err {stdErrFdIn, stdErrFdOut};
        out.first.Reset();
        err.first.Reset();
        ark::os::unique_fd::UniqueFd defaultStdOut {1};
        ark::os::unique_fd::UniqueFd defaultStdErr {2};
        os::Dup2(out.second, defaultStdOut);
        os::Dup2(err.second, defaultStdErr);
        out.second.Release();
        err.second.Release();
        defaultStdOut.Release();
        defaultStdErr.Release();
    };
    return handlePipes;
}

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
os::memory::Mutex g_terminatedChildSetLock {};
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
static std::set<ani_double> g_terminatedChildSet GUARDED_BY(g_terminatedChildSetLock) {};

static void SpawnChildProcess(ani_env *env, ani_object child, ani_string cmd, ani_int timeout, ani_int signal)
{
    auto stdOutFd = os::CreatePipe();
    if (!stdOutFd.first.IsValid()) {
        ThrowNewError(env, "Lstd/core/RuntimeException;", "Failed to create a child process", "Lstd/core/String;:V");
        return;
    }
    auto stdErrFd = os::CreatePipe();
    if (!stdErrFd.first.IsValid()) {
        ThrowNewError(env, "Lstd/core/RuntimeException;", "Failed to create a child process", "Lstd/core/String;:V");
        return;
    }

    auto pipeHandler =
        GetPipeHandler(stdOutFd.first.Get(), stdOutFd.second.Get(), stdErrFd.first.Get(), stdErrFd.second.Get());

    auto cmdString = ConvertFromAniString(env, cmd);
    auto result = ark::os::exec::ExecWithCallbackNoWait(pipeHandler, "/bin/sh", "-c", cmdString.c_str());

    stdOutFd.second.Reset();
    stdErrFd.second.Reset();

    if (result.HasValue()) {
        ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Double(child, "pid", result.Value()));
    } else {
        stdOutFd.first.Reset();
        stdErrFd.first.Reset();
        ThrowNewError(env, "Lstd/core/RuntimeException;", "Failed to create a child process", "Lstd/core/String;:V");
        return;
    }

    ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Double(child, "ppid", ark::os::thread::GetPid()));
    ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Int(child, "outFd", stdOutFd.first.Release()));
    ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Int(child, "errorFd", stdErrFd.first.Release()));

    if (timeout == 0) {
        return;
    }

    ani_double pidToTerminate;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Double(child, "pid", &pidToTerminate));

    auto terminator = [timeout, signal, pidToTerminate] {
        ark::os::thread::NativeSleep(timeout);
        os::memory::LockHolder lock {g_terminatedChildSetLock};
        if (g_terminatedChildSet.count(pidToTerminate) == 0 && kill(pidToTerminate, signal) == 0) {
            auto signalType = std::array {SIG_INT, SIG_QUIT, SIG_KILL, SIG_TERM};
            if (std::find(signalType.begin(), signalType.end(), signal) != signalType.end()) {
                g_terminatedChildSet.insert(pidToTerminate);
            }
        }
    };

    std::thread timeoutListener {terminator};
    timeoutListener.detach();
}

static void ReadFinalizer(ani_env *env, ani_object child, bool isStdErr)
{
    bool terminated = false;

    ani_double exitCode;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Double(child, "exitCode", &exitCode));

    if (exitCode > -1) {
        terminated = true;
    } else {
        os::memory::LockHolder lock {g_terminatedChildSetLock};
        ani_double pid;
        ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Double(child, "pid", &pid));
        terminated = g_terminatedChildSet.count(pid) != 0U;
    }

    if (terminated) {
        std::string fdName = isStdErr ? "errorFd" : "outFd";
        ani_int fd;
        ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Int(child, fdName.data(), &fd));
        ark::os::unique_fd::UniqueFd out {fd};
        out.Reset();
        ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Int(child, fdName.data(), -1));
    }
}

static void ReadChildProcessStdOut(ani_env *env, ani_object child)
{
    ani_int fd;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Int(child, "outFd", &fd));
    if (fd == -1) {
        return;
    }

    ani_ref bufferObject;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Ref(child, "outBuffer", &bufferObject));

    ani_ref outBuffer;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Ref(reinterpret_cast<ani_object>(bufferObject), "data", &outBuffer));

    while (true) {
        ani_int outBytesRead;
        ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Int(child, "outBytesRead", &outBytesRead));

        ani_int outBufferLength;
        ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Int(reinterpret_cast<ani_object>(bufferObject), "_byteLength",
                                                          &outBufferLength));

        if (outBytesRead >= outBufferLength) {
            break;
        }

        constexpr int MAX_SIZE = 1024;
        std::array<int8_t, MAX_SIZE> buffer {};
        int bytesRead = read(fd, buffer.data(), sizeof(buffer.size()) - 1);
        if (bytesRead == 0) {
            ReadFinalizer(env, child, false);
            break;
        }

        if (bytesRead + outBytesRead > outBufferLength) {
            bytesRead = outBufferLength - outBytesRead;
            ark::os::unique_fd::UniqueFd out {fd};
            out.Reset();
            ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Int(child, "outFd", -1));
        }

        ANI_FATAL_IF_ERROR(env->Array_SetRegion_Byte(reinterpret_cast<ani_array_byte>(outBuffer), outBytesRead,
                                                     bytesRead, buffer.data()));
        ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Int(child, "outBytesRead", outBytesRead + bytesRead));
    }
}

static void ReadChildProcessStdErr(ani_env *env, ani_object child)
{
    ani_int fd;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Int(child, "errorFd", &fd));
    if (fd == -1) {
        return;
    }

    ani_ref bufferObject;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Ref(child, "errBuffer", &bufferObject));

    ani_ref errBuffer;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Ref(reinterpret_cast<ani_object>(bufferObject), "data", &errBuffer));

    ani_type childClassType;
    ANI_FATAL_IF_ERROR(env->Object_GetType(child, &childClassType));

    ani_field errBytesReadId;
    ANI_FATAL_IF_ERROR(env->Class_FindField(static_cast<ani_class>(childClassType), "errBytesRead", &errBytesReadId));

    while (true) {
        ani_int errBytesRead;
        ANI_FATAL_IF_ERROR(env->Object_GetField_Int(child, errBytesReadId, &errBytesRead));

        ani_int errBufferLength;
        ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Int(reinterpret_cast<ani_object>(bufferObject), "_byteLength",
                                                          &errBufferLength));

        if (errBytesRead >= errBufferLength) {
            break;
        }

        constexpr int MAX_SIZE = 1024;
        std::array<int8_t, MAX_SIZE> buffer {};
        int bytesRead = read(fd, buffer.data(), sizeof(buffer.size()) - 1);
        if (bytesRead == 0) {
            ReadFinalizer(env, child, true);
            break;
        }

        if (bytesRead + errBytesRead > errBufferLength) {
            bytesRead = errBufferLength - errBytesRead;

            ark::os::unique_fd::UniqueFd out {fd};
            out.Reset();
            ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Int(child, "errorFd", -1));
        }

        ANI_FATAL_IF_ERROR(env->Array_SetRegion_Byte(reinterpret_cast<ani_array_byte>(errBuffer), errBytesRead,
                                                     bytesRead, buffer.data()));
        ANI_FATAL_IF_ERROR(env->Object_SetField_Int(child, errBytesReadId, errBytesRead + bytesRead));
    }
}

static void WaitChildProcess(ani_env *env, ani_object child)
{
    ani_double pid;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Double(child, "pid", &pid));

    ani_type childClassType;
    ANI_FATAL_IF_ERROR(env->Object_GetType(child, &childClassType));

    ani_field exitCodeId;
    ANI_FATAL_IF_ERROR(env->Class_FindField(static_cast<ani_class>(childClassType), "exitCode", &exitCodeId));

    ani_double exitCode;
    ANI_FATAL_IF_ERROR(env->Object_GetField_Double(child, exitCodeId, &exitCode));

    if (exitCode < 0) {
        auto result = ark::os::exec::Wait(pid, false);
        if (result.HasValue()) {
            ANI_FATAL_IF_ERROR(env->Object_SetField_Double(child, exitCodeId, result.Value()));
        } else {
            ThrowNewError(env, "Lstd/core/RuntimeException;", "Wait failed", "Lstd/core/String;:V");
            return;
        }
    }

    ReadChildProcessStdOut(env, child);
    ReadChildProcessStdErr(env, child);

    {
        os::memory::LockHolder lock {g_terminatedChildSetLock};
        g_terminatedChildSet.erase(pid);
    }
}

static void KillChildProcess(ani_env *env, ani_object child, ani_int signal)
{
    auto signalType = std::array {SIG_INT, SIG_QUIT, SIG_KILL, SIG_TERM};

    auto intSignal = static_cast<int>(signal);

    ani_double pid;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Double(child, "pid", &pid));

    if (ark::os::kill_process::Kill(pid, intSignal) == 0) {
        if (std::find(signalType.begin(), signalType.end(), intSignal) != signalType.end()) {
            os::memory::LockHolder lock {g_terminatedChildSetLock};
            g_terminatedChildSet.insert(pid);
        }
        ANI_FATAL_IF_ERROR(env->Object_SetFieldByName_Boolean(child, "killed", 1U));
        return;
    }

    ThrowNewError(env, "Lstd/core/RuntimeException;", "Kill failed", "Lstd/core/String;:V");
}

static void CloseChildProcess(ani_env *env, ani_object child)
{
    ani_type childClassType;
    ANI_FATAL_IF_ERROR(env->Object_GetType(child, &childClassType));

    ani_field exitCodeId;
    ANI_FATAL_IF_ERROR(env->Class_FindField(static_cast<ani_class>(childClassType), "exitCode", &exitCodeId));

    ani_double pid;
    ANI_FATAL_IF_ERROR(env->Object_GetFieldByName_Double(child, "pid", &pid));

    ani_double exitCode;
    ANI_FATAL_IF_ERROR(env->Object_GetField_Double(child, exitCodeId, &exitCode));

    if (exitCode > -1) {
        return;
    }

    {
        os::memory::LockHolder lock {g_terminatedChildSetLock};

        if (g_terminatedChildSet.count(pid) != 0U) {
            return;
        }
    }

    auto status = ark::os::kill_process::Close(pid);
    if (LIKELY(status != -1)) {
        ANI_FATAL_IF_ERROR(env->Object_SetField_Double(child, exitCodeId, status));
        return;
    }

    ThrowNewError(env, "Lstd/core/RuntimeException;", "Close failed", "Lstd/core/String;:V");
}

static ani_double PManagerGetUidForName(ani_env *env, [[maybe_unused]] ani_object process, ani_string name)
{
    auto str = ConvertFromAniString(env, name);
    auto result = ark::os::system_environment::GetUidForName(str);
    return result;
}

static ani_double PManagerGetThreadPriority([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process,
                                            ani_double tid)
{
    return ark::os::thread::GetPriority(tid);
}

static ani_double PManagerGetSystemConfig([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process,
                                          ani_double name)
{
    return ark::os::system_environment::GetSystemConfig(name);
}

static ani_string PManagerGetEnvironmentVar(ani_env *env, [[maybe_unused]] ani_object process, ani_string name)
{
    auto str = ConvertFromAniString(env, name);
    auto envVar = ark::os::system_environment::GetEnvironmentVar(str.c_str());
    return CreateUtf8String(env, envVar.data(), envVar.size());
}

static void PManagerExit([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process, ani_double code)
{
    std::exit(code);
}

static ani_boolean PManagerKill(ani_env *env, [[maybe_unused]] ani_object process, ani_double signal, ani_double pid)
{
    int integerPid = static_cast<int>(pid);
    auto ownPid = ark::os::thread::GetPid();
    if (integerPid == 0 || integerPid == -1 || integerPid == ownPid || integerPid == -ownPid) {
        ThrowNewError(env, "Lstd/core/IllegalArgumentException;", "Invalid pid argument", "Lstd/core/String;:V");
        return 0U;
    }

    constexpr int MIN_SIGNAL_VALUE = 1;
    constexpr int MAX_SINAL_VALUE = 64;

    if (std::trunc(signal) != signal || signal < MIN_SIGNAL_VALUE || signal > MAX_SINAL_VALUE) {
        ThrowNewError(env, "Lstd/core/IllegalArgumentException;", "Invalid signal argument", "Lstd/core/String;:V");
        return 0U;
    }

    return static_cast<ani_boolean>(ark::os::kill_process::Kill(integerPid, signal) == 0);
}

static ani_double GetTid([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::thread::GetCurrentThreadId();
}

static ani_double GetPid([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::thread::GetPid();
}

static ani_double GetPPid([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::thread::GetPPid();
}

static ani_double GetUid([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::thread::GetUid();
}

static ani_double GetEuid([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::thread::GetEuid();
}

static ani_double GetGid([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::thread::GetGid();
}

static ani_double GetEgid([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::thread::GetEgid();
}

static ani_array_double GetGroupIDs(ani_env *env, [[maybe_unused]] ani_object process)
{
    auto groups = ark::os::thread::GetGroups();
    auto groupIds = std::vector<ani_double>(groups.begin(), groups.end());

    ani_array_double result;
    ANI_FATAL_IF_ERROR(env->Array_New_Double(groups.size(), &result));

    if (groups.empty()) {
        ThrowNewError(env, "Lstd/core/RuntimeException;", "Failed to get process groups", "Lstd/core/String;:V");
        return result;
    }

    ANI_FATAL_IF_ERROR(env->Array_SetRegion_Double(result, 0, groups.size(), groupIds.data()));

    return result;
}

static ani_boolean Is64BitProcess([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    constexpr int SIZE_OF_64_BIT_PTR = 8;
    return static_cast<ani_boolean>(sizeof(char *) == SIZE_OF_64_BIT_PTR);
}

static ani_double GetProcessStartRealTime([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    return ark::os::time::GetStartRealTime<std::chrono::milliseconds>();
}

static ani_double GetProcessPastCpuTime([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    constexpr int PROCESS_CLOCK = 2;
    return ark::os::time::GetClockTime<std::chrono::milliseconds>(PROCESS_CLOCK);
}

static void AbortProcess([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    std::abort();
}

static ani_string GetCurrentWorkingDirectory(ani_env *env, [[maybe_unused]] ani_object process)
{
    auto workDir = ark::os::GetCurrentWorkingDirectory();
    return CreateUtf8String(env, workDir.data(), workDir.size());
}

static void ChangeCurrentWorkingDirectory(ani_env *env, [[maybe_unused]] ani_object process, ani_string path)
{
    auto str = ConvertFromAniString(env, path);
    os::ChangeCurrentWorkingDirectory(str);
}

static ani_double GetSystemUptime([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object process)
{
    constexpr int BOOTTIME_CLOCK = 7;
    return ark::os::time::GetClockTime<std::chrono::milliseconds>(BOOTTIME_CLOCK);
}

void RegisterProcessNativeMethods(ani_env *env)
{
    const auto childProcessImpls =
        std::array {ani_native_function {"readOutput", ":V", reinterpret_cast<void *>(ReadChildProcessStdOut)},
                    ani_native_function {"readErrorOutput", ":V", reinterpret_cast<void *>(ReadChildProcessStdErr)},
                    ani_native_function {"close", ":V", reinterpret_cast<void *>(CloseChildProcess)},
                    ani_native_function {"killImpl", "I:V", reinterpret_cast<void *>(KillChildProcess)},
                    ani_native_function {"spawn", "Lstd/core/String;II:V", reinterpret_cast<void *>(SpawnChildProcess)},
                    ani_native_function {"waitImpl", ":D", reinterpret_cast<void *>(WaitChildProcess)}};

    const auto processManagerImpls = std::array {
        ani_native_function {"getUidForName", "Lstd/core/String;:D", reinterpret_cast<void *>(PManagerGetUidForName)},
        ani_native_function {"getThreadPriority", "D:D", reinterpret_cast<void *>(PManagerGetThreadPriority)},
        ani_native_function {"getSystemConfig", "D:D", reinterpret_cast<void *>(PManagerGetSystemConfig)},
        ani_native_function {"getEnvironmentVar", "Lstd/core/String;:Lstd/core/String;",
                             reinterpret_cast<void *>(PManagerGetEnvironmentVar)},
        ani_native_function {"exit", "D:V", reinterpret_cast<void *>(PManagerExit)},
        ani_native_function {"kill", "DD:Z", reinterpret_cast<void *>(PManagerKill)},
    };

    const auto processImpls = std::array {
        ani_native_function {"getTidImpl", ":D", reinterpret_cast<void *>(GetTid)},
        ani_native_function {"getPidImpl", ":D", reinterpret_cast<void *>(GetPid)},
        ani_native_function {"getPpidImpl", ":D", reinterpret_cast<void *>(GetPPid)},
        ani_native_function {"getUidImpl", ":D", reinterpret_cast<void *>(GetUid)},
        ani_native_function {"getEuidImpl", ":D", reinterpret_cast<void *>(GetEuid)},
        ani_native_function {"getGidImpl", ":D", reinterpret_cast<void *>(GetGid)},
        ani_native_function {"getEgidImpl", ":D", reinterpret_cast<void *>(GetEgid)},
        ani_native_function {"getGroupsImpl", ":[D", reinterpret_cast<void *>(GetGroupIDs)},
        ani_native_function {"is64Bit", ":Z", reinterpret_cast<void *>(Is64BitProcess)},
        ani_native_function {"getStartRealtime", ":D", reinterpret_cast<void *>(GetProcessStartRealTime)},
        ani_native_function {"getPastCpuTime", ":D", reinterpret_cast<void *>(GetProcessPastCpuTime)},
        ani_native_function {"abort", ":V", reinterpret_cast<void *>(AbortProcess)},
        ani_native_function {"cwd", ":Lstd/core/String;", reinterpret_cast<void *>(GetCurrentWorkingDirectory)},
        ani_native_function {"chdir", "Lstd/core/String;:V", reinterpret_cast<void *>(ChangeCurrentWorkingDirectory)},
        ani_native_function {"uptime", ":D", reinterpret_cast<void *>(GetSystemUptime)}};

    ani_class childProcessKlass;
    ANI_FATAL_IF_ERROR(env->FindClass("Lescompat/StdProcess/ChildProcess;", &childProcessKlass));
    ANI_FATAL_IF_ERROR(
        env->Class_BindNativeMethods(childProcessKlass, childProcessImpls.data(), childProcessImpls.size()));

    ani_class processManagerKlass;
    ANI_FATAL_IF_ERROR(env->FindClass("Lescompat/StdProcess/ProcessManager;", &processManagerKlass));
    ANI_FATAL_IF_ERROR(
        env->Class_BindNativeMethods(processManagerKlass, processManagerImpls.data(), processManagerImpls.size()));

    ani_class processKlass;
    ANI_FATAL_IF_ERROR(env->FindClass("Lescompat/StdProcess/process;", &processKlass));
    ANI_FATAL_IF_ERROR(env->Class_BindNativeMethods(processKlass, processImpls.data(), processImpls.size()));
}

}  // namespace ark::ets::stdlib

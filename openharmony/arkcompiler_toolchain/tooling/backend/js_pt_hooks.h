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

#ifndef ECMASCRIPT_TOOLING_BACKEND_JS_PT_HOOKS_H
#define ECMASCRIPT_TOOLING_BACKEND_JS_PT_HOOKS_H

#include "tooling/base/pt_events.h"
#include "tooling/base/pt_script.h"

#include "ecmascript/debugger/js_debugger_interface.h"
#include "libpandabase/macros.h"

namespace panda::ecmascript::tooling {
class DebuggerImpl;

class JSPtHooks : public PtHooks {
public:
    explicit JSPtHooks(DebuggerImpl *debugger) : debugger_(debugger) {}
    ~JSPtHooks() override = default;

    void DebuggerStmt(const JSPtLocation &location) override;
    void Breakpoint(const JSPtLocation &location) override;
    void LoadModule(std::string_view pandaFileName, std::string_view entryPoint) override;
    void Exception(const JSPtLocation &location) override;
    bool SingleStep(const JSPtLocation &location) override;
    bool NativeOut() override;
    void NativeCalling(const void *nativeAddress) override;
    void NativeReturn(const void *nativeAddress) override;
    void VmStart() override {}
    void VmDeath() override {}
    void SendableMethodEntry(JSHandle<Method> method) override;

private:
    NO_COPY_SEMANTIC(JSPtHooks);
    NO_MOVE_SEMANTIC(JSPtHooks);

    DebuggerImpl *debugger_ {nullptr};
    bool firstTime_ {true};
};
}  // namespace panda::ecmascript::tooling
#endif  // ECMASCRIPT_TOOLING_BACKEND_JS_PT_HOOKS_H
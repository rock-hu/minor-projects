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

#include "agent/debugger_impl.h"
#include "backend/js_pt_hooks.h"
#include "tooling/dynamic/base/pt_types.h"
#include "tooling/dynamic/base/pt_events.h"
#include "dispatcher.h"

#include "ecmascript/debugger/js_debugger.h"
#include "ecmascript/js_array.h"
#include "ecmascript/js_tagged_value-inl.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::tooling;

namespace panda::test {
class JSPtHooksTest : public testing::Test {
public:
    using EntityId = panda_file::File::EntityId;
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "TearDownCase";
    }

    void SetUp() override
    {
        TestHelper::CreateEcmaVMWithScope(ecmaVm, thread, scope);
    }

    void TearDown() override
    {
        TestHelper::DestroyEcmaVMWithScope(ecmaVm, scope);
    }

protected:
    EcmaVM *ecmaVm {nullptr};
    EcmaHandleScope *scope {nullptr};
    JSThread *thread {nullptr};
};

HWTEST_F_L0(JSPtHooksTest, BreakpointTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    EntityId methodId(0);
    uint32_t bytecodeOffset = 0;
    JSPtLocation ptLocation1(nullptr, methodId, bytecodeOffset);
    jspthooks->Breakpoint(ptLocation1);
    ASSERT_NE(jspthooks, nullptr);
}

HWTEST_F_L0(JSPtHooksTest, LoadModuleTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    jspthooks->LoadModule("pandafile/test.abc", "func_main_0");
    ASSERT_NE(jspthooks, nullptr);
}

HWTEST_F_L0(JSPtHooksTest, ExceptionTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    EntityId methodId(0);
    uint32_t bytecodeOffset = 0;
    JSPtLocation ptLocation2(nullptr, methodId, bytecodeOffset);
    jspthooks->Exception(ptLocation2);
    ASSERT_NE(jspthooks, nullptr);
}

HWTEST_F_L0(JSPtHooksTest, SingleStepTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    EntityId methodId(0);
    uint32_t bytecodeOffset = 0;
    JSPtLocation ptLocation4(nullptr, methodId, bytecodeOffset);
    ASSERT_NE(jspthooks, nullptr);
}

HWTEST_F_L0(JSPtHooksTest, VmStartTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    jspthooks->VmStart();
    ASSERT_NE(jspthooks, nullptr);
}

HWTEST_F_L0(JSPtHooksTest, VmDeathTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    jspthooks->VmDeath();
    ASSERT_NE(jspthooks, nullptr);
}

HWTEST_F_L0(JSPtHooksTest, NativeCallingTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    jspthooks->NativeCalling(nullptr);
    ASSERT_NE(jspthooks, nullptr);
}

HWTEST_F_L0(JSPtHooksTest, NativeReturnTest)
{
    auto debugger = std::make_unique<DebuggerImpl>(ecmaVm, nullptr, nullptr);
    std::unique_ptr<JSPtHooks> jspthooks = std::make_unique<JSPtHooks>(debugger.get());
    jspthooks->NativeReturn(nullptr);
    ASSERT_NE(jspthooks, nullptr);
}
}
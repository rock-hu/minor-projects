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

#include "test/utils/test_util.h"

namespace panda::ecmascript::tooling::test {
TestMap TestUtil::testMap_;
os::memory::Mutex TestUtil::eventMutex_;
os::memory::ConditionVariable TestUtil::eventCv_;
DebugEvent TestUtil::lastEvent_ = DebugEvent::UNINITIALIZED;
bool TestUtil::initialized_ = false;
os::memory::Mutex TestUtil::suspendMutex_;
os::memory::ConditionVariable TestUtil::suspendCv_;
bool TestUtil::suspended_ = false;
JSPtLocation TestUtil::lastEventLocation_(nullptr, EntityId(0), 0);

std::ostream &operator<<(std::ostream &out, DebugEvent value)
{
    const char *s = nullptr;

#define ADD_CASE(entry) \
    case (entry):       \
        s = #entry;     \
        break

    switch (value) {
        ADD_CASE(DebugEvent::BREAKPOINT);
        ADD_CASE(DebugEvent::LOAD_MODULE);
        ADD_CASE(DebugEvent::PAUSED);
        ADD_CASE(DebugEvent::EXCEPTION);
        ADD_CASE(DebugEvent::STEP_COMPLETE);
        ADD_CASE(DebugEvent::METHOD_ENTRY);
        ADD_CASE(DebugEvent::SINGLE_STEP);
        ADD_CASE(DebugEvent::VM_START);
        ADD_CASE(DebugEvent::VM_INITIALIZATION);
        ADD_CASE(DebugEvent::VM_DEATH);
        ADD_CASE(DebugEvent::UNINITIALIZED);
        default: {
            ASSERT(false && "Unknown DebugEvent");  // NOLINT
        }
    }

#undef ADD_CASE

    return out << s;
}

std::ostream &operator<<(std::ostream &out, std::nullptr_t)
{
    return out << "nullptr";
}

TestEvents::TestEvents()
{
    scenario = []() {
        ASSERT_EXITED();
        return true;
    };
}
}  // namespace panda::ecmascript::tooling::test

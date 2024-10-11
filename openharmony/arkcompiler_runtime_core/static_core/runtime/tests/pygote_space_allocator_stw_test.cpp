/**
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

#include "runtime/tests/pygote_space_allocator_test_base.h"

namespace ark::mem {

class PygoteSpaceAllocatorStwTest : public PygoteSpaceAllocatorTest {
public:
    NO_COPY_SEMANTIC(PygoteSpaceAllocatorStwTest);
    NO_MOVE_SEMANTIC(PygoteSpaceAllocatorStwTest);

    PygoteSpaceAllocatorStwTest()
    {
        // Logger::InitializeStdLogging(Logger::Level::DEBUG, Logger::Component::ALL);
        options_.SetShouldLoadBootPandaFiles(false);
        options_.SetShouldInitializeIntrinsics(false);
        options_.SetStartAsZygote(true);
        options_.SetGcType("stw");
        options_.SetExplicitConcurrentGcEnabled(false);
        Runtime::Create(options_);
        thread_ = ark::MTManagedThread::GetCurrent();
        thread_->ManagedCodeBegin();
    }

    ~PygoteSpaceAllocatorStwTest() override
    {
        // Logger::Destroy();
        thread_->ManagedCodeEnd();
        Runtime::Destroy();
    }
};

TEST_F(PygoteSpaceAllocatorStwTest, InitAllocTest)
{
    InitAllocTest();
}

TEST_F(PygoteSpaceAllocatorStwTest, ForkedAllocTest)
{
    ForkedAllocTest();
}

TEST_F(PygoteSpaceAllocatorStwTest, NonMovableLiveObjectAllocTest)
{
    NonMovableLiveObjectAllocTest();
}

TEST_F(PygoteSpaceAllocatorStwTest, NonMovableUnliveObjectAllocTest)
{
    NonMovableUnliveObjectAllocTest();
}

TEST_F(PygoteSpaceAllocatorStwTest, MovableLiveObjectAllocTest)
{
    MovableLiveObjectAllocTest();
}

TEST_F(PygoteSpaceAllocatorStwTest, MovableUnliveObjectAllocTest)
{
    MovableUnliveObjectAllocTest();
}

TEST_F(PygoteSpaceAllocatorStwTest, MuchObjectAllocTest)
{
    MuchObjectAllocTest();
}

}  // namespace ark::mem

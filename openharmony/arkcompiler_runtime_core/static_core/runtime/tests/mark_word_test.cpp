/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include <random>

#include "gtest/gtest.h"
#include "runtime/include/managed_thread.h"
#include "runtime/mark_word.cpp"

// Test addresses should be aligned by 4
static constexpr size_t ADDRESS_ALIGNMENT = ~3U;

namespace ark {

class MarkWordTest : public testing::Test {
public:
    MarkWordTest() = default;

protected:
    void SetUp() override
    {
        // Logger::InitializeStdLogging(Logger::Level::DEBUG, Logger::Component::ALL);
    }

    void TearDown() override
    {
        // Logger::Destroy();
    }

    enum MarkWordFieldsMaxValues : MarkWord::MarkWordSize {
        MAX_THREAD_ID = (1UL << MarkWord::MarkWordRepresentation::LIGHT_LOCK_THREADID_SIZE) - 1UL,
        MAX_LOCK_COUNT = (1UL << MarkWord::MarkWordRepresentation::LIGHT_LOCK_LOCK_COUNT_SIZE) - 1UL,
        MAX_MONITOR_ID = (1UL << MarkWord::MarkWordRepresentation::MONITOR_POINTER_SIZE) - 1UL,
        MAX_HASH = (1UL << MarkWord::MarkWordRepresentation::HASH_SIZE) - 1UL,
        MAX_FORWARDING_ADDRESS = std::numeric_limits<MarkWord::MarkWordSize>::max() &
                                 MarkWord::MarkWordRepresentation::FORWARDING_ADDRESS_MASK_IN_PLACE,
    };

    class RandomTestValuesGetter {
    public:
        using MarkWordDistribution = std::uniform_int_distribution<MarkWord::MarkWordSize>;

        // NOLINTNEXTLINE(cert-msc51-cpp)
        RandomTestValuesGetter()
        {
#ifdef PANDA_NIGHTLY_TEST_ON
            seed_ = std::random_device()();
#else
            // NOLINTNEXTLINE(readability-magic-numbers)
            seed_ = 0xC0E67D50;
#endif
            gen_ = std::mt19937(seed_);

            threadIdRange_ = MarkWordDistribution(0, MAX_THREAD_ID);
            lockCountRange_ = MarkWordDistribution(0, MAX_LOCK_COUNT);
            monitorIdRange_ = MarkWordDistribution(0, MAX_MONITOR_ID);
            hashRange_ = MarkWordDistribution(0, MAX_HASH);
            forwardingAddressRange_ = MarkWordDistribution(0, MAX_FORWARDING_ADDRESS);
        }

        ManagedThread::ThreadId GetThreadId()
        {
            return threadIdRange_(gen_);
        }

        uint32_t GetLockCount()
        {
            return lockCountRange_(gen_);
        }

        Monitor::MonitorId GetMonitorId()
        {
            return monitorIdRange_(gen_);
        }

        uint32_t GetHash()
        {
            return hashRange_(gen_);
        }

        MarkWord::MarkWordSize GetForwardingAddress()
        {
            return (forwardingAddressRange_(gen_) &
                    MarkWord::MarkWordRepresentation::FORWARDING_ADDRESS_MASK_IN_PLACE) &
                   ADDRESS_ALIGNMENT;
        }

        uint32_t GetSeed()
        {
            return seed_;
        }

    private:
        uint32_t seed_;
        std::mt19937 gen_;
        MarkWordDistribution threadIdRange_;
        MarkWordDistribution lockCountRange_;
        MarkWordDistribution monitorIdRange_;
        MarkWordDistribution hashRange_;
        MarkWordDistribution forwardingAddressRange_;
    };

    class MaxTestValuesGetter {
    public:
        ManagedThread::ThreadId GetThreadId() const
        {
            return MAX_THREAD_ID;
        }

        uint32_t GetLockCount() const
        {
            return MAX_LOCK_COUNT;
        }

        Monitor::MonitorId GetMonitorId() const
        {
            return static_cast<Monitor::MonitorId>(MAX_MONITOR_ID);
        }

        uint32_t GetHash() const
        {
            return static_cast<uint32_t>(MAX_HASH);
        }

        MarkWord::MarkWordSize GetForwardingAddress() const
        {
            return MAX_FORWARDING_ADDRESS & ADDRESS_ALIGNMENT;
        }

        uint32_t GetSeed() const
        {
            // We don't have a seed for this case
            return 0;
        }
    };

    template <class Getter>
    class MarkWordWrapper {
    public:
        explicit MarkWordWrapper(bool isMarkedForGc = false, bool isReadBarrierSet = false)
        {
            if (isMarkedForGc) {
                mw_ = mw_.SetMarkedForGC();
            }
            if (isReadBarrierSet) {
                mw_ = mw_.SetReadBarrier();
            }
        };

        void CheckUnlocked(bool isMarkedForGc = false, bool isReadBarrierSet = false)
        {
            ASSERT_EQ(mw_.GetState(), MarkWord::ObjectState::STATE_UNLOCKED) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.IsMarkedForGC(), isMarkedForGc) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.IsReadBarrierSet(), isReadBarrierSet) << " seed = " << paramGetter_.GetSeed();
        }

        void CheckLightweightLock(const ManagedThread::ThreadId tId, const uint32_t lockCount, bool isMarkedForGc,
                                  bool isReadBarrierSet = false)
        {
            ASSERT_EQ(mw_.GetState(), MarkWord::ObjectState::STATE_LIGHT_LOCKED)
                << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.GetThreadId(), tId) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.GetLockCount(), lockCount) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.IsMarkedForGC(), isMarkedForGc) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.IsReadBarrierSet(), isReadBarrierSet) << " seed = " << paramGetter_.GetSeed();
        }

        void CheckHeavyweightLock(const Monitor::MonitorId mId, bool isMarkedForGc, bool isReadBarrierSet = false)
        {
            ASSERT_EQ(mw_.GetState(), MarkWord::ObjectState::STATE_HEAVY_LOCKED)
                << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.GetMonitorId(), mId) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.IsMarkedForGC(), isMarkedForGc) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.IsReadBarrierSet(), isReadBarrierSet) << " seed = " << paramGetter_.GetSeed();
        }

        void CheckHashed(uint32_t hash, bool isMarkedForGc, bool isReadBarrierSet = false)
        {
            if (mw_.CONFIG_IS_HASH_IN_OBJ_HEADER) {
                ASSERT_EQ(mw_.GetState(), MarkWord::ObjectState::STATE_HASHED) << " seed = " << paramGetter_.GetSeed();
                ASSERT_EQ(mw_.GetHash(), hash) << " seed = " << paramGetter_.GetSeed();
                ASSERT_EQ(mw_.IsMarkedForGC(), isMarkedForGc) << " seed = " << paramGetter_.GetSeed();
                ASSERT_EQ(mw_.IsReadBarrierSet(), isReadBarrierSet) << " seed = " << paramGetter_.GetSeed();
            }
        }

        void CheckGC(MarkWord::MarkWordSize forwardingAddress)
        {
            ASSERT_EQ(mw_.GetState(), MarkWord::ObjectState::STATE_GC) << " seed = " << paramGetter_.GetSeed();
            ASSERT_EQ(mw_.GetForwardingAddress(), forwardingAddress) << " seed = " << paramGetter_.GetSeed();
        }

        void DecodeLightLock(ManagedThread::ThreadId tId, uint32_t lCount)
        {
            mw_ = mw_.DecodeFromLightLock(tId, lCount);
        }

        void DecodeHeavyLock(Monitor::MonitorId mId)
        {
            mw_ = mw_.DecodeFromMonitor(mId);
        }

        void DecodeHash(uint32_t hash)
        {
            mw_ = mw_.DecodeFromHash(hash);
        }

        void DecodeForwardingAddress(MarkWord::MarkWordSize fAddress)
        {
            mw_ = mw_.DecodeFromForwardingAddress(fAddress);
        }

        void DecodeAndCheckLightLock(bool isMarkedForGc = false, bool isReadBarrierSet = false)
        {
            auto tId = paramGetter_.GetThreadId();
            auto lCount = paramGetter_.GetLockCount();
            DecodeLightLock(tId, lCount);
            CheckLightweightLock(tId, lCount, isMarkedForGc, isReadBarrierSet);
        }

        void DecodeAndCheckHeavyLock(bool isMarkedForGc = false, bool isReadBarrierSet = false)
        {
            auto mId = paramGetter_.GetMonitorId();
            DecodeHeavyLock(mId);
            CheckHeavyweightLock(mId, isMarkedForGc, isReadBarrierSet);
        }

        void DecodeAndCheckHashed(bool isMarkedForGc = false, bool isReadBarrierSet = false)
        {
            auto hash = paramGetter_.GetHash();
            DecodeHash(hash);
            CheckHashed(hash, isMarkedForGc, isReadBarrierSet);
        }

        void DecodeAndCheckGC()
        {
            auto fAddress = paramGetter_.GetForwardingAddress();
            DecodeForwardingAddress(fAddress);
            CheckGC(fAddress);
        }

        void SetMarkedForGC()
        {
            mw_ = mw_.SetMarkedForGC();
        }

        void SetReadBarrier()
        {
            mw_ = mw_.SetReadBarrier();
        }

    private:
        MarkWord mw_;
        Getter paramGetter_;
    };

    template <class Getter>
    void CheckMakeHashed(bool isMarkedForGc, bool isReadBarrierSet);

    template <class Getter>
    void CheckMakeLightweightLock(bool isMarkedForGc, bool isReadBarrierSet);

    template <class Getter>
    void CheckMakeHeavyweightLock(bool isMarkedForGc, bool isReadBarrierSet);

    template <class Getter>
    void CheckMakeGC();

    template <class Getter>
    void CheckMarkingWithGC();

    template <class Getter>
    void CheckReadBarrierSet();
};

template <class Getter>
void MarkWordTest::CheckMakeHashed(bool isMarkedForGc, bool isReadBarrierSet)
{
    // nothing, gc = markedForGC, rb = readBarrierSet, state = unlocked
    MarkWordWrapper<Getter> wrapper(isMarkedForGc, isReadBarrierSet);

    // check new hash
    wrapper.DecodeAndCheckHashed(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckHashed(isMarkedForGc, isReadBarrierSet);

    // check after lightweight lock
    wrapper.DecodeAndCheckLightLock(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckHashed(isMarkedForGc, isReadBarrierSet);

    // check after heavyweight lock
    wrapper.DecodeAndCheckHeavyLock(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckHashed(isMarkedForGc, isReadBarrierSet);
}

TEST_F(MarkWordTest, CreateHashedWithRandValues)
{
    CheckMakeHashed<RandomTestValuesGetter>(false, false);
    CheckMakeHashed<RandomTestValuesGetter>(false, true);
    CheckMakeHashed<RandomTestValuesGetter>(true, false);
    CheckMakeHashed<RandomTestValuesGetter>(true, true);
}

TEST_F(MarkWordTest, CreateHashedWithMaxValues)
{
    CheckMakeHashed<MaxTestValuesGetter>(false, false);
    CheckMakeHashed<MaxTestValuesGetter>(false, true);
    CheckMakeHashed<MaxTestValuesGetter>(true, false);
    CheckMakeHashed<MaxTestValuesGetter>(true, true);
}

template <class Getter>
void MarkWordTest::CheckMakeLightweightLock(bool isMarkedForGc, bool isReadBarrierSet)
{
    // nothing, gc = markedForGC, rb = readBarrierSet, state = unlocked
    MarkWordWrapper<Getter> wrapper(isMarkedForGc, isReadBarrierSet);

    // check new lightweight lock
    wrapper.DecodeAndCheckLightLock(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckLightLock(isMarkedForGc, isReadBarrierSet);

    // check after hash
    wrapper.DecodeAndCheckHashed(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckLightLock(isMarkedForGc, isReadBarrierSet);

    // check after heavyweight lock
    wrapper.DecodeAndCheckHeavyLock(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckLightLock(isMarkedForGc, isReadBarrierSet);
}

TEST_F(MarkWordTest, CreateLightweightLockWithRandValues)
{
    CheckMakeLightweightLock<RandomTestValuesGetter>(false, false);
    CheckMakeLightweightLock<RandomTestValuesGetter>(false, true);
    CheckMakeLightweightLock<RandomTestValuesGetter>(true, false);
    CheckMakeLightweightLock<RandomTestValuesGetter>(true, true);
}

TEST_F(MarkWordTest, CreateLightweightLockWithMaxValues)
{
    CheckMakeLightweightLock<MaxTestValuesGetter>(false, false);
    CheckMakeLightweightLock<MaxTestValuesGetter>(false, true);
    CheckMakeLightweightLock<MaxTestValuesGetter>(true, false);
    CheckMakeLightweightLock<MaxTestValuesGetter>(true, true);
}

template <class Getter>
void MarkWordTest::CheckMakeHeavyweightLock(bool isMarkedForGc, bool isReadBarrierSet)
{
    // nothing, gc = markedForGC, rb = readBarrierSet, state = unlocked
    MarkWordWrapper<Getter> wrapper(isMarkedForGc, isReadBarrierSet);

    // check new heavyweight lock
    wrapper.DecodeAndCheckHeavyLock(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckHeavyLock(isMarkedForGc, isReadBarrierSet);

    // check after hash
    wrapper.DecodeAndCheckHashed(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckHeavyLock(isMarkedForGc, isReadBarrierSet);

    // check after lightweight lock
    wrapper.DecodeAndCheckLightLock(isMarkedForGc, isReadBarrierSet);
    wrapper.DecodeAndCheckHeavyLock(isMarkedForGc, isReadBarrierSet);
}

TEST_F(MarkWordTest, CreateHeavyweightLockWithRandValues)
{
    CheckMakeHeavyweightLock<RandomTestValuesGetter>(false, false);
    CheckMakeHeavyweightLock<RandomTestValuesGetter>(false, true);
    CheckMakeHeavyweightLock<RandomTestValuesGetter>(true, false);
    CheckMakeHeavyweightLock<RandomTestValuesGetter>(true, true);
}

TEST_F(MarkWordTest, CreateHeavyweightLockWithMaxValues)
{
    CheckMakeHeavyweightLock<MaxTestValuesGetter>(false, false);
    CheckMakeHeavyweightLock<MaxTestValuesGetter>(false, true);
    CheckMakeHeavyweightLock<MaxTestValuesGetter>(true, false);
    CheckMakeHeavyweightLock<MaxTestValuesGetter>(true, true);
}

template <class Getter>
void MarkWordTest::CheckMakeGC()
{
    // check new gc
    {
        MarkWordWrapper<Getter> wrapper;
        wrapper.DecodeAndCheckGC();
        wrapper.DecodeAndCheckGC();
    }

    // check after hash
    {
        MarkWordWrapper<Getter> wrapper;
        wrapper.DecodeAndCheckHashed();
        wrapper.DecodeAndCheckGC();
    }

    // check after lightweight lock
    {
        MarkWordWrapper<Getter> wrapper;
        wrapper.DecodeAndCheckLightLock();
        wrapper.DecodeAndCheckGC();
    }

    // check after heavyweight lock
    {
        MarkWordWrapper<Getter> wrapper;
        wrapper.DecodeAndCheckHeavyLock();
        wrapper.DecodeAndCheckGC();
    }
}

TEST_F(MarkWordTest, CreateGCWithRandomValues)
{
    CheckMakeGC<RandomTestValuesGetter>();
}

TEST_F(MarkWordTest, CreateGCWithMaxValues)
{
    CheckMakeGC<MaxTestValuesGetter>();
}

template <class Getter>
void MarkWordTest::CheckMarkingWithGC()
{
    Getter paramGetter;

    // with unlocked
    {
        MarkWordWrapper<Getter> wrapper;

        wrapper.SetMarkedForGC();
        wrapper.CheckUnlocked(true);
    }

    // with lightweight locked
    {
        MarkWordWrapper<Getter> wrapper;
        auto tId = paramGetter.GetThreadId();
        auto lCount = paramGetter.GetLockCount();
        wrapper.DecodeLightLock(tId, lCount);

        wrapper.SetMarkedForGC();
        wrapper.CheckLightweightLock(tId, lCount, true);
    }

    // with heavyweight locked
    {
        MarkWordWrapper<Getter> wrapper;
        auto mId = paramGetter.GetMonitorId();
        wrapper.DecodeHeavyLock(mId);

        wrapper.SetMarkedForGC();
        wrapper.CheckHeavyweightLock(mId, true);
    }

    // with hashed
    {
        MarkWordWrapper<Getter> wrapper;
        auto hash = paramGetter.GetHash();
        wrapper.DecodeHash(hash);

        wrapper.SetMarkedForGC();
        wrapper.CheckHashed(hash, true);
    }
}

TEST_F(MarkWordTest, MarkWithGCWithRandValues)
{
    CheckMarkingWithGC<RandomTestValuesGetter>();
}

TEST_F(MarkWordTest, MarkWithGCWithMaxValues)
{
    CheckMarkingWithGC<MaxTestValuesGetter>();
}

template <class Getter>
void MarkWordTest::CheckReadBarrierSet()
{
    Getter paramGetter;

    // with unlocked
    {
        MarkWordWrapper<Getter> wrapper;

        wrapper.SetReadBarrier();
        wrapper.CheckUnlocked(false, true);
    }

    // with lightweight locked
    {
        MarkWordWrapper<Getter> wrapper;
        auto tId = paramGetter.GetThreadId();
        auto lCount = paramGetter.GetLockCount();
        wrapper.DecodeLightLock(tId, lCount);

        wrapper.SetReadBarrier();
        wrapper.CheckLightweightLock(tId, lCount, false, true);
    }

    // with heavyweight locked
    {
        MarkWordWrapper<Getter> wrapper;
        auto mId = paramGetter.GetMonitorId();
        wrapper.DecodeHeavyLock(mId);

        wrapper.SetReadBarrier();
        wrapper.CheckHeavyweightLock(mId, false, true);
    }

    // with hashed
    {
        MarkWordWrapper<Getter> wrapper;
        auto hash = paramGetter.GetHash();
        wrapper.DecodeHash(hash);

        wrapper.SetReadBarrier();
        wrapper.CheckHashed(hash, false, true);
    }
}

TEST_F(MarkWordTest, ReadBarrierSetWithRandValues)
{
    CheckReadBarrierSet<RandomTestValuesGetter>();
}

TEST_F(MarkWordTest, ReadBarrierSetWithMaxValues)
{
    CheckReadBarrierSet<MaxTestValuesGetter>();
}

}  //  namespace ark

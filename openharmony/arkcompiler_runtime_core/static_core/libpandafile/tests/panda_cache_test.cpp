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

#include "file.h"
#include "panda_cache.h"

#include <gtest/gtest.h>
#include <thread>

namespace ark {

class Method;
class Field;
class Class;

namespace panda_file::test {

using EntityId = File::EntityId;

static void *GetNewMockPointer()
{
    static int id = 1;
    return reinterpret_cast<void *>(id++);
}
TEST(PandaCache, TestMethodCache)
{
    PandaCache cache;
    // NOLINTNEXTLINE(readability-magic-numbers)
    EntityId id1(100);
    ASSERT_EQ(cache.GetMethodFromCache(id1), nullptr);

    auto *method1 = reinterpret_cast<Method *>(GetNewMockPointer());
    cache.SetMethodCache(id1, method1);
    ASSERT_EQ(cache.GetMethodFromCache(id1), method1);

    // NOLINTNEXTLINE(readability-magic-numbers)
    EntityId id2(10000);
    auto *method2 = reinterpret_cast<Method *>(GetNewMockPointer());
    cache.SetMethodCache(id2, method2);
    ASSERT_EQ(cache.GetMethodFromCache(id2), method2);
}

TEST(PandaCache, TestFieldCache)
{
    PandaCache cache;
    // NOLINTNEXTLINE(readability-magic-numbers)
    EntityId id1(100);
    // NOTE(yxr) : make sure no conflicts
    EntityId newId1(id1.GetOffset() << 2U);
    ASSERT_EQ(cache.GetFieldFromCache(newId1), nullptr);

    auto *field1 = reinterpret_cast<Field *>(GetNewMockPointer());
    cache.SetFieldCache(newId1, field1);
    ASSERT_EQ(cache.GetFieldFromCache(newId1), field1);

    // NOLINTNEXTLINE(readability-magic-numbers)
    EntityId id2(10000);
    // NOTE(yxr) : make sure no conflicts
    EntityId newId2(id2.GetOffset() << 2U);
    auto *field2 = reinterpret_cast<Field *>(GetNewMockPointer());
    cache.SetFieldCache(newId2, field2);
    ASSERT_EQ(cache.GetFieldFromCache(newId2), field2);
}

TEST(PandaCache, TestClassCache)
{
    PandaCache cache;
    // NOLINTNEXTLINE(readability-magic-numbers)
    EntityId id1(100);
    ASSERT_EQ(cache.GetClassFromCache(id1), nullptr);

    auto *class1 = reinterpret_cast<Class *>(GetNewMockPointer());
    cache.SetClassCache(id1, class1);
    ASSERT_EQ(cache.GetClassFromCache(id1), class1);

    // NOLINTNEXTLINE(readability-magic-numbers)
    EntityId id2(10000);
    auto *class2 = reinterpret_cast<Class *>(GetNewMockPointer());
    cache.SetClassCache(id2, class2);
    ASSERT_EQ(cache.GetClassFromCache(id2), class2);
}

struct ElementMock {
    int data;
};

static ElementMock *GetNewMockElement(int i)
{
    auto *m = new ElementMock();
    m->data = i;
    return m;
}

static const int NUMBER_OF_READERS = 4;
static const int NUMBER_OF_ELEMENTS = 4;

class CacheOps {
public:
    explicit CacheOps(PandaCache *cache) : cache_(cache) {}

    void RunWriter()
    {
        for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
            EntityId id(i);
            auto *m = GetNewMockElement(i);
            SetElement(id, m);
            ASSERT_EQ(GetElement(id), m);
        }
    }

    void RunReader()
    {
        for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
            EntityId id(i);
            auto *m = GetElement(id);
            while (m == nullptr) {
                m = GetElement(id);
            }
            int d = m->data;
            ASSERT_EQ(d, i);
        }
    }

protected:
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    PandaCache *cache_;
    virtual ElementMock *GetElement(EntityId id) = 0;
    virtual void SetElement(EntityId id, ElementMock *m) = 0;
};

class MethodCacheOps : public CacheOps {
public:
    explicit MethodCacheOps(PandaCache *cache) : CacheOps(cache) {}

protected:
    ElementMock *GetElement(EntityId id) override
    {
        Method *f = cache_->GetMethodFromCache(id);
        if (f == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<ElementMock *>(f);
    }

    void SetElement(EntityId id, ElementMock *m) override
    {
        auto *f = reinterpret_cast<Method *>(m);
        cache_->SetMethodCache(id, f);
    }
};

class FieldCacheOps : public CacheOps {
public:
    explicit FieldCacheOps(PandaCache *cache) : CacheOps(cache) {}

protected:
    ElementMock *GetElement(EntityId id) override
    {
        // NOTE(yxr) : make sure no conflicts
        // CacheOps.RunReader expect no conflicts
        EntityId newId(id.GetOffset() << 2U);
        Field *f = cache_->GetFieldFromCache(newId);
        if (f == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<ElementMock *>(f);
    }

    void SetElement(EntityId id, ElementMock *m) override
    {
        // NOTE(yxr) : make sure no conflicts
        // CacheOps.RunReader expect no conflicts
        EntityId newId(id.GetOffset() << 2U);
        auto *f = reinterpret_cast<Field *>(m);
        cache_->SetFieldCache(newId, f);
    }
};

class ClassCacheOps : public CacheOps {
public:
    explicit ClassCacheOps(PandaCache *cache) : CacheOps(cache) {}

protected:
    ElementMock *GetElement(EntityId id) override
    {
        Class *cl = cache_->GetClassFromCache(id);
        if (cl == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<ElementMock *>(cl);
    }

    void SetElement(EntityId id, ElementMock *m) override
    {
        auto *cl = reinterpret_cast<Class *>(m);
        cache_->SetClassCache(id, cl);
    }
};

void MethodWriterThread(PandaCache *cache)
{
    MethodCacheOps ops(cache);
    ops.RunWriter();
}

void MethodReaderThread(PandaCache *cache)
{
    MethodCacheOps ops(cache);
    ops.RunReader();
}

void FieldWriterThread(PandaCache *cache)
{
    FieldCacheOps ops(cache);
    ops.RunWriter();
}

void FieldReaderThread(PandaCache *cache)
{
    FieldCacheOps ops(cache);
    ops.RunReader();
}

void ClassWriterThread(PandaCache *cache)
{
    ClassCacheOps ops(cache);
    ops.RunWriter();
}

void ClassReaderThread(PandaCache *cache)
{
    ClassCacheOps ops(cache);
    ops.RunReader();
}

void CleanMethodMocks(const PandaCache *cache)
{
    for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
        EntityId id(i);
        auto *m = reinterpret_cast<ElementMock *>(cache->GetMethodFromCache(id));
        delete m;
    }
}

void CleanFieldMocks(const PandaCache *cache)
{
    for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
        EntityId id(i);
        // NOTE(yxr) : make sure no conflicts
        EntityId newId(id.GetOffset() << 2U);
        auto *m = reinterpret_cast<ElementMock *>(cache->GetFieldFromCache(newId));
        delete m;
    }
}

void CleanClassMocks(const PandaCache *cache)
{
    for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
        EntityId id(i);
        auto *m = reinterpret_cast<ElementMock *>(cache->GetClassFromCache(id));
        delete m;
    }
}

TEST(PandaCache, TestManyThreadsMethodCache)
{
    PandaCache cache;

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    std::thread threads[NUMBER_OF_READERS];
    auto writer = std::thread(MethodWriterThread, &cache);
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < NUMBER_OF_READERS; i++) {
        threads[i] = std::thread(MethodReaderThread, &cache);
    }
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < NUMBER_OF_READERS; i++) {
        threads[i].join();
    }
    writer.join();
    CleanMethodMocks(&cache);
}

TEST(PandaCache, TestManyThreadsFieldCache)
{
    PandaCache cache;

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    std::thread threads[NUMBER_OF_READERS];
    auto writer = std::thread(FieldWriterThread, &cache);
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < NUMBER_OF_READERS; i++) {
        threads[i] = std::thread(FieldReaderThread, &cache);
    }
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < NUMBER_OF_READERS; i++) {
        threads[i].join();
    }
    writer.join();
    CleanFieldMocks(&cache);
}

TEST(PandaCache, TestManyThreadsClassCache)
{
    PandaCache cache;

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    std::thread threads[NUMBER_OF_READERS];
    auto writer = std::thread(ClassWriterThread, &cache);
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < NUMBER_OF_READERS; i++) {
        threads[i] = std::thread(ClassReaderThread, &cache);
    }
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < NUMBER_OF_READERS; i++) {
        threads[i].join();
    }
    writer.join();
    CleanClassMocks(&cache);
}

}  // namespace panda_file::test
}  // namespace ark

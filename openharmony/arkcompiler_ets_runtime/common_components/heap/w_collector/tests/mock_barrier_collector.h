/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef COMMON_COMPONENTS_HEAP_W_COLLECTOR_MOCK_BARRIER_COLLECTOR_H
#define COMMON_COMPONENTS_HEAP_W_COLLECTOR_MOCK_BARRIER_COLLECTOR_H

#include "common_components/heap/collector/collector.h"

namespace common {
class MockCollector : public Collector {
public:
    void Init(const RuntimeParam& param) override {}
    void RunGarbageCollection(uint64_t, GCReason, GCType) override {}
    BaseObject* ForwardObject(BaseObject*) override
    {
        return nullptr;
    }
    bool ShouldIgnoreRequest(GCRequest&) override
    {
        return false;
    }
    bool IsFromObject(BaseObject*) const override
    {
        return false;
    }
    bool IsUnmovableFromObject(BaseObject*) const override
    {
        return false;
    }
    BaseObject* FindToVersion(BaseObject*) const override
    {
        return nullptr;
    }

    bool TryUpdateRefField(BaseObject* obj, RefField<>& field, BaseObject*& toVersion) const override
    {
        toVersion = reinterpret_cast<BaseObject*>(field.GetFieldValue());
        return true;
    }

    bool TryForwardRefField(BaseObject*, RefField<>&, BaseObject*&) const override
    {
        return false;
    }
    bool TryUntagRefField(BaseObject*, RefField<>&, BaseObject*&) const override
    {
        return false;
    }
    RefField<> GetAndTryTagRefField(BaseObject*) const override
    {
        return RefField<>(nullptr);
    }
    bool IsOldPointer(RefField<>&) const override
    {
        return false;
    }
    bool IsCurrentPointer(RefField<>&) const override
    {
        return false;
    }
    void AddRawPointerObject(BaseObject*) override {}
    void RemoveRawPointerObject(BaseObject*) override {}
};

class MockCollectorUnmovableTest : public MockCollector {
public:
    bool IsFromObject(BaseObject*) const override
    {
        return true;
    }
    bool IsUnmovableFromObject(BaseObject*) const override
    {
        return true;
    }
};

class MockCollectorForwardTest : public MockCollector {
public:
    bool IsFromObject(BaseObject*) const override
    {
        return true;
    }
    bool TryForwardRefField(BaseObject*, RefField<>&, BaseObject*&) const override
    {
        return true;
    }
};
} // namespace common

#endif // COMMON_COMPONENTS_HEAP_W_COLLECTOR_MOCK_BARRIER_COLLECTOR_H
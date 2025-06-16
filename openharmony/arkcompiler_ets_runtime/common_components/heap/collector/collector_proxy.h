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

#ifndef COMMON_COMPONENTS_HEAP_COLLECTOR_COLLECTOR_PROXY_H
#define COMMON_COMPONENTS_HEAP_COLLECTOR_COLLECTOR_PROXY_H

#include "common_components/heap/collector/collector.h"
#include "common_components/heap/collector/collector_resources.h"
#include "common_components/heap/w_collector/w_collector.h"

namespace common {
// CollectorProxy is a special kind of collector, it is derived from Base class Collector, thus behaves like a real
// collector. However, it actually manages a set of collectors implemented yet, and delegate garbage-collecting to
// one of these collectors.
// CollectorProxy should inherit collector interfaces, but no datas
class CollectorProxy : public Collector {
public:
    explicit CollectorProxy(Allocator& allocator, CollectorResources& resources)
        : wCollector_(allocator, resources)
    {
        collectorType_ = CollectorType::PROXY_COLLECTOR;
    }

    ~CollectorProxy() override = default;

    void Init(const RuntimeParam& param) override;
    void Fini() override;

    GCPhase GetGCPhase() const override { return currentCollector_->GetGCPhase(); }

    void SetGCPhase(const GCPhase phase) override { currentCollector_->SetGCPhase(phase); }

    // dispatch garbage collection to the right collector
    PUBLIC_API void RunGarbageCollection(uint64_t gcIndex, GCReason reason) override;

    bool ShouldIgnoreRequest(GCRequest& request) override { return currentCollector_->ShouldIgnoreRequest(request); }

    TraceCollector& GetCurrentCollector() const { return *currentCollector_; }

    BaseObject* FindToVersion(BaseObject* obj) const override { return currentCollector_->FindToVersion(obj); }

    bool IsOldPointer(RefField<>& ref) const override { return currentCollector_->IsOldPointer(ref); }
    bool IsCurrentPointer(RefField<>& ref) const override { return currentCollector_->IsCurrentPointer(ref); }

    bool IsFromObject(BaseObject* obj) const override { return currentCollector_->IsFromObject(obj); }

    // bool IsGhostFromObject(BaseObject* obj) const override { return currentCollector->IsGhostFromObject(obj); }

    bool IsUnmovableFromObject(BaseObject* obj) const override { return currentCollector_->IsUnmovableFromObject(obj); }

    void AddRawPointerObject(BaseObject* obj) override { return currentCollector_->AddRawPointerObject(obj); }
    void RemoveRawPointerObject(BaseObject* obj) override { return currentCollector_->RemoveRawPointerObject(obj); }

    BaseObject* ForwardObject(BaseObject* obj) override { return currentCollector_->ForwardObject(obj); }

    bool TryUpdateRefField(BaseObject* obj, RefField<>& field, BaseObject*& toVersion) const override
    {
        return currentCollector_->TryUpdateRefField(obj, field, toVersion);
    }

    bool TryForwardRefField(BaseObject* obj, RefField<>& field, BaseObject*& toVersion) const override
    {
        return currentCollector_->TryForwardRefField(obj, field, toVersion);
    }

    bool TryUntagRefField(BaseObject* obj, RefField<>& field, BaseObject*& target) const override
    {
        return currentCollector_->TryUntagRefField(obj, field, target);
    }

    RefField<> GetAndTryTagRefField(BaseObject* obj) const override
    {
        return currentCollector_->GetAndTryTagRefField(obj);
    }

private:
    // supported collector set
    TraceCollector* currentCollector_ = nullptr;
    WCollector wCollector_;
};
} // namespace common

#endif // COMMON_COMPONENTS_HEAP_COLLECTOR_COLLECTOR_PROXY_H

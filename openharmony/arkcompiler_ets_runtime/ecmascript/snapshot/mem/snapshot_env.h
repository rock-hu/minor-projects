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

#ifndef ECMASCRIPT_SNAPSHOT_MEM_SNAPSHOT_ENV_H
#define ECMASCRIPT_SNAPSHOT_MEM_SNAPSHOT_ENV_H

#include <unordered_map>

#include "ecmascript/js_thread.h"
#include "ecmascript/mem/visitor.h"

namespace panda::ecmascript {
class EcmaVM;

class SnapshotEnv final {
public:
    explicit SnapshotEnv(EcmaVM *vm) : vm_(vm) {}
    ~SnapshotEnv() = default;

    void AddGlobalConstToMap();

    void ClearEnvMap()
    {
        globalObjectMap_.clear();
    }

    void ProcessSnapShotEnv(const WeakRootVisitor& visitor);
    void IteratorSnapShotEnv(WeakVisitor& visitor);

    void Push(JSTaggedType objectAddr, uint32_t index)
    {
        globalObjectMap_.emplace(objectAddr, index);
    }

    void Remove(JSTaggedType objectAddr)
    {
        globalObjectMap_.erase(objectAddr);
    }

    uint32_t FindEnvObjectIndex(JSTaggedType objectAddr) const
    {
        if (globalObjectMap_.find(objectAddr) != globalObjectMap_.end()) {
            return globalObjectMap_.find(objectAddr)->second;
        }
        return MAX_UINT_32;
    }

    JSTaggedType RelocateRootObjectAddr(uint32_t index);

    static constexpr size_t MAX_UINT_32 = 0xFFFFFFFF;

private:
    NO_MOVE_SEMANTIC(SnapshotEnv);
    NO_COPY_SEMANTIC(SnapshotEnv);

    void InitGlobalConst();
    void InitGlobalEnv();

    EcmaVM *vm_;
    std::unordered_map<JSTaggedType, uint32_t> globalObjectMap_;
    std::atomic<uint32_t> multiThreadCheckValue_ {0};
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_SNAPSHOT_MEM_SNAPSHOT_ENV_H

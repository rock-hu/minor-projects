/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_PGO_EXTRA_PROFILER_H
#define ECMASCRIPT_PGO_EXTRA_PROFILER_H

#include <chrono>
#include <memory>

#include "ecmascript/common.h"
#include "ecmascript/ecma_macros.h"
#include "ecmascript/elements.h"

namespace panda::ecmascript {
namespace pgo {
class ExtraProfileTypeInfo : public TaggedObject {
public:
    CAST_CHECK(ExtraProfileTypeInfo, IsExtraProfileTypeInfo);

    static constexpr size_t RECEIVER_OBJECT_OFFSET = TaggedObjectSize();
    ACCESSORS(ReceiverObject, RECEIVER_OBJECT_OFFSET, HOLDER_OBJECT_OFFSET);
    ACCESSORS(HolderObject, HOLDER_OBJECT_OFFSET, LAST_OFFSET);
    DEFINE_ALIGN_SIZE(LAST_OFFSET);

    DECL_VISIT_OBJECT(RECEIVER_OBJECT_OFFSET, LAST_OFFSET);

    JSHClass* GetReceiverHClass(const JSThread *thread) const
    {
        return GetReceiverObject(thread).GetHeapObject()->GetClass();
    }

    JSHClass* GetHolderHClass(const JSThread *thread) const
    {
        return GetHolderObject(thread).GetTaggedObject()->GetClass();
    }

    void SetReceiver(const JSThread *thread, JSTaggedValue value)
    {
        SetReceiverObject(thread, value);
    }

    void SetHolder(const JSThread *thread, JSTaggedValue value)
    {
        SetHolderObject(thread, value);
    }

    JSTaggedValue GetReceiver(const JSThread *thread) const
    {
        return GetReceiverObject(thread);
    }

    JSTaggedValue GetHolder(const JSThread *thread) const
    {
        return GetHolderObject(thread);
    }

    void Clear(const JSThread *thread)
    {
        SetReceiverObject(thread, JSTaggedValue::Hole());
        SetHolderObject(thread, JSTaggedValue::Hole());
    }

    void ClearReceiver(const JSThread *thread)
    {
        SetReceiverObject(thread, JSTaggedValue::Hole());
    }

    void ClearHolder(const JSThread *thread)
    {
        SetHolderObject(thread, JSTaggedValue::Hole());
    }

    bool EuqalTo(JSThread *thread, const ExtraProfileTypeInfo& other) const
    {
        return GetReceiverObject(thread) == other.GetReceiverObject(thread) &&
               GetHolderObject(thread) == other.GetHolderObject(thread);
    }

    bool IsValid(JSThread *thread) const
    {
        return !GetReceiverObject(thread).IsHole() && !GetReceiverObject(thread).IsUndefined();
    }

    DECL_DUMP()
};

} // namespace pgo
} // namespace panda::ecmascript
#endif // ECMASCRIPT_PGO_EXTRA_PROFILER_H

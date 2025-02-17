/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_JS_API_JS_API_STACK_H
#define ECMASCRIPT_JS_API_JS_API_STACK_H

#include "ecmascript/js_object.h"
#include "ecmascript/js_tagged_value-inl.h"

namespace panda::ecmascript {
class JSAPIStack : public JSObject {
public:
    static constexpr uint32_t DEFAULT_CAPACITY_LENGTH = 10;
    static JSAPIStack *Cast(TaggedObject *object)
    {
        ASSERT(JSTaggedValue(object).IsJSAPIStack());
        return static_cast<JSAPIStack *>(object);
    }

    static JSTaggedValue Push(JSThread *thread, const JSHandle<JSAPIStack> &stack,
                              const JSHandle<JSTaggedValue> &value);

    static JSHandle<TaggedArray> OwnKeys(JSThread *thread, const JSHandle<JSAPIStack> &obj);

    static JSHandle<TaggedArray> OwnEnumKeys(JSThread *thread, const JSHandle<JSAPIStack> &obj);

    static bool GetOwnProperty(JSThread *thread, const JSHandle<JSAPIStack> &obj, const JSHandle<JSTaggedValue> &key);
    static OperationResult GetProperty(JSThread *thread, const JSHandle<JSAPIStack> &obj,
                                       const JSHandle<JSTaggedValue> &key);
    static bool SetProperty(JSThread *thread, const JSHandle<JSAPIStack> &obj,
                            const JSHandle<JSTaggedValue> &key,
                            const JSHandle<JSTaggedValue> &value);
    bool Empty();

    bool Has(JSTaggedValue value) const;

    int Search(const JSHandle<JSTaggedValue> &value);

    inline int32_t GetSize() const
    {
        return GetTop();
    }

    JSTaggedValue Peek();

    JSTaggedValue Pop(JSThread *thread);

    JSTaggedValue Get(const uint32_t index);

    JSTaggedValue PUBLIC_API Set(JSThread *thread, const uint32_t index, JSTaggedValue value);

    static constexpr size_t TOP_OFFSET = JSObject::SIZE;
    ACCESSORS_PRIMITIVE_FIELD(Top, int, TOP_OFFSET, LAST_OFFSET)
    DEFINE_ALIGN_SIZE(LAST_OFFSET);

    DECL_VISIT_OBJECT_FOR_JS_OBJECT(JSObject, LAST_OFFSET, LAST_OFFSET)
    DECL_DUMP()
private:
    inline static uint32_t ComputeCapacity(uint32_t oldCapacity)
    {
        uint32_t newCapacity = oldCapacity + (oldCapacity >> 1U);
        return newCapacity > DEFAULT_CAPACITY_LENGTH ? newCapacity : DEFAULT_CAPACITY_LENGTH;
    }
    static JSHandle<TaggedArray> GrowCapacity(const JSThread *thread, const JSHandle<JSAPIStack> &obj,
                                              uint32_t capacity);
};
}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_JS_API_JS_API_STACK_H

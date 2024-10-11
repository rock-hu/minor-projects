/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_COMPILER_BUILTINS_CONTAINERS_DEQUE_STUB_BUILDER_H
#define ECMASCRIPT_COMPILER_BUILTINS_CONTAINERS_DEQUE_STUB_BUILDER_H
#include "ecmascript/compiler/stub_builder-inl.h"
#include "ecmascript/js_api/js_api_deque.h"

namespace panda::ecmascript::kungfu {
class ContainersDequeStubBuilder : public StubBuilder {
public:
    explicit ContainersDequeStubBuilder(StubBuilder *parent)
        : StubBuilder(parent) {}
    ~ContainersDequeStubBuilder() override = default;
    NO_MOVE_SEMANTIC(ContainersDequeStubBuilder);
    NO_COPY_SEMANTIC(ContainersDequeStubBuilder);
    void GenerateCircuit() override {}

    GateRef GetSize(GateRef obj)
    {
        GateRef elementsOffset = IntPtr(JSObject::ELEMENTS_OFFSET);
        GateRef elements = Load(VariableType::JS_POINTER(), obj, elementsOffset);
        GateRef capacity = Load(VariableType::INT32(), elements, IntPtr(TaggedArray::LENGTH_OFFSET));
        GateRef first = GetFirst(obj);
        GateRef last = GetLast(obj);
        return Int32Mod(Int32Add(Int32Sub(last, first), capacity), capacity);
    }

    GateRef Get(GateRef obj, GateRef index)
    {
        GateRef elementsOffset = IntPtr(JSObject::ELEMENTS_OFFSET);
        GateRef elements = Load(VariableType::JS_POINTER(), obj, elementsOffset);
        GateRef capacity = Load(VariableType::INT32(), elements, IntPtr(TaggedArray::LENGTH_OFFSET));
        GateRef first = GetFirst(obj);
        GateRef curIndex = Int32Mod(Int32Add(first, index), capacity);
        return GetValueFromTaggedArray(elements, curIndex);
    }

    GateRef GetFirst(GateRef obj)
    {
        return Load(VariableType::INT32(), obj, IntPtr(JSAPIDeque::FIRST_OFFSET));
    }

    GateRef GetLast(GateRef obj)
    {
        return Load(VariableType::INT32(), obj, IntPtr(JSAPIDeque::LAST_OFFSET));
    }

    GateRef GetElementsLength(GateRef obj)
    {
        GateRef elementsOffset = IntPtr(JSObject::ELEMENTS_OFFSET);
        GateRef elements = Load(VariableType::JS_POINTER(), obj, elementsOffset);
        return Load(VariableType::INT32(), elements, IntPtr(TaggedArray::LENGTH_OFFSET));
    }
};
}  // namespace panda::ecmascript::kungfu
#endif  // ECMASCRIPT_COMPILER_BUILTINS_CONTAINERS_DEQUE_STUB_BUILDER_H
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

#ifndef ECMASCRIPT_COMPILER_BUILTINS_CONTAINERS_LIST_STUB_BUILDER_H
#define ECMASCRIPT_COMPILER_BUILTINS_CONTAINERS_LIST_STUB_BUILDER_H
#include "ecmascript/compiler/stub_builder-inl.h"
#include "ecmascript/js_api/js_api_list.h"

namespace panda::ecmascript::kungfu {
class ContainersListStubBuilder : public StubBuilder {
public:
    explicit ContainersListStubBuilder(StubBuilder *parent)
        : StubBuilder(parent) {}
    ~ContainersListStubBuilder() override = default;
    NO_MOVE_SEMANTIC(ContainersListStubBuilder);
    NO_COPY_SEMANTIC(ContainersListStubBuilder);
    void GenerateCircuit() override {}

    GateRef GetTableLength(GateRef obj)
    {
        GateRef tableOffset = IntPtr(JSAPIList::SINGLY_LIST_OFFSET);
        GateRef table = Load(VariableType::JS_POINTER(), obj, tableOffset);
        GateRef value = GetValueFromTaggedArray(table, Int32(TaggedSingleList::NUMBER_OF_NODE_INDEX));
        return TaggedGetInt(value);
    }

    GateRef GetNode(GateRef obj, GateRef index)
    {
        GateRef tableOffset = IntPtr(JSAPIList::SINGLY_LIST_OFFSET);
        GateRef table = Load(VariableType::JS_POINTER(), obj, tableOffset);
        return GetValueFromTaggedArray(table, index);
    }
};
}  // namespace panda::ecmascript::kungfu
#endif  // ECMASCRIPT_COMPILER_BUILTINS_CONTAINERS_LIST_STUB_BUILDER_H
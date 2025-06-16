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

#include <atomic>
#include <cstdint>

#include "objects/composite_base_class.h"
#include "objects/base_object.h"

namespace common {
void BaseClassRoots::InitializeCompositeBaseClass(CompositeBaseClassAllocator &allocator)
{
    if (initialized_.exchange(true)) {
        return;
    }
    CreateCompositeBaseClass(CommonType::LINE_STRING, allocator);
    CreateCompositeBaseClass(CommonType::SLICED_STRING, allocator);
    CreateCompositeBaseClass(CommonType::TREE_STRING, allocator);
}

void BaseClassRoots::CreateCompositeBaseClass(CommonType type, CompositeBaseClassAllocator& allocator)
{
    CompositeBaseClass* classObject = allocator();
    classObject->class_.ClearBitField();
    classObject->class_.SetObjectType(type);
    size_t index = TypeToIndex[static_cast<size_t>(type)];
    compositeBaseClasses_[index] = classObject;
    baseClasses_[index] = &classObject->class_;
}

BaseClass* BaseClassRoots::GetBaseClass(CommonType type) const
{
    return baseClasses_[TypeToIndex[static_cast<size_t>(type)]];
}

void BaseClassRoots::IterateCompositeBaseClass(const RefFieldVisitor& visitorFunc)
{
    if (!initialized_) {
        return;
    }
    for (auto& it : compositeBaseClasses_) {
        visitorFunc(reinterpret_cast<RefField<>&>(it));
    }
}

} // namespace panda

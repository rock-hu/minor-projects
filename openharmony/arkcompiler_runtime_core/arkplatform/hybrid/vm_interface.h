/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef PANDA_VM_INTERFACE_H
#define PANDA_VM_INTERFACE_H

#include <cstdint>

namespace arkplatform {

class VMInterface {
public:
    enum class VMInterfaceType: uint8_t {
        ETS_VM_IFACE = 0,
        ECMA_VM_IFACE
    };

    VMInterface() = default;

    VMInterface(const VMInterface &) = delete;
    void operator=(const VMInterface &) = delete;

    VMInterface(VMInterface &&) = delete;
    VMInterface &operator=(VMInterface &&) = delete;

    virtual ~VMInterface() = default;

    virtual VMInterfaceType GetVMType() const = 0;
};

} // namespace arkplatform

#endif // PANDA_VM_INTERFACE_H
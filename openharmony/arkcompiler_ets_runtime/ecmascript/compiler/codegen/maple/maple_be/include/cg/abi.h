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

#ifndef MAPLEBE_INCLUDE_CG_ABI_H
#define MAPLEBE_INCLUDE_CG_ABI_H

#include <cstdint>
#include "types_def.h"
#include "operand.h"

namespace maplebe {
enum ArgumentClass : uint8 {
    kNoClass,
    kIntegerClass,
    kFloatClass,
    kPointerClass,
    kVectorClass,
    kMemoryClass,
    kShortVectorClass,
    kCompositeTypeHFAClass, /* Homegeneous Floating-point Aggregates for AArch64 */
    kCompositeTypeHVAClass, /* Homegeneous Short-Vector Aggregates for AArch64 */
};

using regno_t = uint32_t;

} /* namespace maplebe */

#endif /* MAPLEBE_INCLUDE_CG_ABI_H */

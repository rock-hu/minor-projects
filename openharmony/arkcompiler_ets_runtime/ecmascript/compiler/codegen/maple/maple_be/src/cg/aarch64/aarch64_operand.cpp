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

#include "aarch64_operand.h"
#include <fstream>
#include <string>
#include "aarch64_abi.h"
#include "aarch64_cgfunc.h"
#include "aarch64_cg.h"

namespace maplebe {
bool StImmOperand::Less(const Operand &right) const
{
    if (&right == this) {
        return false;
    }

    /* For different type. */
    if (GetKind() != right.GetKind()) {
        return GetKind() < right.GetKind();
    }

    const StImmOperand *rightOpnd = static_cast<const StImmOperand *>(&right);
    if (symbol != rightOpnd->symbol) {
        return symbol < rightOpnd->symbol;
    }
    if (offset != rightOpnd->offset) {
        return offset < rightOpnd->offset;
    }
    return relocs < rightOpnd->relocs;
}

bool ExtendShiftOperand::Less(const Operand &right) const
{
    if (&right == this) {
        return false;
    }
    /* For different type. */
    if (GetKind() != right.GetKind()) {
        return GetKind() < right.GetKind();
    }

    const ExtendShiftOperand *rightOpnd = static_cast<const ExtendShiftOperand *>(&right);

    /* The same type. */
    if (extendOp != rightOpnd->extendOp) {
        return extendOp < rightOpnd->extendOp;
    }
    return shiftAmount < rightOpnd->shiftAmount;
}
} /* namespace maplebe */

/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#define xlr x30

.global ExecuteImplStub
.type ExecuteImplStub, %function
/* parameter:
 * x0 thread
 * x1 PC
 * x2 I-frame
 * x3 jump_to_eh
 * x4 impl
 */
ExecuteImplStub:
    .cfi_startproc
    stp x20, xlr, [sp, #-16]!
    .cfi_adjust_cfa_offset 16
    .cfi_rel_offset x20, 0
    .cfi_rel_offset xlr, 8
    /*
     * Using CFI opcode to get the interpreter PC from
     * saved register with offset.
     *
     * 0x16 DW_CFA_val_expression
     * 0    tempReg
     * 0x09 size of expression
     * 0x0C DW_OP_const4u
     * 0x41, 0x45, 0x58, 0x31 magic = "AEX1"
     * 0x13 DW_OP_drop
     * 0x92 DW_OP_bregx
     * 20   the register which store interpreter PC
     * 0    offset
     */
    .cfi_escape 0x16, 0, 0x09, 0x0C, 0x41, 0x45, 0x58, 0x31, 0x13, 0x92, 20, 0

    mov x20, x1
    blr x4

    ldp x20, xlr, [sp], #16
    .cfi_restore x20
    .cfi_restore xlr
    .cfi_adjust_cfa_offset -16
    ret
    .cfi_endproc

#undefine xlr

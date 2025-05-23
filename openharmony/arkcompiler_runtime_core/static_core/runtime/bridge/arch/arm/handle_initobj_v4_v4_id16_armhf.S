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

    // handle initobj.short || initobj

    NEXT_SHORTY r2
    cmp r2, #0
    beq .Linvoke_from_bridge

    sub r3, r9, #(FRAME_VREGS_OFFSET - FRAME_ACC_OFFSET)
    PUSH_ARG r3, r5, r6, r7, r2, r3, .Lloop_handle_initobj

.Lloop_handle_initobj:
    NEXT_SHORTY r2
    cmp r2, #0
    beq .Linvoke_from_bridge

    // read the pair of arg indexes into r3 and advance insn_ptr
    ldrb r10, [r8], #1
    and r3, r10, #0xF
    add r3, r9, r3, lsl #3

    // handle the first arg index
    PUSH_ARG r3, r5, r6, r7, r2, r3, 10f

10: // handle the second arg index
    NEXT_SHORTY r2
    cmp r2, #0
    beq .Linvoke_from_bridge

    asr r3, r10, #4
    add r3, r9, r3, lsl #3

    PUSH_ARG r3, r5, r6, r7, r2, r3, .Lloop_handle_initobj
    b .Lloop_handle_initobj

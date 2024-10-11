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

#include "runtime/interpreter/interpreter.h"

#include "runtime/interpreter/arch/macros.h"
#include "runtime/interpreter/interpreter_impl.h"

#ifdef PANDA_INTERPRETER_ARCH_GLOBAL_REGS_H_
#error "arch/global_reg.h must not be included as it can broke ABI"
#endif

namespace ark::interpreter {

void Execute(ManagedThread *thread, const uint8_t *pc, Frame *frame, bool jumpToEh)
{
    ExecuteImpl(thread, pc, frame, jumpToEh);
    RESTORE_GLOBAL_REGS();
}

}  // namespace ark::interpreter

namespace ark {
ALWAYS_INLINE inline const uint8_t *Frame::GetInstrOffset()
{
    return (method_->GetInstructions());
}

}  // namespace ark

/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef LIBLLVMBACKEND_UTILS_H
#define LIBLLVMBACKEND_UTILS_H

#include "llvm_ark_interface.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

namespace ark::llvmbackend::utils {

int64_t GetMethodIdFromAttr(llvm::CallInst *call);
bool HasCallsWithDeopt(llvm::Function &func);
void CopyDebugLoc(llvm::CallInst *from, llvm::CallInst *to);
llvm::SmallVector<llvm::OperandBundleDef> CopyDeoptBundle(llvm::CallInst *from);
llvm::Value *CreateLoadClassFromObject(llvm::Value *object, llvm::IRBuilder<> *builder,
                                       ark::llvmbackend::LLVMArkInterface *arkInterface);
llvm::Value *CreateLoadMethodUsingVTable(llvm::Value *thiz, llvm::Function *caller, size_t methodId,
                                         llvm::IRBuilder<> *builder, LLVMArkInterface *arkInterface);

}  // namespace ark::llvmbackend::utils

#endif  // LIBLLVMBACKEND_UTILS_H

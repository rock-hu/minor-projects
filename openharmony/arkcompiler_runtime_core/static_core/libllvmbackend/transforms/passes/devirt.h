/**
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

#ifndef LIBLLVMBACKEND_TRANSFORMS_PASSES_DEVIRT_PASS_H
#define LIBLLVMBACKEND_TRANSFORMS_PASSES_DEVIRT_PASS_H

#include <unordered_map>

#include "llvm_ark_interface.h"

#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/PassManager.h>

namespace ark::llvmbackend {
struct LLVMCompilerOptions;
}  // namespace ark::llvmbackend

namespace llvm {
class CallInst;
class Instruction;
}  // namespace llvm

namespace ark::llvmbackend::passes {

class Devirt : public llvm::PassInfoMixin<Devirt> {
public:
    explicit Devirt(LLVMArkInterface *arkInterface = nullptr);

    static bool ShouldInsert(const ark::llvmbackend::LLVMCompilerOptions *options);

    static Devirt Create(LLVMArkInterface *arkInterface, const ark::llvmbackend::LLVMCompilerOptions *options);

    // NOLINTNEXTLINE(readability-identifier-naming)
    llvm::PreservedAnalyses run(llvm::Function &function, llvm::FunctionAnalysisManager &analysisManager);

private:
    LLVMArkInterface *arkInterface_;

public:
    static constexpr llvm::StringRef ARG_NAME = "devirt";
};

}  // namespace ark::llvmbackend::passes

#endif  // LIBLLVMBACKEND_TRANSFORMS_PASSES_DEVIRT_PASS_H

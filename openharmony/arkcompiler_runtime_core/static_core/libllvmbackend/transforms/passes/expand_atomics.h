/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef LIBLLVMBACKEND_TRANSFORMS_PASSES_EXPAND_ATOMICS_H
#define LIBLLVMBACKEND_TRANSFORMS_PASSES_EXPAND_ATOMICS_H

#include <llvm/IR/PassManager.h>

namespace ark::llvmbackend {
struct LLVMCompilerOptions;
}  // namespace ark::llvmbackend

namespace ark::llvmbackend::passes {

class ExpandAtomics : public llvm::PassInfoMixin<ExpandAtomics> {
public:
    static constexpr llvm::StringRef ARG_NAME = "expand-atomics";

    explicit ExpandAtomics();

    static bool ShouldInsert([[maybe_unused]] const ark::llvmbackend::LLVMCompilerOptions *options)
    {
        return true;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    llvm::PreservedAnalyses run(llvm::Function &function, llvm::FunctionAnalysisManager &analysisManager);

private:
    bool InsertAddrSpaceCast(llvm::Instruction *atomicInstruction);
};

}  // namespace ark::llvmbackend::passes

#endif  // LIBLLVMBACKEND_TRANSFORMS_PASSES_EXPAND_ATOMICS_H

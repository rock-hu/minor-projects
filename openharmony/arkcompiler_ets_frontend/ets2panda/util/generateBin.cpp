/*
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

#include "generateBin.h"
#include "bytecode_optimizer/bytecodeopt_options.h"
#include "bytecode_optimizer/optimize_bytecode.h"
#include "compiler/compiler_logger.h"
#include "compiler/compiler_options.h"

namespace ark::es2panda::util {

[[maybe_unused]] static void InitializeLogging(const util::Options *options)
{
    ark::Logger::ComponentMask componentMask;
    componentMask.set(ark::Logger::Component::ASSEMBLER);
    componentMask.set(ark::Logger::Component::COMPILER);
    componentMask.set(ark::Logger::Component::BYTECODE_OPTIMIZER);

    if (!ark::Logger::IsInitialized()) {
        ark::Logger::InitializeStdLogging(Logger::LevelFromString(options->LogLevel()), componentMask);
    } else {
        ark::Logger::EnableComponent(componentMask);
    }
}

#ifdef PANDA_WITH_BYTECODE_OPTIMIZER
static int OptimizeBytecode(ark::pandasm::Program *prog, const util::Options *options, const ReporterFun &reporter,
                            std::map<std::string, size_t> *statp,
                            ark::pandasm::AsmEmitter::PandaFileToPandaAsmMaps *mapsp)
{
    if (options->OptLevel() != 0) {
        InitializeLogging(options);
        if (!ark::pandasm::AsmEmitter::Emit(options->CompilerOutput(), *prog, statp, mapsp, true)) {
            reporter("Failed to emit binary data: " + ark::pandasm::AsmEmitter::GetLastError());
            return 1;
        }

        ark::bytecodeopt::g_options.SetOptLevel(options->OptLevel());
        // Set default value instead of maximum set in ark::bytecodeopt::SetCompilerOptions()
        ark::compiler::CompilerLogger::Init({"all"});
        ark::compiler::g_options.SetCompilerMaxBytecodeSize(ark::compiler::g_options.GetCompilerMaxBytecodeSize());
        ark::bytecodeopt::OptimizeBytecode(prog, mapsp, options->CompilerOutput(), options->IsDynamic(), true);
    }

    return 0;
}
#endif

static int GenerateProgramImpl(ark::pandasm::Program *prog, const util::Options *options, const ReporterFun &reporter,
                               std::map<std::string, size_t> *statp,
                               ark::pandasm::AsmEmitter::PandaFileToPandaAsmMaps *mapsp)
{
    if (options->CompilerOptions().dumpAsm) {
        es2panda::Compiler::DumpAsm(prog);
    }

    if (!ark::pandasm::AsmEmitter::AssignProfileInfo(prog)) {
        reporter("AssignProfileInfo failed");
        return 1;
    }

    if (!ark::pandasm::AsmEmitter::Emit(options->CompilerOutput(), *prog, statp, mapsp, true)) {
        reporter("Failed to emit binary data: " + ark::pandasm::AsmEmitter::GetLastError());
        return 1;
    }

    if (options->SizeStat()) {
        size_t totalSize = 0;
        std::cout << "Panda file size statistic:" << std::endl;
        constexpr std::array<std::string_view, 2> INFO_STATS = {"instructions_number", "codesize"};

        auto &stat = *statp;
        for (const auto &[name, size] : stat) {
            if (find(INFO_STATS.begin(), INFO_STATS.end(), name) != INFO_STATS.end()) {
                continue;
            }
            std::cout << name << " section: " << size << std::endl;
            totalSize += size;
        }

        for (const auto &name : INFO_STATS) {
            std::cout << name << ": " << stat.at(std::string(name)) << std::endl;
        }

        std::cout << "total: " << totalSize << std::endl;
    }

    return 0;
}

int GenerateProgram(ark::pandasm::Program *prog, const util::Options *options, const ReporterFun &reporter)
{
    std::map<std::string, size_t> stat;
    std::map<std::string, size_t> *statp = options->OptLevel() != 0 ? &stat : nullptr;
    ark::pandasm::AsmEmitter::PandaFileToPandaAsmMaps maps {};
    ark::pandasm::AsmEmitter::PandaFileToPandaAsmMaps *mapsp = options->OptLevel() != 0 ? &maps : nullptr;

#ifdef PANDA_WITH_BYTECODE_OPTIMIZER
    if (OptimizeBytecode(prog, options, reporter, statp, mapsp) != 0) {
        return 1;
    }
#endif
    if (GenerateProgramImpl(prog, options, reporter, statp, mapsp) != 0) {
        return 1;
    }

    return 0;
}

}  // namespace ark::es2panda::util

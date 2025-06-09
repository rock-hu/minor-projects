/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "bytecode_optimizer/bytecodeopt_options.h"
#include "bytecode_optimizer/optimize_bytecode.h"
#include "compiler/compiler_logger.h"
#include "mem/arena_allocator.h"
#include "mem/pool_manager.h"
#include "es2panda.h"
#include "util/arktsconfig.h"
#include "util/diagnosticEngine.h"
#include "util/generateBin.h"
#include "util/options.h"
#include "util/plugin.h"
#include "libpandabase/os/stacktrace.h"
#include "generated/diagnostic.h"

#include <iostream>
#include <memory>
#include <optional>
namespace ark::es2panda::aot {
using mem::MemConfig;

class MemManager {
public:
    explicit MemManager()
    {
        MemConfig::Initialize(0, 0, COMPILER_SIZE, 0, 0, 0);
        PoolManager::Initialize(PoolType::MMAP);
    }

    NO_COPY_SEMANTIC(MemManager);
    NO_MOVE_SEMANTIC(MemManager);

    ~MemManager()
    {
        PoolManager::Finalize();
        MemConfig::Finalize();
    }
};

static int CompileFromSource(es2panda::Compiler &compiler, es2panda::SourceFile &input, const util::Options &options,
                             util::DiagnosticEngine &diagnosticEngine)
{
    auto program = std::unique_ptr<pandasm::Program> {compiler.Compile(input, options, diagnosticEngine)};
    if (program == nullptr) {
        const auto &err = compiler.GetError();

        if (err.Type() == util::DiagnosticType::INVALID) {
            if (diagnosticEngine.IsAnyError()) {
                return 1;
            }
            // Intentional exit or --parse-only option usage.
            return 0;
        }
        diagnosticEngine.Log(err);
        return 1;
    }

    return util::GenerateProgram(
        program.get(), options,
        [&diagnosticEngine](const diagnostic::DiagnosticKind &kind, const util::DiagnosticMessageParams &params) {
            diagnosticEngine.LogDiagnostic(kind, params);
        });
}

static int CompileFromConfig(es2panda::Compiler &compiler, util::Options *options,
                             util::DiagnosticEngine &diagnosticEngine)
{
    auto compilationList = FindProjectSources(options->ArkTSConfig());
    if (compilationList.empty()) {
        diagnosticEngine.LogDiagnostic(diagnostic::NO_INPUT, util::DiagnosticMessageParams {});
        return 1;
    }

    unsigned overallRes = 0;
    for (auto &[src, dst] : compilationList) {
        std::ifstream inputStream(src);
        if (inputStream.fail()) {
            diagnosticEngine.LogDiagnostic(diagnostic::OPEN_FAILED, util::DiagnosticMessageParams {src});
            return 1;
        }

        std::stringstream ss;
        ss << inputStream.rdbuf();
        std::string parserInput = ss.str();
        inputStream.close();
        es2panda::SourceFile input(src, parserInput, options->IsModule());
        options->SetOutput(dst);
        LOG_IF(options->IsListFiles(), INFO, ES2PANDA)
            << "> es2panda: compiling from '" << src << "' to '" << dst << "'";

        auto res = CompileFromSource(compiler, input, *options, diagnosticEngine);
        if (res != 0) {
            diagnosticEngine.LogDiagnostic(diagnostic::COMPILE_FAILED, util::DiagnosticMessageParams {src, dst});
            overallRes |= static_cast<unsigned>(res);
        }
    }

    return overallRes;
}

static std::optional<std::vector<util::Plugin>> InitializePlugins(std::vector<std::string> const &names,
                                                                  util::DiagnosticEngine &diagnosticEngine)
{
    std::vector<util::Plugin> res;
    for (auto &name : names) {
        auto plugin = util::Plugin(util::StringView {name});
        if (!plugin.IsOk()) {
            diagnosticEngine.LogDiagnostic(diagnostic::PLUGIN_LOAD_FAILED,
                                           util::DiagnosticMessageParams {util::StringView(name)});
            return std::nullopt;
        }
        plugin.Initialize();
        res.push_back(std::move(plugin));
    }
    return {std::move(res)};
}

static int Run(Span<const char *const> args)
{
    auto diagnosticEngine = util::DiagnosticEngine();
    auto options = std::make_unique<util::Options>(args[0], diagnosticEngine);
    if (!options->Parse(args)) {
        return 1;
    }
    diagnosticEngine.SetWError(options->IsEtsWarningsWerror());
    ark::Logger::ComponentMask mask {};
    mask.set(ark::Logger::Component::ES2PANDA);
    ark::Logger::InitializeStdLogging(options->LogLevel(), mask);
    util::DiagnosticEngine::InitializeSignalHandlers();

    auto pluginsOpt = InitializePlugins(options->GetPlugins(), diagnosticEngine);
    if (!pluginsOpt.has_value()) {
        return 1;
    }

    es2panda::Compiler compiler(options->GetExtension(), options->GetThread(), std::move(pluginsOpt.value()));
    if (options->IsListPhases()) {
        std::cerr << "Available phases:" << std::endl;
        std::cerr << compiler.GetPhasesList();
        return 1;
    }

    if (options->GetCompilationMode() == CompilationMode::PROJECT) {
        return CompileFromConfig(compiler, options.get(), diagnosticEngine);
    }

    std::string sourceFile;
    std::string_view parserInput;
    if (options->GetCompilationMode() == CompilationMode::GEN_STD_LIB) {
        sourceFile = "etsstdlib.ets";
        parserInput = "";
    } else {
        sourceFile = options->SourceFileName();
        auto [buf, size] = options->CStrParserInputContents();
        parserInput = std::string_view(buf, size);
    }
    es2panda::SourceFile input(sourceFile, parserInput, options->IsModule());
    return CompileFromSource(compiler, input, *options.get(), diagnosticEngine);
}
}  // namespace ark::es2panda::aot

int main(int argc, const char *argv[])
{
    ark::es2panda::aot::MemManager mm;
    return ark::es2panda::aot::Run(ark::Span<const char *const>(argv, argc));
}

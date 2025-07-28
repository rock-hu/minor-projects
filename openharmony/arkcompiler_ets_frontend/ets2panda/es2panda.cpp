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

#include "compiler/core/compilerImpl.h"
#include "generated/diagnostic.h"
#include "public/public.h"
#include "util/diagnostic.h"
#include "util/generateBin.h"

#include "es2panda.h"

namespace ark::es2panda {
constexpr size_t DEFAULT_THREAD_COUNT = 2;

namespace util {
class Options;
}  // namespace util

template <class T>
T DirName(T const &path, T const &delims = ark::os::file::File::GetPathDelim())
{
    std::size_t pos = path.find_last_of(delims);
    if (pos == std::string::npos) {
        return "./";
    }

    if (pos == 0) {
        return delims;
    }

    std::string_view dirPath = path.substr(0, pos);
    if (dirPath.compare(".") == 0 || dirPath.compare("..") == 0) {
        return path.substr(0, pos + 1);
    }

    return path.substr(0, pos);
}

SourceFile::SourceFile(std::string_view fn, std::string_view s) : filePath(fn), fileFolder(DirName(fn)), source(s) {}

SourceFile::SourceFile(std::string_view fn, std::string_view s, bool m)
    : filePath(fn), fileFolder(DirName(fn)), source(s), isModule(m)
{
}
SourceFile::SourceFile(std::string_view fn, std::string_view s, bool m, std::string_view d)
    : filePath(fn), fileFolder(DirName(fn)), source(s), isModule(m), dest(d)
{
}

SourceFile::SourceFile(std::string_view fn, std::string_view s, std::string_view rp, bool m, bool d)
    : filePath(fn),
      fileFolder(DirName(fn)),
      source(s),
      resolvedPath(DirName(rp)),
      isModule(m),
      isDeclForDynamicStaticInterop(d)
{
}

Compiler::Compiler(ScriptExtension ext) : Compiler(ext, DEFAULT_THREAD_COUNT, {}) {}

Compiler::Compiler(ScriptExtension ext, size_t threadCount) : Compiler(ext, threadCount, {}) {}

Compiler::Compiler(ScriptExtension ext, size_t threadCount, std::vector<util::Plugin> &&plugins)
    : plugins_(std::move(plugins)), compiler_(new compiler::CompilerImpl(threadCount, &plugins_)), ext_(ext)
{
}

Compiler::~Compiler()
{
    delete compiler_;
}

pandasm::Program *Compiler::Compile(const SourceFile &input, const util::Options &options,
                                    util::DiagnosticEngine &diagnosticEngine, uint32_t parseStatus)
{
    public_lib::Context context;
    ArenaAllocator allocator(SpaceType::SPACE_TYPE_COMPILER, nullptr, true);
    context.allocator = &allocator;
    context.compilingState = public_lib::CompilingState::SINGLE_COMPILING;

    try {
        return compiler_->Compile(compiler::CompilationUnit {input, options, parseStatus, ext_, diagnosticEngine},
                                  &context);
    } catch (const util::ThrowableDiagnostic &e) {
        error_ = e;
        return nullptr;
    }
}

unsigned int Compiler::CompileM(std::vector<SourceFile> &inputs, util::Options &options,
                                util::DiagnosticEngine &diagnosticEngine, std::vector<pandasm::Program *> &result)
{
    public_lib::Context context;
    context.transitionMemory =
        new public_lib::TransitionMemory(new ArenaAllocator(SpaceType::SPACE_TYPE_COMPILER, nullptr, true));
    context.allocator = context.transitionMemory->PermanentAllocator();

    context.compilingState = public_lib::CompilingState::MULTI_COMPILING_INIT;
    unsigned int overallRes = 0;
    for (auto &input : inputs) {
        try {
            options.SetOutput(std::string(input.dest));
            LOG_IF(options.IsListFiles(), INFO, ES2PANDA)
                << "> es2panda: compiling from '" << input.filePath << "' to '" << input.dest << "'";
            auto program =
                compiler_->Compile(compiler::CompilationUnit {input, options, 0, ext_, diagnosticEngine}, &context);
            result.push_back(program);
        } catch (const util::ThrowableDiagnostic &err) {
            overallRes |= 1U;
            diagnosticEngine.Log(err);
        }
        context.compilingState = public_lib::CompilingState::MULTI_COMPILING_FOLLOW;
    }
    delete context.transitionMemory;
    return overallRes;
}

std::string Compiler::GetPhasesList() const
{
    return compiler::CompilerImpl::GetPhasesList(ext_);
}

void Compiler::DumpAsm(const pandasm::Program *prog)
{
    compiler::CompilerImpl::DumpAsm(prog);
}

util::DiagnosticEngine *g_diagnosticEngine = nullptr;

}  // namespace ark::es2panda

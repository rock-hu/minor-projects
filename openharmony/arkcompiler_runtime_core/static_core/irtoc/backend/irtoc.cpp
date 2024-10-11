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
#include "compilation.h"
#include "compiler_options.h"
#include "irtoc_options.h"
#include "generated/base_options.h"

namespace ark::irtoc {

int Run(int argc, const char **argv)
{
    ark::PandArgParser paParser;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    ark::irtoc::g_options.AddOptions(&paParser);
    base_options::Options baseOptions("");
    baseOptions.AddOptions(&paParser);
    ark::compiler::g_options.AddOptions(&paParser);
#ifdef PANDA_LLVM_IRTOC
    ark::llvmbackend::g_options.AddOptions(&paParser);
#endif
    if (!paParser.Parse(argc, argv)) {
        std::cerr << "Error: " << paParser.GetErrorString() << "\n";
        return -1;
    }

    Logger::Initialize(baseOptions);

    ark::compiler::CompilerLogger::SetComponents(ark::compiler::g_options.GetCompilerLog());

    if (std::getenv("IRTOC_VERBOSE") != nullptr) {
        Logger::SetLevel(Logger::Level::DEBUG);
        Logger::ResetComponentMask();
        Logger::EnableComponent(Logger::Component::IRTOC);
    }

    auto res = Compilation().Run();
    if (!res.HasValue()) {
        std::cerr << res.Error() << std::endl;
        return 1;
    }
    return 0;
}

}  // namespace ark::irtoc

int main(int argc, const char **argv)
{
    return ark::irtoc::Run(argc, argv);
}

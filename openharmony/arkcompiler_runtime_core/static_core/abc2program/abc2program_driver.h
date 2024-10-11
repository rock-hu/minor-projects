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

#ifndef ABC2PROGRAM_ABC2PROGRAM_DRIVER_H
#define ABC2PROGRAM_ABC2PROGRAM_DRIVER_H

#include <string>
#include <assembly-program.h>
#include "abc2program_compiler.h"

namespace ark::abc2program {

class Abc2ProgramDriver {
public:
    int Run(int argc, const char **argv);
    bool Run(const std::string &inputFilePath, const std::string &outputFilePath);
    bool Compile(const std::string &inputFilePath);
    const pandasm::Program &GetProgram() const;
    pandasm::Program &GetProgram();

private:
    bool Dump(const std::string &outputFilePath);
    bool Compile(const std::string &inputFilePath, pandasm::Program &program);
    Abc2ProgramCompiler compiler_;
    pandasm::Program program_;
};  // class Abc2ProgramDriver

}  // namespace ark::abc2program

#endif  // ABC2PROGRAM_ABC2PROGRAM_DRIVER_H

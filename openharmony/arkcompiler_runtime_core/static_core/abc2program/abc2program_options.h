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

#ifndef ABC2PROGRAM_ABC2PROGRAM_OPTIONS_H
#define ABC2PROGRAM_ABC2PROGRAM_OPTIONS_H

#include <string>
#include "utils/pandargs.h"
#include "utils/logger.h"

namespace ark::abc2program {

class Abc2ProgramOptions {
public:
    bool Parse(int argc, const char **argv);
    const std::string &GetInputFilePath() const;
    const std::string &GetOutputFilePath() const;

private:
    bool ProcessArgs(int argc, const char **argv);
    void ConstructErrorMsg();
    void PrintErrorMsg() const;
    ark::PandArgParser paParser_;
    std::string inputFilePath_;
    std::string outputFilePath_;
    std::string errorMsg_;
    ark::PandArg<bool> *helpArg_ = nullptr;
    ark::PandArg<bool> *debugArg_ = nullptr;
    ark::PandArg<std::string> *debugFileArg_ = nullptr;
    ark::PandArg<std::string> *inputFileArg_ = nullptr;
    ark::PandArg<std::string> *outputFileArg_ = nullptr;
};  // class Abc2ProgramOptions

}  // namespace ark::abc2program

#endif  // ABC2PROGRAM_ABC2PROGRAM_OPTIONS_H
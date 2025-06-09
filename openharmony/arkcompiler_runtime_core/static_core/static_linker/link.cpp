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

#include <algorithm>

#include "libpandabase/os/file.h"
#include "libpandabase/utils/pandargs.h"
#include "libpandabase/utils/logger.h"
#include "generated/link_options.h"

#include "linker.h"

namespace {

int PrintHelp(const ark::PandArgParser &paParser)
{
    const auto a = paParser.GetErrorString();
    if (!a.empty()) {
        std::cerr << "Error: " << a << std::endl;
    }

    std::cerr << "Usage:" << std::endl;
    std::cerr << "ark_link [OPTIONS] -- FILES..." << std::endl << std::endl;
    std::cerr << "Supported options:" << std::endl << std::endl;
    std::cerr << paParser.GetHelpString() << std::endl;
    return 1;
}

std::string MangleClass(std::string s)
{
    s.insert(s.begin(), 'L');
    s += ";";
    return s;
}

bool SplitContentByLine(const std::string &input, std::vector<std::string> &lines)
{
    std::ifstream ifs;
    std::string line;
    ifs.open(input);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open source list file '" << input << "' during input file collection." << std::endl
                  << "Please check if the file exists or the path is correct, "
                  << "and you have the necessary permissions to access it." << std::endl;
        return false;
    }

    while (std::getline(ifs, line)) {
        lines.push_back(line);
    }
    ifs.close();
    return true;
}

bool CollectAbcFiles(const std::vector<std::string> &files, std::vector<std::string> &abcFiles)
{
    for (const auto &file : files) {
        if (file.length() > 0 && file[0] == '@') {
            auto inputAbs = ark::os::file::File::GetAbsolutePath(file.substr(1));
            if (!inputAbs) {
                std::cerr << "Failed to find file '" << file.substr(1) << "' during input file resolution" << std::endl
                          << "Please check if the file name is correct, the file exists at the specified path, "
                          << "and your project has the necessary permissions to access it." << std::endl;
                return false;
            }
            auto fpath = inputAbs.Value();
            std::vector<std::string> lines;
            if (!SplitContentByLine(fpath, lines)) {
                return false;
            }

            std::move(lines.begin(), lines.end(), std::back_inserter(abcFiles));
        } else {
            abcFiles.push_back(file);
        }
    }
    return true;
}

}  // namespace

int main(int argc, const char *argv[])
{
    ark::PandArgParser paParser;
    ark::static_linker::Options options {*argv};
    options.AddOptions(&paParser);
    paParser.EnableRemainder();

    if (!paParser.Parse(argc, argv)) {
        return PrintHelp(paParser);
    }

    ark::Logger::InitializeStdLogging(
        ark::Logger::LevelFromString(options.GetLogLevel()),
        ark::Logger::ComponentMask().set(ark::Logger::Component::STATIC_LINKER).set(ark::Logger::Component::PANDAFILE));

    const auto files = paParser.GetRemainder();
    std::vector<std::string> abcFiles;
    if (!CollectAbcFiles(files, abcFiles)) {
        return 1;
    }
    if (abcFiles.empty()) {
        std::cerr << "must have at least one file" << std::endl;
        return PrintHelp(paParser);
    }

    if (options.GetOutput().empty()) {
        auto const &fn = abcFiles[0];
        options.SetOutput(fn.substr(0, fn.find_last_of('.')) + ".linked.abc");
    }

    auto conf = ark::static_linker::DefaultConfig();

    conf.stripDebugInfo = options.IsStripDebugInfo();

    auto classesVecToSet = [](const std::vector<std::string> &v, std::set<std::string> &s) {
        s.clear();
        std::transform(v.begin(), v.end(), std::inserter(s, s.begin()), MangleClass);
    };

    classesVecToSet(options.GetPartialClasses(), conf.partial);
    classesVecToSet(options.GetRemainsPartialClasses(), conf.remainsPartial);

    auto res = ark::static_linker::Link(conf, options.GetOutput(), abcFiles);

    size_t i = 0;
    for (const auto &s : res.errors) {
        std::cerr << "# " << ++i << "\n";
        std::cerr << s << std::endl;
    }

    if (options.IsShowStats()) {
        std::cout << "stats:\n" << res.stats << std::endl;
    }

    const auto wasError = !res.errors.empty();
    return static_cast<int>(wasError);
}

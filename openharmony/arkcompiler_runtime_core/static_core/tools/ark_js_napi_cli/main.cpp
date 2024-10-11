/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <csignal>  // NOLINTNEXTLINE(modernize-deprecated-headers)
#include <vector>

#include "js_runtime.h"

namespace panda::ecmascript {
int Main(const int argc, const char **argv)
{
    auto runtime = panda::JsRuntime::Create();

    arg_list_t filenames;
    if (!runtime->ProcessOptions(argc, argv, &filenames)) {
        return 1;
    }

    if (!runtime->Init()) {
        std::cerr << "Cannot initialize js runtime" << std::endl;
        return -1;
    }

    bool ret = true;
    for (const auto &filename : filenames) {
        auto res = runtime->Execute(filename);
        if (!res) {
            std::cerr << "Cannot execute panda file '" << filename << "'" << std::endl;
            ret = false;
            break;
        }
    }

    runtime->Loop();

    return ret ? 0 : -1;
}
}  // namespace panda::ecmascript

int main(int argc, const char **argv)
{
    return panda::ecmascript::Main(argc, argv);
}

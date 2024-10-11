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

#include "utils/utils.h"

#include <iostream>
#include <iterator>
#include <fstream>

namespace panda::utils {

void GetAsset(const std::string &uri, std::vector<uint8_t> &content, std::string &ami)
{
    size_t index = uri.find_last_of(".");
    if (index == std::string::npos) {
        std::cerr << "Invalid uri: " << uri << std::endl;
        return;
    }

    auto path = uri.substr(0, index) + ".abc";
    std::ifstream fs(path, std::ios::binary);
    if (!fs) {
        std::cerr << "Cannot read file: " << path << std::endl;
        return;
    }

    fs.unsetf(std::ios::skipws);

    std::streampos size;
    fs.seekg(0, std::ios::end);
    size = fs.tellg();
    fs.seekg(0, std::ios::beg);

    content.reserve(size);

    content.insert(content.begin(), std::istream_iterator<uint8_t>(fs), std::istream_iterator<uint8_t>());

    ami = path;
}

}  // namespace panda::utils

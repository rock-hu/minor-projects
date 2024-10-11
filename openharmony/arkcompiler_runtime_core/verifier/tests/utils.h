/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <unordered_map>

namespace panda::verifier {
    void GenerateModifiedAbc(const std::vector<unsigned char> &buffer, const std::string &filename);
    void ConvertToLittleEndian(std::vector<unsigned char> &inner_literal_id, const uint32_t &id);
    void ModifyBuffer(std::unordered_map<uint32_t, uint32_t> &inner_literal_map, std::vector<unsigned char> &buffer);
} // namespace panda::verifier

#endif

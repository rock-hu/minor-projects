/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef ASSEMBLER_ASSEMBLY_DEBUG_H
#define ASSEMBLER_ASSEMBLY_DEBUG_H

#include <string>

namespace panda::pandasm::debuginfo {

struct Ins {
    size_t line_number = 0;
    uint32_t column_number = 0;
    std::string whole_line = "";  // TODO(mbolshov): redundant given file and line_number
    size_t bound_left = 0;
    size_t bound_right = 0;

    void SetLineNumber(size_t ln)
    {
        line_number = ln;
    }

    void SetColumnNumber(size_t cn)
    {
        column_number = cn;
    }

    Ins() = default;
    Ins(size_t l_n, std::string &f_c, size_t b_l, size_t b_r)
        : line_number(l_n), whole_line(std::move(f_c)), bound_left(b_l), bound_right(b_r)
    {
    }
};

struct LocalVariable {
    std::string name;
    std::string signature;
    std::string signature_type;
    int32_t reg = 0;
    uint32_t start = 0;
    uint32_t length = 0;
};

}  // namespace panda::pandasm::debuginfo

#endif  // ASSEMBLER_ASSEMBLY_DEBUG_H

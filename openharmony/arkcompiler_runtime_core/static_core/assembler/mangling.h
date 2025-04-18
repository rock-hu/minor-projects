/*
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

#ifndef PANDA_ASSEMBLER_MANGLING_H
#define PANDA_ASSEMBLER_MANGLING_H

#include "assembly-function.h"

#include <string>
#include <vector>

namespace ark::pandasm {
constexpr const std::string_view MANGLE_BEGIN = ":";
constexpr const std::string_view MANGLE_SEPARATOR = ";";

constexpr const std::string_view SIGNATURE_BEGIN = MANGLE_BEGIN;
constexpr const std::string_view SIGNATURE_TYPES_SEPARATOR = ",";
constexpr const std::string_view SIGNATURE_TYPES_SEPARATOR_L = "(";
constexpr const std::string_view SIGNATURE_TYPES_SEPARATOR_R = ")";

inline std::string DeMangleName(const std::string &name)
{
    auto iter = name.find_first_of(MANGLE_BEGIN);
    if (iter != std::string::npos) {
        return name.substr(0, name.find_first_of(MANGLE_BEGIN));
    }
    return name;
}

inline std::string MangleFunctionName(const std::string &name, const std::vector<pandasm::Function::Parameter> &params,
                                      const pandasm::Type &returnType)
{
    std::string mangleName {name};
    mangleName += MANGLE_BEGIN;
    for (const auto &p : params) {
        mangleName += p.type.GetName() + std::string(MANGLE_SEPARATOR);
    }
    mangleName += returnType.GetName() + std::string(MANGLE_SEPARATOR);

    return mangleName;
}

inline std::string MangleFieldName(const std::string &name, const pandasm::Type &type)
{
    std::string mangleName {name};
    mangleName += MANGLE_BEGIN;
    mangleName += type.GetName() + std::string(MANGLE_SEPARATOR);
    return mangleName;
}

inline std::string GetFunctionSignatureFromName(std::string name,
                                                const std::vector<pandasm::Function::Parameter> &params)
{
    name += SIGNATURE_BEGIN;
    name += SIGNATURE_TYPES_SEPARATOR_L;

    bool first = true;
    for (const auto &p : params) {
        name += (first) ? "" : SIGNATURE_TYPES_SEPARATOR;
        first = false;
        name += p.type.GetPandasmName();
    }

    name += SIGNATURE_TYPES_SEPARATOR_R;

    return name;
}

inline std::string GetFunctionNameFromSignature(const std::string &sig)
{
    return DeMangleName(sig);
}

inline bool IsSignatureOrMangled(const std::string &str)
{
    return str.find(SIGNATURE_BEGIN) != std::string::npos;
}

}  // namespace ark::pandasm

#endif  // PANDA_ASSEMBLER_MANGLING_H

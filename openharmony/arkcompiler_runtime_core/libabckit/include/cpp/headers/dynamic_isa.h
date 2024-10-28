/*
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

#ifndef CPP_ABCKIT_DYNAMIC_ISA_H
#define CPP_ABCKIT_DYNAMIC_ISA_H

#include "cpp/headers/config.h"
#include "cpp/headers/declarations.h"
#include "cpp/headers/instruction.h"
#include <memory>

namespace abckit {

// Third type of Entity? Or just a view?
class DynamicIsa final {
    // To access private constructor.
    // We restrict constructors in order to prevent C/C++ API mix-up by user.
    friend class Graph;

public:
    DynamicIsa(const DynamicIsa &other) = delete;
    DynamicIsa &operator=(const DynamicIsa &other) = delete;
    DynamicIsa(DynamicIsa &&other) = delete;
    DynamicIsa &operator=(DynamicIsa &&other) = delete;
    ~DynamicIsa() = default;

    // Rvalue annotated so we can call it only in callchain context
    Instruction CreateLoadString(const std::string &str) &&;
    Instruction CreateTryldglobalbyname(const std::string &str) &&;
    Instruction CreateCallArg1(const Instruction &acc, const Instruction &input0) &&;

    // Other dynamic API methods declarations

private:
    explicit DynamicIsa(const Graph &graph) : graph_(graph) {};
    const Graph &graph_;
};

}  // namespace abckit

#endif  // CPP_ABCKIT_DYNAMIC_ISA_H

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

#ifndef PANDA_GUARD_OBFUSCATE_PROGRAM_H
#define PANDA_GUARD_OBFUSCATE_PROGRAM_H

#include "node.h"

namespace panda::guard {

class Program final : public IEntity {
public:
    explicit Program(pandasm::Program *program) : prog_(program) {}

    void Create() override;

    void Obfuscate() override;

private:
    void ForEachFunction(const std::function<FunctionTraver> &callback);

    void RemoveConsoleLog();

    void UpdateReference();

public:
    pandasm::Program *prog_ = nullptr;
    std::map<std::string, std::shared_ptr<Node>> nodeTable_ {};
};

}  // namespace panda::guard

#endif  // PANDA_GUARD_OBFUSCATE_PROGRAM_H

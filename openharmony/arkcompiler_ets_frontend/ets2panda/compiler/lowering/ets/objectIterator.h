/*
 * Copyright (c) 2021 - 2025 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_COMPILER_LOWERING_OBJECT_ITERATOR_H
#define ES2PANDA_COMPILER_LOWERING_OBJECT_ITERATOR_H

#include "compiler/lowering/phase.h"

namespace ark::es2panda::parser {
class ETSParser;
}  // namespace ark::es2panda::parser

namespace ark::es2panda::compiler {

class ObjectIteratorLowering : public PhaseForBodies {
public:
    std::string_view Name() const override;
    bool PerformForModule(public_lib::Context *ctx, parser::Program *program) override;

private:
    [[nodiscard]] ir::Statement *ProcessObjectIterator(public_lib::Context *ctx,
                                                       ir::ForOfStatement *forOfStatement) const;

    void TransferForOfLoopBody(ir::Statement *forBody, ir::BlockStatement *whileBody) const noexcept;
};

}  // namespace ark::es2panda::compiler

#endif

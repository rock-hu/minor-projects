/*
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

#ifndef ES2PANDA_COMPILER_IR_MODULERECORD_EMITTER_H
#define ES2PANDA_COMPILER_IR_MODULERECORD_EMITTER_H

#include <assembly-literals.h>
#include <parser/module/sourceTextModuleRecord.h>

#include <vector>

namespace panda::es2panda::compiler {
class ModuleRecordEmitter {
public:
    explicit ModuleRecordEmitter(parser::SourceTextModuleRecord *moduleRecord, int32_t bufferIdx,
                                 int32_t phaseBufferIdx)
        : moduleRecord_(moduleRecord), bufferIdx_(bufferIdx), phaseBufferIdx_(phaseBufferIdx) {}
    ~ModuleRecordEmitter() = default;
    NO_COPY_SEMANTIC(ModuleRecordEmitter);
    NO_MOVE_SEMANTIC(ModuleRecordEmitter);

    int32_t Index() const
    {
        return bufferIdx_;
    }

    int32_t PhaseIndex() const
    {
        return phaseBufferIdx_;
    }

    auto &Buffer()
    {
        return buffer_;
    }

    auto &PhaseBuffer()
    {
        return phaseBuffer_;
    }

    std::unordered_set<uint32_t> &GetConstantLocalExportSlots()
    {
        return constant_local_export_slots_;
    }

    bool NeedEmitPhaseRecord() const
    {
        return phaseBufferIdx_ != -1;
    }

    void Generate();

private:
    void GenModuleRequests();
    void GenRegularImportEntries();
    void GenNamespaceImportEntries();
    void GenLocalExportEntries();
    void GenIndirectExportEntries();
    void GenStarExportEntries();

    parser::SourceTextModuleRecord *moduleRecord_;
    int32_t bufferIdx_ {};
    int32_t phaseBufferIdx_ {};
    std::vector<panda::pandasm::LiteralArray::Literal> buffer_;
    std::vector<panda::pandasm::LiteralArray::Literal> phaseBuffer_;
    std::unordered_set<uint32_t> constant_local_export_slots_;
};
}  // namespace panda::es2panda::compiler

#endif

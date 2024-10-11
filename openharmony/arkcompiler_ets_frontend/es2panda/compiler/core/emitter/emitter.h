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

#ifndef ES2PANDA_COMPILER_IR_EMITTER_H
#define ES2PANDA_COMPILER_IR_EMITTER_H

#include <assembly-literals.h>
#include <compiler/core/emitter/moduleRecordEmitter.h>
#include <ir/astNode.h>
#include <lexer/token/sourceLocation.h>
#include <macros.h>
#include <util/patchFix.h>
#include <util/ustring.h>

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace panda::pandasm {
struct Program;
struct Function;
struct Ins;
struct Record;
}  // namespace panda::pandasm

namespace panda::es2panda::ir {
class Statement;
}  // namespace panda::es2panda::ir

namespace panda::es2panda::binder {
class Scope;
}  // namespace panda::es2panda::binder

namespace panda::es2panda::compiler {
class PandaGen;
class LiteralBuffer;
class DebugInfo;
class Label;
class IRNode;
class CompilerContext;

using Literal = panda::pandasm::LiteralArray::Literal;

class FunctionEmitter {
public:
    explicit FunctionEmitter(ArenaAllocator *allocator, const PandaGen *pg);
    ~FunctionEmitter() = default;
    NO_COPY_SEMANTIC(FunctionEmitter);
    NO_MOVE_SEMANTIC(FunctionEmitter);

    panda::pandasm::Function *Function()
    {
        return func_;
    }

    auto &LiteralBuffers()
    {
        return literalBuffers_;
    }

    auto &LiteralArrays()
    {
        return literalArrays_;
    }

    auto &ExternalAnnotationRecords()
    {
        return externalAnnotationRecords_;
    }

    void Generate(util::PatchFix *patchFixHelper);
    const ArenaSet<util::StringView> &Strings() const;

private:
    uint32_t UpdateForReturnIns(const ir::AstNode *astNode, panda::pandasm::Ins *pandaIns);
    void GenInstructionDebugInfo(const IRNode *ins, panda::pandasm::Ins *pandaIns);
    void GenFunctionInstructions();
    void GenFunctionCatchTables();
    void GenScopeVariableInfo(const binder::Scope *scope);
    void GenSourceFileDebugInfo();
    void GenVariablesDebugInfo();
    void GenFunctionKind();
    void GenIcSize();
    pandasm::AnnotationElement CreateAnnotationElement(const std::string &propName, const ir::Expression *initValue);
    pandasm::AnnotationData CreateAnnotation(const ir::Annotation *anno);
    void GenAnnotations();
    util::StringView SourceCode() const;
    lexer::LineIndex &GetLineIndex() const;

    void GenLiteralBuffers();
    void GenBufferLiterals(const LiteralBuffer *buff);
    void GenConcurrentFunctionModuleRequests();

    const PandaGen *pg_;
    panda::pandasm::Function *func_ {};
    ArenaVector<std::pair<int32_t, std::vector<Literal>>> literalBuffers_;
    size_t offset_ {0};

    ArenaVector<std::pair<std::string, std::vector<Literal>>> literalArrays_;
    ArenaVector<panda::pandasm::Record> externalAnnotationRecords_;
};

class Emitter {
public:
    explicit Emitter(CompilerContext *context);
    ~Emitter();
    NO_COPY_SEMANTIC(Emitter);
    NO_MOVE_SEMANTIC(Emitter);

    void AddAnnotationRecord(const std::string &annoName, const ir::ClassDeclaration *classDecl);
    void AddFunction(FunctionEmitter *func, CompilerContext *context);
    void AddSourceTextModuleRecord(ModuleRecordEmitter *module, CompilerContext *context);
    void AddScopeNamesRecord(CompilerContext *context);
    static void GenBufferLiterals(ArenaVector<std::pair<int32_t, std::vector<Literal>>> &literalBuffers,
                                  const LiteralBuffer *buff);
    static void DumpAsm(const panda::pandasm::Program *prog);
    panda::pandasm::Program *Finalize(bool dumpDebugInfo, util::PatchFix *patchFixHelper);
    panda::pandasm::Program *GetProgram() const;
    void GenJsonContentRecord(const CompilerContext *context);
    void GenRecordNameInfo() const;
    std::mutex &GetEmitterLock()
    {
        return m_;
    };

    const std::unordered_set<uint32_t> &GetConstantLocalExportSlots()
    {
        return constant_local_export_slots_;
    }

    static std::vector<std::pair<std::string, std::vector<Literal>>> CreateLiteralArray(const ir::Expression *array,
                                                                                        const std::string &baseName);

private:
    void CreateStringClass();
    panda::pandasm::Type DeduceArrayEnumType(const ir::Expression *value, uint8_t rank, bool &needToCreateArrayValue);

    void SetCommonjsField(bool isCommonjs);
    void SetPkgNameField(const std::string &pkgName);
    void GenCommonjsRecord() const;
    void AddHasTopLevelAwaitRecord(bool hasTLA, const CompilerContext *context);
    void AddSharedModuleRecord(const CompilerContext *context);
    void AddModuleRequestPhaseRecord(ModuleRecordEmitter *module, CompilerContext *context);

    void CreateEnumProp(const ir::ClassProperty *prop, const std::string &annoName, panda::pandasm::Field &annoProp);
    void CreateLiteralArrayProp(const ir::ClassProperty *prop, const std::string &annoName,
                                panda::pandasm::Field &annoProp);
    panda::pandasm::Field CreateAnnotationProp(const ir::ClassProperty *prop, const std::string &annoName);

    std::mutex m_;
    panda::pandasm::Program *prog_;
    panda::pandasm::Record *rec_;
    // Constant local export module variable slots.
    // Used by the branch elimination optimization to identify initial constants.
    // Should be replaced by analyzing the stconstmodulevar instruction in the
    // bytecode file after it is introduced
    std::unordered_set<uint32_t> constant_local_export_slots_;
};
}  // namespace panda::es2panda::compiler

#endif

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_UTIL_PATCHFIX_H
#define ES2PANDA_UTIL_PATCHFIX_H

#include <assembly-function.h>
#include <assembly-ins.h>
#include <assembly-literals.h>
#include <assembly-program.h>
#include <libpandabase/utils/arena_containers.h>
#include <util/helpers.h>
#include <util/symbolTable.h>

#include <mutex>

namespace panda::es2panda::binder {
class VariableScope;
}  // namespace panda::es2panda::binder

namespace panda::es2panda::compiler {
class PandaGen;
}  // namespace panda::es2panda::compiler

namespace panda::es2panda::util {

enum class PatchFixKind { DUMPSYMBOLTABLE, HOTFIX, COLDFIX, HOTRELOAD, COLDRELOAD };

class PatchFix {
    using LiteralBuffers = ArenaVector<std::pair<int32_t, std::vector<panda::pandasm::LiteralArray::Literal>>>;

public:
    PatchFix(bool generateSymbolFile, bool generatePatch, PatchFixKind patchFixKind, const std::string &recordName,
        util::SymbolTable *symbolTable)
        : generateSymbolFile_(generateSymbolFile), generatePatch_(generatePatch), patchFixKind_(patchFixKind),
        recordName_(recordName),
        symbolTable_(symbolTable),
        allocator_(SpaceType::SPACE_TYPE_COMPILER, nullptr, true),
        topScopeLexEnvs_(allocator_.Adapter()),
        patchFuncNames_(allocator_.Adapter()),
        newFuncNames_(allocator_.Adapter()),
        funcDefineIns_(allocator_.Adapter()),
        modifiedClassNames_(allocator_.Adapter()),
        classMemberFunctions_(allocator_.Adapter()),
        funcDefinedClasses_(allocator_.Adapter()),
        targetApiVersion_(symbolTable->GetTargetApiVersion()),
        targetApiSubVersion_(symbolTable->GetTargetApiSubVersion()) {
            originFunctionInfo_ = symbolTable_->GetOriginFunctionInfo();
            originModuleInfo_ = symbolTable_->GetOriginModuleInfo();
            originRecordHashFunctionNames_ = symbolTable_->GetOriginRecordHashFunctionNames();
            patchMain0_ = recordName_ + ".patch_main_0";
            patchMain1_ = recordName_ + ".patch_main_1";
            funcMain0_ = recordName_ + ".func_main_0";
        }

    void Finalize(panda::pandasm::Program **prog);
    bool IsScopeValidToPatchLexical(binder::VariableScope *scope) const;
    uint32_t GetSlotIdFromSymbolTable(const std::string &variableName);
    uint32_t GetEnvSizeOfFuncMain0();
    void AllocSlotfromPatchEnv(const std::string &variableName);
    uint32_t GetPatchLexicalIdx(const std::string &variableName);
    bool IsAdditionalVarInPatch(uint32_t slot);
    void ProcessFunction(const compiler::PandaGen *pg, panda::pandasm::Function *func, LiteralBuffers &literalBuffers);
    void ProcessModule(const std::string &recordName, std::vector<panda::pandasm::LiteralArray::Literal> &moduleBuffer);
    void ProcessJsonContentRecord(const std::string &recordName, const std::string &jsonFileContent);
    void CheckAndRestoreSpecialFunctionName(uint32_t globalIndexForSpecialFunc, std::string &funcName,
        std::string recordName);
    bool IsDumpSymbolTable() const;
    bool IsHotFix() const;
    bool IsColdFix() const;
    bool IsHotReload() const;
    bool IsColdReload() const;

private:
    void DumpFunctionInfo(const compiler::PandaGen *pg, panda::pandasm::Function *func, LiteralBuffers &literalBuffers);
    void HandleFunction(const compiler::PandaGen *pg, panda::pandasm::Function *func, LiteralBuffers &literalBuffers);
    void CollectFunctionsWithDefinedClasses(std::string funcName, std::string className);
    std::vector<std::pair<std::string, std::string>> GenerateFunctionAndClassHash(panda::pandasm::Function *func,
        LiteralBuffers &literalBuffers);
    void DumpModuleInfo(const std::string &recordName,
        std::vector<panda::pandasm::LiteralArray::Literal> &moduleBuffer);
    void ValidateModuleInfo(const std::string &recordName,
        std::vector<panda::pandasm::LiteralArray::Literal> &moduleBuffer);
    void DumpJsonContentRecInfo(const std::string &recordName, const std::string &jsonFileContent);
    void ValidateJsonContentRecInfo(const std::string &recordName, const std::string &jsonFileContent);

    std::string ExpandLiteral(int64_t bufferIdx, LiteralBuffers &literalBuffers);
    std::string ConvertLiteralToString(std::vector<panda::pandasm::LiteralArray::Literal> &literalBuffer);
    void CollectFuncDefineIns(panda::pandasm::Function *func);
    void AddHeadAndTailInsForPatchFuncMain0(std::vector<panda::pandasm::Ins> &ins);
    void AddTailInsForPatchFuncMain1(std::vector<panda::pandasm::Ins> &ins);
    void CreateFunctionPatchMain0AndMain1(panda::pandasm::Function &patchFuncMain0,
        panda::pandasm::Function &patchFuncMain1);
    bool IsAnonymousOrSpecialOrDuplicateFunction(const std::string &funcName);
    bool CompareLexenv(const std::string &funcName, const compiler::PandaGen *pg,
        SymbolTable::OriginFunctionInfo &bytecodeInfo);
    bool CompareClassHash(std::vector<std::pair<std::string, std::string>> &hashList,
        SymbolTable::OriginFunctionInfo &bytecodeInfo);
    void CollectClassMemberFunctions(const std::string &className, int64_t bufferIdx, LiteralBuffers &literalBuffers);
    std::vector<std::string> GetLiteralMethods(int64_t bufferIdx, LiteralBuffers &literalBuffers);
    void HandleModifiedClasses(panda::pandasm::Program *prog);
    void HandleModifiedDefinedClassFunc(panda::pandasm::Program *prog);
    int64_t GetLiteralIdxFromStringId(const std::string &stringId);

    std::mutex m_;
    uint32_t topScopeIdx_ {0};
    bool patchError_ {false};
    bool generateSymbolFile_ {false};
    bool generatePatch_ {false};
    PatchFixKind patchFixKind_;
    std::string recordName_;
    std::string funcMain0_;
    std::string patchMain0_;  // stores newly added function define ins, runtime will execute
    std::string patchMain1_;  // stores modified function and class define ins, runtime will scan but not execute

    util::SymbolTable* symbolTable_ {nullptr};
    ArenaAllocator allocator_;
    ArenaUnorderedMap<std::string, util::SymbolTable::OriginFunctionInfo> *originFunctionInfo_ {nullptr};
    ArenaUnorderedMap<std::string, std::string> *originModuleInfo_ {nullptr};
    ArenaUnorderedMap<std::string, std::unordered_map<std::string, std::string>> *originRecordHashFunctionNames_ {
        nullptr};
    ArenaUnorderedMap<std::string, uint32_t> topScopeLexEnvs_;
    ArenaSet<std::string> patchFuncNames_;
    ArenaSet<std::string> newFuncNames_;
    ArenaVector<panda::pandasm::Ins> funcDefineIns_;
    ArenaSet<std::string> modifiedClassNames_;
    ArenaUnorderedMap<std::string, std::vector<std::string>> classMemberFunctions_;
    ArenaUnorderedMap<std::string, std::vector<std::string>> funcDefinedClasses_;
    int targetApiVersion_ {0};
    std::string targetApiSubVersion_;
};

} // namespace panda::es2panda::util
#endif // ES2PANDA_UTIL_PATCHFIX_H

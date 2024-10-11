/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "sourceTextModuleRecord.h"
#include <binder/scope.h>

namespace panda::es2panda::parser {
    int SourceTextModuleRecord::AddModuleRequest(const ModuleRequestRecord record)
    {
        ASSERT(!record.source_.Empty());
        hasLazyImport_ = hasLazyImport_ || record.isLazy_;

        int moduleRequestsSize = static_cast<int>(moduleRequestsMap_.size());
        if (moduleRequestsMap_.find(record) == moduleRequestsMap_.end()) {
            moduleRequests_.emplace_back(record);
        }
        auto insertedRes = moduleRequestsMap_.insert(std::make_pair(record, moduleRequestsSize));
        return insertedRes.first->second;
    }

    // import x from 'test.js'
    // import {x} from 'test.js'
    // import {x as y} from 'test.js'
    // import defaultExport from 'test.js'
    void SourceTextModuleRecord::AddImportEntry(SourceTextModuleRecord::ImportEntry *entry)
    {
        CHECK_NOT_NULL(entry);
        ASSERT(entry != nullptr);
        ASSERT(!entry->importName_.Empty());
        ASSERT(!entry->localName_.Empty());
        ASSERT(entry->moduleRequestIdx_ != -1);
        regularImportEntries_.insert(std::make_pair(entry->localName_, entry));
        // the implicit indirect exports should be insert into indirectExportsEntries
        // when add an ImportEntry.
        // e.g. export { x }; import { x } from 'test.js'
        CheckImplicitIndirectExport(entry);
    }

    // import * as x from 'test.js'
    void SourceTextModuleRecord::AddStarImportEntry(SourceTextModuleRecord::ImportEntry *entry)
    {
        ASSERT(!entry->localName_.Empty());
        ASSERT(entry->importName_.Empty());
        ASSERT(entry->moduleRequestIdx_ != -1);
        namespaceImportEntries_.push_back(entry);
    }

    // export {x}
    // export {x as y}
    // export VariableStatement
    // export Declaration
    // export default ...
    bool SourceTextModuleRecord::AddLocalExportEntry(SourceTextModuleRecord::ExportEntry *entry)
    {
        CHECK_NOT_NULL(entry);
        ASSERT(entry->importName_.Empty());
        ASSERT(!entry->localName_.Empty());
        ASSERT(!entry->exportName_.Empty());
        ASSERT(entry->moduleRequestIdx_ == -1);

        // the implicit indirect exports should be insert into indirectExportsEntries
        // when add an ExportEntry.
        // e.g. import { x } from 'test.js'; export { x }
        if (CheckImplicitIndirectExport(entry)) {
            return true;
        }
        if (!HasDuplicateExport(entry->exportName_)) {
            localExportEntries_.insert(std::make_pair(entry->localName_, entry));
            return true;
        }
        return false;
    }

    // export {x} from 'test.js'
    // export {x as y} from 'test.js'
    // import { x } from 'test.js'; export { x }
    bool SourceTextModuleRecord::AddIndirectExportEntry(SourceTextModuleRecord::ExportEntry *entry)
    {
        CHECK_NOT_NULL(entry);
        ASSERT(entry != nullptr);
        ASSERT(!entry->importName_.Empty());
        ASSERT(!entry->exportName_.Empty());
        ASSERT(entry->localName_.Empty());
        ASSERT(entry->moduleRequestIdx_ != -1);
        if (!HasDuplicateExport(entry->exportName_)) {
            indirectExportEntries_.push_back(entry);
            return true;
        }
        return false;
    }

    // export * from 'test.js'
    void SourceTextModuleRecord::AddStarExportEntry(SourceTextModuleRecord::ExportEntry *entry)
    {
        ASSERT(entry->importName_.Empty());
        ASSERT(entry->localName_.Empty());
        ASSERT(entry->exportName_.Empty());
        ASSERT(entry->moduleRequestIdx_ != -1);
        starExportEntries_.push_back(entry);
    }

    bool SourceTextModuleRecord::HasDuplicateExport(util::StringView exportName) const
    {
        for (auto const &entryUnit : localExportEntries_) {
            const SourceTextModuleRecord::ExportEntry *e = entryUnit.second;
            if (exportName == e->exportName_) {
                return true;
            }
        }

        for (const auto *e : indirectExportEntries_) {
            if (exportName == e->exportName_) {
                return true;
            }
        }

        return false;
    }

    bool SourceTextModuleRecord::CheckImplicitIndirectExport(SourceTextModuleRecord::ExportEntry *exportEntry)
    {
        CHECK_NOT_NULL(exportEntry);
        ASSERT(exportEntry != nullptr);
        ASSERT(!exportEntry->localName_.Empty());
        auto regularImport = regularImportEntries_.find(exportEntry->localName_);
        if (regularImport != regularImportEntries_.end()) {
            ConvertLocalExportToIndirect(regularImport->second, exportEntry);
            return AddIndirectExportEntry(exportEntry);
        }
        return false;
    }

    void SourceTextModuleRecord::CheckImplicitIndirectExport(SourceTextModuleRecord::ImportEntry *importEntry)
    {
        ASSERT(!importEntry->localName_.Empty());
        auto range = localExportEntries_.equal_range(importEntry->localName_);
        // not found implicit indirect
        if (range.first == range.second) {
            return;
        }

        for (auto it = range.first; it != range.second; ++it) {
            SourceTextModuleRecord::ExportEntry *exportEntry = it->second;
            ConvertLocalExportToIndirect(importEntry, exportEntry);
            indirectExportEntries_.push_back(exportEntry);
        }
        localExportEntries_.erase(range.first, range.second);
    }

    void SourceTextModuleRecord::ConvertLocalExportToIndirect(SourceTextModuleRecord::ImportEntry *importEntry,
                                                              SourceTextModuleRecord::ExportEntry *exportEntry)
    {
        CHECK_NOT_NULL(importEntry);
        ASSERT(exportEntry->importName_.Empty());
        ASSERT(exportEntry->moduleRequestIdx_ == -1);
        ASSERT(!importEntry->importName_.Empty());
        ASSERT(importEntry->moduleRequestIdx_ != -1);
        exportEntry->importName_ = importEntry->importName_;
        exportEntry->moduleRequestIdx_ = importEntry->moduleRequestIdx_;
        exportEntry->localName_ = util::StringView("");
    }

    void SourceTextModuleRecord::AssignIndexToModuleVariable(binder::ModuleScope *moduleScope)
    {
        uint32_t index = 0;
        for (auto it = localExportEntries_.begin(); it != localExportEntries_.end();
             it = localExportEntries_.upper_bound(it->first)) {
            auto variable = CheckAndAssignIndex(moduleScope, it->first, &index);
            if (variable != nullptr && variable->IsModuleVariable() && variable->Declaration()->IsConstDecl()) {
                auto range = localExportEntries_.equal_range(it->first);
                for (auto local_iter = range.first; local_iter != range.second; local_iter++) {
                    local_iter->second->SetAsConstant();
                }
            }
        }

        index = 0;
        for (const auto &elem : regularImportEntries_) {
            CheckAndAssignIndex(moduleScope, elem.first, &index);
        }
    }

    binder::Variable *SourceTextModuleRecord::CheckAndAssignIndex(binder::ModuleScope *moduleScope,
                                                                  util::StringView name,
                                                                  uint32_t *index) const
    {
        auto modulevar = moduleScope->FindLocal(name);
        if (modulevar != nullptr) {
            moduleScope->AssignIndexToModuleVariable(name, *index);
            (*index)++;
        }
        return modulevar;
    }

    void SourceTextModuleRecord::RemoveDefaultLocalExportEntry()
    {
        util::StringView localName = parser::SourceTextModuleRecord::DEFAULT_LOCAL_NAME;
        localExportEntries_.erase(localName);
    }

    int SourceTextModuleRecord::GetModuleRequestIdx(const util::StringView localName)
    {
        for (const auto &it : regularImportEntries_) {
            if (it.first != localName) {
                continue;
            }

            return it.second->moduleRequestIdx_;
        }

        for (const auto &it : namespaceImportEntries_) {
            if (it->localName_ != localName) {
                continue;
            }

            return it->moduleRequestIdx_;
        }

        return SourceTextModuleRecord::INVALID_MODULEREQUEST_ID;
    }
} // namespace panda::es2panda::parser

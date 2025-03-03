/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecmascript/module/module_tools.h"

#include "ecmascript/ecma_context.h"
#include "ecmascript/global_env.h"
#include "ecmascript/jspandafile/js_pandafile_executor.h"
#include "ecmascript/module/js_module_manager.h"
#include "ecmascript/module/js_shared_module.h"
#include "ecmascript/module/module_logger.h"
#include "ecmascript/module/module_manager_helper.h"
#include "ecmascript/module/module_path_helper.h"
#include "ecmascript/require/js_cjs_module.h"


namespace panda::ecmascript {

JSTaggedValue ModuleTools::GetModuleValueFromIndexBindingForLog(
    JSThread *thread, JSHandle<SourceTextModule> module, JSTaggedValue resolvedBinding, int32_t index)
{
    JSHandle<ResolvedRecordIndexBinding> binding(thread, resolvedBinding);
    JSHandle<SourceTextModule> resolvedModule =
        ModuleManagerHelper::GetResolvedRecordIndexBindingModule(thread, module, binding);
    ModuleLogger *moduleLogger =
        thread->GetCurrentEcmaContext()->GetModuleLogger();
    moduleLogger->InsertModuleLoadInfo(module, resolvedModule, index);
    return ModuleManagerHelper::GetModuleValue(thread, resolvedModule, binding->GetIndex());
}

// use for moduleLogger is not nullptr
JSTaggedValue ModuleTools::GetModuleValueFromRecordBindingForLog(
    JSThread *thread, JSHandle<SourceTextModule> module, JSTaggedValue resolvedBinding, int32_t index)
{
    JSHandle<ResolvedRecordBinding> binding(thread, resolvedBinding);
    JSHandle<SourceTextModule> resolvedModule =
        ModuleManagerHelper::GetResolvedRecordBindingModule(thread, module, binding);
    ModuleLogger *moduleLogger =
        thread->GetCurrentEcmaContext()->GetModuleLogger();
    moduleLogger->InsertModuleLoadInfo(module, resolvedModule, index);
    return ModuleManagerHelper::GetModuleValue(thread, resolvedModule, binding->GetBindingName());
}

// use for moduleLogger is not nullptr
JSTaggedValue ModuleTools::ProcessModuleLoadInfo(JSThread *thread, JSHandle<SourceTextModule> currentModule,
    JSTaggedValue resolvedBinding, int32_t index)
{
    ModuleLogger *moduleLogger = thread->GetCurrentEcmaContext()->GetModuleLogger();
    JSMutableHandle<JSTaggedValue> res(thread, thread->GlobalConstants()->GetUndefined());
    if (resolvedBinding.IsResolvedIndexBinding()) {
        ResolvedIndexBinding *binding = ResolvedIndexBinding::Cast(resolvedBinding.GetTaggedObject());
        JSTaggedValue resolvedModule = binding->GetModule();
        JSHandle<SourceTextModule> module(thread, resolvedModule);
        ASSERT(resolvedModule.IsSourceTextModule());

        // Support for only modifying var value of HotReload.
        // Cause patchFile exclude the record of importing modifying var. Can't reresolve moduleRecord.
        EcmaContext *context = thread->GetCurrentEcmaContext();
        if (context->GetStageOfHotReload() == StageOfHotReload::LOAD_END_EXECUTE_PATCHMAIN) {
            const JSHandle<JSTaggedValue> resolvedModuleOfHotReload =
                context->FindPatchModule(module->GetEcmaModuleRecordNameString());
            if (!resolvedModuleOfHotReload->IsHole()) {
                resolvedModule = resolvedModuleOfHotReload.GetTaggedValue();
                JSHandle<SourceTextModule> moduleOfHotReload(thread, resolvedModule);
                moduleLogger->InsertModuleLoadInfo(currentModule, module, index);
                return ModuleManagerHelper::GetModuleValue(thread, moduleOfHotReload, binding->GetIndex());
            }
        }
        moduleLogger->InsertModuleLoadInfo(currentModule, module, index);
        return ModuleManagerHelper::GetModuleValue(thread, module, binding->GetIndex());
    }
    if (resolvedBinding.IsResolvedBinding()) {
        ResolvedBinding *binding = ResolvedBinding::Cast(resolvedBinding.GetTaggedObject());
        JSTaggedValue resolvedModule = binding->GetModule();
        JSHandle<SourceTextModule> module(thread, resolvedModule);
        if (SourceTextModule::IsNativeModule(module->GetTypes())) {
            moduleLogger->InsertModuleLoadInfo(currentModule, module, index);
            return ModuleManagerHelper::UpdateBindingAndGetModuleValue(
                thread, currentModule, module, index, binding->GetBindingName());
        }
        if (module->GetTypes() == ModuleTypes::CJS_MODULE) {
            moduleLogger->InsertModuleLoadInfo(currentModule, module, index);
            return ModuleManagerHelper::UpdateBindingAndGetModuleValue(
                thread, currentModule, module, index, binding->GetBindingName());
        }
    }
    if (resolvedBinding.IsResolvedRecordIndexBinding()) {
        return GetModuleValueFromIndexBindingForLog(thread, currentModule, resolvedBinding, index);
    }
    if (resolvedBinding.IsResolvedRecordBinding()) { // LCOV_EXCL_BR_LINE
        return GetModuleValueFromRecordBindingForLog(thread, currentModule, resolvedBinding, index);
    }
    LOG_ECMA(FATAL) << "Get module value failed, mistaken ResolvedBinding";
    UNREACHABLE();
}

JSTaggedValue ModuleTools::ProcessModuleNameSpaceLoadInfo(
    JSThread *thread, JSHandle<SourceTextModule> currentModule, JSHandle<SourceTextModule> requiredModule)
{
    ModuleTypes moduleType = requiredModule->GetTypes();
    ModuleLogger *moduleLogger = thread->GetCurrentEcmaContext()->GetModuleLogger();
    // if requiredModule is Native module
    if (SourceTextModule::IsNativeModule(moduleType)) {
        moduleLogger->InsertModuleLoadInfo(currentModule, requiredModule, -1);
        return SourceTextModule::Cast(requiredModule.GetTaggedValue())->GetModuleValue(thread, 0, false);
    }
    // if requiredModule is CommonJS
    if (moduleType == ModuleTypes::CJS_MODULE) {
        moduleLogger->InsertModuleLoadInfo(currentModule, requiredModule, -1);
        CString cjsModuleName = SourceTextModule::GetModuleName(requiredModule.GetTaggedValue());
        JSHandle<JSTaggedValue> moduleNameHandle(thread->GetEcmaVM()->GetFactory()->NewFromUtf8(cjsModuleName));
        return CjsModule::SearchFromModuleCache(thread, moduleNameHandle).GetTaggedValue();
    }
    // if requiredModule is ESM
    moduleLogger->InsertModuleLoadInfo(currentModule, requiredModule, -1);
    JSHandle<JSTaggedValue> moduleNamespace = SourceTextModule::GetModuleNamespace(thread, requiredModule);
    ASSERT(moduleNamespace->IsModuleNamespace());
    return moduleNamespace.GetTaggedValue();
}

JSTaggedValue ModuleTools::GetLazyModuleValueFromIndexBindingForLog(
    JSThread *thread, JSHandle<SourceTextModule> module, JSTaggedValue resolvedBinding, int32_t index)
{
    JSHandle<ResolvedRecordIndexBinding> binding(thread, resolvedBinding);
    JSHandle<JSTaggedValue> recordName(thread, binding->GetModuleRecord());
    ASSERT(recordName->IsString());
    // moduleRecord is string, find at current vm
    ModuleManager *moduleManager = thread->GetCurrentEcmaContext()->GetModuleManager();
    CString recordNameStr = ModulePathHelper::Utf8ConvertToString(recordName.GetTaggedValue());
    JSHandle<SourceTextModule> resolvedModule;
    if (moduleManager->IsLocalModuleLoaded(recordNameStr)) {
        resolvedModule = moduleManager->HostGetImportedModule(recordNameStr);
        if (!moduleManager->IsEvaluatedModule(recordNameStr)) {
            SourceTextModule::Evaluate(thread, resolvedModule, nullptr);
            RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, JSTaggedValue::Exception());
        }
    } else {
        auto isMergedAbc = !module->GetEcmaModuleRecordNameString().empty();
        CString fileName = ModulePathHelper::Utf8ConvertToString(binding->GetAbcFileName());
        if (!JSPandaFileExecutor::LazyExecuteModule(thread,
            recordNameStr, fileName, isMergedAbc)) { // LCOV_EXCL_BR_LINE
            LOG_ECMA(FATAL) << "LazyExecuteModule failed";
        }
        resolvedModule = moduleManager->HostGetImportedModule(recordNameStr);
    }
    ModuleLogger *moduleLogger =
        thread->GetCurrentEcmaContext()->GetModuleLogger();
    moduleLogger->InsertModuleLoadInfo(module, resolvedModule, index);
    return ModuleManagerHelper::GetModuleValue(thread, resolvedModule, binding->GetIndex());
}

// use for moduleLogger is not nullptr
JSTaggedValue ModuleTools::GetLazyModuleValueFromRecordBindingForLog(
    JSThread *thread, JSHandle<SourceTextModule> module, JSTaggedValue resolvedBinding, int32_t index)
{
    JSHandle<ResolvedRecordBinding> binding(thread, resolvedBinding);
    JSHandle<JSTaggedValue> recordName(thread, binding->GetModuleRecord());
    ASSERT(recordName->IsString());
    // moduleRecord is string, find at current vm
    ModuleManager *moduleManager = thread->GetCurrentEcmaContext()->GetModuleManager();
    CString recordNameStr = ModulePathHelper::Utf8ConvertToString(recordName.GetTaggedValue());
    JSHandle<SourceTextModule> resolvedModule;
    if (moduleManager->IsLocalModuleLoaded(recordNameStr)) {
        resolvedModule = moduleManager->HostGetImportedModule(recordNameStr);
        if (!moduleManager->IsEvaluatedModule(recordNameStr)) {
            SourceTextModule::Evaluate(thread, resolvedModule, nullptr);
            RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, JSTaggedValue::Exception());
        }
    } else {
        auto isMergedAbc = !module->GetEcmaModuleRecordNameString().empty();
        CString fileName = module->GetEcmaModuleFilenameString();
        if (!JSPandaFileExecutor::LazyExecuteModule(thread,
            recordNameStr, fileName, isMergedAbc)) { // LCOV_EXCL_BR_LINE
            LOG_ECMA(FATAL) << "LazyExecuteModule failed";
        }
        resolvedModule = moduleManager->HostGetImportedModule(recordNameStr);
    }
    ModuleLogger *moduleLogger =
        thread->GetCurrentEcmaContext()->GetModuleLogger();
    moduleLogger->InsertModuleLoadInfo(module, resolvedModule, index);
    return ModuleManagerHelper::GetModuleValue(thread, resolvedModule, binding->GetBindingName());
}

// use for moduleLogger is not nullptr
JSTaggedValue ModuleTools::ProcessLazyModuleLoadInfo(JSThread *thread, JSHandle<SourceTextModule> currentModule,
    JSTaggedValue resolvedBinding, int32_t index)
{
    ModuleLogger *moduleLogger = thread->GetCurrentEcmaContext()->GetModuleLogger();
    JSMutableHandle<JSTaggedValue> res(thread, thread->GlobalConstants()->GetUndefined());
    if (resolvedBinding.IsResolvedIndexBinding()) {
        JSHandle<ResolvedIndexBinding> binding(thread, resolvedBinding);
        JSTaggedValue resolvedModule = binding->GetModule();
        JSMutableHandle<SourceTextModule> module(thread, resolvedModule);
        ASSERT(resolvedModule.IsSourceTextModule());
        // Support for only modifying var value of HotReload.
        // Cause patchFile exclude the record of importing modifying var. Can't reresolve moduleRecord.
        EcmaContext *context = thread->GetCurrentEcmaContext();
        if (context->GetStageOfHotReload() == StageOfHotReload::LOAD_END_EXECUTE_PATCHMAIN) {
            const JSHandle<JSTaggedValue> resolvedModuleOfHotReload =
                context->FindPatchModule(module->GetEcmaModuleRecordNameString());
            if (!resolvedModuleOfHotReload->IsHole()) {
                resolvedModule = resolvedModuleOfHotReload.GetTaggedValue();
                module.Update(resolvedModule);
            }
        }
        if (module->GetStatus() < ModuleStatus::EVALUATED) {
            SourceTextModule::Evaluate(thread, module, nullptr, 0, ExecuteTypes::LAZY);
        }
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, JSTaggedValue::Exception());
        moduleLogger->InsertModuleLoadInfo(currentModule, module, index);
        return ModuleManagerHelper::GetModuleValue(thread, module, binding->GetIndex());
    }
    if (resolvedBinding.IsResolvedBinding()) {
        JSHandle<ResolvedBinding> binding(thread, resolvedBinding);
        JSTaggedValue resolvedModule = binding->GetModule();
        JSHandle<SourceTextModule> module(thread, resolvedModule);
        ModuleTypes moduleType = module->GetTypes();
        if (SourceTextModule::IsNativeModule(moduleType)) {
            SourceTextModule::EvaluateNativeModule(thread, module, moduleType);
            moduleLogger->InsertModuleLoadInfo(currentModule, module, index);
            return ModuleManagerHelper::UpdateBindingAndGetModuleValue(
                thread, currentModule, module, index, binding->GetBindingName());
        }
        if (SourceTextModule::IsCjsModule(moduleType)) {
            ModuleStatus status = module->GetStatus();
            if (status < ModuleStatus::EVALUATED) {
                SourceTextModule::ModuleExecution(thread, module, nullptr, 0);
                SourceTextModule::RecordEvaluatedOrError(thread, module);
                RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, JSTaggedValue::Exception());
            }
            moduleLogger->InsertModuleLoadInfo(currentModule, module, index);
            return ModuleManagerHelper::UpdateBindingAndGetModuleValue(
                thread, currentModule, module, index, binding->GetBindingName());
        }
    }
    if (resolvedBinding.IsResolvedRecordIndexBinding()) {
        return GetLazyModuleValueFromIndexBindingForLog(thread, currentModule, resolvedBinding, index);
    }
    if (resolvedBinding.IsResolvedRecordBinding()) { // LCOV_EXCL_BR_LINE
        return GetLazyModuleValueFromRecordBindingForLog(thread, currentModule, resolvedBinding, index);
    }
    LOG_ECMA(FATAL) << "Get module value failed, mistaken ResolvedBinding";
    UNREACHABLE();
}
} // namespace panda::ecmascript

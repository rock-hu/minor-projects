/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ani.h"
#include "ani_options_parser.h"
#include "ani_options.h"
#include "generated/base_options.h"
#include "plugins/ets/runtime/ani/ani_checkers.h"
#include "plugins/ets/runtime/ani/ani_interaction_api.h"
#include "plugins/ets/runtime/ets_coroutine.h"
#include "plugins/ets/runtime/ets_vm.h"
#include "plugins/ets/runtime/interop_js/interop_context_api.h"
#include "utils/logger.h"
#include "utils/pandargs.h"

// CC-OFFNXT(huge_method[C++]) solid logic
extern "C" ani_status ANI_CreateVM(const ani_options *options, uint32_t version, ani_vm **result)
{
    ANI_DEBUG_TRACE(env);
    ANI_CHECK_RETURN_IF_EQ(result, nullptr, ANI_INVALID_ARGS);
    ANI_CHECK_RETURN_IF_EQ(options, nullptr, ANI_INVALID_ARGS);
    if (!ark::ets::ani::IsVersionSupported(version)) {
        return ANI_INVALID_VERSION;
    }

    size_t optionsSize = options->nr_options;
    const ani_option *optionsArr = options->options;

    ark::ets::ani::ANIOptionsParser aniParser(optionsSize, optionsArr);

    // NOTE(konstanting, #23205): please note that compiler options are not supported by ANI_CreateVM.
    // Compiler logging is not supported too. Please look at e.g. ets_vm_api.cpp:CreateRuntime
    // and ets_vm_plugin.cpp:AddOptions for reference if you would like to support the mentioned
    // features.
    ark::ets::ani::ANIOptions aniOptions;
    ark::base_options::Options baseOptions("");
    ark::PandArgParser paParser;

    // NOTE(konstanting, #23205): consider adding options validation (Validate() method)
    // Add runtime options
    baseOptions.AddOptions(&paParser);
    aniOptions.AddOptions(&paParser);

    if (!paParser.Parse(aniParser.GetRuntimeOptions())) {
        std::string errorMessage = paParser.GetErrorString();
        if (!errorMessage.empty() && errorMessage.back() == '\n') {
            // Trim new line
            errorMessage = std::string(errorMessage.c_str(), errorMessage.length() - 1);
        }
        ark::Logger::Initialize(baseOptions, aniParser.GetLoggerCallback());
        LOG(ERROR, ANI) << errorMessage;
        return ANI_ERROR;
    }
    ark::Logger::Initialize(baseOptions, aniParser.GetLoggerCallback());

#ifndef PANDA_ETS_INTEROP_JS
    if (aniParser.IsInteropMode()) {
        // no interop options allowed in the interop-free build!
        return ANI_INVALID_ARGS;
    }
#endif /* PANDA_ETS_INTEROP_JS */

    if (!ark::Runtime::Create(aniOptions)) {
        LOG(ERROR, ANI) << "Cannot create runtime";
        return ANI_ERROR;
    }

    auto coroutine = ark::ets::EtsCoroutine::GetCurrent();
    ASSERT(coroutine != nullptr);
#ifdef PANDA_ETS_INTEROP_JS
    if (aniParser.IsInteropMode()) {
        bool created = ark::ets::interop::js::CreateMainInteropContext(coroutine, aniParser.GetInteropEnv());
        if (!created) {
            LOG(ERROR, ANI) << "Cannot create interop context";
            ark::Runtime::Destroy();
            return ANI_ERROR;
        }
    }
#endif /* PANDA_ETS_INTEROP_JS */

    *result = coroutine->GetPandaVM();
    ASSERT(*result != nullptr);

    // NOTE:
    //  Don't change the following log message, because it used for testing logger callback.
    //  Or change log message at the same time as the ani_option_logger_parser_success_test.cpp test.
    LOG(INFO, ANI) << "ani_vm has been created";
    return ANI_OK;
}

// CC-OFFNXT(G.FUN.02-CPP) project code stytle
extern "C" ani_status ANI_GetCreatedVMs(ani_vm **vmsBuffer, ani_size vmsBufferLength, ani_size *result)
{
    ANI_DEBUG_TRACE(env);
    ANI_CHECK_RETURN_IF_EQ(vmsBuffer, nullptr, ANI_INVALID_ARGS);
    ANI_CHECK_RETURN_IF_EQ(result, nullptr, ANI_INVALID_ARGS);

    auto *thread = ark::Thread::GetCurrent();
    if (thread == nullptr) {
        *result = 0;
        return ANI_OK;
    }
    auto *coroutine = ark::ets::EtsCoroutine::CastFromThread(thread);
    if (coroutine != nullptr) {
        if (vmsBufferLength < 1) {
            return ANI_INVALID_ARGS;
        }

        vmsBuffer[0] = coroutine->GetPandaVM();  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        *result = 1;
    } else {
        *result = 0;
    }
    return ANI_OK;
}

namespace ark::ets::ani {

static ani_status DestroyVM(ani_vm *vm)
{
    ANI_DEBUG_TRACE(env);
    ANI_CHECK_RETURN_IF_EQ(vm, nullptr, ANI_INVALID_ARGS);

    auto runtime = Runtime::GetCurrent();
    if (runtime == nullptr) {
        LOG(ERROR, ANI) << "Cannot destroy ANI VM, there is no current runtime";
        return ANI_ERROR;
    }

    auto pandaVm = PandaEtsVM::FromAniVM(vm);
    auto mainVm = runtime->GetPandaVM();
    if (pandaVm == mainVm) {
        Runtime::Destroy();
    } else {
        PandaEtsVM::Destroy(pandaVm);
    }

    return ANI_OK;
}

NO_UB_SANITIZE static ani_status GetEnv(ani_vm *vm, uint32_t version, ani_env **result)
{
    ANI_DEBUG_TRACE(env);
    ANI_CHECK_RETURN_IF_EQ(vm, nullptr, ANI_INVALID_ARGS);
    ANI_CHECK_RETURN_IF_EQ(result, nullptr, ANI_INVALID_ARGS);

    if (!IsVersionSupported(version)) {
        return ANI_ERROR;  // NOTE: Unsupported version?
    }

    PandaEtsVM *pandaVM = PandaEtsVM::FromAniVM(vm);
    EtsCoroutine *coroutine = EtsCoroutine::CastFromThread(pandaVM->GetAssociatedThread());
    if (coroutine == nullptr) {
        LOG(ERROR, ANI) << "Cannot get environment";
        return ANI_ERROR;
    }
    *result = coroutine->GetEtsNapiEnv();
    return ANI_OK;
}

static ani_status AttachCurrentThread(ani_vm *vm, const ani_options *options, uint32_t version, ani_env **result)
{
    ANI_DEBUG_TRACE(vm);
    ANI_CHECK_RETURN_IF_EQ(vm, nullptr, ANI_INVALID_ARGS);
    ANI_CHECK_RETURN_IF_EQ(result, nullptr, ANI_INVALID_ARGS);
    ANI_CHECK_RETURN_IF_EQ(IsVersionSupported(version), false, ANI_INVALID_VERSION);

    if (Thread::GetCurrent() != nullptr) {
        LOG(ERROR, ANI) << "Cannot attach current thread, thread has already been attached";
        return ANI_ERROR;
    }

    bool interopEnabled = false;
    if (options != nullptr) {
        for (size_t i = 0; i < options->nr_options; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            PandaString opt(options->options[i].option);
            if (opt == "--interop=enable") {
                interopEnabled = true;
            } else if (opt == "--interop=disable") {
                interopEnabled = false;
            }
        }
    }

    auto *runtime = Runtime::GetCurrent();
    auto *etsVM = PandaEtsVM::FromAniVM(vm);
    auto *coroMan = etsVM->GetCoroutineManager();
    auto *exclusiveCoro = coroMan->CreateExclusiveWorkerForThread(runtime, etsVM);
    if (exclusiveCoro == nullptr) {
        LOG(ERROR, ANI) << "Cannot attach current thread, reached the limit of EAWorkers";
        return ANI_ERROR;
    }

    ASSERT(exclusiveCoro == Coroutine::GetCurrent());

    if (interopEnabled) {
        auto *ifaceTable = EtsCoroutine::CastFromThread(coroMan->GetMainThread())->GetExternalIfaceTable();
        auto *jsEnv = ifaceTable->CreateJSRuntime();
        ASSERT(jsEnv != nullptr);
        ifaceTable->CreateInteropCtx(exclusiveCoro, jsEnv);
    }
    *result = PandaEtsNapiEnv::GetCurrent();
    return ANI_OK;
}

static ani_status DetachCurrentThread(ani_vm *vm)
{
    ANI_DEBUG_TRACE(vm);
    ANI_CHECK_RETURN_IF_EQ(vm, nullptr, ANI_INVALID_ARGS);
    if (Thread::GetCurrent() == nullptr) {
        LOG(ERROR, ANI) << "Cannot detach current thread, thread is not attached";
        return ANI_ERROR;
    }

    auto *etsVM = PandaEtsVM::FromAniVM(vm);
    auto *coroMan = etsVM->GetCoroutineManager();
    auto result = coroMan->DestroyExclusiveWorker();
    if (!result) {
        LOG(ERROR, ANI) << "Cannot DetachThread, thread was not attached";
        return ANI_ERROR;
    }
    ASSERT(Thread::GetCurrent() == nullptr);
    return ANI_OK;
}

// clang-format off
const __ani_vm_api VM_API = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    DestroyVM,
    GetEnv,
    AttachCurrentThread,
    DetachCurrentThread,
};
// clang-format on

const __ani_vm_api *GetVMAPI()
{
    return &VM_API;
}
}  // namespace ark::ets::ani

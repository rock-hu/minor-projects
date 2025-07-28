/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "plugins/ets/runtime/interop_js/stack_info.h"

#include "macros.h"
#include "plugins/ets/runtime/interop_js/interop_context.h"
#include "runtime/coroutines/stackful_coroutine.h"
#include "interop_js/napi_impl/napi_impl.h"

#include <cstddef>
#include <memory>

// CC-OFFNXT(G.NAM.03) has to be compatible with JS NAPI struct
// NOLINTNEXTLINE(readability-identifier-naming)
#ifndef PANDA_JS_ETS_HYBRID_MODE
struct napi_stack_info {
    size_t stackStart;
    size_t stackSize;
};
#endif

// NOLINTNEXTLINE(readability-identifier-naming)
napi_status __attribute__((weak)) napi_set_stackinfo(napi_env env, napi_stack_info *info);
// NOLINTNEXTLINE(readability-identifier-naming)
napi_status __attribute__((weak)) napi_get_stackinfo(napi_env env, napi_stack_info *result);

namespace ark::ets::interop::js {

StackInfoManagerOhos::StackInfoManagerOhos(InteropCtx *ctx, EtsCoroutine *coro) : StackInfoManagerBase {ctx, coro} {}

// NOLINTNEXTLINE(modernize-use-equals-default)
StackInfoManagerOhos::~StackInfoManagerOhos() {}

void StackInfoManagerOhos::InitStackInfoIfNeeded()
{
    ASSERT(EtsCoroutine::GetCurrent() == mainCoro_);
    if (LIKELY(!mainStackInfo_)) {
        mainStackInfo_ = std::make_unique<napi_stack_info>();
        auto env = ctx_->GetJSEnv();
        napi_get_stackinfo(env, mainStackInfo_.get());
    }
}

void StackInfoManagerOhos::UpdateStackInfoIfNeeded()
{
    if (UNLIKELY(!mainStackInfo_)) {
        return;
    }
    auto *coro = EtsCoroutine::GetCurrent();
    if (coro == mainCoro_) {
        napi_set_stackinfo(ctx_->GetJSEnv(), mainStackInfo_.get());
    } else {
        void *stackAddr = nullptr;
        size_t stackSize {};
        size_t guardSize {};
        ASSERT(coro != nullptr);
        coro->GetContext<StackfulCoroutineContext>()->RetrieveStackInfo(stackAddr, stackSize, guardSize);
        napi_stack_info currentStackinfo {reinterpret_cast<size_t>(stackAddr), stackSize};
        napi_set_stackinfo(ctx_->GetJSEnv(), &currentStackinfo);
    }
}

}  // namespace ark::ets::interop::js

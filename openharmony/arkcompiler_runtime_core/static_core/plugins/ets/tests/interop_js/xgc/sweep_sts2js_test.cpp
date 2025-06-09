/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <iostream>
#include <array>

#include "hybrid/ecma_vm_interface.h"
#include "plugins/ets/runtime/ets_vm.h"
#include "plugins/ets/runtime/types/ets_object.h"
#include "plugins/ets/runtime/interop_js/interop_context.h"
#include "plugins/ets/tests/interop_js/xgc/test_xgc_vm_adaptor.h"

namespace ark::ets::interop::js {

struct TestXGCEcmaAdaptorValues {
    std::vector<std::string> GetErrors()
    {
        return errors;
    }

    std::vector<std::string> errors;
};
static TestXGCEcmaAdaptorValues g_xgcAdaptorValues;

class TestXGCEcmaVmAdaptor : public TestXGCVmAdaptor {
public:
    explicit TestXGCEcmaVmAdaptor(napi_env env, TestXGCEcmaAdaptorValues *values)
        : TestXGCVmAdaptor(env), values_(values)
    {
    }

    void MarkFromObject([[maybe_unused]] napi_ref obj) override
    {
        std::stringstream err;
        err << "Unexpected call of MarkFromObject";
        values_->errors.push_back(err.str());
        return;
    }

private:
    TestXGCEcmaAdaptorValues *values_;
};

class TestGCListener : public mem::GCListener {
public:
    void GCStarted([[maybe_unused]] const GCTask &task, [[maybe_unused]] size_t heapSize) override
    {
        auto *xrefStorage = InteropCtx::Current()->GetSharedRefStorage();
        if (xrefStorage->Size() != 1U) {
            std::stringstream err;
            err << "Expected one xrefs, but got " << xrefStorage->Size();
            errorMessages_.push_back(err.str());
            return;
        }
    }

    void GCFinished(const GCTask &task, [[maybe_unused]] size_t heapSizeBeforeGc,
                    [[maybe_unused]] size_t heapSize) override
    {
        auto *xrefStorage = InteropCtx::Current()->GetSharedRefStorage();
        if (xrefStorage->Size() != 0U) {
            std::stringstream err;
            err << "Expected all xrefs collected, but got " << xrefStorage->Size();
            errorMessages_.push_back(err.str());
            return;
        }
    }

    const std::vector<std::string> &GetErrors() const
    {
        return errorMessages_;
    }

private:
    std::vector<std::string> errorMessages_;
};

static TestGCListener g_gcListener;
}  // namespace ark::ets::interop::js

#include "test_module.h"

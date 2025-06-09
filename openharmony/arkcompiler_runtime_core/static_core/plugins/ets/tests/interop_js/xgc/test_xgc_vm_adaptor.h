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

#ifndef PLUGINS_ETS_TESTS_INTEROP_JS_XGC_TEST_XGC_VM_ADAPTOR_H
#define PLUGINS_ETS_TESTS_INTEROP_JS_XGC_TEST_XGC_VM_ADAPTOR_H

#include "plugins/ets/runtime/interop_js/xgc/xgc_vm_adaptor.h"

namespace ark::ets::interop::js {

class TestXGCVmAdaptor : public XGCVmAdaptor {
public:
    explicit TestXGCVmAdaptor(napi_env env) : XGCVmAdaptor(env, nullptr) {}
    NO_COPY_SEMANTIC(TestXGCVmAdaptor);
    NO_MOVE_SEMANTIC(TestXGCVmAdaptor);
    ~TestXGCVmAdaptor() override = default;

    void MarkFromObject([[maybe_unused]] napi_ref ref) override {}
    bool StartXRefMarking() override
    {
        return true;
    }
};

}  // namespace ark::ets::interop::js

#endif  // PLUGINS_ETS_TESTS_INTEROP_JS_XGC_TEST_XGC_VM_ADAPTOR_H
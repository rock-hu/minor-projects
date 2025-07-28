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

function runTest() {
    let etsVm = requireNapiPreview('ets_interop_js_napi', true);

    const etsVmRes = etsVm.createRuntime({
        'load-runtimes': 'ets',
        'boot-panda-files': 'etsstdlib.abc:' + 'taskpool_test.abc',
        'gc-trigger-type': 'heap-trigger',
        'compiler-enable-jit': 'false',
        'taskpool-support-interop': 'true',
        'run-gc-in-place': 'true',
        'coroutine-workers-count': '1',
    });
    if (!etsVmRes) {
        throw new Error('Failed to create ETS runtime');
    } 

    let RunTasksWithJsCallTest = etsVm.getFunction('Ltaskpool_test/ETSGLOBAL;', 'RunTasksWithJsCallTest');
    RunTasksWithJsCallTest();
    let RunTasksWithJsAsyncCallTest = etsVm.getFunction('Ltaskpool_test/ETSGLOBAL;', 'RunTasksWithJsAsyncCallTest');
    RunTasksWithJsAsyncCallTest();
}

runTest();

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
let g_etsVm;
let g_testModule;

class JsTestClass {
    constructor() {
        this.obj = null;
    }
}

function init(module, stsFile) {
    g_etsVm = requireNapiPreview('ets_interop_js_napi', true);
    g_testModule = requireNapiPreview(module, true);

    const etsVmRes = g_etsVm.createRuntime({
        'load-runtimes': 'ets',
        'log-components': 'ets_interop_js',
        'boot-panda-files': 'etsstdlib.abc:' + stsFile,
        'gc-trigger-type': 'heap-trigger',
        'compiler-enable-jit': 'false',
        'run-gc-in-place': 'true',
        'coroutine-workers-count': '1',
        'enable-xgc-verifier': 'true',
    });
    if (!etsVmRes) {
        throw new Error('Failed to create ETS runtime');
    } else {
        print('ETS runtime created');
    }
    g_testModule.setup();
    return g_etsVm;
}

function triggerXGC() {
    const xgc = g_etsVm.getFunction('Lxgc_test_ets/ETSGLOBAL;', 'xgc');
    xgc();
}

function checkXRefsNumber(jsNum, stsNum) {
    g_testModule.checkXRefsNumber(jsNum, stsNum);
}

function validationXGCResult(beforeJsNum, beforeStsNum, afterJsNum, afterStsNum) {
    checkXRefsNumber(beforeJsNum, beforeStsNum);
    triggerXGC();
    checkXRefsNumber(afterJsNum, afterStsNum);
}

module.exports = {
    init,
    triggerXGC,
    checkXRefsNumber,
    validationXGCResult,
    JsTestClass,
};

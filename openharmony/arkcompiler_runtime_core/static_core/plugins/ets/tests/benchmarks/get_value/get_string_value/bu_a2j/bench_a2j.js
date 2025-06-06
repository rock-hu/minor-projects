/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

function main() {
    console.log('Starting...');
    let penv = process.env;
    console.log('penv.MODULE_PATH: ' + penv.MODULE_PATH);
    let stsVm = require(penv.MODULE_PATH + '/ets_interop_js_napi.node');
    console.log(penv.ARK_ETS_STDLIB_PATH + ':' + penv.ARK_ETS_INTEROP_JS_GTEST_ABC_PATH);
    const stsRT = stsVm.createRuntime({
        'boot-panda-files': penv.ARK_ETS_STDLIB_PATH + ':' + penv.ARK_ETS_INTEROP_JS_GTEST_ABC_PATH,
        'panda-files': penv.ARK_ETS_INTEROP_JS_GTEST_ABC_PATH,
        'gc-trigger-type': 'heap-trigger',
        'compiler-enable-jit': 'false',
        'run-gc-in-place': 'false',
    });

    if (!stsRT) {
        console.error('Failed to create ETS runtime');
        return 1;
    }

    const getObj = stsVm.getFunction('LETSGLOBAL;', 'getStsObj');

    let totalTime = 0n;

    for (let i = 0; i < 10000; i++) { // Number of iterations was picked experimentally
        const stsObj = getObj();
        let start = process.hrtime.bigint();

        const stringVal = stsObj.stsString;

        totalTime += (process.hrtime.bigint() - start);
    }

    console.log('Benchmark result: A2j ' + totalTime);
    return null;
}

main();
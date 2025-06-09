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
const { init, validationXGCResult } = require('./mark_test_utils.js');
let gEtsVm;

/**
 * Proxy js objects on the sts side
 * js obj <- sts pobj <- sts obj
 */
function proxyJsObjectTest() {
    const proxyJsObject = gEtsVm.getFunction('Lxgc_test/ETSGLOBAL;', 'proxyJsObject');
    proxyJsObject(Promise.resolve(), false, false);
}

/**
 * Proxy sts objects on the js side
 * js pobj -> sts obj
 */
function proxyStsObjectTest() {
    const createStsObject = gEtsVm.getFunction('Lxgc_test/ETSGLOBAL;', 'createStsObject');
    createStsObject(false, false);
}

/**
 * Proxy objects testing
 * js obj <- sts pobj <- sts obj
 * js pobj -> sts obj
 */
function proxyObjectTest() {
    const proxyJsObject = gEtsVm.getFunction('Lxgc_test/ETSGLOBAL;', 'proxyJsObject');
    proxyJsObject(Promise.resolve(), false, false);
    const createStsObject = gEtsVm.getFunction('Lxgc_test/ETSGLOBAL;', 'createStsObject');
    createStsObject(false, false);
}

gEtsVm = init('mark_test_create_proxy_module', 'xgc_tests.abc');

proxyJsObjectTest();
validationXGCResult(1, 0, 0, 0);

proxyStsObjectTest();
validationXGCResult(0, 1, 0, 0);

proxyObjectTest();
validationXGCResult(1, 1, 0, 0);

print('mark_test_create_proxy executes successful');

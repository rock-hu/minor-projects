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
const { init, triggerXGC, checkXRefsNumber, validationXGCResult } = require('./mark_test_utils.js');

let gObj = Promise.resolve();
let gInlineObj = Promise.resolve();
let gEtsVm;

// clear the cross-reference objects that are referenced by the active objects
function clearActiveRef() {
    gObj = Promise.resolve();
    gInlineObj = Promise.resolve();
    const clearActiveRef = gEtsVm.getFunction('Lxgc_test/ETSGLOBAL;', 'clearActiveRef');
    clearActiveRef();
}

function clearRefStorage() {
    clearActiveRef();
    triggerXGC();
    checkXRefsNumber(0, 0);
}

/**
 * Create js object
 * @param isRootRef1  A root object references return object
 * @param isRootRef2  Return object references the a root object
 */
function createJsObject(isRootRef1, isRootRef2) {
    let obj = Promise.resolve();
    if (isRootRef1) {
        gObj.ref = obj;
    }
    if (isRootRef2) {
        obj.ref = gInlineObj;
    }
    return obj;
}

/**
 * Proxy js object test
 * The reference table is not reclaimed only if isRootRef3 is true
 *          root1 -> js obj  <- sts pobj <- sts obj <- root3
 *                    |                       |
 *                    v                       v
 *                  root2                    root4
 */
function proxyJsObjectTest(isRootRef1, isRootRef2, isRootRef3, isRootRef4) {
    let obj = createJsObject(isRootRef1, isRootRef2);
    const proxyJsObject = gEtsVm.getFunction('Lxgc_test/ETSGLOBAL;', 'proxyJsObject');
    proxyJsObject(obj, isRootRef3, isRootRef4);
}

gEtsVm = init('mark_test_sts_ref_js_module', 'xgc_tests.abc');

proxyJsObjectTest(false, false, false, false);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(false, false, false, true);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(false, false, true, false);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

proxyJsObjectTest(false, false, true, true);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

proxyJsObjectTest(false, true, false, false);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(false, true, false, true);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(false, true, true, false);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

proxyJsObjectTest(false, true, true, true);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

proxyJsObjectTest(true, false, false, false);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(true, false, false, true);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(true, false, true, false);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

proxyJsObjectTest(true, false, true, true);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

proxyJsObjectTest(true, true, false, false);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(true, true, false, true);
validationXGCResult(1, 0, 0, 0);

proxyJsObjectTest(true, true, true, false);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

proxyJsObjectTest(true, true, true, true);
validationXGCResult(1, 0, 1, 0);
clearRefStorage();

print('mark_test_sts_ref_js executes sucessful');

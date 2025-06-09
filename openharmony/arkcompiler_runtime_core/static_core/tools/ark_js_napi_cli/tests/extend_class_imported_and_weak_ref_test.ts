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

// This test check by WeakRef on JS side that JS object of JS class
// that is derived from ETS class and transferred from ETS -> JS
// creating cyclic reference - can be collected by GC

import { interop } from './gc_test_common';

class JsDerivedClass extends interop.PandaBaseClass {
    derivedObj: Object;
    derivedStr: string;
    constructor() {
        super();
        this.derivedObj = new Object();
        this.derivedStr = this.str + 'JsDerivedClass';
    }
}

function getObjWeakRef(): WeakRef<JsDerivedClass> {
    let obj = new JsDerivedClass();
    let wr = new WeakRef(obj);
    return wr;
}

function main(): void {
    let wr = getObjWeakRef();
    interop.RunInteropGC();
    interop.RunPandaGC();
    globalThis.test.RunJsGC();
    if (wr.deref() !== undefined) {
        throw Error('WeakRef referred object is not collected after GCs');
    }    
}

main();

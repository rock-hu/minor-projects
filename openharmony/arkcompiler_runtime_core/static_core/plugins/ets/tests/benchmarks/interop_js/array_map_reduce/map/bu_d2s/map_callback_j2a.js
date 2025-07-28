/*
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

/**
 * @State
 * @Include ../../../shared/js/initEtsVm.js
 * @Include ../bu_d2d/randomArray.js
 * @Tags interop, d2s
 */
function mapCallback() {

    this.addPrefix = null;
    const arrayLength = 100;
    const stringLength = 25;
    let testArray = [];
    let newTestArray = [];

    /**
     * @Setup
     */
    this.setup = function () {
        let stsVm = initEtsVm();
        this.addPrefix = stsVm.getFunction('LlibImport/ETSGLOBAL;', 'addPrefix');
        generateRandomArray(arrayLength, stringLength, testArray);
    };

    /**
     * @Benchmark
     */
    this.test = function() {
        // using arrow because callBackFn is actually a 'f(num, idx, arr)'
        newTestArray = testArray.map((x) => this.addPrefix(x));
    };

}

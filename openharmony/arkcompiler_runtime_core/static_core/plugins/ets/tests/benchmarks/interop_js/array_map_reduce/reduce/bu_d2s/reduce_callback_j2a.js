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
 * @Tags interop, d2s
 */
function reduceCallback() {

    const min = 0;
    const max = 9999999;
    const arrayLength = 100;
    let testArray = [];
    let sumArray;
    this.sumFunction = null;

    function generateRandomArray(length, min, max, arr ) {
        for (let i = 0; i < length; i++) {
            const randomNumber = Math.floor(Math.random() * (max - min + 1)) + min;
            arr.push(randomNumber);
        }
    };

    /**
     * @Setup
     */
    this.setup = function () {
        let stsVm = initEtsVm();
        this.sumFunction = stsVm.getFunction('LlibImport/ETSGLOBAL;', 'sumFunction');
        generateRandomArray(arrayLength, min, max, testArray);
    };

    /**
     * @Benchmark
     * @returns {Int}
     */
    this.test = function() {
        sumArray = testArray.reduce((x, y) => this.sumFunction(x, y), 0);
        return sumArray;
    };

}

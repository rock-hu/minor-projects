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
 * @Tags interop, bu_j2j
 */
function mapCallbackJ2j() {
    
    const arrayLength = 100;
    const stringLength = 25;
    let testArray = [];
    let newTestArray = [];

    function generateRandomString(length) {
        const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';
        let result = '';
        const charactersLength = characters.length;
    
        for (let i = 0; i < length; i++) {
            const randomIndex = Math.floor(Math.random() * charactersLength);
            result += characters[randomIndex];
        }
    
        return result;
    };

    function generateRandomArray(length, stringLength, arr) {
        for (let i = 0; i < length; i++) {
            let randomString = generateRandomString(stringLength);
            arr.push(randomString);
        }
        return;
    };

    function addPrefix(text) {
        return '_' + text;
    };

    /**
     * @Setup
     */
    this.setup = function () {
        console.log('Starting...');
        generateRandomArray(arrayLength, stringLength, testArray);
        return;
    };

    /**
     * @Benchmark
     */
    this.test = function() {
        newTestArray = testArray.map(addPrefix);
        return;
    };

    return;
}

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

/**
 * @State
 * @Include ./generateNumber.js
 * @Import {binaryStringToNumberToString,charToNumberToString} from './libConversions.ts'
 * @Import {decimalStringToNumberToString} from './libConversions.ts'
 * @Import {hexStringToNumberToString,octalStringToNumberToString} from './libConversions.ts'
 * @Tags interop, d2d
 */
function testToString() {

    this.strChar = 'A';
    this.strBin = '';
    this.strNumber = '';
    this.strHex = '';
    this.strOctal = '';

    /**
     * @Setup
     */
    this.setup = function () {
        const seed = 123;
        const data = generateNumber(seed);
        this.strBin = data.toString(2);
        this.strNumber = data.toString(10);
        this.strHex = data.toString(16);
        this.strOctal = data.toString(8);
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.bin = function() {
        return binaryStringToNumberToString(this.strBin);
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.decimal = function() {
        return decimalStringToNumberToString(this.strNumber);
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.hex = function() {
        return hexStringToNumberToString(this.strHex);
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.octal = function() {
        return octalStringToNumberToString(this.strOctal);
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.chr = function() {
        return charToNumberToString(this.strChar);
    };

}

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
 * @Import {jsNumber,jsByte,jsShort,jsInt,jsLong,jsFloat,jsDouble} from '../../shared/js/libValues.js'
 * @Import {jsBigint,jsBoolean,jsNull,jsUndefined,jsString} from '../../shared/js/libValues.js'
 * @Tags interop, d2d
 */
function getValue() {

    /**
     * @Benchmark
     * @returns {Int}
     */
    this.getNumber = function() {
        let numVal = jsNumber;
        return numVal;
    };


    /**
     * @Benchmark
     * @returns {Char}
     */
    this.getByte = function() {
        let byteVal = jsByte;
        return byteVal;
    };

    /**
     * @Benchmark
     * @returns {Int}
     */
    this.getShort = function() {
        let shortVal = jsShort;
        return shortVal;
    };

    /**
     * @Benchmark
     * @returns {Int}
     */
    this.getInt = function() {
        let intVal = jsInt;
        return intVal;
    };

    /**
     * @Benchmark
     * @returns {Int}
     */
    this.getLong = function() {
        let longVal = jsLong;
        return longVal;
    };

    /**
     * @Benchmark
     * @returns {Float}
     */
    this.getFloat = function() {
        let floatVal = jsFloat;
        return floatVal;
    };

    /**
     * @Benchmark
     * @returns {Float}
     */
    this.getDouble = function() {
        let doubleVal = jsDouble;
        return doubleVal;
    };

    /**
     * @Benchmark
     * @returns {Int}
     */
    this.getBigInt = function() {
        let bigIntVal = jsBigint;
        return bigIntVal;
    };

    /**
     * @Benchmark
     * @returns {Bool}
     */
    this.getBool = function() {
        let boolVal = jsBoolean;
        return boolVal;
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.getUndefined = function() {
        let undefinedVal = jsUndefined;
        return undefinedVal;
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.getNull = function() {
        let nullVal = jsNull;
        return nullVal;
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.getString = function() {
        let strVal = jsString;
        return strVal;
    };

}

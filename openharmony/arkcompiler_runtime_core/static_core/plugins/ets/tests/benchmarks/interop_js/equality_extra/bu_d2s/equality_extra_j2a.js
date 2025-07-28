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
 * @Include ../../shared/js/initEtsVm.js
 * @Include ../../shared/js/comparison.js
 * @Tags interop, d2s
 */
function equalityExtra() {

    this.bench = null;
    this.foo = null;
    this.fooOther = null;

    /**
     * @Setup
     */
    this.setup = function () {
        let stsVm = initEtsVm();
        const State = stsVm.getClass('LlibExtraHolder/ExtraHolder;');
        this.foo = stsVm.getFunction('LlibExtraHolder/ETSGLOBAL;', 'foo');
        this.fooOther = stsVm.getFunction('LlibExtraHolder/ETSGLOBAL;', 'fooOther');
        this.ets = new State();
        return 0;
    };

    /**
     * @Benchmark
     * @returns {Bool}
     */
    this.compareArrays = function() {
        let result = true;
        result &= comparison(this.ets.etsArr, this.ets.etsArr, equal);
        result &= comparison(this.ets.etsArr, this.ets.etsArrCopy, equal);
        result &= comparison(this.ets.etsArr, this.ets.etsArrOther, noEqual);
        return result;
    };

    /**
     * @Benchmark
     * @returns {Bool}
     */
    this.compareArrowFuncs = function() {
        let result = true;
        result &= comparison(this.ets.etsArrowFunction, this.ets.etsArrowFunction, equal);
        result &= comparison(this.ets.etsArrowFunction, this.ets.etsArrowFunctionCopy, equal);
        result &= comparison(this.ets.etsArrowFunction, this.ets.etsArrowFunctionOther, noEqual);
        return result;
    };

    /**
     * @Benchmark
     * @returns {Bool}
     */
    this.compareFunc = function() {
        let result = true;
        result &= comparison(this.foo, this.foo, equal);
        result &= comparison(this.foo, this.fooOther, noEqual);
        return result;
    };

}

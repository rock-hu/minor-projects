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
 * @Tags interop, d2s
 */
function testStringify() {
    this.array = null;

    /**
     * @Setup
     */
    this.setup = function () {
        let etsVm = initEtsVm();
        this.holder = etsVm.getClass('LlibArrayHolder/ArrayHolder;');
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.testArray = function() {
        return JSON.stringify(this.holder.arr);
    };

    /**
     * @Benchmark
     * @returns {Obj}
     */
    this.testObject = function() {
        return JSON.stringify(this.holder.obj);
    };

}

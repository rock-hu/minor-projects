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

export function foo1(a: number, b: string, c?: boolean, d: number = 1): void {
    ASSERT_TRUE(a === 1 && b === "str" && c === undefined && d === 1);
}

export function foo2(a: number, b: string, c?: boolean, d: number = 1): void {
    ASSERT_TRUE(a === 1 && b === "str" && c === true && d === 1);
}

export function foo3(a: number, b: string, c?: boolean, d: number = 1): void {
    ASSERT_TRUE(a === 1 && b === "str" && c === true && d === 2);
}

export function fun1(a: number, b: string, c: boolean = true, d?: number): void {
    ASSERT_TRUE(a === 1 && b === "str" && c === true && d === undefined);
}

export function fun2(a: number, b: string, c: boolean = true, d?: number): void {
    ASSERT_TRUE(a === 1 && b === "str" && c === false && d === undefined);
}

export function fun3(a: number, b: string, c: boolean = true, d?: number): void {
    ASSERT_TRUE(a === 1 && b === "str" && c === false && d === 2);
}

const etsVm = globalThis.gtest.etsVm;
let test_optional_params = etsVm.getFunction('Loptional_params/ETSGLOBAL;', 'test_optional_params');
test_optional_params();
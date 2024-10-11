/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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


declare function AssertType(value: any, type: string): void;

const numbers: number[] = [1, 2, 3, 4, 5];

numbers.forEach((num: number): void => {
    AssertType(num, "number");
});

class A {
    x: number;
    y: number;
    constructor(num1: number, num2: number) {
        this.x = num1;
        this.y = num2;
    }
};

let ret = ((x: number, y: number): A => {
    AssertType(x, "number");
    AssertType(y, "number");
    let a = new A(x, y);
    return a;
})(1, 2);

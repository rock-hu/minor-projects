/*
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

function main(){
    let x  = `This is an example of a multiline string,
                    which should be enclosed in
                    backticks`;
    type UT = `This is an example of a multiline string,
                    which should be enclosed in
                    backticks` | "X";
    let x2:UT = `This is an example of a multiline string,
                    which should be enclosed in
                    backticks` 
    assertEQ(x, x2)
    assertEQ(x2, `This is an example of a multiline string,
                    which should be enclosed in
                    backticks`)
    assertEQ(x2, "This is an example of a multiline string,\n                    which should be enclosed in\n                    backticks")

    let x3  = `This is an example of a multiline string`;
    type UT2 = `This is an example of a multiline string` | "X";
    let x4:UT2 = `This is an example of a multiline string` 
    assertEQ(x3, x4)
    assertEQ(x4, `This is an example of a multiline string`)
    assertEQ(x4, "This is an example of a multiline string")

    let a = 10
    let b = 20
    let x5  = `The result of ${a} * ${b} is ${a * b}`;
    type UT3 = `The result of ${a} * ${b} is ${a * b}` | "X";
    let x6:UT3 = `The result of ${a} * ${b} is ${a * b}` 
    assertEQ(x5, x6)
    assertEQ(x6, `The result of ${a} * ${b} is ${a * b}`)
    assertEQ(x6, "The result of " + a + " * " + b + " is " + a * b)
    

    let x7: [number, string, Int] = [2,  `This is an example of a multiline string,
                    which should be enclosed in
                    backticks`, 3];
    assertEQ(x7[1], "This is an example of a multiline string,\n                    which should be enclosed in\n                    backticks")
}

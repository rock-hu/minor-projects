/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

function main(): void {
    let a : int = 10;
    let b : int = 10;
    let c : int = 100;
    let d : int = 10000;

    let x : int = (a == b ? 2 : 4)
    assertEQ(x, 2)

    let y : int = (c == d ? 8 : 16)
    assertEQ(y, 16)

    assertEQ(foo(true), "true")
    assertEQ(foo(false), "false")
}

function foo(b: boolean): String {
    return b ? "true" : "false";
}

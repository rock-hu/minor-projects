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

function goo (i: int, p1: I1, s: String): void {}
function goo (i: int, p2: I5, s: String): void {}
function goo (i: int, p2: I2, s: String): void {}
interface I1 {}
interface I2 {}
interface I3 {}
interface I4 {}
interface I5 {}
class X implements I3, I2, I5, I1, I4 {}

function main (): void {
    goo (0, new X(), "ambiguous")
}

/* @@? 27:5 Error TypeError: Call to `goo` is ambiguous as `2` versions of `goo` are available: `goo(i: int, p1: I1, s: String): void` and `goo(i: int, p2: I5, s: String): void`  */
/* @@? 27:5 Error TypeError: Call to `goo` is ambiguous as `2` versions of `goo` are available: `goo(i: int, p1: I1, s: String): void` and `goo(i: int, p2: I2, s: String): void`  */

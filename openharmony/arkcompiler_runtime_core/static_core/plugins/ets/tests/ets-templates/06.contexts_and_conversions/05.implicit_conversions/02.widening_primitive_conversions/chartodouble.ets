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

/*---
desc: Casting conversions char to double.
---*/

let ch: string = 'A';
let doubleValue: number = ch.charCodeAt(0);
let expectedDoubleValue: number = 65.0;

function main(): void {
  if (doubleValue === expectedDoubleValue) {
    console.log("Test passed! Char successfully converted to double.");
  } else {
    console.error(`Test failed! Expected ${expectedDoubleValue}, but got ${doubleValue}`);
  }
}
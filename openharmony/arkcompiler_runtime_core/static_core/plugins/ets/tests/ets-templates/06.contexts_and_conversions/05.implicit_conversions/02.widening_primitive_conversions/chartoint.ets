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
desc: Casting conversions char to int.
---*/

let ch: string = 'A';
let intValue: number = ch.charCodeAt(0);
let expectedIntValue: number = 65;

function main(): void {
  if (intValue === expectedIntValue) {
    console.log("Test passed! Char successfully converted to int.");
  } else {
    console.error(`Test failed! Expected ${expectedIntValue}, but got ${intValue}`);
  }
}
/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

function foo(x: Double) {}

function main(): void {
  let a : Int = new Int(2);
  /* @@ label */foo(/* @@ label1 */a);
  return;
}

/* @@@ label1 Error TypeError: Type 'Int' is not compatible with type 'Double' at index 1  */
/* @@@ label Error TypeError: No matching call signature for foo(Int)  */

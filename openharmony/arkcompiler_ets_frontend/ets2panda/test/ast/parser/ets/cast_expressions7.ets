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

class A {}
class B extends A {}

function main(): void {
  let b: B[] = [new B()];
  let a: A[] = b as A[];
  let b2: B[] = a as B[];

  let int_a: int[] = [1,2,3];
  let int_a2 = int_a as int[];

  let long_a: long[] = int_a as long[];
}

/* @@? 21:16 Error TypeError: Cannot cast type 'Array<B>' to 'Array<A>' */
/* @@? 22:17 Error TypeError: Cannot cast type 'Array<A>' to 'Array<B>' */
/* @@? 27:24 Error TypeError: Cannot cast type 'Array<Int>' to 'Array<Long>' */
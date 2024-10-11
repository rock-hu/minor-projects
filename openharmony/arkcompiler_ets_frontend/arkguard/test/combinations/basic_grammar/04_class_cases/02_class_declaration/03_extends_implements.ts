/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import assert from "assert";
class C5 {
  prop_c5: number = 0;
  method_c5(): number {
    return 5;
  }
}
interface I5 {
  inter_p5: number;
  method_i5():number;
}

class C6 extends C5 implements I5{
  inter_p5: number = 1;
  method_i5(): number {
    return 2* 5;
  }
}
let insC5 = new C5();
assert(insC5.prop_c5 === 0);
assert(insC5.method_c5() === 5);

let insC6 = new C6();
assert(insC6.prop_c5 === 0);
assert(insC6.method_c5() === 5);
assert(insC6.inter_p5 === 1);
assert(insC6.method_i5() === 10);
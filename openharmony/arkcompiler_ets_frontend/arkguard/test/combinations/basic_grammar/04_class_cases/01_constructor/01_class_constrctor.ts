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
// 单个构造器
class A1 {
  prop1: number;
  constructor(para: number) {
    this.prop1 = para;
  }
  method1() {
  }
}
let insA1 = new A1(1);
assert(insA1.prop1 === 1)
// 多个构造器
class A2 {
  prop2: string;
  constructor(para: 'hello');
  constructor(para: 'bye');
  constructor(para: string);
  constructor(para: any) {
    this.prop2 = para;
  }
  method2() {
  }
}
let insA2 = new A2('bee');
assert(insA2.prop2 === 'bee')
// class声明 - 两个构造器不连续
declare namespace ns {
  class A3 {
    constructor();
    str3: string;
    constructor(x: number);
  }
}

declare class A4 {
  constructor();
  str4: string;
  constructor(x: number);
}
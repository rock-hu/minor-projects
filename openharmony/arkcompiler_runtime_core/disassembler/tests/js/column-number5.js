/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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


class Student {
  constructor() {
    this.name = 'Tom';
    this.age = 1;
  }
}

let c = 9;

function foo1() {
  let a = 2;
  let b = 1;
  let m = a + b;
  let n = m + c;
  return n;
}

function foo2() {
  foo1();
}

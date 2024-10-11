/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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


// static blocks in class can access private fields in the class
let sum: number = 100;
function addResult() {
  return sum + 1;
}

let PF = class PrivateFoo {
  static num: number = 0;
  static get count(): number {
    return PrivateFoo.num;
  }
  get count(): number {
    return PrivateFoo.num;
  }
  static {
    try {
      PrivateFoo.num += addResult();
    } catch {
    }
  }
}

let pf: PrivateFoo = new PF();
print(pf.count);
print(pf.count);
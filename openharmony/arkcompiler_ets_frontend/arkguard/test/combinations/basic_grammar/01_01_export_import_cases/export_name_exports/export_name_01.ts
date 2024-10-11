/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

let var1 = 1;
let var2 = 2;
function foo1() {
  return 11;
}
function foo2(){
  return 22;
}
class Person1 {
  age:number = 1;
  get getAge(): number {
    return this.age;
  }
}
export {}
export {var1};
export {var2, foo1};
export {foo2, Person1,}


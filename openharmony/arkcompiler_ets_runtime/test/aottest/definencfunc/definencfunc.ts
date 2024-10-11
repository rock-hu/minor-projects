/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

declare function print(str:any):string;

var arrow_func = ():string => "arrow function";

var arrow_sum = (arg1:number, arg2:number):number => {
    return arg1 + arg2;
}

print(arrow_func());
print(arrow_sum(2, 3));

class C3 {}
const v20 = new C3();
const v21 = new C3();
function f26() {
  const o39 = {
    "a": 1,
    "d": 1000000000000,
    __proto__: v21,
    toLowerCase(a32, a33, a34) {
    },
    "d": 2,
    536870889: 1,
  };
  return o39;
}
f26();
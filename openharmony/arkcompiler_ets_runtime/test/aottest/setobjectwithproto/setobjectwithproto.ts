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
var proto = {};
var proto2 = {};
var obj ={
    __proto__: proto
}
print(obj.__proto__ == proto)
print(obj.__proto__ == proto2);

class C12 extends Set {
}
const v13 = new C12();
const o59 = {
    __proto__: v13,
};
try {
    o59.values();
} catch (error) {
    print(o59.__proto__ == v13);
}

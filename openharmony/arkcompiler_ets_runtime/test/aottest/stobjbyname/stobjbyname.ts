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

interface objInterface {
    [key: string]: any
}
let obj:objInterface = {};
obj.a = 100;
obj.b = "helloworld";
print(obj.a)
print(obj.b);

class A{};
function foo(x: any) {
    try {
        x.constructor = 0;
        print(x.constructor);
    } catch (e) {
        print("foo");
    }
}


let a = new A();
foo(a);
if (ArkTools.isAOTCompiled(foo)) {
    let a = new A();
    foo(a);
    let b = 1;
    foo(b);
}
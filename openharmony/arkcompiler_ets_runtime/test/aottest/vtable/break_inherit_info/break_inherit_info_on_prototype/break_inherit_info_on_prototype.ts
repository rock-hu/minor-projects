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

declare function print(a0:any, a1?:any):string;

class A {
    x:number;
    y:number;

    foo() {
        print("A foo");
    }
}

class B extends A {
    z:string;
}

function testVTable(o:A) {
    o.foo();
}

let b = new B();
testVTable(b);

// break TS Chain on prototype and notify through chain
Object.defineProperty(B.prototype, "foo", { value: function() {
        print("B foo");
    }
})

testVTable(b);  // occur deopt

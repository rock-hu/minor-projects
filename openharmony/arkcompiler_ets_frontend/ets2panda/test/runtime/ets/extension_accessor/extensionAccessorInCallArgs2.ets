/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

class A {
    static xstate: int = 0;
}

get x(this: A): int { return A.xstate }
set x(this: A, v: int) { A.xstate = v }

class B {
    get y(): A {
        return new A();
    }
}

function main():void {
    let b = new B();
    b.y.x = 123;
    assertEQ(A.xstate, 123);
    assertEQ(b.y.x, 123);
    assertTrue(b.y.x == 123);
}

/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

interface A {
    foo(): A
}

interface B extends A {
    foo(): B
}

class C implements A {
    private value: string = "";

    foo(): A {
        this.value = "foo(): A"
        return new C();
     }

    getValue(): string {
        return this.value;
    }
}

class D implements B {
    private value: string = "";

    foo(): B {
        this.value = "foo(): B"
        return new D();
    }

    getValue(): string {
        return this.value;
    }
}

function main() {
    let c = new C();
    c.foo();
    assertEQ(c.getValue(), "foo(): A")

    let d = new D();
    d.foo();
    assertEQ(d.getValue(), "foo(): B")
}

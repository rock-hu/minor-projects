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

/*---
desc: Testing access to constants in multi-level inheritance.
---*/

interface G {
    readonly CONSTANT: string;
}

class A {
    static readonly CONSTANT: string = "Constant from A";
}

class B extends A {
    static readonly CONSTANT: string = "Constant from B";
}

class C extends B implements G {
    readonly CONSTANT: string = "Constant from interface C";
}

function main(): void {
    assertEQ(C.CONSTANT, "Constant from B");

    const c: C = new C();
    assertEQ(c.CONSTANT, "Constant from interface C");
}
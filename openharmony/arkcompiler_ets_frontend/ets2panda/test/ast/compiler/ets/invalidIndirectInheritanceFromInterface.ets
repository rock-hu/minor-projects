/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
    x(): boolean {
        return true;
    }
}

class C implements A {
}

class B extends /* @@ label */C {
    public x: boolean;
    public getX(): boolean {
       return /* @@ label1 */this.x();
    }
}

function main(): void {
    let instance: B = new B();
    assertEQ(instance.getX(), true);
}

/* @@@ label1 Error TypeError: Type 'boolean' has no call signatures.  */
/* @@@ label Error TypeError: Cannot inherit from class C, because field x is inherited with a different declaration type  */

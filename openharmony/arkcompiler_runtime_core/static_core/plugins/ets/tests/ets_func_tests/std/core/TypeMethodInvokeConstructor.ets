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

class C {
    constructor(a: int) {
        this.a = a
    }

    a: int
}

function main(): void {
    const c = new C(1)
    const cType = Type.of(c) as ClassType
    assertEQ( cType.getConstructorsNum(), 1)
    const m = cType.getConstructor(0)
    const args : FixedArray<NullishType> = new NullishType[1];
    args[0] = 10
    let res = m.invoke(null, args) as C
    assertEQ( res.a, 10)

    args[0] = 30
    res = m.invoke(null, args) as C
    assertEQ( res.a, 30)
}

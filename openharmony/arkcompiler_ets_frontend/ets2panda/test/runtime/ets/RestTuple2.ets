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
    sum(...numbers: [number, number, number]): number {
        return numbers[0] + numbers[1] + numbers[2]
    }

    sum2(a: int, ...numbers: [number, number, number]): number {
        return a + numbers[0] + numbers[1] + numbers[2]
    }
}

function main() {
    let tuple : [number, number, number] = [30, 40, 50]
    let a = new A()

    let res = a.sum(10, 20, 30)
    let res2 = a.sum2(10, 20, 30, 40)

    let res3 = a.sum(...tuple)
    let res4 = a.sum2(10, ...tuple)

    let res5 = a.sum(...([30, 40, 50] as [number, number, number]))
    let res6 = a.sum2(10, ...([30, 40, 50] as [number, number, number]))

    assertEQ(res, 60)
    assertEQ(res2, 100)
    assertEQ(res3, 120)
    assertEQ(res4, 130)
    assertEQ(res5, 120)
    assertEQ(res6, 130)
}

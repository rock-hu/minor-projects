/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
    foo () : int[] {
        return [7]
    }
}

function foo() : int[]{
    return [1, 2] 
}

function getStringArr() {
    return ["A"]
}

function main() {
    let a = new A()
    let arr1:Int[] = [...a.foo(), ...foo()]
    assertEQ(arr1.length, 3)
    assertEQ(arr1[0], 7)
    assertEQ(arr1[1], 1)
    assertEQ(arr1[2], 2)

    let arr2 = [...getStringArr()]
    assertEQ(arr2.length, 1)
    assertEQ(arr2[0], "A")

    let getArr = (): string[] => {
        return ["a", "b"]
    }
    let arr3 : string[] = [...arr2, ...getArr(), "B"]
    assertEQ(arr3.length, 4)
    assertEQ(arr3[0], "A")
    assertEQ(arr3[1], "a")
    assertEQ(arr3[2], "b")
    assertEQ(arr3[3], "B")

}


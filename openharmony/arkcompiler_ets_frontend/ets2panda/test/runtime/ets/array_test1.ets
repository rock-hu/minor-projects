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

function main() {
    let a : Array<int> = [1,2]
    a[0] = 1;
    a[1] = 2;
    assertEQ(a.length, 2);
    assertEQ(a.pop(), 2);
    assertEQ(a.length, 1)

    let c : Array<Array<Array<int>>> = [[[1,2,3],[4,5,6]], [[7,8],[9,10]]]
    assertEQ(c[1][1][1], 10)
    assertEQ(c[0][0][0], 1)
    c[1][1][1] = 100
    c[0][0][0] = 5
    assertEQ(c[1][1][1], 100)
    assertEQ(c[0][0][0], 5)
    assertEQ(c[0][1][0], 4)
    assertEQ(c[0][1][1], 5)
    assertEQ(c[0][1][2], 6)
    assertEQ(c[1][1][0], 9)
}
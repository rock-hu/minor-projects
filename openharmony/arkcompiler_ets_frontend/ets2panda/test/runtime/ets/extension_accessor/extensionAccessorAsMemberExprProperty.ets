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
    idx_1: Int = 0;
    idx_2: int = 1;
}

get idx1(this: A): Int { return this.idx_1 }
get idx2(this: A): int { return this.idx_2 }

function main(): void {
    let a = new A();
    let arr = ["getIdx1", "getIdx2"];
    assertEQ(arr[a.idx1], "getIdx1")
    assertEQ(arr[a.idx2], "getIdx2")
    assertEQ(arr[new A().idx1], "getIdx1")
    assertEQ(arr[new A().idx2], "getIdx2")
}

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

interface itf {
    x1: (int|string)
    x2: (String|Boolean)
    x3: (null|int)
}

class A implements itf {
    x1 = 1
    x2 = true
    x3 = null
}


function main() {
    let a: A = new A()
    assertEQ(a.x1, 1)
    assertEQ(a.x2, true)
    assertEQ(a.x3, null)
}

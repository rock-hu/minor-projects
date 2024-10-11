/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

let id = 0;
function generateId() {
    return "patch" + ++id
}

class A {
    constructor(a) {
        this.a = a
    }

    onCreate() {
        print(this.a)
        print("patch onCreate")
    }

    render() {
        var sum = (a) => {
            return a + " patch "
        }
        print(sum(1) + "render")
    }
}
globalThis.A = A
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
    name: string
    constructor (n:string) {
        this.name = n;
    } 
}
function apply(aa: A[], ff: (this: A) => this) {
    for (let a of aa) {
        assertEQ(a.ff().name, a.name)
        assertEQ(a.ff().ff().name, a.name)
        assertEQ(ff(a).name, a.name)
        assertEQ(ff(a).ff().name, a.name)
    }
}
let aa: A[] = [new A("aa"), new A("bb")]
apply(aa, (this: A): this => { return this } )

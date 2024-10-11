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

export class D {
    constructor() {
        this.name = "module patch name"
    }
    PrintD() {
        print("module patch D : " + this.name);
    }
}

export class C {
    constructor() {
    }
    PrintC() {
        print("module patch C");
    }
}

export class B {
    constructor() {
        this.name = "module patch name"
        this.age = "module patch age"
    }
    PrintB() {
        print("module patch B : " + this.name);
    }
    PrintP() {
        print ("module patch P : " + this.age);
    }
}

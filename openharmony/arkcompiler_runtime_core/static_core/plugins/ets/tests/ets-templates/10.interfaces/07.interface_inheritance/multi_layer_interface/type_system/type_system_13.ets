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

/*---
desc: Inherit a generic class in a class and implement an interface to verify the correctness of the type system.
tags: [compile-only, negative]
---*/

class BBox<T> {
    value: T;
    constructor(value: T) {
        this.value = value;
    }
}

interface Printable {
    print(): string;
}

class StringBBox extends BBox<number> implements Printable {
    print(): number {
        return 123;
    }
}
/**
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

class GoodGreeter {
    name: string;

    public b: int;

    fooExample() : void {}

    static $_invoke(): int {
        return 5;
    }

    constructor() {
        this.name = "hello";
        this.b = 7;
    }
}

let a = GoodGreeter();
let c: GoodGreeter = new GoodGreeter();


function main() {
    let m: int = 1;
    let n: int = 2;
    let f: int = c.b;
    let k: string = c.name;
    console.log(k);
}

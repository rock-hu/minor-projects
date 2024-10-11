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

declare function print(str:any):string;

var b:Boolean = new Boolean(2);
print(b);

var n:Number = new Number(123);
print(n);

var o:Object = new Object("newobj");
print(o);

var a:Array<number> = new Array(10);
print(a.length);

class TClass {
    dd: number;

    div(v1: number[]): number {
        return 1.0 / v1[0];
    }
    constructor(p1: number[]) {
        let d = this.div(p1);
        this.dd = d / 1.0;
    }
}

let p1: number[] = [-10, 10, -10];
new TClass(p1);

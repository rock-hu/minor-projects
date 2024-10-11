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
function f(a:any,...A:any) {
    for (let p in A) {
        print(A[p]);
    }
}

f(1, 2, 3);
f(1, "success", "fail");

// The following test cases have exposed a bug: the variable actualRestNum in RuntimeOptCopyRestArgs
// computed mistakely and may out of uint32_t range.
function foo(x: number, y?: number, ...restArgs: number[]):void {
    let arr = [...restArgs];
    print(arr.length);
}

foo(1);

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

interface I {
    s: string;
}

let res = 0;
function foo(f1: number, ...args: (Object | null | undefined)[]) {
    res = 1;
}

function foo(f1: number, i: I, ...args: (Object | null | undefined)[]) {
    res = 2;
}

function main(): void {
    const i: I = {s: "sss"};
    foo(1, "sss");
    assertEQ(res, 1);
    foo(1, i as I, "sss");
    assertEQ(res, 2);
}
/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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


function func(a: { a: number, b: string }, b: number): { a: number[], b: boolean } {
    return { a: [1, 2, 3], b: true };
}

var /* @@ label */a: { a: string[], b: boolean } = func({ a: 5, b: "foo" }, 6);
/* @@@ label Error TypeError: Type '{ a: number[]; b: boolean; }' is not assignable to type '{ a: string[]; b: boolean; }'.  */

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


interface a {
    (a: number, b: string): number,
    (a: number, b: string): number,
}

interface b {
    (a: number, b: string): number,
}

var c: a;
var /* @@ label */c: b;
/* @@@ label Error TypeError: Subsequent variable declaration must have the same type. Variable 'c' must be of type 'a', but here has type 'b'.  */

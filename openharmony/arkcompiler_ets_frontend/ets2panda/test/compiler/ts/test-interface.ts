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


interface foo {
    a?: number,
    b: string | boolean;
    c(a: (a: number, b: void) => string, b?: number[]): undefined;
    (a: string): string,
    (a: number): number,
    readonly (a: number): number,
    [a:number]:number,
    new(a: null, b?: string): { a: number, b: string, c?([a, b]): string }
}

interface bar {
    readonly a?: number,
    readonly b: string | boolean;
    readonly [a:number]:number
}
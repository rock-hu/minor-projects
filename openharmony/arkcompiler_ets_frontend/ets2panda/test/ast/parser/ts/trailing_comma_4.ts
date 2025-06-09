/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

function foo(...number: int[]): int {
    return number[0]
}

let a = [1,, 2,] // OK
foo(...a,)
foo(a[0],)
foo(a[0], a[1], ...a,)
foo(a[0],,a[0])
foo(a[0],,)
foo(,a[0])
foo(a[0] a[1])
foo(,)

/* @@? 24:10 Error SyntaxError: Unexpected token ','. */
/* @@? 25:10 Error SyntaxError: Unexpected token ','. */
/* @@? 26:5 Error SyntaxError: Unexpected token ','. */
/* @@? 27:10 Error SyntaxError: Unexpected token, expected ',' or ')'. */
/* @@? 27:10 Error SyntaxError: Unexpected token 'a'. */
/* @@? 27:14 Error SyntaxError: Unexpected token ')'. */
/* @@? 28:5 Error SyntaxError: Unexpected token ','. */

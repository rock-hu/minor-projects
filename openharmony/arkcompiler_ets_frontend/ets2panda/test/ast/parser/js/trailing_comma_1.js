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

function foo(...number/* @@ label */,/* @@ label1 */) {
    return number[0]
}

function foo2(number,) {
    return number
}

function foo3(/* @@ label2 */, number) {
    return number
}

let a = [1, , 2,,] // OK - omitted expressions
foo(...a,)
foo(a[0],)
foo(a[0], a[1], ...a,)
foo(a[0],/* @@ label3 */,a[0])
foo(a[0],/* @@ label4 */,)
foo(/* @@ label5 */,a[0])
foo(a[0] /* @@ label6 */a[1]/* @@ label7 */)
foo(/* @@ label8 */,)


/* @@@ label Error SyntaxError: Rest parameter must be the last formal parameter. */
/* @@@ label1 Error SyntaxError: Unexpected token, expected '{'. */
/* @@@ label2 Error SyntaxError: Unexpected token. */
/* @@@ label3 Error SyntaxError: Unexpected token ','. */
/* @@@ label4 Error SyntaxError: Unexpected token ','. */
/* @@@ label5 Error SyntaxError: Unexpected token ','. */
/* @@@ label6 Error SyntaxError: Unexpected token, expected ',' or ')'. */
/* @@@ label6 Error SyntaxError: Unexpected token 'a'. */
/* @@@ label7 Error SyntaxError: Unexpected token ')'. */
/* @@@ label8 Error SyntaxError: Unexpected token ','. */
/* @@@ label9 Error SyntaxError: Expected '}', got 'end of stream'. */
/* @@ label9 */
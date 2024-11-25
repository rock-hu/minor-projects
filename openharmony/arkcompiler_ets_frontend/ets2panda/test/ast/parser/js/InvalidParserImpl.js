/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

class 12 {}

class A {
    #x;
    #x;

    async get abc() {}

    static static s;

    #get abc() {
        return s;
    }

    #constructor(x) {
        this.s = x;
    }

    static constructor() {}
    static prototype() {}

    constructor;
    static prototype;

    constructor() {}
    constructor() {}

    async constructor() {}
}

function f(eval, arguments) {}

for (var i of a) {
    continue label;
}

/* @@? 16:7 Error SyntaxError: Unexpected token, expected an identifier. */
/* @@? 16:10 Error SyntaxError: Unexpected token '{'. */
/* @@? 22:5 Error SyntaxError: Private field has already been declared */
/* @@? 22:15 Error SyntaxError: Invalid accessor */
/* @@? 24:12 Error SyntaxError: Unexpected modifier */
/* @@? 24:12 Error SyntaxError: Duplicated modifier is not allowed */
/* @@? 26:6 Error SyntaxError: Unexpected identifier */
/* @@? 30:6 Error SyntaxError: Private identifier can not be constructor */
/* @@? 35:12 Error SyntaxError: Classes may not have static property named prototype */
/* @@? 37:5 Error SyntaxError: Classes may not have a field named 'constructor' */
/* @@? 38:12 Error SyntaxError: Classes may not have static property named prototype */
/* @@? 40:5 Error SyntaxError: Multiple constructor implementations are not allowed. */
/* @@? 41:5 Error SyntaxError: Multiple constructor implementations are not allowed. */
/* @@? 43:11 Error SyntaxError: Constructor can not be special method */
/* @@? 43:5 Error SyntaxError: Multiple constructor implementations are not allowed. */
/* @@? 46:12 Error SyntaxError: 'eval' or 'arguments' can't be defined or assigned to in strict mode code */
/* @@? 46:18 Error SyntaxError: 'eval' or 'arguments' can't be defined or assigned to in strict mode code */
/* @@? 49:14 Error SyntaxError: Undefined label */

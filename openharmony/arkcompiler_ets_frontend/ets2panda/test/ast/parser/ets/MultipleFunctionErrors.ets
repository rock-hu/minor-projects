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

function /* @@ label */(): void {
}

function /* @@ label1 */(): /* @@ label2 */{
    /* @@ label3 */return /* @@ label4 */77;
}

/* @@@ label Error SyntaxError: Unexpected token, expected an identifier. */
/* @@@ label1 Error SyntaxError: Unexpected token, expected an identifier. */
/* @@@ label1 Error TypeError: Only abstract or native methods can't have body. */
/* @@@ label2 Error SyntaxError: Using object literals to declare types in place is not supported. Please declare types and interfaces explicitly! */
/* @@@ label3 Error SyntaxError: Unexpected token 'return'. */
/* @@@ label4 Error SyntaxError: Unexpected token '77'. */

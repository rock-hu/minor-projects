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

do export  Boolean [1] = delete
   while(true)

/* @@? 16:4 Error SyntaxError: Unexpected token 'export'. */
/* @@? 16:4 Error SyntaxError: Missing body in do while statement */
/* @@? 16:4 Error SyntaxError: Unexpected token, expected 'while'. */
/* @@? 16:12 Error SyntaxError: Unexpected token, expected '('. */
/* @@? 16:20 Error SyntaxError: Invalid left-hand side in array destructuring pattern. */
/* @@? 16:20 Error TypeError: Invalid left-hand side of assignment expression */
/* @@? 17:4 Error SyntaxError: Expected ')', got 'while'. */
/* @@? 27:1 Error SyntaxError: Unexpected token 'end of stream'. */

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

let a = (... ( ()   => {
 
})) = : async () => {
 
} []

/* @@? 16:9 Error TypeError: Invalid left-hand side of assignment expression */
/* @@? 16:9 Error TypeError: No matching call signature for *ERROR_LITERAL*(() => void) */
/* @@? 16:9 Error TypeError: Invalid left-hand side of assignment expression */
/* @@? 16:9 Error TypeError: No matching call signature for *ERROR_LITERAL*(() => void) */
/* @@? 16:10 Error SyntaxError: Unexpected token '...'. */
/* @@? 18:5 Error SyntaxError: Invalid left-hand side in assignment expression. */
/* @@? 18:7 Error SyntaxError: Unexpected token ':'. */
/* @@? 18:9 Error SyntaxError: Unexpected token 'async'. */
/* @@? 18:15 Error SyntaxError: 'async' flags must be used for functions only at top-level. */
/* @@? 18:16 Error SyntaxError: Unexpected token, expected an identifier. */
/* @@? 18:16 Error SyntaxError: Unexpected token, expected '('. */
/* @@? 18:16 Error TypeError: Only abstract or native methods can't have body. */
/* @@? 18:18 Error SyntaxError: Unexpected token '=>'. */
/* @@? 18:18 Error SyntaxError: Unexpected token, expected ',' or ')'. */
/* @@? 18:18 Error SyntaxError: Parameter declaration should have an explicit type annotation. */
/* @@? 18:18 Error SyntaxError: Unexpected token, expected an identifier. */
/* @@? 18:21 Error SyntaxError: Unexpected token '{'. */
/* @@? 20:3 Error TypeError: Can't resolve array type */

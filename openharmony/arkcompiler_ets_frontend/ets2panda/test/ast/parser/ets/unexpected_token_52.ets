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

let a = {
    get [b)(){},
    ...a,
}

/* @@? 16:5 Error TypeError: Cannot infer type for a because class composite needs an explicit target type */
/* @@? 17:9 Error SyntaxError: Unexpected token. */
/* @@? 17:10 Error SyntaxError: Unexpected token, expected '('. */
/* @@? 17:10 Error SyntaxError: Object pattern can't contain methods. */
/* @@? 17:12 Error SyntaxError: Expected '{', got '('. */
/* @@? 17:12 Error SyntaxError: Unexpected token. */
/* @@? 17:13 Error SyntaxError: Unexpected token ')'. */
/* @@? 17:14 Error SyntaxError: Unexpected token '{'. */
/* @@? 17:16 Error SyntaxError: Unexpected token ','. */
/* @@? 18:5 Error SyntaxError: Unexpected token '...'. */
/* @@? 18:8 Error SyntaxError: Unexpected token 'a'. */
/* @@? 18:9 Error SyntaxError: Unexpected token ','. */
/* @@? 19:1 Error SyntaxError: Unexpected token '}'. */

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

interface A { foo(): void}
const x: A = {foo() {}}

/* @@? 17:15 Error TypeError: Method 'foo' cannot be used as a key of object literal. */
/* @@? 17:18 Error SyntaxError: Unexpected token, expected ':'. */
/* @@? 17:19 Error SyntaxError: Unexpected token ')'. */
/* @@? 17:21 Error SyntaxError: Unexpected token. */
/* @@? 17:22 Error SyntaxError: Unexpected token '}'. */
/* @@? 17:23 Error SyntaxError: Unexpected token '}'. */

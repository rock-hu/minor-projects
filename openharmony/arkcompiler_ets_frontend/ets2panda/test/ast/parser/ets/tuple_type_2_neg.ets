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


let a: [number, ...number[], number] = [1, 2, 3];

/* @@? 17:17 Error SyntaxError: Invalid Type. */
/* @@? 17:17 Error SyntaxError: Unexpected token, expected ',' or ']'. */
/* @@? 17:17 Error SyntaxError: Unexpected token '...'. */
/* @@? 17:20 Error SyntaxError: Unexpected token 'number'. */
/* @@? 17:28 Error SyntaxError: Unexpected token ','. */
/* @@? 17:30 Error SyntaxError: Unexpected token 'number'. */
/* @@? 17:30 Error TypeError: Type name 'number' used in the wrong context */
/* @@? 17:36 Error SyntaxError: Unexpected token ']'. */

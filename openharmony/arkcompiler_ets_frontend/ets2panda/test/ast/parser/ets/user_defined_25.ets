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

interface /* @@ label1 */type {
  type: number /* @@ label2 */= /* @@ label3 */0
/* @@ label4 */}

/* @@@ label1 Error SyntaxError: Cannot be used as user-defined type. */
/* @@@ label2 Error SyntaxError: Interface member initialization is prohibited. */
/* @@@ label3 Error SyntaxError: Unexpected token, expected ','. */
/* @@? 17:48 Error SyntaxError: Unexpected token, expected 'private' or identifier. */
/* @@@ label4 Error SyntaxError: Identifier expected. */

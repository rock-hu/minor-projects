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

class A{
    private \\\\foo(){}
}

/* @@? 17:13 Error SyntaxError: Invalid character. */
/* @@? 17:13 Error SyntaxError: Access modifier must precede field and method modifiers. */
/* @@? 17:14 Error SyntaxError: Invalid character. */
/* @@? 17:14 Error SyntaxError: Keyword must not contain escaped characters. */
/* @@? 17:14 Error SyntaxError: Access modifier must precede field and method modifiers. */
/* @@? 17:15 Error SyntaxError: Invalid character. */
/* @@? 17:15 Error SyntaxError: Keyword must not contain escaped characters. */
/* @@? 17:15 Error SyntaxError: Access modifier must precede field and method modifiers. */
/* @@? 17:16 Error SyntaxError: Invalid character. */
/* @@? 17:16 Error SyntaxError: Keyword must not contain escaped characters. */
/* @@? 17:16 Error SyntaxError: Access modifier must precede field and method modifiers. */

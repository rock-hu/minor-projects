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

(...a*b+c)/(d)-e;

/* @@? 16:6 Error SyntaxError: Rest parameter must be the last formal parameter. */
/* @@? 16:6 Error SyntaxError: Unexpected token, expected: '=>'. */
/* @@? 16:10 Error SyntaxError: Unexpected token ')'. */
/* @@? 16:10 Error SyntaxError: Unexpected token: ')'. */
/* @@? 16:18 Error SyntaxError: Unterminated RegExp */

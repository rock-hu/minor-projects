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

class A {
    a: readonly/* @@ A_a */;
    b: readonly /* @@ A_b */false;
    c: readonly /* @@ A_c */1;
}    
    
/* @@@ A_a Error SyntaxError: Invalid Type. */
/* @@@ A_a Error SyntaxError: 'readonly' type modifier is only permitted on resizable array and tuple types. */

/* @@@ A_b Error SyntaxError: Invalid Type. */
/* @@@ A_b Error SyntaxError: 'readonly' type modifier is only permitted on resizable array and tuple types. */
/* @@@ A_b Error SyntaxError: Unexpected token 'false'. */

/* @@@ A_c Error SyntaxError: Invalid Type. */
/* @@@ A_c Error SyntaxError: 'readonly' type modifier is only permitted on resizable array and tuple types. */
/* @@@ A_c Error SyntaxError: Unexpected token '1'. */

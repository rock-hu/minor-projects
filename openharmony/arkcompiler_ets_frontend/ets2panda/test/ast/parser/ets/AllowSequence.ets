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

function main(): void {
    for (let i = 0, j = 0; i < 10; ++i, j += 2) {
        let x = i + j;
        x = (++x, x++);
        console.println(x);
    }
}

/* @@? 19:17 Error SyntaxError: Unexpected token, expected ')'. */
/* @@? 19:19 Error SyntaxError: Unexpected token 'x'. */
/* @@? 19:22 Error SyntaxError: Unexpected token ')'. */

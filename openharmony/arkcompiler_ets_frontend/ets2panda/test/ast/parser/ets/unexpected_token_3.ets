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

function labeledTest01(): int {
    let value: int = 0;
    let result: int = 0;

    label1:
    for (let i = 0; i < 10; i++) {
        switch (value) {
            case 0:
            case 1:
                result += 2;
                ++i;
                continue /* @@ label */^/* @@ label1 */^label1;
            case 2:
                result = 1;
                break;
            default:
                result = 3400;
        }
    }

    return result;
}

/* @@@ label Error SyntaxError: Unexpected token, expected an identifier. */
/* @@@ label1 Error SyntaxError: Unexpected token '^'. */
/* @@? 27:57 Error SyntaxError: Unexpected token 'label1'. */
/* @@? 27:57 Error TypeError: Identifier 'label1' is used in wrong context. */

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

function main(): int {
    let sum: int
    let arr: int[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    for (let x of arr) {
        sum += x
    }
    if (sum > 0) {/* @@ label */continue;/* @@ label1 */continue;/* @@ label2 */continue;}; // misplaced continue statement, out of loop
    sum+arr[0];
    /* @@ label3 */continue
    return 0;
}
/* @@@ label Error TypeError: Control flow redirection statement can not be used out of loop or switch statement.  */
/* @@@ label1 Error TypeError: Control flow redirection statement can not be used out of loop or switch statement.  */
/* @@@ label2 Error TypeError: Control flow redirection statement can not be used out of loop or switch statement.  */
/* @@@ label3 Error TypeError: Control flow redirection statement can not be used out of loop or switch statement.  */
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

function foo(callback: (x: int, y: String) => boolean): void {
    let x: boolean = callback(5, "Hello");
}

/* @@ label */function foo(callback: (x: double, y: String) => boolean): void {
    let x: boolean = callback(5, "Hello");
}

function main(): void {
    foo((x: int, y: String): boolean => {
        return y.length == x;
    });
    foo((x: double, y: String): boolean => {
        return y.length == x;
    });
    // Should report an error
    /* @@ label1 */foo((x, y) => {
        console.println("hello");
    });
}

/* @@@ label Error TypeError: Function foo with this assembly signature already declared.  */
/* @@@ label1 Error TypeError: Reference to foo is ambiguous  */

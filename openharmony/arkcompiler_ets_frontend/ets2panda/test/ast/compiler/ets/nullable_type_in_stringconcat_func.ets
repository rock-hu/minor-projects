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
 
 function foo(a: String|null, b: Int|null): String {
    return /* @@ label */a + b;
}

function foo2(a: String|null, b: String|null): String {
    return /* @@ label1 */a + b;
}

function main() {
    let result = foo(null, null);
    let result2 = foo2(null, null);
}

/* @@@ label Error TypeError: Bad operand type: multiple types left in the normalized union type (String|null). Unions are not allowed in binary expressions except equality.  */
/* @@@ label1 Error TypeError: Bad operand type: multiple types left in the normalized union type (String|null). Unions are not allowed in binary expressions except equality.  */

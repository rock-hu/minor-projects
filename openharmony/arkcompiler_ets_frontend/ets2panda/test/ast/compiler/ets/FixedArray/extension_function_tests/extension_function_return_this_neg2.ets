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

class A {}
class B {}

function f1(this : B): FixedArray<this> {
    return this;
}

/* @@? 19:35 Error SyntaxError: A 'this' type is available only as return type in a non-static method of a class or struct and extension functions. */
/* @@? 19:35 Error TypeError: A 'this' cannot be used as type of array. */
/* @@? 20:12 Error TypeError: Type 'B' is not compatible with the enclosing method's return type 'FixedArray<ETSGLOBAL>' */

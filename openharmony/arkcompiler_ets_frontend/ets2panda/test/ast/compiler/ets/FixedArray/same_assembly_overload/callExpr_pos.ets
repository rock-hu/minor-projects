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

class A<T> {}
declare function foo(a:A<number>):void
declare function foo(a:A<string>):number
declare function foo(a:int, b:int):void
declare function foo(a:double):void
declare function foo(...args:FixedArray<string>):void
let b :A<number>
foo(b)
foo(1,2)
foo(1.1)


/* @@? 17:1 Warning Warning: Function foo with this assembly signature already declared. */
/* @@? 23:5 Error TypeError: Variable 'b' is used before being assigned. */

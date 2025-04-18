/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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


function /* @@ label */foo(a: number, b: string) {
    var a: number = foo(1, "bar");
    return foo(2, "baz");
    return 12;
}

var b: any;
var b = foo(2, "bar");

/* @@@ label Error TypeError: foo implicitly has return type 'any' because it does not have a return type annotation and is referenced directly or indirectly in one of its return expressions.  */

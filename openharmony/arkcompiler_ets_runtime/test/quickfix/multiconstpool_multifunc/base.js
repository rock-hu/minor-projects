/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
function foo() {
    REPLACE_FUNC_FOO1
    print("base foo")
}
REPLACE_FUNC_FOO2
function foo35000() {
    var a35000 = "35000";
    var a35001 = "35001";
    var a35002 = "35002";
    var a35003 = "35003";
    print("base foo35000")
}

globalThis.foo = foo
globalThis.foo1 = foo1

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/*
 * @tc.name:globalrecord
 * @tc.desc:test global record
 * @tc.type: FUNC
 * @tc.require: issueI5NO8G
 */
"use strict"

class View{}
let a = "a";
const b = "b";

print(View.name);

class myString extends String{}
var view = new myString("extends String");
print(view);

print(a);
a = "aa";
print(a);

print(b);
try {
    b = "bb";
} catch (error) {
    print("const can not be modified");
}
print(b);

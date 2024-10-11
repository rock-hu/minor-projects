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
 * @tc.name:newobjdynrange
 * @tc.desc:test newobjdynrange
 * @tc.type: FUNC
 * @tc.require: issueI5NO8G
 */
function foo(arg1, arg2) {
    this.arg1 = arg1
    this.arg2 = arg2
}
var p = new foo("arg1", "arg2")
print(p.arg1 === "arg1" && p.arg2 === "arg2")

const bind_func = Array.prototype.shift.bind();
try {
    new bind_func(1, ...[1, 2, 3]);
} catch (e) {
    print(e.name);
}
function func() {
    func.prototype = "hello"
}
for (let i = 0; i < 2; i++) {
    let obj = new func();
    print(Object.getPrototypeOf(obj))
}

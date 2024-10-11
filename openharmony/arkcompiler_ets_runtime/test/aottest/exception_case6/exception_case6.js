/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

try {
    function foo(a, b, c) {
        d.e;
    }
    var bar = foo.bind(undefined, [2, 3])
    bar(1);
} catch (e) {
    print(e)
    let stack = e.stack
    let array = stack.split('\n')
    for (let line of array) {
        let start = line.lastIndexOf('/') + 1
        let end = line.length - 1
        if (start < end) {
            print(line.slice(start, end))
        } else {
            print(line)
        }
    }
}
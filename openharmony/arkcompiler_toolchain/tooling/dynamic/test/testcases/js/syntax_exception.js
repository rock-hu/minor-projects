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

function countSyntaxException() {
    var sum = 0;
    try {
        adddlert("hello! ");
    }
    catch(err) {
        print(err);
        sum += 1;
    } 
    return sum;
}

print("SyntaxException test begin")
try {
    countSyntaxException()
} catch (e) {
    print("SyntaxException test ok")
}
print("SyntaxException test end")
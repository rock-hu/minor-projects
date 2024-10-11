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

/*
 * @tc.name:addelementinternal
 * @tc.desc:test addelementinternal function
 * @tc.type: FUNC
 * @tc.require: issueI5NO8G
 */
let arr1 = {x:1,y:3};
let arr2 = [];
arr2.__proto__ = 123;
for (let i = 0; i < 5; i++) {
    arr1[i] = `value ${i}`;
    arr2[i] = i + 1;
}

for (let i = 0; i < 5; i++) {
    print(arr1[i]);
    print(arr2[i]);
}

let arr3 = {x: 1,y:3};
for (let i = 0; i < 5; i++) {
    arr3[i] = `value ${i}`;
    if (i ==3) delete arr3[i];
}
for (let i = 0; i < 5; i++) {
    print(arr3[i]);
}
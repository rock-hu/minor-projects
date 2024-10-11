/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

let robotA = [1, "mower", "mowing"];
let i = undefined;
let nameA = undefined;

for ([, nameA] = robotA, i = 0; i < 1; i++) {
    print(nameA);
}

for ([, nameA = "name"] = robotA, i = 0; i < 1; i++) {
    print(nameA);
}
